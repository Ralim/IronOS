#include "bl602_glb.h"
#include "bl602_mfg_efuse.h"
#include "bl602_ef_ctrl.h"

static uint8_t rf_cal_slots = 3;
extern void main(void);
#define RF_CAL_SLOT_CFG_OFFSET (4 * 13)
#ifdef BFLB_MCU_SDK
#include "bflb_platform.h"
#define mfg_print MSG
#else
#define mfg_print printf
#endif

uint8_t mfg_efuse_get_rf_cal_slots(void)
{
#if 0
	uint8_t *pslot;
	pslot = (uint8_t *)((((uint32_t)main) & 0xfff00000) + RF_CAL_SLOT_CFG_OFFSET);

	if(*pslot != 0)
	{
		rf_cal_slots = *pslot;
	}

#endif
    return rf_cal_slots;
}

void mfg_efuse_set_rf_cal_slots(uint8_t slots)
{
    rf_cal_slots = slots;
}

uint8_t mfg_efuse_is_xtal_capcode_slot_empty(uint8_t reload)
{
    uint8_t empty = 0;
    uint8_t hdiv = 0, bdiv = 0;
    HBN_ROOT_CLK_Type rtClk = GLB_Get_Root_CLK_Sel();

    bdiv = GLB_Get_BCLK_Div();
    hdiv = GLB_Get_HCLK_Div();
#if 1
    HBN_Set_ROOT_CLK_Sel(HBN_ROOT_CLK_XTAL);

    if (rf_cal_slots >= 1 && EF_Ctrl_Is_CapCode_Slot_Empty(0, reload)) {
        mfg_print("Empty slot:%d\r\n", 0);
        empty = 1;
    } else if (rf_cal_slots >= 2 && EF_Ctrl_Is_CapCode_Slot_Empty(1, reload)) {
        mfg_print("Empty slot:%d\r\n", 1);
        empty = 1;
    } else if (rf_cal_slots >= 3 && EF_Ctrl_Is_CapCode_Slot_Empty(2, reload)) {
        mfg_print("Empty slot:%d\r\n", 2);
        empty = 1;
    } else {
        mfg_print("No empty slot found\r\n");
    }

    GLB_Set_System_CLK_Div(hdiv, bdiv);
    HBN_Set_ROOT_CLK_Sel(rtClk);
#endif
    return empty;
}

int8_t mfg_efuse_write_xtal_capcode_pre(uint8_t capcode, uint8_t program)
{
    BL_Err_Type ret = SUCCESS;
    uint8_t slot = 0xff;
    uint8_t hdiv = 0, bdiv = 0;
    HBN_ROOT_CLK_Type rtClk = GLB_Get_Root_CLK_Sel();

    bdiv = GLB_Get_BCLK_Div();
    hdiv = GLB_Get_HCLK_Div();
#if 1
    HBN_Set_ROOT_CLK_Sel(HBN_ROOT_CLK_XTAL);

    if (rf_cal_slots >= 1 && EF_Ctrl_Is_CapCode_Slot_Empty(0, 1)) {
        slot = 0;
    } else if (rf_cal_slots >= 2 && EF_Ctrl_Is_CapCode_Slot_Empty(1, 1)) {
        slot = 1;
    } else if (rf_cal_slots >= 3 && EF_Ctrl_Is_CapCode_Slot_Empty(2, 1)) {
        slot = 2;
    } else {
        mfg_print("No empty slot found\r\n");
    }

    if (slot != 0xff) {
        ret = EF_Ctrl_Write_CapCode_Opt(slot, capcode, program);
        mfg_print("Write slot:%d\r\n", slot);
    }

    GLB_Set_System_CLK_Div(hdiv, bdiv);
    HBN_Set_ROOT_CLK_Sel(rtClk);
#endif

    if (ret == SUCCESS) {
        return 0;
    } else {
        return -1;
    }
}

void mfg_efuse_write_xtal_capcode(void)
{
    uint8_t hdiv = 0, bdiv = 0;
    HBN_ROOT_CLK_Type rtClk = GLB_Get_Root_CLK_Sel();

    bdiv = GLB_Get_BCLK_Div();
    hdiv = GLB_Get_HCLK_Div();
#if 1
    HBN_Set_ROOT_CLK_Sel(HBN_ROOT_CLK_XTAL);

    EF_Ctrl_Program_Direct_R0(0, NULL, 0);

    while (SET == EF_Ctrl_Busy())
        ;

    GLB_Set_System_CLK_Div(hdiv, bdiv);
    HBN_Set_ROOT_CLK_Sel(rtClk);
#endif
}

int8_t mfg_efuse_read_xtal_capcode(uint8_t *capcode, uint8_t reload)
{
    uint8_t slot = 0xff;
    BL_Err_Type ret = ERROR;
    uint8_t hdiv = 0, bdiv = 0;
    HBN_ROOT_CLK_Type rtClk = GLB_Get_Root_CLK_Sel();

    bdiv = GLB_Get_BCLK_Div();
    hdiv = GLB_Get_HCLK_Div();
#if 1
    HBN_Set_ROOT_CLK_Sel(HBN_ROOT_CLK_XTAL);

    if (rf_cal_slots >= 3 && (!EF_Ctrl_Is_CapCode_Slot_Empty(2, reload))) {
        slot = 2;
    } else if (rf_cal_slots >= 2 && (!EF_Ctrl_Is_CapCode_Slot_Empty(1, reload))) {
        slot = 1;
    } else if (rf_cal_slots >= 1 && (!EF_Ctrl_Is_CapCode_Slot_Empty(0, reload))) {
        slot = 0;
    }

    if (slot != 0xff) {
        mfg_print("Read slot:%d\r\n", slot);
        ret = EF_Ctrl_Read_CapCode_Opt(slot, capcode, reload);
    } else {
        mfg_print("No written slot found\r\n");
    }

    GLB_Set_System_CLK_Div(hdiv, bdiv);
    HBN_Set_ROOT_CLK_Sel(rtClk);
#endif

    if (ret == SUCCESS) {
        return 0;
    } else {
        return -1;
    }
}

uint8_t mfg_efuse_is_poweroffset_slot_empty(uint8_t reload)
{
    uint8_t empty = 0;
    uint8_t hdiv = 0, bdiv = 0;
    HBN_ROOT_CLK_Type rtClk = GLB_Get_Root_CLK_Sel();

    bdiv = GLB_Get_BCLK_Div();
    hdiv = GLB_Get_HCLK_Div();
#if 1
    HBN_Set_ROOT_CLK_Sel(HBN_ROOT_CLK_XTAL);

    if (rf_cal_slots >= 1 && EF_Ctrl_Is_PowerOffset_Slot_Empty(0, reload)) {
        mfg_print("Empty slot:%d\r\n", 0);
        empty = 1;
    } else if (rf_cal_slots >= 2 && EF_Ctrl_Is_PowerOffset_Slot_Empty(1, reload)) {
        mfg_print("Empty slot:%d\r\n", 1);
        empty = 1;
    } else if (rf_cal_slots >= 3 && EF_Ctrl_Is_PowerOffset_Slot_Empty(2, reload)) {
        mfg_print("Empty slot:%d\r\n", 2);
        empty = 1;
    } else {
        mfg_print("No empty slot found\r\n");
    }

    GLB_Set_System_CLK_Div(hdiv, bdiv);
    HBN_Set_ROOT_CLK_Sel(rtClk);
#endif
    return empty;
}

int8_t mfg_efuse_write_poweroffset_pre(int8_t pwrOffset[14], uint8_t program)
{
    BL_Err_Type ret = SUCCESS;
    uint8_t slot = 0xff;
    int8_t pwrOffsetTmp[3];
    uint8_t hdiv = 0, bdiv = 0;
    HBN_ROOT_CLK_Type rtClk = GLB_Get_Root_CLK_Sel();

    bdiv = GLB_Get_BCLK_Div();
    hdiv = GLB_Get_HCLK_Div();

#if 1
    HBN_Set_ROOT_CLK_Sel(HBN_ROOT_CLK_XTAL);

    if (rf_cal_slots >= 1 && EF_Ctrl_Is_PowerOffset_Slot_Empty(0, 1)) {
        slot = 0;
    } else if (rf_cal_slots >= 2 && EF_Ctrl_Is_PowerOffset_Slot_Empty(1, 1)) {
        slot = 1;
    } else if (rf_cal_slots >= 3 && EF_Ctrl_Is_PowerOffset_Slot_Empty(2, 1)) {
        slot = 2;
    } else {
        mfg_print("No empty slot found\r\n");
    }

    if (slot != 0xff) {
        pwrOffsetTmp[0] = pwrOffset[0];
        pwrOffsetTmp[1] = pwrOffset[6];
        pwrOffsetTmp[2] = pwrOffset[12];
        ret = EF_Ctrl_Write_PowerOffset_Opt(slot, pwrOffsetTmp, program);
        mfg_print("Write slot:%d\r\n", slot);
    }

    GLB_Set_System_CLK_Div(hdiv, bdiv);
    HBN_Set_ROOT_CLK_Sel(rtClk);
#endif

    if (ret == SUCCESS) {
        return 0;
    } else {
        return -1;
    }
}

void mfg_efuse_write_poweroffset(void)
{
    uint8_t hdiv = 0, bdiv = 0;
    HBN_ROOT_CLK_Type rtClk = GLB_Get_Root_CLK_Sel();

    bdiv = GLB_Get_BCLK_Div();
    hdiv = GLB_Get_HCLK_Div();
#if 1
    HBN_Set_ROOT_CLK_Sel(HBN_ROOT_CLK_XTAL);

    EF_Ctrl_Program_Direct_R0(0, NULL, 0);

    while (SET == EF_Ctrl_Busy())
        ;

    GLB_Set_System_CLK_Div(hdiv, bdiv);
    HBN_Set_ROOT_CLK_Sel(rtClk);
#endif
}

int8_t mfg_efuse_read_poweroffset(int8_t pwrOffset[14], uint8_t reload)
{
    uint8_t slot = 0xff;
    BL_Err_Type ret = ERROR;
    int8_t pwrOffsetTmp[3];

    int32_t step = 0;
    uint8_t hdiv = 0, bdiv = 0;
    HBN_ROOT_CLK_Type rtClk = GLB_Get_Root_CLK_Sel();

    bdiv = GLB_Get_BCLK_Div();
    hdiv = GLB_Get_HCLK_Div();

#if 1
    HBN_Set_ROOT_CLK_Sel(HBN_ROOT_CLK_XTAL);

    if (rf_cal_slots >= 3 && (!EF_Ctrl_Is_PowerOffset_Slot_Empty(2, reload))) {
        slot = 2;
    } else if (rf_cal_slots >= 2 && (!EF_Ctrl_Is_PowerOffset_Slot_Empty(1, reload))) {
        slot = 1;
    } else if (rf_cal_slots >= 1 && (!EF_Ctrl_Is_PowerOffset_Slot_Empty(0, reload))) {
        slot = 0;
    }

    if (slot != 0xff) {
        mfg_print("Read slot:%d\r\n", slot);
        ret = EF_Ctrl_Read_PowerOffset_Opt(slot, pwrOffsetTmp, reload);
        memset(pwrOffset, 0, 14);

        pwrOffset[0] = pwrOffsetTmp[0];

        step = (pwrOffsetTmp[1] - pwrOffsetTmp[0]) * 100 / 6;
        pwrOffset[1] = (step + 50) / 100 + pwrOffsetTmp[0];
        pwrOffset[2] = (step * 2 + 50) / 100 + pwrOffsetTmp[0];
        pwrOffset[3] = (step * 3 + 50) / 100 + pwrOffsetTmp[0];
        pwrOffset[4] = (step * 4 + 50) / 100 + pwrOffsetTmp[0];
        pwrOffset[5] = (step * 5 + 50) / 100 + pwrOffsetTmp[0];

        pwrOffset[6] = pwrOffsetTmp[1];

        step = (pwrOffsetTmp[2] - pwrOffsetTmp[1]) * 100 / 6;
        pwrOffset[7] = (step + 50) / 100 + pwrOffsetTmp[1];
        pwrOffset[8] = (step * 2 + 50) / 100 + pwrOffsetTmp[1];
        pwrOffset[9] = (step * 3 + 50) / 100 + pwrOffsetTmp[1];
        pwrOffset[10] = (step * 4 + 50) / 100 + pwrOffsetTmp[1];
        pwrOffset[11] = (step * 5 + 50) / 100 + pwrOffsetTmp[1];

        pwrOffset[12] = pwrOffsetTmp[2];

        pwrOffset[13] = (step * 7 + 50) / 100 + pwrOffsetTmp[1];
    } else {
        mfg_print("No written slot found\r\n");
    }

    GLB_Set_System_CLK_Div(hdiv, bdiv);
    HBN_Set_ROOT_CLK_Sel(rtClk);
#endif

    if (ret == SUCCESS) {
        return 0;
    } else {
        return -1;
    }
}

uint8_t mfg_efuse_is_macaddr_slot_empty(uint8_t reload)
{
    uint8_t empty = 0;
    uint8_t hdiv = 0, bdiv = 0;
    HBN_ROOT_CLK_Type rtClk = GLB_Get_Root_CLK_Sel();

    bdiv = GLB_Get_BCLK_Div();
    hdiv = GLB_Get_HCLK_Div();
#if 1
    HBN_Set_ROOT_CLK_Sel(HBN_ROOT_CLK_XTAL);

    if (rf_cal_slots >= 1 && EF_Ctrl_Is_MAC_Address_Slot_Empty(0, reload)) {
        mfg_print("Empty slot:%d\r\n", 0);
        empty = 1;
    } else if (rf_cal_slots >= 2 && EF_Ctrl_Is_MAC_Address_Slot_Empty(1, reload)) {
        mfg_print("Empty slot:%d\r\n", 1);
        empty = 1;
    } else if (rf_cal_slots >= 3 && EF_Ctrl_Is_MAC_Address_Slot_Empty(2, reload)) {
        mfg_print("Empty slot:%d\r\n", 2);
        empty = 1;
    } else {
        mfg_print("No empty slot found\r\n");
    }

    GLB_Set_System_CLK_Div(hdiv, bdiv);
    HBN_Set_ROOT_CLK_Sel(rtClk);
#endif
    return empty;
}

int8_t mfg_efuse_write_macaddr_pre(uint8_t mac[6], uint8_t program)
{
    BL_Err_Type ret = SUCCESS;
    uint8_t slot = 0xff;
    uint8_t hdiv = 0, bdiv = 0;
    HBN_ROOT_CLK_Type rtClk = GLB_Get_Root_CLK_Sel();

    bdiv = GLB_Get_BCLK_Div();
    hdiv = GLB_Get_HCLK_Div();

#if 1
    HBN_Set_ROOT_CLK_Sel(HBN_ROOT_CLK_XTAL);

    if (rf_cal_slots >= 1 && EF_Ctrl_Is_MAC_Address_Slot_Empty(0, 1)) {
        slot = 0;
    } else if (rf_cal_slots >= 2 && EF_Ctrl_Is_MAC_Address_Slot_Empty(1, 1)) {
        slot = 1;
    } else if (rf_cal_slots >= 3 && EF_Ctrl_Is_MAC_Address_Slot_Empty(2, 1)) {
        slot = 2;
    } else {
        mfg_print("No empty slot found\r\n");
    }

    if (slot != 0xff) {
        ret = EF_Ctrl_Write_MAC_Address_Opt(slot, mac, program);
        mfg_print("Write slot:%d\r\n", slot);
    }

    GLB_Set_System_CLK_Div(hdiv, bdiv);
    HBN_Set_ROOT_CLK_Sel(rtClk);

#endif

    if (ret == SUCCESS) {
        return 0;
    } else {
        return -1;
    }
}

void mfg_efuse_write_macaddr(void)
{
    uint8_t hdiv = 0, bdiv = 0;
    HBN_ROOT_CLK_Type rtClk = GLB_Get_Root_CLK_Sel();

    bdiv = GLB_Get_BCLK_Div();
    hdiv = GLB_Get_HCLK_Div();
#if 1
    HBN_Set_ROOT_CLK_Sel(HBN_ROOT_CLK_XTAL);

    EF_Ctrl_Program_Direct_R0(0, NULL, 0);

    while (SET == EF_Ctrl_Busy())
        ;

    //EF_Ctrl_Program_Direct_R1(0,NULL,0);
    //while(SET==EF_Ctrl_Busy());

    GLB_Set_System_CLK_Div(hdiv, bdiv);
    HBN_Set_ROOT_CLK_Sel(rtClk);
#endif
}

int8_t mfg_efuse_read_macaddr(uint8_t mac[6], uint8_t reload)
{
    uint8_t slot = 0xff;
    BL_Err_Type ret = ERROR;
    uint8_t hdiv = 0, bdiv = 0;
    HBN_ROOT_CLK_Type rtClk = GLB_Get_Root_CLK_Sel();

    bdiv = GLB_Get_BCLK_Div();
    hdiv = GLB_Get_HCLK_Div();

#if 1
    HBN_Set_ROOT_CLK_Sel(HBN_ROOT_CLK_XTAL);

    if (rf_cal_slots >= 3 && (!EF_Ctrl_Is_MAC_Address_Slot_Empty(2, reload))) {
        slot = 2;
    } else if (rf_cal_slots >= 2 && (!EF_Ctrl_Is_MAC_Address_Slot_Empty(1, reload))) {
        slot = 1;
    } else if (rf_cal_slots >= 1 && (!EF_Ctrl_Is_MAC_Address_Slot_Empty(0, reload))) {
        slot = 0;
    }

    if (slot != 0xff) {
        mfg_print("Read slot:%d\r\n", slot);
        ret = EF_Ctrl_Read_MAC_Address_Opt(slot, mac, reload);
    } else {
        mfg_print("No written slot found\r\n");
    }

    GLB_Set_System_CLK_Div(hdiv, bdiv);
    HBN_Set_ROOT_CLK_Sel(rtClk);
#endif

    if (ret == SUCCESS) {
        return 0;
    } else {
        return -1;
    }
}

int8_t mfg_efuse_write_pre(uint32_t addr, uint32_t *data, uint32_t countInword)
{
    BL_Err_Type ret = SUCCESS;
    uint8_t hdiv = 0, bdiv = 0;
    HBN_ROOT_CLK_Type rtClk = GLB_Get_Root_CLK_Sel();

    if (addr > 128) {
        return -1;
    }

    bdiv = GLB_Get_BCLK_Div();
    hdiv = GLB_Get_HCLK_Div();

    HBN_Set_ROOT_CLK_Sel(HBN_ROOT_CLK_XTAL);

    EF_Ctrl_Write_R0(addr / 4, data, countInword);

    GLB_Set_System_CLK_Div(hdiv, bdiv);
    HBN_Set_ROOT_CLK_Sel(rtClk);

    if (ret == SUCCESS) {
        return 0;
    } else {
        return -1;
    }
}

int8_t mfg_efuse_read(uint32_t addr, uint32_t *data, uint32_t countInword, uint8_t reload)
{
    BL_Err_Type ret = SUCCESS;
    uint8_t hdiv = 0, bdiv = 0;
    HBN_ROOT_CLK_Type rtClk = GLB_Get_Root_CLK_Sel();

    if (addr > 128) {
        return -1;
    }

    bdiv = GLB_Get_BCLK_Div();
    hdiv = GLB_Get_HCLK_Div();

    HBN_Set_ROOT_CLK_Sel(HBN_ROOT_CLK_XTAL);

    if (reload) {
        EF_Ctrl_Read_Direct_R0(addr / 4, data, countInword);
    } else {
        EF_Ctrl_Read_R0(addr / 4, data, countInword);
    }

    GLB_Set_System_CLK_Div(hdiv, bdiv);
    HBN_Set_ROOT_CLK_Sel(rtClk);

    if (ret == SUCCESS) {
        return 0;
    } else {
        return -1;
    }
}

int8_t mfg_efuse_program(void)
{
    BL_Err_Type ret = SUCCESS;
    uint8_t hdiv = 0, bdiv = 0;
    HBN_ROOT_CLK_Type rtClk = GLB_Get_Root_CLK_Sel();

    bdiv = GLB_Get_BCLK_Div();
    hdiv = GLB_Get_HCLK_Div();

    HBN_Set_ROOT_CLK_Sel(HBN_ROOT_CLK_XTAL);

    EF_Ctrl_Program_Direct_R0(0, NULL, 0);

    while (SET == EF_Ctrl_Busy())
        ;

    GLB_Set_System_CLK_Div(hdiv, bdiv);
    HBN_Set_ROOT_CLK_Sel(rtClk);

    if (ret == SUCCESS) {
        return 0;
    } else {
        return -1;
    }
}
