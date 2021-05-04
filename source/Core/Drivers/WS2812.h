/*
 * WS2812.h
 *
 *  Created on: 2 May 2021
 *      Author: Ralim
 */
#include "Pins.h"
#include "Setup.h"
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#ifndef CORE_DRIVERS_WS2812_H_
#define CORE_DRIVERS_WS2812_H_

#ifndef WS2812_LED_CHANNEL_COUNT
#define WS2812_LED_CHANNEL_COUNT 3
#endif

#define WS2812_RAW_BYTES_PER_LED (WS2812_LED_CHANNEL_COUNT * 8)

template <uint32_t LED_GPIO, uint16_t LED_PIN, int LED_COUNT> class WS2812 {
private:
  uint8_t leds_colors[WS2812_LED_CHANNEL_COUNT * LED_COUNT];

public:
  void led_update() {
    __disable_irq();
    // Bitbang it out as our cpu irq latency is too high
    for (unsigned int i = 0; i < sizeof(leds_colors); i++) {
      // Shove out MSB first
      for (int x = 0; x < 8; x++) {
        ((GPIO_TypeDef *)WS2812_GPIO_Port)->BSRR = WS2812_Pin;
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
        ((GPIO_TypeDef *)WS2812_GPIO_Port)->BSRR = (uint32_t)WS2812_Pin << 16u;
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

  void init(void) { memset(leds_colors, 0, sizeof(leds_colors)); }

  void led_set_color(size_t index, uint8_t r, uint8_t g, uint8_t b) {
    leds_colors[index * WS2812_LED_CHANNEL_COUNT + 0] = g;
    leds_colors[index * WS2812_LED_CHANNEL_COUNT + 1] = r;
    leds_colors[index * WS2812_LED_CHANNEL_COUNT + 2] = b;
  }

  void led_set_color_all(uint8_t r, uint8_t g, uint8_t b) {
    for (int index = 0; index < LED_COUNT; index++) {
      leds_colors[index * WS2812_LED_CHANNEL_COUNT + 0] = g;
      leds_colors[index * WS2812_LED_CHANNEL_COUNT + 1] = r;
      leds_colors[index * WS2812_LED_CHANNEL_COUNT + 2] = b;
    }
  }
};

#endif /* CORE_DRIVERS_WS2812_H_ */
