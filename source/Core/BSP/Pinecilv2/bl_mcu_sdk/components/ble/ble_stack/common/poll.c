/*
 * Copyright (c) 2017 Wind River Systems, Inc.
 *
 * SPDX-License-Identifier: Apache-2.0
 */

/**
 * @file
 *
 * @brief Kernel asynchronous event polling interface.
 *
 * This polling mechanism allows waiting on multiple events concurrently,
 * either events triggered directly, or from kernel objects or other kernel
 * constructs.
 */

#include <stdio.h>

#include <zephyr.h>
#include <zephyr/types.h>
#include <misc/slist.h>
#include <misc/dlist.h>
#include <misc/__assert.h>

struct k_sem g_poll_sem;

void k_poll_event_init(struct k_poll_event *event, u32_t type,
                       int mode, void *obj)
{
    __ASSERT(mode == K_POLL_MODE_NOTIFY_ONLY,
             "only NOTIFY_ONLY mode is supported\n");
    __ASSERT(type < (1 << _POLL_NUM_TYPES), "invalid type\n");
    __ASSERT(obj, "must provide an object\n");

    event->poller = NULL;
    /* event->tag is left uninitialized: the user will set it if needed */
    event->type = type;
    event->state = K_POLL_STATE_NOT_READY;
    event->mode = mode;
    event->unused = 0;
    event->obj = obj;
}

/* must be called with interrupts locked */
static inline int is_condition_met(struct k_poll_event *event, u32_t *state)
{
    switch (event->type) {
        case K_POLL_TYPE_SEM_AVAILABLE:
            if (k_sem_count_get(event->sem) > 0) {
                *state = K_POLL_STATE_SEM_AVAILABLE;
                return 1;
            }
            break;
        case K_POLL_TYPE_DATA_AVAILABLE:
            if (!k_queue_is_empty(event->queue)) {
                *state = K_POLL_STATE_FIFO_DATA_AVAILABLE;
                return 1;
            }
            break;
        case K_POLL_TYPE_SIGNAL:
            if (event->signal->signaled) {
                *state = K_POLL_STATE_SIGNALED;
                return 1;
            }
            break;
        case K_POLL_TYPE_IGNORE:
            return 0;
        default:
            __ASSERT(0, "invalid event type (0x%x)\n", event->type);
            break;
    }

    return 0;
}

static inline void add_event(sys_dlist_t *events, struct k_poll_event *event,
                             struct _poller *poller)
{
    sys_dlist_append(events, &event->_node);
}

/* must be called with interrupts locked */
static inline int register_event(struct k_poll_event *event,
                                 struct _poller *poller)
{
    switch (event->type) {
        case K_POLL_TYPE_SEM_AVAILABLE:
            __ASSERT(event->sem, "invalid semaphore\n");
            add_event(&event->sem->poll_events, event, poller);
            break;
        case K_POLL_TYPE_DATA_AVAILABLE:
            __ASSERT(event->queue, "invalid queue\n");
            add_event(&event->queue->poll_events, event, poller);
            break;
        case K_POLL_TYPE_SIGNAL:
            __ASSERT(event->signal, "invalid poll signal\n");
            add_event(&event->signal->poll_events, event, poller);
            break;
        case K_POLL_TYPE_IGNORE:
            /* nothing to do */
            break;
        default:
            __ASSERT(0, "invalid event type\n");
            break;
    }

    event->poller = poller;

    return 0;
}

/* must be called with interrupts locked */
static inline void clear_event_registration(struct k_poll_event *event)
{
    event->poller = NULL;

    switch (event->type) {
        case K_POLL_TYPE_SEM_AVAILABLE:
            __ASSERT(event->sem, "invalid semaphore\n");
            sys_dlist_remove(&event->_node);
            break;
        case K_POLL_TYPE_DATA_AVAILABLE:
            __ASSERT(event->queue, "invalid queue\n");
            sys_dlist_remove(&event->_node);
            break;
        case K_POLL_TYPE_SIGNAL:
            __ASSERT(event->signal, "invalid poll signal\n");
            sys_dlist_remove(&event->_node);
            break;
        case K_POLL_TYPE_IGNORE:
            /* nothing to do */
            break;
        default:
            __ASSERT(0, "invalid event type\n");
            break;
    }
}

/* must be called with interrupts locked */
static inline void clear_event_registrations(struct k_poll_event *events,
                                             int last_registered,
                                             unsigned int key)
{
    for (; last_registered >= 0; last_registered--) {
        clear_event_registration(&events[last_registered]);
        irq_unlock(key);
        key = irq_lock();
    }
}

static inline void set_event_ready(struct k_poll_event *event, u32_t state)
{
    event->poller = NULL;
    event->state |= state;
}

static bool polling_events(struct k_poll_event *events, int num_events,
                           s32_t timeout, int *last_registered)
{
    int rc;
    bool polling = true;
    unsigned int key;

    for (int ii = 0; ii < num_events; ii++) {
        u32_t state;
        key = irq_lock();
        if (is_condition_met(&events[ii], &state)) {
            set_event_ready(&events[ii], state);
            polling = false;
        } else if (timeout != K_NO_WAIT && polling) {
            rc = register_event(&events[ii], NULL);
            if (rc == 0) {
                ++(*last_registered);
            } else {
                __ASSERT(0, "unexpected return code\n");
            }
        }
        irq_unlock(key);
    }
    return polling;
}

int k_poll(struct k_poll_event *events, int num_events, s32_t timeout)
{
    __ASSERT(events, "NULL events\n");
    __ASSERT(num_events > 0, "zero events\n");

    int last_registered = -1;
    unsigned int key;
    bool polling = true;

    /* find events whose condition is already fulfilled */
    polling = polling_events(events, num_events, timeout, &last_registered);

    if (polling == false) {
        goto exit;
    }

    k_sem_take(&g_poll_sem, timeout);

    last_registered = -1;
    polling_events(events, num_events, timeout, &last_registered);
exit:
    key = irq_lock();
    clear_event_registrations(events, last_registered, key);
    irq_unlock(key);
    return 0;
}

/* must be called with interrupts locked */
static int _signal_poll_event(struct k_poll_event *event, u32_t state,
                              int *must_reschedule)
{
    *must_reschedule = 0;
    set_event_ready(event, state);
    return 0;
}

int k_poll_signal_raise(struct k_poll_signal *signal, int result)
{
    unsigned int key = irq_lock();
    struct k_poll_event *poll_event;
    int must_reschedule;

    signal->result = result;
    signal->signaled = 1;

    poll_event = (struct k_poll_event *)sys_dlist_get(&signal->poll_events);
    if (!poll_event) {
        irq_unlock(key);
        return 0;
    }

    int rc = _signal_poll_event(poll_event, K_POLL_STATE_SIGNALED,
                                &must_reschedule);

    k_sem_give(&g_poll_sem);
    irq_unlock(key);
    return rc;
}
