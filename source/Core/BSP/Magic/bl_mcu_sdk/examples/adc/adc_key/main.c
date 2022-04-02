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
#include "hal_adc.h"
#include "hal_gpio.h"

#define KEY0_ADC_VALUE     (0)
#define KEY1_ADC_VALUE     (100)
#define KEY2_ADC_VALUE     (200)
#define KEY3_ADC_VALUE     (300)
#define KEY4_ADC_VALUE     (430)
#define KEY_NO_VALUE       (3000)
#define KEY_MEASURE_OFFSET (30)

#define KEY_FILTER_NUM (200)
typedef struct
{
    uint32_t vaildKeyNum;
    uint16_t keyValue[KEY_FILTER_NUM];
} adc_keys_status;

adc_channel_t posChList[] = { ADC_CHANNEL8 };
adc_channel_t negChList[] = { ADC_CHANNEL_GND };
adc_channel_val_t result_val;

adc_keys_status key_machine;

struct device *adc_key;

uint32_t adc_value[2] = { 0 };
uint32_t key_voltage = 0;

/**
 * @brief init adc key machine
 *
 */
static void key_machine_init(void)
{
    key_machine.vaildKeyNum = 0;
    memset(key_machine.keyValue, 0xff, KEY_FILTER_NUM * 2);
}

/**
 * @brief Get the adc value range object
 *
 * @param Vmv input adc value ,unit is mV
 * @return uint16_t
 */
static uint16_t get_adc_value_range(uint32_t Vmv)
{
    if (Vmv > KEY4_ADC_VALUE - KEY_MEASURE_OFFSET && Vmv < KEY4_ADC_VALUE + KEY_MEASURE_OFFSET) {
        return KEY4_ADC_VALUE;
    } else if (Vmv > KEY3_ADC_VALUE - KEY_MEASURE_OFFSET && Vmv < KEY3_ADC_VALUE + KEY_MEASURE_OFFSET) {
        return KEY3_ADC_VALUE;
    } else if (Vmv > KEY2_ADC_VALUE - KEY_MEASURE_OFFSET && Vmv < KEY2_ADC_VALUE + KEY_MEASURE_OFFSET) {
        return KEY2_ADC_VALUE;
    } else if (Vmv > KEY1_ADC_VALUE - KEY_MEASURE_OFFSET && Vmv < KEY1_ADC_VALUE + KEY_MEASURE_OFFSET) {
        return KEY1_ADC_VALUE;
    } else if (Vmv < KEY_MEASURE_OFFSET) {
        return KEY0_ADC_VALUE;
    } else {
        return KEY_NO_VALUE;
    }
}

/**
 * @brief Get the adc key value object
 *
 * @param Vmv input adc value ,unit is mV
 * @return int
 */
static int get_adc_key_value(uint32_t Vmv)
{
    volatile uint16_t key_num[5] = { 0 };
    uint8_t bigger = 0, i = 0, j = 0;

    if (Vmv >= KEY0_ADC_VALUE && Vmv <= KEY4_ADC_VALUE + KEY_MEASURE_OFFSET) {
        key_machine.keyValue[key_machine.vaildKeyNum] = get_adc_value_range(Vmv);
        key_machine.vaildKeyNum++;

        if (key_machine.vaildKeyNum > KEY_FILTER_NUM) {
            for (i = 0; i < KEY_FILTER_NUM; i++) {
                //Count the most Key Value
                switch (key_machine.keyValue[i]) {
                    case KEY0_ADC_VALUE:
                        key_num[0]++;
                        break;

                    case KEY1_ADC_VALUE:
                        key_num[1]++;
                        break;

                    case KEY2_ADC_VALUE:
                        key_num[2]++;
                        break;

                    case KEY3_ADC_VALUE:
                        key_num[3]++;
                        break;

                    case KEY4_ADC_VALUE:
                        key_num[4]++;
                        break;

                    default:
                        break;
                }
            }

            for (i = 0; i < 5; i++) {
                for (j = 0; j < 5; j++) {
                    if (key_num[i] >= key_num[j]) {
                        bigger = 1;
                    } else {
                        bigger = 0;
                        break;
                    }
                }

                if (bigger) {
                    key_machine_init();
                    return i;
                }
            }
        }
    }

    return KEY_NO_VALUE;
}

// uint32_t sum = 0;
// static uint32_t adc_val_filter(void)
// {
//     static uint32_t cnt_filter = 0;
//     static uint32_t volt_value = 0;

//     volt_value  += adc_value[0];

//     if( ++cnt_filter >= 20)
//     {
//         cnt_filter = 0;
//         key_voltage = (volt_value / 20 * 30 + key_voltage * 70) / 100;
//         volt_value = 0;
//     }
// }

int main(void)
{
    bflb_platform_init(0);
    uint16_t keyValue = 0;

    adc_channel_cfg_t adc_channel_cfg;

    adc_channel_cfg.pos_channel = posChList;
    adc_channel_cfg.neg_channel = negChList;
    adc_channel_cfg.num = 1;

    adc_register(ADC0_INDEX, "adc_key");

    adc_key = device_find("adc_key");

    if (adc_key) {
        ADC_DEV(adc_key)->continuous_conv_mode = ENABLE;
        device_open(adc_key, DEVICE_OFLAG_STREAM_RX);

        if (device_control(adc_key, DEVICE_CTRL_ADC_CHANNEL_CONFIG, &adc_channel_cfg) == ERROR) {
            MSG("ADC channel config error , Please check the channel corresponding to IO is initial success by board system or Channel is invaild \r\n");
            BL_CASE_FAIL;
            while (1)
                ;
        }

    } else {
        MSG("device open failed\r\n");
    }

    key_machine_init();

    adc_channel_start(adc_key);

    while (1) {
        device_read(adc_key, 0, (void *)&result_val, 1); /*max size is 32*/
        keyValue = get_adc_key_value(result_val.volt * 1000);

        if (keyValue != KEY_NO_VALUE) {
            MSG("key %d pressed\r\n", keyValue);
            MSG("result_val.volt: %0.2f mv\n", (result_val.volt * 1000));
        }
    }
}