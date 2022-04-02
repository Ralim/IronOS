#include "bl602.h"
#include "bl602_glb.h"
#include "bl602_hbn.h"
#include "risc-v/Core/Include/clic.h"

/*----------------------------------------------------------------------------
  Define clocks
 *----------------------------------------------------------------------------*/
#define SYSTEM_CLOCK (32000000UL)

/*----------------------------------------------------------------------------
  Vector Table
 *----------------------------------------------------------------------------*/
#define VECT_TAB_OFFSET 0x00 /*!< Vector Table base offset field.
                                   This value must be a multiple of 0x200. */

/*----------------------------------------------------------------------------
  System initialization function
 *----------------------------------------------------------------------------*/

void system_bor_init(void)
{
    HBN_BOR_CFG_Type borCfg = { 1 /* pu_bor */, 0 /* irq_bor_en */, 1 /* bor_vth */, 1 /* bor_sel */ };
    HBN_Set_BOR_Cfg(&borCfg);
}

void SystemInit(void)
{
    uint32_t *p;
    uint32_t i = 0;
    uint32_t tmpVal = 0;

    __disable_irq();

    /* disable hardware_pullup_pull_down (reg_en_hw_pu_pd = 0) */
    tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_MODE);
    tmpVal = BL_CLR_REG_BIT(tmpVal, HBN_REG_EN_HW_PU_PD);
    BL_WR_REG(HBN_BASE, HBN_IRQ_MODE, tmpVal);

    /* GLB_Set_EM_Sel(GLB_EM_0KB); */
    tmpVal = BL_RD_REG(GLB_BASE, GLB_SEAM_MISC);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_EM_SEL, GLB_EM_0KB);
    BL_WR_REG(GLB_BASE, GLB_SEAM_MISC, tmpVal);

    /* Fix 26M xtal clkpll_sdmin */
    tmpVal = BL_RD_REG(PDS_BASE, PDS_CLKPLL_SDM);

    if (0x49D39D == BL_GET_REG_BITS_VAL(tmpVal, PDS_CLKPLL_SDMIN)) {
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, PDS_CLKPLL_SDMIN, 0x49D89E);
        BL_WR_REG(PDS_BASE, PDS_CLKPLL_SDM, tmpVal);
    }

    /* Restore default setting*/
    /* GLB_UART_Sig_Swap_Set(UART_SIG_SWAP_NONE); */
    tmpVal = BL_RD_REG(GLB_BASE, GLB_PARM);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_UART_SWAP_SET, UART_SIG_SWAP_NONE);
    BL_WR_REG(GLB_BASE, GLB_PARM, tmpVal);
    /* GLB_JTAG_Sig_Swap_Set(JTAG_SIG_SWAP_NONE); */
    tmpVal = BL_RD_REG(GLB_BASE, GLB_PARM);
    tmpVal = BL_SET_REG_BITS_VAL(tmpVal, GLB_JTAG_SWAP_SET, JTAG_SIG_SWAP_NONE);
    BL_WR_REG(GLB_BASE, GLB_PARM, tmpVal);

    /* CLear all interrupt */
    p = (uint32_t *)(CLIC_HART0_ADDR + CLIC_INTIE);

    for (i = 0; i < (IRQn_LAST + 3) / 4; i++) {
        p[i] = 0;
    }

    p = (uint32_t *)(CLIC_HART0_ADDR + CLIC_INTIP);

    for (i = 0; i < (IRQn_LAST + 3) / 4; i++) {
        p[i] = 0;
    }

    /* init bor for all platform */
    system_bor_init();
    /* global IRQ enable */
    __enable_irq();
}

void System_Post_Init(void)
{
    PDS_Trim_RC32M();
    HBN_Trim_RC32K();
}