#include "BWifi.h"
#include "BTooth.h"
#include "MQTT.h"
#include "config.h"
#include "display.h"
#include "SystemStats.h"

unsigned long lastTime1 = 0;
unsigned long timerDelay1 = 3000;

void setup() {
  Serial.begin(115200);
  #ifdef RELAISMODE
    pinMode(RELAIS_PIN, OUTPUT);
    #ifdef DEBUG
      Serial.println(F("deactivate relais contact"));
    #endif
    digitalWrite(RELAIS_PIN, RELAIS_LOW);
  #endif
  #ifdef SLEEP_TIME_ON_BT_NOT_AVAIL
    esp_sleep_enable_timer_wakeup(SLEEP_TIME_ON_BT_NOT_AVAIL * 60 * 1000000ULL);
  #endif
  initSystemStats();
  loadBluettiSettings();
  if (isDisplayEnabled()){
    initDisplay();
  }
  initBWifi(false);
  initBluetooth();
  initMQTT();
  wrDisp_Status("Running!");
}

void loop() {
  handleDisplay();
  handleBluetooth();
  handleMQTT(); 
  handleWebserver();
  delay(1); // lets the idle task run, needed for the CPU load and lowers power use
}
