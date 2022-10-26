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
#include "ble_peripheral.h"
#include "log.h"
#include "bl702_glb.h"
#include "ble_characteristics.h"
#include "hal_clock.h"
#include "ble.h"




void ble_stack_start(void)
{
    MSG("BLE Starting\n");
    GLB_Set_EM_Sel(GLB_EM_8KB);
    ble_controller_init(configMAX_PRIORITIES - 1);

    // // Initialize BLE Host stack
    hci_driver_init();

    bt_enable(bt_enable_cb);
    MSG("BLE Starting...Done\n");
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