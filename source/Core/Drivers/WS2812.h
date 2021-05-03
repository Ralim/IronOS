/*
 * WS2812.h
 *
 *  Created on: 2 May 2021
 *      Author: Ralim
 */
#include "Setup.h"
#include <stddef.h>
#include <stdint.h>

#ifndef CORE_DRIVERS_WS2812_H_
#define CORE_DRIVERS_WS2812_H_
#ifndef WS2812_LED_COUNT
#define WS2812_LED_COUNT 2
#endif
#ifndef WS2812_LED_CHANNEL_COUNT
#define WS2812_LED_CHANNEL_COUNT 3
#endif
#define WS2812_RAW_BYTES_PER_LED (WS2812_LED_CHANNEL_COUNT * 8)
class WS2812 {
public:
	static void init(void);
	static void  led_update();
	static void led_set_color(size_t index, uint8_t r, uint8_t g, uint8_t b);
	static void led_set_color_all(uint8_t r, uint8_t g, uint8_t b);

private:

	static uint8_t leds_colors[WS2812_LED_CHANNEL_COUNT * WS2812_LED_COUNT];
};

#endif /* CORE_DRIVERS_WS2812_H_ */
