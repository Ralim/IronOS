#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define configUSE_PREEMPTION                    1
#define configSUPPORT_STATIC_ALLOCATION         1
#define configSUPPORT_DYNAMIC_ALLOCATION        0
#define configUSE_IDLE_HOOK                     1
#define configUSE_TICK_HOOK                     0
#define configCPU_CLOCK_HZ                      (60000000) // FREQ_SYS, PLL 60 MHz (see Setup.cpp)
#define configTICK_RATE_HZ                      (1000)
#define configMAX_PRIORITIES                    (7)
#define configMINIMAL_STACK_SIZE                ((uint16_t)192)
#define configTOTAL_HEAP_SIZE                   ((size_t)0)
#define configMAX_TASK_NAME_LEN                 (16)
#define configUSE_TASK_NOTIFICATIONS            1
#define configUSE_MUTEXES                       1
#define configQUEUE_REGISTRY_SIZE               0
#define configUSE_TIMERS                        0
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 1
#define configCHECK_FOR_STACK_OVERFLOW          2
#define configTICK_TYPE_WIDTH_IN_BITS           TICK_TYPE_WIDTH_32_BITS
#define configUSE_CO_ROUTINES                   0
#define configMAX_CO_ROUTINE_PRIORITIES         (2)

#define INCLUDE_vTaskPrioritySet            1
#define INCLUDE_uxTaskPriorityGet           0
#define INCLUDE_vTaskDelete                 0
#define INCLUDE_vTaskCleanUpResources       0
#define INCLUDE_vTaskSuspend                0
#define INCLUDE_vTaskDelayUntil             1
#define INCLUDE_vTaskDelay                  1
#define INCLUDE_xTaskGetSchedulerState      1
#define INCLUDE_uxTaskGetStackHighWaterMark 1

void fault_halt(void); // startup_CH571.S: heater off, spin until the watchdog resets
#define configASSERT(x)                                                                                                                                                                                \
  if ((x) == 0) {                                                                                                                                                                                      \
    fault_halt();                                                                                                                                                                                      \
  }

#ifdef __cplusplus
}
#endif

#endif /* FREERTOS_CONFIG_H */
