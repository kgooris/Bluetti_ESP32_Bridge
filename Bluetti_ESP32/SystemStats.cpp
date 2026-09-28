#include "SystemStats.h"
#include "esp_freertos_hooks.h"
#include "esp_timer.h"

// The idle hook returns false, so FreeRTOS calls it again and again as fast as possible while the core
// has nothing else to do. Two calls close together mean the core was idle in between, a longer gap
// means another task ran. Adding up the short gaps gives the idle time of each core.
static const int64_t MAX_IDLE_GAP_US = 200;

static volatile uint32_t idleUs[2] = {0, 0};
static int64_t lastCallUs[2] = {0, 0};
static uint32_t lastIdleUs[2] = {0, 0};
static int64_t lastSampleUs = 0;
static int load[2] = {0, 0};

static void countIdle(int core){
  int64_t now = esp_timer_get_time();
  int64_t gap = now - lastCallUs[core];
  if (lastCallUs[core] != 0 && gap < MAX_IDLE_GAP_US){
    idleUs[core] += (uint32_t)gap;
  }
  lastCallUs[core] = now;
}

static bool idleHookCore0(){
  countIdle(0);
  return false;
}

static bool idleHookCore1(){
  countIdle(1);
  return false;
}

void initSystemStats(){
  esp_register_freertos_idle_hook_for_cpu(idleHookCore0, 0);
  esp_register_freertos_idle_hook_for_cpu(idleHookCore1, 1);
  lastSampleUs = esp_timer_get_time();
}

void updateSystemStats(){
  int64_t now = esp_timer_get_time();
  int64_t elapsed = now - lastSampleUs;
  if (elapsed < 2000000){
    return;
  }
  for (int core = 0; core < 2; core++){
    uint32_t idleNow = idleUs[core];
    uint32_t idle = idleNow - lastIdleUs[core]; // unsigned, survives the counter wrapping
    lastIdleUs[core] = idleNow;
    int idlePercent = (int)((int64_t)idle * 100 / elapsed);
    if (idlePercent > 100) idlePercent = 100;
    load[core] = 100 - idlePercent;
  }
  lastSampleUs = now;
}

int cpuLoadPercent(int core){
  return (core == 0 || core == 1) ? load[core] : 0;
}
