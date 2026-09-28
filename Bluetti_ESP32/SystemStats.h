#ifndef SYSTEM_STATS_H
#define SYSTEM_STATS_H
#include "Arduino.h"

// CPU load per core, estimated from the time the FreeRTOS idle task ran
extern void initSystemStats();
// refreshes the load values, at most once every 2 seconds
extern void updateSystemStats();
// 0-100, core 0 or 1
extern int cpuLoadPercent(int core);

#endif
