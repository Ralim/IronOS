#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

#include "Interrupts.h"
#include "ts21.h"
#include <stdint.h>

#define configUSE_PREEMPTION                    1
#define configUSE_PORT_OPTIMISED_TASK_SELECTION 0
#define configCPU_CLOCK_HZ                      TS21_CLOCK_HZ
#define configTICK_RATE_HZ                      1000
#define configMAX_PRIORITIES                    7
#define configMINIMAL_STACK_SIZE                128
#define configMAX_TASK_NAME_LEN                 12
#define configIDLE_SHOULD_YIELD                 0
#define configUSE_TASK_NOTIFICATIONS            1
#define configUSE_MUTEXES                       1
#define configUSE_RECURSIVE_MUTEXES             0
#define configUSE_COUNTING_SEMAPHORES           0
#define configQUEUE_REGISTRY_SIZE               0
#define configUSE_TIME_SLICING                  1
#define configUSE_NEWLIB_REENTRANT              0
#define configENABLE_BACKWARD_COMPATIBILITY     0
#define configTICK_TYPE_WIDTH_IN_BITS           TICK_TYPE_WIDTH_32_BITS
#define configSUPPORT_STATIC_ALLOCATION         1
#define configSUPPORT_DYNAMIC_ALLOCATION        0
#define configUSE_IDLE_HOOK                     1
#define configUSE_TICK_HOOK                     1
#define configCHECK_FOR_STACK_OVERFLOW          2
#define configUSE_MALLOC_FAILED_HOOK            0
#define configUSE_TRACE_FACILITY                0
#define configUSE_CO_ROUTINES                   0
#define configUSE_TIMERS                        0
#define configMTIME_BASE_ADDRESS                0
#define configMTIMECMP_BASE_ADDRESS             0
#define INCLUDE_vTaskDelay                      1
#define INCLUDE_vTaskSuspend                    1
#define INCLUDE_xTaskGetSchedulerState          1
#define INCLUDE_xTaskGetCurrentTaskHandle       1
#define INCLUDE_uxTaskGetStackHighWaterMark     1
#define INCLUDE_vTaskDelayUntil                 1
#define configASSERT(x)                                                                                                                                                                                \
  do {                                                                                                                                                                                                 \
    if (!(x)) {                                                                                                                                                                                        \
      ts21_fault(TS21_ERROR_TRAP);                                                                                                                                                                     \
    }                                                                                                                                                                                                  \
  } while (0)

#endif
