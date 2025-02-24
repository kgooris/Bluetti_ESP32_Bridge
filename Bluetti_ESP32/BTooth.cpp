#include "BluettiConfig.h"
#include "BTooth.h"
#include "utils.h"
#include "PayloadParser.h"
#include "BWifi.h"
#include "config.h"
#include "display.h"
#include "MQTT.h"


int pollTick = 0;
BLEScan* pBLEScan;
unsigned long prevTimerBTScan = 0;
int counter = 0;
int blenulptrCounter = 0;

struct command_handle {
  uint8_t page;
  uint8_t offset;
  int length;
};

QueueHandle_t commandHandleQueue;
QueueHandle_t sendQueue;

unsigned long lastBTMessage = 0;
unsigned long lastBTConnectToServer = 0;

class MyClientCallback : public BLEClientCallbacks {
  void onConnect(BLEClient* pclient) {
    Serial.println(F("BLE - onConnect"));
     #ifdef DISPLAYSSD1306
      disp_setBlueTooth(true);
     #endif
  }

  void onDisconnect(BLEClient* pclient) {
    connected = false;
    Serial.println(F("BLE - onDisconnect"));
    #ifdef DISPLAYSSD1306
      disp_setBlueTooth(false);
    #endif
    #ifdef RELAISMODE
      #ifdef DEBUG
        Serial.println(F("deactivate relais contact"));
      #endif
      digitalWrite(RELAIS_PIN, RELAIS_LOW);
    #endif
  }
};

/**
 * Scan for BLE servers and find the first one that advertises the service we are looking for.
 */
class BluettiAdvertisedDeviceCallbacks: public BLEAdvertisedDeviceCallbacks {
 /**
   * Called for each advertising BLE server.
   */
  void onResult(BLEAdvertisedDevice *advertisedDevice) {
    Serial.print(F("[BLE] Advertised Device found: "));
    Serial.println(advertisedDevice->toString().c_str());

     ESPBluettiSettings settings = get_esp32_bluetti_settings();
    // We have found a device, let us now see if it contains the service we are looking for.
    if (advertisedDevice->haveServiceUUID() && advertisedDevice->isAdvertisingService(serviceUUID) && (strcmp(advertisedDevice->getName().c_str(),settings.bluetti_device_id)==0) ) {
      BLEDevice::getScan()->stop();
      bluettiDevice = advertisedDevice;
      doConnect = true;
      doScan = true;
    }
  } 
};

void initBluetooth(){
  BLEDevice::init("");
  pBLEScan = BLEDevice::getScan();
  //pBLEScan->setAdvertisedDeviceCallbacks(new BluettiAdvertisedDeviceCallbacks());
  //pBLEScan->setInterval(1349);
  //pBLEScan->setWindow(449);
  //pBLEScan->setActiveScan(true);
  //pBLEScan->start(5, false);
  
  commandHandleQueue = xQueueCreate( 5, sizeof(bt_command_t ) );
  sendQueue = xQueueCreate( 5, sizeof(bt_command_t) );
}


static void notifyCallback(
  BLERemoteCharacteristic* pBLERemoteCharacteristic,
  uint8_t* pData,
  size_t length,
  bool isNotify) {

#ifdef DEBUG
    Serial.println("[BT] F01 - Write Response");
    /* pData Debug... */
    for (int i=1; i<=length; i++){
       
      Serial.printf("%02x", pData[i-1]);
      
      if(i % 2 == 0){
        Serial.print(" "); 
      } 

      if(i % 16 == 0){
        Serial.println();  
      }
    }
    Serial.println();
#endif

    bt_command_t command_handle;
    if(xQueueReceive(commandHandleQueue, &command_handle, 500)){
      parse_bluetooth_data(command_handle.page, command_handle.offset, pData, length);
    }
   
}

bool connectToServer() {
    Serial.print(F("[BT] Forming a connection to "));
    Serial.println(bluettiDevice->getAddress().toString().c_str());

    BLEDevice::setMTU(517); // set client to request maximum MTU from server (default is 23 otherwise)
    BLEClient*  pClient  = BLEDevice::createClient();
    Serial.println(F("[BT] - Created client"));

    pClient->setClientCallbacks(new MyClientCallback());

    // Connect to the remove BLE Server.
    pClient->connect(bluettiDevice);  // if you pass BLEAdvertisedDevice instead of address, it will be recognized type of peer device address (public or private)
    Serial.println(F("[BT] - Connected to server"));
    // pClient->setMTU(517); //set client to request maximum MTU from server (default is 23 otherwise)
  
    // Obtain a reference to the service we are after in the remote BLE server.
    BLERemoteService* pRemoteService = pClient->getService(serviceUUID);
    if (pRemoteService == nullptr) {
      Serial.print(F("[BT] Failed to find our service UUID: "));
      Serial.println(serviceUUID.toString().c_str());
      pClient->disconnect();
      pClient->deleteServices();
      // for fixing UUID service nullptr, check 5 times, if no luck reboot.
      blenulptrCounter++;
      if (blenulptrCounter > 5)
      {
        ESP.restart();
      }
      return false;
    }
    Serial.println(F("[BT] - Found our service"));


    // Obtain a reference to the characteristic in the service of the remote BLE server.
    pRemoteWriteCharacteristic = pRemoteService->getCharacteristic(WRITE_UUID);
    if (pRemoteWriteCharacteristic == nullptr) {
      Serial.print(F("[BT] Failed to find our characteristic UUID: "));
      Serial.println(WRITE_UUID.toString().c_str());
      pClient->disconnect();
      pClient->deleteServices();
      ESP.restart();
      return false;
    }
    Serial.println(F("[BT] - Found our Write characteristic"));

        // Obtain a reference to the characteristic in the service of the remote BLE server.
    pRemoteNotifyCharacteristic = pRemoteService->getCharacteristic(NOTIFY_UUID);
    if (pRemoteNotifyCharacteristic == nullptr) {
      Serial.print(F("[BT] Failed to find our characteristic UUID: "));
      Serial.println(NOTIFY_UUID.toString().c_str());
      pClient->disconnect();
      pClient->deleteServices();
      return false;
    }
    Serial.println(F("[BT] - Found our Write characteristic"));

    // Read the value of the characteristic.
    if(pRemoteWriteCharacteristic->canRead()) {
      std::string value = pRemoteWriteCharacteristic->readValue();
      Serial.print(F("[BT] The characteristic value was: "));
      Serial.println(value.c_str());
    }

    if(pRemoteNotifyCharacteristic->canNotify())
      pRemoteNotifyCharacteristic->registerForNotify(notifyCallback);

    connected = true;
     #ifdef RELAISMODE
      #ifdef DEBUG
        Serial.println(F("[BT] activate relais contact"));
      #endif
      digitalWrite(RELAIS_PIN, RELAIS_HIGH);
    #endif

    return true;
}


void handleBTCommandQueue(){

    bt_command_t command;
    if(xQueueReceive(sendQueue, &command, 0)) {
      
#ifdef DEBUG
    Serial.print("[BT] Write Request FF02 - Value: ");
    
    for(int i=0; i<8; i++){
       if ( i % 2 == 0){ Serial.print(" "); };
       Serial.printf("%02x", ((uint8_t*)&command)[i]);
    }
    
    Serial.println("");
#endif
      pRemoteWriteCharacteristic->writeValue((uint8_t*)&command, sizeof(command),true);
 
     };  
}

void sendBTCommand(bt_command_t command){
    bt_command_t cmd = command;
    xQueueSend(sendQueue, &cmd, 0);
}

void handleBluetooth(){
  if (doConnect == true)
  {
    if ((millis() - lastBTConnectToServer) > BLUETOOTH_QUERY_MESSAGE_DELAY)
    {
      lastBTConnectToServer = millis();
      if (connectToServer())
      {
          Serial.println(F("[BT] We are now connected to the Bluetti BLE Server."));
      }
      else
      {
          Serial.println(F("[BT] We have failed to connect to the server; there is nothing more we will do."));
          doConnect = false;
      }
    }
  }

  if ((millis() - lastBTMessage) > (MAX_DISCONNECTED_TIME_UNTIL_REBOOT * 60000)){ 
    Serial.println(F("[BT] disconnected over allowed limit, reboot device"));
    #ifdef SLEEP_TIME_ON_BT_NOT_AVAIL
        esp_deep_sleep_start();
    #else
        ESP.restart();
    #endif
  }

  if (connected) {

    // poll for device state
    if ( millis() - lastBTMessage > BLUETOOTH_QUERY_MESSAGE_DELAY){

       bt_command_t command;
       command.prefix = 0x01;
       command.field_update_cmd = 0x03;
       command.page = bluetti_polling_command[pollTick].f_page;
       command.offset = bluetti_polling_command[pollTick].f_offset;
       command.len = (uint16_t) bluetti_polling_command[pollTick].f_size << 8;
       command.check_sum = modbus_crc((uint8_t*)&command,6);

       xQueueSend(commandHandleQueue, &command, portMAX_DELAY);
       xQueueSend(sendQueue, &command, portMAX_DELAY);

       if (pollTick == sizeof(bluetti_polling_command)/sizeof(device_field_data_t)-1 ){
           pollTick = 0;
       } else {
           pollTick++;
       }
            
      lastBTMessage = millis();
    }

    handleBTCommandQueue();
    
  }else {
    // check every 10 seconds only.
    if (millis() - prevTimerBTScan >= 10000)
    {
      prevTimerBTScan = millis();
      counter++;
      if (pBLEScan->isScanning() == false)
      {
        BLEDevice::deinit(true);
        BLEDevice::init("");
        pBLEScan = BLEDevice::getScan();
        pBLEScan->setAdvertisedDeviceCallbacks(new BluettiAdvertisedDeviceCallbacks());
        pBLEScan->setActiveScan(true);
        pBLEScan->setInterval(1349);
        pBLEScan->setWindow(449);
        #ifdef DISPLAYSSD1306
           // making sure, display has proper values during loop lock
           disp_setBTPrevStateIcon(0);
           wrDisp_blueToothSignal(false);
        #endif

        //this scan locks the loop for 10 second, meaning all blinking leds and screen updates are not happening.
        //the bluetooth icon will not blink correctly every 10 seconds for 5 seconds when a scan is happenig and is not connected
        #ifdef DISPLAYSSD1306
           // update status to BleScan
           wrDisp_Status("BLEScan..");
        #endif
        pBLEScan->start(5, false);
        #ifdef DISPLAYSSD1306
          // update status to BleScan
          wrDisp_Status("Running!");
        #endif 
      }
    }
  }
}

void btResetStack()
{
  connected=false;

}

bool isBTconnected(){
  return connected;
}

unsigned long getLastBTMessageTime(){
    return lastBTMessage;
}


