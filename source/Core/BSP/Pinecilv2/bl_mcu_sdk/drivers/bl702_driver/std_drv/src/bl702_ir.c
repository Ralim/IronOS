/**
  ******************************************************************************
  * @file    bl702_ir.c
  * @version V1.0
  * @date
  * @brief   This file is the standard driver c file
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

#include "bl702_ir.h"
#include "bl702_glb.h"

/** @addtogroup  BL702_Peripheral_Driver
 *  @{
 */

/** @addtogroup  IR
 *  @{
 */

/** @defgroup  IR_Private_Macros
 *  @{
 */
#define NEC_HEAD_H_MIN          17000
#define NEC_HEAD_H_MAX          19000
#define NEC_HEAD_L_MIN          8400
#define NEC_HEAD_L_MAX          9600
#define NEC_BIT0_H_MIN          525
#define NEC_BIT0_H_MAX          1725
#define RC5_ONE_PLUSE_MIN       1175
#define RC5_ONE_PLUSE_MAX       2375
#define RC5_TWO_PLUSE_MIN       2955
#define RC5_TWO_PLUSE_MAX       4155
#define IR_TX_INT_TIMEOUT_COUNT (100 * 160 * 1000)
#define IR_RX_INT_TIMEOUT_COUNT (100 * 160 * 1000)

/*@} end of group IR_Private_Macros */

/** @defgroup  IR_Private_Types
 *  @{
 */

/*@} end of group IR_Private_Types */

/** @defgroup  IR_Private_Variables
 *  @{
 */
static intCallback_Type *irIntCbfArra[IR_INT_ALL] = { NULL, NULL };

/*@} end of group IR_Private_Variables */

/** @defgroup  IR_Global_Variables
 *  @{
 */

/*@} end of group IR_Global_Variables */

/** @defgroup  IR_Private_Fun_Declaration
 *  @{
 */

/*@} end of group IR_Private_Fun_Declaration */

/** @defgroup  IR_Private_Functions
 *  @{
 */

/*@} end of group IR_Private_Functions */

/** @defgroup  IR_Public_Functions
 *  @{
 */

/****************************************************************************/ /**
 * @brief  IR RX IRQ handler function
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
#ifndef BFLB_USE_HAL_DRIVER
void IRRX_IRQHandler(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(IR_BASE, IRRX_INT_STS);

    if (BL_IS_REG_BIT_SET(tmpVal, IRRX_END_INT) && !BL_IS_REG_BIT_SET(tmpVal, IR_CR_IRRX_END_MASK)) {
        BL_WR_REG(IR_BASE, IRRX_INT_STS, BL_SET_REG_BIT(tmpVal, IR_CR_IRRX_END_CLR));

        if (irIntCbfArra[IR_INT_RX] != NULL) {
            irIntCbfArra[IR_INT_RX]();
        }
    }
}
#endif

/****************************************************************************/ /**
 * @brief  IR TX IRQ handler function
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
#ifndef BFLB_USE_HAL_DRIVER
void IRTX_IRQHandler(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(IR_BASE, IRTX_INT_STS);

    if (BL_IS_REG_BIT_SET(tmpVal, IRTX_END_INT) && !BL_IS_REG_BIT_SET(tmpVal, IR_CR_IRTX_END_MASK)) {
        BL_WR_REG(IR_BASE, IRTX_INT_STS, BL_SET_REG_BIT(tmpVal, IR_CR_IRTX_END_CLR));

        if (irIntCbfArra[IR_INT_TX] != NULL) {
            irIntCbfArra[IR_INT_TX]();
        }
    }
}
#endif

/****************************************************************************/ /**
 * @brief  IR tx initialization function
 *
 * @param  irTxCfg: IR tx configuration structure pointer
 *
 * @return SUCCESS
 *
*******************************************************************************/
BL_Err_Type IR_TxInit(IR_TxCfg_Type *irTxCfg)
{
    uint32_t tmpVal;

    /* Disable clock gate */
    GLB_AHB_Slave1_Clock_Gate(DISABLE, BL_AHB_SLAVE1_IRR);

    tmpVal = BL_RD_REG(IR_BASE, IRTX_CONFIG);
    /* Set data bit */
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, IR_CR_IRTX_DATA_NUM, irTxCfg->dataBits - 1);
    /* Set tail pulse */
    ENABLE == irTxCfg->tailPulseInverse ? (tmpVal = BL_SET_REG_BIT(tmpVal, IR_CR_IRTX_TAIL_HL_INV)) : (tmpVal = BL_CLR_REG_BIT(tmpVal, IR_CR_IRTX_TAIL_HL_INV));
    ENABLE == irTxCfg->tailPulse ? (tmpVal = BL_SET_REG_BIT(tmpVal, IR_CR_IRTX_TAIL_EN)) : (tmpVal = BL_CLR_REG_BIT(tmpVal, IR_CR_IRTX_TAIL_EN));
    /* Set head pulse */
    ENABLE == irTxCfg->headPulseInverse ? (tmpVal = BL_SET_REG_BIT(tmpVal, IR_CR_IRTX_HEAD_HL_INV)) : (tmpVal = BL_CLR_REG_BIT(tmpVal, IR_CR_IRTX_HEAD_HL_INV));
    ENABLE == irTxCfg->headPulse ? (tmpVal = BL_SET_REG_BIT(tmpVal, IR_CR_IRTX_HEAD_EN)) : (tmpVal = BL_CLR_REG_BIT(tmpVal, IR_CR_IRTX_HEAD_EN));
    /* Enable or disable logic 1 and 0 pulse inverse */
    ENABLE == irTxCfg->logic1PulseInverse ? (tmpVal = BL_SET_REG_BIT(tmpVal, IR_CR_IRTX_LOGIC1_HL_INV)) : (tmpVal = BL_CLR_REG_BIT(tmpVal, IR_CR_IRTX_LOGIC1_HL_INV));
    ENABLE == irTxCfg->logic0PulseInverse ? (tmpVal = BL_SET_REG_BIT(tmpVal, IR_CR_IRTX_LOGIC0_HL_INV)) : (tmpVal = BL_CLR_REG_BIT(tmpVal, IR_CR_IRTX_LOGIC0_HL_INV));
    /* Enable or disable data pulse */
    ENABLE == irTxCfg->dataPulse ? (tmpVal = BL_SET_REG_BIT(tmpVal, IR_CR_IRTX_DATA_EN)) : (tmpVal = BL_CLR_REG_BIT(tmpVal, IR_CR_IRTX_DATA_EN));
    /* Enable or disable output modulation */
    ENABLE == irTxCfg->outputModulation ? (tmpVal = BL_SET_REG_BIT(tmpVal, IR_CR_IRTX_MOD_EN)) : (tmpVal = BL_CLR_REG_BIT(tmpVal, IR_CR_IRTX_MOD_EN));
    /* Enable or disable output inverse */
    ENABLE == irTxCfg->outputInverse ? (tmpVal = BL_SET_REG_BIT(tmpVal, IR_CR_IRTX_OUT_INV)) : (tmpVal = BL_CLR_REG_BIT(tmpVal, IR_CR_IRTX_OUT_INV));

    /* Write back */
    BL_WR_REG(IR_BASE, IRTX_CONFIG, tmpVal);

#ifndef BFLB_USE_HAL_DRIVER
    Interrupt_Handler_Register(IRTX_IRQn, IRTX_IRQHandler);
#endif
    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  IR tx pulse width configure function
 *
 * @param  irTxPulseWidthCfg: IR tx pulse width configuration structure pointer
 *
 * @return SUCCESS
 *
*******************************************************************************/
BL_Err_Type IR_TxPulseWidthConfig(IR_TxPulseWidthCfg_Type *irTxPulseWidthCfg)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(IR_BASE, IRTX_PW);
    /* Set logic 0 pulse phase 0 width */
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, IR_CR_IRTX_LOGIC0_PH0_W, irTxPulseWidthCfg->logic0PulseWidth_0 - 1);
    /* Set logic 0 pulse phase 1 width */
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, IR_CR_IRTX_LOGIC0_PH1_W, irTxPulseWidthCfg->logic0PulseWidth_1 - 1);
    /* Set logic 1 pulse phase 0 width */
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, IR_CR_IRTX_LOGIC1_PH0_W, irTxPulseWidthCfg->logic1PulseWidth_0 - 1);
    /* Set logic 1 pulse phase 1 width */
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, IR_CR_IRTX_LOGIC1_PH1_W, irTxPulseWidthCfg->logic1PulseWidth_1 - 1);
    /* Set head pulse phase 0 width */
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, IR_CR_IRTX_HEAD_PH0_W, irTxPulseWidthCfg->headPulseWidth_0 - 1);
    /* Set head pulse phase 1 width */
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, IR_CR_IRTX_HEAD_PH1_W, irTxPulseWidthCfg->headPulseWidth_1 - 1);
    /* Set tail pulse phase 0 width */
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, IR_CR_IRTX_TAIL_PH0_W, irTxPulseWidthCfg->tailPulseWidth_0 - 1);
    /* Set tail pulse phase 1 width */
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, IR_CR_IRTX_TAIL_PH1_W, irTxPulseWidthCfg->tailPulseWidth_1 - 1);
    BL_WR_REG(IR_BASE, IRTX_PW, tmpVal);

    tmpVal = BL_RD_REG(IR_BASE, IRTX_PULSE_WIDTH);
    /* Set modulation phase 0 width */
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, IR_CR_IRTX_MOD_PH0_W, irTxPulseWidthCfg->moduWidth_0 - 1);
    /* Set modulation phase 1 width */
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, IR_CR_IRTX_MOD_PH1_W, irTxPulseWidthCfg->moduWidth_1 - 1);
    /* Set pulse width unit */
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, IR_CR_IRTX_PW_UNIT, irTxPulseWidthCfg->pulseWidthUnit - 1);
    BL_WR_REG(IR_BASE, IRTX_PULSE_WIDTH, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  IR tx software mode pulse width(multiples of pulse width unit) configure function
 *
 * @param  irTxSWMPulseWidthCfg: IR tx software mode pulse width configuration structure pointer
 *
 * @return SUCCESS
 *
*******************************************************************************/
BL_Err_Type IR_TxSWMPulseWidthConfig(IR_TxSWMPulseWidthCfg_Type *irTxSWMPulseWidthCfg)
{
    /* Set swm pulse width,multiples of pulse width unit */
    BL_WR_REG(IR_BASE, IRTX_SWM_PW_0, irTxSWMPulseWidthCfg->swmData0);
    BL_WR_REG(IR_BASE, IRTX_SWM_PW_1, irTxSWMPulseWidthCfg->swmData1);
    BL_WR_REG(IR_BASE, IRTX_SWM_PW_2, irTxSWMPulseWidthCfg->swmData2);
    BL_WR_REG(IR_BASE, IRTX_SWM_PW_3, irTxSWMPulseWidthCfg->swmData3);
    BL_WR_REG(IR_BASE, IRTX_SWM_PW_4, irTxSWMPulseWidthCfg->swmData4);
    BL_WR_REG(IR_BASE, IRTX_SWM_PW_5, irTxSWMPulseWidthCfg->swmData5);
    BL_WR_REG(IR_BASE, IRTX_SWM_PW_6, irTxSWMPulseWidthCfg->swmData6);
    BL_WR_REG(IR_BASE, IRTX_SWM_PW_7, irTxSWMPulseWidthCfg->swmData7);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  IR rx initialization function
 *
 * @param  irRxCfg: IR rx configuration structure pointer
 *
 * @return SUCCESS
 *
*******************************************************************************/
BL_Err_Type IR_RxInit(IR_RxCfg_Type *irRxCfg)
{
    uint32_t tmpVal;

    /* Check the parameters */
    CHECK_PARAM(IS_IR_RXMODE_TYPE(irRxCfg->rxMode));

    /* Disable clock gate */
    GLB_AHB_Slave1_Clock_Gate(DISABLE, BL_AHB_SLAVE1_IRR);

    tmpVal = BL_RD_REG(IR_BASE, IRRX_CONFIG);

    /* Set rx mode */
    switch (irRxCfg->rxMode) {
        case IR_RX_NEC:
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, IR_CR_IRRX_MODE, 0x0);
            break;

        case IR_RX_RC5:
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, IR_CR_IRRX_MODE, 0x1);
            break;

        case IR_RX_SWM:
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, IR_CR_IRRX_MODE, 0x2);
            break;

        default:
            break;
    }

    /* Enable or disable input inverse */
    ENABLE == irRxCfg->inputInverse ? (tmpVal = BL_SET_REG_BIT(tmpVal, IR_CR_IRRX_IN_INV)) : (tmpVal = BL_CLR_REG_BIT(tmpVal, IR_CR_IRRX_IN_INV));
    /* Enable or disable rx input de-glitch function */
    ENABLE == irRxCfg->rxDeglitch ? (tmpVal = BL_SET_REG_BIT(tmpVal, IR_CR_IRRX_DEG_EN)) : (tmpVal = BL_CLR_REG_BIT(tmpVal, IR_CR_IRRX_DEG_EN));
    /* Set de-glitch function cycle count */
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, IR_CR_IRRX_DEG_CNT, irRxCfg->DeglitchCnt);
    /* Write back */
    BL_WR_REG(IR_BASE, IRRX_CONFIG, tmpVal);

    tmpVal = BL_RD_REG(IR_BASE, IRRX_PW_CONFIG);
    /* Set pulse width threshold to trigger end condition */
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, IR_CR_IRRX_END_TH, irRxCfg->endThreshold - 1);
    /* Set pulse width threshold for logic0/1 detection */
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, IR_CR_IRRX_DATA_TH, irRxCfg->dataThreshold - 1);
    /* Write back */
    BL_WR_REG(IR_BASE, IRRX_PW_CONFIG, tmpVal);

#ifndef BFLB_USE_HAL_DRIVER
    Interrupt_Handler_Register(IRRX_IRQn, IRRX_IRQHandler);
#endif

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  IR set default value of all registers function
 *
 * @param  None
 *
 * @return SUCCESS
 *
*******************************************************************************/
BL_Err_Type IR_DeInit(void)
{
    GLB_AHB_Slave1_Reset(BL_AHB_SLAVE1_IRR);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  IR enable function
 *
 * @param  direct: IR direction type
 *
 * @return SUCCESS
 *
*******************************************************************************/
BL_Err_Type IR_Enable(IR_Direction_Type direct)
{
    uint32_t tmpVal;

    /* Check the parameters */
    CHECK_PARAM(IS_IR_DIRECTION_TYPE(direct));

    if (direct == IR_TX || direct == IR_TXRX) {
        /* Enable ir tx unit */
        tmpVal = BL_RD_REG(IR_BASE, IRTX_CONFIG);
        BL_WR_REG(IR_BASE, IRTX_CONFIG, BL_SET_REG_BIT(tmpVal, IR_CR_IRTX_EN));
    }

    if (direct == IR_RX || direct == IR_TXRX) {
        /* Enable ir rx unit */
        tmpVal = BL_RD_REG(IR_BASE, IRRX_CONFIG);
        BL_WR_REG(IR_BASE, IRRX_CONFIG, BL_SET_REG_BIT(tmpVal, IR_CR_IRRX_EN));
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  IR disable function
 *
 * @param  direct: IR direction type
 *
 * @return SUCCESS
 *
*******************************************************************************/
BL_Err_Type IR_Disable(IR_Direction_Type direct)
{
    uint32_t tmpVal;

    /* Check the parameters */
    CHECK_PARAM(IS_IR_DIRECTION_TYPE(direct));

    if (direct == IR_TX || direct == IR_TXRX) {
        /* Disable ir tx unit */
        tmpVal = BL_RD_REG(IR_BASE, IRTX_CONFIG);
        BL_WR_REG(IR_BASE, IRTX_CONFIG, BL_CLR_REG_BIT(tmpVal, IR_CR_IRTX_EN));
    }

    if (direct == IR_RX || direct == IR_TXRX) {
        /* Disable ir rx unit */
        tmpVal = BL_RD_REG(IR_BASE, IRRX_CONFIG);
        BL_WR_REG(IR_BASE, IRRX_CONFIG, BL_CLR_REG_BIT(tmpVal, IR_CR_IRRX_EN));
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  IR tx software mode enable or disable function
 *
 * @param  txSWM: Enable or disable
 *
 * @return SUCCESS
 *
*******************************************************************************/
BL_Err_Type IR_TxSWM(BL_Fun_Type txSWM)
{
    uint32_t tmpVal;

    /* Enable or disable tx swm */
    tmpVal = BL_RD_REG(IR_BASE, IRTX_CONFIG);

    if (ENABLE == txSWM) {
        BL_WR_REG(IR_BASE, IRTX_CONFIG, BL_SET_REG_BIT(tmpVal, IR_CR_IRTX_SWM_EN));
    } else {
        BL_WR_REG(IR_BASE, IRTX_CONFIG, BL_CLR_REG_BIT(tmpVal, IR_CR_IRTX_SWM_EN));
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  IR clear rx fifo function
 *
 * @param  None
 *
 * @return SUCCESS
 *
*******************************************************************************/
BL_Err_Type IR_RxFIFOClear(void)
{
    uint32_t tmpVal;

    /* Clear rx fifo */
    tmpVal = BL_RD_REG(IR_BASE, IRRX_SWM_FIFO_CONFIG_0);
    BL_WR_REG(IR_BASE, IRRX_SWM_FIFO_CONFIG_0, BL_SET_REG_BIT(tmpVal, IR_RX_FIFO_CLR));

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  IR send data function
 *
 * @param  irWord: IR tx data word 0 or 1
 * @param  data: data to send
 *
 * @return SUCCESS
 *
*******************************************************************************/
BL_Err_Type IR_SendData(IR_Word_Type irWord, uint32_t data)
{
    /* Check the parameters */
    CHECK_PARAM(IS_IR_WORD_TYPE(irWord));

    /* Write word 0 or word 1 */
    if (IR_WORD_0 == irWord) {
        BL_WR_REG(IR_BASE, IRTX_DATA_WORD0, data);
    } else {
        BL_WR_REG(IR_BASE, IRTX_DATA_WORD1, data);
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  IR software mode send pulse width data function
 *
 * @param  data: data to send
 * @param  length: Length of send buffer
 *
 * @return SUCCESS
 *
*******************************************************************************/
BL_Err_Type IR_SWMSendData(uint16_t *data, uint8_t length)
{
    uint8_t i = 0, j = 0;
    uint16_t minData = data[0];
    uint32_t tmpVal;
    uint32_t pwVal = 0;
    uint32_t count = (length + 7) / 8;

    /* Search for min value */
    for (i = 1; i < length; i++) {
        if (minData > data[i] && data[i] != 0) {
            minData = data[i];
        }
    }

    /* Set pulse width unit */
    tmpVal = BL_RD_REG(IR_BASE, IRTX_PULSE_WIDTH);
    BL_WR_REG(IR_BASE, IRTX_PULSE_WIDTH, BL_SET_REG_BITS_VAL(tmpVal, IR_CR_IRTX_PW_UNIT, minData));

    /* Set tx SWM pulse width data as multiples of pulse width unit */
    for (i = 0; i < count; i++) {
        pwVal = 0;

        if (i < count - 1) {
            for (j = 0; j < 8; j++) {
                tmpVal = ((2 * data[j + i * 8] + minData) / (2 * minData) - 1) & 0xf;
                pwVal |= tmpVal << (4 * j);
            }

            *(volatile uint32_t *)(IR_BASE + IRTX_SWM_PW_0_OFFSET + i * 4) = pwVal;
        } else {
            for (j = 0; j < length % 8; j++) {
                tmpVal = ((2 * data[j + i * 8] + minData) / (2 * minData) - 1) & 0xf;
                pwVal |= tmpVal << (4 * j);
            }

            *(volatile uint32_t *)(IR_BASE + IRTX_SWM_PW_0_OFFSET + i * 4) = pwVal;
        }
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  IR send command function
 *
 * @param  word1: IR send data word 1
 * @param  word0: IR send data word 0
 *
 * @return SUCCESS
 *
*******************************************************************************/
BL_Err_Type IR_SendCommand(uint32_t word1, uint32_t word0)
{
    uint32_t timeoutCnt = IR_TX_INT_TIMEOUT_COUNT;

    /* Write data */
    IR_SendData(IR_WORD_1, word1);
    IR_SendData(IR_WORD_0, word0);

    /* Mask tx interrupt */
    IR_IntMask(IR_INT_TX, MASK);

    /* Clear tx interrupt */
    IR_ClrIntStatus(IR_INT_TX);

    /* Enable ir tx */
    IR_Enable(IR_TX);

    /* Wait for tx interrupt */
    while (SET != IR_GetIntStatus(IR_INT_TX)) {
        timeoutCnt--;

        if (timeoutCnt == 0) {
            IR_Disable(IR_TX);

            return TIMEOUT;
        }
    }

    /* Disable ir tx */
    IR_Disable(IR_TX);

    /* Clear tx interrupt */
    IR_ClrIntStatus(IR_INT_TX);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  IR send command in software mode function
 *
 * @param  data: IR fifo data to send
 * @param  length: Length of data
 *
 * @return SUCCESS
 *
*******************************************************************************/
BL_Err_Type IR_SWMSendCommand(uint16_t *data, uint8_t length)
{
    uint32_t timeoutCnt = IR_TX_INT_TIMEOUT_COUNT;

    /* Write fifo */
    IR_SWMSendData(data, length);

    /* Mask tx interrupt */
    IR_IntMask(IR_INT_TX, MASK);

    /* Clear tx interrupt */
    IR_ClrIntStatus(IR_INT_TX);

    /* Enable ir tx */
    IR_Enable(IR_TX);

    /* Wait for tx interrupt */
    while (SET != IR_GetIntStatus(IR_INT_TX)) {
        timeoutCnt--;

        if (timeoutCnt == 0) {
            IR_Disable(IR_TX);

            return TIMEOUT;
        }
    }

    /* Disable ir tx */
    IR_Disable(IR_TX);

    /* Clear tx interrupt */
    IR_ClrIntStatus(IR_INT_TX);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  IR send in NEC protocol
 *
 * @param  address: Address
 * @param  command: Command
 *
 * @return SUCCESS
 *
*******************************************************************************/
BL_Err_Type IR_SendNEC(uint8_t address, uint8_t command)
{
    uint32_t tmpVal = ((~command & 0xff) << 24) + (command << 16) + ((~address & 0xff) << 8) + address;

    IR_SendCommand(0, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  IR interrupt mask or unmask function
 *
 * @param  intType: IR interrupt type
 * @param  intMask: Mask or unmask
 *
 * @return SUCCESS
 *
*******************************************************************************/
BL_Err_Type IR_IntMask(IR_INT_Type intType, BL_Mask_Type intMask)
{
    uint32_t tmpVal;

    /* Check the parameters */
    CHECK_PARAM(IS_IR_INT_TYPE(intType));

    if (intType == IR_INT_TX || intType == IR_INT_ALL) {
        /* Mask or unmask tx interrupt */
        tmpVal = BL_RD_REG(IR_BASE, IRTX_INT_STS);
        BL_WR_REG(IR_BASE, IRTX_INT_STS, BL_SET_REG_BITS_VAL(tmpVal, IR_CR_IRTX_END_MASK, intMask));
    }

    if (intType == IR_INT_RX || intType == IR_INT_ALL) {
        /* Mask or unmask rx interrupt */
        tmpVal = BL_RD_REG(IR_BASE, IRRX_INT_STS);
        BL_WR_REG(IR_BASE, IRRX_INT_STS, BL_SET_REG_BITS_VAL(tmpVal, IR_CR_IRRX_END_MASK, intMask));
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Clear ir interrupt function
 *
 * @param  intType: IR interrupt type
 *
 * @return SUCCESS
 *
*******************************************************************************/
BL_Err_Type IR_ClrIntStatus(IR_INT_Type intType)
{
    uint32_t tmpVal;

    /* Check the parameters */
    CHECK_PARAM(IS_IR_INT_TYPE(intType));

    if (intType == IR_INT_TX || intType == IR_INT_ALL) {
        /* Clear tx interrupt */
        tmpVal = BL_RD_REG(IR_BASE, IRTX_INT_STS);
        BL_WR_REG(IR_BASE, IRTX_INT_STS, BL_SET_REG_BIT(tmpVal, IR_CR_IRTX_END_CLR));
    }

    if (intType == IR_INT_RX || intType == IR_INT_ALL) {
        /* Clear rx interrupt */
        tmpVal = BL_RD_REG(IR_BASE, IRRX_INT_STS);
        BL_WR_REG(IR_BASE, IRRX_INT_STS, BL_SET_REG_BIT(tmpVal, IR_CR_IRRX_END_CLR));
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  IR install interrupt callback function
 *
 * @param  intType: IR interrupt type
 * @param  cbFun: Pointer to interrupt callback function. The type should be void (*fn)(void)
 *
 * @return SUCCESS
 *
*******************************************************************************/
BL_Err_Type IR_Int_Callback_Install(IR_INT_Type intType, intCallback_Type *cbFun)
{
    /* Check the parameters */
    CHECK_PARAM(IS_IR_INT_TYPE(intType));

    irIntCbfArra[intType] = cbFun;

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  IR get interrupt status function
 *
 * @param  intType: IR int type
 *
 * @return IR tx or rx interrupt status
 *
*******************************************************************************/
BL_Sts_Type IR_GetIntStatus(IR_INT_Type intType)
{
    uint32_t tmpVal = 0;

    /* Check the parameters */
    CHECK_PARAM(IS_IR_INT_TYPE(intType));

    /* Read tx or rx interrupt status */
    if (IR_INT_TX == intType) {
        tmpVal = BL_RD_REG(IR_BASE, IRTX_INT_STS);
        tmpVal = BL_GET_REG_BITS_VAL(tmpVal, IRTX_END_INT);
    } else if (IR_INT_RX == intType) {
        tmpVal = BL_RD_REG(IR_BASE, IRRX_INT_STS);
        tmpVal = BL_GET_REG_BITS_VAL(tmpVal, IRRX_END_INT);
    }

    if (tmpVal) {
        return SET;
    } else {
        return RESET;
    }
}

/****************************************************************************/ /**
 * @brief  IR get rx fifo underflow or overflow status function
 *
 * @param  fifoSts: IR fifo status type
 *
 * @return IR rx fifo status
 *
*******************************************************************************/
BL_Sts_Type IR_GetRxFIFOStatus(IR_FifoStatus_Type fifoSts)
{
    uint32_t tmpVal;

    /* Check the parameters */
    CHECK_PARAM(IS_IR_FIFOSTATUS_TYPE(fifoSts));

    /* Read rx fifo status */
    tmpVal = BL_RD_REG(IR_BASE, IRRX_SWM_FIFO_CONFIG_0);

    if (fifoSts == IR_RX_FIFO_UNDERFLOW) {
        tmpVal = BL_GET_REG_BITS_VAL(tmpVal, IR_RX_FIFO_UNDERFLOW);
    } else {
        tmpVal = BL_GET_REG_BITS_VAL(tmpVal, IR_RX_FIFO_OVERFLOW);
    }

    if (tmpVal) {
        return SET;
    } else {
        return RESET;
    }
}

/****************************************************************************/ /**
 * @brief  IR receive data function
 *
 * @param  irWord: IR rx data word 0 or 1
 *
 * @return Data received
 *
*******************************************************************************/
uint32_t IR_ReceiveData(IR_Word_Type irWord)
{
    uint32_t tmpVal;

    /* Check the parameters */
    CHECK_PARAM(IS_IR_WORD_TYPE(irWord));

    /* Read word 0 or word 1 */
    if (IR_WORD_0 == irWord) {
        tmpVal = BL_RD_REG(IR_BASE, IRRX_DATA_WORD0);
    } else {
        tmpVal = BL_RD_REG(IR_BASE, IRRX_DATA_WORD1);
    }

    return tmpVal;
}

/****************************************************************************/ /**
 * @brief  IR software mode receive pulse width data function
 *
 * @param  data: Data received
 * @param  length: Max length of receive buffer
 *
 * @return Length of datas received
 *
*******************************************************************************/
uint8_t IR_SWMReceiveData(uint16_t *data, uint8_t length)
{
    uint8_t rxLen = 0;

    while (rxLen < length && IR_GetRxFIFOCount() > 0) {
        /* Read data */
        data[rxLen++] = BL_RD_REG(IR_BASE, IRRX_SWM_FIFO_RDATA) & 0xffff;
    }

    return rxLen;
}

/****************************************************************************/ /**
 * @brief  IR receive in NEC protocol
 *
 * @param  address: Address
 * @param  command: Command
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type IR_ReceiveNEC(uint8_t *address, uint8_t *command)
{
    uint32_t tmpVal = IR_ReceiveData(IR_WORD_0);

    *address = tmpVal & 0xff;
    *command = (tmpVal >> 16) & 0xff;

    if ((~(*address) & 0xff) != ((tmpVal >> 8) & 0xff) || (~(*command) & 0xff) != ((tmpVal >> 24) & 0xff)) {
        return ERROR;
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  IR get rx data bit count function
 *
 * @param  None
 *
 * @return IR rx data bit count
 *
*******************************************************************************/
uint8_t IR_GetRxDataBitCount(void)
{
    uint32_t tmpVal;

    /* Read rx data bit count */
    tmpVal = BL_RD_REG(IR_BASE, IRRX_DATA_COUNT);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, IR_STS_IRRX_DATA_CNT);

    return tmpVal;
}

/****************************************************************************/ /**
 * @brief  IR get rx fifo count function
 *
 * @param  None
 *
 * @return IR rx fifo available count
 *
*******************************************************************************/
uint8_t IR_GetRxFIFOCount(void)
{
    uint32_t tmpVal;

    /* Read rx fifo count */
    tmpVal = BL_RD_REG(IR_BASE, IRRX_SWM_FIFO_CONFIG_0);
    tmpVal = BL_GET_REG_BITS_VAL(tmpVal, IR_RX_FIFO_CNT);

    return tmpVal;
}

/****************************************************************************/ /**
 * @brief  IR learning to set rx and tx mode function
 *
 * @param  data: Buffer to save data
 * @param  length: Length of data
 *
 * @return Protocol type
 *
*******************************************************************************/
IR_RxMode_Type IR_LearnToInit(uint32_t *data, uint8_t *length)
{
    uint32_t tmpVal;
    uint32_t timeoutCnt = IR_RX_INT_TIMEOUT_COUNT;

    /* Disable clock gate */
    GLB_AHB_Slave1_Clock_Gate(DISABLE, BL_AHB_SLAVE1_IRR);

    /* Disable rx,set rx in software mode and enable rx input inverse */
    tmpVal = BL_RD_REG(IR_BASE, IRRX_CONFIG);
    tmpVal = BL_CLR_REG_BIT(tmpVal, IR_CR_IRRX_EN);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, IR_CR_IRRX_MODE, 0x2);
    tmpVal = BL_SET_REG_BIT(tmpVal, IR_CR_IRRX_IN_INV);
    BL_WR_REG(IR_BASE, IRRX_CONFIG, tmpVal);
    /* Set pulse width threshold to trigger end condition */
    tmpVal = BL_RD_REG(IR_BASE, IRRX_PW_CONFIG);
    BL_WR_REG(IR_BASE, IRRX_PW_CONFIG, BL_SET_REG_BITS_VAL(tmpVal, IR_CR_IRRX_END_TH, 19999));

    /* Clear and mask rx interrupt */
    tmpVal = BL_RD_REG(IR_BASE, IRRX_INT_STS);
    tmpVal = BL_SET_REG_BIT(tmpVal, IR_CR_IRRX_END_MASK);
    BL_WR_REG(IR_BASE, IRRX_INT_STS, BL_SET_REG_BIT(tmpVal, IR_CR_IRRX_END_CLR));

    /* Enable rx */
    tmpVal = BL_RD_REG(IR_BASE, IRRX_CONFIG);
    BL_WR_REG(IR_BASE, IRRX_CONFIG, BL_SET_REG_BIT(tmpVal, IR_CR_IRRX_EN));

    /* Wait for rx interrupt */
    while (SET != IR_GetIntStatus(IR_INT_RX)) {
        timeoutCnt--;

        if (timeoutCnt == 0) {
            IR_Disable(IR_RX);

            return IR_RX_SWM;
        }
    }

    /* Disable rx */
    tmpVal = BL_RD_REG(IR_BASE, IRRX_CONFIG);
    BL_WR_REG(IR_BASE, IRRX_CONFIG, BL_CLR_REG_BIT(tmpVal, IR_CR_IRRX_EN));

    /* Clear rx interrupt */
    tmpVal = BL_RD_REG(IR_BASE, IRRX_INT_STS);
    BL_WR_REG(IR_BASE, IRRX_INT_STS, BL_SET_REG_BIT(tmpVal, IR_CR_IRRX_END_CLR));

    /*Receive data */
    *length = IR_GetRxFIFOCount();
    *length = IR_SWMReceiveData((uint16_t *)data, *length);

    /* Judge protocol type */
    if (NEC_HEAD_H_MIN < (data[0] & 0xffff) && (data[0] & 0xffff) < NEC_HEAD_H_MAX && NEC_HEAD_L_MIN < (data[0] >> 16) && (data[0] >> 16) < NEC_HEAD_L_MAX && NEC_BIT0_H_MIN < (data[1] & 0xffff) && (data[1] & 0xffff) < NEC_BIT0_H_MAX) {
        /* Set rx in NEC mode */
        tmpVal = BL_RD_REG(IR_BASE, IRRX_CONFIG);
        BL_WR_REG(IR_BASE, IRRX_CONFIG, BL_SET_REG_BITS_VAL(tmpVal, IR_CR_IRRX_MODE, 0x0));
        /* Set pulse width threshold to trigger end condition and pulse width threshold for logic0/1 detection */
        BL_WR_REG(IR_BASE, IRRX_PW_CONFIG, 0x23270d47);
        /* Set tx in NEC mode */
        /* Tx configure */
        BL_WR_REG(IR_BASE, IRTX_CONFIG, 0x1f514);
        /* Set logic 0,logic 1,head and tail pulse width */
        BL_WR_REG(IR_BASE, IRTX_PW, 0x7f2000);
        /* Set modulation phase width and pulse width unit */
        BL_WR_REG(IR_BASE, IRTX_PULSE_WIDTH, 0x22110464);

        return IR_RX_NEC;
    } else if (RC5_ONE_PLUSE_MIN < (data[0] & 0xffff) && (data[0] & 0xffff) < RC5_ONE_PLUSE_MAX && ((RC5_ONE_PLUSE_MIN < (data[0] >> 16) && (data[0] >> 16) < RC5_ONE_PLUSE_MAX) || (RC5_TWO_PLUSE_MIN < (data[0] >> 16) && (data[0] >> 16) < RC5_TWO_PLUSE_MAX)) &&
               ((RC5_ONE_PLUSE_MIN < (data[1] & 0xffff) && (data[1] & 0xffff) < RC5_ONE_PLUSE_MAX) || (RC5_TWO_PLUSE_MIN < (data[1] & 0xffff) && (data[1] & 0xffff) < RC5_TWO_PLUSE_MAX))) {
        /* Set rx in RC-5 mode */
        tmpVal = BL_RD_REG(IR_BASE, IRRX_CONFIG);
        BL_WR_REG(IR_BASE, IRRX_CONFIG, BL_SET_REG_BITS_VAL(tmpVal, IR_CR_IRRX_MODE, 0x1));
        /* Set pulse width threshold to trigger end condition and pulse width threshold for logic0/1 detection */
        BL_WR_REG(IR_BASE, IRRX_PW_CONFIG, 0x13870a6a);
        /* Set tx in RC-5 mode */
        /* Tx configure */
        BL_WR_REG(IR_BASE, IRTX_CONFIG, 0xc134);
        /* Set logic 0,logic 1,head and tail pulse width */
        BL_WR_REG(IR_BASE, IRTX_PW, 0);
        /* Set modulation phase width and pulse width unit */
        BL_WR_REG(IR_BASE, IRTX_PULSE_WIDTH, 0x221106f1);

        return IR_RX_RC5;
    } else if ((data[0] >> 16) != 0) {
        /* Set tx in software mode */
        /* Tx configure */
        BL_WR_REG(IR_BASE, IRTX_CONFIG, *length << 12 | 0xc);
        /* Set modulation phase width */
        BL_WR_REG(IR_BASE, IRTX_PULSE_WIDTH, 0x22110000);

        return IR_RX_SWM;
    } else {
        tmpVal = BL_RD_REG(IR_BASE, IRRX_CONFIG);
        tmpVal = BL_GET_REG_BITS_VAL(tmpVal, IR_CR_IRRX_MODE);

        if (tmpVal == 0) {
            return IR_RX_NEC;
        } else if (tmpVal == 1) {
            return IR_RX_RC5;
        } else {
            return IR_RX_SWM;
        }
    }
}

/****************************************************************************/ /**
 * @brief  IR receive data according to mode which is learned function
 *
 * @param  mode: Protocol type
 * @param  data: Buffer to save data
 *
 * @return Length of data
 *
*******************************************************************************/
uint8_t IR_LearnToReceive(IR_RxMode_Type mode, uint32_t *data)
{
    uint8_t length = 0;
    uint32_t timeoutCnt = IR_RX_INT_TIMEOUT_COUNT;

    /* Check the parameters */
    CHECK_PARAM(IS_IR_RXMODE_TYPE(mode));

    /* Disable ir rx */
    IR_Disable(IR_RX);

    /* Clear and mask rx interrupt */
    IR_ClrIntStatus(IR_INT_RX);
    IR_IntMask(IR_INT_RX, MASK);

    /* Enable ir rx */
    IR_Enable(IR_RX);

    /* Wait for rx interrupt */
    while (SET != IR_GetIntStatus(IR_INT_RX)) {
        timeoutCnt--;

        if (timeoutCnt == 0) {
            IR_Disable(IR_RX);

            return TIMEOUT;
        }
    }

    /* Disable ir rx */
    IR_Disable(IR_RX);

    /* Clear rx interrupt */
    IR_ClrIntStatus(IR_INT_RX);

    /* Receive data according to mode */
    if (mode == IR_RX_NEC || mode == IR_RX_RC5) {
        /* Get data bit count */
        length = IR_GetRxDataBitCount();
        data[0] = IR_ReceiveData(IR_WORD_0);
    } else {
        /* Get fifo count */
        length = IR_GetRxFIFOCount();
        length = IR_SWMReceiveData((uint16_t *)data, length);
    }

    return length;
}

/****************************************************************************/ /**
 * @brief  IR send data according to mode which is learned function
 *
 * @param  mode: Protocol type
 * @param  data: Buffer of data to send
 * @param  length: Length of data
 *
 * @return SUCCESS
 *
*******************************************************************************/
BL_Err_Type IR_LearnToSend(IR_RxMode_Type mode, uint32_t *data, uint8_t length)
{
    uint32_t tmpVal;

    /* Check the parameters */
    CHECK_PARAM(IS_IR_RXMODE_TYPE(mode));

    /* Set send length */
    tmpVal = BL_RD_REG(IR_BASE, IRTX_CONFIG);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, IR_CR_IRTX_DATA_NUM, length - 1);
    BL_WR_REG(IR_BASE, IRTX_CONFIG, tmpVal);

    if (mode == IR_RX_NEC || mode == IR_RX_RC5) {
        IR_SendCommand(0, data[0]);
    } else {
        IR_SWMSendCommand((uint16_t *)data, length);
    }

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  IR init to control led function
 *
 * @param  clk: Clock source
 * @param  div: Clock division(1~64)
 * @param  unit: Pulse width unit(multiples of clock pulse width, 1~4096)
 * @param  code0H: code 0 high level time(multiples of pulse width unit, 1~16)
 * @param  code0L: code 0 low level time(multiples of pulse width unit, 1~16)
 * @param  code1H: code 1 high level time(multiples of pulse width unit, 1~16)
 * @param  code1L: code 1 low level time(multiples of pulse width unit, 1~16)
 *
 * @return SUCCESS
 *
*******************************************************************************/
BL_Err_Type IR_LEDInit(HBN_XCLK_CLK_Type clk, uint8_t div, uint8_t unit, uint8_t code0H, uint8_t code0L, uint8_t code1H, uint8_t code1L)
{
    IR_TxCfg_Type txCfg = {
        24,      /* 24-bit data */
        DISABLE, /* Disable signal of tail pulse inverse */
        DISABLE, /* Disable signal of tail pulse */
        DISABLE, /* Disable signal of head pulse inverse */
        DISABLE, /* Disable signal of head pulse */
        DISABLE, /* Disable signal of logic 1 pulse inverse */
        DISABLE, /* Disable signal of logic 0 pulse inverse */
        ENABLE,  /* Enable signal of data pulse */
        DISABLE, /* Disable signal of output modulation */
        ENABLE   /* Enable signal of output inverse */
    };

    IR_TxPulseWidthCfg_Type txPWCfg = {
        code0L, /* Pulse width of logic 0 pulse phase 1 */
        code0H, /* Pulse width of logic 0 pulse phase 0 */
        code1L, /* Pulse width of logic 1 pulse phase 1 */
        code1H, /* Pulse width of logic 1 pulse phase 0 */
        1,      /* Pulse width of head pulse phase 1 */
        1,      /* Pulse width of head pulse phase 0 */
        1,      /* Pulse width of tail pulse phase 1 */
        1,      /* Pulse width of tail pulse phase 0 */
        1,      /* Modulation phase 1 width */
        1,      /* Modulation phase 0 width */
        unit    /* Pulse width unit */
    };

    HBN_Set_XCLK_CLK_Sel(clk);
    GLB_Set_IR_CLK(ENABLE, GLB_IR_CLK_SRC_XCLK, div - 1);

    /* Disable ir before config */
    IR_Disable(IR_TXRX);

    /* IR tx init */
    IR_TxInit(&txCfg);
    IR_TxPulseWidthConfig(&txPWCfg);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  IR send 24-bit data to control led function
 *
 * @param  data: Data to send(24-bit)
 *
 * @return SUCCESS
 *
*******************************************************************************/
BL_Err_Type IR_LEDSend(uint32_t data)
{
    /* Change MSB_first to LSB_first */
    data = ((data >> 1) & 0x55555555) | ((data << 1) & 0xaaaaaaaa);
    data = ((data >> 2) & 0x33333333) | ((data << 2) & 0xcccccccc);
    data = ((data >> 4) & 0x0f0f0f0f) | ((data << 4) & 0xf0f0f0f0);
    data = ((data >> 16) & 0xff) | (data & 0xff00) | ((data << 16) & 0xff0000);
    IR_SendCommand(0, data);

    return SUCCESS;
}

/*@} end of group IR_Public_Functions */

/*@} end of group IR */

/*@} end of group BL702_Peripheral_Driver */
