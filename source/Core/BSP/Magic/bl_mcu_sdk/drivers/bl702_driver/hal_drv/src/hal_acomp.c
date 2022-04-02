#include "hal_acomp.h"
#include "bl702_acomp.h"
#include "hbn_reg.h"

void acomp_init(uint8_t idx, acomp_device_t *device)
{
    uint32_t tmpVal;

    if (idx == 0) {
        tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_MODE);
        tmpVal &= ~(1 << 20);
        tmpVal &= ~(1 << 21);
        BL_WR_REG(HBN_BASE, HBN_IRQ_MODE, tmpVal);

        /* Disable ACOMP first */
        tmpVal = BL_RD_REG(AON_BASE, AON_ACOMP0_CTRL);
        tmpVal = BL_CLR_REG_BIT(tmpVal, AON_ACOMP0_EN);
        tmpVal = BL_WR_REG(AON_BASE, AON_ACOMP0_CTRL, tmpVal);

        /* Set ACOMP config */
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_ACOMP0_MUXEN, 1);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_ACOMP0_POS_SEL, device->pos_ch);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_ACOMP0_NEG_SEL, device->neg_ch);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_ACOMP0_LEVEL_SEL, AON_ACOMP_LEVEL_FACTOR_1);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_ACOMP0_BIAS_PROG, AON_ACOMP_BIAS_POWER_MODE1);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_ACOMP0_HYST_SELP, device->pos_hysteresis_vol);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_ACOMP0_HYST_SELN, device->neg_hysteresis_vol);
        BL_WR_REG(AON_BASE, AON_ACOMP0_CTRL, tmpVal);

    } else {
        tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_MODE);
        tmpVal &= ~(1 << 22);
        tmpVal &= ~(1 << 23);
        BL_WR_REG(HBN_BASE, HBN_IRQ_MODE, tmpVal);

        /* Disable ACOMP first */
        tmpVal = BL_RD_REG(AON_BASE, AON_ACOMP1_CTRL);
        tmpVal = BL_CLR_REG_BIT(tmpVal, AON_ACOMP1_EN);
        tmpVal = BL_WR_REG(AON_BASE, AON_ACOMP1_CTRL, tmpVal);

        /* Set ACOMP config */
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_ACOMP1_MUXEN, 1);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_ACOMP1_POS_SEL, device->pos_ch);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_ACOMP1_NEG_SEL, device->neg_ch);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_ACOMP1_LEVEL_SEL, AON_ACOMP_LEVEL_FACTOR_1);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_ACOMP1_BIAS_PROG, AON_ACOMP_BIAS_POWER_MODE1);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_ACOMP1_HYST_SELP, device->pos_hysteresis_vol);
        tmpVal = BL_SET_REG_BITS_VAL(tmpVal, AON_ACOMP1_HYST_SELN, device->neg_hysteresis_vol);
        BL_WR_REG(AON_BASE, AON_ACOMP1_CTRL, tmpVal);
    }
}

void acomp_enable(uint8_t idx)
{
    uint32_t tmpVal;
    if (idx == 0) {
        tmpVal = BL_RD_REG(AON_BASE, AON_ACOMP0_CTRL);
        tmpVal = BL_SET_REG_BIT(tmpVal, AON_ACOMP0_EN);
        BL_WR_REG(AON_BASE, AON_ACOMP0_CTRL, tmpVal);
    } else {
        tmpVal = BL_RD_REG(AON_BASE, AON_ACOMP1_CTRL);
        tmpVal = BL_SET_REG_BIT(tmpVal, AON_ACOMP1_EN);
        BL_WR_REG(AON_BASE, AON_ACOMP1_CTRL, tmpVal);
    }
}

void acomp_disable(uint8_t idx)
{
    uint32_t tmpVal;
    if (idx == 0) {
        tmpVal = BL_RD_REG(AON_BASE, AON_ACOMP0_CTRL);
        tmpVal = BL_CLR_REG_BIT(tmpVal, AON_ACOMP0_EN);
        BL_WR_REG(AON_BASE, AON_ACOMP0_CTRL, tmpVal);
    } else {
        tmpVal = BL_RD_REG(AON_BASE, AON_ACOMP1_CTRL);
        tmpVal = BL_CLR_REG_BIT(tmpVal, AON_ACOMP1_EN);
        BL_WR_REG(AON_BASE, AON_ACOMP1_CTRL, tmpVal);
    }
}

void acomp_interrupt_mask(uint8_t idx, uint32_t flag)
{
    uint32_t tmpVal;
    tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_MODE);
    if (idx == 0) {
        tmpVal &= ~(flag << 20);
    } else {
        tmpVal &= ~(flag << 22);
    }
}

void acomp_interrupt_unmask(uint8_t idx, uint32_t flag)
{
    uint32_t tmpVal;

    if (idx == 0) {
        /* set clear bit */
        tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_CLR);
        tmpVal |= (1 << 20);
        BL_WR_REG(HBN_BASE, HBN_IRQ_CLR, tmpVal);

        /* unset clear bit */
        tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_CLR);
        tmpVal &= (~(1 << 20));
        BL_WR_REG(HBN_BASE, HBN_IRQ_CLR, tmpVal);

        tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_MODE);
        tmpVal |= (flag << 20);
    } else {
        /* set clear bit */
        tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_CLR);
        tmpVal |= (1 << 22);
        BL_WR_REG(HBN_BASE, HBN_IRQ_CLR, tmpVal);

        /* unset clear bit */
        tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_CLR);
        tmpVal &= (~(1 << 22));
        BL_WR_REG(HBN_BASE, HBN_IRQ_CLR, tmpVal);

        tmpVal = BL_RD_REG(HBN_BASE, HBN_IRQ_MODE);
        tmpVal |= (flag << 22);
    }

    BL_WR_REG(HBN_BASE, HBN_IRQ_MODE, tmpVal);
}

int acomp_get_result(uint8_t idx)
{
    uint32_t tmpVal;
    tmpVal = BL_RD_REG(AON_BASE, AON_ACOMP_CTRL);

    /* Disable ACOMP first */
    if (idx == 0) {
        if (BL_IS_REG_BIT_SET(tmpVal, AON_ACOMP0_OUT_RAW)) {
            return 1;
        } else {
            return 0;
        }
    } else {
        if (BL_IS_REG_BIT_SET(tmpVal, AON_ACOMP1_OUT_RAW)) {
            return 1;
        } else {
            return 0;
        }
    }
}