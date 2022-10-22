/****************************************************************************
FILE NAME
    ble_peripheral_tp_server.c

DESCRIPTION
    test profile demo

NOTES
*/
/****************************************************************************/

#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <FreeRTOS.h>
#include "bflb_platform.h"
#include <task.h>
#include "BSP.h"
#include "bluetooth.h"
#include "conn.h"
#include "gatt.h"
#include "hci_core.h"
#include "uuid.h"
#include "ble_peripheral_tp_server.h"
#include "log.h"
#include "bl702_glb.h"




int ble_start_adv(void)
{
    struct bt_le_adv_param adv_param = {
        //options:3, connectable undirected, adv one time
        .options = 3,
        .interval_min = BT_GAP_ADV_FAST_INT_MIN_3,
        .interval_max = BT_GAP_ADV_FAST_INT_MAX_3,
    };

    char *adv_name = "BL_TEST_01"; // This name must be the same as adv_name in ble_central
    struct bt_data adv_data[2] = {
        BT_DATA_BYTES(BT_DATA_FLAGS, (BT_LE_AD_NO_BREDR | BT_LE_AD_GENERAL)),
        BT_DATA(BT_DATA_NAME_COMPLETE, adv_name, strlen(adv_name)),
    };

    return bt_le_adv_start(&adv_param, adv_data, ARRAY_SIZE(adv_data), &adv_data[1], 1);
}


void bt_enable_cb(int err)
{
    ble_tp_init();

    ble_start_adv();
}



void ble_stack_start(void)
{
    MSG("[OS] ble_stack_start...\r\n");
    GLB_Set_EM_Sel(GLB_EM_8KB);
    MSG("[OS] ble_controller_init...\r\n");
    ble_controller_init(configMAX_PRIORITIES - 1);
    MSG("[OS] ble_controller_init...Done\r\n");

    // // Initialize BLE Host stack
    MSG("[OS] hci_driver_init...\r\n");
    hci_driver_init();
    MSG("[OS] hci_driver_init...Done\r\n");

    MSG("[OS] bt_enable...\r\n");
    bt_enable(bt_enable_cb);
    MSG("[OS] bt_enable...Done\r\n");
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



void vApplicationMallocFailedHook(void)
{
    MSG("vApplicationMallocFailedHook\r\n");

    while (1)
        ;
}