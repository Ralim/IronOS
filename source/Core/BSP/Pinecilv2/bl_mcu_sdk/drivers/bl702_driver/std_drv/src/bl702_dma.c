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

#include "bl702_dma.h"
#include "bl702.h"
#include "bl702_glb.h"
#include "string.h"

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
#define DMA_Get_Channel(ch) (DMA_BASE + DMA_CHANNEL_OFFSET + (ch) * 0x100)
static intCallback_Type *dmaIntCbfArra[DMA_CH_MAX][DMA_INT_ALL] = {
    {NULL, NULL},
    {NULL, NULL},
    {NULL, NULL},
    {NULL, NULL}
};
// static DMA_LLI_Ctrl_Type PingPongListArra[DMA_CH_MAX][2];

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

/**
 * @brief  DMA interrupt handler
 *
 * @param  None
 *
 * @return None
 *
 *******************************************************************************/
#ifndef BFLB_USE_HAL_DRIVER
void DMA_ALL_IRQHandler(void) {
  uint32_t tmpVal;
  uint32_t intClr;
  uint8_t  ch;
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

/**
 * @brief  DMA enable
 *
 * @param  None
 *
 * @return None
 *
 *******************************************************************************/
void DMA_Enable(void) {
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

/**
 * @brief  DMA disable
 *
 * @param  None
 *
 * @return None
 *
 *******************************************************************************/
void DMA_Disable(void) {
  uint32_t tmpVal;
  /* Get DMA register */
  uint32_t DMAChs = DMA_BASE;

  tmpVal = BL_RD_REG(DMAChs, DMA_TOP_CONFIG);
  tmpVal = BL_CLR_REG_BIT(tmpVal, DMA_E);
  BL_WR_REG(DMAChs, DMA_TOP_CONFIG, tmpVal);
}

/**
 * @brief  DMA channel init
 *
 * @param  chCfg: DMA configuration
 *
 * @return None
 *
 *******************************************************************************/
void DMA_Channel_Init(DMA_Channel_Cfg_Type *chCfg) {
  uint32_t tmpVal;
  /* Get channel register */
  uint32_t DMAChs = DMA_Get_Channel(chCfg->ch);

  /* Check the parameters */
  CHECK_PARAM(IS_DMA_CHAN_TYPE(chCfg->ch));
  CHECK_PARAM(IS_DMA_TRANS_WIDTH_TYPE(chCfg->srcTransfWidth));
  CHECK_PARAM(IS_DMA_TRANS_WIDTH_TYPE(chCfg->dstTransfWidth));
  CHECK_PARAM(IS_DMA_BURST_SIZE_TYPE(chCfg->srcBurstSize));
  CHECK_PARAM(IS_DMA_BURST_SIZE_TYPE(chCfg->dstBurstSize));
  CHECK_PARAM(IS_DMA_TRANS_DIR_TYPE(chCfg->dir));
  CHECK_PARAM(IS_DMA_PERIPH_REQ_TYPE(chCfg->dstPeriph));
  CHECK_PARAM(IS_DMA_PERIPH_REQ_TYPE(chCfg->srcPeriph));

  /* Disable clock gate */
  // Turns on clock
  GLB_AHB_Slave1_Clock_Gate(DISABLE, BL_AHB_SLAVE1_DMA);

  /* Config channel config */
  BL_WR_REG(DMAChs, DMA_SRCADDR, chCfg->srcDmaAddr);
  BL_WR_REG(DMAChs, DMA_DSTADDR, chCfg->destDmaAddr);

  tmpVal = BL_RD_REG(DMAChs, DMA_CONTROL);
  tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DMA_TRANSFERSIZE, chCfg->transfLength);
  tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DMA_SWIDTH, chCfg->srcTransfWidth);
  tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DMA_DWIDTH, chCfg->dstTransfWidth);
  tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DMA_SBSIZE, chCfg->srcBurstSize);
  tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DMA_DBSIZE, chCfg->dstBurstSize);

  tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DMA_DST_ADD_MODE, chCfg->dstAddMode);
  tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DMA_DST_MIN_MODE, chCfg->dstMinMode);
  tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DMA_FIX_CNT, chCfg->fixCnt);

  /* FIXME: how to deal with SLargerD */
  tmpVal = BL_CLR_REG_BIT(tmpVal, DMA_SLARGERD); // Reserved bit 25
  tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DMA_SI, chCfg->srcAddrInc);
  tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DMA_DI, chCfg->destAddrInc);
  BL_WR_REG(DMAChs, DMA_CONTROL, tmpVal);

  tmpVal = BL_RD_REG(DMAChs, DMA_CONFIG);
  tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DMA_FLOWCNTRL, chCfg->dir);
  tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DMA_DSTPERIPHERAL, chCfg->dstPeriph);
  tmpVal = BL_SET_REG_BITS_VAL(tmpVal, DMA_SRCPERIPHERAL, chCfg->srcPeriph);
  BL_WR_REG(DMAChs, DMA_CONFIG, tmpVal);
  // Clear interrupts
  *((uint32_t *)0x4000c008) = 1 << (chCfg->ch); // Clear transfer complete
  *((uint32_t *)0x4000c010) = 1 << (chCfg->ch); // Clear Error
}

/**
 * @brief  DMA channel update source memory address and len
 *
 * @param  ch: DMA channel
 * @param  memAddr: source memoty address
 * @param  len: source memory data length
 *
 * @return None
 *
 *******************************************************************************/
void DMA_Channel_Update_SrcMemcfg(uint8_t ch, uint32_t memAddr, uint32_t len) {
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

/**
 * @brief  DMA channel update destination memory address and len
 *
 * @param  ch: DMA channel
 * @param  memAddr: destination memoty address
 * @param  len: destination memory data length
 *
 * @return None
 *
 *******************************************************************************/
void DMA_Channel_Update_DstMemcfg(uint8_t ch, uint32_t memAddr, uint32_t len) {
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

/**
 * @brief  Get DMA channel tranfersize
 *
 * @param  ch: DMA channel
 *
 * @return tranfersize size
 *
 *******************************************************************************/
uint32_t DMA_Channel_TranferSize(uint8_t ch) {
  /* Get channel register */
  uint32_t DMAChs = DMA_Get_Channel(ch);

  /* Check the parameters */
  CHECK_PARAM(IS_DMA_CHAN_TYPE(ch));

  return BL_GET_REG_BITS_VAL(BL_RD_REG(DMAChs, DMA_CONTROL), DMA_TRANSFERSIZE);
}

/**
 * @brief  Get DMA channel busy status
 *
 * @param  ch: DMA channel
 *
 * @return SET or RESET
 *
 *******************************************************************************/
BL_Sts_Type DMA_Channel_Is_Busy(uint8_t ch) {
  /* Get channel register */
  uint32_t DMAChs = DMA_Get_Channel(ch);

  /* Check the parameters */
  CHECK_PARAM(IS_DMA_CHAN_TYPE(ch));

  return BL_IS_REG_BIT_SET(BL_RD_REG(DMAChs, DMA_CONFIG), DMA_E) == 1 ? SET : RESET;
}

/**
 * @brief  DMA enable
 *
 * @param  ch: DMA channel number
 *
 * @return None
 *
 *******************************************************************************/
void DMA_Channel_Enable(uint8_t ch) {
  uint32_t tmpVal;
  /* Get channel register */
  uint32_t DMAChs = DMA_Get_Channel(ch);

  /* Check the parameters */
  CHECK_PARAM(IS_DMA_CHAN_TYPE(ch));

  tmpVal = BL_RD_REG(DMAChs, DMA_CONFIG);
  tmpVal = BL_SET_REG_BIT(tmpVal, DMA_E);
  BL_WR_REG(DMAChs, DMA_CONFIG, tmpVal);
}

/**
 * @brief  DMA disable
 *
 * @param  ch: DMA channel number
 *
 * @return None
 *
 *******************************************************************************/
void DMA_Channel_Disable(uint8_t ch) {
  uint32_t tmpVal;
  /* Get channel register */
  uint32_t DMAChs = DMA_Get_Channel(ch);

  /* Check the parameters */
  CHECK_PARAM(IS_DMA_CHAN_TYPE(ch));

  tmpVal = BL_RD_REG(DMAChs, DMA_CONFIG);
  tmpVal = BL_CLR_REG_BIT(tmpVal, DMA_E);
  BL_WR_REG(DMAChs, DMA_CONFIG, tmpVal);
}

/**
 * @brief  DMA init LLI transfer
 *
 * @param  ch: DMA channel number
 * @param  lliCfg: LLI configuration
 *
 * @return None
 *
 *******************************************************************************/
void DMA_LLI_Init(uint8_t ch, DMA_LLI_Cfg_Type *lliCfg) {
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

/**
 * @brief  DMA channel update LLI
 *
 * @param  ch: DMA channel number
 * @param  LLI: LLI addr
 *
 * @return None
 *
 *******************************************************************************/
void DMA_LLI_Update(uint8_t ch, uint32_t LLI) {
  /* Get channel register */
  uint32_t DMAChs = DMA_Get_Channel(ch);

  /* Check the parameters */
  CHECK_PARAM(IS_DMA_CHAN_TYPE(ch));

  /* Config channel config */
  // BL_WR_REG(DMAChs, DMA_LLI, LLI);
  BL702_MemCpy4((uint32_t *)DMAChs, (uint32_t *)LLI, 4);
}

/**
 * @brief  Mask/Unmask the DMA interrupt
 *
 * @param  ch: DMA channel number
 * @param  intType: Specifies the interrupt type
 * @param  intMask: Enable/Disable Specified interrupt type
 *
 * @return None
 *
 *******************************************************************************/
void DMA_IntMask(uint8_t ch, DMA_INT_Type intType, BL_Mask_Type intMask) {
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

/**
 * @brief  Install DMA interrupt callback function
 *
 * @param  dmaChan: DMA Channel type
 * @param  intType: DMA interrupt type
 * @param  cbFun: Pointer to interrupt callback function. The type should be void (*fn)(void)
 *
 * @return None
 *
 *******************************************************************************/
void DMA_Int_Callback_Install(DMA_Chan_Type dmaChan, DMA_INT_Type intType, intCallback_Type *cbFun) {
  /* Check the parameters */
  CHECK_PARAM(IS_DMA_CHAN_TYPE(dmaChan));
  CHECK_PARAM(IS_DMA_INT_TYPE(intType));

  dmaIntCbfArra[dmaChan][intType] = cbFun;
}

/*@} end of group DMA_Public_Functions */

/*@} end of group DMA */

/*@} end of group BL702_Peripheral_Driver */
