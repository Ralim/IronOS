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
#define WS2812_LED_COUNT 4
#endif
#ifndef WS2812_LED_CHANNEL_COUNT
#define WS2812_LED_CHANNEL_COUNT 3
#endif
#define WS2812_RAW_BYTES_PER_LED (WS2812_LED_CHANNEL_COUNT * 8)
class WS2812 {
public:
	static void init(void);
	static uint8_t led_update(uint8_t block);
	static void led_set_color(size_t index, uint8_t r, uint8_t g, uint8_t b);
	static void led_set_color_all(uint8_t r, uint8_t g, uint8_t b);

private:
	static uint8_t led_is_update_finished(void);
	static void led_start_reset_pulse(uint8_t num);
	static void DMAHalfComplete(DMA_HandleTypeDef *hdma);
	static void DMAComplete(DMA_HandleTypeDef *hdma);
	static void led_update_sequence(uint8_t tc);
	static void led_fill_led_pwm_data(size_t ledx, volatile  uint16_t *ptr);
	static uint8_t leds_colors[WS2812_LED_CHANNEL_COUNT * WS2812_LED_COUNT];
	static volatile uint16_t tmp_led_data[2 * WS2812_RAW_BYTES_PER_LED];
	static volatile uint8_t is_reset_pulse; /*!< Status if we are sending reset pulse or led data */
	static volatile uint8_t is_updating; /*!< Is updating in progress? */
	static volatile uint32_t current_led; /*!< Current LED number we are sending */
};

#endif /* CORE_DRIVERS_WS2812_H_ */
