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
#include "bflb_platform.h"
#include "hal_uart.h"
#include <FreeRTOS.h>
#include "semphr.h"
#include "bluetooth.h"
#include "gap.h"
#include "bl702_glb.h"
#include "ble_central_tp_client.h"
#include "conn.h"
#include "log.h"
#include "ble_lib_api.h"
#include "hci_driver.h"
#include "bl702_sec_eng.h"

#define NAME_LEN 30

extern uint8_t _heap_start;
extern uint8_t _heap_size; // @suppress("Type cannot be resolved")
static HeapRegion_t xHeapRegions[] = {
    { &_heap_start, (unsigned int)&_heap_size },
    { NULL, 0 }, /* Terminates the array. */
    { NULL, 0 }  /* Terminates the array. */
};

uint8_t sharedBuf[16];

void user_vAssertCalled(void) __attribute__((weak, alias("vAssertCalled")));
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

void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName)
{
    MSG("vApplicationStackOverflowHook\r\n");

    if (pcTaskName) {
        MSG("Stack name %s\r\n", pcTaskName);
    }

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

/*************************************************************************
NAME
    data_cb
*/
static bool data_cb(struct bt_data *data, void *user_data)
{
    char *name = user_data;
    u8_t len;

    switch (data->type) {
        case BT_DATA_NAME_SHORTENED:
        case BT_DATA_NAME_COMPLETE:
            len = (data->data_len > NAME_LEN - 1) ? (NAME_LEN - 1) : (data->data_len);
            memcpy(name, data->data, len);
            return false;

        default:
            return true;
    }
}

/*************************************************************************
NAME
    device_found
*/

static void device_found(const bt_addr_le_t *addr, s8_t rssi, u8_t evtype,
                         struct net_buf_simple *buf)
{
    char le_addr[BT_ADDR_LE_STR_LEN];
    char name[30];
    int err;
    char *adv_name = "BL_TEST_01"; //This name must be the same as adv_name in ble_central

    (void)memset(name, 0, sizeof(name));
    bt_data_parse(buf, data_cb, name);
    bt_addr_le_to_str(addr, le_addr, sizeof(le_addr));

    BT_WARN("[DEVICE]: %s, AD evt type %u, RSSI %i %s", le_addr, evtype, rssi, name);

    if (strcmp(name, adv_name) == 0) {
        struct bt_conn *conn;
        struct bt_le_conn_param param = {
            .interval_min = BT_GAP_INIT_CONN_INT_MIN,
            .interval_max = BT_GAP_INIT_CONN_INT_MAX,
            .latency = 0,
            .timeout = 400,
        };
        err = bt_le_scan_stop();

        if (err) {
            BT_WARN("Stopping scanning failed (err %d)", err);
        } else {
            BT_WARN("Scan successfully stopped");
        }

        conn = bt_conn_create_le(addr, &param);

        if (!conn) {
            BT_WARN("Connection failed");
        } else {
            BT_WARN("Connection pending");
        }
    }
}

/*************************************************************************
NAME
     ble_start_scan
*/
static void ble_start_scan(void)
{
    int err;
    struct bt_le_scan_param scan_param = {
        .type = 0,
        .filter_dup = 1,
        .interval = BT_GAP_SCAN_FAST_INTERVAL,
        .window = BT_GAP_SCAN_FAST_WINDOW,
    };
    err = bt_le_scan_start(&scan_param, device_found);

    if (err) {
        BT_WARN("Failed to start scan (err %d)", err);
    } else {
        BT_WARN("Start scan successfully");
    }
}

/*************************************************************************
NAME
     bt_enable_cb
*/

static void bt_enable_cb(int err)
{
    ble_tp_init();
    ble_start_scan();
}

/*************************************************************************
NAME
     ble_stack_start
*/
void ble_stack_start(void)
{
    GLB_Set_EM_Sel(GLB_EM_8KB);

    // Initialize BLE controller
    MSG("[OS] ble_controller_init...\r\n");
    ble_controller_init(configMAX_PRIORITIES - 1);

    // Initialize BLE Host stack
    MSG("[OS] hci_driver_init...\r\n");

    hci_driver_init();

    MSG("[OS] bt_enable...\r\n");
    bt_enable(bt_enable_cb);
}

/*************************************************************************
NAME
    ble_init
*/
void ble_init(void)
{
    ble_stack_start();
}

/*************************************************************************
NAME
    ble_init_task
*/
static void ble_init_task(void *pvParameters)
{
    ble_init();
    vTaskDelete(NULL);
}

int main(void)
{
    static StackType_t ble_init_stack[1024];
    static StaticTask_t ble_init_task_h;
    uint32_t tmpVal = 0;

    bflb_platform_init(0);
    HBN_Set_XCLK_CLK_Sel(HBN_XCLK_CLK_XTAL);

    //Set capcode
    tmpVal = BL_RD_REG(AON_BASE, AON_XTAL_CFG);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_XTAL_CAPCODE_IN_AON, 33);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_XTAL_CAPCODE_OUT_AON, 33);
    BL_WR_REG(AON_BASE, AON_XTAL_CFG, tmpVal);

    Sec_Eng_Trng_Enable();

    vPortDefineHeapRegions(xHeapRegions);

    MSG("[OS] ble init  task...\r\n");
    xTaskCreateStatic(ble_init_task, (char *)"ble_init", sizeof(ble_init_stack) / 4, NULL, 15, ble_init_stack, &ble_init_task_h);

    vTaskStartScheduler();

    BL_CASE_SUCCESS;
    while (1) {
        bflb_platform_delay_ms(100);
    }
}
