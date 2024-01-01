/*
 * xx
 */

#ifndef _MULTI_ADV_H_
#define _MULTI_ADV_H_

#define MAX_MULTI_ADV_INSTANT 4
#define MAX_AD_DATA_LEN       31

#define TIME_PRIOD_MS   (10 * (MAX_MULTI_ADV_INSTANT - 2))
#define SLOT_PER_PERIOD (TIME_PRIOD_MS * 8 / 5)

#define MAX_MIN_MULTI (30000 / TIME_PRIOD_MS)

#define HIGH_DUTY_CYCLE_INTERVAL (20 * 8 / 5)

struct multi_adv_instant {
    uint8_t inuse_flag;

    /* for parameters  */
    struct bt_le_adv_param param;
    uint8_t ad[MAX_AD_DATA_LEN];
    uint8_t ad_len;
    uint8_t sd[MAX_AD_DATA_LEN];
    uint8_t sd_len;

    /* own address maybe used */
    bt_addr_t own_addr;
    uint8_t own_addr_valid;

    /* for schedule */
    int instant_id;
    int instant_interval;
    int instant_offset;
    uint32_t clock;
    uint32_t clock_instant_offset;
    uint32_t clock_instant_total;
    uint32_t next_wakeup_time;
};

typedef enum {
    SCHEDULE_IDLE,
    SCHEDULE_READY,
    SCHEDULE_START,
    SCHEDULE_STOP,
} SCHEDULE_STATE;

struct multi_adv_scheduler {
    SCHEDULE_STATE schedule_state;
    uint8_t schedule_timer_active;
    uint32_t slot_clock;
    uint16_t slot_offset;
    uint16_t next_slot_offset;
    uint32_t next_slot_clock;
};

int bt_le_multi_adv_thread_init(void);
int bt_le_multi_adv_start(const struct bt_le_adv_param *param,
                          const struct bt_data *ad, size_t ad_len,
                          const struct bt_data *sd, size_t sd_len, int *instant_id);
int bt_le_multi_adv_stop(int instant_id);

bool bt_le_multi_adv_id_is_vaild(int instant_id);

#endif
