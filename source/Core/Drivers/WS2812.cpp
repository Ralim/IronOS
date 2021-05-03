/*
 * WS2812.cpp
 *
 *  Created on: 2 May 2021
 *      Author: Ralim
 */

#include <WS2812.h>
#include "Pins.h"
#include <string.h>
uint8_t WS2812::leds_colors[WS2812_LED_CHANNEL_COUNT * WS2812_LED_COUNT];

void WS2812::init(void) {
	memset(leds_colors, 0, sizeof(leds_colors));

}

void WS2812::led_update() {
	__disable_irq();
//Bitbang it out as our cpu irq latency is too high
	for (unsigned int i = 0; i < sizeof(leds_colors); i++) {
		//Shove out MSB first
		for (int x = 0; x < 8; x++) {
			WS2812_GPIO_Port->BSRR = WS2812_Pin;
			if ((leds_colors[i] & (1 << (7 - x))) == (1 << (7 - x))) {
				__asm__ __volatile__("nop");
				__asm__ __volatile__("nop");
				__asm__ __volatile__("nop");
				__asm__ __volatile__("nop");
				__asm__ __volatile__("nop");
				__asm__ __volatile__("nop");
				__asm__ __volatile__("nop");
				__asm__ __volatile__("nop");
				__asm__ __volatile__("nop");
				__asm__ __volatile__("nop");
				__asm__ __volatile__("nop");
				__asm__ __volatile__("nop");
				__asm__ __volatile__("nop");
				__asm__ __volatile__("nop");
				__asm__ __volatile__("nop");
				__asm__ __volatile__("nop");
				__asm__ __volatile__("nop");
				__asm__ __volatile__("nop");
				__asm__ __volatile__("nop");
				__asm__ __volatile__("nop");
				__asm__ __volatile__("nop");
				__asm__ __volatile__("nop");
				__asm__ __volatile__("nop");
				__asm__ __volatile__("nop");
				__asm__ __volatile__("nop");
				__asm__ __volatile__("nop");
				__asm__ __volatile__("nop");
				__asm__ __volatile__("nop");
				__asm__ __volatile__("nop");
				__asm__ __volatile__("nop");
			} else {

				__asm__ __volatile__("nop");
				__asm__ __volatile__("nop");
				__asm__ __volatile__("nop");
				__asm__ __volatile__("nop");
				__asm__ __volatile__("nop");
				__asm__ __volatile__("nop");
			}
			WS2812_GPIO_Port->BSRR = (uint32_t) WS2812_Pin << 16u;
			__asm__ __volatile__("nop");
			__asm__ __volatile__("nop");
			__asm__ __volatile__("nop");
			__asm__ __volatile__("nop");
			__asm__ __volatile__("nop");
			__asm__ __volatile__("nop");
			__asm__ __volatile__("nop");
			__asm__ __volatile__("nop");
			__asm__ __volatile__("nop");
			__asm__ __volatile__("nop");
			__asm__ __volatile__("nop");
			__asm__ __volatile__("nop");
			__asm__ __volatile__("nop");
			__asm__ __volatile__("nop");
			__asm__ __volatile__("nop");
			__asm__ __volatile__("nop");
			__asm__ __volatile__("nop");
			__asm__ __volatile__("nop");
			__asm__ __volatile__("nop");
			__asm__ __volatile__("nop");
			__asm__ __volatile__("nop");
			__asm__ __volatile__("nop");
			__asm__ __volatile__("nop");
			__asm__ __volatile__("nop");
			__asm__ __volatile__("nop");
			__asm__ __volatile__("nop");
			__asm__ __volatile__("nop");
		}
	}
	__enable_irq();
}

void WS2812::led_set_color(size_t index, uint8_t r, uint8_t g, uint8_t b) {
	leds_colors[index * WS2812_LED_CHANNEL_COUNT + 0] = g;
	leds_colors[index * WS2812_LED_CHANNEL_COUNT + 1] = r;
	leds_colors[index * WS2812_LED_CHANNEL_COUNT + 2] = b;
}

void WS2812::led_set_color_all(uint8_t r, uint8_t g, uint8_t b) {
	for (int index = 0; index < WS2812_LED_COUNT; index++) {
		leds_colors[index * WS2812_LED_CHANNEL_COUNT + 0] = g;
		leds_colors[index * WS2812_LED_CHANNEL_COUNT + 1] = r;
		leds_colors[index * WS2812_LED_CHANNEL_COUNT + 2] = b;
	}
}
