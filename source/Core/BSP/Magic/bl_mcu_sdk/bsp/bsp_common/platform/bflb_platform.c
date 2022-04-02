/**
 * @file bflb_platform.c
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
#include "hal_flash.h"
#include "hal_mtimer.h"
#include "drv_mmheap.h"
#include "hal_common.h"
#include "ring_buffer.h"
#include "bflb_platform.h"

extern uint32_t __HeapBase;
extern uint32_t __HeapLimit;

static uint8_t uart_dbg_disable = 0;

struct heap_info mmheap_root;

static struct heap_region system_mmheap[] = {
    { NULL, 0 },
    { NULL, 0 }, /* Terminates the array. */
};

__WEAK__ void board_init(void)
{
}
__WEAK__ void bl_show_info(void)
{
}

__WEAK__ enum uart_index_type board_get_debug_uart_index(void)
{
    return 0;
}

void bl_show_flashinfo(void)
{
    SPI_Flash_Cfg_Type flashCfg;
    uint8_t *pFlashCfg = NULL;
    uint32_t flashCfgLen = 0;
    uint32_t flashJedecId = 0;

    flashJedecId = flash_get_jedecid();
    flash_get_cfg(&pFlashCfg, &flashCfgLen);
    arch_memcpy((void *)&flashCfg, pFlashCfg, flashCfgLen);
    MSG("show flash cfg:\r\n");
    MSG("jedec id   0x%06X\r\n", flashJedecId);
    MSG("mid            0x%02X\r\n", flashCfg.mid);
    MSG("iomode         0x%02X\r\n", flashCfg.ioMode);
    MSG("clk delay      0x%02X\r\n", flashCfg.clkDelay);
    MSG("clk invert     0x%02X\r\n", flashCfg.clkInvert);
    MSG("read reg cmd0  0x%02X\r\n", flashCfg.readRegCmd[0]);
    MSG("read reg cmd1  0x%02X\r\n", flashCfg.readRegCmd[1]);
    MSG("write reg cmd0 0x%02X\r\n", flashCfg.writeRegCmd[0]);
    MSG("write reg cmd1 0x%02X\r\n", flashCfg.writeRegCmd[1]);
    MSG("qe write len   0x%02X\r\n", flashCfg.qeWriteRegLen);
    MSG("cread support  0x%02X\r\n", flashCfg.cReadSupport);
    MSG("cread code     0x%02X\r\n", flashCfg.cReadMode);
    MSG("burst wrap cmd 0x%02X\r\n", flashCfg.burstWrapCmd);
    MSG("-------------------\r\n");
}

void bflb_platform_init(uint32_t baudrate)
{
    static uint8_t initialized = 0;
    BL_Err_Type ret = ERROR;

    cpu_global_irq_disable();

    ret = flash_init();

    board_init();

    if (!uart_dbg_disable) {
        uart_register(board_get_debug_uart_index(), "debug_log");
        struct device *uart = device_find("debug_log");

        if (uart) {
            device_open(uart, DEVICE_OFLAG_STREAM_TX | DEVICE_OFLAG_INT_RX);
            device_set_callback(uart, NULL);
            device_control(uart, DEVICE_CTRL_CLR_INT, (void *)(UART_RX_FIFO_IT));
        }

        bl_show_info();
    }

    if (!initialized) {
        system_mmheap[0].addr = (uint8_t *)&__HeapBase;
        system_mmheap[0].mem_size = ((size_t)&__HeapLimit - (size_t)&__HeapBase);

        if (system_mmheap[0].mem_size > 0) {
            mmheap_init(&mmheap_root, system_mmheap);
        }

        MSG("dynamic memory init success,heap size = %d Kbyte \r\n", system_mmheap[0].mem_size / 1024);
        initialized = 1;
        if (ret != SUCCESS) {
            MSG("flash init fail!!!\r\n");
        }
        bl_show_flashinfo();
    }

    cpu_global_irq_enable();
}

#if ((defined BOOTROM) || (defined BFLB_EFLASH_LOADER))
static uint8_t eflash_loader_logbuf[1024] __attribute__((section(".system_ram_noinit")));
static uint32_t log_len = 0;
uint32_t bflb_platform_get_log(uint8_t *data, uint32_t maxlen)
{
    uint32_t len = log_len;
    if (len > maxlen) {
        len = maxlen;
    }
    memcpy(data, eflash_loader_logbuf, len);
    return len;
}
#endif

void bflb_platform_printf(char *fmt, ...)
{
    struct device *uart;
    char print_buf[128];
    va_list ap;

    if (!uart_dbg_disable) {
        va_start(ap, fmt);
        vsnprintf(print_buf, sizeof(print_buf) - 1, fmt, ap);
        va_end(ap);
#if ((defined BOOTROM) || (defined BFLB_EFLASH_LOADER))
        uint32_t len = strlen(print_buf);
        if (log_len + len < sizeof(eflash_loader_logbuf)) {
            memcpy(eflash_loader_logbuf + log_len, print_buf, len);
            log_len += len;
        }
#endif
        uart = device_find("debug_log");
        device_write(uart, 0, (uint8_t *)print_buf, strlen(print_buf));
    }
}

void bflb_platform_print_set(uint8_t disable)
{
    uart_dbg_disable = disable;
}

uint8_t bflb_platform_print_get(void)
{
    return uart_dbg_disable;
}

void bflb_platform_deinit(void)
{
    if (!uart_dbg_disable) {
        struct device *uart = device_find("debug_log");
        if (uart) {
            device_close(uart);
            device_unregister("debug_log");
        }
    }
}

void bflb_platform_dump(uint8_t *data, uint32_t len)
{
    uint32_t i = 0;

    if (!uart_dbg_disable) {
        for (i = 0; i < len; i++) {
            if (i % 16 == 0) {
                bflb_platform_printf("\r\n");
            }

            bflb_platform_printf("%02x ", data[i]);
        }

        bflb_platform_printf("\r\n");
    }
}

void bflb_platform_reg_dump(uint32_t addr)
{
    bflb_platform_printf("%08x[31:0]=%08x\r\n", addr, *(volatile uint32_t *)(addr));
}

void bflb_platform_init_time()
{
}

void bflb_platform_deinit_time()
{
}

void bflb_platform_set_alarm_time(uint64_t time, void (*interruptFun)(void))
{
    mtimer_set_alarm_time(time, interruptFun);
}

void bflb_platform_clear_time()
{
}

void bflb_platform_start_time()
{
}

void bflb_platform_stop_time()
{
}

uint64_t bflb_platform_get_time_ms()
{
    return mtimer_get_time_ms();
}

uint64_t bflb_platform_get_time_us()
{
    return mtimer_get_time_us();
}

void bflb_platform_delay_ms(uint32_t ms)
{
    mtimer_delay_ms(ms);
}
void bflb_platform_delay_us(uint32_t us)
{
    mtimer_delay_us(us);
}

void bflb_print_device_list(void)
{
    struct device *dev;
    dlist_t *node;
    uint8_t device_index = 0;

    MSG("Device List Print\r\n");

    dlist_for_each(node, device_get_list_header())
    {
        dev = dlist_entry(node, struct device, list);

        MSG("Index %d\r\nDevice Name = %s \r\n", device_index, dev->name);

        switch (dev->type) {
            case DEVICE_CLASS_GPIO:
                MSG("Device Type = %s \r\n", "GPIO");
                break;

            case DEVICE_CLASS_UART:
                MSG("Device Type = %s \r\n", "UART");
                break;

            case DEVICE_CLASS_SPI:
                MSG("Device Type = %s \r\n", "SPI");
                break;

            case DEVICE_CLASS_I2C:
                MSG("Device Type = %s \r\n", "I2C");
                break;

            case DEVICE_CLASS_ADC:
                MSG("Device Type = %s \r\n", "ADC");
                break;

            case DEVICE_CLASS_DMA:
                MSG("Device Type = %s \r\n", "DMA");
                break;

            case DEVICE_CLASS_TIMER:
                MSG("Device Type = %s \r\n", "TIMER");
                break;

            case DEVICE_CLASS_PWM:
                MSG("Device Type = %s \r\n", "PWM");
                break;

            case DEVICE_CLASS_SDIO:
                MSG("Device Type = %s \r\n", "SDIO");
                break;

            case DEVICE_CLASS_USB:
                MSG("Device Type = %s \r\n", "USB");
                break;

            case DEVICE_CLASS_I2S:
                MSG("Device Type = %s \r\n", "I2S");
                break;

            case DEVICE_CLASS_CAMERA:
                MSG("Device Type = %s \r\n", "CAMERA");
                break;

            case DEVICE_CLASS_NONE:
                break;

            default:
                break;
        }

        MSG("Device Handle = 0x%x \r\n", dev);
        MSG("---------------------\r\n", dev);

        device_index++;
    }
}
