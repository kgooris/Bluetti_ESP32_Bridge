#include "DeviceRegistry.h"
#include "PowerStation.h"
#include "config.h"
#include "BWifi.h"

#include "Device_AC300.h"
#include "Device_AC200M.h"
#include "Device_EP500.h"
#include "Device_EB3A.h"
#include "Device_EP500P.h"
#include "Device_AC500.h"
#include "Device_EP600.h"

#define COUNT_OF(table) (sizeof(table) / sizeof(device_field_data_t))
#define DEVICE_DEF(TYPE, NAME, PREFIX, NS) \
  { TYPE, NAME, PREFIX, \
    NS::bluetti_device_state, COUNT_OF(NS::bluetti_device_state), \
    NS::bluetti_device_command, COUNT_OF(NS::bluetti_device_command), \
    NS::bluetti_polling_command, COUNT_OF(NS::bluetti_polling_command) }

static const DeviceDef devices[] = {
  DEVICE_DEF(AC300,  "AC300",  "AC300",  ac300),
  DEVICE_DEF(AC200M, "AC200M", "AC200M", ac200m),
  DEVICE_DEF(EP500,  "EP500",  "EP500",  ep500),
  DEVICE_DEF(EP500P, "EP500P", "EP500P", ep500p),
  DEVICE_DEF(EB3A,   "EB3A",   "EB3A",   eb3a),
  DEVICE_DEF(AC500,  "AC500",  "AC500",  ac500),
  DEVICE_DEF(EP600,  "EP600",  "EP600",  ep600),
};

size_t deviceCount(){
  return sizeof(devices) / sizeof(devices[0]);
}

const DeviceDef* deviceAt(size_t index){
  return index < deviceCount() ? &devices[index] : nullptr;
}

const DeviceDef* deviceByType(int type){
  for (size_t i = 0; i < deviceCount(); i++){
    if (devices[i].type == type) return &devices[i];
  }
  return nullptr;
}

// longest matching prefix wins, so "EP500P..." is not taken for an EP500
const DeviceDef* deviceByBleName(const char* bleName){
  const DeviceDef* best = nullptr;
  size_t bestLength = 0;
  for (size_t i = 0; i < deviceCount(); i++){
    size_t length = strlen(devices[i].blePrefix);
    if (length > bestLength && strncasecmp(bleName, devices[i].blePrefix, length) == 0){
      best = &devices[i];
      bestLength = length;
    }
  }
  return best;
}

static const char* activeSource = "default";

const char* activeDeviceSource(){
  activeDevice(); // makes sure the choice has been made
  return activeSource;
}

const DeviceDef* activeDevice(){
  static const DeviceDef* active = nullptr;
  if (active != nullptr){
    return active;
  }

  ESPBluettiSettings settings = get_esp32_bluetti_settings();
  const char* source = "config.h default";
  const DeviceDef* device = nullptr;
  if (settings.salt == EEPROM_SALT){
    device = deviceByType(settings.bluetti_type);
    source = "setup portal";
    activeSource = "manual";
    if (device == nullptr){
      device = deviceByBleName(settings.bluetti_device_id);
      source = "Bluetooth name";
      activeSource = "auto";
    }
  }
  if (device == nullptr){
    device = deviceByType(BLUETTI_TYPE);
    source = "config.h default";
    activeSource = "default";
  }
  if (device == nullptr){
    device = &devices[0];
  }
  Serial.printf("[Device] Power station model %s (%s)\n", device->name, source);
  active = device;
  return active;
}
