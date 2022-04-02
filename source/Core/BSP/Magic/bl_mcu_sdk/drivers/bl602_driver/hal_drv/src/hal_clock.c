#include "bl602_glb.h"
#include "hal_clock.h"

static uint32_t mtimer_get_clk_src_div(void)
{
    return (system_clock_get(SYSTEM_CLOCK_BCLK) / 1000 / 1000 - 1);
}

void system_clock_init(void)
{
    /*select root clock*/
    GLB_Set_System_CLK(XTAL_TYPE, BSP_ROOT_CLOCK_SOURCE);
    /*set fclk/hclk and bclk clock*/
    GLB_Set_System_CLK_Div(BSP_FCLK_DIV, BSP_BCLK_DIV);
    GLB_Set_MTimer_CLK(1, GLB_MTIMER_CLK_BCLK, mtimer_get_clk_src_div());
#if XTAL_32K_TYPE == INTERNAL_RC_32K
    HBN_32K_Sel(HBN_32K_RC);
    HBN_Power_Off_Xtal_32K();
#else
    HBN_32K_Sel(HBN_32K_XTAL);
    HBN_Power_On_Xtal_32K();
#endif
#if (XTAL_TYPE == INTERNAL_RC_32M) || (XTAL_TYPE == XTAL_NONE)
    HBN_Set_XCLK_CLK_Sel(HBN_XCLK_CLK_RC32M);
#else
    HBN_Set_XCLK_CLK_Sel(HBN_XCLK_CLK_XTAL);
#endif
}

void system_mtimer_clock_init(void)
{
    GLB_Set_MTimer_CLK(1, GLB_MTIMER_CLK_BCLK, mtimer_get_clk_src_div());
}

void system_mtimer_clock_reinit(void)
{
    /* reinit clock to 10M */
    GLB_Set_MTimer_CLK(1, GLB_MTIMER_CLK_BCLK, 7);
}

void peripheral_clock_init(void)
{
#if defined(BSP_USING_UART0) || defined(BSP_USING_UART1)
#if BSP_UART_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_PLL_160M
    GLB_Set_UART_CLK(ENABLE, HBN_UART_CLK_160M, BSP_UART_CLOCK_DIV);
#elif BSP_UART_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_FCLK
    GLB_Set_UART_CLK(ENABLE, HBN_UART_CLK_FCLK, BSP_UART_CLOCK_DIV);
#endif
#endif

#if defined(BSP_USING_I2C0)
    GLB_Set_I2C_CLK(ENABLE, BSP_I2C_CLOCK_DIV);
#endif

#if defined(BSP_USING_SPI0)
    GLB_Set_SPI_CLK(ENABLE, BSP_SPI_CLOCK_DIV);
#endif

#if defined(BSP_USING_PWM)
#if BSP_PWM_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_RC_32K

#elif BSP_PWM_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_BCLK

#elif BSP_PWM_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_XCLK

#endif
#endif

#if defined(BSP_USING_IR)
    GLB_Set_IR_CLK(ENABLE, 0, BSP_IR_CLOCK_DIV);
#endif

#if defined(BSP_USING_ADC0)
#if BSP_ADC_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_AUPLL
    GLB_Set_ADC_CLK(ENABLE, GLB_ADC_CLK_AUDIO_PLL, BSP_ADC_CLOCK_DIV);
#elif BSP_ADC_CLOCK_SOURCE == ROOT_CLOCK_SOURCE_XCLK
    GLB_Set_ADC_CLK(ENABLE, GLB_ADC_CLK_XCLK, BSP_ADC_CLOCK_DIV);
#endif

#endif

#if defined(BSP_USING_DAC0)
    GLB_Set_DAC_CLK(ENABLE, GLB_ADC_CLK_XCLK, BSP_DAC_CLOCK_DIV);
#endif
}
uint32_t system_clock_get(enum system_clock_type type)
{
    switch (type) {
        case SYSTEM_CLOCK_ROOT_CLOCK:
            if (GLB_Get_Root_CLK_Sel() == 0) {
                return 32 * 1000 * 1000;
            } else if (GLB_Get_Root_CLK_Sel() == 1)
                return 32 * 1000 * 1000;
            else {
                uint32_t tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG0);
                tmpVal = BL_GET_REG_BITS_VAL(tmpVal, GLB_REG_PLL_SEL);
                if (tmpVal == 0) {
                    return 48 * 1000 * 1000;
                } else if (tmpVal == 1) {
                    return 120 * 1000 * 1000;
                } else if (tmpVal == 2) {
                    return 160 * 1000 * 1000;
                } else if (tmpVal == 3) {
                    return 192 * 1000 * 1000;
                } else {
                    return 0;
                }
            }
        case SYSTEM_CLOCK_FCLK:
            return system_clock_get(SYSTEM_CLOCK_ROOT_CLOCK) / (GLB_Get_HCLK_Div() + 1);

        case SYSTEM_CLOCK_BCLK:
            return system_clock_get(SYSTEM_CLOCK_ROOT_CLOCK) / (GLB_Get_HCLK_Div() + 1) / (GLB_Get_BCLK_Div() + 1);

        case SYSTEM_CLOCK_XCLK:
            switch (XTAL_TYPE) {
                case XTAL_NONE:
                    return 32000000;
                case EXTERNAL_XTAL_24M:
                    return 24000000;
                case EXTERNAL_XTAL_32M:
                    return 32000000;
                case EXTERNAL_XTAL_38P4M:
                    return 38400000;
                case EXTERNAL_XTAL_40M:
                    return 40000000;
                case EXTERNAL_XTAL_26M:
                    return 26000000;
                case INTERNAL_RC_32M:
                    return 32000000;
                default:
                    break;
            }

        default:
            break;
    }

    return 0;
}
uint32_t peripheral_clock_get(enum peripheral_clock_type type)
{
    uint32_t tmpVal;
    uint32_t div;

    switch (type) {
#if defined(BSP_USING_UART0) || defined(BSP_USING_UART1)
        case PERIPHERAL_CLOCK_UART:
            tmpVal = BL_RD_REG(HBN_BASE, HBN_GLB);
            tmpVal = BL_GET_REG_BITS_VAL(tmpVal, HBN_UART_CLK_SEL);

            div = BL_RD_REG(GLB_BASE, GLB_CLK_CFG2);
            div = BL_GET_REG_BITS_VAL(div, GLB_UART_CLK_DIV);
            if (tmpVal == HBN_UART_CLK_FCLK) {
                return system_clock_get(SYSTEM_CLOCK_FCLK) / (div + 1);
            } else if (tmpVal == HBN_UART_CLK_160M) {
                return 160000000 / (div + 1);
            }
#endif
#if defined(BSP_USING_SPI0)
        case PERIPHERAL_CLOCK_SPI:
            tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG3);
            div = BL_GET_REG_BITS_VAL(tmpVal, GLB_SPI_CLK_DIV);
            return system_clock_get(SYSTEM_CLOCK_BCLK) / (div + 1);
#endif

        case PERIPHERAL_CLOCK_I2C:
#if defined(BSP_USING_I2C0)
            tmpVal = BL_RD_REG(GLB_BASE, GLB_CLK_CFG3);
            div = BL_GET_REG_BITS_VAL(tmpVal, GLB_I2C_CLK_DIV);
            return system_clock_get(SYSTEM_CLOCK_BCLK) / (div + 1);
#endif

        case PERIPHERAL_CLOCK_ADC:
        case PERIPHERAL_CLOCK_DAC:
            return 32000000;

        default:
            break;
    }

    (void)(tmpVal);
    (void)(div);
    return 0;
}