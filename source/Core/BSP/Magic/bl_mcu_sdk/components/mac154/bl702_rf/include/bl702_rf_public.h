#ifndef _BL702_RF_PUBLIC_H_
#define _BL702_RF_PUBLIC_H_


#include <stdint.h>


uint32_t rf702_set_init_tsen_value(int16_t tsen_value);
uint32_t rf702_inc_cal_tsen_based(int16_t tsen_value);

// callback function, should be implemented by user
void rf_reset_done_callback(void);


#endif
