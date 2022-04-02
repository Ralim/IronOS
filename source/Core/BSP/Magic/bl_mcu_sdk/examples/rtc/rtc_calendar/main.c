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
#include "hal_rtc.h"
#include "timestamp.h"

int main(void)
{
    rtc_time beijing_time;

    bflb_platform_init(0);
    rtc_init(0);                   //set 0 means comparing none
    rtc_set_timestamp(1628074512); //1628074512 :2021-8-4 18:55:12
    while (1) {
        bflb_platform_delay_ms(1000);
        unixtime2bejingtime((uint32_t)rtc_get_timestamp(), &beijing_time);
        MSG("%d/%02d/%02d-%02d:%02d:%02d convert is: %u\r\n",
            beijing_time.year, beijing_time.month, beijing_time.day,
            beijing_time.hour, beijing_time.minute, beijing_time.second, (uint32_t)rtc_get_timestamp());
    }
}
