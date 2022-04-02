/*
 * Copyright (c) 2006-2021, RT-Thread Development Team
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Change Logs:
 * Date           Author       Notes
 */
#include "bflb_platform.h"
#include <rthw.h>
#include "board.h"
#include "hal_uart.h"
#include "ring_buffer.h"
#include "shell.h"

#define UART_RX_SIZE 256
uint8_t uart_rx_mem[UART_RX_SIZE];
Ring_Buffer_Type uart_rx_rb;
static struct rt_semaphore shell_rx_sem; /* 定义一个静态信号量 */

void shell_irq_callback(struct device *dev, void *args, uint32_t size, uint32_t state)
{
    Ring_Buffer_Write_Byte(&uart_rx_rb, *(uint8_t *)args);
    rt_sem_release(&shell_rx_sem);
}

void systick_isr()
{
    rt_interrupt_enter();
    rt_tick_increase();
    rt_interrupt_leave();
}

void rt_hw_console_output(const char *str)
{
    rt_size_t i = 0, size = 0;
    char a = '\r';

    struct device *uart = device_find("debug_log");

    size = rt_strlen(str);
    for (i = 0; i < size; i++) {
        if (*(str + i) == '\n') {
            device_write(uart, 0, (uint8_t *)&a, 1);
        }
        device_write(uart, 0, (uint8_t *)(str + i), 1);
    }
}

char rt_hw_console_getchar(void)
{
    char ch = 0;
    /* 从 ringbuffer 中拿出数据 */
    while (Ring_Buffer_Read_Byte(&uart_rx_rb, (rt_uint8_t *)&ch) != 1) {
        rt_sem_take(&shell_rx_sem, RT_WAITING_FOREVER);
    }
    return ch;
}

void rt_hw_board_init(void)
{
    bflb_platform_init(0);

    bflb_platform_set_alarm_time(RT_TICK_PER_SECOND, systick_isr);

#if defined(RT_USING_FINSH)
    struct device *uart = device_find("debug_log");
    if (uart) {
        Ring_Buffer_Init(&uart_rx_rb, uart_rx_mem, UART_RX_SIZE, NULL, NULL);
        device_set_callback(uart, shell_irq_callback);
        device_control(uart, DEVICE_CTRL_SET_INT, (void *)(UART_RX_FIFO_IT));
    }
#endif
#if defined(RT_USING_HEAP)
    rt_system_heap_init((void *)HEAP_BEGIN, (void *)HEAP_END);
#endif

#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif

#ifdef RT_USING_DEVICE
    rt_console_set_device(RT_CONSOLE_DEVICE_NAME);
#endif

    return;
}

int uart_rx_sem_init(void)
{
    rt_sem_init(&(shell_rx_sem), "shell_rx", 0, 0);
    return 0;
}
INIT_BOARD_EXPORT(uart_rx_sem_init);
INIT_APP_EXPORT(finsh_system_init);
