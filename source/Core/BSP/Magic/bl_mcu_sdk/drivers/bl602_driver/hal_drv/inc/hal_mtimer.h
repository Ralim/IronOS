#ifndef __HAL_MTIMER__H__
#define __HAL_MTIMER__H__

#include "stdint.h"

void mtimer_set_alarm_time(uint64_t ticks, void (*interruptfun)(void));
uint32_t mtimer_get_clk_src_div(void);
uint64_t mtimer_get_time_ms();
uint64_t mtimer_get_time_us();
void mtimer_delay_ms(uint32_t time);
void mtimer_delay_us(uint32_t time);
#endif