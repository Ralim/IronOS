/**
 * @file hal_usb.c
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
#include "hal_usb.h"
#include "hal_dma.h"
#include "hal_mtimer.h"
#include "bl702_usb.h"
#include "bl702_glb.h"

#define USE_INTERNAL_TRANSCEIVER
//#define ENABLE_LPM_INT
//#define ENABLE_SOF3MS_INT
//#define ENABLE_ERROR_INT

#define MIN(a, b) (((a) < (b)) ? (a) : (b))

#define USB_DC_LOG_WRN(a, ...) //bflb_platform_printf(a, ##__VA_ARGS__)
#define USB_DC_LOG_DBG(a, ...)
#define USB_DC_LOG_ERR(a, ...) //bflb_platform_printf(a, ##__VA_ARGS__)
#define USB_DC_LOG(a, ...)

static usb_dc_device_t usb_fs_device;
static void USB_FS_IRQ(void);

static dma_lli_ctrl_t usb_lli_list = {
    .src_addr = 0,
    .dst_addr = 0,
    .nextlli = 0,
    .cfg.bits.fix_cnt = 0,
    .cfg.bits.dst_min_mode = 0,
    .cfg.bits.dst_add_mode = 0,
    .cfg.bits.SI = 0,
    .cfg.bits.DI = 0,
    .cfg.bits.SWidth = DMA_TRANSFER_WIDTH_8BIT,
    .cfg.bits.DWidth = DMA_TRANSFER_WIDTH_8BIT,
    .cfg.bits.SBSize = 0,
    .cfg.bits.DBSize = 0,
    .cfg.bits.I = 0,
    .cfg.bits.TransferSize = 0
};

static void usb_set_power_up(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_USB_XCVR);
    tmpVal = BL_SET_REG_BIT(tmpVal, GLB_PU_USB);
    BL_WR_REG(GLB_BASE, GLB_USB_XCVR, tmpVal);
}

static void usb_set_power_off(void)
{
    uint32_t tmpVal = 0;

    tmpVal = BL_RD_REG(GLB_BASE, GLB_USB_XCVR);
    tmpVal = BL_CLR_REG_BIT(tmpVal, GLB_PU_USB);
    BL_WR_REG(GLB_BASE, GLB_USB_XCVR, tmpVal);
}

static uint8_t usb_ep_is_enabled(uint8_t ep)
{
    uint8_t ep_idx = USB_EP_GET_IDX(ep);

    /* Check if ep enabled */
    if ((USB_EP_DIR_IS_OUT(ep)) &&
        usb_fs_device.out_ep[ep_idx].ep_ena) {
        return 1;
    } else if ((USB_EP_DIR_IS_IN(ep)) &&
               usb_fs_device.in_ep[ep_idx].ep_ena) {
        return 1;
    }

    return 0;
}

static void usb_xcvr_config(BL_Fun_Type NewState)
{
    uint32_t tmpVal = 0;

    if (NewState != DISABLE) {
#if defined(USE_EXTERNAL_TRANSCEIVER)
        tmpVal = BL_RD_REG(GLB_BASE, GLB_USB_XCVR_CONFIG);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_REG_USB_USE_XCVR, 0); //use external tranceiver
        BL_WR_REG(GLB_BASE, GLB_USB_XCVR_CONFIG, tmpVal);
#elif defined(USE_INTERNAL_TRANSCEIVER)
#if 1
        tmpVal = BL_RD_REG(GLB_BASE, GLB_USB_XCVR);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_PU_USB, 1);
        BL_WR_REG(GLB_BASE, GLB_USB_XCVR, tmpVal);

        tmpVal = BL_RD_REG(GLB_BASE, GLB_USB_XCVR);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_USB_SUS, 0);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_USB_SPD, 1); //0 for 1.1 ls,1 for 1.1 fs
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_USB_DATA_CONVERT, 0);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_USB_OEB_SEL, 0);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_USB_ROUT_PMOS, 3);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_USB_ROUT_NMOS, 3);
        BL_WR_REG(GLB_BASE, GLB_USB_XCVR, tmpVal);

        tmpVal = BL_RD_REG(GLB_BASE, GLB_USB_XCVR_CONFIG);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_USB_SLEWRATE_P_RISE, 2); //1 for 1.1 ls
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_USB_SLEWRATE_P_FALL, 2); //1 for 1.1 ls
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_USB_SLEWRATE_M_RISE, 2); //1 for 1.1 ls
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_USB_SLEWRATE_M_FALL, 2); //1 for 1.1 ls
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_USB_RES_PULLUP_TUNE, 5);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_REG_USB_USE_XCVR, 1);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_USB_BD_VTH, 1);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_USB_V_HYS_P, 2);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_USB_V_HYS_M, 2);
        BL_WR_REG(GLB_BASE, GLB_USB_XCVR_CONFIG, tmpVal);

        ///* force BD=1, not use */
        //tmpVal=BL_RD_REG(GLB_BASE,GLB_USB_XCVR);
        //tmpVal=BL_SET_REG_BIT(tmpVal,GLB_PU_USB_LDO);
        //BL_WR_REG(GLB_BASE,GLB_USB_XCVR,tmpVal);

        /* BD_voltage_thresdhold=2.8V */
        tmpVal = BL_RD_REG(GLB_BASE, GLB_USB_XCVR_CONFIG);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_USB_BD_VTH, 7);
        BL_WR_REG(GLB_BASE, GLB_USB_XCVR_CONFIG, tmpVal);

#else
        tmpVal = BL_RD_REG(GLB_BASE, GLB_USB_XCVR);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_PU_USB, 1);
        BL_WR_REG(GLB_BASE, GLB_USB_XCVR, tmpVal);

        tmpVal = BL_RD_REG(GLB_BASE, GLB_USB_XCVR);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_USB_SUS, 0);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_USB_SPD, 0); //0 for 1.1 ls,1 for 1.1 fs
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_USB_DATA_CONVERT, 0);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_USB_OEB_SEL, 0);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_USB_ROUT_PMOS, 3);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_USB_ROUT_NMOS, 3);
        BL_WR_REG(GLB_BASE, GLB_USB_XCVR, tmpVal);

        tmpVal = BL_RD_REG(GLB_BASE, GLB_USB_XCVR_CONFIG);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_USB_SLEWRATE_P_RISE, 1); //4 for 1.1 fs
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_USB_SLEWRATE_P_FALL, 1); //3 for 1.1 fs
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_USB_SLEWRATE_M_RISE, 1); //4 for 1.1 fs
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_USB_SLEWRATE_M_FALL, 1); //3 for 1.1 fs
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_USB_RES_PULLUP_TUNE, 5);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_REG_USB_USE_XCVR, 1);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_USB_BD_VTH, 1);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_USB_V_HYS_P, 2);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_USB_V_HYS_M, 2);
        BL_WR_REG(GLB_BASE, GLB_USB_XCVR_CONFIG, tmpVal);
#endif

        tmpVal = BL_RD_REG(GLB_BASE, GLB_USB_XCVR);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_USB_ENUM, 1);
        BL_WR_REG(GLB_BASE, GLB_USB_XCVR, tmpVal);
#endif
    } else {
#ifdef USE_INTERNAL_TRANSCEIVER
        tmpVal = BL_RD_REG(GLB_BASE, GLB_USB_XCVR);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_USB_ENUM, 0);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_PU_USB, 0);
        BL_WR_REG(GLB_BASE, GLB_USB_XCVR, tmpVal);

        ///* force BD=1, not use */
        //tmpVal=BL_RD_REG(GLB_BASE,GLB_USB_XCVR);
        //tmpVal=BL_SET_REG_BIT(tmpVal,GLB_PU_USB_LDO);
        //BL_WR_REG(GLB_BASE,GLB_USB_XCVR,tmpVal);

        /* BD_voltage_thresdhold=2.8V */
        tmpVal = BL_RD_REG(GLB_BASE, GLB_USB_XCVR_CONFIG);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_USB_BD_VTH, 7);
        BL_WR_REG(GLB_BASE, GLB_USB_XCVR_CONFIG, tmpVal);

#else
        tmpVal = BL_RD_REG(GLB_BASE, GLB_USB_XCVR_CONFIG);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_REG_USB_USE_XCVR, 1); //use internal tranceiver
        BL_WR_REG(GLB_BASE, GLB_USB_XCVR_CONFIG, tmpVal);

        tmpVal = BL_RD_REG(GLB_BASE, GLB_USB_XCVR);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_USB_ENUM, 0);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_PU_USB, 1);
        BL_WR_REG(GLB_BASE, GLB_USB_XCVR, tmpVal);
#endif
    }
}

/**
 * @brief
 *
 * @param dev
 * @param oflag
 * @return int
 */
int usb_open(struct device *dev, uint16_t oflag)
{
    USB_Config_Type usbCfg = { 0 };

    usb_set_power_off();
    mtimer_delay_ms(10);
    usb_set_power_up();

    usb_xcvr_config(DISABLE);
    usb_xcvr_config(ENABLE);

    CPU_Interrupt_Disable(USB_IRQn);

    usbCfg.DeviceAddress = 0;
    usbCfg.EnumInEn = ENABLE;
    usbCfg.EnumOutEn = ENABLE;
    usbCfg.RomBaseDescriptorUsed = 0;
    usbCfg.SoftwareCtrl = 1;
    usbCfg.EnumMaxPacketSize = USB_CTRL_EP_MPS;

    /* Init Device */
    USB_Set_Config(DISABLE, &usbCfg);

    usb_fs_device.out_ep[0].ep_ena = 1U;
    usb_fs_device.in_ep[0].ep_ena = 1U;
    usb_fs_device.out_ep[0].ep_cfg.ep_mps = USB_CTRL_EP_MPS;
    usb_fs_device.out_ep[0].ep_cfg.ep_type = USBD_EP_TYPE_CTRL;
    usb_fs_device.in_ep[0].ep_cfg.ep_mps = USB_CTRL_EP_MPS;
    usb_fs_device.in_ep[0].ep_cfg.ep_type = USBD_EP_TYPE_CTRL;

    /* USB interrupt enable config */
    BL_WR_REG(USB_BASE, USB_INT_EN, 0);
    USB_IntEn(USB_INT_RESET, ENABLE);          //1
    USB_IntEn(USB_INT_EP0_SETUP_DONE, ENABLE); //5
    USB_IntEn(USB_INT_EP0_IN_DONE, ENABLE);    //7
    USB_IntEn(USB_INT_EP0_OUT_DONE, ENABLE);   //9
    USB_IntEn(USB_INT_RESET_END, ENABLE);      //27

    /* USB interrupt mask config */
    BL_WR_REG(USB_BASE, USB_INT_MASK, 0xffffffff);
    USB_IntMask(USB_INT_RESET, UNMASK);          //1
    USB_IntMask(USB_INT_EP0_SETUP_DONE, UNMASK); //5
    USB_IntMask(USB_INT_EP0_IN_DONE, UNMASK);    //7
    USB_IntMask(USB_INT_EP0_OUT_DONE, UNMASK);   //9
    USB_IntMask(USB_INT_RESET_END, UNMASK);      //27

#ifdef ENABLE_LPM_INT
    USB_IntEn(USB_INT_LPM_PACKET, ENABLE);
    USB_IntEn(USB_INT_LPM_WAKEUP, ENABLE);
    USB_IntMask(USB_INT_LPM_PACKET, UNMASK);
    USB_IntMask(USB_INT_LPM_WAKEUP, UNMASK);

    USB_LPM_Enable();
    USB_Set_LPM_Default_Response(USB_LPM_DEFAULT_RESP_ACK);

#endif

#ifdef ENABLE_SOF3MS_INT
    /* disable sof3ms until reset_end */
    USB_IntEn(USB_INT_LOST_SOF_3_TIMES, DISABLE);
    USB_IntMask(USB_INT_LOST_SOF_3_TIMES, MASK);

    /* recommended enable sof3ms after reset_end */
    USB_IntEn(USB_INT_LOST_SOF_3_TIMES, ENABLE);
    USB_IntMask(USB_INT_LOST_SOF_3_TIMES, UNMASK);
#endif

#ifdef ENABLE_ERROR_INT
    USB_IntEn(USB_INT_ERROR, ENABLE);
    USB_IntMask(USB_INT_ERROR, UNMASK);
#endif
    /*Clear pending interrupts*/
    USB_Clr_IntStatus(USB_INT_ALL);

    Interrupt_Handler_Register(USB_IRQn, USB_FS_IRQ);
    CPU_Interrupt_Enable(USB_IRQn);
    USB_Enable();

    return 0;
}
/**
 * @brief
 *
 * @param dev
 * @return int
 */
int usb_close(struct device *dev)
{
    /* disable all interrupts and force USB reset */
    CPU_Interrupt_Disable(USB_IRQn);
    USB_IntMask(USB_INT_LPM_WAKEUP, MASK);
    USB_IntMask(USB_INT_LPM_PACKET, MASK);

    USB_Disable();

    /* clear interrupt status register */
    USB_Clr_IntStatus(USB_INT_ALL);

    usb_set_power_off();

    usb_xcvr_config(DISABLE);
    GLB_AHB_Slave1_Reset(BL_AHB_SLAVE1_USB);
    return 0;
}
/**
 * @brief
 *
 * @param dev
 * @param cmd
 * @param args
 * @return int
 */
int usb_control(struct device *dev, int cmd, void *args)
{
    struct usb_dc_device *usb_device = (struct usb_dc_device *)dev;

    switch (cmd) {
        case DEVICE_CTRL_SET_INT /* constant-expression */: {
            uint32_t offset = __builtin_ctz((uint32_t)args);

            while (offset < 24) {
                if ((uint32_t)args & (1 << offset)) {
                    USB_IntEn(offset, ENABLE);
                    USB_IntMask(offset, UNMASK);
                }

                offset++;
            }
            break;
        }
        case DEVICE_CTRL_CLR_INT /* constant-expression */: {
            uint32_t offset = __builtin_ctz((uint32_t)args);

            while (offset < 24) {
                if ((uint32_t)args & (1 << offset)) {
                    USB_IntEn(offset, DISABLE);
                    USB_IntMask(offset, MASK);
                }

                offset++;
            }
            break;
        }
        case DEVICE_CTRL_USB_DC_SET_ACK /* constant-expression */:
            USB_Set_EPx_Status(USB_EP_GET_IDX(((uint32_t)args) & 0x7f), USB_EP_STATUS_ACK);
            return 0;
        case DEVICE_CTRL_USB_DC_ENUM_ON: {
            uint32_t tmpVal;
            tmpVal = BL_RD_REG(GLB_BASE, GLB_USB_XCVR);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_USB_ENUM, 1);
            BL_WR_REG(GLB_BASE, GLB_USB_XCVR, tmpVal);
            return 0;
        }
        case DEVICE_CTRL_USB_DC_ENUM_OFF: {
            uint32_t tmpVal;
            tmpVal = BL_RD_REG(GLB_BASE, GLB_USB_XCVR);
            tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_USB_ENUM, 0);
            BL_WR_REG(GLB_BASE, GLB_USB_XCVR, tmpVal);
            return 0;
        }
        case DEVICE_CTRL_USB_DC_GET_EP_TX_FIFO_CNT:
            return USB_Get_EPx_TX_FIFO_CNT(((uint32_t)args) & 0x7f);

        case DEVICE_CTRL_USB_DC_GET_EP_RX_FIFO_CNT:
            return USB_Get_EPx_RX_FIFO_CNT(((uint32_t)args) & 0x7f);
        case DEVICE_CTRL_ATTACH_TX_DMA /* constant-expression */:
            usb_device->tx_dma = (struct device *)args;
            break;

        case DEVICE_CTRL_ATTACH_RX_DMA /* constant-expression */:
            usb_device->rx_dma = (struct device *)args;
            break;

        case DEVICE_CTRL_USB_DC_SET_TX_DMA /* constant-expression */:
            USB_Set_EPx_TX_DMA_Interface_Config(((uint32_t)args) & 0x7f, ENABLE);
            break;

        case DEVICE_CTRL_USB_DC_SET_RX_DMA /* constant-expression */:
            USB_Set_EPx_RX_DMA_Interface_Config(((uint32_t)args) & 0x7f, ENABLE);
            break;

        default:
            break;
    }

    return 0;
}

int usb_write(struct device *dev, uint32_t pos, const void *buffer, uint32_t size)
{
    struct usb_dc_device *usb_device = (struct usb_dc_device *)dev;
    uint8_t ep_idx = USB_EP_GET_IDX(pos);

    if (usb_device->in_ep[ep_idx].ep_cfg.ep_type == USBD_EP_TYPE_ISOC) {
        uint32_t usb_ep_addr = USB_BASE + 0x308 + ep_idx * 0x10;

        dma_channel_stop(usb_device->tx_dma);
        usb_lli_list.src_addr = (uint32_t)buffer;
        usb_lli_list.dst_addr = usb_ep_addr;
        usb_lli_list.cfg.bits.TransferSize = size;
        usb_lli_list.cfg.bits.DI = 0;
        usb_lli_list.cfg.bits.SI = 1;
        usb_lli_list.cfg.bits.SBSize = DMA_BURST_16BYTE;
        usb_lli_list.cfg.bits.DBSize = DMA_BURST_1BYTE;
        dma_channel_update(usb_device->tx_dma, (void *)((uint32_t)&usb_lli_list));
        dma_channel_start(usb_device->tx_dma);
        return 0;
    } else {
    }

    return -1;
}

int usb_read(struct device *dev, uint32_t pos, void *buffer, uint32_t size)
{
    struct usb_dc_device *usb_device = (struct usb_dc_device *)dev;
    uint8_t ep_idx = USB_EP_GET_IDX(pos);

    if (usb_device->out_ep[ep_idx].ep_cfg.ep_type == USBD_EP_TYPE_ISOC) {
        uint32_t usb_ep_addr = USB_BASE + 0x308 + ep_idx * 0x1c;

        dma_channel_stop(usb_device->tx_dma);
        usb_lli_list.src_addr = usb_ep_addr;
        usb_lli_list.dst_addr = (uint32_t)buffer;
        usb_lli_list.cfg.bits.TransferSize = size;
        usb_lli_list.cfg.bits.DI = 1;
        usb_lli_list.cfg.bits.SI = 0;
        usb_lli_list.cfg.bits.SBSize = DMA_BURST_1BYTE;
        usb_lli_list.cfg.bits.DBSize = DMA_BURST_16BYTE;
        dma_channel_update(usb_device->rx_dma, (void *)((uint32_t)&usb_lli_list));
        dma_channel_start(usb_device->rx_dma);
        return 0;
    } else {
    }

    return -1;
}

/**
 * @brief
 *
 * @param index
 * @param name
 * @param flag
 * @return int
 */
int usb_dc_register(enum usb_index_type index, const char *name)
{
    struct device *dev;

    if (USB_MAX_INDEX == 0) {
        return -DEVICE_EINVAL;
    }

    dev = &(usb_fs_device.parent);

    dev->open = usb_open;
    dev->close = usb_close;
    dev->control = usb_control;
    dev->write = usb_write;
    dev->read = usb_read;

    dev->type = DEVICE_CLASS_USB;
    dev->handle = NULL;

    return device_register(dev, name);
}

/**
 * @brief Set USB device address
 *
 * @param[in] addr Device address
 *
 * @return 0 on success, negative errno code on fail.
 */
int usb_dc_set_dev_address(const uint8_t addr)
{
    USB_Set_Device_Addr(addr);
    return 0;
}

/**
 * @brief configure and enable endpoint
 * This function sets endpoint configuration according to one specified in USB
 * endpoint descriptor and then enables it for data transfers.
 *
 * @param dev
 * @param ep_cfg  ep_cfg Endpoint
 * @return int
 */
int usb_dc_ep_open(struct device *dev, const struct usb_dc_ep_cfg *ep_cfg)
{
    uint8_t ep;
    EP_Config_Type epCfg;

    if (!ep_cfg) {
        return -1;
    }

    ep = ep_cfg->ep_addr;

    uint8_t ep_idx = USB_EP_GET_IDX(ep);

    USB_DC_LOG_DBG("%s ep %x, mps %d, type %d\r\n", __func__, ep, ep_cfg->ep_mps, ep_cfg->ep_type);

    if (ep_idx == 0) {
        return 0;
    }

    if (USB_EP_DIR_IS_OUT(ep)) {
        epCfg.dir = EP_OUT;
        epCfg.EPMaxPacketSize = ep_cfg->ep_mps;
        usb_fs_device.out_ep[ep_idx].ep_cfg.ep_mps = ep_cfg->ep_mps;
        usb_fs_device.out_ep[ep_idx].ep_cfg.ep_type = ep_cfg->ep_type;
    } else {
        epCfg.dir = EP_IN;
        epCfg.EPMaxPacketSize = ep_cfg->ep_mps;
        usb_fs_device.in_ep[ep_idx].ep_cfg.ep_mps = ep_cfg->ep_mps;
        usb_fs_device.in_ep[ep_idx].ep_cfg.ep_type = ep_cfg->ep_type;
    }

    switch (ep_cfg->ep_type) {
        case USBD_EP_TYPE_CTRL:
            epCfg.type = USB_DC_EP_TYPE_CTRL;
            break;

        case USBD_EP_TYPE_ISOC:
            epCfg.type = USB_DC_EP_TYPE_ISOC;
            break;

        case USBD_EP_TYPE_BULK:
            epCfg.type = USB_DC_EP_TYPE_BULK;
            break;

        case USBD_EP_TYPE_INTR:
            epCfg.type = USB_DC_EP_TYPE_INTR;
            break;

        default:
            return -1;
    }

    USB_Set_EPx_Config(ep_idx, &epCfg);

    if (USB_EP_DIR_IS_OUT(ep)) {
        /* Clear NAK and enable ep */
        USB_Set_EPx_Status(USB_EP_GET_IDX(ep), USB_EP_STATUS_ACK);
        usb_fs_device.out_ep[ep_idx].ep_ena = 1U;
    } else {
        //USB_Set_EPx_Status(USB_EP_GET_IDX(ep), USB_EP_STATUS_ACK);
        USB_Set_EPx_Status(USB_EP_GET_IDX(ep), USB_EP_STATUS_NACK);
        usb_fs_device.in_ep[ep_idx].ep_ena = 1U;
    }

    return 0;
}

int usb_dc_ep_close(const uint8_t ep)
{
    return 0;
}

/**
 * @brief Set stall condition for the selected endpoint
 *
 * @param[in] ep Endpoint address corresponding to the one
 *               listed in the device configuration table
 *
 * @return 0 on success, negative errno code on fail.
 */
int usb_dc_ep_set_stall(const uint8_t ep)
{
    uint32_t tmpVal = 0;
    uint8_t ep_idx = USB_EP_GET_IDX(ep);

    if (USB_EP_DIR_IS_OUT(ep)) {
        usb_fs_device.out_ep[ep_idx].is_stalled = 1U;
    } else {
        usb_fs_device.in_ep[ep_idx].is_stalled = 1U;
    }

    switch (ep_idx) {
        case 0:
            tmpVal = BL_RD_REG(USB_BASE, USB_CONFIG);
            tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_USB_EP0_SW_STALL);
            BL_WR_REG(USB_BASE, USB_CONFIG, tmpVal);
            break;
        case 1:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP1_CONFIG);
            tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP1_STALL);
            BL_WR_REG(USB_BASE, USB_EP1_CONFIG, tmpVal);
            break;
        case 2:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP2_CONFIG);
            tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP2_STALL);
            BL_WR_REG(USB_BASE, USB_EP2_CONFIG, tmpVal);
            break;
        case 3:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP3_CONFIG);
            tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP3_STALL);
            BL_WR_REG(USB_BASE, USB_EP3_CONFIG, tmpVal);
            break;
        case 4:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP4_CONFIG);
            tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP4_STALL);
            BL_WR_REG(USB_BASE, USB_EP4_CONFIG, tmpVal);
            break;
        case 5:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP5_CONFIG);
            tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP5_STALL);
            BL_WR_REG(USB_BASE, USB_EP5_CONFIG, tmpVal);
            break;
        case 6:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP6_CONFIG);
            tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP6_STALL);
            BL_WR_REG(USB_BASE, USB_EP6_CONFIG, tmpVal);
            break;
        case 7:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP7_CONFIG);
            tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP7_STALL);
            BL_WR_REG(USB_BASE, USB_EP7_CONFIG, tmpVal);
            break;

        default:
            break;
    }
    return 0;
}
/**
 * @brief Clear stall condition for the selected endpoint
 *
 * @param[in] ep Endpoint address corresponding to the one
 *               listed in the device configuration table
 *
 * @return 0 on success, negative errno code on fail.
 */
int usb_dc_ep_clear_stall(const uint8_t ep)
{
    uint8_t ep_idx = USB_EP_GET_IDX(ep);
    uint32_t tmpVal = 0;
    if (USB_EP_DIR_IS_OUT(ep)) {
        usb_fs_device.out_ep[ep_idx].is_stalled = 0;
    } else {
        usb_fs_device.in_ep[ep_idx].is_stalled = 0;
    }
    switch (ep_idx) {
        case 0:
            break;
        case 1:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP1_CONFIG);
            tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP1_RDY);
            tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP1_NACK);
            tmpVal = BL_CLR_REG_BIT(tmpVal, USB_CR_EP1_STALL);
            BL_WR_REG(USB_BASE, USB_EP1_CONFIG, tmpVal);
            break;
        case 2:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP2_CONFIG);
            tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP2_RDY);
            tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP2_NACK);
            tmpVal = BL_CLR_REG_BIT(tmpVal, USB_CR_EP2_STALL);
            BL_WR_REG(USB_BASE, USB_EP2_CONFIG, tmpVal);
            break;
        case 3:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP3_CONFIG);
            tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP3_RDY);
            tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP3_NACK);
            tmpVal = BL_CLR_REG_BIT(tmpVal, USB_CR_EP3_STALL);
            BL_WR_REG(USB_BASE, USB_EP3_CONFIG, tmpVal);
            break;
        case 4:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP4_CONFIG);
            tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP4_RDY);
            tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP4_NACK);
            tmpVal = BL_CLR_REG_BIT(tmpVal, USB_CR_EP4_STALL);
            BL_WR_REG(USB_BASE, USB_EP4_CONFIG, tmpVal);
            break;
        case 5:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP5_CONFIG);
            tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP5_RDY);
            tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP5_NACK);
            tmpVal = BL_CLR_REG_BIT(tmpVal, USB_CR_EP5_STALL);
            BL_WR_REG(USB_BASE, USB_EP5_CONFIG, tmpVal);
            break;
        case 6:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP6_CONFIG);
            tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP6_RDY);
            tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP6_NACK);
            tmpVal = BL_CLR_REG_BIT(tmpVal, USB_CR_EP6_STALL);
            BL_WR_REG(USB_BASE, USB_EP6_CONFIG, tmpVal);
            break;
        case 7:
            tmpVal = BL_RD_REG(USB_BASE, USB_EP7_CONFIG);
            tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP7_RDY);
            tmpVal = BL_SET_REG_BIT(tmpVal, USB_CR_EP7_NACK);
            tmpVal = BL_CLR_REG_BIT(tmpVal, USB_CR_EP7_STALL);
            BL_WR_REG(USB_BASE, USB_EP7_CONFIG, tmpVal);
            break;

        default:
            break;
    }
    return 0;
}

/**
 * @brief Check if the selected endpoint is stalled
 *
 * @param dev usb device
 * @param[in]  ep       Endpoint address corresponding to the one
 *                      listed in the device configuration table
 * @param[out] stalled  Endpoint stall status
 *
 * @return 0 on success, negative errno code on fail.
 */
int usb_dc_ep_is_stalled(struct device *dev, const uint8_t ep, uint8_t *stalled)
{
    uint8_t ep_idx = USB_EP_GET_IDX(ep);

    if (!stalled) {
        return -1;
    }

    *stalled = 0U;

    if (USB_EP_DIR_IS_OUT(ep)) {
        if ((USB_Get_EPx_Status(ep_idx) & USB_EP_STATUS_STALL) && usb_fs_device.out_ep[ep_idx].is_stalled) {
            *stalled = 1U;
        }
    } else {
        if ((USB_Get_EPx_Status(ep_idx) & USB_EP_STATUS_STALL) && usb_fs_device.in_ep[ep_idx].is_stalled) {
            *stalled = 1U;
        }
    }

    return 0;
}

/**
 * @brief Write data to the specified endpoint
 *
 * This function is called to write data to the specified endpoint. The
 * supplied usbd_endpoint_callback function will be called when data is transmitted
 * out.
 *
 * @param dev
 * @param[in]  ep        Endpoint address corresponding to the one
 *                       listed in the device configuration table
 * @param[in]  data      Pointer to data to write
 * @param[in]  data_len  Length of the data requested to write. This may
 *                       be zero for a zero length status packet.
 * @param[out] ret_bytes Bytes scheduled for transmission. This value
 *                       may be NULL if the application expects all
 *                       bytes to be written
 *
 * @return 0 on success, negative errno code on fail.
 */
int usb_dc_ep_write(struct device *dev, const uint8_t ep, const uint8_t *data, uint32_t data_len, uint32_t *ret_bytes)
{
    uint8_t ep_idx;
    uint32_t timeout = 0x00FFFFFF;
    uint32_t ep_tx_fifo_addr;

    ep_idx = USB_EP_GET_IDX(ep);

    /* Check if IN ep */
    if (USB_EP_GET_DIR(ep) != USB_EP_DIR_IN) {
        return -USB_DC_EP_DIR_ERR;
    }

    /* Check if ep enabled */
    if (!usb_ep_is_enabled(ep)) {
        return -USB_DC_EP_EN_ERR;
    }

    if (!data && data_len) {
        USB_DC_LOG_ERR("data is null\r\n");
        return -USB_DC_ADDR_ERR;
    }

    /* Check if ep free */
    while (!USB_Is_EPx_RDY_Free(ep_idx) && (usb_fs_device.in_ep[ep_idx].ep_cfg.ep_type != USBD_EP_TYPE_ISOC)) {
        timeout--;

        if (!timeout) {
            USB_DC_LOG_ERR("ep%d wait free timeout\r\n", ep);
            return -USB_DC_EP_TIMEOUT_ERR;
        }
    }

    if (!data_len) {
        /* Zero length packet */
        if ((USB_EP_GET_IDX(ep) != 0)) {
            /* Clear NAK and enable ep */
            USB_Set_EPx_Rdy(USB_EP_GET_IDX(ep));
            return USB_DC_OK;
        }
        return USB_DC_OK;
    }

    if (data_len > usb_fs_device.in_ep[ep_idx].ep_cfg.ep_mps) {
        /* Check if transfer len is too big */
        data_len = usb_fs_device.in_ep[ep_idx].ep_cfg.ep_mps;
    }

    /* Wait for FIFO space available */
    do {
        uint32_t avail_space = USB_Get_EPx_TX_FIFO_CNT(ep_idx);

        if (avail_space >= usb_fs_device.in_ep[ep_idx].ep_cfg.ep_mps) {
            break;
        }

        //USB_DC_LOG_ERR("EP%d have remain data\r\n", ep_idx);
    } while (1);

    /*
     * Write data to FIFO, make sure that we are protected against
     * other USB register accesses.  According to "DesignWare Cores
     * USB 1.1/2.0 Device Subsystem-AHB/VCI Databook": "During FIFO
     * access, the application must not access the UDC/Subsystem
     * registers or vendor registers (for ULPI mode). After starting
     * to access a FIFO, the application must complete the transaction
     * before accessing the register."
     */
    ep_tx_fifo_addr = USB_BASE + USB_EP0_TX_FIFO_WDATA_OFFSET + ep_idx * 0x10;

    if ((data_len == 1) && (ep_idx == 0)) {
        USB_Set_EPx_Xfer_Size(EP_ID0, 1);
    } else if (ep_idx == 0) {
        USB_Set_EPx_Xfer_Size(EP_ID0, 64);
    }

    memcopy_to_fifo((void *)ep_tx_fifo_addr, (uint8_t *)data, data_len);
    /* Clear NAK and enable ep */
    if (USB_EP_GET_IDX(ep) != 0)
        USB_Set_EPx_Rdy(USB_EP_GET_IDX(ep));
    USB_DC_LOG_DBG("EP%d write %u bytes\r\n", ep_idx, data_len);

    if (ret_bytes) {
        *ret_bytes = data_len;
    }

    return USB_DC_OK;
}

/**
 * @brief Read data from the specified endpoint
 *
 * This is similar to usb_dc_ep_read, the difference being that, it doesn't
 * clear the endpoint NAKs so that the consumer is not bogged down by further
 * upcalls till he is done with the processing of the data. The caller should
 * reactivate ep by invoking usb_dc_ep_read_continue() do so.
 *
 * @param dev
 * @param[in]  ep           Endpoint address corresponding to the one
 *                          listed in the device configuration table
 * @param[in]  data         Pointer to data buffer to write to
 * @param[in]  max_data_len Max length of data to read
 * @param[out] read_bytes   Number of bytes read. If data is NULL and
 *                          max_data_len is 0 the number of bytes
 *                          available for read should be returned.
 *
 * @return 0 on success, negative errno code on fail.
 */
int usb_dc_ep_read(struct device *dev, const uint8_t ep, uint8_t *data, uint32_t data_len, uint32_t *read_bytes)
{
    uint8_t ep_idx = USB_EP_GET_IDX(ep);
    uint32_t read_count;
    uint32_t ep_rx_fifo_addr;
    uint32_t timeout = 0x00FFFFFF;

    /* Check if OUT ep */
    if (USB_EP_GET_DIR(ep) != USB_EP_DIR_OUT) {
        USB_DC_LOG_ERR("Wrong endpoint direction\r\n");
        return -USB_DC_EP_DIR_ERR;
    }

    /* Check if ep enabled */
    if (!usb_ep_is_enabled(ep)) {
        USB_DC_LOG_ERR("Not enabled endpoint\r\n");
        return -USB_DC_EP_EN_ERR;
    }

    if (!data && data_len) {
        USB_DC_LOG_ERR("data is null\r\n");
        return -USB_DC_ADDR_ERR;
    }

    /* Check if ep free */
    while (!USB_Is_EPx_RDY_Free(ep_idx) && (usb_fs_device.out_ep[ep_idx].ep_cfg.ep_type != USBD_EP_TYPE_ISOC)) {
        timeout--;

        if (!timeout) {
            USB_DC_LOG_ERR("ep%d wait free timeout\r\n", ep);
            return -USB_DC_EP_TIMEOUT_ERR;
        }
    }

    if (!data_len) {
        if ((USB_EP_GET_IDX(ep) != 0)) {
            /* Clear NAK and enable ep */
            USB_Set_EPx_Rdy(USB_EP_GET_IDX(ep));
            return USB_DC_OK;
        }
    }

    read_count = USB_Get_EPx_RX_FIFO_CNT(ep_idx);
    read_count = MIN(read_count, data_len);

    /* Data in the FIFOs is always stored per 8-bit word*/
    ep_rx_fifo_addr = (USB_BASE + USB_EP0_RX_FIFO_RDATA_OFFSET + ep_idx * 0x10);
    fifocopy_to_mem((void *)ep_rx_fifo_addr, data, read_count);
    USB_DC_LOG_DBG("Read EP%d, req %d, read %d bytes\r\n", ep, data_len, read_count);

    if (read_bytes) {
        *read_bytes = read_count;
    }

    return USB_DC_OK;
}
/**
 * @brief
 *
 * @param dev
 * @param rb
 * @param ep
 * @return int
 */
int usb_dc_receive_to_ringbuffer(struct device *dev, Ring_Buffer_Type *rb, uint8_t ep)
{
    uint8_t ep_idx;
    uint8_t recv_len;
    static bool overflow_flag = false;

    /* Check if OUT ep */
    if (USB_EP_GET_DIR(ep) != USB_EP_DIR_OUT) {
        USB_DC_LOG_ERR("Wrong endpoint direction\r\n");
        return -USB_DC_EP_DIR_ERR;
    }

    /* Check if ep enabled */
    if (!usb_ep_is_enabled(ep)) {
        return -USB_DC_EP_EN_ERR;
    }

    ep_idx = USB_EP_GET_IDX(ep);

    recv_len = USB_Get_EPx_RX_FIFO_CNT(ep_idx);

    /*if rx fifo count equal 0,it means last is send nack and ringbuffer is smaller than 64,
    * so,if ringbuffer is larger than 64,set ack to recv next data.
    */
    if (overflow_flag && (Ring_Buffer_Get_Empty_Length(rb) > 64) && (!recv_len)) {
        overflow_flag = false;
        USB_Set_EPx_Rdy(ep_idx);
        return 0;
    } else {
        uint32_t addr = USB_BASE + 0x11C + (ep_idx - 1) * 0x10;
        Ring_Buffer_Write_Callback(rb, recv_len, fifocopy_to_mem, (void *)addr);

        if (Ring_Buffer_Get_Empty_Length(rb) < 64) {
            overflow_flag = true;
            return -USB_DC_RB_SIZE_SMALL_ERR;
        }

        USB_Set_EPx_Rdy(ep_idx);
        return 0;
    }
}
/**
 * @brief
 *
 * @param dev
 * @param rb
 * @param ep
 * @return int
 */
int usb_dc_send_from_ringbuffer(struct device *dev, Ring_Buffer_Type *rb, uint8_t ep)
{
    uint8_t ep_idx;
    static bool zlp_flag = false;
    static uint32_t send_total_len = 0;

    ep_idx = USB_EP_GET_IDX(ep);

    /* Check if IN ep */
    if (USB_EP_GET_DIR(ep) != USB_EP_DIR_IN) {
        return -USB_DC_EP_DIR_ERR;
    }

    /* Check if ep enabled */
    if (!usb_ep_is_enabled(ep)) {
        return -USB_DC_EP_EN_ERR;
    }

    uint32_t addr = USB_BASE + 0x118 + (ep_idx - 1) * 0x10;

    if (zlp_flag == false) {
        if ((USB_Get_EPx_TX_FIFO_CNT(ep_idx) == USB_FS_MAX_PACKET_SIZE) && Ring_Buffer_Get_Length(rb)) {
            uint32_t actual_len = Ring_Buffer_Read_Callback(rb, USB_FS_MAX_PACKET_SIZE, memcopy_to_fifo, (void *)addr);
            send_total_len += actual_len;

            if (!Ring_Buffer_Get_Length(rb) && (!(send_total_len % 64))) {
                zlp_flag = true;
            }

            USB_Set_EPx_Rdy(ep_idx);
            return 0;
        } else {
            return -USB_DC_RB_SIZE_SMALL_ERR;
        }
    } else {
        zlp_flag = false;
        send_total_len = 0;
        USB_Set_EPx_Rdy(ep_idx);
        return -USB_DC_ZLP_ERR;
    }
}

/**
 * @brief
 *
 * @param device
 */
void usb_dc_isr(usb_dc_device_t *device)
{
    USB_EP_ID epnum = EP_ID0;

    /* EP1_DONE -> EP2_DONE -> ...... -> EP7_DONE*/
    for (USB_INT_Type epint = USB_INT_EP1_DONE; epint <= USB_INT_EP7_DONE; epint += 2) {
        if (USB_Get_IntStatus(epint)) {
            epnum = (epint - USB_INT_EP0_OUT_CMD) >> 1;
            if (!USB_Is_EPx_RDY_Free(epnum) && (device->out_ep[epnum].ep_cfg.ep_type != USBD_EP_TYPE_ISOC)) {
                USB_DC_LOG_ERR("ep%d out busy\r\n", epnum);
                continue;
            }
            USB_Clr_IntStatus(epint);
            device->parent.callback(&device->parent, (void *)((uint32_t)USB_SET_EP_OUT(epnum)), 0, USB_DC_EVENT_EP_OUT_NOTIFY);
        }
    }

    /* EP1_CMD -> EP2_CMD -> ...... -> EP7_CMD*/
    for (USB_INT_Type epint = USB_INT_EP1_CMD; epint <= USB_INT_EP7_CMD; epint += 2) {
        if (USB_Get_IntStatus(epint)) {
            epnum = (epint - USB_INT_EP0_OUT_CMD) >> 1;
            if (!USB_Is_EPx_RDY_Free(epnum) && (device->in_ep[epnum].ep_cfg.ep_type != USBD_EP_TYPE_ISOC)) {
                USB_DC_LOG_DBG("ep%d in busy\r\n", epnum);
                continue;
            }
            USB_Clr_IntStatus(epint);
            device->parent.callback(&device->parent, (void *)((uint32_t)USB_SET_EP_IN(epnum)), 0, USB_DC_EVENT_EP_IN_NOTIFY);
        }
    }

    /* EP0 setup done */
    if (USB_Get_IntStatus(USB_INT_EP0_SETUP_DONE)) {
        if (!USB_Is_EPx_RDY_Free(0)) {
            USB_DC_LOG_DBG("ep0 setup busy\r\n");
            return;
        }
        USB_Clr_IntStatus(USB_INT_EP0_SETUP_DONE);
        device->parent.callback(&device->parent, NULL, 0, USB_DC_EVENT_SETUP_NOTIFY);
        USB_Set_EPx_Rdy(EP_ID0);
        return;
    }

    /* EP0 in done */
    if (USB_Get_IntStatus(USB_INT_EP0_IN_DONE)) {
        if (!USB_Is_EPx_RDY_Free(0)) {
            USB_DC_LOG_DBG("ep0 in busy\r\n");
            return;
        }
        USB_Clr_IntStatus(USB_INT_EP0_IN_DONE);
        device->parent.callback(&device->parent, (void *)0x80, 0, USB_DC_EVENT_EP0_IN_NOTIFY);
        USB_Set_EPx_Rdy(EP_ID0);
        return;
    }

    /* EP0 out done */
    if (USB_Get_IntStatus(USB_INT_EP0_OUT_DONE)) {
        if (!USB_Is_EPx_RDY_Free(0)) {
            USB_DC_LOG_DBG("ep0 out busy\r\n");
            return;
        }
        USB_Clr_IntStatus(USB_INT_EP0_OUT_DONE);
        device->parent.callback(&device->parent, (void *)0x00, 0, USB_DC_EVENT_EP0_OUT_NOTIFY);
        USB_Set_EPx_Rdy(EP_ID0);
        return;
    }

    /* sof */
    if (USB_Get_IntStatus(USB_INT_SOF)) {
        USB_Clr_IntStatus(USB_INT_SOF);
        device->parent.callback(&device->parent, NULL, 0, USB_DC_EVENT_SOF);
        return;
    }

    /* reset */
    if (USB_Get_IntStatus(USB_INT_RESET)) {
        USB_Clr_IntStatus(USB_INT_RESET);
        device->parent.callback(&device->parent, NULL, 0, USB_DC_EVENT_RESET);
        return;
    }

    /* reset end */
    if (USB_Get_IntStatus(USB_INT_RESET_END)) {
        USB_Clr_IntStatus(USB_INT_RESET_END);
        USB_Set_EPx_Rdy(EP_ID0);
        return;
    }

    /* vbus toggle */
    if (USB_Get_IntStatus(USB_INT_VBUS_TGL)) {
        USB_Clr_IntStatus(USB_INT_VBUS_TGL);
        return;
    }
#ifdef ENABLE_LPM_INT
    /* LPM wakeup */
    if (USB_Get_IntStatus(USB_INT_LPM_WAKEUP)) {
        USB_Clr_IntStatus(USB_INT_LPM_WAKEUP);
        return;
    }

    /* LPM packet */
    if (USB_Get_IntStatus(USB_INT_LPM_PACKET)) {
        /*************************************/
        /* Force low-power mode in the macrocell */
        if (USB_Get_IntStatus(USB_INT_LPM_WAKEUP) == 0) {
        }

        /*************************************/
        USB_Clr_IntStatus(USB_INT_LPM_PACKET);
        return;
    }
#endif
#ifdef ENABLE_SOF3MS_INT
    /* lost 3 SOF */
    if (USB_Get_IntStatus(USB_INT_LOST_SOF_3_TIMES)) {
        USB_DC_LOG_ERR("Lost 3 SOFs\r\n");
        /*************************************/
        /*************************************/
        USB_Clr_IntStatus(USB_INT_LOST_SOF_3_TIMES);
        return;
    }
#endif
#ifdef ENABLE_ERROR_INT
    /* error */
    if (USB_Get_IntStatus(USB_INT_ERROR)) {
        USB_DC_LOG("USB bus error 0x%08x; EP2 fifo status 0x%08x\r\n", *(volatile uint32_t *)(0x4000D81C), *(volatile uint32_t *)(0x4000D920));
        /*************************************/
        /*************************************/
        device->parent.callback(&device->parent, NULL, 0, USB_DC_EVENT_ERROR);
        USB_Clr_IntStatus(USB_INT_ERROR);
        return;
    }
#endif
}
/**
 * @brief
 *
 */
void USB_FS_IRQ(void)
{
    usb_dc_isr(&usb_fs_device);
}