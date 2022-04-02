/**
 * @file ips.h
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
//#include "drv_list.h"

typedef enum {
    FILE_START = 0x50,
    FILE_DATA = 0X51,
    FILE_END = 0X52,

    CMD_PLAY_START = 0X40,
    CMD_PLAY_PAUSE = 0X41,
    CMD_PLAY_STOP = 0X42,

    CMD_VOLUME_INCREASE = 0x43,
    CMD_VOLUME_DECREASE = 0x44,
    CMD_VOLUME_SET = 0x45,
    CMD_VOLUME_GET = 0x46,

    CMD_SAMPLING_RATE = 0x47,
    CMD_CHANNEL_NUM = 0X48,

    CMD_RECORD_START = 0x49,
    CMD_RECORD_STOP = 0x4A,
} isp_cmd_id_t;

typedef enum {
    DATA_UVVV = 0X53,
    DATA_VUVV = 0X54,

} isp_file_type_t;

typedef enum {
    REPLY_SUCCES = 0x4B4Fu,
    REPLY_ERROR = 0x4552u,
} ips_reply_t;

/* 内部状态类型 */
typedef enum {
    NO_TASK = 0,
    SEND_DATA = 1,    /* 发数据中 */
    RECEIVE_DATA,     /* 接收数据中 */
    SEND_WAIT_ACK,    /* 发送后等待 ack */
    RECEIVE_WAIT_ACK, /* 接收后对方在等待 ack */
    OTHER_STATE,      /*其他状态，一般是接收数据或命令的帧头过程中 */
} isp_state_mode_t;

typedef struct {
    isp_state_mode_t isp_state_mode; /* 单帧状态 */
    uint8_t already_steps;           /* 状态机控制 */
    uint16_t buff_length_max;        /* buff 最大长度 */
    ips_reply_t ips_reply;           /* ack 状态 */
    uint16_t receive_length;         /* 已经接收数量 */
} isp_status_t;

/* 回调触发原因 */
typedef enum {
    ISP_CALLBACK_SEND_SUCCES_ACK,   /* 发送后收到正确的ACK */
    ISP_CALLBACK_SEND_ERROR_ACK,    /* 发送后收到错误的ACK */
    ISP_CALLBACK_RECEIVE_ACK_WAIT,  /* 数据已经收到,校验正确，对方在等待ACK */
    ISP_CALLBACK_RECEIVE_NACK_WAIT, /* 数据已经收到,校验错误，对方在等待NACK */
    ISP_CALLBACK_SEND_ACK_WAIT,     /* 数据已经发出，等待对面ACK */
    ISP_CALLBACK_CMD_,              /* 收到命令,对方在等待ACK */
} isp_callback_reason_t;

typedef struct __attribute__((packed)) isp_obj_struct {
    isp_cmd_id_t cmd_id;
    uint8_t check;
    uint16_t length;
    isp_file_type_t file_type;

    uint8_t *file_data;
    uint8_t auot_ack; /* 数据包自动回ack，0自动，非零不自动*/
    uint32_t time_out;
    isp_status_t status; /* 中间变量，用以状态机控制 */
    void (*isp_callback)(struct isp_obj_struct *isp_obj, isp_callback_reason_t isp_callback_reason);
} isp_obj_t;

int isp_uart_send_data(isp_obj_t *isp_obj_uart);

uint8_t isp_uart_init(isp_obj_t *isp_obj);

int isp_uart_send_ack(isp_obj_t *isp_obj_uart, ips_reply_t ips_reply);
