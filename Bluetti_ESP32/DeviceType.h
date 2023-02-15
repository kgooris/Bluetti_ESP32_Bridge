#ifndef __DEVICE_TYPE_H__
#define __DEVICE_TYPE_H__
#include "Arduino.h"

enum field_types{
   UINT_FIELD,
   BOOL_FIELD,
   ENUM_FIELD,
   STRING_FIELD,
   DECIMAL_ARRAY_FIELD,
   DECIMAL_FIELD,
   VERSION_FIELD,
   SN_FIELD, 
   TYPE_UNDEFINED
};

enum field_names {
  DC_OUTPUT_POWER,
  DC_OUTPUT_ON,
  AC_OUTPUT_POWER,
  AC_OUTPUT_ON,
  AC_OUTPUT_MODE,
  POWER_GENERATION,
  TOTAL_BATTERY_PERCENT, 
  DC_INPUT_POWER,
  AC_INPUT_POWER,
  AC_INPUT_VOLTAGE,
  AC_INPUT_FREQUENCY,
  PACK_VOLTAGE,
  SERIAL_NUMBER,
  ARM_VERSION,
  DSP_VERSION,
  DEVICE_TYPE, 
  UPS_MODE,
  AUTO_SLEEP_MODE,
  GRID_CHARGE_ON,
  FIELD_UNDEFINED,
  INTERNAL_AC_VOLTAGE,
  INTERNAL_AC_FREQUENCY,
  INTERNAL_CURRENT_ONE,
  INTERNAL_POWER_ONE,
  INTERNAL_CURRENT_TWO,
  INTERNAL_POWER_TWO,
  INTERNAL_CURRENT_THREE,
  INTERNAL_POWER_THREE,
  INTERNAL_DC_INPUT_VOLTAGE,
  INTERNAL_DC_INPUT_POWER,
  INTERNAL_DC_INPUT_CURRENT,
  PACK_BATTERY_PERCENT,
  PACK_NUM,
  INTERNAL_PACK_VOLTAGE,
  INTERNAL_CELL01_VOLTAGE,
  INTERNAL_CELL02_VOLTAGE,
  INTERNAL_CELL03_VOLTAGE,
  INTERNAL_CELL04_VOLTAGE,
  INTERNAL_CELL05_VOLTAGE,
  INTERNAL_CELL06_VOLTAGE,
  INTERNAL_CELL07_VOLTAGE,
  INTERNAL_CELL08_VOLTAGE,
  INTERNAL_CELL09_VOLTAGE,
  INTERNAL_CELL10_VOLTAGE,
  INTERNAL_CELL11_VOLTAGE,
  INTERNAL_CELL12_VOLTAGE,
  INTERNAL_CELL13_VOLTAGE,
  INTERNAL_CELL14_VOLTAGE,
  INTERNAL_CELL15_VOLTAGE,
  INTERNAL_CELL16_VOLTAGE,
  PACK_NUM_MAX
};

typedef struct device_field_data {
  enum field_names f_name;
  uint8_t f_page;
  uint8_t f_offset;
  int8_t f_size;
  int8_t f_scale;
  int8_t f_enum;
  enum field_types f_type;
} device_field_data_t;


#endif
