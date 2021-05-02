#include "BSP.h"
#include "FreeRTOS.h"
#include "I2C_Wrapper.hpp"
#include "QC3.h"
#include "Settings.h"
#include "WS2812.h"
#include "cmsis_os.h"
#include "fusbpd.h"
#include "main.hpp"
#include "power.hpp"
#include "stdlib.h"
#include "task.h"

// Initialisation to be performed with scheduler active
void postRToSInit() {
	WS2812::init();
	WS2812::led_set_color(0, 0xAA, 0x55, 0x00);
	while (true) {
//		osDelay(1);
//		WS2812::led_set_color(0, 0xFF, 0xFF, 0xFF);
//		WS2812::led_update(1);
		osDelay(10);
		WS2812::led_update(1);
	}
}
