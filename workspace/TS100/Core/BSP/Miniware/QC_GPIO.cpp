/*
 * QC.c
 *
 *  Created on: 29 May 2020
 *      Author: Ralim
 */
#include "BSP.h"
#include "Pins.h"
#include "QC3.h"
#include "Settings.h"
#include "stm32f1xx_hal.h"
void QC_DPlusZero_Six() {
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_RESET);  // pull down D+
}
void QC_DNegZero_Six() {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_RESET);
}
void QC_DPlusThree_Three() {
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_3, GPIO_PIN_SET);  // pull up D+
}
void QC_DNegThree_Three() {
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_10, GPIO_PIN_SET);
    HAL_GPIO_WritePin(GPIOA, GPIO_PIN_8, GPIO_PIN_SET);
}
void QC_DM_PullDown() {
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_PULLDOWN;
    GPIO_InitStruct.Pin = GPIO_PIN_11;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}
void QC_DM_No_PullDown() {
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Pin = GPIO_PIN_11;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}
void QC_Init_GPIO() {
    // Setup any GPIO into the right states for QC
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Pin = GPIO_PIN_3;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_10;
    HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
    // Turn off output mode on pins that we can
    GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = GPIO_NOPULL;
    GPIO_InitStruct.Pin = GPIO_PIN_11 | GPIO_PIN_12 | GPIO_PIN_14 | GPIO_PIN_13;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}
void QC_Post_Probe_En() {
    GPIO_InitTypeDef GPIO_InitStruct;
    GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.Pin = GPIO_PIN_8 | GPIO_PIN_10;
    GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
    HAL_GPIO_Init(GPIOA, &GPIO_InitStruct);
}

uint8_t QC_DM_PulledDown() { return HAL_GPIO_ReadPin(GPIOA, GPIO_PIN_11) == GPIO_PIN_RESET ? 1 : 0; }

void QC_resync() {
#ifdef MODEL_TS80
    seekQC((systemSettings.cutoutSetting) ? 120 : 90,
           systemSettings.voltageDiv);  // Run the QC seek again if we have drifted too much
#endif
}