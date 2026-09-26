#ifndef BWIFI_H
#define BWIFI_H
#include "Arduino.h"
#include "config.h"

typedef struct{
  int  salt = EEPROM_SALT;
  char mqtt_server[40] = "127.0.0.1";
  char mqtt_port[6]  = "1883";
  char mqtt_username[40] = "";
  char mqtt_password[40] = "";
  char bluetti_device_id[40] = "Bluetti Blutetooth Id";
  char ota_username[40] = "";
  char ota_password[40] = "";
  // 0 = not set (use default), 1 = disabled, 2 = enabled. Appended at the end so older EEPROM contents stay valid.
  uint8_t display_enabled = 0;
  uint8_t ha_discovery = 0;
} ESPBluettiSettings;

extern ESPBluettiSettings get_esp32_bluetti_settings();
extern void loadBluettiSettings();
extern bool isDisplayEnabled();
extern bool isHaDiscoveryEnabled();
extern void initBWifi(bool resetWifi);
extern void handleWebserver();
String processorWebsiteUpdates(const String& var);
extern void AddtoMsgView(String data);
  
#endif
