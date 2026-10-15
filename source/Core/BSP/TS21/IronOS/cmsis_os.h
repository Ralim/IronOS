#ifndef TS21_CMSIS_OS_H
#define TS21_CMSIS_OS_H

#include "FreeRTOS.h"
#include "semphr.h"
#include "task.h"

typedef TaskHandle_t osThreadId;
typedef StaticTask_t osStaticThreadDef_t;
typedef struct {
  TaskFunction_t function;
  const char    *name;
  uint32_t       depth;
  UBaseType_t    priority;
  StackType_t   *stack;
  StaticTask_t  *control;
} osThreadDef_t;

enum { osPriorityBelowNormal = 2, osPriorityNormal = 3, osPriorityAboveNormal = 4, osPriorityRealtime = 6 };

#define osThreadStaticDef(name, function, priority, instances, depth, stack, control) const osThreadDef_t os_thread_##name = {(TaskFunction_t)(function), #name, depth, priority, stack, control}
#define osThread(name)                                                                (&os_thread_##name)

static inline osThreadId osThreadCreate(const osThreadDef_t *definition, void *argument) {
  TaskHandle_t result = xTaskCreateStatic(definition->function, definition->name, definition->depth, argument, definition->priority, definition->stack, definition->control);
  configASSERT(result != NULL);
  return result;
}
static inline void osDelay(uint32_t ticks) { vTaskDelay(ticks); }
static inline void osKernelStart(void) {
  vTaskStartScheduler();
  ts21_fault(TS21_ERROR_TRAP);
}

#endif
