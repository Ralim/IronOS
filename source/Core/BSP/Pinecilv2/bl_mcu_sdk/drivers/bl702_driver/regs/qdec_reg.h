/**
  ******************************************************************************
  * @file    qdec_reg.h
  * @version V1.2
  * @date    2020-04-08
  * @brief   This file is the description of.IP register
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2020 Bouffalo Lab</center></h2>
  *
  * Redistribution and use in source and binary forms, with or without modification,
  * are permitted provided that the following conditions are met:
  *   1. Redistributions of source code must retain the above copyright notice,
  *      this list of conditions and the following disclaimer.
  *   2. Redistributions in binary form must reproduce the above copyright notice,
  *      this list of conditions and the following disclaimer in the documentation
  *      and/or other materials provided with the distribution.
  *   3. Neither the name of Bouffalo Lab nor the names of its contributors
  *      may be used to endorse or promote products derived from this software
  *      without specific prior written permission.
  *
  * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
  * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
  * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
  * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE
  * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
  * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
  * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
  * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
  * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
  * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
  *
  ******************************************************************************
  */
#ifndef __QDEC_REG_H__
#define __QDEC_REG_H__

#include "bl702.h"

/* 0x0 : qdec0_ctrl0 */
#define QDEC0_CTRL0_OFFSET   (0x0)
#define QDEC_EN              QDEC_EN
#define QDEC_EN_POS          (0U)
#define QDEC_EN_LEN          (1U)
#define QDEC_EN_MSK          (((1U << QDEC_EN_LEN) - 1) << QDEC_EN_POS)
#define QDEC_EN_UMSK         (~(((1U << QDEC_EN_LEN) - 1) << QDEC_EN_POS))
#define QDEC_LED_EN          QDEC_LED_EN
#define QDEC_LED_EN_POS      (1U)
#define QDEC_LED_EN_LEN      (1U)
#define QDEC_LED_EN_MSK      (((1U << QDEC_LED_EN_LEN) - 1) << QDEC_LED_EN_POS)
#define QDEC_LED_EN_UMSK     (~(((1U << QDEC_LED_EN_LEN) - 1) << QDEC_LED_EN_POS))
#define QDEC_LED_POL         QDEC_LED_POL
#define QDEC_LED_POL_POS     (2U)
#define QDEC_LED_POL_LEN     (1U)
#define QDEC_LED_POL_MSK     (((1U << QDEC_LED_POL_LEN) - 1) << QDEC_LED_POL_POS)
#define QDEC_LED_POL_UMSK    (~(((1U << QDEC_LED_POL_LEN) - 1) << QDEC_LED_POL_POS))
#define QDEC_DEG_EN          QDEC_DEG_EN
#define QDEC_DEG_EN_POS      (3U)
#define QDEC_DEG_EN_LEN      (1U)
#define QDEC_DEG_EN_MSK      (((1U << QDEC_DEG_EN_LEN) - 1) << QDEC_DEG_EN_POS)
#define QDEC_DEG_EN_UMSK     (~(((1U << QDEC_DEG_EN_LEN) - 1) << QDEC_DEG_EN_POS))
#define QDEC_DEG_CNT         QDEC_DEG_CNT
#define QDEC_DEG_CNT_POS     (4U)
#define QDEC_DEG_CNT_LEN     (4U)
#define QDEC_DEG_CNT_MSK     (((1U << QDEC_DEG_CNT_LEN) - 1) << QDEC_DEG_CNT_POS)
#define QDEC_DEG_CNT_UMSK    (~(((1U << QDEC_DEG_CNT_LEN) - 1) << QDEC_DEG_CNT_POS))
#define QDEC_SPL_PERIOD      QDEC_SPL_PERIOD
#define QDEC_SPL_PERIOD_POS  (8U)
#define QDEC_SPL_PERIOD_LEN  (4U)
#define QDEC_SPL_PERIOD_MSK  (((1U << QDEC_SPL_PERIOD_LEN) - 1) << QDEC_SPL_PERIOD_POS)
#define QDEC_SPL_PERIOD_UMSK (~(((1U << QDEC_SPL_PERIOD_LEN) - 1) << QDEC_SPL_PERIOD_POS))
#define QDEC_RPT_PERIOD      QDEC_RPT_PERIOD
#define QDEC_RPT_PERIOD_POS  (12U)
#define QDEC_RPT_PERIOD_LEN  (16U)
#define QDEC_RPT_PERIOD_MSK  (((1U << QDEC_RPT_PERIOD_LEN) - 1) << QDEC_RPT_PERIOD_POS)
#define QDEC_RPT_PERIOD_UMSK (~(((1U << QDEC_RPT_PERIOD_LEN) - 1) << QDEC_RPT_PERIOD_POS))

/* 0x4 : qdec0_ctrl1 */
#define QDEC0_CTRL1_OFFSET   (0x4)
#define QDEC_ACC_MODE        QDEC_ACC_MODE
#define QDEC_ACC_MODE_POS    (0U)
#define QDEC_ACC_MODE_LEN    (1U)
#define QDEC_ACC_MODE_MSK    (((1U << QDEC_ACC_MODE_LEN) - 1) << QDEC_ACC_MODE_POS)
#define QDEC_ACC_MODE_UMSK   (~(((1U << QDEC_ACC_MODE_LEN) - 1) << QDEC_ACC_MODE_POS))
#define QDEC_SPL_MODE        QDEC_SPL_MODE
#define QDEC_SPL_MODE_POS    (1U)
#define QDEC_SPL_MODE_LEN    (1U)
#define QDEC_SPL_MODE_MSK    (((1U << QDEC_SPL_MODE_LEN) - 1) << QDEC_SPL_MODE_POS)
#define QDEC_SPL_MODE_UMSK   (~(((1U << QDEC_SPL_MODE_LEN) - 1) << QDEC_SPL_MODE_POS))
#define QDEC_RPT_MODE        QDEC_RPT_MODE
#define QDEC_RPT_MODE_POS    (2U)
#define QDEC_RPT_MODE_LEN    (1U)
#define QDEC_RPT_MODE_MSK    (((1U << QDEC_RPT_MODE_LEN) - 1) << QDEC_RPT_MODE_POS)
#define QDEC_RPT_MODE_UMSK   (~(((1U << QDEC_RPT_MODE_LEN) - 1) << QDEC_RPT_MODE_POS))
#define QDEC_INPUT_SWAP      QDEC_INPUT_SWAP
#define QDEC_INPUT_SWAP_POS  (3U)
#define QDEC_INPUT_SWAP_LEN  (1U)
#define QDEC_INPUT_SWAP_MSK  (((1U << QDEC_INPUT_SWAP_LEN) - 1) << QDEC_INPUT_SWAP_POS)
#define QDEC_INPUT_SWAP_UMSK (~(((1U << QDEC_INPUT_SWAP_LEN) - 1) << QDEC_INPUT_SWAP_POS))
#define QDEC_LED_PERIOD      QDEC_LED_PERIOD
#define QDEC_LED_PERIOD_POS  (16U)
#define QDEC_LED_PERIOD_LEN  (9U)
#define QDEC_LED_PERIOD_MSK  (((1U << QDEC_LED_PERIOD_LEN) - 1) << QDEC_LED_PERIOD_POS)
#define QDEC_LED_PERIOD_UMSK (~(((1U << QDEC_LED_PERIOD_LEN) - 1) << QDEC_LED_PERIOD_POS))

/* 0x8 : qdec0_value */
#define QDEC0_VALUE_OFFSET (0x8)
#define QDEC_ACC1_VAL      QDEC_ACC1_VAL
#define QDEC_ACC1_VAL_POS  (0U)
#define QDEC_ACC1_VAL_LEN  (16U)
#define QDEC_ACC1_VAL_MSK  (((1U << QDEC_ACC1_VAL_LEN) - 1) << QDEC_ACC1_VAL_POS)
#define QDEC_ACC1_VAL_UMSK (~(((1U << QDEC_ACC1_VAL_LEN) - 1) << QDEC_ACC1_VAL_POS))
#define QDEC_ACC2_VAL      QDEC_ACC2_VAL
#define QDEC_ACC2_VAL_POS  (16U)
#define QDEC_ACC2_VAL_LEN  (4U)
#define QDEC_ACC2_VAL_MSK  (((1U << QDEC_ACC2_VAL_LEN) - 1) << QDEC_ACC2_VAL_POS)
#define QDEC_ACC2_VAL_UMSK (~(((1U << QDEC_ACC2_VAL_LEN) - 1) << QDEC_ACC2_VAL_POS))
#define QDEC_SPL_VAL       QDEC_SPL_VAL
#define QDEC_SPL_VAL_POS   (28U)
#define QDEC_SPL_VAL_LEN   (2U)
#define QDEC_SPL_VAL_MSK   (((1U << QDEC_SPL_VAL_LEN) - 1) << QDEC_SPL_VAL_POS)
#define QDEC_SPL_VAL_UMSK  (~(((1U << QDEC_SPL_VAL_LEN) - 1) << QDEC_SPL_VAL_POS))

/* 0x10 : qdec0_int_en */
#define QDEC0_INT_EN_OFFSET   (0x10)
#define QDEC_RPT_RDY_EN       QDEC_RPT_RDY_EN
#define QDEC_RPT_RDY_EN_POS   (0U)
#define QDEC_RPT_RDY_EN_LEN   (1U)
#define QDEC_RPT_RDY_EN_MSK   (((1U << QDEC_RPT_RDY_EN_LEN) - 1) << QDEC_RPT_RDY_EN_POS)
#define QDEC_RPT_RDY_EN_UMSK  (~(((1U << QDEC_RPT_RDY_EN_LEN) - 1) << QDEC_RPT_RDY_EN_POS))
#define QDEC_SPL_RDY_EN       QDEC_SPL_RDY_EN
#define QDEC_SPL_RDY_EN_POS   (1U)
#define QDEC_SPL_RDY_EN_LEN   (1U)
#define QDEC_SPL_RDY_EN_MSK   (((1U << QDEC_SPL_RDY_EN_LEN) - 1) << QDEC_SPL_RDY_EN_POS)
#define QDEC_SPL_RDY_EN_UMSK  (~(((1U << QDEC_SPL_RDY_EN_LEN) - 1) << QDEC_SPL_RDY_EN_POS))
#define QDEC_DBL_RDY_EN       QDEC_DBL_RDY_EN
#define QDEC_DBL_RDY_EN_POS   (2U)
#define QDEC_DBL_RDY_EN_LEN   (1U)
#define QDEC_DBL_RDY_EN_MSK   (((1U << QDEC_DBL_RDY_EN_LEN) - 1) << QDEC_DBL_RDY_EN_POS)
#define QDEC_DBL_RDY_EN_UMSK  (~(((1U << QDEC_DBL_RDY_EN_LEN) - 1) << QDEC_DBL_RDY_EN_POS))
#define QDEC_OVERFLOW_EN      QDEC_OVERFLOW_EN
#define QDEC_OVERFLOW_EN_POS  (3U)
#define QDEC_OVERFLOW_EN_LEN  (1U)
#define QDEC_OVERFLOW_EN_MSK  (((1U << QDEC_OVERFLOW_EN_LEN) - 1) << QDEC_OVERFLOW_EN_POS)
#define QDEC_OVERFLOW_EN_UMSK (~(((1U << QDEC_OVERFLOW_EN_LEN) - 1) << QDEC_OVERFLOW_EN_POS))

/* 0x14 : qdec0_int_sts */
#define QDEC0_INT_STS_OFFSET   (0x14)
#define QDEC_RPT_RDY_STS       QDEC_RPT_RDY_STS
#define QDEC_RPT_RDY_STS_POS   (0U)
#define QDEC_RPT_RDY_STS_LEN   (1U)
#define QDEC_RPT_RDY_STS_MSK   (((1U << QDEC_RPT_RDY_STS_LEN) - 1) << QDEC_RPT_RDY_STS_POS)
#define QDEC_RPT_RDY_STS_UMSK  (~(((1U << QDEC_RPT_RDY_STS_LEN) - 1) << QDEC_RPT_RDY_STS_POS))
#define QDEC_SPL_RDY_STS       QDEC_SPL_RDY_STS
#define QDEC_SPL_RDY_STS_POS   (1U)
#define QDEC_SPL_RDY_STS_LEN   (1U)
#define QDEC_SPL_RDY_STS_MSK   (((1U << QDEC_SPL_RDY_STS_LEN) - 1) << QDEC_SPL_RDY_STS_POS)
#define QDEC_SPL_RDY_STS_UMSK  (~(((1U << QDEC_SPL_RDY_STS_LEN) - 1) << QDEC_SPL_RDY_STS_POS))
#define QDEC_DBL_RDY_STS       QDEC_DBL_RDY_STS
#define QDEC_DBL_RDY_STS_POS   (2U)
#define QDEC_DBL_RDY_STS_LEN   (1U)
#define QDEC_DBL_RDY_STS_MSK   (((1U << QDEC_DBL_RDY_STS_LEN) - 1) << QDEC_DBL_RDY_STS_POS)
#define QDEC_DBL_RDY_STS_UMSK  (~(((1U << QDEC_DBL_RDY_STS_LEN) - 1) << QDEC_DBL_RDY_STS_POS))
#define QDEC_OVERFLOW_STS      QDEC_OVERFLOW_STS
#define QDEC_OVERFLOW_STS_POS  (3U)
#define QDEC_OVERFLOW_STS_LEN  (1U)
#define QDEC_OVERFLOW_STS_MSK  (((1U << QDEC_OVERFLOW_STS_LEN) - 1) << QDEC_OVERFLOW_STS_POS)
#define QDEC_OVERFLOW_STS_UMSK (~(((1U << QDEC_OVERFLOW_STS_LEN) - 1) << QDEC_OVERFLOW_STS_POS))

/* 0x18 : qdec0_int_clr */
#define QDEC0_INT_CLR_OFFSET   (0x18)
#define QDEC_RPT_RDY_CLR       QDEC_RPT_RDY_CLR
#define QDEC_RPT_RDY_CLR_POS   (0U)
#define QDEC_RPT_RDY_CLR_LEN   (1U)
#define QDEC_RPT_RDY_CLR_MSK   (((1U << QDEC_RPT_RDY_CLR_LEN) - 1) << QDEC_RPT_RDY_CLR_POS)
#define QDEC_RPT_RDY_CLR_UMSK  (~(((1U << QDEC_RPT_RDY_CLR_LEN) - 1) << QDEC_RPT_RDY_CLR_POS))
#define QDEC_SPL_RDY_CLR       QDEC_SPL_RDY_CLR
#define QDEC_SPL_RDY_CLR_POS   (1U)
#define QDEC_SPL_RDY_CLR_LEN   (1U)
#define QDEC_SPL_RDY_CLR_MSK   (((1U << QDEC_SPL_RDY_CLR_LEN) - 1) << QDEC_SPL_RDY_CLR_POS)
#define QDEC_SPL_RDY_CLR_UMSK  (~(((1U << QDEC_SPL_RDY_CLR_LEN) - 1) << QDEC_SPL_RDY_CLR_POS))
#define QDEC_DBL_RDY_CLR       QDEC_DBL_RDY_CLR
#define QDEC_DBL_RDY_CLR_POS   (2U)
#define QDEC_DBL_RDY_CLR_LEN   (1U)
#define QDEC_DBL_RDY_CLR_MSK   (((1U << QDEC_DBL_RDY_CLR_LEN) - 1) << QDEC_DBL_RDY_CLR_POS)
#define QDEC_DBL_RDY_CLR_UMSK  (~(((1U << QDEC_DBL_RDY_CLR_LEN) - 1) << QDEC_DBL_RDY_CLR_POS))
#define QDEC_OVERFLOW_CLR      QDEC_OVERFLOW_CLR
#define QDEC_OVERFLOW_CLR_POS  (3U)
#define QDEC_OVERFLOW_CLR_LEN  (1U)
#define QDEC_OVERFLOW_CLR_MSK  (((1U << QDEC_OVERFLOW_CLR_LEN) - 1) << QDEC_OVERFLOW_CLR_POS)
#define QDEC_OVERFLOW_CLR_UMSK (~(((1U << QDEC_OVERFLOW_CLR_LEN) - 1) << QDEC_OVERFLOW_CLR_POS))

/* 0x40 : qdec1_ctrl0 */
#define QDEC1_CTRL0_OFFSET   (0x40)
#define QDEC_EN              QDEC_EN
#define QDEC_EN_POS          (0U)
#define QDEC_EN_LEN          (1U)
#define QDEC_EN_MSK          (((1U << QDEC_EN_LEN) - 1) << QDEC_EN_POS)
#define QDEC_EN_UMSK         (~(((1U << QDEC_EN_LEN) - 1) << QDEC_EN_POS))
#define QDEC_LED_EN          QDEC_LED_EN
#define QDEC_LED_EN_POS      (1U)
#define QDEC_LED_EN_LEN      (1U)
#define QDEC_LED_EN_MSK      (((1U << QDEC_LED_EN_LEN) - 1) << QDEC_LED_EN_POS)
#define QDEC_LED_EN_UMSK     (~(((1U << QDEC_LED_EN_LEN) - 1) << QDEC_LED_EN_POS))
#define QDEC_LED_POL         QDEC_LED_POL
#define QDEC_LED_POL_POS     (2U)
#define QDEC_LED_POL_LEN     (1U)
#define QDEC_LED_POL_MSK     (((1U << QDEC_LED_POL_LEN) - 1) << QDEC_LED_POL_POS)
#define QDEC_LED_POL_UMSK    (~(((1U << QDEC_LED_POL_LEN) - 1) << QDEC_LED_POL_POS))
#define QDEC_DEG_EN          QDEC_DEG_EN
#define QDEC_DEG_EN_POS      (3U)
#define QDEC_DEG_EN_LEN      (1U)
#define QDEC_DEG_EN_MSK      (((1U << QDEC_DEG_EN_LEN) - 1) << QDEC_DEG_EN_POS)
#define QDEC_DEG_EN_UMSK     (~(((1U << QDEC_DEG_EN_LEN) - 1) << QDEC_DEG_EN_POS))
#define QDEC_DEG_CNT         QDEC_DEG_CNT
#define QDEC_DEG_CNT_POS     (4U)
#define QDEC_DEG_CNT_LEN     (4U)
#define QDEC_DEG_CNT_MSK     (((1U << QDEC_DEG_CNT_LEN) - 1) << QDEC_DEG_CNT_POS)
#define QDEC_DEG_CNT_UMSK    (~(((1U << QDEC_DEG_CNT_LEN) - 1) << QDEC_DEG_CNT_POS))
#define QDEC_SPL_PERIOD      QDEC_SPL_PERIOD
#define QDEC_SPL_PERIOD_POS  (8U)
#define QDEC_SPL_PERIOD_LEN  (4U)
#define QDEC_SPL_PERIOD_MSK  (((1U << QDEC_SPL_PERIOD_LEN) - 1) << QDEC_SPL_PERIOD_POS)
#define QDEC_SPL_PERIOD_UMSK (~(((1U << QDEC_SPL_PERIOD_LEN) - 1) << QDEC_SPL_PERIOD_POS))
#define QDEC_RPT_PERIOD      QDEC_RPT_PERIOD
#define QDEC_RPT_PERIOD_POS  (12U)
#define QDEC_RPT_PERIOD_LEN  (16U)
#define QDEC_RPT_PERIOD_MSK  (((1U << QDEC_RPT_PERIOD_LEN) - 1) << QDEC_RPT_PERIOD_POS)
#define QDEC_RPT_PERIOD_UMSK (~(((1U << QDEC_RPT_PERIOD_LEN) - 1) << QDEC_RPT_PERIOD_POS))

/* 0x44 : qdec1_ctrl1 */
#define QDEC1_CTRL1_OFFSET   (0x44)
#define QDEC_ACC_MODE        QDEC_ACC_MODE
#define QDEC_ACC_MODE_POS    (0U)
#define QDEC_ACC_MODE_LEN    (1U)
#define QDEC_ACC_MODE_MSK    (((1U << QDEC_ACC_MODE_LEN) - 1) << QDEC_ACC_MODE_POS)
#define QDEC_ACC_MODE_UMSK   (~(((1U << QDEC_ACC_MODE_LEN) - 1) << QDEC_ACC_MODE_POS))
#define QDEC_SPL_MODE        QDEC_SPL_MODE
#define QDEC_SPL_MODE_POS    (1U)
#define QDEC_SPL_MODE_LEN    (1U)
#define QDEC_SPL_MODE_MSK    (((1U << QDEC_SPL_MODE_LEN) - 1) << QDEC_SPL_MODE_POS)
#define QDEC_SPL_MODE_UMSK   (~(((1U << QDEC_SPL_MODE_LEN) - 1) << QDEC_SPL_MODE_POS))
#define QDEC_RPT_MODE        QDEC_RPT_MODE
#define QDEC_RPT_MODE_POS    (2U)
#define QDEC_RPT_MODE_LEN    (1U)
#define QDEC_RPT_MODE_MSK    (((1U << QDEC_RPT_MODE_LEN) - 1) << QDEC_RPT_MODE_POS)
#define QDEC_RPT_MODE_UMSK   (~(((1U << QDEC_RPT_MODE_LEN) - 1) << QDEC_RPT_MODE_POS))
#define QDEC_INPUT_SWAP      QDEC_INPUT_SWAP
#define QDEC_INPUT_SWAP_POS  (3U)
#define QDEC_INPUT_SWAP_LEN  (1U)
#define QDEC_INPUT_SWAP_MSK  (((1U << QDEC_INPUT_SWAP_LEN) - 1) << QDEC_INPUT_SWAP_POS)
#define QDEC_INPUT_SWAP_UMSK (~(((1U << QDEC_INPUT_SWAP_LEN) - 1) << QDEC_INPUT_SWAP_POS))
#define QDEC_LED_PERIOD      QDEC_LED_PERIOD
#define QDEC_LED_PERIOD_POS  (16U)
#define QDEC_LED_PERIOD_LEN  (9U)
#define QDEC_LED_PERIOD_MSK  (((1U << QDEC_LED_PERIOD_LEN) - 1) << QDEC_LED_PERIOD_POS)
#define QDEC_LED_PERIOD_UMSK (~(((1U << QDEC_LED_PERIOD_LEN) - 1) << QDEC_LED_PERIOD_POS))

/* 0x48 : qdec1_value */
#define QDEC1_VALUE_OFFSET (0x48)
#define QDEC_ACC1_VAL      QDEC_ACC1_VAL
#define QDEC_ACC1_VAL_POS  (0U)
#define QDEC_ACC1_VAL_LEN  (16U)
#define QDEC_ACC1_VAL_MSK  (((1U << QDEC_ACC1_VAL_LEN) - 1) << QDEC_ACC1_VAL_POS)
#define QDEC_ACC1_VAL_UMSK (~(((1U << QDEC_ACC1_VAL_LEN) - 1) << QDEC_ACC1_VAL_POS))
#define QDEC_ACC2_VAL      QDEC_ACC2_VAL
#define QDEC_ACC2_VAL_POS  (16U)
#define QDEC_ACC2_VAL_LEN  (4U)
#define QDEC_ACC2_VAL_MSK  (((1U << QDEC_ACC2_VAL_LEN) - 1) << QDEC_ACC2_VAL_POS)
#define QDEC_ACC2_VAL_UMSK (~(((1U << QDEC_ACC2_VAL_LEN) - 1) << QDEC_ACC2_VAL_POS))
#define QDEC_SPL_VAL       QDEC_SPL_VAL
#define QDEC_SPL_VAL_POS   (28U)
#define QDEC_SPL_VAL_LEN   (2U)
#define QDEC_SPL_VAL_MSK   (((1U << QDEC_SPL_VAL_LEN) - 1) << QDEC_SPL_VAL_POS)
#define QDEC_SPL_VAL_UMSK  (~(((1U << QDEC_SPL_VAL_LEN) - 1) << QDEC_SPL_VAL_POS))

/* 0x50 : qdec1_int_en */
#define QDEC1_INT_EN_OFFSET   (0x50)
#define QDEC_RPT_RDY_EN       QDEC_RPT_RDY_EN
#define QDEC_RPT_RDY_EN_POS   (0U)
#define QDEC_RPT_RDY_EN_LEN   (1U)
#define QDEC_RPT_RDY_EN_MSK   (((1U << QDEC_RPT_RDY_EN_LEN) - 1) << QDEC_RPT_RDY_EN_POS)
#define QDEC_RPT_RDY_EN_UMSK  (~(((1U << QDEC_RPT_RDY_EN_LEN) - 1) << QDEC_RPT_RDY_EN_POS))
#define QDEC_SPL_RDY_EN       QDEC_SPL_RDY_EN
#define QDEC_SPL_RDY_EN_POS   (1U)
#define QDEC_SPL_RDY_EN_LEN   (1U)
#define QDEC_SPL_RDY_EN_MSK   (((1U << QDEC_SPL_RDY_EN_LEN) - 1) << QDEC_SPL_RDY_EN_POS)
#define QDEC_SPL_RDY_EN_UMSK  (~(((1U << QDEC_SPL_RDY_EN_LEN) - 1) << QDEC_SPL_RDY_EN_POS))
#define QDEC_DBL_RDY_EN       QDEC_DBL_RDY_EN
#define QDEC_DBL_RDY_EN_POS   (2U)
#define QDEC_DBL_RDY_EN_LEN   (1U)
#define QDEC_DBL_RDY_EN_MSK   (((1U << QDEC_DBL_RDY_EN_LEN) - 1) << QDEC_DBL_RDY_EN_POS)
#define QDEC_DBL_RDY_EN_UMSK  (~(((1U << QDEC_DBL_RDY_EN_LEN) - 1) << QDEC_DBL_RDY_EN_POS))
#define QDEC_OVERFLOW_EN      QDEC_OVERFLOW_EN
#define QDEC_OVERFLOW_EN_POS  (3U)
#define QDEC_OVERFLOW_EN_LEN  (1U)
#define QDEC_OVERFLOW_EN_MSK  (((1U << QDEC_OVERFLOW_EN_LEN) - 1) << QDEC_OVERFLOW_EN_POS)
#define QDEC_OVERFLOW_EN_UMSK (~(((1U << QDEC_OVERFLOW_EN_LEN) - 1) << QDEC_OVERFLOW_EN_POS))

/* 0x54 : qdec1_int_sts */
#define QDEC1_INT_STS_OFFSET   (0x54)
#define QDEC_RPT_RDY_STS       QDEC_RPT_RDY_STS
#define QDEC_RPT_RDY_STS_POS   (0U)
#define QDEC_RPT_RDY_STS_LEN   (1U)
#define QDEC_RPT_RDY_STS_MSK   (((1U << QDEC_RPT_RDY_STS_LEN) - 1) << QDEC_RPT_RDY_STS_POS)
#define QDEC_RPT_RDY_STS_UMSK  (~(((1U << QDEC_RPT_RDY_STS_LEN) - 1) << QDEC_RPT_RDY_STS_POS))
#define QDEC_SPL_RDY_STS       QDEC_SPL_RDY_STS
#define QDEC_SPL_RDY_STS_POS   (1U)
#define QDEC_SPL_RDY_STS_LEN   (1U)
#define QDEC_SPL_RDY_STS_MSK   (((1U << QDEC_SPL_RDY_STS_LEN) - 1) << QDEC_SPL_RDY_STS_POS)
#define QDEC_SPL_RDY_STS_UMSK  (~(((1U << QDEC_SPL_RDY_STS_LEN) - 1) << QDEC_SPL_RDY_STS_POS))
#define QDEC_DBL_RDY_STS       QDEC_DBL_RDY_STS
#define QDEC_DBL_RDY_STS_POS   (2U)
#define QDEC_DBL_RDY_STS_LEN   (1U)
#define QDEC_DBL_RDY_STS_MSK   (((1U << QDEC_DBL_RDY_STS_LEN) - 1) << QDEC_DBL_RDY_STS_POS)
#define QDEC_DBL_RDY_STS_UMSK  (~(((1U << QDEC_DBL_RDY_STS_LEN) - 1) << QDEC_DBL_RDY_STS_POS))
#define QDEC_OVERFLOW_STS      QDEC_OVERFLOW_STS
#define QDEC_OVERFLOW_STS_POS  (3U)
#define QDEC_OVERFLOW_STS_LEN  (1U)
#define QDEC_OVERFLOW_STS_MSK  (((1U << QDEC_OVERFLOW_STS_LEN) - 1) << QDEC_OVERFLOW_STS_POS)
#define QDEC_OVERFLOW_STS_UMSK (~(((1U << QDEC_OVERFLOW_STS_LEN) - 1) << QDEC_OVERFLOW_STS_POS))

/* 0x58 : qdec1_int_clr */
#define QDEC1_INT_CLR_OFFSET   (0x58)
#define QDEC_RPT_RDY_CLR       QDEC_RPT_RDY_CLR
#define QDEC_RPT_RDY_CLR_POS   (0U)
#define QDEC_RPT_RDY_CLR_LEN   (1U)
#define QDEC_RPT_RDY_CLR_MSK   (((1U << QDEC_RPT_RDY_CLR_LEN) - 1) << QDEC_RPT_RDY_CLR_POS)
#define QDEC_RPT_RDY_CLR_UMSK  (~(((1U << QDEC_RPT_RDY_CLR_LEN) - 1) << QDEC_RPT_RDY_CLR_POS))
#define QDEC_SPL_RDY_CLR       QDEC_SPL_RDY_CLR
#define QDEC_SPL_RDY_CLR_POS   (1U)
#define QDEC_SPL_RDY_CLR_LEN   (1U)
#define QDEC_SPL_RDY_CLR_MSK   (((1U << QDEC_SPL_RDY_CLR_LEN) - 1) << QDEC_SPL_RDY_CLR_POS)
#define QDEC_SPL_RDY_CLR_UMSK  (~(((1U << QDEC_SPL_RDY_CLR_LEN) - 1) << QDEC_SPL_RDY_CLR_POS))
#define QDEC_DBL_RDY_CLR       QDEC_DBL_RDY_CLR
#define QDEC_DBL_RDY_CLR_POS   (2U)
#define QDEC_DBL_RDY_CLR_LEN   (1U)
#define QDEC_DBL_RDY_CLR_MSK   (((1U << QDEC_DBL_RDY_CLR_LEN) - 1) << QDEC_DBL_RDY_CLR_POS)
#define QDEC_DBL_RDY_CLR_UMSK  (~(((1U << QDEC_DBL_RDY_CLR_LEN) - 1) << QDEC_DBL_RDY_CLR_POS))
#define QDEC_OVERFLOW_CLR      QDEC_OVERFLOW_CLR
#define QDEC_OVERFLOW_CLR_POS  (3U)
#define QDEC_OVERFLOW_CLR_LEN  (1U)
#define QDEC_OVERFLOW_CLR_MSK  (((1U << QDEC_OVERFLOW_CLR_LEN) - 1) << QDEC_OVERFLOW_CLR_POS)
#define QDEC_OVERFLOW_CLR_UMSK (~(((1U << QDEC_OVERFLOW_CLR_LEN) - 1) << QDEC_OVERFLOW_CLR_POS))

/* 0x80 : qdec2_ctrl0 */
#define QDEC2_CTRL0_OFFSET   (0x80)
#define QDEC_EN              QDEC_EN
#define QDEC_EN_POS          (0U)
#define QDEC_EN_LEN          (1U)
#define QDEC_EN_MSK          (((1U << QDEC_EN_LEN) - 1) << QDEC_EN_POS)
#define QDEC_EN_UMSK         (~(((1U << QDEC_EN_LEN) - 1) << QDEC_EN_POS))
#define QDEC_LED_EN          QDEC_LED_EN
#define QDEC_LED_EN_POS      (1U)
#define QDEC_LED_EN_LEN      (1U)
#define QDEC_LED_EN_MSK      (((1U << QDEC_LED_EN_LEN) - 1) << QDEC_LED_EN_POS)
#define QDEC_LED_EN_UMSK     (~(((1U << QDEC_LED_EN_LEN) - 1) << QDEC_LED_EN_POS))
#define QDEC_LED_POL         QDEC_LED_POL
#define QDEC_LED_POL_POS     (2U)
#define QDEC_LED_POL_LEN     (1U)
#define QDEC_LED_POL_MSK     (((1U << QDEC_LED_POL_LEN) - 1) << QDEC_LED_POL_POS)
#define QDEC_LED_POL_UMSK    (~(((1U << QDEC_LED_POL_LEN) - 1) << QDEC_LED_POL_POS))
#define QDEC_DEG_EN          QDEC_DEG_EN
#define QDEC_DEG_EN_POS      (3U)
#define QDEC_DEG_EN_LEN      (1U)
#define QDEC_DEG_EN_MSK      (((1U << QDEC_DEG_EN_LEN) - 1) << QDEC_DEG_EN_POS)
#define QDEC_DEG_EN_UMSK     (~(((1U << QDEC_DEG_EN_LEN) - 1) << QDEC_DEG_EN_POS))
#define QDEC_DEG_CNT         QDEC_DEG_CNT
#define QDEC_DEG_CNT_POS     (4U)
#define QDEC_DEG_CNT_LEN     (4U)
#define QDEC_DEG_CNT_MSK     (((1U << QDEC_DEG_CNT_LEN) - 1) << QDEC_DEG_CNT_POS)
#define QDEC_DEG_CNT_UMSK    (~(((1U << QDEC_DEG_CNT_LEN) - 1) << QDEC_DEG_CNT_POS))
#define QDEC_SPL_PERIOD      QDEC_SPL_PERIOD
#define QDEC_SPL_PERIOD_POS  (8U)
#define QDEC_SPL_PERIOD_LEN  (4U)
#define QDEC_SPL_PERIOD_MSK  (((1U << QDEC_SPL_PERIOD_LEN) - 1) << QDEC_SPL_PERIOD_POS)
#define QDEC_SPL_PERIOD_UMSK (~(((1U << QDEC_SPL_PERIOD_LEN) - 1) << QDEC_SPL_PERIOD_POS))
#define QDEC_RPT_PERIOD      QDEC_RPT_PERIOD
#define QDEC_RPT_PERIOD_POS  (12U)
#define QDEC_RPT_PERIOD_LEN  (16U)
#define QDEC_RPT_PERIOD_MSK  (((1U << QDEC_RPT_PERIOD_LEN) - 1) << QDEC_RPT_PERIOD_POS)
#define QDEC_RPT_PERIOD_UMSK (~(((1U << QDEC_RPT_PERIOD_LEN) - 1) << QDEC_RPT_PERIOD_POS))

/* 0x84 : qdec2_ctrl1 */
#define QDEC2_CTRL1_OFFSET   (0x84)
#define QDEC_ACC_MODE        QDEC_ACC_MODE
#define QDEC_ACC_MODE_POS    (0U)
#define QDEC_ACC_MODE_LEN    (1U)
#define QDEC_ACC_MODE_MSK    (((1U << QDEC_ACC_MODE_LEN) - 1) << QDEC_ACC_MODE_POS)
#define QDEC_ACC_MODE_UMSK   (~(((1U << QDEC_ACC_MODE_LEN) - 1) << QDEC_ACC_MODE_POS))
#define QDEC_SPL_MODE        QDEC_SPL_MODE
#define QDEC_SPL_MODE_POS    (1U)
#define QDEC_SPL_MODE_LEN    (1U)
#define QDEC_SPL_MODE_MSK    (((1U << QDEC_SPL_MODE_LEN) - 1) << QDEC_SPL_MODE_POS)
#define QDEC_SPL_MODE_UMSK   (~(((1U << QDEC_SPL_MODE_LEN) - 1) << QDEC_SPL_MODE_POS))
#define QDEC_RPT_MODE        QDEC_RPT_MODE
#define QDEC_RPT_MODE_POS    (2U)
#define QDEC_RPT_MODE_LEN    (1U)
#define QDEC_RPT_MODE_MSK    (((1U << QDEC_RPT_MODE_LEN) - 1) << QDEC_RPT_MODE_POS)
#define QDEC_RPT_MODE_UMSK   (~(((1U << QDEC_RPT_MODE_LEN) - 1) << QDEC_RPT_MODE_POS))
#define QDEC_INPUT_SWAP      QDEC_INPUT_SWAP
#define QDEC_INPUT_SWAP_POS  (3U)
#define QDEC_INPUT_SWAP_LEN  (1U)
#define QDEC_INPUT_SWAP_MSK  (((1U << QDEC_INPUT_SWAP_LEN) - 1) << QDEC_INPUT_SWAP_POS)
#define QDEC_INPUT_SWAP_UMSK (~(((1U << QDEC_INPUT_SWAP_LEN) - 1) << QDEC_INPUT_SWAP_POS))
#define QDEC_LED_PERIOD      QDEC_LED_PERIOD
#define QDEC_LED_PERIOD_POS  (16U)
#define QDEC_LED_PERIOD_LEN  (9U)
#define QDEC_LED_PERIOD_MSK  (((1U << QDEC_LED_PERIOD_LEN) - 1) << QDEC_LED_PERIOD_POS)
#define QDEC_LED_PERIOD_UMSK (~(((1U << QDEC_LED_PERIOD_LEN) - 1) << QDEC_LED_PERIOD_POS))

/* 0x88 : qdec2_value */
#define QDEC2_VALUE_OFFSET (0x88)
#define QDEC_ACC1_VAL      QDEC_ACC1_VAL
#define QDEC_ACC1_VAL_POS  (0U)
#define QDEC_ACC1_VAL_LEN  (16U)
#define QDEC_ACC1_VAL_MSK  (((1U << QDEC_ACC1_VAL_LEN) - 1) << QDEC_ACC1_VAL_POS)
#define QDEC_ACC1_VAL_UMSK (~(((1U << QDEC_ACC1_VAL_LEN) - 1) << QDEC_ACC1_VAL_POS))
#define QDEC_ACC2_VAL      QDEC_ACC2_VAL
#define QDEC_ACC2_VAL_POS  (16U)
#define QDEC_ACC2_VAL_LEN  (4U)
#define QDEC_ACC2_VAL_MSK  (((1U << QDEC_ACC2_VAL_LEN) - 1) << QDEC_ACC2_VAL_POS)
#define QDEC_ACC2_VAL_UMSK (~(((1U << QDEC_ACC2_VAL_LEN) - 1) << QDEC_ACC2_VAL_POS))
#define QDEC_SPL_VAL       QDEC_SPL_VAL
#define QDEC_SPL_VAL_POS   (28U)
#define QDEC_SPL_VAL_LEN   (2U)
#define QDEC_SPL_VAL_MSK   (((1U << QDEC_SPL_VAL_LEN) - 1) << QDEC_SPL_VAL_POS)
#define QDEC_SPL_VAL_UMSK  (~(((1U << QDEC_SPL_VAL_LEN) - 1) << QDEC_SPL_VAL_POS))

/* 0x90 : qdec2_int_en */
#define QDEC2_INT_EN_OFFSET   (0x90)
#define QDEC_RPT_RDY_EN       QDEC_RPT_RDY_EN
#define QDEC_RPT_RDY_EN_POS   (0U)
#define QDEC_RPT_RDY_EN_LEN   (1U)
#define QDEC_RPT_RDY_EN_MSK   (((1U << QDEC_RPT_RDY_EN_LEN) - 1) << QDEC_RPT_RDY_EN_POS)
#define QDEC_RPT_RDY_EN_UMSK  (~(((1U << QDEC_RPT_RDY_EN_LEN) - 1) << QDEC_RPT_RDY_EN_POS))
#define QDEC_SPL_RDY_EN       QDEC_SPL_RDY_EN
#define QDEC_SPL_RDY_EN_POS   (1U)
#define QDEC_SPL_RDY_EN_LEN   (1U)
#define QDEC_SPL_RDY_EN_MSK   (((1U << QDEC_SPL_RDY_EN_LEN) - 1) << QDEC_SPL_RDY_EN_POS)
#define QDEC_SPL_RDY_EN_UMSK  (~(((1U << QDEC_SPL_RDY_EN_LEN) - 1) << QDEC_SPL_RDY_EN_POS))
#define QDEC_DBL_RDY_EN       QDEC_DBL_RDY_EN
#define QDEC_DBL_RDY_EN_POS   (2U)
#define QDEC_DBL_RDY_EN_LEN   (1U)
#define QDEC_DBL_RDY_EN_MSK   (((1U << QDEC_DBL_RDY_EN_LEN) - 1) << QDEC_DBL_RDY_EN_POS)
#define QDEC_DBL_RDY_EN_UMSK  (~(((1U << QDEC_DBL_RDY_EN_LEN) - 1) << QDEC_DBL_RDY_EN_POS))
#define QDEC_OVERFLOW_EN      QDEC_OVERFLOW_EN
#define QDEC_OVERFLOW_EN_POS  (3U)
#define QDEC_OVERFLOW_EN_LEN  (1U)
#define QDEC_OVERFLOW_EN_MSK  (((1U << QDEC_OVERFLOW_EN_LEN) - 1) << QDEC_OVERFLOW_EN_POS)
#define QDEC_OVERFLOW_EN_UMSK (~(((1U << QDEC_OVERFLOW_EN_LEN) - 1) << QDEC_OVERFLOW_EN_POS))

/* 0x94 : qdec2_int_sts */
#define QDEC2_INT_STS_OFFSET   (0x94)
#define QDEC_RPT_RDY_STS       QDEC_RPT_RDY_STS
#define QDEC_RPT_RDY_STS_POS   (0U)
#define QDEC_RPT_RDY_STS_LEN   (1U)
#define QDEC_RPT_RDY_STS_MSK   (((1U << QDEC_RPT_RDY_STS_LEN) - 1) << QDEC_RPT_RDY_STS_POS)
#define QDEC_RPT_RDY_STS_UMSK  (~(((1U << QDEC_RPT_RDY_STS_LEN) - 1) << QDEC_RPT_RDY_STS_POS))
#define QDEC_SPL_RDY_STS       QDEC_SPL_RDY_STS
#define QDEC_SPL_RDY_STS_POS   (1U)
#define QDEC_SPL_RDY_STS_LEN   (1U)
#define QDEC_SPL_RDY_STS_MSK   (((1U << QDEC_SPL_RDY_STS_LEN) - 1) << QDEC_SPL_RDY_STS_POS)
#define QDEC_SPL_RDY_STS_UMSK  (~(((1U << QDEC_SPL_RDY_STS_LEN) - 1) << QDEC_SPL_RDY_STS_POS))
#define QDEC_DBL_RDY_STS       QDEC_DBL_RDY_STS
#define QDEC_DBL_RDY_STS_POS   (2U)
#define QDEC_DBL_RDY_STS_LEN   (1U)
#define QDEC_DBL_RDY_STS_MSK   (((1U << QDEC_DBL_RDY_STS_LEN) - 1) << QDEC_DBL_RDY_STS_POS)
#define QDEC_DBL_RDY_STS_UMSK  (~(((1U << QDEC_DBL_RDY_STS_LEN) - 1) << QDEC_DBL_RDY_STS_POS))
#define QDEC_OVERFLOW_STS      QDEC_OVERFLOW_STS
#define QDEC_OVERFLOW_STS_POS  (3U)
#define QDEC_OVERFLOW_STS_LEN  (1U)
#define QDEC_OVERFLOW_STS_MSK  (((1U << QDEC_OVERFLOW_STS_LEN) - 1) << QDEC_OVERFLOW_STS_POS)
#define QDEC_OVERFLOW_STS_UMSK (~(((1U << QDEC_OVERFLOW_STS_LEN) - 1) << QDEC_OVERFLOW_STS_POS))

/* 0x98 : qdec2_int_clr */
#define QDEC2_INT_CLR_OFFSET   (0x98)
#define QDEC_RPT_RDY_CLR       QDEC_RPT_RDY_CLR
#define QDEC_RPT_RDY_CLR_POS   (0U)
#define QDEC_RPT_RDY_CLR_LEN   (1U)
#define QDEC_RPT_RDY_CLR_MSK   (((1U << QDEC_RPT_RDY_CLR_LEN) - 1) << QDEC_RPT_RDY_CLR_POS)
#define QDEC_RPT_RDY_CLR_UMSK  (~(((1U << QDEC_RPT_RDY_CLR_LEN) - 1) << QDEC_RPT_RDY_CLR_POS))
#define QDEC_SPL_RDY_CLR       QDEC_SPL_RDY_CLR
#define QDEC_SPL_RDY_CLR_POS   (1U)
#define QDEC_SPL_RDY_CLR_LEN   (1U)
#define QDEC_SPL_RDY_CLR_MSK   (((1U << QDEC_SPL_RDY_CLR_LEN) - 1) << QDEC_SPL_RDY_CLR_POS)
#define QDEC_SPL_RDY_CLR_UMSK  (~(((1U << QDEC_SPL_RDY_CLR_LEN) - 1) << QDEC_SPL_RDY_CLR_POS))
#define QDEC_DBL_RDY_CLR       QDEC_DBL_RDY_CLR
#define QDEC_DBL_RDY_CLR_POS   (2U)
#define QDEC_DBL_RDY_CLR_LEN   (1U)
#define QDEC_DBL_RDY_CLR_MSK   (((1U << QDEC_DBL_RDY_CLR_LEN) - 1) << QDEC_DBL_RDY_CLR_POS)
#define QDEC_DBL_RDY_CLR_UMSK  (~(((1U << QDEC_DBL_RDY_CLR_LEN) - 1) << QDEC_DBL_RDY_CLR_POS))
#define QDEC_OVERFLOW_CLR      QDEC_OVERFLOW_CLR
#define QDEC_OVERFLOW_CLR_POS  (3U)
#define QDEC_OVERFLOW_CLR_LEN  (1U)
#define QDEC_OVERFLOW_CLR_MSK  (((1U << QDEC_OVERFLOW_CLR_LEN) - 1) << QDEC_OVERFLOW_CLR_POS)
#define QDEC_OVERFLOW_CLR_UMSK (~(((1U << QDEC_OVERFLOW_CLR_LEN) - 1) << QDEC_OVERFLOW_CLR_POS))

struct qdec_reg {
    /* 0x0 : qdec0_ctrl0 */
    union {
        struct
        {
            uint32_t qdec_en        : 1;  /* [    0],        r/w,        0x0 */
            uint32_t led_en         : 1;  /* [    1],        r/w,        0x0 */
            uint32_t led_pol        : 1;  /* [    2],        r/w,        0x1 */
            uint32_t deg_en         : 1;  /* [    3],        r/w,        0x0 */
            uint32_t deg_cnt        : 4;  /* [ 7: 4],        r/w,        0x0 */
            uint32_t spl_period     : 4;  /* [11: 8],        r/w,        0x2 */
            uint32_t rpt_period     : 16; /* [27:12],        r/w,        0xa */
            uint32_t reserved_28_31 : 4;  /* [31:28],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } qdec0_ctrl0;

    /* 0x4 : qdec0_ctrl1 */
    union {
        struct
        {
            uint32_t acc_mode       : 1;  /* [    0],        r/w,        0x1 */
            uint32_t spl_mode       : 1;  /* [    1],        r/w,        0x0 */
            uint32_t rpt_mode       : 1;  /* [    2],        r/w,        0x0 */
            uint32_t input_swap     : 1;  /* [    3],        r/w,        0x0 */
            uint32_t reserved_4_15  : 12; /* [15: 4],       rsvd,        0x0 */
            uint32_t led_period     : 9;  /* [24:16],        r/w,        0x0 */
            uint32_t reserved_25_31 : 7;  /* [31:25],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } qdec0_ctrl1;

    /* 0x8 : qdec0_value */
    union {
        struct
        {
            uint32_t acc1_val       : 16; /* [15: 0],          r,        0x0 */
            uint32_t acc2_val       : 4;  /* [19:16],          r,        0x0 */
            uint32_t reserved_20_27 : 8;  /* [27:20],       rsvd,        0x0 */
            uint32_t spl_val        : 2;  /* [29:28],          r,        0x0 */
            uint32_t reserved_30_31 : 2;  /* [31:30],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } qdec0_value;

    /* 0xc  reserved */
    uint8_t RESERVED0xc[4];

    /* 0x10 : qdec0_int_en */
    union {
        struct
        {
            uint32_t rpt_rdy_en    : 1;  /* [    0],        r/w,        0x1 */
            uint32_t spl_rdy_en    : 1;  /* [    1],        r/w,        0x0 */
            uint32_t dbl_rdy_en    : 1;  /* [    2],        r/w,        0x0 */
            uint32_t overflow_en   : 1;  /* [    3],        r/w,        0x0 */
            uint32_t reserved_4_31 : 28; /* [31: 4],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } qdec0_int_en;

    /* 0x14 : qdec0_int_sts */
    union {
        struct
        {
            uint32_t rpt_rdy_sts   : 1;  /* [    0],          r,        0x0 */
            uint32_t spl_rdy_sts   : 1;  /* [    1],          r,        0x0 */
            uint32_t dbl_rdy_sts   : 1;  /* [    2],          r,        0x0 */
            uint32_t overflow_sts  : 1;  /* [    3],          r,        0x0 */
            uint32_t reserved_4_31 : 28; /* [31: 4],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } qdec0_int_sts;

    /* 0x18 : qdec0_int_clr */
    union {
        struct
        {
            uint32_t rpt_rdy_clr   : 1;  /* [    0],        w1c,        0x0 */
            uint32_t spl_rdy_clr   : 1;  /* [    1],        w1c,        0x0 */
            uint32_t dbl_rdy_clr   : 1;  /* [    2],        w1c,        0x0 */
            uint32_t overflow_clr  : 1;  /* [    3],        w1c,        0x0 */
            uint32_t reserved_4_31 : 28; /* [31: 4],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } qdec0_int_clr;

    /* 0x1c  reserved */
    uint8_t RESERVED0x1c[36];

    /* 0x40 : qdec1_ctrl0 */
    union {
        struct
        {
            uint32_t qdec_en        : 1;  /* [    0],        r/w,        0x0 */
            uint32_t led_en         : 1;  /* [    1],        r/w,        0x0 */
            uint32_t led_pol        : 1;  /* [    2],        r/w,        0x1 */
            uint32_t deg_en         : 1;  /* [    3],        r/w,        0x0 */
            uint32_t deg_cnt        : 4;  /* [ 7: 4],        r/w,        0x0 */
            uint32_t spl_period     : 4;  /* [11: 8],        r/w,        0x2 */
            uint32_t rpt_period     : 16; /* [27:12],        r/w,        0xa */
            uint32_t reserved_28_31 : 4;  /* [31:28],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } qdec1_ctrl0;

    /* 0x44 : qdec1_ctrl1 */
    union {
        struct
        {
            uint32_t acc_mode       : 1;  /* [    0],        r/w,        0x1 */
            uint32_t spl_mode       : 1;  /* [    1],        r/w,        0x0 */
            uint32_t rpt_mode       : 1;  /* [    2],        r/w,        0x0 */
            uint32_t input_swap     : 1;  /* [    3],        r/w,        0x0 */
            uint32_t reserved_4_15  : 12; /* [15: 4],       rsvd,        0x0 */
            uint32_t led_period     : 9;  /* [24:16],        r/w,        0x0 */
            uint32_t reserved_25_31 : 7;  /* [31:25],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } qdec1_ctrl1;

    /* 0x48 : qdec1_value */
    union {
        struct
        {
            uint32_t acc1_val       : 16; /* [15: 0],          r,        0x0 */
            uint32_t acc2_val       : 4;  /* [19:16],          r,        0x0 */
            uint32_t reserved_20_27 : 8;  /* [27:20],       rsvd,        0x0 */
            uint32_t spl_val        : 2;  /* [29:28],          r,        0x0 */
            uint32_t reserved_30_31 : 2;  /* [31:30],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } qdec1_value;

    /* 0x4c  reserved */
    uint8_t RESERVED0x4c[4];

    /* 0x50 : qdec1_int_en */
    union {
        struct
        {
            uint32_t rpt_rdy_en    : 1;  /* [    0],        r/w,        0x1 */
            uint32_t spl_rdy_en    : 1;  /* [    1],        r/w,        0x0 */
            uint32_t dbl_rdy_en    : 1;  /* [    2],        r/w,        0x0 */
            uint32_t overflow_en   : 1;  /* [    3],        r/w,        0x0 */
            uint32_t reserved_4_31 : 28; /* [31: 4],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } qdec1_int_en;

    /* 0x54 : qdec1_int_sts */
    union {
        struct
        {
            uint32_t rpt_rdy_sts   : 1;  /* [    0],          r,        0x0 */
            uint32_t spl_rdy_sts   : 1;  /* [    1],          r,        0x0 */
            uint32_t dbl_rdy_sts   : 1;  /* [    2],          r,        0x0 */
            uint32_t overflow_sts  : 1;  /* [    3],          r,        0x0 */
            uint32_t reserved_4_31 : 28; /* [31: 4],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } qdec1_int_sts;

    /* 0x58 : qdec1_int_clr */
    union {
        struct
        {
            uint32_t rpt_rdy_clr   : 1;  /* [    0],        w1c,        0x0 */
            uint32_t spl_rdy_clr   : 1;  /* [    1],        w1c,        0x0 */
            uint32_t dbl_rdy_clr   : 1;  /* [    2],        w1c,        0x0 */
            uint32_t overflow_clr  : 1;  /* [    3],        w1c,        0x0 */
            uint32_t reserved_4_31 : 28; /* [31: 4],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } qdec1_int_clr;

    /* 0x5c  reserved */
    uint8_t RESERVED0x5c[36];

    /* 0x80 : qdec2_ctrl0 */
    union {
        struct
        {
            uint32_t qdec_en        : 1;  /* [    0],        r/w,        0x0 */
            uint32_t led_en         : 1;  /* [    1],        r/w,        0x0 */
            uint32_t led_pol        : 1;  /* [    2],        r/w,        0x1 */
            uint32_t deg_en         : 1;  /* [    3],        r/w,        0x0 */
            uint32_t deg_cnt        : 4;  /* [ 7: 4],        r/w,        0x0 */
            uint32_t spl_period     : 4;  /* [11: 8],        r/w,        0x2 */
            uint32_t rpt_period     : 16; /* [27:12],        r/w,        0xa */
            uint32_t reserved_28_31 : 4;  /* [31:28],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } qdec2_ctrl0;

    /* 0x84 : qdec2_ctrl1 */
    union {
        struct
        {
            uint32_t acc_mode       : 1;  /* [    0],        r/w,        0x1 */
            uint32_t spl_mode       : 1;  /* [    1],        r/w,        0x0 */
            uint32_t rpt_mode       : 1;  /* [    2],        r/w,        0x0 */
            uint32_t input_swap     : 1;  /* [    3],        r/w,        0x0 */
            uint32_t reserved_4_15  : 12; /* [15: 4],       rsvd,        0x0 */
            uint32_t led_period     : 9;  /* [24:16],        r/w,        0x0 */
            uint32_t reserved_25_31 : 7;  /* [31:25],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } qdec2_ctrl1;

    /* 0x88 : qdec2_value */
    union {
        struct
        {
            uint32_t acc1_val       : 16; /* [15: 0],          r,        0x0 */
            uint32_t acc2_val       : 4;  /* [19:16],          r,        0x0 */
            uint32_t reserved_20_27 : 8;  /* [27:20],       rsvd,        0x0 */
            uint32_t spl_val        : 2;  /* [29:28],          r,        0x0 */
            uint32_t reserved_30_31 : 2;  /* [31:30],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } qdec2_value;

    /* 0x8c  reserved */
    uint8_t RESERVED0x8c[4];

    /* 0x90 : qdec2_int_en */
    union {
        struct
        {
            uint32_t rpt_rdy_en    : 1;  /* [    0],        r/w,        0x1 */
            uint32_t spl_rdy_en    : 1;  /* [    1],        r/w,        0x0 */
            uint32_t dbl_rdy_en    : 1;  /* [    2],        r/w,        0x0 */
            uint32_t overflow_en   : 1;  /* [    3],        r/w,        0x0 */
            uint32_t reserved_4_31 : 28; /* [31: 4],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } qdec2_int_en;

    /* 0x94 : qdec2_int_sts */
    union {
        struct
        {
            uint32_t rpt_rdy_sts   : 1;  /* [    0],          r,        0x0 */
            uint32_t spl_rdy_sts   : 1;  /* [    1],          r,        0x0 */
            uint32_t dbl_rdy_sts   : 1;  /* [    2],          r,        0x0 */
            uint32_t overflow_sts  : 1;  /* [    3],          r,        0x0 */
            uint32_t reserved_4_31 : 28; /* [31: 4],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } qdec2_int_sts;

    /* 0x98 : qdec2_int_clr */
    union {
        struct
        {
            uint32_t rpt_rdy_clr   : 1;  /* [    0],        w1c,        0x0 */
            uint32_t spl_rdy_clr   : 1;  /* [    1],        w1c,        0x0 */
            uint32_t dbl_rdy_clr   : 1;  /* [    2],        w1c,        0x0 */
            uint32_t overflow_clr  : 1;  /* [    3],        w1c,        0x0 */
            uint32_t reserved_4_31 : 28; /* [31: 4],       rsvd,        0x0 */
        } BF;
        uint32_t WORD;
    } qdec2_int_clr;
};

typedef volatile struct qdec_reg qdec_reg_t;

#endif /* __QDEC_REG_H__ */
