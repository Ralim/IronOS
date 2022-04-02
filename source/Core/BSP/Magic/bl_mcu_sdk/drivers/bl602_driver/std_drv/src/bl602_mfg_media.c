#include "bl602_mfg_media.h"

static uint8_t rf_para_on_flash = 0;

int8_t mfg_media_init_need_lock(SPI_Flash_Cfg_Type *flashCfg)
{
    if (0 == mfg_flash_init(flashCfg)) {
        rf_para_on_flash = 1;
    } else {
        rf_para_on_flash = 0;
    }

    return 0;
}

int8_t mfg_media_init_with_lock(SPI_Flash_Cfg_Type *flashCfg)
{
    int8_t ret;

    cpu_global_irq_disable();
    ret = mfg_media_init_need_lock(flashCfg);
    cpu_global_irq_enable();

    return ret;
}

uint8_t mfg_media_is_xtal_capcode_slot_empty(uint8_t reload)
{
    if (rf_para_on_flash) {
        return 1;
    } else {
        return mfg_efuse_is_xtal_capcode_slot_empty(reload);
    }
}

int8_t mfg_media_write_xtal_capcode_pre_need_lock(uint8_t capcode, uint8_t program)
{
    if (rf_para_on_flash) {
        return mfg_flash_write_xtal_capcode_pre(capcode, program);
    } else {
        return mfg_efuse_write_xtal_capcode_pre(capcode, program);
    }
}

int8_t mfg_media_write_xtal_capcode_pre_with_lock(uint8_t capcode, uint8_t program)
{
    int8_t ret;

    cpu_global_irq_disable();
    ret = mfg_media_write_xtal_capcode_pre_need_lock(capcode, program);
    cpu_global_irq_enable();

    return ret;
}

void mfg_media_write_xtal_capcode_need_lock(void)
{
    if (rf_para_on_flash) {
        return mfg_flash_write_xtal_capcode();
    } else {
        return mfg_efuse_write_xtal_capcode();
    }
}

void mfg_media_write_xtal_capcode_with_lock(void)
{
    cpu_global_irq_disable();
    mfg_media_write_xtal_capcode_need_lock();
    cpu_global_irq_enable();
}

int8_t mfg_media_read_xtal_capcode_need_lock(uint8_t *capcode, uint8_t reload)
{
    if (rf_para_on_flash) {
        return mfg_flash_read_xtal_capcode(capcode, reload);
    } else {
        return mfg_efuse_read_xtal_capcode(capcode, reload);
    }
}

int8_t mfg_media_read_xtal_capcode_with_lock(uint8_t *capcode, uint8_t reload)
{
    int8_t ret;

    cpu_global_irq_disable();
    ret = mfg_media_read_xtal_capcode_need_lock(capcode, reload);
    cpu_global_irq_enable();

    return ret;
}

uint8_t mfg_media_is_poweroffset_slot_empty(uint8_t reload)
{
    if (rf_para_on_flash) {
        return 1;
    } else {
        return mfg_efuse_is_poweroffset_slot_empty(reload);
    }
}

int8_t mfg_media_write_poweroffset_pre_need_lock(int8_t pwrOffset[14], uint8_t program)
{
    if (rf_para_on_flash) {
        return mfg_flash_write_poweroffset_pre(pwrOffset, program);
    } else {
        return mfg_efuse_write_poweroffset_pre(pwrOffset, program);
    }
}

int8_t mfg_media_write_poweroffset_pre_with_lock(int8_t pwrOffset[14], uint8_t program)
{
    int ret;

    cpu_global_irq_disable();
    ret = mfg_media_write_poweroffset_pre_need_lock(pwrOffset, program);
    cpu_global_irq_enable();

    return ret;
}

void mfg_media_write_poweroffset_need_lock(void)
{
    if (rf_para_on_flash) {
        return mfg_flash_write_poweroffset();
    } else {
        return mfg_efuse_write_poweroffset();
    }
}

void mfg_media_write_poweroffset_with_lock(void)
{
    cpu_global_irq_disable();
    mfg_media_write_poweroffset_need_lock();
    cpu_global_irq_enable();
}

int8_t mfg_media_read_poweroffset_need_lock(int8_t pwrOffset[14], uint8_t reload)
{
    if (rf_para_on_flash) {
        return mfg_flash_read_poweroffset(pwrOffset, reload);
    } else {
        return mfg_efuse_read_poweroffset(pwrOffset, reload);
    }
}

int8_t mfg_media_read_poweroffset_with_lock(int8_t pwrOffset[14], uint8_t reload)
{
    int ret;

    cpu_global_irq_disable();
    ret = mfg_media_read_poweroffset_need_lock(pwrOffset, reload);
    cpu_global_irq_enable();

    return ret;
}

uint8_t mfg_media_is_macaddr_slot_empty(uint8_t reload)
{
    if (rf_para_on_flash) {
        return 1;
    } else {
        return mfg_efuse_is_macaddr_slot_empty(reload);
    }
}

int8_t mfg_media_write_macaddr_pre_need_lock(uint8_t mac[6], uint8_t program)
{
    if (rf_para_on_flash) {
        return mfg_flash_write_macaddr_pre(mac, program);
    } else {
        return mfg_efuse_write_macaddr_pre(mac, program);
    }
}

int8_t mfg_media_write_macaddr_pre_with_lock(uint8_t mac[6], uint8_t program)
{
    int ret;

    cpu_global_irq_disable();
    ret = mfg_media_write_macaddr_pre_need_lock(mac, program);
    cpu_global_irq_enable();

    return ret;
}

void mfg_media_write_macaddr_need_lock(void)
{
    if (rf_para_on_flash) {
        return mfg_flash_write_macaddr();
    } else {
        return mfg_efuse_write_macaddr();
    }
}

void mfg_media_write_macaddr_with_lock(void)
{
    cpu_global_irq_disable();
    mfg_media_write_macaddr_need_lock();
    cpu_global_irq_enable();
}

int8_t mfg_media_read_macaddr_need_lock(uint8_t mac[6], uint8_t reload)
{
    if (rf_para_on_flash) {
        return mfg_flash_read_macaddr(mac, reload);
    } else {
        return mfg_efuse_read_macaddr(mac, reload);
    }
}

int8_t mfg_media_read_macaddr_with_lock(uint8_t mac[6], uint8_t reload)
{
    int ret;

    cpu_global_irq_disable();
    ret = mfg_media_read_macaddr_need_lock(mac, reload);
    cpu_global_irq_enable();

    return ret;
}
