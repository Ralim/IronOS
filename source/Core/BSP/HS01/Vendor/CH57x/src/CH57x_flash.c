/********************************** (C) COPYRIGHT *******************************
 * File Name          : CH57x_flash.c
 * Author             : WCH
 * Version            : V1.2
 * Date               : 2021/11/17
 * Description
 * Copyright (c) 2021 Nanjing Qinheng Microelectronics Co., Ltd.
 * SPDX-License-Identifier: Apache-2.0
 *******************************************************************************/

#include "CH57x_common.h"

/*********************************************************************
 * @fn      FLASH_ROM_READ
 *
 * @brief   Read Flash
 *
 * @param   StartAddr   - read address
 * @param   Buffer      - read buffer
 * @param   len         - read len
 *
 * @return  none
 */
void FLASH_ROM_READ(uint32_t StartAddr, void *Buffer, uint32_t len) {
  uint32_t  i, Length = (len + 3) >> 2;
  uint32_t *pCode = (uint32_t *)StartAddr;
  uint32_t *pBuf  = (uint32_t *)Buffer;

  for (i = 0; i < Length; i++) {
    *pBuf++ = *pCode++;
  }
}
