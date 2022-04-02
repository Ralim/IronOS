/**
 * @file main.c
 * @brief
 *
 * Copyright (c) 2021 Bouffalolab team
 *
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.  The
 * ASF licenses this file to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance with the
 * License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.  See the
 * License for the specific language governing permissions and limitations
 * under the License.
 *
 */
#include "hal_uart.h"
#include <FreeRTOS.h>
#include "semphr.h"
#include "bflb_platform.h"

static uint8_t freertos_heap[configTOTAL_HEAP_SIZE];

static HeapRegion_t xHeapRegions[] = {
    { (uint8_t *)freertos_heap, 0 },
    { NULL, 0 }, /* Terminates the array. */
    { NULL, 0 }  /* Terminates the array. */
};
static StackType_t consumer_stack[512];
static StaticTask_t consumer_handle;
static StackType_t producer_stack[512];
static StaticTask_t producer_handle;

uint8_t sharedBuf[16];
SemaphoreHandle_t sem_empty = NULL;
SemaphoreHandle_t sem_full = NULL;
SemaphoreHandle_t mtx_lock = NULL;

void vAssertCalled(void)
{
    MSG("vAssertCalled\r\n");

    while (1)
        ;
}

void vApplicationTickHook(void)
{
    //MSG("vApplicationTickHook\r\n");
}

void vApplicationStackOverflowHook(void)
{
    MSG("vApplicationStackOverflowHook\r\n");

    while (1)
        ;
}

void vApplicationMallocFailedHook(void)
{
    MSG("vApplicationMallocFailedHook\r\n");

    while (1)
        ;
}
void vApplicationGetIdleTaskMemory(StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize)
{
    /* If the buffers to be provided to the Idle task are declared inside this
    function then they must be declared static - otherwise they will be allocated on
    the stack and so not exists after this function exits. */
    static StaticTask_t xIdleTaskTCB;
    static StackType_t uxIdleTaskStack[configMINIMAL_STACK_SIZE];

    /* Pass out a pointer to the StaticTask_t structure in which the Idle task's
    state will be stored. */
    *ppxIdleTaskTCBBuffer = &xIdleTaskTCB;

    /* Pass out the array that will be used as the Idle task's stack. */
    *ppxIdleTaskStackBuffer = uxIdleTaskStack;

    /* Pass out the size of the array pointed to by *ppxIdleTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configMINIMAL_STACK_SIZE is specified in words, not bytes. */
    *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
}

/* configSUPPORT_STATIC_ALLOCATION and configUSE_TIMERS are both set to 1, so the
application must provide an implementation of vApplicationGetTimerTaskMemory()
to provide the memory that is used by the Timer service task. */
void vApplicationGetTimerTaskMemory(StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize)
{
    /* If the buffers to be provided to the Timer task are declared inside this
    function then they must be declared static - otherwise they will be allocated on
    the stack and so not exists after this function exits. */
    static StaticTask_t xTimerTaskTCB;
    static StackType_t uxTimerTaskStack[configTIMER_TASK_STACK_DEPTH];

    /* Pass out a pointer to the StaticTask_t structure in which the Timer
    task's state will be stored. */
    *ppxTimerTaskTCBBuffer = &xTimerTaskTCB;

    /* Pass out the array that will be used as the Timer task's stack. */
    *ppxTimerTaskStackBuffer = uxTimerTaskStack;

    /* Pass out the size of the array pointed to by *ppxTimerTaskStackBuffer.
    Note that, as the array is necessarily of type StackType_t,
    configTIMER_TASK_STACK_DEPTH is specified in words, not bytes. */
    *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
}

static void consumer_task(void *pvParameters)
{
    MSG("Consumer task enter \r\n");
    vTaskDelay(1000);
    MSG("Consumer task start \r\n");
    MSG("begin to loop %s\r\n", __FILE__);

    while (1) {
        if (xSemaphoreTake(sem_full, portMAX_DELAY) == pdTRUE) {
            xSemaphoreTake(mtx_lock, portMAX_DELAY);
            MSG("Consumer get:%s\r\n", sharedBuf);
            xSemaphoreGive(mtx_lock);
            xSemaphoreGive(sem_empty);
        } else {
            MSG("Take sem_full fail\r\n");
        }
    }

    vTaskDelete(NULL);
}

static void producer_task(void *pvParameters)
{
    uint8_t buf = 100;

    MSG("Producer task enter \r\n");
    vTaskDelay(1000);
    MSG("Producer task start \r\n");

    while (1) {
        if (xSemaphoreTake(sem_empty, portMAX_DELAY) == pdTRUE) {
            xSemaphoreTake(mtx_lock, portMAX_DELAY);
            buf++;
            sprintf((char *)sharedBuf, "%d", buf);
            MSG("Producer generates:%s\r\n", sharedBuf);
            xSemaphoreGive(mtx_lock);
            xSemaphoreGive(sem_full);
            vTaskDelay(buf);
        } else {
            MSG("Take sem_empty fail\r\n");
        }
    }

    vTaskDelete(NULL);
}

int main(void)
{
    bflb_platform_init(0);

    xHeapRegions[0].xSizeInBytes = configTOTAL_HEAP_SIZE;
    vPortDefineHeapRegions(xHeapRegions);

    configASSERT((configMAX_PRIORITIES > 4));

    /* Create semaphore */
    vSemaphoreCreateBinary(sem_empty);
    vSemaphoreCreateBinary(sem_full);
    vSemaphoreCreateBinary(mtx_lock);

    if (sem_empty == NULL || sem_full == NULL || mtx_lock == NULL) {
        MSG("Create sem fail\r\n");
        BL_CASE_FAIL;
    }

    MSG("[OS] Starting consumer task...\r\n");
    xTaskCreateStatic(consumer_task, (char *)"consumer_task", sizeof(consumer_stack) / 4, NULL, configMAX_PRIORITIES - 2, consumer_stack, &consumer_handle);
    MSG("[OS] Starting producer task...\r\n");
    xTaskCreateStatic(producer_task, (char *)"producer_task", sizeof(producer_stack) / 4, NULL, configMAX_PRIORITIES - 3, producer_stack, &producer_handle);

    vTaskStartScheduler();

    BL_CASE_SUCCESS;
    while (1) {
        bflb_platform_delay_ms(100);
    }
}
