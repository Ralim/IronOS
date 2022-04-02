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
#include "ble_peripheral_tp_server.h"
#include "ble_lib_api.h"
#include "hci_driver.h"
#include "bl702_sec_eng.h"
#include "hal_clock.h"
#include "bl702_romdriver.h"

#if defined(CONFIG_BT_OAD_SERVER)
#include "oad_main.h"
#include "oad_service.h"
#endif

#include "hal_pm.h"
#include "hal_pm_util.h"

extern uint32_t __hbn_load_addr;
extern uint32_t __hbn_ram_start__;
extern uint32_t __hbn_ram_end__;

extern uint32_t __itcm_load_addr;
extern uint32_t __dtcm_load_addr;
extern uint32_t __system_ram_load_addr;

extern uint32_t __tcm_code_start__;
extern uint32_t __tcm_code_end__;
extern uint32_t __tcm_data_start__;
extern uint32_t __tcm_data_end__;
extern uint32_t __system_ram_data_start__;
extern uint32_t __system_ram_data_end__;

#define ITCH_LOAD_ADDR __itcm_load_addr
#define TCM_CODE_START __tcm_code_start__
#define TCM_CODE_END   __tcm_code_end__

#define DTCH_LOAD_ADDR __dtcm_load_addr
#define TCM_DATA_START __tcm_data_start__
#define TCM_DATA_END   __tcm_data_start__

#define MASK_DEBUG_MSG 0

extern uint8_t _heap_start;
extern uint8_t _heap_size; // @suppress("Type cannot be resolved")
extern uint8_t _heap2_start;
extern uint8_t _heap2_size; // @suppress("Type cannot be resolved")

static HeapRegion_t xHeapRegions[] = {
    { &_heap_start, (unsigned int)&_heap_size },
    { &_heap2_start, (unsigned int)&_heap2_size },
    { NULL, 0 }, /* Terminates the array. */
    { NULL, 0 }  /* Terminates the array. */
};

bool pds_start = false;
bool wfi_in = false;

#define TIME_5MS_IN_32768CYCLE (164) // (45000/(1000000/32768))
void bl_pds_restore(void);

//For test
extern int32_t rwip_get_sleep_stat_cnt(void);
//end

uint8_t sharedBuf[16];
void user_vAssertCalled(void) __attribute__((weak, alias("vAssertCalled")));

static void ATTR_PDS_RAM_SECTION bl702_low_power_config(void)
{
    uint8_t i;
    static uint32_t regValue = 0;

    // Power off DLL
    GLB_Power_Off_DLL();

    // Disable secure engine
    Sec_Eng_Trng_Disable();
    SEC_Eng_Turn_Off_Sec_Ring();

    // Disable Zigbee clock
    GLB_Set_MAC154_ZIGBEE_CLK(0);

    // Set GPIO to High-Z state
    for (i = 0; i <= 37; i++) {
// jtag pins
#if 0
        if((i == 0) || (i == 1) || (i == 2) || (i == 9)){
            continue;
        }
#endif

        // uart pins
        if ((i == 14) || (i == 15)) {
            continue;
        }
        // flash pins
        if ((i >= 23) && (i <= 28)) {
            continue;
        }

        GLB_GPIO_Set_HZ(i);
    }

    if (regValue == 0) {
        // Gate peripheral clock
        for (i = 0; i <= 31; i++) {
            if (i == BL_AHB_SLAVE1_GLB) {
                continue;
            }

            if (i == BL_AHB_SLAVE1_MIX) {
                continue;
            }

            if (i == BL_AHB_SLAVE1_EFUSE) {
                continue;
            }

            if (i == BL_AHB_SLAVE1_L1C) {
                continue;
            }

            if (i == BL_AHB_SLAVE1_SFC) {
                continue;
            }

            if (i == BL_AHB_SLAVE1_PDS_HBN_AON_HBNRAM) {
                continue;
            }

            if (i == BL_AHB_SLAVE1_UART0) {
                continue;
            }

            if (i == BL_AHB_SLAVE1_TMR) {
                continue;
            }

            GLB_AHB_Slave1_Clock_Gate(1, i);
            regValue = BL_RD_REG(GLB_BASE, GLB_CGEN_CFG1);
        }

    } else {
        BL_WR_REG(GLB_BASE, GLB_CGEN_CFG1, regValue);
    }
}

void vApplicationIdleHook(void)
{
    if (!wfi_in) {
        __asm volatile(
            "   wfi     ");
        /*empty*/
    }
}
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

extern bool le_check_valid_scan(void);
extern bool le_check_valid_conn(void);
//Allocate retention memory
void *bl_alloc_retmem(size_t xWantedSize)
{
    return pvPortMalloc(xWantedSize);
}

void bflb_load_hbn_ram(void)
{
    uint32_t *pSrc, *pDest;
    /* BF Add HBNRAM data copy */
    pSrc = &__hbn_load_addr;
    pDest = &__hbn_ram_start__;

    for (; pDest < &__hbn_ram_end__;) {
        *pDest++ = *pSrc++;
    }
}

// can be placed in flash, here placed in pds section to reduce fast boot time
static void ATTR_PDS_RAM_SECTION user_pds_restore_tcm(void)
{
    uint32_t src = 0;
    uint32_t dst = 0;
    uint32_t end = 0;

    /* Copy ITCM code */
    src = (uint32_t)&ITCH_LOAD_ADDR;
    dst = (uint32_t)&TCM_CODE_START;
    end = (uint32_t)&TCM_CODE_END;

    while (dst < end) {
        *(uint32_t *)dst = *(uint32_t *)src;
        src += 4;
        dst += 4;
    }
}

/**
 * @brief gate all clock but CPU and ble
 *
 */
static void ATTR_HBN_RAM_SECTION system_clock_gate(void)
{
    uint32_t tmpVal;
    tmpVal = BL_RD_REG(GLB_BASE, GLB_CGEN_CFG1);
    tmpVal &= (~(1 << BL_AHB_SLAVE1_GPIP));    //2
    tmpVal &= (~(1 << BL_AHB_SLAVE1_SEC_DBG)); //3
    tmpVal &= (~(1 << BL_AHB_SLAVE1_SEC));     //4
    tmpVal &= (~(1 << BL_AHB_SLAVE1_TZ1));     //5
    tmpVal &= (~(1 << BL_AHB_SLAVE1_TZ2));     //6
    tmpVal &= (~(1 << BL_AHB_SLAVE1_DMA));     //12
    tmpVal &= (~(1 << BL_AHB_SLAVE1_EMAC));    //13
    // tmpVal &= (~(1 << BL_AHB_SLAVE1_UART0));//14
    tmpVal &= (~(1 << BL_AHB_SLAVE1_UART1)); //15
    tmpVal &= (~(1 << BL_AHB_SLAVE1_SPI));   //16
    tmpVal &= (~(1 << BL_AHB_SLAVE1_I2C));   //17
    tmpVal &= (~(1 << BL_AHB_SLAVE1_PWM));
    tmpVal &= (~(1 << BL_AHB_SLAVE1_IRR));
    tmpVal &= (~(1 << BL_AHB_SLAVE1_CKS));
    tmpVal &= (~(1 << BL_AHB_SLAVE1_QDEC));
    tmpVal &= (~(1 << BL_AHB_SLAVE1_KYS));
    tmpVal &= (~(1 << BL_AHB_SLAVE1_I2S));
    tmpVal &= (~(1 << BL_AHB_SLAVE1_USB));
    tmpVal &= (~(1 << BL_AHB_SLAVE1_CAM));
    tmpVal &= (~(1 << BL_AHB_SLAVE1_MJPEG));

    BL_WR_REG(GLB_BASE, GLB_CGEN_CFG1, tmpVal);

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CGEN_CFG0);
    tmpVal &= (~(1 << 1)); //SDU clock gating
    tmpVal &= (~(1 << 2)); //SEC clock gating
    tmpVal &= (~(1 << 3)); //DMA clock gating
    tmpVal &= (~(1 << 4)); //CCI clock gating
    BL_WR_REG(GLB_BASE, GLB_CGEN_CFG0, tmpVal);
}

static BL_Err_Type ATTR_HBN_RAM_SECTION check_xtal_power_on(void)
{
    uint32_t tmpVal = 0;
    uint32_t timeOut = 0;

    tmpVal = BL_RD_REG(AON_BASE, AON_TSEN);

    /* Polling for ready */
    while ((!BL_IS_REG_BIT_SET(tmpVal, AON_XTAL_RDY)) && (timeOut < 120)) {
        RomDriver_BL702_Delay_US(10);
        timeOut++;
        tmpVal = BL_RD_REG(AON_BASE, AON_TSEN);
    }

    if (timeOut >= 120) {
        return TIMEOUT;
    }

    return SUCCESS;
}

// can be placed in flash, here placed in pds section to reduce fast boot time
static void ATTR_PDS_RAM_SECTION user_pds_recovery_board(void)
{
    uint32_t tmpVal;

    system_clock_gate();

    tmpVal = BL_RD_REG(GLB_BASE, GLB_CGEN_CFG2);
    tmpVal &= (~(1 << 0)); //ZIGBEE clock gating
    tmpVal &= (~(1 << 4)); //BLE clock gating
    BL_WR_REG(GLB_BASE, GLB_CGEN_CFG2, tmpVal);

#if XTAL_TYPE != INTERNAL_RC_32M
    check_xtal_power_on();
#endif
    RomDriver_HBN_Set_ROOT_CLK_Sel(HBN_ROOT_CLK_XTAL);
    SystemCoreClockSet(32000000);
}

void ATTR_PDS_RAM_SECTION user_pds_recovery_hardware(void)
{
    user_pds_recovery_board();
    user_pds_restore_tcm();

#if MASK_DEBUG_MSG == 0
    /* UART_IO recovery */
    GLB_GPIO_Cfg_Type gpio_cfg;

    gpio_cfg.drive = 0;
    gpio_cfg.smtCtrl = 1;
    gpio_cfg.gpioMode = GPIO_MODE_AF;
    gpio_cfg.pullType = GPIO_PULL_UP;
    gpio_cfg.gpioPin = GLB_GPIO_PIN_14;
    gpio_cfg.gpioFun = GPIO_FUN_UART;
    GLB_UART_Fun_Sel((GLB_GPIO_PIN_14 % 8), (GPIO_FUN_UART0_TX & 0x07));
    GLB_UART_Fun_Sel((GLB_GPIO_PIN_15 % 8), (GPIO_FUN_UART0_RX & 0x07));
    GLB_GPIO_Init(&gpio_cfg);
    gpio_cfg.gpioPin = GLB_GPIO_PIN_15;
    GLB_GPIO_Init(&gpio_cfg);
#endif
}

void enter_sleep(uint32_t pdsSleepCycles)
{
    uint32_t actualSleepDuration_ms;
    uint32_t mtimerClkCfg;
    uint32_t ulCurrentTimeHigh, ulCurrentTimeLow;
    volatile uint32_t *const pulTimeHigh = (volatile uint32_t *const)(configCLINT_BASE_ADDRESS + 0xBFFC);
    volatile uint32_t *const pulTimeLow = (volatile uint32_t *const)(configCLINT_BASE_ADDRESS + 0xBFF8);

    extern volatile uint64_t *const pullMachineTimerCompareRegister;
    extern const size_t uxTimerIncrementsForOneTick;
    extern void vPortSetupTimerInterrupt(void);

    mtimerClkCfg = *(volatile uint32_t *)0x40000090; // store mtimer clock

    *pullMachineTimerCompareRegister -= (uxTimerIncrementsForOneTick + 1); // avoid mtimer interrupt pending
    *(volatile uint8_t *)0x02800407 = 0;

    do {
        ulCurrentTimeHigh = *pulTimeHigh;
        ulCurrentTimeLow = *pulTimeLow;
    } while (ulCurrentTimeHigh != *pulTimeHigh);

    wfi_in = true;
    actualSleepDuration_ms = hal_pds_enter_with_time_compensation(PM_PDS_LEVEL_31, pdsSleepCycles);
    wfi_in = false;

    *(volatile uint32_t *)0x40000090 = mtimerClkCfg;

    *pulTimeHigh = ulCurrentTimeHigh;
    *pulTimeLow = ulCurrentTimeLow;

    vPortSetupTimerInterrupt();
    *(volatile uint8_t *)0x02800407 = 1;

    vTaskStepTick(actualSleepDuration_ms);
}

void vApplicationSleep(TickType_t xExpectedIdleTime_ms)
{
    int32_t bleSleepDuration_32768cycles = 0;
    int32_t expectedIdleTime_32768cycles = 0;
    eSleepModeStatus eSleepStatus;
    bool freertos_max_idle = false;

    if (pds_start == 0 || le_check_valid_scan())
        return;

    if (xExpectedIdleTime_ms + xTaskGetTickCount() == portMAX_DELAY) {
        freertos_max_idle = true;
    } else {
        xExpectedIdleTime_ms -= 1;
        expectedIdleTime_32768cycles = 32768 * xExpectedIdleTime_ms / 1000;
    }

    if ((!freertos_max_idle) && (expectedIdleTime_32768cycles < TIME_5MS_IN_32768CYCLE)) {
        return;
    }

    eSleepStatus = eTaskConfirmSleepModeStatus();
    if (eSleepStatus == eAbortSleep || ble_controller_sleep_is_ongoing()) {
        return;
    }
    bleSleepDuration_32768cycles = ble_controller_sleep();

    if (bleSleepDuration_32768cycles < TIME_5MS_IN_32768CYCLE) {
        return;
    } else {
        MSG("Sleep_cycles=%ld,state_cnt=%d\r\n", bleSleepDuration_32768cycles, rwip_get_sleep_stat_cnt());

        uint32_t reduceSleepTime;
        SPI_Flash_Cfg_Type *flashCfg;
        uint32_t len;
        extern BL_Err_Type flash_get_cfg(uint8_t * *cfg_addr, uint32_t * len);
        flash_get_cfg((uint8_t **)&flashCfg, &len);
        uint8_t ioMode = flashCfg->ioMode & 0xF;
        uint8_t contRead = flashCfg->cReadSupport;
        uint8_t cpuClk = GLB_Get_Root_CLK_Sel();
        if (ioMode == 4 && contRead == 1 && cpuClk == GLB_ROOT_CLK_XTAL) {
            reduceSleepTime = 100;
        } else if (ioMode == 1 && contRead == 0 && cpuClk == GLB_ROOT_CLK_XTAL) {
            reduceSleepTime = 130;
        } else {
            reduceSleepTime = 130;
        }

        if (eSleepStatus == eStandardSleep && ((!freertos_max_idle) && (expectedIdleTime_32768cycles < bleSleepDuration_32768cycles))) {
            enter_sleep(expectedIdleTime_32768cycles - reduceSleepTime);
        } else {
            enter_sleep(bleSleepDuration_32768cycles - reduceSleepTime);
        }

        bl_pds_restore();
    }
}

void bl_pds_restore(void)
{
#if MASK_DEBUG_MSG == 0
    struct device *uart = device_find("debug_log");
    if (uart) {
        device_close(uart);
        device_open(uart, DEVICE_OFLAG_STREAM_TX | DEVICE_OFLAG_INT_RX);
        device_set_callback(uart, NULL);
        device_control(uart, DEVICE_CTRL_CLR_INT, (void *)(UART_RX_FIFO_IT));
    }
#endif
    bl702_low_power_config();
    ble_controller_sleep_restore();
}

int ble_start_adv(void)
{
    struct bt_le_adv_param adv_param = {
        //options:3, connectable undirected, adv one time
        .options = 3,
        .interval_min = 0x280,
        .interval_max = 0x280,
    };

    char *adv_name = "BL_PDS_TEST_01"; // This name must be the same as adv_name in ble_central
    struct bt_data adv_data[] = {
        BT_DATA(BT_DATA_NAME_COMPLETE, adv_name, strlen(adv_name)),
    };

    return bt_le_adv_start(&adv_param, adv_data, ARRAY_SIZE(adv_data), NULL, 0);
}

#if defined(CONFIG_BT_OAD_SERVER)
bool app_check_oad(u32_t cur_file_ver, u32_t new_file_ver)
{
    //App layer decides whether to do oad according to file version
    /*if(new_file_ver > cur_file_ver)
        return true;
    else
        return false;*/
    return true;
}
#endif

void bt_enable_cb(int err)
{
    MSG("ble_tp_init\r\n");
    ble_tp_init();
#if defined(CONFIG_BT_OAD_SERVER)
    oad_service_enable(app_check_oad);
#endif
    MSG("Start adv\r\n");
    ble_start_adv();
    MSG("Advertising.........\r\n");
    bl702_low_power_config();
    pds_start = true;
}

void ble_stack_start(void)
{
    MSG("[OS] ble_controller_init...\r\n");
    GLB_Set_EM_Sel(GLB_EM_8KB);
    ble_controller_init(configMAX_PRIORITIES - 1);

    // Initialize BLE Host stack
    MSG("[OS] hci_driver_init...\r\n");
    hci_driver_init();

    MSG("[OS] bt_enable...\r\n");
    bt_enable(bt_enable_cb);
}

void ble_init(void)
{
    ble_stack_start();
}

static void ble_init_task(void *pvParameters)
{
    ble_init();
    vTaskDelete(NULL);
}

int main(void)
{
    static StaticTask_t ble_init_task_h;
    uint32_t tmpVal = 0;

    bflb_load_hbn_ram();
    bflb_platform_print_set(MASK_DEBUG_MSG);
    bflb_platform_init(0);
    HBN_Set_Ldo11_Rt_Vout(HBN_LDO_LEVEL_1P00V);
    HBN_Set_Ldo11_Soc_Vout(HBN_LDO_LEVEL_1P00V);
    user_pds_recovery_board();

    HBN_Clear_RTC_Counter();
    HBN_Enable_RTC_Counter();
    pm_set_hardware_recovery_callback(user_pds_recovery_hardware);

    //Set capcode
    tmpVal = BL_RD_REG(AON_BASE, AON_XTAL_CFG);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_XTAL_CAPCODE_IN_AON, 33);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_XTAL_CAPCODE_OUT_AON, 33);
    BL_WR_REG(AON_BASE, AON_XTAL_CFG, tmpVal);

    ble_controller_set_tx_pwr(0);

    vPortDefineHeapRegions(xHeapRegions);

    MSG("[OS] ble_init_task.....\r\n");
    xTaskCreate(ble_init_task, "ble_init_task", 512, NULL, 15, (TaskHandle_t *const) & ble_init_task_h);

    vTaskStartScheduler();

    BL_CASE_SUCCESS;
    while (1) {
        bflb_platform_delay_ms(100);
    }
}
