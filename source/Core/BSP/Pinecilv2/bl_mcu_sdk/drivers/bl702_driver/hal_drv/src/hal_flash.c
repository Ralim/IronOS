/**
 * @file hal_flash.c
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
#include "bl702_glb.h"
#include "bl702_xip_sflash.h"
#include "bl702_xip_sflash_ext.h"
#include "bl702_sf_cfg.h"
#include "bl702_sf_cfg_ext.h"
#include "hal_flash.h"

static uint32_t g_jedec_id = 0;
static SPI_Flash_Cfg_Type g_flash_cfg;

/**
 * @brief flash_get_jedecid
 *
 * @return BL_Err_Type
 */
uint32_t flash_get_jedecid(void)
{
    uint32_t jid = 0;

    jid = ((g_jedec_id&0xff)<<16) + (g_jedec_id&0xff00) + ((g_jedec_id&0xff0000)>>16);
    return jid;
}

/**
 * @brief flash_get_cfg
 *
 * @return BL_Err_Type
 */
BL_Err_Type flash_get_cfg(uint8_t **cfg_addr, uint32_t *len)
{
    *cfg_addr = (uint8_t *)&g_flash_cfg;
    *len = sizeof(SPI_Flash_Cfg_Type);

    return SUCCESS;
}

/**
 * @brief flash_set_qspi_enable
 *
 * @return BL_Err_Type
 */
static BL_Err_Type ATTR_TCM_SECTION flash_set_qspi_enable(SPI_Flash_Cfg_Type *p_flash_cfg)
{
    if ((p_flash_cfg->ioMode & 0x0f) == SF_CTRL_QO_MODE || (p_flash_cfg->ioMode & 0x0f) == SF_CTRL_QIO_MODE) {
        SFlash_Qspi_Enable(p_flash_cfg);
    }

    return SUCCESS;
}

/**
 * @brief flash_set_l1c_wrap
 *
 * @return BL_Err_Type
 */
static BL_Err_Type ATTR_TCM_SECTION flash_set_l1c_wrap(SPI_Flash_Cfg_Type *p_flash_cfg)
{
    if (((p_flash_cfg->ioMode >> 4) & 0x01) == 1) {
        L1C_Set_Wrap(DISABLE);
    } else {
        L1C_Set_Wrap(ENABLE);
        if ((p_flash_cfg->ioMode & 0x0f) == SF_CTRL_QO_MODE || (p_flash_cfg->ioMode & 0x0f) == SF_CTRL_QIO_MODE) {
            SFlash_SetBurstWrap(p_flash_cfg);
        }
    }

    return SUCCESS;
}

/**
 * @brief flash_config_init
 *
 * @return BL_Err_Type
 */
static BL_Err_Type ATTR_TCM_SECTION flash_config_init(SPI_Flash_Cfg_Type *p_flash_cfg, uint8_t *jedec_id)
{
    BL_Err_Type ret = ERROR;
    uint32_t jid = 0;
    uint32_t offset = 0;

    cpu_global_irq_disable();
    XIP_SFlash_Opt_Enter();
    XIP_SFlash_State_Save(p_flash_cfg, &offset);
    SFlash_GetJedecId(p_flash_cfg, (uint8_t *)&jid);
    arch_memcpy(jedec_id, (uint8_t *)&jid, 3);
    jid &= 0xFFFFFF;
    g_jedec_id = jid;
    ret = SF_Cfg_Get_Flash_Cfg_Need_Lock_Ext(jid, p_flash_cfg);
    if (ret == SUCCESS) {
        p_flash_cfg->mid = (jid & 0xff);
    }

    /* Set flash controler from p_flash_cfg */
    flash_set_qspi_enable(p_flash_cfg);
    flash_set_l1c_wrap(p_flash_cfg);
    XIP_SFlash_State_Restore(p_flash_cfg, p_flash_cfg->ioMode & 0x0f, offset);
    XIP_SFlash_Opt_Exit();
    cpu_global_irq_enable();

    return ret;
}

/**
 * @brief multi flash adapter
 *
 * @return BL_Err_Type
 */
BL_Err_Type ATTR_TCM_SECTION flash_init(void)
{
    BL_Err_Type ret = ERROR;
    uint8_t clkDelay = 1;
    uint8_t clkInvert = 1;
    uint32_t jedec_id = 0;

    cpu_global_irq_disable();
    L1C_Cache_Flush_Ext();
    SF_Cfg_Get_Flash_Cfg_Need_Lock_Ext(0, &g_flash_cfg);
    L1C_Cache_Flush_Ext();
    cpu_global_irq_enable();
    if (g_flash_cfg.mid != 0xff) {
        return SUCCESS;
    }
    clkDelay = g_flash_cfg.clkDelay;
    clkInvert = g_flash_cfg.clkInvert;
    g_flash_cfg.ioMode = g_flash_cfg.ioMode & 0x0f;

    ret = flash_config_init(&g_flash_cfg, (uint8_t *)&jedec_id);
#if 0
    MSG("flash ID = %08x\r\n", jedec_id);
    bflb_platform_dump((uint8_t *)&g_flash_cfg, sizeof(g_flash_cfg));
    if (ret != SUCCESS) {
        MSG("flash config init fail!\r\n");
    }
#endif
    g_flash_cfg.clkDelay = clkDelay;
    g_flash_cfg.clkInvert = clkInvert;

    return ret;
}

/**
 * @brief read jedec id
 *
 * @param data
 * @return BL_Err_Type
 */
BL_Err_Type ATTR_TCM_SECTION flash_read_jedec_id(uint8_t *data)
{
    uint32_t jid = 0;

    cpu_global_irq_disable();
    XIP_SFlash_Opt_Enter();
    XIP_SFlash_GetJedecId_Need_Lock(&g_flash_cfg, g_flash_cfg.ioMode & 0x0f, (uint8_t *)&jid);
    XIP_SFlash_Opt_Exit();
    cpu_global_irq_enable();
    jid &= 0xFFFFFF;
    arch_memcpy(data, (void *)&jid, 4);

    return SUCCESS;
}

/**
 * @brief read flash data via xip
 *
 * @param addr
 * @param data
 * @param len
 * @return BL_Err_Type
 */
BL_Err_Type ATTR_TCM_SECTION flash_read_via_xip(uint32_t addr, uint8_t *data, uint32_t len)
{
    cpu_global_irq_disable();
    L1C_Cache_Flush_Ext();
    XIP_SFlash_Read_Via_Cache_Need_Lock(addr, data, len);
    L1C_Cache_Flush_Ext();
    cpu_global_irq_enable();

    return SUCCESS;
}

/**
 * @brief flash read data
 *
 * @param addr
 * @param data
 * @param len
 * @return BL_Err_Type
 */
BL_Err_Type ATTR_TCM_SECTION flash_read(uint32_t addr, uint8_t *data, uint32_t len)
{
    BL_Err_Type ret = ERROR;

    cpu_global_irq_disable();
    XIP_SFlash_Opt_Enter();
    ret = XIP_SFlash_Read_Need_Lock(&g_flash_cfg, g_flash_cfg.ioMode & 0x0f, addr, data, len);
    XIP_SFlash_Opt_Exit();
    cpu_global_irq_enable();

    return ret;
}

/**
 * @brief flash write data
 *
 * @param addr
 * @param data
 * @param len
 * @return BL_Err_Type
 */
BL_Err_Type ATTR_TCM_SECTION flash_write(uint32_t addr, uint8_t *data, uint32_t len)
{
    BL_Err_Type ret = ERROR;

    cpu_global_irq_disable();
    XIP_SFlash_Opt_Enter();
    ret = XIP_SFlash_Write_Need_Lock(&g_flash_cfg, g_flash_cfg.ioMode & 0x0f, addr, data, len);
    XIP_SFlash_Opt_Exit();
    cpu_global_irq_enable();

    return ret;
}

/**
 * @brief flash erase
 *
 * @param startaddr
 * @param endaddr
 * @return BL_Err_Type
 */
BL_Err_Type ATTR_TCM_SECTION flash_erase(uint32_t startaddr, uint32_t len)
{
    BL_Err_Type ret = ERROR;

    cpu_global_irq_disable();
    XIP_SFlash_Opt_Enter();
    ret = XIP_SFlash_Erase_Need_Lock(&g_flash_cfg, g_flash_cfg.ioMode & 0x0f, startaddr, startaddr + len - 1);
    XIP_SFlash_Opt_Exit();
    cpu_global_irq_enable();

    return ret;
}

/**
 * @brief flash write protect set
 *
 * @param protect
 * @return BL_Err_Type
 */
BL_Err_Type ATTR_TCM_SECTION flash_write_protect_set(SFlash_Protect_Kh25v40_Type protect)
{
    BL_Err_Type ret = ERROR;

    cpu_global_irq_disable();
    XIP_SFlash_Opt_Enter();
    ret = XIP_SFlash_KH25V40_Write_Protect_Need_Lock(&g_flash_cfg, protect);
    XIP_SFlash_Opt_Exit();
    cpu_global_irq_enable();

    return ret;
}

/**
 * @brief flash clear status register
 *
 * @param None
 * @return BL_Err_Type
 */
BL_Err_Type ATTR_TCM_SECTION flash_clear_status_register(void)
{
    BL_Err_Type ret = ERROR;

    cpu_global_irq_disable();
    XIP_SFlash_Opt_Enter();
    ret = XIP_SFlash_Clear_Status_Register_Need_Lock(&g_flash_cfg);
    XIP_SFlash_Opt_Exit();
    cpu_global_irq_enable();

    return ret;
}

/**
 * @brief set flash cache
 *
 * @param cont_read
 * @param cache_enable
 * @param cache_way_disable
 * @param flash_offset
 * @return BL_Err_Type
 */
BL_Err_Type ATTR_TCM_SECTION flash_set_cache(uint8_t cont_read, uint8_t cache_enable, uint8_t cache_way_disable, uint32_t flash_offset)
{
    uint32_t tmp[1];
    BL_Err_Type stat;

    SF_Ctrl_Set_Owner(SF_CTRL_OWNER_SAHB);

    XIP_SFlash_Opt_Enter();
    /* To make it simple, exit cont read anyway */
    SFlash_Reset_Continue_Read(&g_flash_cfg);

    if (g_flash_cfg.cReadSupport == 0) {
        cont_read = 0;
    }

    if (cont_read == 1) {
        stat = SFlash_Read(&g_flash_cfg, g_flash_cfg.ioMode & 0xf, 1, 0x00000000, (uint8_t *)tmp, sizeof(tmp));

        if (SUCCESS != stat) {
            XIP_SFlash_Opt_Exit();
            return ERROR;
        }
    }

    /* Set default value */
    L1C_Cache_Enable_Set(0xf);

    if (cache_enable) {
        SF_Ctrl_Set_Flash_Image_Offset(flash_offset);
        SFlash_Cache_Read_Enable(&g_flash_cfg, g_flash_cfg.ioMode & 0xf, cont_read, cache_way_disable);
    }
    XIP_SFlash_Opt_Exit();

    return SUCCESS;
}
