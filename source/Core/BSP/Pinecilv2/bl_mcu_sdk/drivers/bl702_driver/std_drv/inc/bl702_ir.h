/**
  ******************************************************************************
  * @file    bl702_ir.h
  * @version V1.0
  * @date
  * @brief   This file is the standard driver header file
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
#ifndef __BL702_IR_H__
#define __BL702_IR_H__

#include "ir_reg.h"
#include "bl702_common.h"
#include "bl702_hbn.h"

/** @addtogroup  BL702_Peripheral_Driver
 *  @{
 */

/** @addtogroup  IR
 *  @{
 */

/** @defgroup  IR_Public_Types
 *  @{
 */

/**
 *  @brief IR direction type definition
 */
typedef enum {
    IR_TX,   /*!< IR TX direction */
    IR_RX,   /*!< IR RX direction */
    IR_TXRX, /*!< IR TX and RX direction */
} IR_Direction_Type;

/**
 *  @brief IR word type definition
 */
typedef enum {
    IR_WORD_0, /*!< IR data word 0 */
    IR_WORD_1, /*!< IR data word 1 */
} IR_Word_Type;

/**
 *  @brief IR RX mode type definition
 */
typedef enum {
    IR_RX_NEC, /*!< IR RX NEC mode */
    IR_RX_RC5, /*!< IR RX RC5 mode */
    IR_RX_SWM, /*!< IR RX software pulse-width detection mode */
} IR_RxMode_Type;

/**
 *  @brief IR interrupt type definition
 */
typedef enum {
    IR_INT_TX,  /*!< IR TX transfer end interrupt */
    IR_INT_RX,  /*!< IR RX transfer end interrupt */
    IR_INT_ALL, /*!< IR all interrupt */
} IR_INT_Type;

/**
 *  @brief IR fifo underflow or overflow type definition
 */
typedef enum {
    IR_RX_FIFO_UNDERFLOW, /*!< Underflow flag of rx fifo */
    IR_RX_FIFO_OVERFLOW,  /*!< Overflow flag of rx fifo */
} IR_FifoStatus_Type;

/**
 *  @brief IR TX configuration structure type definition
 */
typedef struct
{
    uint8_t dataBits;               /*!< Bit count of data phase */
    BL_Fun_Type tailPulseInverse;   /*!< Enable or disable signal of tail pulse inverse (don't care if SWM is enabled) */
    BL_Fun_Type tailPulse;          /*!< Enable or disable signal of tail pulse (don't care if SWM is enabled) */
    BL_Fun_Type headPulseInverse;   /*!< Enable or disable signal of head pulse inverse (don't care if SWM is enabled) */
    BL_Fun_Type headPulse;          /*!< Enable or disable signal of head pulse (don't care if SWM is enabled) */
    BL_Fun_Type logic1PulseInverse; /*!< Enable or disable signal of logic 1 pulse inverse (don't care if SWM is enabled) */
    BL_Fun_Type logic0PulseInverse; /*!< Enable or disable signal of logic 0 pulse inverse (don't care if SWM is enabled) */
    BL_Fun_Type dataPulse;          /*!< Enable or disable signal of data pulse (don't care if SWM is enabled) */
    BL_Fun_Type outputModulation;   /*!< Enable or disable signal of output modulation */
    BL_Fun_Type outputInverse;      /*!< Enable or disable signal of output inverse,0:output stays at low during idle state,1:stay at high */
} IR_TxCfg_Type;

/**
 *  @brief IR TX pulse width configuration structure type definition
 */
typedef struct
{
    uint8_t logic0PulseWidth_1; /*!< Pulse width of logic 0 pulse phase 1 (don't care if SWM is enabled) */
    uint8_t logic0PulseWidth_0; /*!< Pulse width of logic 0 pulse phase 0 (don't care if SWM is enabled) */
    uint8_t logic1PulseWidth_1; /*!< Pulse width of logic 1 pulse phase 1 (don't care if SWM is enabled) */
    uint8_t logic1PulseWidth_0; /*!< Pulse width of logic 1 pulse phase 0 (don't care if SWM is enabled) */
    uint8_t headPulseWidth_1;   /*!< Pulse width of head pulse phase 1 (don't care if SWM is enabled) */
    uint8_t headPulseWidth_0;   /*!< Pulse width of head pulse phase 0 (don't care if SWM is enabled) */
    uint8_t tailPulseWidth_1;   /*!< Pulse width of tail pulse phase 1 (don't care if SWM is enabled) */
    uint8_t tailPulseWidth_0;   /*!< Pulse width of tail pulse phase 0 (don't care if SWM is enabled) */
    uint8_t moduWidth_1;        /*!< Modulation phase 1 width */
    uint8_t moduWidth_0;        /*!< Modulation phase 0 width */
    uint16_t pulseWidthUnit;    /*!< Pulse width unit */
} IR_TxPulseWidthCfg_Type;

/**
 *  @brief IR TX software mode pulse width(multiples of pulse width unit) configuration structure type definition
 */
typedef struct
{
    uint32_t swmData0; /*!< SWM pulse width data   0~7,multiples of pulse width unit,each pulse is represented by 4-bit */
    uint32_t swmData1; /*!< SWM pulse width data  8~15,multiples of pulse width unit,each pulse is represented by 4-bit */
    uint32_t swmData2; /*!< SWM pulse width data 16~23,multiples of pulse width unit,each pulse is represented by 4-bit */
    uint32_t swmData3; /*!< SWM pulse width data 24~31,multiples of pulse width unit,each pulse is represented by 4-bit */
    uint32_t swmData4; /*!< SWM pulse width data 32~39,multiples of pulse width unit,each pulse is represented by 4-bit */
    uint32_t swmData5; /*!< SWM pulse width data 40~47,multiples of pulse width unit,each pulse is represented by 4-bit */
    uint32_t swmData6; /*!< SWM pulse width data 48~55,multiples of pulse width unit,each pulse is represented by 4-bit */
    uint32_t swmData7; /*!< SWM pulse width data 56~63,multiples of pulse width unit,each pulse is represented by 4-bit */
} IR_TxSWMPulseWidthCfg_Type;

/**
 *  @brief IR RX configuration structure type definition
 */
typedef struct
{
    IR_RxMode_Type rxMode;    /*!< Set ir rx mode */
    BL_Fun_Type inputInverse; /*!< Enable or disable signal of input inverse */
    uint16_t endThreshold;    /*!< Pulse width threshold to trigger end condition */
    uint16_t dataThreshold;   /*!< Pulse width threshold for logic 0/1 detection (don't care if SWM is enabled) */
    BL_Fun_Type rxDeglitch;   /*!< Enable or disable signal of rx input de-glitch function */
    uint8_t DeglitchCnt;      /*!< De-glitch function cycle count */
} IR_RxCfg_Type;

/*@} end of group IR_Public_Types */

/** @defgroup  IR_Public_Constants
 *  @{
 */

/** @defgroup  IR_DIRECTION_TYPE
 *  @{
 */
#define IS_IR_DIRECTION_TYPE(type) (((type) == IR_TX) || \
                                    ((type) == IR_RX) || \
                                    ((type) == IR_TXRX))

/** @defgroup  IR_WORD_TYPE
 *  @{
 */
#define IS_IR_WORD_TYPE(type) (((type) == IR_WORD_0) || \
                               ((type) == IR_WORD_1))

/** @defgroup  IR_RXMODE_TYPE
 *  @{
 */
#define IS_IR_RXMODE_TYPE(type) (((type) == IR_RX_NEC) || \
                                 ((type) == IR_RX_RC5) || \
                                 ((type) == IR_RX_SWM))

/** @defgroup  IR_INT_TYPE
 *  @{
 */
#define IS_IR_INT_TYPE(type) (((type) == IR_INT_TX) || \
                              ((type) == IR_INT_RX) || \
                              ((type) == IR_INT_ALL))

/** @defgroup  IR_FIFOSTATUS_TYPE
 *  @{
 */
#define IS_IR_FIFOSTATUS_TYPE(type) (((type) == IR_RX_FIFO_UNDERFLOW) || \
                                     ((type) == IR_RX_FIFO_OVERFLOW))

/*@} end of group IR_Public_Constants */

/** @defgroup  IR_Public_Macros
 *  @{
 */
#define IR_RX_FIFO_SIZE 64

/*@} end of group IR_Public_Macros */

/** @defgroup  IR_Public_Functions
 *  @{
 */

/**
 *  @brief IR Functions
 */
#ifndef BFLB_USE_HAL_DRIVER
void IRRX_IRQHandler(void);
void IRTX_IRQHandler(void);
#endif
BL_Err_Type IR_TxInit(IR_TxCfg_Type *irTxCfg);
BL_Err_Type IR_TxPulseWidthConfig(IR_TxPulseWidthCfg_Type *irTxPulseWidthCfg);
BL_Err_Type IR_TxSWMPulseWidthConfig(IR_TxSWMPulseWidthCfg_Type *irTxSWMPulseWidthCfg);
BL_Err_Type IR_RxInit(IR_RxCfg_Type *irRxCfg);
BL_Err_Type IR_DeInit(void);
BL_Err_Type IR_Enable(IR_Direction_Type direct);
BL_Err_Type IR_Disable(IR_Direction_Type direct);
BL_Err_Type IR_TxSWM(BL_Fun_Type txSWM);
BL_Err_Type IR_RxFIFOClear(void);
BL_Err_Type IR_SendData(IR_Word_Type irWord, uint32_t data);
BL_Err_Type IR_SWMSendData(uint16_t *data, uint8_t length);
BL_Err_Type IR_SendCommand(uint32_t word1, uint32_t word0);
BL_Err_Type IR_SWMSendCommand(uint16_t *data, uint8_t length);
BL_Err_Type IR_SendNEC(uint8_t address, uint8_t command);
BL_Err_Type IR_IntMask(IR_INT_Type intType, BL_Mask_Type intMask);
BL_Err_Type IR_ClrIntStatus(IR_INT_Type intType);
BL_Err_Type IR_Int_Callback_Install(IR_INT_Type intType, intCallback_Type *cbFun);
BL_Sts_Type IR_GetIntStatus(IR_INT_Type intType);
BL_Sts_Type IR_GetRxFIFOStatus(IR_FifoStatus_Type fifoSts);
uint32_t IR_ReceiveData(IR_Word_Type irWord);
uint8_t IR_SWMReceiveData(uint16_t *data, uint8_t length);
BL_Err_Type IR_ReceiveNEC(uint8_t *address, uint8_t *command);
uint8_t IR_GetRxDataBitCount(void);
uint8_t IR_GetRxFIFOCount(void);
IR_RxMode_Type IR_LearnToInit(uint32_t *data, uint8_t *length);
BL_Err_Type IR_LearnToSend(IR_RxMode_Type mode, uint32_t *data, uint8_t length);
uint8_t IR_LearnToReceive(IR_RxMode_Type mode, uint32_t *data);
BL_Err_Type IR_LEDInit(HBN_XCLK_CLK_Type clk, uint8_t div, uint8_t unit, uint8_t code0H, uint8_t code0L, uint8_t code1H,
                       uint8_t code1L);
BL_Err_Type IR_LEDSend(uint32_t data);

/*@} end of group IR_Public_Functions */

/*@} end of group IR */

/*@} end of group BL702_Peripheral_Driver */

#endif /* __BL702_IR_H__ */
