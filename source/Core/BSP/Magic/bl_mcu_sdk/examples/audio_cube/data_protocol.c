/**
 * @file ips.c
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

#include "drv_mmheap.h"
#include "hal_uart.h"
#include "hal_dma.h"
#include "data_protocol.h"

struct device *uart;
static isp_obj_t *isp_obj_uart;

void ips_obj_init(isp_obj_t *isp_obj_p)
{
    if (!isp_obj_p)
        return;
    isp_obj_p->cmd_id = 0;
    isp_obj_p->check = 0;
    isp_obj_p->length = 0;
    isp_obj_p->file_type = 0;
    isp_obj_p->auot_ack = 1;
    isp_obj_p->status.isp_state_mode = 0;
    isp_obj_p->status.already_steps = 0;
}

/**
 * @brief uart tx dma irq, ch0
 *
 * @param dev
 * @param args
 * @param size
 * @param state
 */
void dma_ch0_irq_callback(struct device *dev, void *args, uint32_t size, uint32_t state)
{
    if (isp_obj_uart == NULL) {
        return;
    }

    /* close dma and dma irq */
    device_control(uart, DEVICE_CTRL_TX_DMA_SUSPEND, NULL);
    device_control(UART_DEV(uart)->tx_dma, DEVICE_CTRL_CLR_INT, NULL);

    if (isp_obj_uart->status.isp_state_mode == SEND_DATA) {
        isp_obj_uart->status.isp_state_mode = SEND_WAIT_ACK;
        isp_obj_uart->isp_callback(isp_obj_uart, ISP_CALLBACK_SEND_ACK_WAIT);
    }
}

// void dma_ch1_irq_callback(struct device *dev, void *args, uint32_t size, uint32_t state)
// {
//     if (isp_obj_uart == NULL) {
//         return;
//     }

//     if (isp_obj_uart->status.isp_state_mode == RECEIVE_DATA) {
//         isp_obj_uart->status.isp_state_mode = RECEIVE_WAIT_ACK;
//         isp_obj_uart->isp_callback(isp_obj_uart, ISP_CALLBACK_RECEIVE_ACK_WAIT);
//     }
//     device_control(uart, DEVICE_CTRL_RX_DMA_SUSPEND, 0);
//     isp_obj_uart->status.already_steps = 0;
//     device_control(uart, DEVICE_CTRL_SET_INT, (void *)(UART_RX_FIFO_IT));

//     MSG("tx dma init\r\n");
// }

uint8_t isp_check(isp_obj_t *isp_obj)
{
    if (isp_obj == NULL) {
        return 0;
    }
    uint8_t check = isp_obj->cmd_id + (isp_obj->length & 0x00FF) + ((isp_obj->length >> 8) & 0x00FF);

    if (isp_obj->length && isp_obj->file_data) {
        for (uint16_t i = 0; i < isp_obj->length; i++) {
            check += isp_obj->file_data[i];
        }
    }
    return check;
}

/**
 * @brief
 *
 * @param isp_obj_uart
 * @return int
 */
int isp_uart_send_data(isp_obj_t *isp_obj_uart)
{
    if (isp_obj_uart == NULL || isp_obj_uart->cmd_id == 0) {
        return -1;
    }

    if ((isp_obj_uart->cmd_id & 0xF0) != 0x50)
        return -2;

    if (isp_obj_uart->status.isp_state_mode != NO_TASK)
        return -3;

    isp_obj_uart->check = isp_check(isp_obj_uart);

    /* use tx blocking */
    device_write(uart, 0, (void *)isp_obj_uart, sizeof(isp_cmd_id_t) + sizeof(uint8_t) + sizeof(uint16_t) + sizeof(isp_file_type_t));

    /* use DMA tx non-blocking */
    device_control(uart, DEVICE_CTRL_TX_DMA_RESUME, NULL);
    device_control(UART_DEV(uart)->tx_dma, DEVICE_CTRL_SET_INT, NULL);
    device_write(uart, 0, isp_obj_uart->file_data, isp_obj_uart->length);

    isp_obj_uart->status.already_steps = 0;
    isp_obj_uart->status.isp_state_mode = SEND_DATA;

    return 0;
}

/**
 * @brief
 *
 * @param isp_obj_uart
 * @return int
 */

int isp_uart_send_ack(isp_obj_t *isp_obj_uart, ips_reply_t ips_reply)
{
    if (isp_obj_uart == NULL || isp_obj_uart->cmd_id == 0) {
        return -1;
    }

    // if (isp_obj_uart->status.isp_state_mode != RECEIVE_WAIT_ACK) {
    //     return -2;
    // }

    /* use DMA tx blocking */
    device_write(uart, 0, (uint8_t *)&ips_reply, 2);

    return 0;
}

/**
 * @brief
 *
 * @param isp_obj
 * @param buff
 * @param size
 */
void isp_task_handler(isp_obj_t *isp_obj, uint8_t *buff, uint32_t size)
{
    static uint32_t time_last = 0;

    uint32_t time_now = bflb_platform_get_time_ms();

    /* debug information  */
    // MSG("hand:%d, len:%d, mode:%d\r\n", isp_obj_uart->status.already_steps, isp_obj_uart->status.receive_length, isp_obj_uart->status.isp_state_mode);

    if (size == 0) {
        if ((time_now - time_last > isp_obj->time_out) && isp_obj->time_out) {
            /* time out */
        }
    }

    for (uint32_t i = 0; i < size; i++) {
        /* receive 状态机 */
        switch (isp_obj_uart->status.already_steps) {
            case 0:
                if (isp_obj_uart->status.isp_state_mode == NO_TASK) {
                    isp_obj_uart->status.isp_state_mode = OTHER_STATE;
                } else if (isp_obj_uart->status.isp_state_mode != SEND_WAIT_ACK) {
                    return;
                }

                isp_obj_uart->cmd_id = *buff;
                isp_obj_uart->status.already_steps++;

                break;
            case 1:
                isp_obj_uart->check = *buff;
                isp_obj_uart->status.already_steps++;

                if (isp_obj_uart->status.isp_state_mode == SEND_WAIT_ACK) {
                    isp_obj_uart->status.isp_state_mode = NO_TASK;
                    if (isp_obj_uart->cmd_id == 0X4F && isp_obj_uart->check == 0X4B) {
                        isp_obj_uart->status.ips_reply = REPLY_SUCCES;
                        isp_obj_uart->isp_callback(isp_obj_uart, ISP_CALLBACK_SEND_SUCCES_ACK);
                        isp_obj_uart->status.isp_state_mode = NO_TASK;
                        isp_obj_uart->status.already_steps = 0;
                    } else if (isp_obj_uart->cmd_id == 0X52 && isp_obj_uart->check == 0X45) {
                        isp_obj_uart->status.ips_reply = REPLY_ERROR;
                        isp_obj_uart->isp_callback(isp_obj_uart, ISP_CALLBACK_SEND_ERROR_ACK);
                        isp_obj_uart->status.isp_state_mode = NO_TASK;
                        isp_obj_uart->status.already_steps = 0;
                    }
                }
                break;
            case 2:
                isp_obj_uart->length = *buff;
                isp_obj_uart->status.already_steps++;
                break;
            case 3:
                isp_obj_uart->length += *buff << 8;
                isp_obj_uart->status.already_steps++;
                break;
            case 4: /* 接收的如果是命令，无法判断命令是简短命令(只需回复2个字节信息)，还是复杂命令(需要回复一段数据包)，需要自己在用户回调函数里处理，但这似乎不太合理 */
                isp_obj_uart->file_type = *buff;
                isp_obj_uart->status.already_steps++;
                if ((isp_obj_uart->cmd_id & 0xF0) == 0x50) {
                    isp_obj_uart->status.isp_state_mode = RECEIVE_DATA;
                    isp_obj_uart->status.receive_length = 0;
                } else {
                    isp_obj_uart->status.already_steps = 0;
                    /* 等待ACK的时候收到命令，同时代表 命令 与 成功ACK */
                    if (isp_obj_uart->status.isp_state_mode == SEND_WAIT_ACK) {
                        /* 响应 CMD 含义 */
                        isp_obj_uart->status.isp_state_mode = RECEIVE_WAIT_ACK;
                        isp_obj_uart->isp_callback(isp_obj_uart, ISP_CALLBACK_CMD_);
                        /* 响应 ACK 含义 */
                        isp_obj_uart->status.ips_reply = REPLY_SUCCES;
                        isp_obj_uart->isp_callback(isp_obj_uart, ISP_CALLBACK_SEND_SUCCES_ACK);
                        isp_obj_uart->status.isp_state_mode = NO_TASK;
                    } else {
                        /* 正常空闲收到命令 */
                        isp_obj_uart->status.isp_state_mode = RECEIVE_WAIT_ACK;
                        isp_obj_uart->isp_callback(isp_obj_uart, ISP_CALLBACK_CMD_);
                    }
                    //isp_obj_uart->status.isp_state_mode = NO_TASK;
                }

                break;
            case 5:
                if (isp_obj_uart->status.receive_length < isp_obj_uart->length) {
                    isp_obj_uart->file_data[isp_obj_uart->status.receive_length] = *buff;
                    isp_obj_uart->status.receive_length++;
                }
                if (isp_obj_uart->status.receive_length >= isp_obj_uart->length) {
                    isp_obj_uart->status.isp_state_mode = RECEIVE_WAIT_ACK;
                    uint8_t check_data = isp_check(isp_obj_uart);
                    //MSG("check:%X\r\n", check);
                    if (isp_obj_uart->check == check_data) {
                        isp_obj_uart->isp_callback(isp_obj_uart, ISP_CALLBACK_RECEIVE_ACK_WAIT);
                        /* auot send ack */
                        if (isp_obj_uart->auot_ack == 0) {
                            isp_uart_send_ack(isp_obj_uart, REPLY_SUCCES);
                            isp_obj_uart->status.isp_state_mode = NO_TASK;
                        }
                    } else {
                        isp_obj_uart->isp_callback(isp_obj_uart, ISP_CALLBACK_RECEIVE_NACK_WAIT);
                        /*  auot send Nack */
                        if (isp_obj_uart->auot_ack == 0) {
                            isp_uart_send_ack(isp_obj_uart, REPLY_ERROR);
                            isp_obj_uart->status.isp_state_mode = NO_TASK;
                        }
                    }

                    isp_obj_uart->status.receive_length = 0;
                    isp_obj_uart->status.already_steps = 0;
                }
                break;
            default:
                break;
        }
        buff++;
    }

    time_last = time_now;
}

/**
 * @brief uart_irq_callback
 *
 * @param dev
 * @param args
 * @param size
 * @param state
 */
void uart_irq_callback(struct device *dev, void *args, uint32_t size, uint32_t state)
{
    static uint8_t fer_err_flag = 0;
    if (state == UART_EVENT_RX_FIFO || state == UART_EVENT_RTO) {
        if (isp_obj_uart == NULL) {
            return;
        }

        /* Tx fifo overflow, send nack*/
        if(fer_err_flag){
            if(state == UART_EVENT_RX_FIFO){

            }else if(state == UART_EVENT_RTO){
                isp_obj_uart->status.isp_state_mode = NO_TASK;
                isp_uart_send_ack(isp_obj_uart, REPLY_ERROR);
                fer_err_flag = 0;
            }
            return;
        }

        isp_task_handler(isp_obj_uart, args, size);
    }
    else if(state == UART_EVENT_RX_FER){
        /* Tx fifo overflow, baud rate should be reduced */
        fer_err_flag = 1;
        device_control(dev,DEVICE_CTRL_UART_CLEAR_RX_FIFO,NULL);
    }
}

/**
 * @brief
 *
 */
uint8_t isp_uart_init(isp_obj_t *isp_obj)
{
    if (isp_obj == NULL)
        return 1;

    uart = device_find("debug_log");
    if(uart){
        device_close(uart);
    }else{
        uart_register(UART0_INDEX, "debug_log");
        uart = device_find("debug_log");
    }

    if (uart) {
        device_close(uart);
        UART_DEV(uart)->id = 0;
        UART_DEV(uart)->baudrate = 3000000;
        UART_DEV(uart)->databits = UART_DATA_LEN_8;
        UART_DEV(uart)->stopbits = UART_STOP_ONE;
        UART_DEV(uart)->parity = UART_PAR_NONE;
        UART_DEV(uart)->fifo_threshold = 4;

        device_close(uart);
        device_open(uart, DEVICE_OFLAG_DMA_TX | DEVICE_OFLAG_INT_RX);
        bflb_platform_delay_ms(5);
        device_set_callback(uart, uart_irq_callback);
        device_control(uart, DEVICE_CTRL_SET_INT, (void *)(UART_RX_FIFO_IT | UART_RTO_IT));
    }

    dma_register(DMA0_CH0_INDEX, "ch0");
    struct device *dma_ch0 = device_find("ch0");

    if (dma_ch0) {
        DMA_DEV(dma_ch0)->direction = DMA_MEMORY_TO_PERIPH;
        DMA_DEV(dma_ch0)->transfer_mode = DMA_LLI_ONCE_MODE;
        DMA_DEV(dma_ch0)->src_req = DMA_REQUEST_NONE;
        DMA_DEV(dma_ch0)->dst_req = DMA_REQUEST_UART0_TX;
        DMA_DEV(dma_ch0)->src_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
        DMA_DEV(dma_ch0)->dst_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
        DMA_DEV(dma_ch0)->src_burst_size = DMA_BURST_1BYTE;
        DMA_DEV(dma_ch0)->dst_burst_size = DMA_BURST_1BYTE;
        DMA_DEV(dma_ch0)->src_width = DMA_TRANSFER_WIDTH_8BIT;
        DMA_DEV(dma_ch0)->dst_width = DMA_TRANSFER_WIDTH_8BIT;
        device_open(dma_ch0, 0);

        device_control(uart, DEVICE_CTRL_ATTACH_TX_DMA, dma_ch0);
        device_set_callback(dma_ch0, dma_ch0_irq_callback);
        /* close dma irq */
        //device_control(UART_DEV(uart)->tx_dma, DEVICE_CTRL_CLR_INT, NULL);
    }

    // dma_register(DMA0_CH1_INDEX, "ch1");
    // struct device *dma_ch1 = device_find("ch1");

    // if (dma_ch1) {
    //     DMA_DEV(dma_ch1)->direction = DMA_PERIPH_TO_MEMORY;
    //     DMA_DEV(dma_ch1)->transfer_mode = DMA_LLI_ONCE_MODE;
    //     DMA_DEV(dma_ch1)->src_req = DMA_REQUEST_UART0_RX;
    //     DMA_DEV(dma_ch1)->dst_req = DMA_REQUEST_NONE;
    //     DMA_DEV(dma_ch1)->src_addr_inc = DMA_ADDR_INCREMENT_DISABLE;
    //     DMA_DEV(dma_ch1)->dst_addr_inc = DMA_ADDR_INCREMENT_ENABLE;
    //     DMA_DEV(dma_ch1)->src_burst_size = DMA_BURST_1BYTE;
    //     DMA_DEV(dma_ch1)->dst_burst_size = DMA_BURST_1BYTE;
    //     DMA_DEV(dma_ch1)->src_width = DMA_TRANSFER_WIDTH_8BIT;
    //     DMA_DEV(dma_ch1)->dst_width = DMA_TRANSFER_WIDTH_8BIT;
    //     device_open(dma_ch1, 0);

    //     device_control(uart, DEVICE_CTRL_ATTACH_RX_DMA, dma_ch1);
    //     device_set_callback(dma_ch1, dma_ch1_irq_callback);
    //     device_control(dma_ch1, DEVICE_CTRL_SET_INT, NULL);
    // }

    device_control(uart, DEVICE_CTRL_TX_DMA_SUSPEND, NULL);

    isp_obj_uart = isp_obj;

    return 0;
}
