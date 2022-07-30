#include "bl702_romdriver.h"

/******************************************************************************/
__ALWAYS_INLINE ATTR_CLOCK_SECTION
    BL_Err_Type
    AON_Power_On_MBG(void)
{
    return RomDriver_AON_Power_On_MBG();
}

__ALWAYS_INLINE ATTR_CLOCK_SECTION
    BL_Err_Type
    AON_Power_Off_MBG(void)
{
    return RomDriver_AON_Power_Off_MBG();
}

__ALWAYS_INLINE ATTR_CLOCK_SECTION
    BL_Err_Type
    AON_Power_On_XTAL(void)
{
    return RomDriver_AON_Power_On_XTAL();
}

__ALWAYS_INLINE ATTR_CLOCK_SECTION
    BL_Err_Type
    AON_Set_Xtal_CapCode(uint8_t capIn, uint8_t capOut)
{
    return RomDriver_AON_Set_Xtal_CapCode(capIn, capOut);
}

__ALWAYS_INLINE ATTR_CLOCK_SECTION
    BL_Err_Type
    AON_Power_Off_XTAL(void)
{
    return RomDriver_AON_Power_Off_XTAL();
}
/******************************************************************************/

/******************************************************************************/
__ALWAYS_INLINE ATTR_TCM_SECTION void ASM_Delay_Us(uint32_t core, uint32_t cnt)
{
    RomDriver_ASM_Delay_Us(core, cnt);
}

__ALWAYS_INLINE ATTR_TCM_SECTION void BL702_Delay_US(uint32_t cnt)
{
    RomDriver_BL702_Delay_US(cnt);
}

__ALWAYS_INLINE ATTR_TCM_SECTION void BL702_Delay_MS(uint32_t cnt)
{
    RomDriver_BL702_Delay_MS(cnt);
}

__ALWAYS_INLINE ATTR_TCM_SECTION void *BL702_MemCpy(void *dst, const void *src, uint32_t n)
{
    return RomDriver_BL702_MemCpy(dst, src, n);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
    uint32_t *
    BL702_MemCpy4(uint32_t *dst, const uint32_t *src, uint32_t n)
{
    return RomDriver_BL702_MemCpy4(dst, src, n);
}

// __ALWAYS_INLINE ATTR_TCM_SECTION
// void* BL702_MemCpy_Fast(void *pdst, const void *psrc, uint32_t n) {
//     return RomDriver_BL702_MemCpy_Fast(pdst, psrc, n);
// }

__ALWAYS_INLINE ATTR_TCM_SECTION void *ARCH_MemCpy_Fast(void *pdst, const void *psrc, uint32_t n)
{
    return RomDriver_ARCH_MemCpy_Fast(pdst, psrc, n);
}

__ALWAYS_INLINE ATTR_TCM_SECTION void *BL702_MemSet(void *s, uint8_t c, uint32_t n)
{
    return RomDriver_BL702_MemSet(s, c, n);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
    uint32_t *
    BL702_MemSet4(uint32_t *dst, const uint32_t val, uint32_t n)
{
    return RomDriver_BL702_MemSet4(dst, val, n);
}

__ALWAYS_INLINE ATTR_TCM_SECTION int BL702_MemCmp(const void *s1, const void *s2, uint32_t n)
{
    return RomDriver_BL702_MemCmp(s1, s2, n);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
    uint32_t
    BFLB_Soft_CRC32(void *dataIn, uint32_t len)
{
    return RomDriver_BFLB_Soft_CRC32(dataIn, len);
}
/******************************************************************************/

/******************************************************************************/
__ALWAYS_INLINE ATTR_CLOCK_SECTION
    GLB_ROOT_CLK_Type
    GLB_Get_Root_CLK_Sel(void)
{
    return RomDriver_GLB_Get_Root_CLK_Sel();
}
#if 0
__ALWAYS_INLINE ATTR_CLOCK_SECTION
    BL_Err_Type
    GLB_Set_System_CLK_Div(uint8_t hclkDiv, uint8_t bclkDiv)
{
    return RomDriver_GLB_Set_System_CLK_Div(hclkDiv, bclkDiv);
}
#endif
__ALWAYS_INLINE ATTR_CLOCK_SECTION
    uint8_t
    GLB_Get_BCLK_Div(void)
{
    return RomDriver_GLB_Get_BCLK_Div();
}

__ALWAYS_INLINE ATTR_CLOCK_SECTION
    uint8_t
    GLB_Get_HCLK_Div(void)
{
    return RomDriver_GLB_Get_HCLK_Div();
}

__ALWAYS_INLINE ATTR_CLOCK_SECTION
    BL_Err_Type
    Update_SystemCoreClockWith_XTAL(GLB_DLL_XTAL_Type xtalType)
{
    return RomDriver_Update_SystemCoreClockWith_XTAL(xtalType);
}

__ALWAYS_INLINE ATTR_CLOCK_SECTION
    BL_Err_Type
    GLB_Set_System_CLK(GLB_DLL_XTAL_Type xtalType, GLB_SYS_CLK_Type clkFreq)
{
    return RomDriver_GLB_Set_System_CLK(xtalType, clkFreq);
}

__ALWAYS_INLINE ATTR_CLOCK_SECTION
    BL_Err_Type
    System_Core_Clock_Update_From_RC32M(void)
{
    return RomDriver_System_Core_Clock_Update_From_RC32M();
}

__ALWAYS_INLINE ATTR_CLOCK_SECTION
    BL_Err_Type
    GLB_Set_SF_CLK(uint8_t enable, GLB_SFLASH_CLK_Type clkSel, uint8_t div)
{
    return RomDriver_GLB_Set_SF_CLK(enable, clkSel, div);
}

__ALWAYS_INLINE ATTR_CLOCK_SECTION
    BL_Err_Type
    GLB_Power_Off_DLL(void)
{
    return RomDriver_GLB_Power_Off_DLL();
}

__ALWAYS_INLINE ATTR_CLOCK_SECTION
    BL_Err_Type
    GLB_Power_On_DLL(GLB_DLL_XTAL_Type xtalType)
{
    return RomDriver_GLB_Power_On_DLL(xtalType);
}

__ALWAYS_INLINE ATTR_CLOCK_SECTION
    BL_Err_Type
    GLB_Enable_DLL_All_Clks(void)
{
    return RomDriver_GLB_Enable_DLL_All_Clks();
}

__ALWAYS_INLINE ATTR_CLOCK_SECTION
    BL_Err_Type
    GLB_Enable_DLL_Clk(GLB_DLL_CLK_Type dllClk)
{
    return RomDriver_GLB_Enable_DLL_Clk(dllClk);
}

__ALWAYS_INLINE ATTR_CLOCK_SECTION
    BL_Err_Type
    GLB_Disable_DLL_All_Clks(void)
{
    return RomDriver_GLB_Disable_DLL_All_Clks();
}

__ALWAYS_INLINE ATTR_CLOCK_SECTION
    BL_Err_Type
    GLB_Disable_DLL_Clk(GLB_DLL_CLK_Type dllClk)
{
    return RomDriver_GLB_Disable_DLL_Clk(dllClk);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
    BL_Err_Type
    GLB_SW_System_Reset(void)
{
    return RomDriver_GLB_SW_System_Reset();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
    BL_Err_Type
    GLB_SW_CPU_Reset(void)
{
    return RomDriver_GLB_SW_CPU_Reset();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
    BL_Err_Type
    GLB_SW_POR_Reset(void)
{
    return RomDriver_GLB_SW_POR_Reset();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
    BL_Err_Type
    GLB_Select_Internal_Flash(void)
{
    return RomDriver_GLB_Select_Internal_Flash();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
    BL_Err_Type
    GLB_Swap_Flash_Pin(void)
{
    return RomDriver_GLB_Swap_Flash_Pin();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
    BL_Err_Type
    GLB_Swap_Flash_CS_IO2_Pin(void)
{
    return RomDriver_GLB_Swap_Flash_CS_IO2_Pin();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
    BL_Err_Type
    GLB_Swap_Flash_IO0_IO3_Pin(void)
{
    return RomDriver_GLB_Swap_Flash_IO0_IO3_Pin();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
    BL_Err_Type
    GLB_Select_Internal_PSram(void)
{
    return RomDriver_GLB_Select_Internal_PSram();
}

/* aon pads GPIO9~GPIO13 IE controlled by HBN reg_aon_pad_ie_smt, abandon romdriver for this reason */
#if 0
__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_GPIO_Init(GLB_GPIO_Cfg_Type *cfg)
{
    return RomDriver_GLB_GPIO_Init(cfg);
}
#endif

#if 0
__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_GPIO_OUTPUT_Enable(GLB_GPIO_Type gpioPin)
{
    return RomDriver_GLB_GPIO_OUTPUT_Enable(gpioPin);
}
#endif

#if 0
__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_GPIO_OUTPUT_Disable(GLB_GPIO_Type gpioPin)
{
    return RomDriver_GLB_GPIO_OUTPUT_Disable(gpioPin);
}
#endif

/* aon pads GPIO9~GPIO13 IE controlled by HBN reg_aon_pad_ie_smt, abandon romdriver for this reason */
#if 0
__ALWAYS_INLINE ATTR_TCM_SECTION
BL_Err_Type GLB_GPIO_Set_HZ(GLB_GPIO_Type gpioPin)
{
    return RomDriver_GLB_GPIO_Set_HZ(gpioPin);
}
#endif

__ALWAYS_INLINE ATTR_TCM_SECTION
    BL_Err_Type
    GLB_Deswap_Flash_Pin(void)
{
    return RomDriver_GLB_Deswap_Flash_Pin();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
    BL_Err_Type
    GLB_Select_External_Flash(void)
{
    return RomDriver_GLB_Select_External_Flash();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
    uint8_t
    GLB_GPIO_Get_Fun(GLB_GPIO_Type gpioPin)
{
    return RomDriver_GLB_GPIO_Get_Fun(gpioPin);
}
/******************************************************************************/

/******************************************************************************/
__ALWAYS_INLINE ATTR_TCM_SECTION
    BL_Sts_Type
    EF_Ctrl_Busy(void)
{
    return RomDriver_EF_Ctrl_Busy();
}

__ALWAYS_INLINE ATTR_TCM_SECTION void EF_Ctrl_Sw_AHB_Clk_0(void)
{
    RomDriver_EF_Ctrl_Sw_AHB_Clk_0();
}

__ALWAYS_INLINE ATTR_TCM_SECTION void EF_Ctrl_Load_Efuse_R0(void)
{
    RomDriver_EF_Ctrl_Load_Efuse_R0();
}

__ALWAYS_INLINE ATTR_TCM_SECTION void EF_Ctrl_Clear(uint32_t index, uint32_t len)
{
    RomDriver_EF_Ctrl_Clear(index, len);
}

__ALWAYS_INLINE ATTR_CLOCK_SECTION
    uint8_t
    EF_Ctrl_Get_Trim_Parity(uint32_t val, uint8_t len)
{
    return RomDriver_EF_Ctrl_Get_Trim_Parity(val, len);
}

__ALWAYS_INLINE ATTR_CLOCK_SECTION void EF_Ctrl_Read_RC32K_Trim(Efuse_Ana_RC32K_Trim_Type *trim)
{
    RomDriver_EF_Ctrl_Read_RC32K_Trim(trim);
}

__ALWAYS_INLINE ATTR_CLOCK_SECTION void EF_Ctrl_Read_RC32M_Trim(Efuse_Ana_RC32M_Trim_Type *trim)
{
    RomDriver_EF_Ctrl_Read_RC32M_Trim(trim);
}
/******************************************************************************/

/******************************************************************************/
__ALWAYS_INLINE ATTR_CLOCK_SECTION
    BL_Err_Type
    PDS_Trim_RC32M(void)
{
    return RomDriver_PDS_Trim_RC32M();
}

__ALWAYS_INLINE ATTR_CLOCK_SECTION
    BL_Err_Type
    PDS_Select_RC32M_As_PLL_Ref(void)
{
    return RomDriver_PDS_Select_RC32M_As_PLL_Ref();
}

__ALWAYS_INLINE ATTR_CLOCK_SECTION
    BL_Err_Type
    PDS_Select_XTAL_As_PLL_Ref(void)
{
    return RomDriver_PDS_Select_XTAL_As_PLL_Ref();
}

__ALWAYS_INLINE ATTR_CLOCK_SECTION
    BL_Err_Type
    PDS_Power_On_PLL(PDS_PLL_XTAL_Type xtalType)
{
    return RomDriver_PDS_Power_On_PLL(xtalType);
}

__ALWAYS_INLINE ATTR_CLOCK_SECTION
    BL_Err_Type
    PDS_Enable_PLL_All_Clks(void)
{
    return RomDriver_PDS_Enable_PLL_All_Clks();
}

__ALWAYS_INLINE ATTR_CLOCK_SECTION
    BL_Err_Type
    PDS_Disable_PLL_All_Clks(void)
{
    return RomDriver_PDS_Disable_PLL_All_Clks();
}

__ALWAYS_INLINE ATTR_CLOCK_SECTION
    BL_Err_Type
    PDS_Enable_PLL_Clk(PDS_PLL_CLK_Type pllClk)
{
    return RomDriver_PDS_Enable_PLL_Clk(pllClk);
}

__ALWAYS_INLINE ATTR_CLOCK_SECTION
    BL_Err_Type
    PDS_Disable_PLL_Clk(PDS_PLL_CLK_Type pllClk)
{
    return RomDriver_PDS_Disable_PLL_Clk(pllClk);
}

__ALWAYS_INLINE ATTR_CLOCK_SECTION
    BL_Err_Type
    PDS_Power_Off_PLL(void)
{
    return RomDriver_PDS_Power_Off_PLL();
}

__ALWAYS_INLINE ATTR_TCM_SECTION void PDS_Reset(void)
{
    RomDriver_PDS_Reset();
}

__ALWAYS_INLINE ATTR_TCM_SECTION void PDS_Enable(PDS_CFG_Type *cfg, uint32_t pdsSleepCnt)
{
    RomDriver_PDS_Enable(cfg, pdsSleepCnt);
}

__ALWAYS_INLINE ATTR_TCM_SECTION void PDS_Auto_Time_Config(uint32_t sleepDuration)
{
    RomDriver_PDS_Auto_Time_Config(sleepDuration);
}

__ALWAYS_INLINE ATTR_TCM_SECTION void PDS_Auto_Enable(PDS_AUTO_POWER_DOWN_CFG_Type *powerCfg, PDS_AUTO_NORMAL_CFG_Type *normalCfg, BL_Fun_Type enable)
{
    RomDriver_PDS_Auto_Enable(powerCfg, normalCfg, enable);
}

__ALWAYS_INLINE ATTR_TCM_SECTION void PDS_Manual_Force_Turn_Off(PDS_FORCE_Type domain)
{
    RomDriver_PDS_Manual_Force_Turn_Off(domain);
}

__ALWAYS_INLINE ATTR_TCM_SECTION void PDS_Manual_Force_Turn_On(PDS_FORCE_Type domain)
{
    RomDriver_PDS_Manual_Force_Turn_On(domain);
}
/******************************************************************************/

/******************************************************************************/
#if 0
__ALWAYS_INLINE ATTR_TCM_SECTION
void HBN_Enable(uint8_t aGPIOIeCfg, HBN_LDO_LEVEL_Type ldoLevel, HBN_LEVEL_Type hbnLevel)
{
    RomDriver_HBN_Enable(aGPIOIeCfg, ldoLevel, hbnLevel);
}
#endif

__ALWAYS_INLINE ATTR_TCM_SECTION
    BL_Err_Type
    HBN_Reset(void)
{
    return RomDriver_HBN_Reset();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
    BL_Err_Type
    HBN_GPIO_Dbg_Pull_Cfg(BL_Fun_Type pupdEn, BL_Fun_Type dlyEn, uint8_t dlySec, HBN_INT_Type gpioIrq, BL_Mask_Type gpioMask)
{
    return RomDriver_HBN_GPIO_Dbg_Pull_Cfg(pupdEn, dlyEn, dlySec, gpioIrq, gpioMask);
}

__ALWAYS_INLINE ATTR_CLOCK_SECTION
    BL_Err_Type
    HBN_Trim_RC32K(void)
{
    return RomDriver_HBN_Trim_RC32K();
}

__ALWAYS_INLINE ATTR_CLOCK_SECTION
    BL_Err_Type
    HBN_Set_ROOT_CLK_Sel(HBN_ROOT_CLK_Type rootClk)
{
    return RomDriver_HBN_Set_ROOT_CLK_Sel(rootClk);
}
/******************************************************************************/

/******************************************************************************/
__ALWAYS_INLINE ATTR_TCM_SECTION
    BL_Err_Type
    XIP_SFlash_State_Save(SPI_Flash_Cfg_Type *pFlashCfg, uint32_t *offset)
{
    return RomDriver_XIP_SFlash_State_Save(pFlashCfg, offset);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
    BL_Err_Type
    XIP_SFlash_State_Restore(SPI_Flash_Cfg_Type *pFlashCfg, SF_Ctrl_IO_Type ioMode, uint32_t offset)
{
    return RomDriver_XIP_SFlash_State_Restore(pFlashCfg, ioMode, offset);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
    BL_Err_Type
    XIP_SFlash_Erase_Need_Lock(SPI_Flash_Cfg_Type *pFlashCfg, SF_Ctrl_IO_Type ioMode, uint32_t startaddr, uint32_t endaddr)
{
    return RomDriver_XIP_SFlash_Erase_Need_Lock(pFlashCfg, ioMode, startaddr, endaddr);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
    BL_Err_Type
    XIP_SFlash_Write_Need_Lock(SPI_Flash_Cfg_Type *pFlashCfg, SF_Ctrl_IO_Type ioMode, uint32_t addr, uint8_t *data, uint32_t len)
{
    return RomDriver_XIP_SFlash_Write_Need_Lock(pFlashCfg, ioMode, addr, data, len);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
    BL_Err_Type
    XIP_SFlash_Read_Need_Lock(SPI_Flash_Cfg_Type *pFlashCfg, SF_Ctrl_IO_Type ioMode, uint32_t addr, uint8_t *data, uint32_t len)
{
    return RomDriver_XIP_SFlash_Read_Need_Lock(pFlashCfg, ioMode, addr, data, len);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
    BL_Err_Type
    XIP_SFlash_GetJedecId_Need_Lock(SPI_Flash_Cfg_Type *pFlashCfg, SF_Ctrl_IO_Type ioMode, uint8_t *data)
{
    return RomDriver_XIP_SFlash_GetJedecId_Need_Lock(pFlashCfg, ioMode, data);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
    BL_Err_Type
    XIP_SFlash_GetDeviceId_Need_Lock(SPI_Flash_Cfg_Type *pFlashCfg, SF_Ctrl_IO_Type ioMode, uint8_t *data)
{
    return RomDriver_XIP_SFlash_GetDeviceId_Need_Lock(pFlashCfg, ioMode, data);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
    BL_Err_Type
    XIP_SFlash_GetUniqueId_Need_Lock(SPI_Flash_Cfg_Type *pFlashCfg, SF_Ctrl_IO_Type ioMode, uint8_t *data, uint8_t idLen)
{
    return RomDriver_XIP_SFlash_GetUniqueId_Need_Lock(pFlashCfg, ioMode, data, idLen);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
    BL_Err_Type
    XIP_SFlash_Read_Via_Cache_Need_Lock(uint32_t addr, uint8_t *data, uint32_t len)
{
    return RomDriver_XIP_SFlash_Read_Via_Cache_Need_Lock(addr, data, len);
}

__ALWAYS_INLINE ATTR_TCM_SECTION int XIP_SFlash_Read_With_Lock(SPI_Flash_Cfg_Type *pFlashCfg, SF_Ctrl_IO_Type ioMode, uint32_t addr, uint8_t *dst, int len)
{
    return RomDriver_XIP_SFlash_Read_With_Lock(pFlashCfg, ioMode, addr, dst, len);
}

__ALWAYS_INLINE ATTR_TCM_SECTION int XIP_SFlash_Write_With_Lock(SPI_Flash_Cfg_Type *pFlashCfg, SF_Ctrl_IO_Type ioMode, uint32_t addr, uint8_t *src, int len)
{
    return RomDriver_XIP_SFlash_Write_With_Lock(pFlashCfg, ioMode, addr, src, len);
}

__ALWAYS_INLINE ATTR_TCM_SECTION int XIP_SFlash_Erase_With_Lock(SPI_Flash_Cfg_Type *pFlashCfg, SF_Ctrl_IO_Type ioMode, uint32_t addr, int len)
{
    return RomDriver_XIP_SFlash_Erase_With_Lock(pFlashCfg, ioMode, addr, len);
}
/******************************************************************************/

/******************************************************************************/
__ALWAYS_INLINE ATTR_TCM_SECTION void SFlash_Init(const SF_Ctrl_Cfg_Type *pSfCtrlCfg)
{
    RomDriver_SFlash_Init(pSfCtrlCfg);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
    BL_Err_Type
    SFlash_SetSPIMode(SF_Ctrl_Mode_Type mode)
{
    return RomDriver_SFlash_SetSPIMode(mode);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
    BL_Err_Type
    SFlash_Read_Reg(SPI_Flash_Cfg_Type *flashCfg, uint8_t regIndex, uint8_t *regValue, uint8_t regLen)
{
    return RomDriver_SFlash_Read_Reg(flashCfg, regIndex, regValue, regLen);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
    BL_Err_Type
    SFlash_Write_Reg(SPI_Flash_Cfg_Type *flashCfg, uint8_t regIndex, uint8_t *regValue, uint8_t regLen)
{
    return RomDriver_SFlash_Write_Reg(flashCfg, regIndex, regValue, regLen);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
    BL_Err_Type
    SFlash_Read_Reg_With_Cmd(SPI_Flash_Cfg_Type *flashCfg, uint8_t readRegCmd, uint8_t *regValue, uint8_t regLen)
{
    return RomDriver_SFlash_Read_Reg_With_Cmd(flashCfg, readRegCmd, regValue, regLen);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
    BL_Err_Type
    SFlash_Write_Reg_With_Cmd(SPI_Flash_Cfg_Type *flashCfg, uint8_t writeRegCmd, uint8_t *regValue, uint8_t regLen)
{
    return RomDriver_SFlash_Write_Reg_With_Cmd(flashCfg, writeRegCmd, regValue, regLen);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
    BL_Sts_Type
    SFlash_Busy(SPI_Flash_Cfg_Type *flashCfg)
{
    return RomDriver_SFlash_Busy(flashCfg);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
    BL_Err_Type
    SFlash_Write_Enable(SPI_Flash_Cfg_Type *flashCfg)
{
    return RomDriver_SFlash_Write_Enable(flashCfg);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
    BL_Err_Type
    SFlash_Qspi_Enable(SPI_Flash_Cfg_Type *flashCfg)
{
    return RomDriver_SFlash_Qspi_Enable(flashCfg);
}

__ALWAYS_INLINE ATTR_TCM_SECTION void SFlash_Volatile_Reg_Write_Enable(SPI_Flash_Cfg_Type *flashCfg)
{
    RomDriver_SFlash_Volatile_Reg_Write_Enable(flashCfg);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
    BL_Err_Type
    SFlash_Chip_Erase(SPI_Flash_Cfg_Type *flashCfg)
{
    return RomDriver_SFlash_Chip_Erase(flashCfg);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
    BL_Err_Type
    SFlash_Sector_Erase(SPI_Flash_Cfg_Type *flashCfg, uint32_t secNum)
{
    return RomDriver_SFlash_Sector_Erase(flashCfg, secNum);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
    BL_Err_Type
    SFlash_Blk32_Erase(SPI_Flash_Cfg_Type *flashCfg, uint32_t blkNum)
{
    return RomDriver_SFlash_Blk32_Erase(flashCfg, blkNum);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
    BL_Err_Type
    SFlash_Blk64_Erase(SPI_Flash_Cfg_Type *flashCfg, uint32_t blkNum)
{
    return RomDriver_SFlash_Blk64_Erase(flashCfg, blkNum);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
    BL_Err_Type
    SFlash_Erase(SPI_Flash_Cfg_Type *flashCfg, uint32_t startaddr, uint32_t endaddr)
{
    return RomDriver_SFlash_Erase(flashCfg, startaddr, endaddr);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
    BL_Err_Type
    SFlash_Program(SPI_Flash_Cfg_Type *flashCfg, SF_Ctrl_IO_Type ioMode, uint32_t addr, uint8_t *data, uint32_t len)
{
    return RomDriver_SFlash_Program(flashCfg, ioMode, addr, data, len);
}

__ALWAYS_INLINE ATTR_TCM_SECTION void SFlash_GetUniqueId(uint8_t *data, uint8_t idLen)
{
    RomDriver_SFlash_GetUniqueId(data, idLen);
}

__ALWAYS_INLINE ATTR_TCM_SECTION void SFlash_GetJedecId(SPI_Flash_Cfg_Type *flashCfg, uint8_t *data)
{
    RomDriver_SFlash_GetJedecId(flashCfg, data);
}

__ALWAYS_INLINE ATTR_TCM_SECTION void SFlash_GetDeviceId(uint8_t *data)
{
    RomDriver_SFlash_GetDeviceId(data);
}

__ALWAYS_INLINE ATTR_TCM_SECTION void SFlash_Powerdown(void)
{
    RomDriver_SFlash_Powerdown();
}

__ALWAYS_INLINE ATTR_TCM_SECTION void SFlash_Releae_Powerdown(SPI_Flash_Cfg_Type *flashCfg)
{
    RomDriver_SFlash_Releae_Powerdown(flashCfg);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
    BL_Err_Type
    SFlash_Restore_From_Powerdown(SPI_Flash_Cfg_Type *pFlashCfg, uint8_t flashContRead)
{
    return RomDriver_SFlash_Restore_From_Powerdown(pFlashCfg, flashContRead);
}

__ALWAYS_INLINE ATTR_TCM_SECTION void SFlash_SetBurstWrap(SPI_Flash_Cfg_Type *flashCfg)
{
    RomDriver_SFlash_SetBurstWrap(flashCfg);
}

__ALWAYS_INLINE ATTR_TCM_SECTION void SFlash_DisableBurstWrap(SPI_Flash_Cfg_Type *flashCfg)
{
    RomDriver_SFlash_DisableBurstWrap(flashCfg);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
    BL_Err_Type
    SFlash_Software_Reset(SPI_Flash_Cfg_Type *flashCfg)
{
    return RomDriver_SFlash_Software_Reset(flashCfg);
}

__ALWAYS_INLINE ATTR_TCM_SECTION void SFlash_Reset_Continue_Read(SPI_Flash_Cfg_Type *flashCfg)
{
    return RomDriver_SFlash_Reset_Continue_Read(flashCfg);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
    BL_Err_Type
    SFlash_Set_IDbus_Cfg(SPI_Flash_Cfg_Type *flashCfg,
                         SF_Ctrl_IO_Type ioMode, uint8_t contRead, uint32_t addr, uint32_t len)
{
    return RomDriver_SFlash_Set_IDbus_Cfg(flashCfg, ioMode, contRead, addr, len);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
    BL_Err_Type
    SFlash_IDbus_Read_Enable(SPI_Flash_Cfg_Type *flashCfg, SF_Ctrl_IO_Type ioMode, uint8_t contRead)
{
    return RomDriver_SFlash_IDbus_Read_Enable(flashCfg, ioMode, contRead);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
    BL_Err_Type
    SFlash_Cache_Read_Enable(SPI_Flash_Cfg_Type *flashCfg,
                             SF_Ctrl_IO_Type ioMode, uint8_t contRead, uint8_t wayDisable)
{
    return RomDriver_SFlash_Cache_Read_Enable(flashCfg, ioMode, contRead, wayDisable);
}

__ALWAYS_INLINE ATTR_TCM_SECTION void SFlash_Cache_Read_Disable(void)
{
    RomDriver_SFlash_Cache_Read_Disable();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
    BL_Err_Type
    SFlash_Read(SPI_Flash_Cfg_Type *flashCfg,
                SF_Ctrl_IO_Type ioMode, uint8_t contRead, uint32_t addr, uint8_t *data, uint32_t len)
{
    return RomDriver_SFlash_Read(flashCfg, ioMode, contRead, addr, data, len);
}
/******************************************************************************/

/******************************************************************************/
__ALWAYS_INLINE ATTR_TCM_SECTION
    BL_Err_Type
    L1C_Cache_Enable_Set(uint8_t wayDisable)
{
    return RomDriver_L1C_Cache_Enable_Set(wayDisable);
}

__ALWAYS_INLINE ATTR_TCM_SECTION void L1C_Cache_Write_Set(BL_Fun_Type wtEn, BL_Fun_Type wbEn, BL_Fun_Type waEn)
{
    RomDriver_L1C_Cache_Write_Set(wtEn, wbEn, waEn);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
    BL_Err_Type
    L1C_Cache_Flush(uint8_t wayDisable)
{
    return RomDriver_L1C_Cache_Flush(wayDisable);
}

__ALWAYS_INLINE ATTR_TCM_SECTION void L1C_Cache_Hit_Count_Get(uint32_t *hitCountLow, uint32_t *hitCountHigh)
{
    RomDriver_L1C_Cache_Hit_Count_Get(hitCountLow, hitCountHigh);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
    uint32_t
    L1C_Cache_Miss_Count_Get(void)
{
    return RomDriver_L1C_Cache_Miss_Count_Get();
}

__ALWAYS_INLINE ATTR_TCM_SECTION void L1C_Cache_Read_Disable(void)
{
    RomDriver_L1C_Cache_Read_Disable();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
    BL_Err_Type
    L1C_Set_Wrap(BL_Fun_Type wrap)
{
    return RomDriver_L1C_Set_Wrap(wrap);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
    BL_Err_Type
    L1C_Set_Way_Disable(uint8_t disableVal)
{
    return RomDriver_L1C_Set_Way_Disable(disableVal);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
    BL_Err_Type
    L1C_IROM_2T_Access_Set(uint8_t enable)
{
    return RomDriver_L1C_IROM_2T_Access_Set(enable);
}
/******************************************************************************/

/******************************************************************************/
__ALWAYS_INLINE ATTR_TCM_SECTION void SF_Ctrl_Enable(const SF_Ctrl_Cfg_Type *cfg)
{
    RomDriver_SF_Ctrl_Enable(cfg);
}

#if 0
__ALWAYS_INLINE ATTR_TCM_SECTION
void SF_Ctrl_Psram_Init(SF_Ctrl_Psram_Cfg *sfCtrlPsramCfg)
{
    RomDriver_SF_Ctrl_Psram_Init(sfCtrlPsramCfg);
}
#endif

__ALWAYS_INLINE ATTR_TCM_SECTION
    uint8_t
    SF_Ctrl_Get_Clock_Delay(void)
{
    return RomDriver_SF_Ctrl_Get_Clock_Delay();
}

__ALWAYS_INLINE ATTR_TCM_SECTION void SF_Ctrl_Set_Clock_Delay(uint8_t delay)
{
    RomDriver_SF_Ctrl_Set_Clock_Delay(delay);
}

__ALWAYS_INLINE ATTR_TCM_SECTION void SF_Ctrl_Cmds_Set(SF_Ctrl_Cmds_Cfg *cmdsCfg)
{
    RomDriver_SF_Ctrl_Cmds_Set(cmdsCfg);
}

__ALWAYS_INLINE ATTR_TCM_SECTION void SF_Ctrl_Set_Owner(SF_Ctrl_Owner_Type owner)
{
    RomDriver_SF_Ctrl_Set_Owner(owner);
}

__ALWAYS_INLINE ATTR_TCM_SECTION void SF_Ctrl_Disable(void)
{
    RomDriver_SF_Ctrl_Disable();
}

__ALWAYS_INLINE ATTR_TCM_SECTION void SF_Ctrl_Select_Pad(SF_Ctrl_Pad_Select sel)
{
    RomDriver_SF_Ctrl_Select_Pad(sel);
}

__ALWAYS_INLINE ATTR_TCM_SECTION void SF_Ctrl_Select_Bank(SF_Ctrl_Select sel)
{
    RomDriver_SF_Ctrl_Select_Bank(sel);
}

__ALWAYS_INLINE ATTR_TCM_SECTION void SF_Ctrl_AES_Enable_BE(void)
{
    RomDriver_SF_Ctrl_AES_Enable_BE();
}

__ALWAYS_INLINE ATTR_TCM_SECTION void SF_Ctrl_AES_Enable_LE(void)
{
    RomDriver_SF_Ctrl_AES_Enable_LE();
}

__ALWAYS_INLINE ATTR_TCM_SECTION void SF_Ctrl_AES_Set_Region(uint8_t region, uint8_t enable,
                                                             uint8_t hwKey, uint32_t startAddr, uint32_t endAddr, uint8_t locked)
{
    RomDriver_SF_Ctrl_AES_Set_Region(region, enable, hwKey, startAddr, endAddr, locked);
}

__ALWAYS_INLINE ATTR_TCM_SECTION void SF_Ctrl_AES_Set_Key(uint8_t region, uint8_t *key, SF_Ctrl_AES_Key_Type keyType)
{
    RomDriver_SF_Ctrl_AES_Set_Key(region, key, keyType);
}

__ALWAYS_INLINE ATTR_TCM_SECTION void SF_Ctrl_AES_Set_Key_BE(uint8_t region, uint8_t *key, SF_Ctrl_AES_Key_Type keyType)
{
    RomDriver_SF_Ctrl_AES_Set_Key_BE(region, key, keyType);
}

__ALWAYS_INLINE ATTR_TCM_SECTION void SF_Ctrl_AES_Set_IV(uint8_t region, uint8_t *iv, uint32_t addrOffset)
{
    RomDriver_SF_Ctrl_AES_Set_IV(region, iv, addrOffset);
}

__ALWAYS_INLINE ATTR_TCM_SECTION void SF_Ctrl_AES_Set_IV_BE(uint8_t region, uint8_t *iv, uint32_t addrOffset)
{
    RomDriver_SF_Ctrl_AES_Set_IV_BE(region, iv, addrOffset);
}

__ALWAYS_INLINE ATTR_TCM_SECTION void SF_Ctrl_AES_Enable(void)
{
    RomDriver_SF_Ctrl_AES_Enable();
}

__ALWAYS_INLINE ATTR_TCM_SECTION void SF_Ctrl_AES_Disable(void)
{
    RomDriver_SF_Ctrl_AES_Disable();
}

__ALWAYS_INLINE ATTR_TCM_SECTION
    uint8_t
    SF_Ctrl_Is_AES_Enable(void)
{
    return RomDriver_SF_Ctrl_Is_AES_Enable();
}

__ALWAYS_INLINE ATTR_TCM_SECTION void SF_Ctrl_Set_Flash_Image_Offset(uint32_t addrOffset)
{
    RomDriver_SF_Ctrl_Set_Flash_Image_Offset(addrOffset);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
    uint32_t
    SF_Ctrl_Get_Flash_Image_Offset(void)
{
    return RomDriver_SF_Ctrl_Get_Flash_Image_Offset();
}

__ALWAYS_INLINE ATTR_TCM_SECTION void SF_Ctrl_Select_Clock(SF_Ctrl_Sahb_Type sahbType)
{
    RomDriver_SF_Ctrl_Select_Clock(sahbType);
}

__ALWAYS_INLINE ATTR_TCM_SECTION void SF_Ctrl_SendCmd(SF_Ctrl_Cmd_Cfg_Type *cfg)
{
    RomDriver_SF_Ctrl_SendCmd(cfg);
}

__ALWAYS_INLINE ATTR_TCM_SECTION void SF_Ctrl_Flash_Read_Icache_Set(SF_Ctrl_Cmd_Cfg_Type *cfg, uint8_t cmdValid)
{
    RomDriver_SF_Ctrl_Flash_Read_Icache_Set(cfg, cmdValid);
}

__ALWAYS_INLINE ATTR_TCM_SECTION void SF_Ctrl_Psram_Write_Icache_Set(SF_Ctrl_Cmd_Cfg_Type *cfg, uint8_t cmdValid)
{
    RomDriver_SF_Ctrl_Psram_Write_Icache_Set(cfg, cmdValid);
}

__ALWAYS_INLINE ATTR_TCM_SECTION void SF_Ctrl_Psram_Read_Icache_Set(SF_Ctrl_Cmd_Cfg_Type *cfg, uint8_t cmdValid)
{
    RomDriver_SF_Ctrl_Psram_Read_Icache_Set(cfg, cmdValid);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
    BL_Sts_Type
    SF_Ctrl_GetBusyState(void)
{
    return RomDriver_SF_Ctrl_GetBusyState();
}

__ALWAYS_INLINE ATTR_TCM_SECTION void SF_Cfg_Deinit_Ext_Flash_Gpio(uint8_t extFlashPin)
{
    RomDriver_SF_Cfg_Deinit_Ext_Flash_Gpio(extFlashPin);
}

__ALWAYS_INLINE ATTR_TCM_SECTION void SF_Cfg_Init_Ext_Flash_Gpio(uint8_t extFlashPin)
{
    RomDriver_SF_Cfg_Init_Ext_Flash_Gpio(extFlashPin);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
    BL_Err_Type
    SF_Cfg_Get_Flash_Cfg_Need_Lock(uint32_t flashID, SPI_Flash_Cfg_Type *pFlashCfg)
{
    return RomDriver_SF_Cfg_Get_Flash_Cfg_Need_Lock(flashID, pFlashCfg);
}

#if 0
__ALWAYS_INLINE ATTR_TCM_SECTION
void SF_Cfg_Init_Flash_Gpio(uint8_t flashPinCfg, uint8_t restoreDefault)
{
    RomDriver_SF_Cfg_Init_Flash_Gpio(flashPinCfg, restoreDefault);
}
#endif

__ALWAYS_INLINE ATTR_TCM_SECTION
    uint32_t
    SF_Cfg_Flash_Identify(uint8_t callFromFlash, uint32_t autoScan, uint32_t flashPinCfg, uint8_t restoreDefault,
                          SPI_Flash_Cfg_Type *pFlashCfg)
{
    return RomDriver_SF_Cfg_Flash_Identify(callFromFlash, autoScan, flashPinCfg, restoreDefault, pFlashCfg);
}
/******************************************************************************/

/******************************************************************************/
#if 0
__ALWAYS_INLINE ATTR_TCM_SECTION
void Psram_Init(SPI_Psram_Cfg_Type *psramCfg, SF_Ctrl_Cmds_Cfg *cmdsCfg, SF_Ctrl_Psram_Cfg *sfCtrlPsramCfg)
{
    RomDriver_Psram_Init(psramCfg, cmdsCfg, sfCtrlPsramCfg);
}
#endif

__ALWAYS_INLINE ATTR_TCM_SECTION void Psram_ReadReg(SPI_Psram_Cfg_Type *psramCfg, uint8_t *regValue)
{
    RomDriver_Psram_ReadReg(psramCfg, regValue);
}

__ALWAYS_INLINE ATTR_TCM_SECTION void Psram_WriteReg(SPI_Psram_Cfg_Type *psramCfg, uint8_t *regValue)
{
    RomDriver_Psram_WriteReg(psramCfg, regValue);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
    BL_Err_Type
    Psram_SetDriveStrength(SPI_Psram_Cfg_Type *psramCfg)
{
    return RomDriver_Psram_SetDriveStrength(psramCfg);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
    BL_Err_Type
    Psram_SetBurstWrap(SPI_Psram_Cfg_Type *psramCfg)
{
    return RomDriver_Psram_SetBurstWrap(psramCfg);
}

__ALWAYS_INLINE ATTR_TCM_SECTION void Psram_ReadId(SPI_Psram_Cfg_Type *psramCfg, uint8_t *data)
{
    RomDriver_Psram_ReadId(psramCfg, data);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
    BL_Err_Type
    Psram_EnterQuadMode(SPI_Psram_Cfg_Type *psramCfg)
{
    return RomDriver_Psram_EnterQuadMode(psramCfg);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
    BL_Err_Type
    Psram_ExitQuadMode(SPI_Psram_Cfg_Type *psramCfg)
{
    return RomDriver_Psram_ExitQuadMode(psramCfg);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
    BL_Err_Type
    Psram_ToggleBurstLength(SPI_Psram_Cfg_Type *psramCfg, PSRAM_Ctrl_Mode ctrlMode)
{
    return RomDriver_Psram_ToggleBurstLength(psramCfg, ctrlMode);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
    BL_Err_Type
    Psram_SoftwareReset(SPI_Psram_Cfg_Type *psramCfg, PSRAM_Ctrl_Mode ctrlMode)
{
    return RomDriver_Psram_SoftwareReset(psramCfg, ctrlMode);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
    BL_Err_Type
    Psram_Set_IDbus_Cfg(SPI_Psram_Cfg_Type *psramCfg,
                        SF_Ctrl_IO_Type ioMode, uint32_t addr, uint32_t len)
{
    return RomDriver_Psram_Set_IDbus_Cfg(psramCfg, ioMode, addr, len);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
    BL_Err_Type
    Psram_Cache_Write_Set(SPI_Psram_Cfg_Type *psramCfg, SF_Ctrl_IO_Type ioMode,
                          BL_Fun_Type wtEn, BL_Fun_Type wbEn, BL_Fun_Type waEn)
{
    return RomDriver_Psram_Cache_Write_Set(psramCfg, ioMode, wtEn, wbEn, waEn);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
    BL_Err_Type
    Psram_Write(SPI_Psram_Cfg_Type *psramCfg,
                SF_Ctrl_IO_Type ioMode, uint32_t addr, uint8_t *data, uint32_t len)
{
    return RomDriver_Psram_Write(psramCfg, ioMode, addr, data, len);
}

__ALWAYS_INLINE ATTR_TCM_SECTION
    BL_Err_Type
    Psram_Read(SPI_Psram_Cfg_Type *psramCfg,
               SF_Ctrl_IO_Type ioMode, uint32_t addr, uint8_t *data, uint32_t len)
{
    return RomDriver_Psram_Read(psramCfg, ioMode, addr, data, len);
}
/******************************************************************************/
