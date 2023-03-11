#ifndef WORK_Q_H
#define WORK_Q_H
#include "atomic.h"
#include "zephyr.h"

#if defined(BFLB_BLE)
struct k_work_q {
    struct k_fifo fifo;
};

typedef struct {
    bl_timer_t timer;
    struct k_delayed_work *delay_work;
} timer_rec_d;

int k_work_q_start();

enum {
    K_WORK_STATE_PENDING,
    K_WORK_STATE_PERIODIC,
};
struct k_work;
/* work define*/
typedef void (*k_work_handler_t)(struct k_work *work);
struct k_work {
    void *_reserved;
    k_work_handler_t handler;
    atomic_t flags[1];
};

#define _K_WORK_INITIALIZER(work_handler) \
    {                                     \
        ._reserved = NULL,                \
        .handler = work_handler,          \
        .flags = { 0 }                    \
    }

#define K_WORK_INITIALIZER __DEPRECATED_MACRO _K_WORK_INITIALIZER

int k_work_init(struct k_work *work, k_work_handler_t handler);
void k_work_submit(struct k_work *work);

/*delay work define*/
struct k_delayed_work {
    struct k_work work;
    struct k_work_q *work_q;
    k_timer_t timer;
};

void k_delayed_work_init(struct k_delayed_work *work, k_work_handler_t handler);
int k_delayed_work_submit(struct k_delayed_work *work, uint32_t delay);
/* Added by bouffalolab */
int k_delayed_work_submit_periodic(struct k_delayed_work *work, s32_t period);
int k_delayed_work_cancel(struct k_delayed_work *work);
s32_t k_delayed_work_remaining_get(struct k_delayed_work *work);
void k_delayed_work_del_timer(struct k_delayed_work *work);
/* Added by bouffalolab */
int k_delayed_work_free(struct k_delayed_work *work);
#endif
#endif /* WORK_Q_H */
