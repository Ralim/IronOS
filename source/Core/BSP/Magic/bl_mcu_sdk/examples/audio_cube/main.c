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
#include "hal_i2s.h"
#include "hal_dma.h"

#include "data_protocol.h"
#include "wav_play_from_isp.h"

audio_dev_t audio_test;

isp_obj_t isp_obj_uart;

struct device *uart_isp;

uint8_t flag = 0;

/**
 * @brief isp 事件回调，一般运行在中断里，不要阻塞操作
 *
 * @param isp_obj
 * @param isp_callback_reason
 */
void isp_uart_callback(isp_obj_t *isp_obj, isp_callback_reason_t isp_callback_reason)
{
    int vol;

    switch (isp_callback_reason) {
        case ISP_CALLBACK_RECEIVE_ACK_WAIT:
            /* receive data information */
            if (isp_obj->cmd_id == 0x50) {
                /* receive data information */
                if (isp_obj->file_type == 0x59 && audio_test.audio_state)
                    audio_test.audio_state = 1;

                isp_obj->auot_ack = 0; /* auto ack */

            } else if (isp_obj->cmd_id == 0x51) {
                /* updata buff */
                if (audio_test.audio_state == 0) { /* Not allowed to play */
                    isp_obj->auot_ack = 0;
                } else if (audio_test.audio_state < 4) { /* start play */
                    audio_test.buff_using = (isp_obj->file_data == audio_test.buff[0]) ? 1 : 0;
                    audio_test.buff_data_size[!audio_test.buff_using] = isp_obj->length;

                    // audio_test.audio_control(&audio_test, AUDIO_CMD_PLAY_START, NULL);
                    if (audio_test.audio_state == 1) {
                        if (audio_test.audio_init(&audio_test, 0, isp_obj->file_data)) {
                            break;
                        }
                    }

                    audio_test.audio_control(&audio_test, AUDIO_CMD_PLAY_START, NULL);

                    isp_obj->file_data = audio_test.buff[!audio_test.buff_using];
                    audio_test.buff_data_size[!audio_test.buff_using] = 0;
                    isp_obj->auot_ack = 0;
                } else if (audio_test.audio_state == 4) { /* play ing */

                    if (isp_obj->file_data == audio_test.buff[0])
                        audio_test.buff_data_size[0] = isp_obj->length;
                    else
                        audio_test.buff_data_size[1] = isp_obj->length;
                    /* non auto ack , audio callback */
                    isp_obj->auot_ack = 1;
                }
            }

            // MSG("ISP_CALLBACK_RECEIVE_ACK_WAIT\r\n");
            break;
        case ISP_CALLBACK_RECEIVE_NACK_WAIT:
            /* receive data err */
            // MSG("ISP_CALLBACK_RECEIVE_NACK_WAIT\r\n");
            isp_obj->auot_ack = 0; /* auto ack */
            break;
        case ISP_CALLBACK_SEND_ACK_WAIT:
            /* receive data end,wait ack */
            // MSG("ISP_CALLBACK_SEND_ACK_WAIT\r\n");
            break;
        case ISP_CALLBACK_SEND_SUCCES_ACK:
            /* send ok,send file next data */

            if (audio_test.audio_state == 11) {
                /* Continue to the recording */
                audio_test.audio_control(&audio_test, AUDIO_CMD_RECORD_START, NULL);

                isp_obj_uart.file_data = audio_test.buff[!audio_test.buff_using];
                isp_obj_uart.length = audio_test.buff_data_size[!audio_test.buff_using];
                isp_uart_send_data(&isp_obj_uart);
                audio_test.buff_data_size[!audio_test.buff_using] = 0;
                audio_test.audio_state = 12;
            }
            // MSG("ISP_CALLBACK_SEND_SUCCES_ACK\r\n");
            break;
        case ISP_CALLBACK_SEND_ERROR_ACK:
            /* send err,resend last data */

            // MSG("ISP_CALLBACK_SEND_ERROR_ACK\r\n");
            break;

        case ISP_CALLBACK_CMD_:
            // MSG("ISP_CALLBACK_CMD_\r\n");

            switch (isp_obj->cmd_id) {
                case CMD_PLAY_START: /* play */
                    if (audio_test.audio_state == 0)
                        audio_test.audio_state = 1;
                    isp_obj->status.isp_state_mode = NO_TASK;
                    isp_uart_send_ack(isp_obj, REPLY_SUCCES);
                    break;
                case CMD_PLAY_PAUSE: /* pause */
                    // if (audio_test.audio_state == 3)
                    //     audio_test.audio_state = 2;
                    isp_obj->status.isp_state_mode = NO_TASK;
                    isp_uart_send_ack(isp_obj, REPLY_SUCCES);
                    break;
                case CMD_PLAY_STOP: /* stop */
                    if (audio_test.audio_state)
                        audio_test.audio_state = 0;
                    // device_close(i2s);
                    isp_obj->status.isp_state_mode = NO_TASK;
                    isp_uart_send_ack(isp_obj, REPLY_SUCCES);
                    break;
                case CMD_VOLUME_INCREASE: /* volume +1 */
                    vol = audio_test.audio_control(&audio_test, AUDIO_CMD_GET_VOLUME, NULL);
                    if (vol >= 100)
                        vol = 99;
                    audio_test.audio_control(&audio_test, AUDIO_CMD_SET_VOLUME, (void *)(vol + 1));
                    isp_obj->status.isp_state_mode = NO_TASK;
                    isp_uart_send_ack(isp_obj, REPLY_SUCCES);
                    break;
                case CMD_VOLUME_DECREASE: /* volume -1 */
                    vol = audio_test.audio_control(&audio_test, AUDIO_CMD_GET_VOLUME, NULL);
                    if (vol <= 0)
                        vol = 1;
                    audio_test.audio_control(&audio_test, AUDIO_CMD_SET_VOLUME, (void *)(vol - 1));
                    isp_obj->status.isp_state_mode = NO_TASK;
                    isp_uart_send_ack(isp_obj, REPLY_SUCCES);
                    break;
                case CMD_VOLUME_SET: /* volume set */
                    vol = isp_obj->file_type;
                    if (vol <= 0)
                        vol = 0;
                    else if (vol >= 100)
                        vol = 100;
                    audio_test.audio_control(&audio_test, AUDIO_CMD_SET_VOLUME, (void *)vol);
                    isp_obj->status.isp_state_mode = NO_TASK;
                    isp_uart_send_ack(isp_obj, REPLY_SUCCES);
                    break;
                case CMD_VOLUME_GET: /* volume get */
                    vol = audio_test.audio_control(&audio_test, AUDIO_CMD_GET_VOLUME, NULL);
                    isp_obj->status.isp_state_mode = NO_TASK;
                    isp_uart_send_ack(isp_obj, (vol << 8) & 0xFF00);
                    break;
                case CMD_SAMPLING_RATE: /* sampling_rate */
                    audio_test.record_config->sampl_freq = isp_obj->file_type;
                    isp_obj->status.isp_state_mode = NO_TASK;
                    isp_uart_send_ack(isp_obj, REPLY_SUCCES);
                    break;
                case CMD_CHANNEL_NUM: /* channel */
                    audio_test.record_config->channel_num = isp_obj->file_type;
                    isp_obj->status.isp_state_mode = NO_TASK;
                    isp_uart_send_ack(isp_obj, REPLY_SUCCES);
                    break;
                case CMD_RECORD_START:                 /* record */
                    if (audio_test.audio_state == 4 || /* play ing */
                        audio_test.audio_init(&audio_test, 1, isp_obj->file_data)) {
                        isp_obj->status.isp_state_mode = NO_TASK;
                        isp_uart_send_ack(isp_obj, REPLY_ERROR);
                        break;
                    }

                    isp_obj->status.isp_state_mode = NO_TASK;
                    isp_uart_send_ack(isp_obj, REPLY_SUCCES);
                    audio_test.audio_state = 10;
                    audio_test.audio_control(&audio_test, AUDIO_CMD_RECORD_START, NULL);
                    break;
                case CMD_RECORD_STOP: /* stop record */
                    audio_test.audio_control(&audio_test, AUDIO_CMD_PLAY_STOP, NULL);
                    audio_test.audio_state = 10;
                    isp_obj->status.isp_state_mode = NO_TASK;
                    isp_uart_send_ack(isp_obj, REPLY_SUCCES);
                    break;
                default:
                    isp_obj->status.isp_state_mode = NO_TASK;
                    isp_uart_send_ack(isp_obj, REPLY_SUCCES);
                    break;
            }
            break;

        default:
            MSG("unkown\r\n");
            break;
    }
}

int main(void)
{
    /* Disable log output */
    bflb_platform_print_set(1);

    bflb_platform_init(0);

    /* init audio */
    // audio_init(&audio_test);
    isp_wav_play_register(&audio_test);
    audio_test.audio_control(&audio_test, AUDIO_CMD_SET_VOLUME, (void *)40);

    /* init isp uart */
    isp_uart_init(&isp_obj_uart);
    isp_obj_uart.isp_callback = isp_uart_callback;
    isp_obj_uart.file_data = audio_test.buff[!audio_test.buff_using];

    while (1) {
        bflb_platform_delay_ms(100);
        /* code */
        if (isp_obj_uart.status.isp_state_mode == NO_TASK) {
            /* code */
        }
    }
}