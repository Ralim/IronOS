/**
  ******************************************************************************
  * @file    bl702_dma.c
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
#include "bl702_dma.h"
#include "string.h"
#include "bl702_glb.h"

/** @addtogroup  BL702_Peripheral_Driver
 *  @{
 */

/** @addtogroup  DMA
 *  @{
 */

/** @defgroup  DMA_Private_Macros
 *  @{
 */
#define DMA_CHANNEL_OFFSET  0x100
#define DMA_Get_Channel(ch) (DMA_BASE + DMA_CHANNEL_OFFSET + (ch)*0x100)
static intCallback_Type *dmaIntCbfArra[DMA_CH_MAX][DMA_INT_ALL] = {
    { NULL, NULL },
    { NULL, NULL },
    { NULL, NULL },
    { NULL, NULL }
};
static DMA_LLI_Ctrl_Type PingPongListArra[DMA_CH_MAX][2];

/*@} end of group DMA_Private_Macros */

/** @defgroup  DMA_Private_Types
 *  @{
 */

/*@} end of group DMA_Private_Types */

/** @defgroup  DMA_Private_Variables
 *  @{
 */

/*@} end of group DMA_Private_Variables */

/** @defgroup  DMA_Global_Variables
 *  @{
 */

/*@} end of group DMA_Global_Variables */

/** @defgroup  DMA_Private_Fun_Declaration
 *  @{
 */

/*@} end of group DMA_Private_Fun_Declaration */

/** @defgroup  DMA_Private_Functions
 *  @{
 */

/****************************************************************************/ /**
 * @brief  DMA interrupt handler
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
#ifndef BFLB_USE_HAL_DRIVER
void DMA_ALL_IRQHandler(void)
{
    uint32_t tmpVal;
    uint32_t intClr;
    uint8_t ch;
    /* Get DMA register */
    uint32_t DMAChs = DMA_BASE;

    for (ch = 0; ch < DMA_CH_MAX; ch++) {
        tmpVal = BL_RD_REG(DMAChs, DMA_INTTCSTATUS);

        if ((BL_GET_REG_BITS_VAL(tmpVal, DMA_INTTCSTATUS) & (1 << ch)) != 0) {
            /* Clear interrupt */
            tmpVal = BL_RD_REG(DMAChs, DMA_INTTCCLEAR);
            intClr = BL_GET_REG_BITS_VAL(tmpVal, DMA_INTTCCLEAR);
            intClr |= (1 << ch);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DMA_INTTCCLEAR, intClr);
            BL_WR_REG(DMAChs, DMA_INTTCCLEAR, tmpVal);

            if (dmaIntCbfArra[ch][DMA_INT_TCOMPLETED] != NULL) {
                /* Call the callback function */
                dmaIntCbfArra[ch][DMA_INT_TCOMPLETED]();
            }
        }
    }

    for (ch = 0; ch < DMA_CH_MAX; ch++) {
        tmpVal = BL_RD_REG(DMAChs, DMA_INTERRORSTATUS);

        if ((BL_GET_REG_BITS_VAL(tmpVal, DMA_INTERRORSTATUS) & (1 << ch)) != 0) {
            /*Clear interrupt */
            tmpVal = BL_RD_REG(DMAChs, DMA_INTERRCLR);
            intClr = BL_GET_REG_BITS_VAL(tmpVal, DMA_INTERRCLR);
            intClr |= (1 << ch);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DMA_INTERRCLR, intClr);
            BL_WR_REG(DMAChs, DMA_INTERRCLR, tmpVal);

            if (dmaIntCbfArra[ch][DMA_INT_ERR] != NULL) {
                /* Call the callback function */
                dmaIntCbfArra[ch][DMA_INT_ERR]();
            }
        }
    }
}
#endif

/*@} end of group DMA_Private_Functions */

/** @defgroup  DMA_Public_Functions
 *  @{
 */

/****************************************************************************/ /**
 * @brief  DMA enable
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void DMA_Enable(void)
{
    uint32_t tmpVal;
    /* Get DMA register */
    uint32_t DMAChs = DMA_BASE;

    tmpVal = BL_RD_REG(DMAChs, DMA_TOP_CONFIG);
    tmpVal = BL_SET_REG_BIT(tmpVal, DMA_E);
    BL_WR_REG(DMAChs, DMA_TOP_CONFIG, tmpVal);
#ifndef BFLB_USE_HAL_DRIVER
    Interrupt_Handler_Register(DMA_ALL_IRQn, DMA_ALL_IRQHandler);
#endif
}

/****************************************************************************/ /**
 * @brief  DMA disable
 *
 * @param  None
 *
 * @return None
 *
*******************************************************************************/
void DMA_Disable(void)
{
    uint32_t tmpVal;
    /* Get DMA register */
    uint32_t DMAChs = DMA_BASE;

    tmpVal = BL_RD_REG(DMAChs, DMA_TOP_CONFIG);
    tmpVal = BL_CLR_REG_BIT(tmpVal, DMA_E);
    BL_WR_REG(DMAChs, DMA_TOP_CONFIG, tmpVal);
}

/****************************************************************************/ /**
 * @brief  DMA channel init
 *
 * @param  chCfg: DMA configuration
 *
 * @return None
 *
*******************************************************************************/
void DMA_Channel_Init(DMA_Channel_Cfg_Type *chCfg)
{
    uint32_t tmpVal;
    /* Get channel register */
    uint32_t DMAChs = DMA_Get_Channel(chCfg->ch);

    /* Check the parameters */
    CHECK_PARAM(IS_DMA_CHAN_TYPE(chCfg->ch));
    CHECK_PARAM(IS_DMA_TRANS_WIDTH_TYPE(chCfg->srcTransfWidth));
    CHECK_PARAM(IS_DMA_TRANS_WIDTH_TYPE(chCfg->dstTransfWidth));
    CHECK_PARAM(IS_DMA_BURST_SIZE_TYPE(chCfg->srcBurstSzie));
    CHECK_PARAM(IS_DMA_BURST_SIZE_TYPE(chCfg->dstBurstSzie));
    CHECK_PARAM(IS_DMA_TRANS_DIR_TYPE(chCfg->dir));
    CHECK_PARAM(IS_DMA_PERIPH_REQ_TYPE(chCfg->dstPeriph));
    CHECK_PARAM(IS_DMA_PERIPH_REQ_TYPE(chCfg->srcPeriph));

    /* Disable clock gate */
    GLB_AHB_Slave1_Clock_Gate(DISABLE, BL_AHB_SLAVE1_DMA);

    /* Config channel config */
    BL_WR_REG(DMAChs, DMA_SRCADDR, chCfg->srcDmaAddr);
    BL_WR_REG(DMAChs, DMA_DSTADDR, chCfg->destDmaAddr);

    tmpVal = BL_RD_REG(DMAChs, DMA_CONTROL);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DMA_TRANSFERSIZE, chCfg->transfLength);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DMA_SWIDTH, chCfg->srcTransfWidth);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DMA_DWIDTH, chCfg->dstTransfWidth);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DMA_SBSIZE, chCfg->srcBurstSzie);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DMA_DBSIZE, chCfg->dstBurstSzie);

    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DMA_DST_ADD_MODE, chCfg->dstAddMode);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DMA_DST_MIN_MODE, chCfg->dstMinMode);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DMA_FIX_CNT, chCfg->fixCnt);

    /* FIXME: how to deal with SLargerD */
    tmpVal = BL_CLR_REG_BIT(tmpVal, DMA_SLARGERD);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DMA_SI, chCfg->srcAddrInc);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DMA_DI, chCfg->destAddrInc);
    BL_WR_REG(DMAChs, DMA_CONTROL, tmpVal);

    tmpVal = BL_RD_REG(DMAChs, DMA_CONFIG);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DMA_FLOWCNTRL, chCfg->dir);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DMA_DSTPERIPHERAL, chCfg->dstPeriph);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DMA_SRCPERIPHERAL, chCfg->srcPeriph);
    BL_WR_REG(DMAChs, DMA_CONFIG, tmpVal);
}

/****************************************************************************/ /**
 * @brief  DMA channel update source memory address and len
 *
 * @param  ch: DMA channel
 * @param  memAddr: source memoty address
 * @param  len: source memory data length
 *
 * @return None
 *
*******************************************************************************/
void DMA_Channel_Update_SrcMemcfg(uint8_t ch, uint32_t memAddr, uint32_t len)
{
    uint32_t tmpVal;
    /* Get channel register */
    uint32_t DMAChs = DMA_Get_Channel(ch);

    /* Check the parameters */
    CHECK_PARAM(IS_DMA_CHAN_TYPE(ch));

    /* config channel config*/
    BL_WR_REG(DMAChs, DMA_SRCADDR, memAddr);
    tmpVal = BL_RD_REG(DMAChs, DMA_CONTROL);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DMA_TRANSFERSIZE, len);
    BL_WR_REG(DMAChs, DMA_CONTROL, tmpVal);
}

/****************************************************************************/ /**
 * @brief  DMA channel update destination memory address and len
 *
 * @param  ch: DMA channel
 * @param  memAddr: destination memoty address
 * @param  len: destination memory data length
 *
 * @return None
 *
*******************************************************************************/
void DMA_Channel_Update_DstMemcfg(uint8_t ch, uint32_t memAddr, uint32_t len)
{
    uint32_t tmpVal;
    /* Get channel register */
    uint32_t DMAChs = DMA_Get_Channel(ch);

    /* Check the parameters */
    CHECK_PARAM(IS_DMA_CHAN_TYPE(ch));

    /* config channel config*/
    BL_WR_REG(DMAChs, DMA_DSTADDR, memAddr);
    tmpVal = BL_RD_REG(DMAChs, DMA_CONTROL);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DMA_TRANSFERSIZE, len);
    BL_WR_REG(DMAChs, DMA_CONTROL, tmpVal);
}

/****************************************************************************/ /**
 * @brief  Get DMA channel tranfersize
 *
 * @param  ch: DMA channel
 *
 * @return tranfersize size
 *
*******************************************************************************/
uint32_t DMA_Channel_TranferSize(uint8_t ch)
{
    /* Get channel register */
    uint32_t DMAChs = DMA_Get_Channel(ch);

    /* Check the parameters */
    CHECK_PARAM(IS_DMA_CHAN_TYPE(ch));

    return BL_GET_REG_BITS_VAL(BL_RD_REG(DMAChs, DMA_CONTROL), DMA_TRANSFERSIZE);
}

/****************************************************************************/ /**
 * @brief  Get DMA channel busy status
 *
 * @param  ch: DMA channel
 *
 * @return SET or RESET
 *
*******************************************************************************/
BL_Sts_Type DMA_Channel_Is_Busy(uint8_t ch)
{
    /* Get channel register */
    uint32_t DMAChs = DMA_Get_Channel(ch);

    /* Check the parameters */
    CHECK_PARAM(IS_DMA_CHAN_TYPE(ch));

    return BL_IS_REG_BIT_SET(BL_RD_REG(DMAChs, DMA_CONFIG), DMA_E) == 1 ? SET : RESET;
}

/****************************************************************************/ /**
 * @brief  DMA enable
 *
 * @param  ch: DMA channel number
 *
 * @return None
 *
*******************************************************************************/
void DMA_Channel_Enable(uint8_t ch)
{
    uint32_t tmpVal;
    /* Get channel register */
    uint32_t DMAChs = DMA_Get_Channel(ch);

    /* Check the parameters */
    CHECK_PARAM(IS_DMA_CHAN_TYPE(ch));

    tmpVal = BL_RD_REG(DMAChs, DMA_CONFIG);
    tmpVal = BL_SET_REG_BIT(tmpVal, DMA_E);
    BL_WR_REG(DMAChs, DMA_CONFIG, tmpVal);
}

/****************************************************************************/ /**
 * @brief  DMA disable
 *
 * @param  ch: DMA channel number
 *
 * @return None
 *
*******************************************************************************/
void DMA_Channel_Disable(uint8_t ch)
{
    uint32_t tmpVal;
    /* Get channel register */
    uint32_t DMAChs = DMA_Get_Channel(ch);

    /* Check the parameters */
    CHECK_PARAM(IS_DMA_CHAN_TYPE(ch));

    tmpVal = BL_RD_REG(DMAChs, DMA_CONFIG);
    tmpVal = BL_CLR_REG_BIT(tmpVal, DMA_E);
    BL_WR_REG(DMAChs, DMA_CONFIG, tmpVal);
}

/****************************************************************************/ /**
 * @brief  DMA init LLI transfer
 *
 * @param  ch: DMA channel number
 * @param  lliCfg: LLI configuration
 *
 * @return None
 *
*******************************************************************************/
void DMA_LLI_Init(uint8_t ch, DMA_LLI_Cfg_Type *lliCfg)
{
    uint32_t tmpVal;
    /* Get channel register */
    uint32_t DMAChs = DMA_Get_Channel(ch);

    /* Check the parameters */
    CHECK_PARAM(IS_DMA_CHAN_TYPE(ch));
    CHECK_PARAM(IS_DMA_TRANS_DIR_TYPE(lliCfg->dir));
    CHECK_PARAM(IS_DMA_PERIPH_REQ_TYPE(lliCfg->dstPeriph));
    CHECK_PARAM(IS_DMA_PERIPH_REQ_TYPE(lliCfg->srcPeriph));

    /* Disable clock gate */
    GLB_AHB_Slave1_Clock_Gate(DISABLE, BL_AHB_SLAVE1_DMA);

    tmpVal = BL_RD_REG(DMAChs, DMA_CONFIG);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DMA_FLOWCNTRL, lliCfg->dir);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DMA_DSTPERIPHERAL, lliCfg->dstPeriph);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DMA_SRCPERIPHERAL, lliCfg->srcPeriph);
    BL_WR_REG(DMAChs, DMA_CONFIG, tmpVal);
}

/****************************************************************************/ /**
 * @brief  DMA channel update LLI
 *
 * @param  ch: DMA channel number
 * @param  LLI: LLI addr
 *
 * @return None
 *
*******************************************************************************/
void DMA_LLI_Update(uint8_t ch, uint32_t LLI)
{
    /* Get channel register */
    uint32_t DMAChs = DMA_Get_Channel(ch);

    /* Check the parameters */
    CHECK_PARAM(IS_DMA_CHAN_TYPE(ch));

    /* Config channel config */
    //BL_WR_REG(DMAChs, DMA_LLI, LLI);
    BL702_MemCpy4((uint32_t *)DMAChs, (uint32_t *)LLI, 4);
}

/****************************************************************************/ /**
 * @brief  DMA LLI PingPong Structure Start
 *
 * @param  dmaPpStruct: dma pp struct pointer
 * @param  Ping_Transfer_len: ping len
 * @param  Pong_Transfer_len: pong len
 *
 * @return Succrss or not
 *
*******************************************************************************/
BL_Err_Type DMA_LLI_PpStruct_Set_Transfer_Len(DMA_LLI_PP_Struct *dmaPpStruct, uint16_t Ping_Transfer_len, uint16_t Pong_Transfer_len)
{
    struct DMA_Control_Reg dmaCtrlRegVal_temp;

    if (Ping_Transfer_len > 4096 || Pong_Transfer_len > 4096) {
        return ERROR;
    }

    dmaCtrlRegVal_temp = PingPongListArra[dmaPpStruct->dmaChan][PING_INDEX].dmaCtrl;
    dmaCtrlRegVal_temp.TransferSize = Ping_Transfer_len;
    PingPongListArra[dmaPpStruct->dmaChan][PING_INDEX].dmaCtrl = dmaCtrlRegVal_temp;

    dmaCtrlRegVal_temp = PingPongListArra[dmaPpStruct->dmaChan][PONG_INDEX].dmaCtrl;
    dmaCtrlRegVal_temp.TransferSize = Pong_Transfer_len;
    PingPongListArra[dmaPpStruct->dmaChan][PONG_INDEX].dmaCtrl = dmaCtrlRegVal_temp;

    DMA_LLI_Init(dmaPpStruct->dmaChan, dmaPpStruct->DMA_LLI_Cfg);
    DMA_LLI_Update(dmaPpStruct->dmaChan, (uint32_t)&PingPongListArra[dmaPpStruct->dmaChan][PING_INDEX]);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  DMA LLI Start New Transmit for Ping-Pong Buf
 *
 * @param  dmaPpBuf: DMA LLI Ping-Pong Buf
 *
 * @return None
 *
*******************************************************************************/
void DMA_LLI_PpBuf_Start_New_Transmit(DMA_LLI_PP_Buf *dmaPpBuf)
{
    CPU_Interrupt_Disable(DMA_ALL_IRQn);

    if (dmaPpBuf->lliListHeader[dmaPpBuf->idleIndex] != NULL) {
        DMA_LLI_Update(dmaPpBuf->dmaChan, (uint32_t)dmaPpBuf->lliListHeader[dmaPpBuf->idleIndex]);
        DMA_Channel_Enable(dmaPpBuf->dmaChan);
        dmaPpBuf->idleIndex = (dmaPpBuf->idleIndex == 0) ? 1 : 0;
    }

    CPU_Interrupt_Enable(DMA_ALL_IRQn);
}

/****************************************************************************/ /**
 * @brief  DMA LLI Remove Completed Ping-Pong Buf List
 *
 * @param  dmaPpBuf: DMA LLI Ping-Pong Buf
 *
 * @return Next Ping-Pong Buf List Header
 *
*******************************************************************************/
DMA_LLI_Ctrl_Type *DMA_LLI_PpBuf_Remove_Completed_List(DMA_LLI_PP_Buf *dmaPpBuf)
{
    CPU_Interrupt_Disable(DMA_ALL_IRQn);

    dmaPpBuf->lliListHeader[!dmaPpBuf->idleIndex] = NULL;
    CPU_Interrupt_Enable(DMA_ALL_IRQn);
    return dmaPpBuf->lliListHeader[!dmaPpBuf->idleIndex];
}

/****************************************************************************/ /**
 * @brief  DMA LLI Append Buf to List
 *
 * @param  dmaPpBuf: DMA LLI Ping-Pong Buf
 * @param  dmaLliList: New LLI Buf to Append
 *
 * @return None
 *
*******************************************************************************/
void DMA_LLI_PpBuf_Append(DMA_LLI_PP_Buf *dmaPpBuf, DMA_LLI_Ctrl_Type *dmaLliList)
{
    DMA_LLI_Ctrl_Type *pLliList = NULL;
    CPU_Interrupt_Disable(DMA_ALL_IRQn);

    pLliList = dmaPpBuf->lliListHeader[dmaPpBuf->idleIndex];

    if (pLliList == NULL) {
        dmaLliList->nextLLI = 0;
        dmaLliList->dmaCtrl.I = 1;
        dmaPpBuf->lliListHeader[dmaPpBuf->idleIndex] = dmaLliList;
    } else {
        /*Append to last */
        while (pLliList->nextLLI != 0) {
            pLliList = (DMA_LLI_Ctrl_Type *)pLliList->nextLLI;
        }

        pLliList->nextLLI = (uint32_t)dmaLliList;
        pLliList->dmaCtrl.I = 0;
        dmaLliList->nextLLI = 0;
        dmaLliList->dmaCtrl.I = 1;
    }

    if (DMA_Channel_Is_Busy(dmaPpBuf->dmaChan) == RESET) {
        /* DMA stopped: maybe stop just a few minutes ago(not enter INT due to CPU_Interrupt_Disable)
           or has already stopped before this function is called */
        if (dmaPpBuf->lliListHeader[!dmaPpBuf->idleIndex] == NULL) {
            /* DMA has already stopped before this function is called */
            DMA_LLI_PpBuf_Start_New_Transmit(dmaPpBuf);
        }
    }

    CPU_Interrupt_Enable(DMA_ALL_IRQn);
}

/****************************************************************************/ /**
 * @brief  DMA LLi Destroy Ping-Pong Buf
 *
 * @param  dmaPpBuf: DMA LLI Ping-Pong Buf
 *
 * @return None
 *
*******************************************************************************/
void DMA_LLI_PpBuf_Destroy(DMA_LLI_PP_Buf *dmaPpBuf)
{
    /* DMA LLI Disable */
    DMA_Channel_Disable(dmaPpBuf->dmaChan);

    if (dmaPpBuf->lliListHeader[0] != NULL && dmaPpBuf->onTransCompleted != NULL) {
        dmaPpBuf->onTransCompleted(dmaPpBuf->lliListHeader[0]);
    }

    dmaPpBuf->lliListHeader[0] = NULL;

    if (dmaPpBuf->lliListHeader[1] != NULL && dmaPpBuf->onTransCompleted != NULL) {
        dmaPpBuf->onTransCompleted(dmaPpBuf->lliListHeader[1]);
    }

    dmaPpBuf->lliListHeader[1] = NULL;
    dmaPpBuf->idleIndex = 0;
}

/****************************************************************************/ /**
 * @brief  Mask/Unmask the DMA interrupt
 *
 * @param  ch: DMA channel number
 * @param  intType: Specifies the interrupt type
 * @param  intMask: Enable/Disable Specified interrupt type
 *
 * @return None
 *
*******************************************************************************/
void DMA_IntMask(uint8_t ch, DMA_INT_Type intType, BL_Mask_Type intMask)
{
    uint32_t tmpVal;
    /* Get channel register */
    uint32_t DMAChs = DMA_Get_Channel(ch);

    /* Check the parameters */
    CHECK_PARAM(IS_DMA_CHAN_TYPE(ch));
    CHECK_PARAM(IS_DMA_INT_TYPE(intType));

    switch (intType) {
        case DMA_INT_TCOMPLETED:
            if (intMask == UNMASK) {
                /* UNMASK(Enable) this interrupt */
                tmpVal = BL_CLR_REG_BIT(BL_RD_REG(DMAChs, DMA_CONFIG), DMA_ITC);
                BL_WR_REG(DMAChs, DMA_CONFIG, tmpVal);
                tmpVal = BL_SET_REG_BIT(BL_RD_REG(DMAChs, DMA_CONTROL), DMA_I);
                BL_WR_REG(DMAChs, DMA_CONTROL, tmpVal);
            } else {
                /* MASK(Disable) this interrupt */
                tmpVal = BL_SET_REG_BIT(BL_RD_REG(DMAChs, DMA_CONFIG), DMA_ITC);
                BL_WR_REG(DMAChs, DMA_CONFIG, tmpVal);
                tmpVal = BL_CLR_REG_BIT(BL_RD_REG(DMAChs, DMA_CONTROL), DMA_I);
                BL_WR_REG(DMAChs, DMA_CONTROL, tmpVal);
            }

            break;

        case DMA_INT_ERR:
            if (intMask == UNMASK) {
                /* UNMASK(Enable) this interrupt */
                tmpVal = BL_CLR_REG_BIT(BL_RD_REG(DMAChs, DMA_CONFIG), DMA_IE);
                BL_WR_REG(DMAChs, DMA_CONFIG, tmpVal);
            } else {
                /* MASK(Disable) this interrupt */
                tmpVal = BL_SET_REG_BIT(BL_RD_REG(DMAChs, DMA_CONFIG), DMA_IE);
                BL_WR_REG(DMAChs, DMA_CONFIG, tmpVal);
            }

            break;

        case DMA_INT_ALL:
            if (intMask == UNMASK) {
                /* UNMASK(Enable) this interrupt */
                tmpVal = BL_RD_REG(DMAChs, DMA_CONFIG);
                tmpVal = BL_CLR_REG_BIT(tmpVal, DMA_ITC);
                tmpVal = BL_CLR_REG_BIT(tmpVal, DMA_IE);
                BL_WR_REG(DMAChs, DMA_CONFIG, tmpVal);
                tmpVal = BL_RD_REG(DMAChs, DMA_CONTROL);
                tmpVal = BL_SET_REG_BIT(tmpVal, DMA_I);
                BL_WR_REG(DMAChs, DMA_CONTROL, tmpVal);
            } else {
                /* MASK(Disable) this interrupt */
                tmpVal = BL_RD_REG(DMAChs, DMA_CONFIG);
                tmpVal = BL_SET_REG_BIT(tmpVal, DMA_ITC);
                tmpVal = BL_SET_REG_BIT(tmpVal, DMA_IE);
                BL_WR_REG(DMAChs, DMA_CONFIG, tmpVal);
                tmpVal = BL_RD_REG(DMAChs, DMA_CONTROL);
                tmpVal = BL_CLR_REG_BIT(tmpVal, DMA_I);
                BL_WR_REG(DMAChs, DMA_CONTROL, tmpVal);
            }

            break;

        default:
            break;
    }
}

/****************************************************************************/ /**
 * @brief  Install DMA interrupt callback function
 *
 * @param  dmaChan: DMA Channel type
 * @param  intType: DMA interrupt type
 * @param  cbFun: Pointer to interrupt callback function. The type should be void (*fn)(void)
 *
 * @return None
 *
*******************************************************************************/
void DMA_Int_Callback_Install(DMA_Chan_Type dmaChan, DMA_INT_Type intType, intCallback_Type *cbFun)
{
    /* Check the parameters */
    CHECK_PARAM(IS_DMA_CHAN_TYPE(dmaChan));
    CHECK_PARAM(IS_DMA_INT_TYPE(intType));

    dmaIntCbfArra[dmaChan][intType] = cbFun;
}

/****************************************************************************/ /**
 * @brief  DMA LLI PingPong Structure Initial
 *
 * @param  dmaPpStruct: DMA LLI PingPong Config Parameter
 *
 * @return start success or not
 *
*******************************************************************************/
BL_Err_Type DMA_LLI_PpStruct_Init(DMA_LLI_PP_Struct *dmaPpStruct)
{
    //setup lliList
    dmaPpStruct->dmaCtrlRegVal.I = 1;
    dmaPpStruct->trans_index = 0;

    if (dmaPpStruct->DMA_LLI_Cfg->dir == DMA_TRNS_M2P) {
        PingPongListArra[dmaPpStruct->dmaChan][PING_INDEX].srcDmaAddr = dmaPpStruct->chache_buf_addr[0];
        PingPongListArra[dmaPpStruct->dmaChan][PING_INDEX].destDmaAddr = dmaPpStruct->operatePeriphAddr;

        PingPongListArra[dmaPpStruct->dmaChan][PONG_INDEX].srcDmaAddr = dmaPpStruct->chache_buf_addr[1];
        PingPongListArra[dmaPpStruct->dmaChan][PONG_INDEX].destDmaAddr = dmaPpStruct->operatePeriphAddr;
    } else if (dmaPpStruct->DMA_LLI_Cfg->dir == DMA_TRNS_P2M) {
        PingPongListArra[dmaPpStruct->dmaChan][PING_INDEX].srcDmaAddr = dmaPpStruct->operatePeriphAddr;
        PingPongListArra[dmaPpStruct->dmaChan][PING_INDEX].destDmaAddr = dmaPpStruct->chache_buf_addr[0];

        PingPongListArra[dmaPpStruct->dmaChan][PONG_INDEX].srcDmaAddr = dmaPpStruct->operatePeriphAddr;
        PingPongListArra[dmaPpStruct->dmaChan][PONG_INDEX].destDmaAddr = dmaPpStruct->chache_buf_addr[1];
    } else {
        return ERROR;
        /*V1.0 version DMA LLI Ping-Pong structure not support P2P & M2M MODE*/
    }

    PingPongListArra[dmaPpStruct->dmaChan][PING_INDEX].nextLLI = (uint32_t)&PingPongListArra[dmaPpStruct->dmaChan][PONG_INDEX];
    PingPongListArra[dmaPpStruct->dmaChan][PING_INDEX].dmaCtrl = dmaPpStruct->dmaCtrlRegVal;

    PingPongListArra[dmaPpStruct->dmaChan][PONG_INDEX].nextLLI = (uint32_t)&PingPongListArra[dmaPpStruct->dmaChan][PING_INDEX];
    PingPongListArra[dmaPpStruct->dmaChan][PONG_INDEX].dmaCtrl = dmaPpStruct->dmaCtrlRegVal;

    DMA_LLI_Init(dmaPpStruct->dmaChan, dmaPpStruct->DMA_LLI_Cfg);

    DMA_LLI_Update(dmaPpStruct->dmaChan, (uint32_t)&PingPongListArra[dmaPpStruct->dmaChan][PING_INDEX]);

    return SUCCESS;
}

/****************************************************************************/ /**
 * @brief  DMA LLI PingPong Structure Start
 *
 * @param  dmaPpStruct: None
 *
 * @return None
 *
*******************************************************************************/
void DMA_LLI_PpStruct_Start(DMA_LLI_PP_Struct *dmaPpStruct)
{
    DMA_Channel_Enable(dmaPpStruct->dmaChan);
}

/****************************************************************************/ /**
 * @brief  DMA LLI PingPong Structure Stop
 *
 * @param  dmaPpStruct: None
 *
 * @return None
 *
*******************************************************************************/
void DMA_LLI_PpStruct_Stop(DMA_LLI_PP_Struct *dmaPpStruct)
{
    DMA_Channel_Disable(dmaPpStruct->dmaChan);
}

/*@} end of group DMA_Public_Functions */

/*@} end of group DMA */

/*@} end of group BL702_Peripheral_Driver */
