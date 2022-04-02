#ifndef BL_PORT_H
#define BL_PORT_H
#if defined(BL_MCU_SDK)
#include "misc.h"
#endif
#include "config.h"
#include <misc/dlist.h>
#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "types.h"
#include "bl_port.h"

#define BT_UINT_MAX     0xffffffff
#define BL_WAIT_FOREVER 0xffffffffu
#define BL_NO_WAIT      0x0
#define ms2tick         pdMS_TO_TICKS

typedef struct {
    void *hdl;
} bl_hdl_t;

typedef bl_hdl_t _queue_t;
typedef bl_hdl_t _sem_t;
typedef uint32_t _stack_element_t;
typedef bl_hdl_t _mutex_t;
typedef bl_hdl_t bl_timer_t;
typedef uint32_t _task_t;

#define _POLL_EVENT_OBJ_INIT(obj) \
    .poll_events = SYS_DLIST_STATIC_INIT(&obj.poll_events),
#define _POLL_EVENT sys_dlist_t poll_events

#define _K_SEM_INITIALIZER(obj, initial_count, count_limit) \
    {                                                       \
    }

#define K_SEM_INITIALIZER DEPRECATED_MACRO _K_SEM_INITIALIZER

#define K_SEM_DEFINE(name, initial_count, count_limit) \
    struct k_sem name                                  \
        __in_section(_k_sem, static, name) =           \
            _K_SEM_INITIALIZER(name, initial_count, count_limit)

#define K_MUTEX_DEFINE(name)                   \
    struct k_mutex name                        \
        __in_section(_k_mutex, static, name) = \
            _K_MUTEX_INITIALIZER(name)

typedef sys_dlist_t _wait_q_t;

struct k_queue {
    void *hdl;
    sys_dlist_t poll_events;
};

/*attention: this is intialied as zero,the queue variable shoule use k_queue_init\k_lifo_init\k_fifo_init again*/
#define _K_QUEUE_INITIALIZER(obj) \
    {                             \
        0                         \
    }
#define K_QUEUE_INITIALIZER DEPRECATED_MACRO _K_QUEUE_INITIALIZER

void k_queue_init(struct k_queue *queue, int size);
void k_queue_free(struct k_queue *queue);
void k_queue_append(struct k_queue *queue, void *data);
void k_queue_prepend(struct k_queue *queue, void *data);
void k_queue_insert(struct k_queue *queue, void *prev, void *data);
void k_queue_append_list(struct k_queue *queue, void *head, void *tail);
void *k_queue_get(struct k_queue *queue, s32_t timeout);
int k_queue_is_empty(struct k_queue *queue);
int k_queue_get_cnt(struct k_queue *queue);

struct k_lifo {
    struct k_queue _queue;
};

#define _K_LIFO_INITIALIZER(obj)                   \
    {                                              \
        ._queue = _K_QUEUE_INITIALIZER(obj._queue) \
    }

#define K_LIFO_INITIALIZER DEPRECATED_MACRO _K_LIFO_INITIALIZER

#define k_lifo_init(lifo, size) \
    k_queue_init((struct k_queue *)lifo, size)

#define k_lifo_put(lifo, data) \
    k_queue_prepend((struct k_queue *)lifo, data)

#define k_lifo_get(lifo, timeout) \
    k_queue_get((struct k_queue *)lifo, timeout)

#define K_LIFO_DEFINE(name)                    \
    struct k_lifo name                         \
        __in_section(_k_queue, static, name) = \
            _K_LIFO_INITIALIZER(name)

struct k_fifo {
    struct k_queue _queue;
};

#define _K_FIFO_INITIALIZER(obj)                   \
    {                                              \
        ._queue = _K_QUEUE_INITIALIZER(obj._queue) \
    }
#define K_FIFO_INITIALIZER DEPRECATED_MACRO _K_FIFO_INITIALIZER

#define k_fifo_init(fifo, size) \
    k_queue_init((struct k_queue *)fifo, size)

#define k_fifo_put(fifo, data) \
    k_queue_append((struct k_queue *)fifo, data)

#define k_fifo_put_from_isr(fifo, data) \
    k_queue_append_from_isr((struct k_queue *)fifo, data)

#define k_fifo_put_list(fifo, head, tail) \
    k_queue_append_list((struct k_queue *)fifo, head, tail)

#define k_fifo_get(fifo, timeout) \
    k_queue_get((struct k_queue *)fifo, timeout)

#define K_FIFO_DEFINE(name)                    \
    struct k_fifo name                         \
        __in_section(_k_queue, static, name) = \
            _K_FIFO_INITIALIZER(name)

/* sem define*/
struct k_sem {
    _sem_t sem;
    sys_dlist_t poll_events;
};

/**
 * @brief Initialize a semaphore.
 */
int k_sem_init(struct k_sem *sem, unsigned int initial_count, unsigned int limit);

/**
 * @brief Take a semaphore.
 */
int k_sem_take(struct k_sem *sem, uint32_t timeout);

/**
 * @brief Give a semaphore.
 */
int k_sem_give(struct k_sem *sem);

/**
 * @brief Delete a semaphore.
 */
int k_sem_delete(struct k_sem *sem);

/**
 * @brief Get a semaphore's count.
 */
unsigned int k_sem_count_get(struct k_sem *sem);

struct k_mutex {
    _mutex_t mutex;
    sys_dlist_t poll_events;
};

typedef void (*k_timer_handler_t)(void *timer);

typedef struct k_timer {
    bl_timer_t timer;
    k_timer_handler_t handler;
    void *args;
    uint32_t timeout;
    uint32_t start_ms;
} k_timer_t;

/**
 * @brief Initialize a timer.
 */
void k_timer_init(k_timer_t *timer, k_timer_handler_t handle, void *args);

void *k_timer_get_id(void *hdl);

/**
 * @brief Start a timer.
 *
 */
void k_timer_start(k_timer_t *timer, uint32_t timeout);

void k_timer_reset(k_timer_t *timer);

/**
 * @brief Stop a timer.
 */
void k_timer_stop(k_timer_t *timer);

/**
 * @brief Delete a timer.
 *
 */
void k_timer_delete(k_timer_t *timer);

/*time define*/
#define MSEC_PER_SEC 1000
#define K_MSEC(ms)   (ms)
#define K_SECONDS(s) K_MSEC((s)*MSEC_PER_SEC)
#define K_MINUTES(m) K_SECONDS((m)*60)
#define K_HOURS(h)   K_MINUTES((h)*60)

#define K_PRIO_COOP(x) x

/**
 * @brief Get time now.
 *
 * @return time(in milliseconds)
 */
int64_t k_uptime_get();
u32_t k_uptime_get_32(void);

struct k_thread {
    _task_t task;
};

typedef _stack_element_t k_thread_stack_t;

inline void k_call_stacks_analyze(void)
{
}

#define K_THREAD_STACK_DEFINE(sym, size) _stack_element_t sym[size]
#define K_THREAD_STACK_SIZEOF(sym)       sizeof(sym)

static inline char *K_THREAD_STACK_BUFFER(k_thread_stack_t *sym)
{
    return (char *)sym;
}

typedef void (*k_thread_entry_t)(void *args);

int k_thread_create(struct k_thread *new_thread, const char *name,
                    size_t stack_size, k_thread_entry_t entry,
                    int prio);

void k_thread_delete(struct k_thread *new_thread);

/**
 * @brief Yield the current thread.
 */
int k_yield();

/**
 * @brief suspend the current thread for a certain time
 */

void k_sleep(s32_t duration);

/**
 * @brief Lock interrupts.
 */
unsigned int irq_lock();

/**
 * @brief Unlock interrupts.
 */
void irq_unlock(unsigned int key);

int k_is_in_isr(void);

#ifdef BIT
#undef BIT
#define BIT(n) (1UL << (n))
#else
#define BIT(n) (1UL << (n))
#endif

long long k_now_ms(void);
void k_get_random_byte_array(uint8_t *buf, size_t len);
void *k_malloc(size_t size);
void k_free(void *buf);
#endif /* BL_PORT_H */
