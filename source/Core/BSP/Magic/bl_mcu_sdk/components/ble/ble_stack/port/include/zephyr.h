#ifndef ZEPHYR_H
#define ZEPHYR_H
#include <stdint.h>
#include <stddef.h>

#include <zephyr/types.h>
#include <misc/slist.h>
#include <misc/dlist.h>
#include "bl_port.h"
#include "work_q.h"

#if defined(__cplusplus)
extern "C" {
#endif

#define _STRINGIFY(x) #x
#if 0
#define ___in_section(a, b, c) \
    __attribute__((section("." _STRINGIFY(a) "." _STRINGIFY(b) "." _STRINGIFY(c))))

#endif
#define ARG_UNUSED(x) (void)(x)

#ifndef __aligned
#define __aligned(x) __attribute__((__aligned__(x)))
#endif

#ifndef __printf_like
#define __printf_like(f, a) __attribute__((format(printf, f, a)))
#endif
#define STACK_ALIGN 4

#define ASSERT(test, fmt, ...)

#define K_FOREVER -1
#define K_NO_WAIT 0

/* Unaligned access */
#define UNALIGNED_GET(p)                     \
    __extension__({                          \
        struct __attribute__((__packed__)) { \
            __typeof__(*(p)) __v;            \
        } *__p = (__typeof__(__p))(p);       \
        __p->__v;                            \
    })

#ifndef UNUSED
#define UNUSED(x) (void)x
#endif

enum _poll_types_bits {
    _POLL_TYPE_IGNORE,
    _POLL_TYPE_SIGNAL,
    _POLL_TYPE_SEM_AVAILABLE,
    _POLL_TYPE_DATA_AVAILABLE,
    _POLL_NUM_TYPES
};

#define _POLL_TYPE_BIT(type) (1 << ((type)-1))

enum _poll_states_bits {
    _POLL_STATE_NOT_READY,
    _POLL_STATE_SIGNALED,
    _POLL_STATE_SEM_AVAILABLE,
    _POLL_STATE_DATA_AVAILABLE,
    _POLL_NUM_STATES
};

#define _POLL_STATE_BIT(state) (1 << ((state)-1))

#define _POLL_EVENT_NUM_UNUSED_BITS                             \
    (32 - (0 + 8                                    /* tag */   \
           + _POLL_NUM_TYPES + _POLL_NUM_STATES + 1 /* modes */ \
           ))

#define K_POLL_SIGNAL_INITIALIZER(obj)                          \
    {                                                           \
        .poll_events = SYS_DLIST_STATIC_INIT(&obj.poll_events), \
        .signaled = 0,                                          \
        .result = 0,                                            \
    }

struct k_poll_event {
    sys_dnode_t _node;
    struct _poller *poller;
    u32_t tag : 8;
    u32_t type : _POLL_NUM_TYPES;
    u32_t state : _POLL_NUM_STATES;
    u32_t mode : 1;
    u32_t unused : _POLL_EVENT_NUM_UNUSED_BITS;
    union {
        void *obj;
        struct k_poll_signal *signal;
        struct k_sem *sem;
        struct k_fifo *fifo;
        struct k_queue *queue;
    };
};

struct k_poll_signal {
    sys_dlist_t poll_events;
    unsigned int signaled;
    int result;
};

#define K_POLL_STATE_NOT_READY           0
#define K_POLL_STATE_EADDRINUSE          1
#define K_POLL_STATE_SIGNALED            2
#define K_POLL_STATE_SEM_AVAILABLE       3
#define K_POLL_STATE_DATA_AVAILABLE      4
#define K_POLL_STATE_FIFO_DATA_AVAILABLE K_POLL_STATE_DATA_AVAILABLE

#define K_POLL_TYPE_IGNORE              0
#define K_POLL_TYPE_SIGNAL              1
#define K_POLL_TYPE_SEM_AVAILABLE       2
#define K_POLL_TYPE_DATA_AVAILABLE      3
#define K_POLL_TYPE_FIFO_DATA_AVAILABLE K_POLL_TYPE_DATA_AVAILABLE

#define K_POLL_EVENT_STATIC_INITIALIZER(event_type, event_mode, event_obj, \
                                        event_tag)                         \
    {                                                                      \
        .type = event_type,                                                \
        .tag = event_tag,                                                  \
        .state = K_POLL_STATE_NOT_READY,                                   \
        .mode = event_mode,                                                \
        .unused = 0,                                                       \
        { .obj = event_obj },                                              \
    }

extern int k_poll_signal_raise(struct k_poll_signal *signal, int result);
extern int k_poll(struct k_poll_event *events, int num_events, s32_t timeout);
extern void k_poll_event_init(struct k_poll_event *event, u32_t type, int mode, void *obj);

/* public - polling modes */
enum k_poll_modes {
    /* polling thread does not take ownership of objects when available */
    K_POLL_MODE_NOTIFY_ONLY = 0,

    K_POLL_NUM_MODES
};

#define k_oops()

//void k_sleep(s32_t duration);

#if defined(__cplusplus)
}
#endif

#endif /* ZEPHYR_H */
