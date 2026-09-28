#ifndef DEVICE_REGISTRY_H
#define DEVICE_REGISTRY_H
#include "Arduino.h"
#include "DeviceType.h"

// One entry per supported power station model with pointers to its tables (defined in Device_*.h)
typedef struct {
  int type;                 // AC300, AC200M, ... (PowerStation.h)
  const char* name;         // shown in the setup portal
  const char* blePrefix;    // start of the Bluetooth name, used for auto detection
  const device_field_data_t* state;
  size_t stateCount;
  const device_field_data_t* command;
  size_t commandCount;
  const device_field_data_t* polling;
  size_t pollingCount;
} DeviceDef;

// Model in use: the one chosen in the setup portal, else detected from the Bluetooth name,
// else BLUETTI_TYPE from config.h
extern const DeviceDef* activeDevice();
// how the model was chosen: "manual" (setup portal), "auto" (Bluetooth name) or "default" (config.h)
extern const char* activeDeviceSource();
extern const DeviceDef* deviceByType(int type);
extern const DeviceDef* deviceByBleName(const char* bleName);
extern size_t deviceCount();
extern const DeviceDef* deviceAt(size_t index);

#endif
