/**
  ******************************************************************************
  * @file    bl702_emac.c
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

#include "bl702.h"
#include "bl702_emac.h"
#include "bl702_glb.h"

/** @addtogroup  BL702_Peripheral_Driver
 *  @{
 */

/** @addtogroup  EMAC
 *  @{
 */

/** @defgroup  EMAC_Private_Macros
 *  @{
 */
#define PHY_MAX_RETRY (0x3F0)

/*@} end of group EMAC_Private_Macros */

/** @defgroup  EMAC_Private_Types
 *  @{
 */

/*@} end of group EMAC_Private_Types */

/** @defgroup  EMAC_Private_Variables
 *  @{
 */
static intCallback_Type *emacIntCbfArra[EMAC_INT_CNT] = { NULL };

/*@} end of group EMAC_Private_Variables */

/** @defgroup  EMAC_Global_Variables
 *  @{
 */

/*@} end of group EMAC_Global_Variables */

/** @defgroup  EMAC_Private_Fun_Declaration
 *  @{
 */

/*@} end of group EMAC_Private_Fun_Declaration */

/** @defgroup  EMAC_Private_Functions
 *  @{
 */

/****************************************************************************/ /**
 * @brief  Set MAC Address
 *
 * @param  macAddr[6]: MAC address buffer array
 *
 * @return None
 *
*******************************************************************************/
static void EMAC_SetMACAddress(uint8_t macAddr[6])
{
    BL_WR_REG(EMAC_BASE, EMAC_MAC_ADDR1, (macAddr[0] << 8) | macAddr[1]);
    BL_WR_REG(EMAC_BASE, EMAC_MAC_ADDR0, (macAddr[2] << 24) | (macAddr[3] << 16) | (macAddr[4] << 8) | (macAddr[5] << 0));
}

/****************************************************************************/ /**
 * @brief  Set PHY Address
 *
 * @param  phyAddress: Phy address
 *
 * @return None
 *
*******************************************************************************/
void EMAC_Phy_SetAddress(uint16_t phyAddress)
{
    uint32_t tmpVal;

    /* Set Phy Address */
    tmpVal = BL_RD_REG(EMAC_BASE, EMAC_MIIADDRESS);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, EMAC_FIAD, phyAddress);
    BL_WR_REG(EMAC_BASE, EMAC_MIIADDRESS, tmpVal);
}

/****************************************************************************/ /**
 * @brief  Set PHY Address
 *
 * @param  phyAddress: Phy address
 *
 * @return None
 *
*******************************************************************************/
void EMAC_Phy_Set_Full_Duplex(uint8_t fullDuplex)
{
    uint32_t tmpVal;

    /* Set MAC duplex config */
    tmpVal = BL_RD_REG(EMAC_BASE, EMAC_MODE);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, EMAC_FULLD, fullDuplex);
    BL_WR_REG(EMAC_BASE, EMAC_MODE, tmpVal);
}

/****************************************************************************/ /**
 * @brief  Read PHY register
 *
 * @param  phyReg: PHY register
 * @param  regValue: PHY register value pointer
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type EMAC_Phy_Read(uint16_t phyReg, uint16_t *regValue)
{
    uint32_t tmpVal;

    /* Set Register Address */
    tmpVal = BL_RD_REG(EMAC_BASE, EMAC_MIIADDRESS);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, EMAC_RGAD, phyReg);
    BL_WR_REG(EMAC_BASE, EMAC_MIIADDRESS, tmpVal);

    /* Trigger read */
    tmpVal = BL_RD_REG(EMAC_BASE, EMAC_MIICOMMAND);
    tmpVal = BL_SET_REG_BIT(tmpVal, EMAC_RSTAT);
    BL_WR_REG(EMAC_BASE, EMAC_MIICOMMAND, tmpVal);

    BL_DRV_DUMMY;

    do {
        tmpVal = BL_RD_REG(EMAC_BASE, EMAC_MIISTATUS);
        BL702_Delay_US(16);
    } while (BL_IS_REG_BIT_SET(tmpVal, EMAC_MIIM_BUSY));

    *regValue = BL_RD_REG(EMAC_BASE, EMAC_MIIRX_DATA);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Write PHY register
 *
 * @param  phyReg: PHY register
 * @param  regValue: PHY register value
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type EMAC_Phy_Write(uint16_t phyReg, uint16_t regValue)
{
    uint32_t tmpVal;

    /* Set Register Address */
    tmpVal = BL_RD_REG(EMAC_BASE, EMAC_MIIADDRESS);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, EMAC_RGAD, phyReg);
    BL_WR_REG(EMAC_BASE, EMAC_MIIADDRESS, tmpVal);

    /* Set Write data */
    BL_WR_REG(EMAC_BASE, EMAC_MIITX_DATA, regValue);

    /* Trigger write */
    tmpVal = BL_RD_REG(EMAC_BASE, EMAC_MIICOMMAND);
    tmpVal = BL_SET_REG_BIT(tmpVal, EMAC_WCTRLDATA);
    BL_WR_REG(EMAC_BASE, EMAC_MIICOMMAND, tmpVal);

    BL_DRV_DUMMY;

    do {
        tmpVal = BL_RD_REG(EMAC_BASE, EMAC_MIISTATUS);
    } while (BL_IS_REG_BIT_SET(tmpVal, EMAC_MIIM_BUSY));

    return SUCCESS;
}

/*@} end of group EMAC_Private_Functions */

/** @defgroup  EMAC_Public_Functions
 *  @{
 */

/****************************************************************************/ /**
 * @brief  Initialize EMAC module
 *
 * @param  cfg: EMAC configuration pointer
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type EMAC_Init(EMAC_CFG_Type *cfg)
{
    uint32_t tmpVal;

    /* Disable clock gate */
    GLB_AHB_Slave1_Clock_Gate(DISABLE, BL_AHB_SLAVE1_EMAC);

    /* Set MAC config */
    tmpVal = BL_RD_REG(EMAC_BASE, EMAC_MODE);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, EMAC_RMII_EN, 1);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, EMAC_RECSMALL, cfg->recvSmallFrame);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, EMAC_PAD, cfg->padEnable);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, EMAC_HUGEN, cfg->recvHugeFrame);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, EMAC_CRCEN, cfg->crcEnable);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, EMAC_NOPRE, cfg->noPreamble);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, EMAC_BRO, cfg->recvBroadCast);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, EMAC_PRO, ENABLE);
    //tmpVal |= (1 << 7); /* local loopback in emac */
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, EMAC_IFG, cfg->interFrameGapCheck);
    BL_WR_REG(EMAC_BASE, EMAC_MODE, tmpVal);

    /* Set inter frame gap value */
    BL_WR_REG(EMAC_BASE, EMAC_IPGT, cfg->interFrameGapValue);

    /* Set MII interface */
    tmpVal = BL_RD_REG(EMAC_BASE, EMAC_MIIMODE);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, EMAC_MIINOPRE, cfg->miiNoPreamble);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, EMAC_CLKDIV, cfg->miiClkDiv);
    BL_WR_REG(EMAC_BASE, EMAC_MIIMODE, tmpVal);

    /* Set collision */
    tmpVal = BL_RD_REG(EMAC_BASE, EMAC_COLLCONFIG);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, EMAC_MAXRET, cfg->maxTxRetry);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, EMAC_COLLVALID, cfg->collisionValid);
    BL_WR_REG(EMAC_BASE, EMAC_COLLCONFIG, tmpVal);

    /* Set frame length */
    tmpVal = BL_RD_REG(EMAC_BASE, EMAC_PACKETLEN);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, EMAC_MINFL, cfg->minFrameLen);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, EMAC_MAXFL, cfg->maxFrameLen);
    BL_WR_REG(EMAC_BASE, EMAC_PACKETLEN, tmpVal);

    EMAC_SetMACAddress(cfg->macAddr);

    void EMAC_IRQHandler(void);
    Interrupt_Handler_Register(EMAC_IRQn, EMAC_IRQHandler);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  DeInitialize EMAC module
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type EMAC_DeInit(void)
{
    EMAC_Disable();

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Initialize EMAC TX RX MDA buffer
 *
 * @param  handle: EMAC handle pointer
 * @param  txBuff: TX buffer
 * @param  txBuffCount: TX buffer count
 * @param  rxBuff: RX buffer
 * @param  rxBuffCount: RX buffer count
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type EMAC_DMADescListInit(EMAC_Handle_Type *handle, uint8_t *txBuff, uint32_t txBuffCount, uint8_t *rxBuff, uint32_t rxBuffCount)
{
    uint32_t i = 0;

    /* Set the Ethernet handler env */
    handle->bd = (EMAC_BD_Desc_Type *)(EMAC_BASE + EMAC_DMA_DESC_OFFSET);
    handle->txIndexEMAC = 0;
    handle->txIndexCPU = 0;
    handle->txBuffLimit = txBuffCount - 1;
    /* The receive descriptors' address starts right after the last transmit BD. */
    handle->rxIndexEMAC = txBuffCount;
    handle->rxIndexCPU = txBuffCount;
    handle->rxBuffLimit = txBuffCount + rxBuffCount - 1;

    /* Fill each DMARxDesc descriptor with the right values */
    for (i = 0; i < txBuffCount; i++) {
        /* Get the pointer on the ith member of the Tx Desc list */
        handle->bd[i].Buffer = (NULL == txBuff) ? 0 : (uint32_t)(txBuff + (ETH_MAX_PACKET_SIZE * i));
        handle->bd[i].C_S_L = 0;
    }

    /* For the last TX DMA Descriptor, it should be wrap back */
    handle->bd[handle->txBuffLimit].C_S_L |= EMAC_BD_FIELD_MSK(TX_WR);

    for (i = txBuffCount; i < (txBuffCount + rxBuffCount); i++) {
        /* Get the pointer on the ith member of the Rx Desc list */
        handle->bd[i].Buffer = (NULL == rxBuff) ? 0 : (uint32_t)(rxBuff + (ETH_MAX_PACKET_SIZE * (i - txBuffCount)));
        handle->bd[i].C_S_L = (ETH_MAX_PACKET_SIZE << 16) |
                              EMAC_BD_FIELD_MSK(RX_IRQ) |
                              EMAC_BD_FIELD_MSK(RX_E);
    }

    /* For the last RX DMA Descriptor, it should be wrap back */
    handle->bd[handle->rxBuffLimit].C_S_L |= EMAC_BD_FIELD_MSK(RX_WR);

    /* For the TX DMA Descriptor, it will wrap to 0 according to EMAC_TX_BD_NUM*/
    BL_WR_REG(EMAC_BASE, EMAC_TX_BD_NUM, txBuffCount);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Get TX MDA buffer descripter for data to send
 *
 * @param  handle: EMAC handle pointer
 * @param  txDMADesc: TX DMA descriptor pointer
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type EMAC_DMATxDescGet(EMAC_Handle_Type *handle, EMAC_BD_Desc_Type **txDMADesc)
{
    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Start TX
 *
 * @param  handle: EMAC handle pointer
 * @param  txDMADesc: TX DMA descriptor pointer
 * @param  len: len
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type EMAC_StartTx(EMAC_Handle_Type *handle, EMAC_BD_Desc_Type *txDMADesc, uint32_t len)
{
    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Enable EMAC module
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type EMAC_Enable(void)
{
    uint32_t tmpVal;

    /* Enable EMAC */
    tmpVal = BL_RD_REG(EMAC_BASE, EMAC_MODE);
    tmpVal = BL_SET_REG_BIT(tmpVal, EMAC_TXEN);
    tmpVal = BL_SET_REG_BIT(tmpVal, EMAC_RXEN);
    BL_WR_REG(EMAC_BASE, EMAC_MODE, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  EMAC_Enable_TX
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type EMAC_Enable_TX(void)
{
    uint32_t tmpVal;

    /* Enable EMAC */
    tmpVal = BL_RD_REG(EMAC_BASE, EMAC_MODE);
    tmpVal = BL_SET_REG_BIT(tmpVal, EMAC_TXEN);
    BL_WR_REG(EMAC_BASE, EMAC_MODE, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  EMAC_Disable_TX
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type EMAC_Disable_TX(void)
{
    uint32_t tmpVal;

    /* Enable EMAC */
    tmpVal = BL_RD_REG(EMAC_BASE, EMAC_MODE);
    tmpVal = BL_CLR_REG_BIT(tmpVal, EMAC_TXEN);
    BL_WR_REG(EMAC_BASE, EMAC_MODE, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  EMAC_Enable_RX
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type EMAC_Enable_RX(void)
{
    uint32_t tmpval;

    /* Enable EMAC TX*/
    tmpval = BL_RD_REG(EMAC_BASE, EMAC_MODE);
    tmpval = BL_SET_REG_BIT(tmpval, EMAC_RXEN);
    BL_WR_REG(EMAC_BASE, EMAC_MODE, tmpval);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  EMAC_Disable_RX
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type EMAC_Disable_RX(void)
{
    uint32_t tmpval;

    /* Disable EMAC RX*/
    tmpval = BL_RD_REG(EMAC_BASE, EMAC_MODE);
    tmpval = BL_CLR_REG_BIT(tmpval, EMAC_RXEN);
    BL_WR_REG(EMAC_BASE, EMAC_MODE, tmpval);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Disable EMAC module
 *
 * @param  None
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type EMAC_Disable(void)
{
    uint32_t tmpVal;

    /* Enable EMAC */
    tmpVal = BL_RD_REG(EMAC_BASE, EMAC_MODE);
    tmpVal = BL_CLR_REG_BIT(tmpVal, EMAC_TXEN);
    tmpVal = BL_CLR_REG_BIT(tmpVal, EMAC_RXEN);
    BL_WR_REG(EMAC_BASE, EMAC_MODE, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  EMAC mask or unmask certain or all interrupt
 *
 * @param  intType: EMAC interrupt type
 * @param  intMask: EMAC interrupt mask value( MASK:disbale interrupt,UNMASK:enable interrupt )
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type EMAC_IntMask(EMAC_INT_Type intType, BL_Mask_Type intMask)
{
    uint32_t tmpVal;

    /* Check the parameters */
    CHECK_PARAM(IS_BL_MASK_TYPE(intMask));

    tmpVal = BL_RD_REG(EMAC_BASE, EMAC_INT_MASK);

    /* Mask or unmask certain or all interrupt */
    if (MASK == intMask) {
        tmpVal |= intType;
    } else {
        tmpVal &= (~intType);
    }

    /* Write back */
    BL_WR_REG(EMAC_BASE, EMAC_INT_MASK, tmpVal);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Get EMAC interrupt status
 *
 * @param  intType: EMAC interrupt type
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Sts_Type EMAC_GetIntStatus(EMAC_INT_Type intType)
{
    uint32_t tmpVal;

    /* Check the parameters */
    CHECK_PARAM(IS_EMAC_INT_TYPE(intType));

    tmpVal = BL_RD_REG(EMAC_BASE, EMAC_INT_SOURCE);

    return (tmpVal & intType) ? SET : RESET;
}

/****************************************************************************/ /**
 * @brief  Clear EMAC interrupt
 *
 * @param  intType: EMAC interrupt type
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type EMAC_ClrIntStatus(EMAC_INT_Type intType)
{
    uint32_t tmpVal;

    /* Check the parameters */
    CHECK_PARAM(IS_EMAC_INT_TYPE(intType));

    tmpVal = BL_RD_REG(EMAC_BASE, EMAC_INT_SOURCE);

    BL_WR_REG(EMAC_BASE, EMAC_INT_SOURCE, tmpVal | intType);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  EMAC_Int_Callback_Install
 *
 * @param  intIdx: EMAC_INT_Index
 * @param  cbFun: call back
 *
 * @return None
 *
*******************************************************************************/
BL_Err_Type EMAC_Int_Callback_Install(EMAC_INT_Index intIdx, intCallback_Type *cbFun)
{
    /* Check the parameters */

    emacIntCbfArra[intIdx] = cbFun;

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  EMAC_IRQHandler
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void EMAC_IRQHandler(void)
{
    uint32_t tmpVal;

    tmpVal = BL_RD_REG(EMAC_BASE, EMAC_INT_MASK);

    if (SET == EMAC_GetIntStatus(EMAC_INT_TX_DONE) && !BL_IS_REG_BIT_SET(tmpVal, EMAC_TXB_M)) {
        EMAC_ClrIntStatus(EMAC_INT_TX_DONE);

        if (emacIntCbfArra[EMAC_INT_TX_DONE_IDX]) {
            emacIntCbfArra[EMAC_INT_TX_DONE_IDX]();
        }
    }

    if (SET == EMAC_GetIntStatus(EMAC_INT_TX_ERROR) && !BL_IS_REG_BIT_SET(tmpVal, EMAC_TXE_M)) {
        EMAC_ClrIntStatus(EMAC_INT_TX_ERROR);

        if (emacIntCbfArra[EMAC_INT_TX_ERROR_IDX]) {
            emacIntCbfArra[EMAC_INT_TX_ERROR_IDX]();
        }
    }

    if (SET == EMAC_GetIntStatus(EMAC_INT_RX_DONE) && !BL_IS_REG_BIT_SET(tmpVal, EMAC_RXB_M)) {
        EMAC_ClrIntStatus(EMAC_INT_RX_DONE);

        if (emacIntCbfArra[EMAC_INT_RX_DONE_IDX]) {
            emacIntCbfArra[EMAC_INT_RX_DONE_IDX]();
        }
    }

    if (SET == EMAC_GetIntStatus(EMAC_INT_RX_ERROR) && !BL_IS_REG_BIT_SET(tmpVal, EMAC_RXE_M)) {
        EMAC_ClrIntStatus(EMAC_INT_RX_ERROR);

        if (emacIntCbfArra[EMAC_INT_RX_ERROR_IDX]) {
            emacIntCbfArra[EMAC_INT_RX_ERROR_IDX]();
        }
    }

    if (SET == EMAC_GetIntStatus(EMAC_INT_RX_BUSY) && !BL_IS_REG_BIT_SET(tmpVal, EMAC_BUSY_M)) {
        EMAC_ClrIntStatus(EMAC_INT_RX_BUSY);

        if (emacIntCbfArra[EMAC_INT_RX_BUSY_IDX]) {
            emacIntCbfArra[EMAC_INT_RX_BUSY_IDX]();
        }
    }

    if (SET == EMAC_GetIntStatus(EMAC_INT_TX_CTRL) && !BL_IS_REG_BIT_SET(tmpVal, EMAC_TXC_M)) {
        EMAC_ClrIntStatus(EMAC_INT_TX_CTRL);

        if (emacIntCbfArra[EMAC_INT_TX_CTRL_IDX]) {
            emacIntCbfArra[EMAC_INT_TX_CTRL_IDX]();
        }
    }

    if (SET == EMAC_GetIntStatus(EMAC_INT_RX_CTRL) && !BL_IS_REG_BIT_SET(tmpVal, EMAC_RXC_M)) {
        EMAC_ClrIntStatus(EMAC_INT_RX_CTRL);

        if (emacIntCbfArra[EMAC_INT_RX_CTRL_IDX]) {
            emacIntCbfArra[EMAC_INT_RX_CTRL_IDX]();
        }
    }
}

/****************************************************************************/ /**
 * @brief  Request to pause TX
 *
 * @param  timeCount: Pause time count
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type EMAC_TxPauseReq(uint16_t timeCount)
{
    BL_WR_REG(EMAC_BASE, EMAC_TXCTRL, (1 << 16) | timeCount);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  Set hash value
 *
 * @param  hash0: Hash value one
 * @param  hash1: Hash value two
 *
 * @return SUCCESS or ERROR
 *
*******************************************************************************/
BL_Err_Type EMAC_SetHash(uint32_t hash0, uint32_t hash1)
{
    BL_WR_REG(EMAC_BASE, EMAC_HASH0_ADDR, hash0);

    BL_WR_REG(EMAC_BASE, EMAC_HASH1_ADDR, hash1);

    return SUCCESS;
}

/*@} end of group EMAC_Public_Functions */

/*@} end of group EMAC */

/*@} end of group BL702_Peripheral_Driver */
