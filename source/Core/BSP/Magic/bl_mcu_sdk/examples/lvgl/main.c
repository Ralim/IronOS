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
 * Note:
 *
 * This Case is currently based on BL706_AVB development board with ili9341 controller screen.
 * If you use other hardware platforms, please modify the pin Settings by yourself
 *
 * Your should Short connect func1 and func2 jump cap on 706_AVB Board,and Plug the screen into the HD13 interface
 * Then the hardware for lvgl case testing is available.
 *
 *
 */
#include "bflb_platform.h"
#include "hal_spi.h"
#include "hal_gpio.h"
#include "hal_dma.h"

#include "xpt2046.h"
#include "lv_port_disp.h"
#include "lv_port_indev.h"
#include "lvgl.h"
#include "lv_examples.h"

/* lvgl log cb */
void lv_log_print_g_cb(lv_log_level_t level, const char *path, uint32_t line, const char *name, const char *str)
{
    switch (level) {
        case LV_LOG_LEVEL_ERROR:
            MSG("ERROR: ");
            break;

        case LV_LOG_LEVEL_WARN:
            MSG("WARNING: ");
            break;

        case LV_LOG_LEVEL_INFO:
            MSG("INFO: ");
            break;

        case LV_LOG_LEVEL_TRACE:
            MSG("TRACE: ");
            break;

        default:
            break;
    }

    MSG("%s- ", name);
    MSG(" %s\r\n,", str);
}

int main(void)
{
    bflb_platform_init(0);

    lv_log_register_print_cb(lv_log_print_g_cb);

    lv_init();
    lv_port_disp_init();
    lv_port_indev_init();

    lv_demo_benchmark();

    BL_CASE_SUCCESS;

    while (1) {
        lv_task_handler();
    }
}
