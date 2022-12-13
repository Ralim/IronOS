/**
  ******************************************************************************
  * @file    bl70x_usb.c
  * @version V1.0
  * @date
  * @brief   This file is the standard driver c file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; COPYRIGHT(c) 2019 Bouffalo Lab</center></h2>
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

#include "bl702_usb.h"
#include "bl702_glb.h"
#include "bl702_common.h"

/** @addtogroup  BL70X_Peripheral_Driver
 *  @{
 */

/** @addtogroup  USB
 *  @{
 */

/** @defgroup  USB_Private_Macros
 *  @{
 */

/*@} end of group USB_Private_Macros */

/** @defgroup  USB_Private_Types
 *  @{
 */

/*@} end of group USB_Private_Types */

/** @defgroup  USB_Private_Variables
 *  @{
 */

/*@} end of group USB_Private_Variables */

/** @defgroup  USB_Global_Variables
 *  @{
 */

/*@} end of group USB_Global_Variables */

/** @defgroup  USB_Private_Fun_Declaration
 *  @{
 */

/*@} end of group USB_Private_Fun_Declaration */

/** @defgroup  USB_Private_Functions
 *  @{
 */

/*@} end of group USB_Private_Functions */

/** @defgroup  USB_Public_Functions
 *  @{
 */

BL_Err_Type USB_Enable(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(USB_BASE, USB_CONFIG);
    tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_USB_EN);
    BL_WR_REG(USB_BASE, USB_CONFIG, tmpVal);

    return SUCCESS;
}

BL_Err_Type USB_Disable(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(USB_BASE, USB_CONFIG);
    tmpVal = BL_CLR_REG_BIT(tmpVal, USB_CR_USB_EN);
    BL_WR_REG(USB_BASE, USB_CONFIG, tmpVal);

    return SUCCESS;
}

BL_Err_Type USB_Set_Config(BL_Fun_Type enable, USB_Config_Type *usbCfg)
{
    uint32_t tmpVal = 0;

    /* disable USB first */
    tmpVal = BL_RD_REG(USB_BASE, USB_CONFIG);
    tmpVal = BL_CLR_REG_BIT(tmpVal, USB_CR_USB_EN);
    BL_WR_REG(USB_BASE, USB_CONFIG, tmpVal);

    /* USB config */
    tmpVal = BL_RD_REG(USB_BASE, USB_CONFIG);

    if (usbCfg->SoftwareCtrl == ENABLE) {
        tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_USB_EP0_SW_CTRL);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, USB_CR_USB_EP0_SW_ADDR, usbCfg->DeviceAddress);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, USB_CR_USB_EP0_SW_SIZE, usbCfg->EnumMaxPacketSize);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, USB_CR_USB_EP0_SW_NACK_IN, usbCfg->EnumInEn);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, USB_CR_USB_EP0_SW_NACK_OUT, usbCfg->EnumOutEn);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, USB_CR_USB_ROM_DCT_EN, usbCfg->RomBaseDescriptorUsed);
    } else {
        tmpVal = BL_CLR_REG_BIT(tmpVal, USB_CR_USB_EP0_SW_CTRL);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, USB_CR_USB_ROM_DCT_EN, usbCfg->RomBaseDescriptorUsed);
    }

    BL_WR_REG(USB_BASE, USB_CONFIG, tmpVal);

    /* enable/disable USB */
    tmpVal = BL_RD_REG(USB_BASE, USB_CONFIG);

    if (enable) {
        tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_USB_EN);
    }

    BL_WR_REG(USB_BASE, USB_CONFIG, tmpVal);

    return SUCCESS;
}

BL_Err_Type USB_Set_Device_Addr(uint8_t addr)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(USB_BASE, USB_CONFIG);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, USB_CR_USB_EP0_SW_ADDR, addr);
    BL_WR_REG(USB_BASE, USB_CONFIG, tmpVal);

    return SUCCESS;
}

uint8_t USB_Get_Device_Addr(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(USB_BASE, USB_CONFIG);

    return BL_GET_REG_BITS_VAL(tmpVal, USB_CR_USB_EP0_SW_ADDR);
}

BL_Err_Type USB_Set_EPx_Xfer_Size(USB_EP_ID epId, uint8_t size)
{
    uint32_t tmpVal = 0;

    if (epId == EP_ID0) {
        tmpVal = BL_RD_REG(USB_BASE, USB_CONFIG);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, USB_CR_USB_EP0_SW_SIZE, size);
        BL_WR_REG(USB_BASE, USB_CONFIG, tmpVal);
    } else {
        switch (epId) {
            case EP_ID1:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP1_CONFIG);
                tmpVal = BL_SET_REG_BITS_VAL(tmpVal, USB_CR_EP1_SIZE, size);
                BL_WR_REG(USB_BASE, USB_EP1_CONFIG, tmpVal);
                break;

            case EP_ID2:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP2_CONFIG);
                tmpVal = BL_SET_REG_BITS_VAL(tmpVal, USB_CR_EP2_SIZE, size);
                BL_WR_REG(USB_BASE, USB_EP2_CONFIG, tmpVal);
                break;

            case EP_ID3:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP3_CONFIG);
                tmpVal = BL_SET_REG_BITS_VAL(tmpVal, USB_CR_EP3_SIZE, size);
                BL_WR_REG(USB_BASE, USB_EP3_CONFIG, tmpVal);
                break;

            case EP_ID4:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP4_CONFIG);
                tmpVal = BL_SET_REG_BITS_VAL(tmpVal, USB_CR_EP4_SIZE, size);
                BL_WR_REG(USB_BASE, USB_EP4_CONFIG, tmpVal);
                break;

            case EP_ID5:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP5_CONFIG);
                tmpVal = BL_SET_REG_BITS_VAL(tmpVal, USB_CR_EP5_SIZE, size);
                BL_WR_REG(USB_BASE, USB_EP5_CONFIG, tmpVal);
                break;

            case EP_ID6:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP6_CONFIG);
                tmpVal = BL_SET_REG_BITS_VAL(tmpVal, USB_CR_EP6_SIZE, size);
                BL_WR_REG(USB_BASE, USB_EP6_CONFIG, tmpVal);
                break;

            case EP_ID7:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP7_CONFIG);
                tmpVal = BL_SET_REG_BITS_VAL(tmpVal, USB_CR_EP7_SIZE, size);
                BL_WR_REG(USB_BASE, USB_EP7_CONFIG, tmpVal);
                break;

            default:
                break;
        }
    }

    return SUCCESS;
}

BL_Err_Type USB_Set_EPx_IN_Busy(USB_EP_ID epId)
{
    uint32_t tmpVal = 0;

    if (epId == EP_ID0) {
        tmpVal = BL_RD_REG(USB_BASE, USB_CONFIG);
        tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_USB_EP0_SW_NACK_IN);
        tmpVal = BL_CLR_REG_BIT(tmpVal, USB_CR_USB_EP0_SW_STALL);
        BL_WR_REG(USB_BASE, USB_CONFIG, tmpVal);
    } else {
        switch (epId) {
            case EP_ID1:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP1_CONFIG);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP1_NACK);
                tmpVal = BL_CLR_REG_BIT(tmpVal, USB_CR_EP1_STALL);
                BL_WR_REG(USB_BASE, USB_EP1_CONFIG, tmpVal);
                break;

            case EP_ID2:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP2_CONFIG);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP2_NACK);
                tmpVal = BL_CLR_REG_BIT(tmpVal, USB_CR_EP2_STALL);
                BL_WR_REG(USB_BASE, USB_EP2_CONFIG, tmpVal);
                break;

            case EP_ID3:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP3_CONFIG);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP3_NACK);
                tmpVal = BL_CLR_REG_BIT(tmpVal, USB_CR_EP3_STALL);
                BL_WR_REG(USB_BASE, USB_EP3_CONFIG, tmpVal);
                break;

            case EP_ID4:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP4_CONFIG);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP4_NACK);
                tmpVal = BL_CLR_REG_BIT(tmpVal, USB_CR_EP4_STALL);
                BL_WR_REG(USB_BASE, USB_EP4_CONFIG, tmpVal);
                break;

            case EP_ID5:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP5_CONFIG);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP5_NACK);
                tmpVal = BL_CLR_REG_BIT(tmpVal, USB_CR_EP5_STALL);
                BL_WR_REG(USB_BASE, USB_EP5_CONFIG, tmpVal);
                break;

            case EP_ID6:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP6_CONFIG);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP6_NACK);
                tmpVal = BL_CLR_REG_BIT(tmpVal, USB_CR_EP6_STALL);
                BL_WR_REG(USB_BASE, USB_EP6_CONFIG, tmpVal);
                break;

            case EP_ID7:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP7_CONFIG);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP7_NACK);
                tmpVal = BL_CLR_REG_BIT(tmpVal, USB_CR_EP7_STALL);
                BL_WR_REG(USB_BASE, USB_EP7_CONFIG, tmpVal);
                break;

            default:
                break;
        }
    }

    return SUCCESS;
}

BL_Err_Type USB_Set_EPx_IN_Stall(USB_EP_ID epId)
{
    uint32_t tmpVal = 0;

    if (epId == EP_ID0) {
        tmpVal = BL_RD_REG(USB_BASE, USB_CONFIG);
        tmpVal = BL_CLR_REG_BIT(tmpVal, USB_CR_USB_EP0_SW_NACK_OUT);
        tmpVal = BL_CLR_REG_BIT(tmpVal, USB_CR_USB_EP0_SW_NACK_IN);
        tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_USB_EP0_SW_STALL);
        BL_WR_REG(USB_BASE, USB_CONFIG, tmpVal);
    } else {
        switch (epId) {
            case EP_ID1:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP1_CONFIG);
                tmpVal = BL_CLR_REG_BIT(tmpVal, USB_CR_EP1_NACK);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP1_STALL);
                BL_WR_REG(USB_BASE, USB_EP1_CONFIG, tmpVal);
                break;

            case EP_ID2:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP2_CONFIG);
                tmpVal = BL_CLR_REG_BIT(tmpVal, USB_CR_EP2_NACK);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP2_STALL);
                BL_WR_REG(USB_BASE, USB_EP2_CONFIG, tmpVal);
                break;

            case EP_ID3:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP3_CONFIG);
                tmpVal = BL_CLR_REG_BIT(tmpVal, USB_CR_EP3_NACK);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP3_STALL);
                BL_WR_REG(USB_BASE, USB_EP3_CONFIG, tmpVal);
                break;

            case EP_ID4:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP4_CONFIG);
                tmpVal = BL_CLR_REG_BIT(tmpVal, USB_CR_EP4_NACK);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP4_STALL);
                BL_WR_REG(USB_BASE, USB_EP4_CONFIG, tmpVal);
                break;

            case EP_ID5:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP5_CONFIG);
                tmpVal = BL_CLR_REG_BIT(tmpVal, USB_CR_EP5_NACK);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP5_STALL);
                BL_WR_REG(USB_BASE, USB_EP5_CONFIG, tmpVal);
                break;

            case EP_ID6:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP6_CONFIG);
                tmpVal = BL_CLR_REG_BIT(tmpVal, USB_CR_EP6_NACK);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP6_STALL);
                BL_WR_REG(USB_BASE, USB_EP6_CONFIG, tmpVal);
                break;

            case EP_ID7:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP7_CONFIG);
                tmpVal = BL_CLR_REG_BIT(tmpVal, USB_CR_EP7_NACK);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP7_STALL);
                BL_WR_REG(USB_BASE, USB_EP7_CONFIG, tmpVal);
                break;

            default:
                break;
        }
    }

    return SUCCESS;
}

BL_Err_Type USB_Set_EPx_OUT_Busy(USB_EP_ID epId)
{
    uint32_t tmpVal = 0;

    if (epId == EP_ID0) {
        tmpVal = BL_RD_REG(USB_BASE, USB_CONFIG);
        tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_USB_EP0_SW_NACK_OUT);
        tmpVal = BL_CLR_REG_BIT(tmpVal, USB_CR_USB_EP0_SW_STALL);
        BL_WR_REG(USB_BASE, USB_CONFIG, tmpVal);
    } else {
        switch (epId) {
            case EP_ID1:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP1_CONFIG);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP1_NACK);
                tmpVal = BL_CLR_REG_BIT(tmpVal, USB_CR_EP1_STALL);
                BL_WR_REG(USB_BASE, USB_EP1_CONFIG, tmpVal);
                break;

            case EP_ID2:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP2_CONFIG);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP2_NACK);
                tmpVal = BL_CLR_REG_BIT(tmpVal, USB_CR_EP2_STALL);
                BL_WR_REG(USB_BASE, USB_EP2_CONFIG, tmpVal);
                break;

            case EP_ID3:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP3_CONFIG);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP3_NACK);
                tmpVal = BL_CLR_REG_BIT(tmpVal, USB_CR_EP3_STALL);
                BL_WR_REG(USB_BASE, USB_EP3_CONFIG, tmpVal);
                break;

            case EP_ID4:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP4_CONFIG);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP4_NACK);
                tmpVal = BL_CLR_REG_BIT(tmpVal, USB_CR_EP4_STALL);
                BL_WR_REG(USB_BASE, USB_EP4_CONFIG, tmpVal);
                break;

            case EP_ID5:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP5_CONFIG);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP5_NACK);
                tmpVal = BL_CLR_REG_BIT(tmpVal, USB_CR_EP5_STALL);
                BL_WR_REG(USB_BASE, USB_EP5_CONFIG, tmpVal);
                break;

            case EP_ID6:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP6_CONFIG);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP6_NACK);
                tmpVal = BL_CLR_REG_BIT(tmpVal, USB_CR_EP6_STALL);
                BL_WR_REG(USB_BASE, USB_EP6_CONFIG, tmpVal);
                break;

            case EP_ID7:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP7_CONFIG);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP7_NACK);
                tmpVal = BL_CLR_REG_BIT(tmpVal, USB_CR_EP7_STALL);
                BL_WR_REG(USB_BASE, USB_EP7_CONFIG, tmpVal);
                break;

            default:
                break;
        }
    }

    return SUCCESS;
}

BL_Err_Type USB_Set_EPx_OUT_Stall(USB_EP_ID epId)
{
    uint32_t tmpVal = 0;

    if (epId == EP_ID0) {
        tmpVal = BL_RD_REG(USB_BASE, USB_CONFIG);
        tmpVal = BL_CLR_REG_BIT(tmpVal, USB_CR_USB_EP0_SW_NACK_OUT);
        tmpVal = BL_CLR_REG_BIT(tmpVal, USB_CR_USB_EP0_SW_NACK_IN);
        tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_USB_EP0_SW_STALL);
        BL_WR_REG(USB_BASE, USB_CONFIG, tmpVal);
    } else {
        switch (epId) {
            case EP_ID1:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP1_CONFIG);
                tmpVal = BL_CLR_REG_BIT(tmpVal, USB_CR_EP1_NACK);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP1_STALL);
                BL_WR_REG(USB_BASE, USB_EP1_CONFIG, tmpVal);
                break;

            case EP_ID2:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP2_CONFIG);
                tmpVal = BL_CLR_REG_BIT(tmpVal, USB_CR_EP2_NACK);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP2_STALL);
                BL_WR_REG(USB_BASE, USB_EP2_CONFIG, tmpVal);
                break;

            case EP_ID3:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP3_CONFIG);
                tmpVal = BL_CLR_REG_BIT(tmpVal, USB_CR_EP3_NACK);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP3_STALL);
                BL_WR_REG(USB_BASE, USB_EP3_CONFIG, tmpVal);
                break;

            case EP_ID4:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP4_CONFIG);
                tmpVal = BL_CLR_REG_BIT(tmpVal, USB_CR_EP4_NACK);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP4_STALL);
                BL_WR_REG(USB_BASE, USB_EP4_CONFIG, tmpVal);
                break;

            case EP_ID5:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP5_CONFIG);
                tmpVal = BL_CLR_REG_BIT(tmpVal, USB_CR_EP5_NACK);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP5_STALL);
                BL_WR_REG(USB_BASE, USB_EP5_CONFIG, tmpVal);
                break;

            case EP_ID6:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP6_CONFIG);
                tmpVal = BL_CLR_REG_BIT(tmpVal, USB_CR_EP6_NACK);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP6_STALL);
                BL_WR_REG(USB_BASE, USB_EP6_CONFIG, tmpVal);
                break;

            case EP_ID7:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP7_CONFIG);
                tmpVal = BL_CLR_REG_BIT(tmpVal, USB_CR_EP7_NACK);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP7_STALL);
                BL_WR_REG(USB_BASE, USB_EP7_CONFIG, tmpVal);
                break;

            default:
                break;
        }
    }

    return SUCCESS;
}

BL_Err_Type USB_Set_EPx_Rdy(USB_EP_ID epId)
{
    uint32_t tmpVal = 0;

    if (epId == EP_ID0) {
        tmpVal = BL_RD_REG(USB_BASE, USB_CONFIG);
        tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_USB_EP0_SW_RDY);
        tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_USB_EP0_SW_NACK_OUT);
        tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_USB_EP0_SW_NACK_IN);
        tmpVal = BL_CLR_REG_BIT(tmpVal, USB_CR_USB_EP0_SW_STALL);
        BL_WR_REG(USB_BASE, USB_CONFIG, tmpVal);
    } else {
        switch (epId) {
            case EP_ID1:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP1_CONFIG);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP1_RDY);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP1_NACK);
                tmpVal = BL_CLR_REG_BIT(tmpVal, USB_CR_EP1_STALL);
                BL_WR_REG(USB_BASE, USB_EP1_CONFIG, tmpVal);
                break;

            case EP_ID2:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP2_CONFIG);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP2_RDY);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP2_NACK);
                tmpVal = BL_CLR_REG_BIT(tmpVal, USB_CR_EP2_STALL);
                BL_WR_REG(USB_BASE, USB_EP2_CONFIG, tmpVal);
                break;

            case EP_ID3:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP3_CONFIG);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP3_RDY);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP3_NACK);
                tmpVal = BL_CLR_REG_BIT(tmpVal, USB_CR_EP3_STALL);
                BL_WR_REG(USB_BASE, USB_EP3_CONFIG, tmpVal);
                break;

            case EP_ID4:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP4_CONFIG);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP4_RDY);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP4_NACK);
                tmpVal = BL_CLR_REG_BIT(tmpVal, USB_CR_EP4_STALL);
                BL_WR_REG(USB_BASE, USB_EP4_CONFIG, tmpVal);
                break;

            case EP_ID5:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP5_CONFIG);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP5_RDY);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP5_NACK);
                tmpVal = BL_CLR_REG_BIT(tmpVal, USB_CR_EP5_STALL);
                BL_WR_REG(USB_BASE, USB_EP5_CONFIG, tmpVal);
                break;

            case EP_ID6:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP6_CONFIG);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP6_RDY);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP6_NACK);
                tmpVal = BL_CLR_REG_BIT(tmpVal, USB_CR_EP6_STALL);
                BL_WR_REG(USB_BASE, USB_EP6_CONFIG, tmpVal);
                break;

            case EP_ID7:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP7_CONFIG);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP7_RDY);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP7_NACK);
                tmpVal = BL_CLR_REG_BIT(tmpVal, USB_CR_EP7_STALL);
                BL_WR_REG(USB_BASE, USB_EP7_CONFIG, tmpVal);
                break;

            default:
                break;
        }
    }

    return SUCCESS;
}

BL_Sts_Type USB_Is_EPx_RDY_Free(USB_EP_ID epId)
{
    uint32_t tmpVal = 0;

    if (epId == EP_ID0) {
        tmpVal = BL_RD_REG(USB_BASE, USB_CONFIG);
        tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_STS_USB_EP0_SW_RDY);
    } else {
        switch (epId) {
            case EP_ID1:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP1_CONFIG);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_STS_EP1_RDY);
                break;

            case EP_ID2:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP2_CONFIG);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_STS_EP2_RDY);
                break;

            case EP_ID3:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP3_CONFIG);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_STS_EP3_RDY);
                break;

            case EP_ID4:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP4_CONFIG);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_STS_EP4_RDY);
                break;

            case EP_ID5:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP5_CONFIG);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_STS_EP5_RDY);
                break;

            case EP_ID6:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP6_CONFIG);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_STS_EP6_RDY);
                break;

            case EP_ID7:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP7_CONFIG);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_STS_EP7_RDY);
                break;

            default:
                break;
        }
    }

    return tmpVal ? RESET : SET;
}

BL_Err_Type USB_Set_EPx_STALL(USB_EP_ID epId)
{
    uint32_t tmpVal = 0;

    if (epId == EP_ID0) {
        tmpVal = BL_RD_REG(USB_BASE, USB_CONFIG);
        tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_USB_EP0_SW_STALL);
        BL_WR_REG(USB_BASE, USB_CONFIG, tmpVal);
    } else {
        switch (epId) {
            case EP_ID1:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP1_CONFIG);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP1_STALL);
                BL_WR_REG(USB_BASE, USB_EP1_CONFIG, tmpVal);
                break;

            case EP_ID2:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP2_CONFIG);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP2_STALL);
                BL_WR_REG(USB_BASE, USB_EP2_CONFIG, tmpVal);
                break;

            case EP_ID3:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP3_CONFIG);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP3_STALL);
                BL_WR_REG(USB_BASE, USB_EP3_CONFIG, tmpVal);
                break;

            case EP_ID4:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP4_CONFIG);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP4_STALL);
                BL_WR_REG(USB_BASE, USB_EP4_CONFIG, tmpVal);
                break;

            case EP_ID5:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP5_CONFIG);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP5_STALL);
                BL_WR_REG(USB_BASE, USB_EP5_CONFIG, tmpVal);
                break;

            case EP_ID6:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP6_CONFIG);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP6_STALL);
                BL_WR_REG(USB_BASE, USB_EP6_CONFIG, tmpVal);
                break;

            case EP_ID7:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP7_CONFIG);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP7_STALL);
                BL_WR_REG(USB_BASE, USB_EP7_CONFIG, tmpVal);
                break;

            default:
                break;
        }
    }

    return SUCCESS;
}

BL_Err_Type USB_Clr_EPx_STALL(USB_EP_ID epId)
{
    uint32_t tmpVal = 0;

    if (epId == EP_ID0) {
        return SUCCESS;
    } else {
        switch (epId) {
            case EP_ID1:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP1_CONFIG);
                tmpVal = BL_CLR_REG_BIT(tmpVal, USB_CR_EP1_STALL);
                BL_WR_REG(USB_BASE, USB_EP1_CONFIG, tmpVal);
                break;

            case EP_ID2:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP2_CONFIG);
                tmpVal = BL_CLR_REG_BIT(tmpVal, USB_CR_EP2_STALL);
                BL_WR_REG(USB_BASE, USB_EP2_CONFIG, tmpVal);
                break;

            case EP_ID3:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP3_CONFIG);
                tmpVal = BL_CLR_REG_BIT(tmpVal, USB_CR_EP3_STALL);
                BL_WR_REG(USB_BASE, USB_EP3_CONFIG, tmpVal);
                break;

            case EP_ID4:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP4_CONFIG);
                tmpVal = BL_CLR_REG_BIT(tmpVal, USB_CR_EP4_STALL);
                BL_WR_REG(USB_BASE, USB_EP4_CONFIG, tmpVal);
                break;

            case EP_ID5:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP5_CONFIG);
                tmpVal = BL_CLR_REG_BIT(tmpVal, USB_CR_EP5_STALL);
                BL_WR_REG(USB_BASE, USB_EP5_CONFIG, tmpVal);
                break;

            case EP_ID6:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP6_CONFIG);
                tmpVal = BL_CLR_REG_BIT(tmpVal, USB_CR_EP6_STALL);
                BL_WR_REG(USB_BASE, USB_EP6_CONFIG, tmpVal);
                break;

            case EP_ID7:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP7_CONFIG);
                tmpVal = BL_CLR_REG_BIT(tmpVal, USB_CR_EP7_STALL);
                BL_WR_REG(USB_BASE, USB_EP7_CONFIG, tmpVal);
                break;

            default:
                break;
        }
    }

    return SUCCESS;
}

BL_Err_Type USB_Set_EPx_Busy(USB_EP_ID epId)
{
    uint32_t tmpVal = 0;

    if (epId == EP_ID0) {
        tmpVal = BL_RD_REG(USB_BASE, USB_CONFIG);
        tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_USB_EP0_SW_NACK_IN);
        tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_USB_EP0_SW_NACK_OUT);
        BL_WR_REG(USB_BASE, USB_CONFIG, tmpVal);
    } else {
        switch (epId) {
            case EP_ID1:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP1_CONFIG);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP1_NACK);
                tmpVal = BL_CLR_REG_BIT(tmpVal, USB_CR_EP1_STALL);
                BL_WR_REG(USB_BASE, USB_EP1_CONFIG, tmpVal);
                break;

            case EP_ID2:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP2_CONFIG);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP2_NACK);
                tmpVal = BL_CLR_REG_BIT(tmpVal, USB_CR_EP2_STALL);
                BL_WR_REG(USB_BASE, USB_EP2_CONFIG, tmpVal);
                break;

            case EP_ID3:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP3_CONFIG);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP3_NACK);
                tmpVal = BL_CLR_REG_BIT(tmpVal, USB_CR_EP3_STALL);
                BL_WR_REG(USB_BASE, USB_EP3_CONFIG, tmpVal);
                break;

            case EP_ID4:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP4_CONFIG);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP4_NACK);
                tmpVal = BL_CLR_REG_BIT(tmpVal, USB_CR_EP4_STALL);
                BL_WR_REG(USB_BASE, USB_EP4_CONFIG, tmpVal);
                break;

            case EP_ID5:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP5_CONFIG);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP5_NACK);
                tmpVal = BL_CLR_REG_BIT(tmpVal, USB_CR_EP5_STALL);
                BL_WR_REG(USB_BASE, USB_EP5_CONFIG, tmpVal);
                break;

            case EP_ID6:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP6_CONFIG);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP6_NACK);
                tmpVal = BL_CLR_REG_BIT(tmpVal, USB_CR_EP6_STALL);
                BL_WR_REG(USB_BASE, USB_EP6_CONFIG, tmpVal);
                break;

            case EP_ID7:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP7_CONFIG);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP7_NACK);
                tmpVal = BL_CLR_REG_BIT(tmpVal, USB_CR_EP7_STALL);
                BL_WR_REG(USB_BASE, USB_EP7_CONFIG, tmpVal);
                break;

            default:
                break;
        }
    }

    return SUCCESS;
}

BL_Err_Type USB_Set_EPx_Status(USB_EP_ID epId, USB_EP_STATUS_Type sts)
{
    switch (sts) {
        case USB_EP_STATUS_ACK:
            USB_Set_EPx_Rdy(epId);
            break;

        case USB_EP_STATUS_NACK:
            USB_Set_EPx_Busy(epId);
            break;

        case USB_EP_STATUS_STALL:
            USB_Set_EPx_STALL(epId);
            break;

        case USB_EP_STATUS_NSTALL:
            USB_Clr_EPx_STALL(epId);
            break;

        default:
            break;
    }

    return SUCCESS;
}

USB_EP_STATUS_Type USB_Get_EPx_Status(USB_EP_ID epId)
{
    uint32_t tmpVal = 0;

    if (epId == EP_ID0) {
        tmpVal = BL_RD_REG(USB_BASE, USB_CONFIG);

        switch ((tmpVal >> 24) & 0x7) {
            case 0:
                return USB_EP_STATUS_ACK;

            case 1:
                return USB_EP_STATUS_STALL;

            case 2:
            case 4:
            case 6:
                return USB_EP_STATUS_NACK;

            default:
                break;
        }
    } else {
        switch (epId) {
            case EP_ID1:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP1_CONFIG);
                break;

            case EP_ID2:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP2_CONFIG);
                break;

            case EP_ID3:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP3_CONFIG);
                break;

            case EP_ID4:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP4_CONFIG);
                break;

            case EP_ID5:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP5_CONFIG);
                break;

            case EP_ID6:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP6_CONFIG);
                break;

            case EP_ID7:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP7_CONFIG);
                break;

            default:
                tmpVal = 0;
                break;
        }

        switch ((tmpVal >> 14) & 0x3) {
            case 0:
                return USB_EP_STATUS_ACK;

            case 1:
                return USB_EP_STATUS_STALL;

            case 2:
                return USB_EP_STATUS_NACK;

            case 3:
            default:
                break;
        }
    }

    return USB_EP_STATUS_NSTALL;
}

BL_Err_Type USB_IntEn(USB_INT_Type intType, uint8_t enable)
{
    uint32_t tmpVal = 0;

    if (USB_INT_ALL == intType) {
        if (enable) {
            BL_WR_REG(USB_BASE, USB_INT_EN, USB_INT_TYPE_ALL);
        } else {
            BL_WR_REG(USB_BASE, USB_INT_EN, ~USB_INT_TYPE_ALL);
        }

        return SUCCESS;
    }

    tmpVal = BL_RD_REG(USB_BASE, USB_INT_EN);

    if (enable) {
        tmpVal |= (1 << intType);
    } else {
        tmpVal &= ~(1 << intType);
    }

    BL_WR_REG(USB_BASE, USB_INT_EN, tmpVal);

    return SUCCESS;
}

BL_Err_Type USB_IntMask(USB_INT_Type intType, BL_Mask_Type intMask)
{
    uint32_t tmpVal = 0;

    if (USB_INT_ALL == intType) {
        if (intMask != UNMASK) {
            BL_WR_REG(USB_BASE, USB_INT_MASK, USB_INT_TYPE_ALL);
        } else {
            BL_WR_REG(USB_BASE, USB_INT_MASK, ~USB_INT_TYPE_ALL);
        }

        return SUCCESS;
    }

    tmpVal = BL_RD_REG(USB_BASE, USB_INT_MASK);

    if (intMask != UNMASK) {
        tmpVal |= (1 << intType);
    } else {
        tmpVal &= ~(1 << intType);
    }

    BL_WR_REG(USB_BASE, USB_INT_MASK, tmpVal);

    return SUCCESS;
}

BL_Sts_Type USB_Get_IntStatus(USB_INT_Type intType)
{
    if (USB_INT_ALL == intType) {
        return BL_RD_REG(USB_BASE, USB_INT_STS) ? SET : RESET;
    }

    return ((BL_RD_REG(USB_BASE, USB_INT_STS) & (1 << intType))) ? SET : RESET;
}

BL_Err_Type USB_Clr_IntStatus(USB_INT_Type intType)
{
    uint32_t tmpVal = 0;

    if (USB_INT_ALL == intType) {
        BL_WR_REG(USB_BASE, USB_INT_CLEAR, USB_INT_TYPE_ALL);

        return SUCCESS;
    }

    tmpVal = BL_RD_REG(USB_BASE, USB_INT_CLEAR);
    tmpVal |= (1 << intType);
    BL_WR_REG(USB_BASE, USB_INT_CLEAR, tmpVal);

    return SUCCESS;
}

BL_Err_Type USB_Clr_EPx_IntStatus(USB_EP_ID epId)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(USB_BASE, USB_INT_CLEAR);

    if (epId == EP_ID0) {
        tmpVal |= (0x3F << 4);
    } else {
        tmpVal |= (0x3 << (epId * 2 + 8));
    }

    BL_WR_REG(USB_BASE, USB_INT_CLEAR, tmpVal);

    return SUCCESS;
}

uint16_t USB_Get_Frame_Num(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(USB_BASE, USB_FRAME_NO);

    return tmpVal & 0x7ff;
}

BL_Err_Type USB_Set_EPx_Config(USB_EP_ID epId, EP_Config_Type *epCfg)
{
    uint32_t tmpVal = 0;

    if (epId == EP_ID0) {
        return ERROR;
    }

    switch (epId) {
        case EP_ID1:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP1_CONFIG);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, USB_CR_EP1_TYPE, epCfg->type);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, USB_CR_EP1_DIR, epCfg->dir);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, USB_CR_EP1_SIZE, epCfg->EPMaxPacketSize);
            BL_WR_REG(USB_BASE, USB_EP1_CONFIG, tmpVal);
            break;

        case EP_ID2:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP2_CONFIG);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, USB_CR_EP2_TYPE, epCfg->type);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, USB_CR_EP2_DIR, epCfg->dir);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, USB_CR_EP2_SIZE, epCfg->EPMaxPacketSize);
            BL_WR_REG(USB_BASE, USB_EP2_CONFIG, tmpVal);
            break;

        case EP_ID3:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP3_CONFIG);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, USB_CR_EP3_TYPE, epCfg->type);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, USB_CR_EP3_DIR, epCfg->dir);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, USB_CR_EP3_SIZE, epCfg->EPMaxPacketSize);
            BL_WR_REG(USB_BASE, USB_EP3_CONFIG, tmpVal);
            break;

        case EP_ID4:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP4_CONFIG);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, USB_CR_EP4_TYPE, epCfg->type);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, USB_CR_EP4_DIR, epCfg->dir);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, USB_CR_EP4_SIZE, epCfg->EPMaxPacketSize);
            BL_WR_REG(USB_BASE, USB_EP4_CONFIG, tmpVal);
            break;

        case EP_ID5:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP5_CONFIG);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, USB_CR_EP5_TYPE, epCfg->type);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, USB_CR_EP5_DIR, epCfg->dir);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, USB_CR_EP5_SIZE, epCfg->EPMaxPacketSize);
            BL_WR_REG(USB_BASE, USB_EP5_CONFIG, tmpVal);
            break;

        case EP_ID6:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP6_CONFIG);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, USB_CR_EP6_TYPE, epCfg->type);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, USB_CR_EP6_DIR, epCfg->dir);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, USB_CR_EP6_SIZE, epCfg->EPMaxPacketSize);
            BL_WR_REG(USB_BASE, USB_EP6_CONFIG, tmpVal);
            break;

        case EP_ID7:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP7_CONFIG);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, USB_CR_EP7_TYPE, epCfg->type);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, USB_CR_EP7_DIR, epCfg->dir);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, USB_CR_EP7_SIZE, epCfg->EPMaxPacketSize);
            BL_WR_REG(USB_BASE, USB_EP7_CONFIG, tmpVal);
            break;

        default:
            break;
    }

    return SUCCESS;
}

BL_Err_Type USB_Set_EPx_Type(USB_EP_ID epId, EP_XFER_Type type)
{
    uint32_t tmpVal = 0;

    if (epId == EP_ID0) {
        return ERROR;
    }

    switch (epId) {
        case EP_ID1:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP1_CONFIG);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, USB_CR_EP1_TYPE, type);
            BL_WR_REG(USB_BASE, USB_EP1_CONFIG, tmpVal);
            break;

        case EP_ID2:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP2_CONFIG);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, USB_CR_EP2_TYPE, type);
            BL_WR_REG(USB_BASE, USB_EP2_CONFIG, tmpVal);
            break;

        case EP_ID3:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP3_CONFIG);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, USB_CR_EP3_TYPE, type);
            BL_WR_REG(USB_BASE, USB_EP3_CONFIG, tmpVal);
            break;

        case EP_ID4:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP4_CONFIG);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, USB_CR_EP4_TYPE, type);
            BL_WR_REG(USB_BASE, USB_EP4_CONFIG, tmpVal);
            break;

        case EP_ID5:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP5_CONFIG);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, USB_CR_EP5_TYPE, type);
            BL_WR_REG(USB_BASE, USB_EP5_CONFIG, tmpVal);
            break;

        case EP_ID6:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP6_CONFIG);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, USB_CR_EP6_TYPE, type);
            BL_WR_REG(USB_BASE, USB_EP6_CONFIG, tmpVal);
            break;

        case EP_ID7:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP7_CONFIG);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, USB_CR_EP7_TYPE, type);
            BL_WR_REG(USB_BASE, USB_EP7_CONFIG, tmpVal);
            break;

        default:
            break;
    }

    return SUCCESS;
}

EP_XFER_Type USB_Get_EPx_Type(USB_EP_ID epId)
{
    uint32_t tmpVal = 0;

    if (epId == EP_ID0) {
        return EP_CTRL;
    }

    switch (epId) {
        case EP_ID1:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP1_CONFIG);
            tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_CR_EP1_TYPE);
            break;

        case EP_ID2:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP2_CONFIG);
            tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_CR_EP2_TYPE);
            break;

        case EP_ID3:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP3_CONFIG);
            tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_CR_EP3_TYPE);
            break;

        case EP_ID4:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP4_CONFIG);
            tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_CR_EP4_TYPE);
            break;

        case EP_ID5:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP5_CONFIG);
            tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_CR_EP5_TYPE);
            break;

        case EP_ID6:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP6_CONFIG);
            tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_CR_EP6_TYPE);
            break;

        case EP_ID7:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP7_CONFIG);
            tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_CR_EP7_TYPE);
            break;

        default:
            break;
    }

    return (EP_XFER_Type)tmpVal;
}

BL_Err_Type USB_Set_EPx_Dir(USB_EP_ID epId, EP_XFER_DIR dir)
{
    uint32_t tmpVal = 0;

    if (epId == EP_ID0) {
        return ERROR;
    }

    switch (epId) {
        case EP_ID1:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP1_CONFIG);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, USB_CR_EP1_DIR, dir);
            BL_WR_REG(USB_BASE, USB_EP1_CONFIG, tmpVal);
            break;

        case EP_ID2:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP2_CONFIG);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, USB_CR_EP2_DIR, dir);
            BL_WR_REG(USB_BASE, USB_EP2_CONFIG, tmpVal);
            break;

        case EP_ID3:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP3_CONFIG);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, USB_CR_EP3_DIR, dir);
            BL_WR_REG(USB_BASE, USB_EP3_CONFIG, tmpVal);
            break;

        case EP_ID4:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP4_CONFIG);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, USB_CR_EP4_DIR, dir);
            BL_WR_REG(USB_BASE, USB_EP4_CONFIG, tmpVal);
            break;

        case EP_ID5:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP5_CONFIG);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, USB_CR_EP5_DIR, dir);
            BL_WR_REG(USB_BASE, USB_EP5_CONFIG, tmpVal);
            break;

        case EP_ID6:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP6_CONFIG);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, USB_CR_EP6_DIR, dir);
            BL_WR_REG(USB_BASE, USB_EP6_CONFIG, tmpVal);
            break;

        case EP_ID7:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP7_CONFIG);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, USB_CR_EP7_DIR, dir);
            BL_WR_REG(USB_BASE, USB_EP7_CONFIG, tmpVal);
            break;

        default:
            break;
    }

    return SUCCESS;
}

EP_XFER_DIR USB_Get_EPx_Dir(USB_EP_ID epId)
{
    uint32_t tmpVal = 0;

    if (epId == EP_ID0) {
        return EP_DISABLED;
    }

    switch (epId) {
        case EP_ID1:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP1_CONFIG);
            tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_CR_EP1_DIR);
            break;

        case EP_ID2:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP2_CONFIG);
            tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_CR_EP2_DIR);
            break;

        case EP_ID3:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP3_CONFIG);
            tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_CR_EP3_DIR);
            break;

        case EP_ID4:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP4_CONFIG);
            tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_CR_EP4_DIR);
            break;

        case EP_ID5:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP5_CONFIG);
            tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_CR_EP5_DIR);
            break;

        case EP_ID6:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP6_CONFIG);
            tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_CR_EP6_DIR);
            break;

        case EP_ID7:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP7_CONFIG);
            tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_CR_EP7_DIR);
            break;

        default:
            break;
    }

    return (EP_XFER_DIR)tmpVal;
}

BL_Err_Type USB_Set_EPx_Size(USB_EP_ID epId, uint32_t size)
{
    uint32_t tmpVal = 0;

    switch (epId) {
        case EP_ID0:
            tmpVal = BL_RD_REG(USB_BASE, USB_CONFIG);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, USB_CR_USB_EP0_SW_SIZE, size);
            BL_WR_REG(USB_BASE, USB_CONFIG, tmpVal);
            break;

        case EP_ID1:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP1_CONFIG);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, USB_CR_EP1_SIZE, size);
            BL_WR_REG(USB_BASE, USB_EP1_CONFIG, tmpVal);
            break;

        case EP_ID2:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP2_CONFIG);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, USB_CR_EP2_SIZE, size);
            BL_WR_REG(USB_BASE, USB_EP2_CONFIG, tmpVal);
            break;

        case EP_ID3:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP3_CONFIG);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, USB_CR_EP3_SIZE, size);
            BL_WR_REG(USB_BASE, USB_EP3_CONFIG, tmpVal);
            break;

        case EP_ID4:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP4_CONFIG);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, USB_CR_EP4_SIZE, size);
            BL_WR_REG(USB_BASE, USB_EP4_CONFIG, tmpVal);
            break;

        case EP_ID5:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP5_CONFIG);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, USB_CR_EP5_SIZE, size);
            BL_WR_REG(USB_BASE, USB_EP5_CONFIG, tmpVal);
            break;

        case EP_ID6:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP6_CONFIG);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, USB_CR_EP6_SIZE, size);
            BL_WR_REG(USB_BASE, USB_EP6_CONFIG, tmpVal);
            break;

        case EP_ID7:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP7_CONFIG);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, USB_CR_EP7_SIZE, size);
            BL_WR_REG(USB_BASE, USB_EP7_CONFIG, tmpVal);
            break;

        default:
            break;
    }

    return SUCCESS;
}

BL_Sts_Type USB_Get_EPx_TX_FIFO_Errors(USB_EP_ID epId, USB_FIFO_ERROR_FLAG_Type errFlag)
{
    uint32_t tmpVal = 0;

    if (errFlag == USB_FIFO_ERROR_OVERFLOW) {
        switch (epId) {
            case EP_ID0:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP0_FIFO_CONFIG);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP0_TX_FIFO_OVERFLOW);
                break;

            case EP_ID1:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP1_FIFO_CONFIG);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP1_TX_FIFO_OVERFLOW);
                break;

            case EP_ID2:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP2_FIFO_CONFIG);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP2_TX_FIFO_OVERFLOW);
                break;

            case EP_ID3:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP3_FIFO_CONFIG);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP3_TX_FIFO_OVERFLOW);
                break;

            case EP_ID4:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP4_FIFO_CONFIG);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP4_TX_FIFO_OVERFLOW);
                break;

            case EP_ID5:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP5_FIFO_CONFIG);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP5_TX_FIFO_OVERFLOW);
                break;

            case EP_ID6:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP6_FIFO_CONFIG);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP6_TX_FIFO_OVERFLOW);
                break;

            case EP_ID7:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP7_FIFO_CONFIG);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP7_TX_FIFO_OVERFLOW);
                break;

            default:
                tmpVal = 0;
                break;
        }
    } else {
        switch (epId) {
            case EP_ID0:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP0_FIFO_CONFIG);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP0_TX_FIFO_UNDERFLOW);
                break;

            case EP_ID1:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP1_FIFO_CONFIG);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP1_TX_FIFO_UNDERFLOW);
                break;

            case EP_ID2:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP2_FIFO_CONFIG);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP2_TX_FIFO_UNDERFLOW);
                break;

            case EP_ID3:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP3_FIFO_CONFIG);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP3_TX_FIFO_UNDERFLOW);
                break;

            case EP_ID4:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP4_FIFO_CONFIG);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP4_TX_FIFO_UNDERFLOW);
                break;

            case EP_ID5:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP5_FIFO_CONFIG);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP5_TX_FIFO_UNDERFLOW);
                break;

            case EP_ID6:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP6_FIFO_CONFIG);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP6_TX_FIFO_UNDERFLOW);
                break;

            case EP_ID7:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP7_FIFO_CONFIG);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP7_TX_FIFO_UNDERFLOW);
                break;

            default:
                tmpVal = 0;
                break;
        }
    }

    return tmpVal ? SET : RESET;
}

BL_Sts_Type USB_Get_EPx_RX_FIFO_Errors(USB_EP_ID epId, USB_FIFO_ERROR_FLAG_Type errFlag)
{
    uint32_t tmpVal = 0;

    if (errFlag == USB_FIFO_ERROR_OVERFLOW) {
        switch (epId) {
            case EP_ID0:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP0_FIFO_CONFIG);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP0_RX_FIFO_OVERFLOW);
                break;

            case EP_ID1:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP1_FIFO_CONFIG);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP1_RX_FIFO_OVERFLOW);
                break;

            case EP_ID2:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP2_FIFO_CONFIG);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP2_RX_FIFO_OVERFLOW);
                break;

            case EP_ID3:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP3_FIFO_CONFIG);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP3_RX_FIFO_OVERFLOW);
                break;

            case EP_ID4:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP4_FIFO_CONFIG);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP4_RX_FIFO_OVERFLOW);
                break;

            case EP_ID5:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP5_FIFO_CONFIG);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP5_RX_FIFO_OVERFLOW);
                break;

            case EP_ID6:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP6_FIFO_CONFIG);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP6_RX_FIFO_OVERFLOW);
                break;

            case EP_ID7:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP7_FIFO_CONFIG);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP7_RX_FIFO_OVERFLOW);
                break;

            default:
                tmpVal = 0;
                break;
        }
    } else {
        switch (epId) {
            case EP_ID0:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP0_FIFO_CONFIG);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP0_RX_FIFO_UNDERFLOW);
                break;

            case EP_ID1:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP1_FIFO_CONFIG);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP1_RX_FIFO_UNDERFLOW);
                break;

            case EP_ID2:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP2_FIFO_CONFIG);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP2_RX_FIFO_UNDERFLOW);
                break;

            case EP_ID3:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP3_FIFO_CONFIG);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP3_RX_FIFO_UNDERFLOW);
                break;

            case EP_ID4:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP4_FIFO_CONFIG);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP4_RX_FIFO_UNDERFLOW);
                break;

            case EP_ID5:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP5_FIFO_CONFIG);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP5_RX_FIFO_UNDERFLOW);
                break;

            case EP_ID6:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP6_FIFO_CONFIG);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP6_RX_FIFO_UNDERFLOW);
                break;

            case EP_ID7:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP7_FIFO_CONFIG);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP7_RX_FIFO_UNDERFLOW);
                break;

            default:
                tmpVal = 0;
                break;
        }
    }

    return tmpVal ? SET : RESET;
}

BL_Err_Type USB_Clr_EPx_TX_FIFO_Errors(USB_EP_ID epId)
{
    uint32_t tmpVal = 0;

    switch (epId) {
        case EP_ID0:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP0_FIFO_CONFIG);
            tmpVal = BL_SET_REG_BIT(tmpVal, USB_EP0_TX_FIFO_CLR);
            BL_WR_REG(USB_BASE, USB_EP0_FIFO_CONFIG, tmpVal);
            break;

        case EP_ID1:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP1_FIFO_CONFIG);
            tmpVal = BL_SET_REG_BIT(tmpVal, USB_EP1_TX_FIFO_CLR);
            BL_WR_REG(USB_BASE, USB_EP1_FIFO_CONFIG, tmpVal);
            break;

        case EP_ID2:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP2_FIFO_CONFIG);
            tmpVal = BL_SET_REG_BIT(tmpVal, USB_EP2_TX_FIFO_CLR);
            BL_WR_REG(USB_BASE, USB_EP2_FIFO_CONFIG, tmpVal);
            break;

        case EP_ID3:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP3_FIFO_CONFIG);
            tmpVal = BL_SET_REG_BIT(tmpVal, USB_EP3_TX_FIFO_CLR);
            BL_WR_REG(USB_BASE, USB_EP3_FIFO_CONFIG, tmpVal);
            break;

        case EP_ID4:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP4_FIFO_CONFIG);
            tmpVal = BL_SET_REG_BIT(tmpVal, USB_EP4_TX_FIFO_CLR);
            BL_WR_REG(USB_BASE, USB_EP4_FIFO_CONFIG, tmpVal);
            break;

        case EP_ID5:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP5_FIFO_CONFIG);
            tmpVal = BL_SET_REG_BIT(tmpVal, USB_EP5_TX_FIFO_CLR);
            BL_WR_REG(USB_BASE, USB_EP5_FIFO_CONFIG, tmpVal);
            break;

        case EP_ID6:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP6_FIFO_CONFIG);
            tmpVal = BL_SET_REG_BIT(tmpVal, USB_EP6_TX_FIFO_CLR);
            BL_WR_REG(USB_BASE, USB_EP6_FIFO_CONFIG, tmpVal);
            break;

        case EP_ID7:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP7_FIFO_CONFIG);
            tmpVal = BL_SET_REG_BIT(tmpVal, USB_EP7_TX_FIFO_CLR);
            BL_WR_REG(USB_BASE, USB_EP7_FIFO_CONFIG, tmpVal);
            break;

        default:
            break;
    }

    return SUCCESS;
}

BL_Err_Type USB_Clr_EPx_RX_FIFO_Errors(USB_EP_ID epId)
{
    uint32_t tmpVal = 0;

    switch (epId) {
        case EP_ID0:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP0_FIFO_CONFIG);
            tmpVal = BL_SET_REG_BIT(tmpVal, USB_EP0_RX_FIFO_CLR);
            BL_WR_REG(USB_BASE, USB_EP0_FIFO_CONFIG, tmpVal);
            break;

        case EP_ID1:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP1_FIFO_CONFIG);
            tmpVal = BL_SET_REG_BIT(tmpVal, USB_EP1_RX_FIFO_CLR);
            BL_WR_REG(USB_BASE, USB_EP1_FIFO_CONFIG, tmpVal);
            break;

        case EP_ID2:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP2_FIFO_CONFIG);
            tmpVal = BL_SET_REG_BIT(tmpVal, USB_EP2_RX_FIFO_CLR);
            BL_WR_REG(USB_BASE, USB_EP2_FIFO_CONFIG, tmpVal);
            break;

        case EP_ID3:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP3_FIFO_CONFIG);
            tmpVal = BL_SET_REG_BIT(tmpVal, USB_EP3_RX_FIFO_CLR);
            BL_WR_REG(USB_BASE, USB_EP3_FIFO_CONFIG, tmpVal);
            break;

        case EP_ID4:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP4_FIFO_CONFIG);
            tmpVal = BL_SET_REG_BIT(tmpVal, USB_EP4_RX_FIFO_CLR);
            BL_WR_REG(USB_BASE, USB_EP4_FIFO_CONFIG, tmpVal);
            break;

        case EP_ID5:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP5_FIFO_CONFIG);
            tmpVal = BL_SET_REG_BIT(tmpVal, USB_EP5_RX_FIFO_CLR);
            BL_WR_REG(USB_BASE, USB_EP5_FIFO_CONFIG, tmpVal);
            break;

        case EP_ID6:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP6_FIFO_CONFIG);
            tmpVal = BL_SET_REG_BIT(tmpVal, USB_EP6_RX_FIFO_CLR);
            BL_WR_REG(USB_BASE, USB_EP6_FIFO_CONFIG, tmpVal);
            break;

        case EP_ID7:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP7_FIFO_CONFIG);
            tmpVal = BL_SET_REG_BIT(tmpVal, USB_EP7_RX_FIFO_CLR);
            BL_WR_REG(USB_BASE, USB_EP7_FIFO_CONFIG, tmpVal);
            break;

        default:
            break;
    }

    return SUCCESS;
}

BL_Err_Type USB_EPx_Write_Data_To_FIFO(USB_EP_ID epId, uint8_t *pData, uint16_t len)
{
    uint32_t timeout = 0x00FFFFFF;

    while ((!USB_Is_EPx_RDY_Free(epId)) && timeout) {
        timeout--;
    }

    if (!timeout) {
        return ERROR;
    }

    if (len == 1) {
        USB_Set_EPx_Xfer_Size(EP_ID0, 1);
    } else {
        USB_Set_EPx_Xfer_Size(EP_ID0, 64);
    }

    for (uint16_t i = 0; i < len; i++) {
        switch (epId) {
            case EP_ID0:
                BL_WR_REG(USB_BASE, USB_EP0_TX_FIFO_WDATA, pData[i]);
                break;

            case EP_ID1:
                BL_WR_REG(USB_BASE, USB_EP1_TX_FIFO_WDATA, pData[i]);
                break;

            case EP_ID2:
                BL_WR_REG(USB_BASE, USB_EP2_TX_FIFO_WDATA, pData[i]);
                break;

            case EP_ID3:
                BL_WR_REG(USB_BASE, USB_EP3_TX_FIFO_WDATA, pData[i]);
                break;

            case EP_ID4:
                BL_WR_REG(USB_BASE, USB_EP4_TX_FIFO_WDATA, pData[i]);
                break;

            case EP_ID5:
                BL_WR_REG(USB_BASE, USB_EP5_TX_FIFO_WDATA, pData[i]);
                break;

            case EP_ID6:
                BL_WR_REG(USB_BASE, USB_EP6_TX_FIFO_WDATA, pData[i]);
                break;

            case EP_ID7:
                BL_WR_REG(USB_BASE, USB_EP7_TX_FIFO_WDATA, pData[i]);
                break;

            default:
                break;
        }
    }

    return SUCCESS;
}

BL_Err_Type USB_EPx_Read_Data_From_FIFO(USB_EP_ID epId, uint8_t *pBuff, uint16_t len)
{
    for (uint16_t i = 0; i < len; i++) {
        switch (epId) {
            case EP_ID0:
                pBuff[i] = (uint8_t)BL_RD_REG(USB_BASE, USB_EP0_RX_FIFO_RDATA);
                break;

            case EP_ID1:
                pBuff[i] = (uint8_t)BL_RD_REG(USB_BASE, USB_EP1_RX_FIFO_RDATA);
                break;

            case EP_ID2:
                pBuff[i] = (uint8_t)BL_RD_REG(USB_BASE, USB_EP2_RX_FIFO_RDATA);
                break;

            case EP_ID3:
                pBuff[i] = (uint8_t)BL_RD_REG(USB_BASE, USB_EP3_RX_FIFO_RDATA);
                break;

            case EP_ID4:
                pBuff[i] = (uint8_t)BL_RD_REG(USB_BASE, USB_EP4_RX_FIFO_RDATA);
                break;

            case EP_ID5:
                pBuff[i] = (uint8_t)BL_RD_REG(USB_BASE, USB_EP5_RX_FIFO_RDATA);
                break;

            case EP_ID6:
                pBuff[i] = (uint8_t)BL_RD_REG(USB_BASE, USB_EP6_RX_FIFO_RDATA);
                break;

            case EP_ID7:
                pBuff[i] = (uint8_t)BL_RD_REG(USB_BASE, USB_EP7_RX_FIFO_RDATA);
                break;

            default:
                break;
        }
    }

    return SUCCESS;
}

BL_Err_Type USB_Set_EPx_TX_DMA_Interface_Config(USB_EP_ID epId, BL_Fun_Type newState)
{
    uint32_t tmpVal = 0;

    if (newState == ENABLE) {
        switch (epId) {
            case EP_ID0:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP0_FIFO_CONFIG);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_EP0_DMA_TX_EN);
                BL_WR_REG(USB_BASE, USB_EP0_FIFO_CONFIG, tmpVal);
                break;

            case EP_ID1:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP1_FIFO_CONFIG);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_EP1_DMA_TX_EN);
                BL_WR_REG(USB_BASE, USB_EP1_FIFO_CONFIG, tmpVal);
                break;

            case EP_ID2:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP2_FIFO_CONFIG);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_EP2_DMA_TX_EN);
                BL_WR_REG(USB_BASE, USB_EP2_FIFO_CONFIG, tmpVal);
                break;

            case EP_ID3:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP3_FIFO_CONFIG);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_EP3_DMA_TX_EN);
                BL_WR_REG(USB_BASE, USB_EP3_FIFO_CONFIG, tmpVal);
                break;

            case EP_ID4:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP4_FIFO_CONFIG);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_EP4_DMA_TX_EN);
                BL_WR_REG(USB_BASE, USB_EP4_FIFO_CONFIG, tmpVal);
                break;

            case EP_ID5:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP5_FIFO_CONFIG);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_EP5_DMA_TX_EN);
                BL_WR_REG(USB_BASE, USB_EP5_FIFO_CONFIG, tmpVal);
                break;

            case EP_ID6:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP6_FIFO_CONFIG);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_EP6_DMA_TX_EN);
                BL_WR_REG(USB_BASE, USB_EP6_FIFO_CONFIG, tmpVal);
                break;

            case EP_ID7:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP7_FIFO_CONFIG);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_EP7_DMA_TX_EN);
                BL_WR_REG(USB_BASE, USB_EP7_FIFO_CONFIG, tmpVal);
                break;

            default:
                break;
        }
    } else {
        switch (epId) {
            case EP_ID0:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP0_FIFO_CONFIG);
                tmpVal = BL_CLR_REG_BIT(tmpVal, USB_EP0_DMA_TX_EN);
                BL_WR_REG(USB_BASE, USB_EP0_FIFO_CONFIG, tmpVal);
                break;

            case EP_ID1:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP1_FIFO_CONFIG);
                tmpVal = BL_CLR_REG_BIT(tmpVal, USB_EP1_DMA_TX_EN);
                BL_WR_REG(USB_BASE, USB_EP1_FIFO_CONFIG, tmpVal);
                break;

            case EP_ID2:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP2_FIFO_CONFIG);
                tmpVal = BL_CLR_REG_BIT(tmpVal, USB_EP2_DMA_TX_EN);
                BL_WR_REG(USB_BASE, USB_EP2_FIFO_CONFIG, tmpVal);
                break;

            case EP_ID3:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP3_FIFO_CONFIG);
                tmpVal = BL_CLR_REG_BIT(tmpVal, USB_EP3_DMA_TX_EN);
                BL_WR_REG(USB_BASE, USB_EP3_FIFO_CONFIG, tmpVal);
                break;

            case EP_ID4:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP4_FIFO_CONFIG);
                tmpVal = BL_CLR_REG_BIT(tmpVal, USB_EP4_DMA_TX_EN);
                BL_WR_REG(USB_BASE, USB_EP4_FIFO_CONFIG, tmpVal);
                break;

            case EP_ID5:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP5_FIFO_CONFIG);
                tmpVal = BL_CLR_REG_BIT(tmpVal, USB_EP5_DMA_TX_EN);
                BL_WR_REG(USB_BASE, USB_EP5_FIFO_CONFIG, tmpVal);
                break;

            case EP_ID6:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP6_FIFO_CONFIG);
                tmpVal = BL_CLR_REG_BIT(tmpVal, USB_EP6_DMA_TX_EN);
                BL_WR_REG(USB_BASE, USB_EP6_FIFO_CONFIG, tmpVal);
                break;

            case EP_ID7:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP7_FIFO_CONFIG);
                tmpVal = BL_CLR_REG_BIT(tmpVal, USB_EP7_DMA_TX_EN);
                BL_WR_REG(USB_BASE, USB_EP7_FIFO_CONFIG, tmpVal);
                break;

            default:
                break;
        }
    }

    return SUCCESS;
}

BL_Err_Type USB_Set_EPx_RX_DMA_Interface_Config(USB_EP_ID epId, BL_Fun_Type newState)
{
    uint32_t tmpVal = 0;

    if (newState == ENABLE) {
        switch (epId) {
            case EP_ID0:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP0_FIFO_CONFIG);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_EP0_DMA_RX_EN);
                BL_WR_REG(USB_BASE, USB_EP0_FIFO_CONFIG, tmpVal);
                break;

            case EP_ID1:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP1_FIFO_CONFIG);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_EP1_DMA_RX_EN);
                BL_WR_REG(USB_BASE, USB_EP1_FIFO_CONFIG, tmpVal);
                break;

            case EP_ID2:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP2_FIFO_CONFIG);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_EP2_DMA_RX_EN);
                BL_WR_REG(USB_BASE, USB_EP2_FIFO_CONFIG, tmpVal);
                break;

            case EP_ID3:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP3_FIFO_CONFIG);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_EP3_DMA_RX_EN);
                BL_WR_REG(USB_BASE, USB_EP3_FIFO_CONFIG, tmpVal);
                break;

            case EP_ID4:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP4_FIFO_CONFIG);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_EP4_DMA_RX_EN);
                BL_WR_REG(USB_BASE, USB_EP4_FIFO_CONFIG, tmpVal);
                break;

            case EP_ID5:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP5_FIFO_CONFIG);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_EP5_DMA_RX_EN);
                BL_WR_REG(USB_BASE, USB_EP5_FIFO_CONFIG, tmpVal);
                break;

            case EP_ID6:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP6_FIFO_CONFIG);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_EP6_DMA_RX_EN);
                BL_WR_REG(USB_BASE, USB_EP6_FIFO_CONFIG, tmpVal);
                break;

            case EP_ID7:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP7_FIFO_CONFIG);
                tmpVal = BL_SET_REG_BIT(tmpVal, USB_EP7_DMA_RX_EN);
                BL_WR_REG(USB_BASE, USB_EP7_FIFO_CONFIG, tmpVal);
                break;

            default:
                break;
        }
    } else {
        switch (epId) {
            case EP_ID0:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP0_FIFO_CONFIG);
                tmpVal = BL_CLR_REG_BIT(tmpVal, USB_EP0_DMA_RX_EN);
                BL_WR_REG(USB_BASE, USB_EP0_FIFO_CONFIG, tmpVal);
                break;

            case EP_ID1:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP1_FIFO_CONFIG);
                tmpVal = BL_CLR_REG_BIT(tmpVal, USB_EP1_DMA_RX_EN);
                BL_WR_REG(USB_BASE, USB_EP1_FIFO_CONFIG, tmpVal);
                break;

            case EP_ID2:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP2_FIFO_CONFIG);
                tmpVal = BL_CLR_REG_BIT(tmpVal, USB_EP2_DMA_RX_EN);
                BL_WR_REG(USB_BASE, USB_EP2_FIFO_CONFIG, tmpVal);
                break;

            case EP_ID3:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP3_FIFO_CONFIG);
                tmpVal = BL_CLR_REG_BIT(tmpVal, USB_EP3_DMA_RX_EN);
                BL_WR_REG(USB_BASE, USB_EP3_FIFO_CONFIG, tmpVal);
                break;

            case EP_ID4:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP4_FIFO_CONFIG);
                tmpVal = BL_CLR_REG_BIT(tmpVal, USB_EP4_DMA_RX_EN);
                BL_WR_REG(USB_BASE, USB_EP4_FIFO_CONFIG, tmpVal);
                break;

            case EP_ID5:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP5_FIFO_CONFIG);
                tmpVal = BL_CLR_REG_BIT(tmpVal, USB_EP5_DMA_RX_EN);
                BL_WR_REG(USB_BASE, USB_EP5_FIFO_CONFIG, tmpVal);
                break;

            case EP_ID6:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP6_FIFO_CONFIG);
                tmpVal = BL_CLR_REG_BIT(tmpVal, USB_EP6_DMA_RX_EN);
                BL_WR_REG(USB_BASE, USB_EP6_FIFO_CONFIG, tmpVal);
                break;

            case EP_ID7:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP7_FIFO_CONFIG);
                tmpVal = BL_CLR_REG_BIT(tmpVal, USB_EP7_DMA_RX_EN);
                BL_WR_REG(USB_BASE, USB_EP7_FIFO_CONFIG, tmpVal);
                break;

            default:
                break;
        }
    }

    return SUCCESS;
}

BL_Err_Type USB_EPx_Write_Data_To_FIFO_DMA(USB_EP_ID epId, uint8_t *pData, uint16_t len)
{
    /* not yet implemented */

    return SUCCESS;
}

BL_Err_Type USB_EPx_Read_Data_From_FIFO_DMA(USB_EP_ID epId, uint8_t *pBuff, uint16_t len)
{
    /* not yet implemented */

    return SUCCESS;
}

uint16_t USB_Get_EPx_TX_FIFO_CNT(USB_EP_ID epId)
{
    uint32_t tmpVal = 0;

    switch (epId) {
        case EP_ID0:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP0_FIFO_STATUS);
            tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP0_TX_FIFO_CNT);
            break;

        case EP_ID1:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP1_FIFO_STATUS);
            tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP1_TX_FIFO_CNT);
            break;

        case EP_ID2:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP2_FIFO_STATUS);
            tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP2_TX_FIFO_CNT);
            break;

        case EP_ID3:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP3_FIFO_STATUS);
            tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP3_TX_FIFO_CNT);
            break;

        case EP_ID4:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP4_FIFO_STATUS);
            tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP4_TX_FIFO_CNT);
            break;

        case EP_ID5:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP5_FIFO_STATUS);
            tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP5_TX_FIFO_CNT);
            break;

        case EP_ID6:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP6_FIFO_STATUS);
            tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP6_TX_FIFO_CNT);
            break;

        case EP_ID7:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP7_FIFO_STATUS);
            tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP7_TX_FIFO_CNT);
            break;

        default:
            tmpVal = 0;
            break;
    }

    return tmpVal;
}

uint16_t USB_Get_EPx_RX_FIFO_CNT(USB_EP_ID epId)
{
    uint32_t tmpVal = 0;

    switch (epId) {
        case EP_ID0:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP0_FIFO_STATUS);
            tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP0_RX_FIFO_CNT);
            break;

        case EP_ID1:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP1_FIFO_STATUS);
            tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP1_RX_FIFO_CNT);
            break;

        case EP_ID2:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP2_FIFO_STATUS);
            tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP2_RX_FIFO_CNT);
            break;

        case EP_ID3:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP3_FIFO_STATUS);
            tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP3_RX_FIFO_CNT);
            break;

        case EP_ID4:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP4_FIFO_STATUS);
            tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP4_RX_FIFO_CNT);
            break;

        case EP_ID5:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP5_FIFO_STATUS);
            tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP5_RX_FIFO_CNT);
            break;

        case EP_ID6:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP6_FIFO_STATUS);
            tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP6_RX_FIFO_CNT);
            break;

        case EP_ID7:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP7_FIFO_STATUS);
            tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP7_RX_FIFO_CNT);
            break;

        default:
            tmpVal = 0;
            break;
    }

    return tmpVal;
}

BL_Sts_Type USB_Get_EPx_TX_FIFO_Status(USB_EP_ID epId, USB_FIFO_STATUS_Type sts)
{
    uint32_t tmpVal = 0;

    if (sts == USB_FIFO_EMPTY) {
        switch (epId) {
            case EP_ID0:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP0_FIFO_STATUS);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP0_TX_FIFO_EMPTY);
                break;

            case EP_ID1:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP1_FIFO_STATUS);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP1_TX_FIFO_EMPTY);
                break;

            case EP_ID2:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP2_FIFO_STATUS);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP2_TX_FIFO_EMPTY);
                break;

            case EP_ID3:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP3_FIFO_STATUS);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP3_TX_FIFO_EMPTY);
                break;

            case EP_ID4:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP4_FIFO_STATUS);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP4_TX_FIFO_EMPTY);
                break;

            case EP_ID5:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP5_FIFO_STATUS);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP5_TX_FIFO_EMPTY);
                break;

            case EP_ID6:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP6_FIFO_STATUS);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP6_TX_FIFO_EMPTY);
                break;

            case EP_ID7:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP7_FIFO_STATUS);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP7_TX_FIFO_EMPTY);
                break;

            default:
                tmpVal = 0;
                break;
        }
    } else {
        switch (epId) {
            case EP_ID0:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP0_FIFO_STATUS);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP0_TX_FIFO_FULL);
                break;

            case EP_ID1:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP1_FIFO_STATUS);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP1_TX_FIFO_FULL);
                break;

            case EP_ID2:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP2_FIFO_STATUS);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP2_TX_FIFO_FULL);
                break;

            case EP_ID3:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP3_FIFO_STATUS);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP3_TX_FIFO_FULL);
                break;

            case EP_ID4:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP4_FIFO_STATUS);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP4_TX_FIFO_FULL);
                break;

            case EP_ID5:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP5_FIFO_STATUS);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP5_TX_FIFO_FULL);
                break;

            case EP_ID6:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP6_FIFO_STATUS);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP6_TX_FIFO_FULL);
                break;

            case EP_ID7:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP7_FIFO_STATUS);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP7_TX_FIFO_FULL);
                break;

            default:
                tmpVal = 0;
                break;
        }
    }

    return tmpVal ? SET : RESET;
}

BL_Sts_Type USB_Get_EPx_RX_FIFO_Status(USB_EP_ID epId, USB_FIFO_STATUS_Type sts)
{
    uint32_t tmpVal = 0;

    if (sts == USB_FIFO_EMPTY) {
        switch (epId) {
            case EP_ID0:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP0_FIFO_STATUS);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP0_RX_FIFO_EMPTY);
                break;

            case EP_ID1:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP1_FIFO_STATUS);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP1_RX_FIFO_EMPTY);
                break;

            case EP_ID2:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP2_FIFO_STATUS);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP2_RX_FIFO_EMPTY);
                break;

            case EP_ID3:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP3_FIFO_STATUS);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP3_RX_FIFO_EMPTY);
                break;

            case EP_ID4:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP4_FIFO_STATUS);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP4_RX_FIFO_EMPTY);
                break;

            case EP_ID5:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP5_FIFO_STATUS);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP5_RX_FIFO_EMPTY);
                break;

            case EP_ID6:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP6_FIFO_STATUS);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP6_RX_FIFO_EMPTY);
                break;

            case EP_ID7:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP7_FIFO_STATUS);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP7_RX_FIFO_EMPTY);
                break;

            default:
                tmpVal = 0;
                break;
        }
    } else {
        switch (epId) {
            case EP_ID0:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP0_FIFO_STATUS);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP0_RX_FIFO_FULL);
                break;

            case EP_ID1:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP1_FIFO_STATUS);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP1_RX_FIFO_FULL);
                break;

            case EP_ID2:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP2_FIFO_STATUS);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP2_RX_FIFO_FULL);
                break;

            case EP_ID3:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP3_FIFO_STATUS);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP3_RX_FIFO_FULL);
                break;

            case EP_ID4:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP4_FIFO_STATUS);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP4_RX_FIFO_FULL);
                break;

            case EP_ID5:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP5_FIFO_STATUS);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP5_RX_FIFO_FULL);
                break;

            case EP_ID6:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP6_FIFO_STATUS);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP6_RX_FIFO_FULL);
                break;

            case EP_ID7:
                tmpVal = BL_RD_REG(USB_BASE, USB_EP7_FIFO_STATUS);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, USB_EP7_RX_FIFO_FULL);
                break;

            default:
                tmpVal = 0;
                break;
        }
    }

    return tmpVal ? SET : RESET;
}

BL_Err_Type USB_Set_Internal_PullUp_Config(BL_Fun_Type newState)
{
    uint32_t tmpVal = 0;

    /* recommended: fclk<=160MHz, bclk<=80MHz */
    tmpVal = BL_RD_REG(GLB_BASE, GLB_USB_XCVR);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_USB_ENUM, newState);
    BL_WR_REG(GLB_BASE, GLB_USB_XCVR, tmpVal);

    return SUCCESS;
}

BL_Sts_Type USB_Get_LPM_Status(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(USB_BASE, USB_LPM_CONFIG);

    return BL_GET_REG_BITS_VAL(tmpVal, USB_STS_LPM) ? SET : RESET;
}

uint16_t USB_Get_LPM_Packet_Attr(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(USB_BASE, USB_LPM_CONFIG);

    return BL_GET_REG_BITS_VAL(tmpVal, USB_STS_LPM_ATTR);
}

BL_Err_Type USB_Set_LPM_Default_Response(USB_LPM_DEFAULT_RESP_Type defaultResp)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(USB_BASE, USB_LPM_CONFIG);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, USB_CR_LPM_RESP, defaultResp);
    BL_WR_REG(USB_BASE, USB_LPM_CONFIG, tmpVal);

    tmpVal = BL_RD_REG(USB_BASE, USB_LPM_CONFIG);
    tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_LPM_RESP_UPD);
    BL_WR_REG(USB_BASE, USB_LPM_CONFIG, tmpVal);

    return SUCCESS;
}

BL_Err_Type USB_LPM_Enable(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(USB_BASE, USB_LPM_CONFIG);
    tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_LPM_EN);
    BL_WR_REG(USB_BASE, USB_LPM_CONFIG, tmpVal);

    return SUCCESS;
}

BL_Err_Type USB_LPM_Disable(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(USB_BASE, USB_LPM_CONFIG);
    tmpVal = BL_CLR_REG_BIT(tmpVal, USB_CR_LPM_EN);
    BL_WR_REG(USB_BASE, USB_LPM_CONFIG, tmpVal);

    return SUCCESS;
}

BL_Err_Type USB_Device_Output_K_State(uint16_t stateWidth)
{
    uint32_t tmpVal = 0;

    CHECK_PARAM((stateWidth <= 0x7FF));

    tmpVal = BL_RD_REG(USB_BASE, USB_RESUME_CONFIG);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, USB_CR_RES_WIDTH, stateWidth);
    BL_WR_REG(USB_BASE, USB_RESUME_CONFIG, tmpVal);

    tmpVal = BL_RD_REG(USB_BASE, USB_RESUME_CONFIG);
    tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_RES_TRIG);
    BL_WR_REG(USB_BASE, USB_RESUME_CONFIG, tmpVal);

    return SUCCESS;
}

uint8_t USB_Get_Current_Packet_PID(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(USB_BASE, USB_FRAME_NO);

    return BL_GET_REG_BITS_VAL(tmpVal, USB_STS_PID);
}

uint8_t USB_Get_Current_Packet_EP(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(USB_BASE, USB_FRAME_NO);

    return BL_GET_REG_BITS_VAL(tmpVal, USB_STS_EP_NO);
}

BL_Sts_Type USB_Get_Error_Status(USB_ERROR_Type err)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(USB_BASE, USB_ERROR);

    return tmpVal & (1 << err) ? SET : RESET;
}

BL_Err_Type USB_Clr_Error_Status(USB_ERROR_Type err)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(USB_BASE, USB_INT_CLEAR);
    tmpVal |= (1 << USB_INT_ERROR);
    BL_WR_REG(USB_BASE, USB_INT_CLEAR, tmpVal);

    return SUCCESS;
}

BL_Err_Type USB_Clr_RstEndIntStatus(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(USB_BASE, USB_INT_CLEAR);
    tmpVal |= (1 << 27);
    BL_WR_REG(USB_BASE, USB_INT_CLEAR, tmpVal);

    return SUCCESS;
}

/*@} end of group USB_Public_Functions */

/*@} end of group USB */

/*@} end of group BL70X_Peripheral_Driver */
