#include <zephyr.h>
#include <misc/util.h>
#include <misc/dlist.h>

#define BT_DBG_ENABLED IS_ENABLED(CONFIG_BLUETOOTH_DEBUG_CORE)

#include <log.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include "atomic.h"

#include "errno.h"
#include <FreeRTOS.h>
#include <task.h>
#include <semphr.h>
#include <timers.h>
#include <stdlib.h>

#if defined(BL_MCU_SDK)
#define TRNG_LOOP_COUNTER (17)
extern BL_Err_Type Sec_Eng_Trng_Get_Random(uint8_t *data, uint32_t len);
extern BL_Err_Type Sec_Eng_Trng_Enable(void);
int bl_rand();
#else
extern int bl_rand();
#endif

int ble_rand()
{
#if defined(CONFIG_HW_SEC_ENG_DISABLE)
    extern long random(void);
    return random();
#else
    return bl_rand();
#endif
}

#if defined(BL_MCU_SDK)
int bl_rand()
{
    unsigned int val;
    int counter = 0;
    int32_t ret = 0;
    do {
        ret = Sec_Eng_Trng_Get_Random((uint8_t *)&val, 4);
        if (ret < -1) {
            return -1;
        }
        if ((counter++) > TRNG_LOOP_COUNTER) {
            break;
        }
    } while (0 == val);
    val >>= 1; //leave signe bit alone
    return val;
}
#endif

void k_queue_init(struct k_queue *queue, int size)
{
    //int size = 20;
    uint8_t blk_size = sizeof(void *) + 1;

    queue->hdl = xQueueCreate(size, blk_size);
    BT_ASSERT(queue->hdl != NULL);

    sys_dlist_init(&queue->poll_events);
}

void k_queue_insert(struct k_queue *queue, void *prev, void *data)
{
    BaseType_t ret;
    (void)ret;

    ret = xQueueSend(queue->hdl, &data, portMAX_DELAY);
    BT_ASSERT(ret == pdPASS);
}

void k_queue_append(struct k_queue *queue, void *data)
{
    k_queue_insert(queue, NULL, data);
}

void k_queue_insert_from_isr(struct k_queue *queue, void *prev, void *data)
{
    BaseType_t xHigherPriorityTaskWoken;

    xQueueSendFromISR(queue->hdl, &data, &xHigherPriorityTaskWoken);
    if (xHigherPriorityTaskWoken == pdTRUE) {
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

void k_queue_append_from_isr(struct k_queue *queue, void *data)
{
    k_queue_insert_from_isr(queue, NULL, data);
}

void k_queue_free(struct k_queue *queue)
{
    if (NULL == queue || NULL == queue->hdl) {
        BT_ERR("Queue is NULL\n");
        return;
    }

    vQueueDelete(queue->hdl);
    queue->hdl = NULL;
    return;
}

void k_queue_prepend(struct k_queue *queue, void *data)
{
    k_queue_insert(queue, NULL, data);
}

void k_queue_append_list(struct k_queue *queue, void *head, void *tail)
{
    struct net_buf *buf_tail = (struct net_buf *)head;

    for (buf_tail = (struct net_buf *)head; buf_tail; buf_tail = buf_tail->frags) {
        k_queue_append(queue, buf_tail);
    }
}

void *k_queue_get(struct k_queue *queue, s32_t timeout)
{
    void *msg = NULL;
    unsigned int t = timeout;
    BaseType_t ret;

    (void)ret;

    if (timeout == K_FOREVER) {
        t = BL_WAIT_FOREVER;
    } else if (timeout == K_NO_WAIT) {
        t = BL_NO_WAIT;
    }

    ret = xQueueReceive(queue->hdl, &msg, t == BL_WAIT_FOREVER ? portMAX_DELAY : ms2tick(t));
    if (ret == pdPASS) {
        return msg;
    } else {
        return NULL;
    }
}

int k_queue_is_empty(struct k_queue *queue)
{
    return uxQueueMessagesWaiting(queue->hdl) ? 0 : 1;
}

int k_queue_get_cnt(struct k_queue *queue)
{
    return uxQueueMessagesWaiting(queue->hdl);
}

int k_sem_init(struct k_sem *sem, unsigned int initial_count, unsigned int limit)
{
    if (NULL == sem) {
        BT_ERR("sem is NULL\n");
        return -EINVAL;
    }

    sem->sem.hdl = xSemaphoreCreateCounting(limit, initial_count);
    sys_dlist_init(&sem->poll_events);
    return 0;
}

int k_sem_take(struct k_sem *sem, uint32_t timeout)
{
    BaseType_t ret;
    unsigned int t = timeout;

    (void)ret;
    if (timeout == K_FOREVER) {
        t = BL_WAIT_FOREVER;
    } else if (timeout == K_NO_WAIT) {
        t = BL_NO_WAIT;
    }

    if (NULL == sem) {
        return -1;
    }

    ret = xSemaphoreTake(sem->sem.hdl, t == BL_WAIT_FOREVER ? portMAX_DELAY : ms2tick(t));
    return ret == pdPASS ? 0 : -1;
}

int k_sem_give(struct k_sem *sem)
{
    BaseType_t ret;
    (void)ret;

    if (NULL == sem) {
        BT_ERR("sem is NULL\n");
        return -EINVAL;
    }

    ret = xSemaphoreGive(sem->sem.hdl);
    return ret == pdPASS ? 0 : -1;
}

int k_sem_delete(struct k_sem *sem)
{
    if (NULL == sem || NULL == sem->sem.hdl) {
        BT_ERR("sem is NULL\n");
        return -EINVAL;
    }

    vSemaphoreDelete(sem->sem.hdl);
    sem->sem.hdl = NULL;
    return 0;
}

unsigned int k_sem_count_get(struct k_sem *sem)
{
    return uxQueueMessagesWaiting(sem->sem.hdl);
}

void k_mutex_init(struct k_mutex *mutex)
{
    if (NULL == mutex) {
        BT_ERR("mutex is NULL\n");
        return;
    }

    mutex->mutex.hdl = xSemaphoreCreateMutex();
    BT_ASSERT(mutex->mutex.hdl != NULL);
    sys_dlist_init(&mutex->poll_events);
}

int64_t k_uptime_get()
{
    return k_now_ms();
}

u32_t k_uptime_get_32(void)
{
    return (u32_t)k_now_ms();
}

int k_thread_create(struct k_thread *new_thread, const char *name,
                    size_t stack_size, k_thread_entry_t entry,
                    int prio)
{
    stack_size /= sizeof(StackType_t);
    xTaskCreate(entry, name, stack_size, NULL, prio, (void *)(&new_thread->task));

    return new_thread->task ? 0 : -1;
}

void k_thread_delete(struct k_thread *new_thread)
{
    if (NULL == new_thread || 0 == new_thread->task) {
        BT_ERR("task is NULL\n");
        return;
    }

    vTaskDelete((void *)(new_thread->task));
    new_thread->task = 0;
    return;
}

int k_yield(void)
{
    taskYIELD();
    return 0;
}

void k_sleep(s32_t dur_ms)
{
    TickType_t ticks;
    ticks = pdMS_TO_TICKS(dur_ms);
    vTaskDelay(ticks);
}

unsigned int irq_lock(void)
{
    taskENTER_CRITICAL();
    return 1;
}

void irq_unlock(unsigned int key)
{
    taskEXIT_CRITICAL();
}

int k_is_in_isr(void)
{
#if defined(ARCH_RISCV)
    return (xPortIsInsideInterrupt());
#else
    /* IRQs + PendSV (14) + SYSTICK (15) are interrupts. */
    return (__get_IPSR() > 13);
#endif

    return 0;
}

void k_timer_init(k_timer_t *timer, k_timer_handler_t handle, void *args)
{
    BT_ASSERT(timer != NULL);
    timer->handler = handle;
    timer->args = args;
    /* Set args as timer id */
    timer->timer.hdl = xTimerCreate("Timer", pdMS_TO_TICKS(1000), 0, args, (TimerCallbackFunction_t)(timer->handler));
    BT_ASSERT(timer->timer.hdl != NULL);
}

void *k_timer_get_id(void *hdl)
{
    return pvTimerGetTimerID((TimerHandle_t)hdl);
}

void k_timer_start(k_timer_t *timer, uint32_t timeout)
{
    BaseType_t ret;
    (void)ret;

    BT_ASSERT(timer != NULL);
    timer->timeout = timeout;
    timer->start_ms = k_now_ms();

    ret = xTimerChangePeriod(timer->timer.hdl, pdMS_TO_TICKS(timeout), 0);
    BT_ASSERT(ret == pdPASS);
    ret = xTimerStart(timer->timer.hdl, 0);
    BT_ASSERT(ret == pdPASS);
}

void k_timer_reset(k_timer_t *timer)
{
    BaseType_t ret;

    (void)ret;
    BT_ASSERT(timer != NULL);

    ret = xTimerReset(timer->timer.hdl, 0);
    BT_ASSERT(ret == pdPASS);
}

void k_timer_stop(k_timer_t *timer)
{
    BaseType_t ret;

    (void)ret;
    BT_ASSERT(timer != NULL);

    ret = xTimerStop(timer->timer.hdl, 0);
    BT_ASSERT(ret == pdPASS);
}

void k_timer_delete(k_timer_t *timer)
{
    BaseType_t ret;
    (void)ret;

    BT_ASSERT(timer != NULL);

    ret = xTimerDelete(timer->timer.hdl, 0);
    BT_ASSERT(ret == pdPASS);
}

long long k_now_ms(void)
{
    return (long long)(xTaskGetTickCount() * 1000) / configTICK_RATE_HZ;
}

void k_get_random_byte_array(uint8_t *buf, size_t len)
{
    // bl_rand() return a word, but *buf may not be word-aligned
    for (int i = 0; i < len; i++) {
        *(buf + i) = (uint8_t)(ble_rand() & 0xFF);
    }
}

void *k_malloc(size_t size)
{
    return pvPortMalloc(size);
}

void k_free(void *buf)
{
    return vPortFree(buf);
}
