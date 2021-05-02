/*
 * WS2812.cpp
 *
 *  Created on: 2 May 2021
 *      Author: Ralim
 */

#include "FreeRTOS.h"
#include "task.h"
#include <WS2812.h>
#include <string.h>
uint8_t           WS2812::leds_colors[WS2812_LED_CHANNEL_COUNT * WS2812_LED_COUNT];
volatile uint16_t WS2812::tmp_led_data[2 * WS2812_RAW_BYTES_PER_LED];
volatile uint8_t  WS2812::is_reset_pulse; /*!< Status if we are sending reset pulse or led data */
volatile uint8_t  WS2812::is_updating;    /*!< Is updating in progress? */
volatile uint32_t WS2812::current_led;    /*!< Current LED number we are sending */

void WS2812::init(void) {
  memset(leds_colors, 0, sizeof(leds_colors));
  hdma_tim1_ch1.XferHalfCpltCallback = DMAHalfComplete;
  hdma_tim1_ch1.XferCpltCallback     = DMAComplete;
  htim1.Instance->CCR1               = htim1.Instance->ARR / 2 - 1;
  htim1.Instance->DIER |= TIM_DIER_CC1DE;
}

uint8_t WS2812::led_update(uint8_t block) {
  if (is_updating) { /* Check if update in progress already */
    return 0;
  }
  is_updating = 1; /* We are now updating */

  led_start_reset_pulse(1); /* Start reset pulse */
  if (block) {
    while (!led_is_update_finished()) {
      vTaskDelay(1);
    }; /* Wait to finish */
  }
  return 1;
}

void WS2812::led_set_color(size_t index, uint8_t r, uint8_t g, uint8_t b) {
  leds_colors[index * WS2812_LED_CHANNEL_COUNT + 0] = r;
  leds_colors[index * WS2812_LED_CHANNEL_COUNT + 1] = g;
  leds_colors[index * WS2812_LED_CHANNEL_COUNT + 2] = b;
}

void WS2812::led_set_color_all(uint8_t r, uint8_t g, uint8_t b) {
  for (int index = 0; index < WS2812_LED_COUNT; index++) {
    leds_colors[index * WS2812_LED_CHANNEL_COUNT + 0] = r;
    leds_colors[index * WS2812_LED_CHANNEL_COUNT + 1] = g;
    leds_colors[index * WS2812_LED_CHANNEL_COUNT + 2] = b;
  }
}

uint8_t WS2812::led_is_update_finished(void) { return !is_updating; }

void WS2812::led_start_reset_pulse(uint8_t num) {
  is_reset_pulse = num; /* Set reset pulse flag */

  memset((void *)tmp_led_data, 0, sizeof(tmp_led_data)); /* Set all bytes to 0 to achieve 50us pulse */

  if (num == 1) {
    tmp_led_data[0] = (htim1.Instance->ARR * 2) / 3; // start with half width pulse
  }

  /* Set DMA to normal mode, set memory to beginning of data and length to 40 elements */
  /* 800kHz PWM x 40 samples = ~50us pulse low */
  hdma_tim1_ch1.Instance->CCR &= (~DMA_CCR_CIRC); // clear circular flag -> normal mode
  hdma_tim1_ch1.State = HAL_DMA_STATE_READY;
  HAL_DMA_Start_IT(&hdma_tim1_ch1, (uint32_t)tmp_led_data, (uint32_t)&htim1.Instance->CCR1, 2 * WS2812_RAW_BYTES_PER_LED);
  HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
}

void WS2812::DMAHalfComplete(DMA_HandleTypeDef *hdma) { led_update_sequence(0); }

void WS2812::DMAComplete(DMA_HandleTypeDef *hdma) { led_update_sequence(1); }

void WS2812::led_update_sequence(uint8_t tc) {
  tc = !!tc; /* Convert to 1 or 0 value only */

  /* Check for reset pulse at the end of PWM stream */
  if (is_reset_pulse == 2) { /* Check for reset pulse at the end */
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
    HAL_DMA_Abort(&hdma_tim1_ch1);
    is_updating = 0; /* We are not updating anymore */
    return;
  }

  /* Check for reset pulse on beginning of PWM stream */
  if (is_reset_pulse == 1) { /* Check if we finished with reset pulse */
    /*
     * When reset pulse is active, we have to wait full DMA response,
     * before we can start modifying array which is shared with DMA and PWM
     */
    if (!tc) { /* We must wait for transfer complete */
      return;  /* Return and wait to finish */
    }

    /* Disable timer output and disable DMA stream */
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
    HAL_DMA_Abort(&hdma_tim1_ch1);

    is_reset_pulse = 0; /* Not in reset pulse anymore */
    current_led    = 0; /* Reset current led */
  } else {
    /*
     * When we are not in reset mode,
     * go to next led and process data for it
     */
    current_led++; /* Go to next LED */
  }

  /*
   * This part is used to prepare data for "next" led,
   * for which update will start once current transfer stops in circular mode
   */
  if (current_led < WS2812_LED_COUNT) {
    /*
     * If we are preparing data for first time (current_led == 0)
     * or if there was no TC event (it was HT):
     *
     *  - Prepare first part of array, because either there is no transfer
     *      or second part (from HT to TC) is now in process for PWM transfer
     *
     * In other case (TC = 1)
     */
    if (current_led == 0 || !tc) {
      led_fill_led_pwm_data(current_led, &tmp_led_data[0]);
    } else {
      led_fill_led_pwm_data(current_led, &tmp_led_data[WS2812_RAW_BYTES_PER_LED]);
    }

    /*
     * If we are preparing first led (current_led = 0), then:
     *
     *  - We setup first part of array for first led,
     *  - We have to prepare second part for second led to have one led prepared in advance
     *  - Set DMA to circular mode and start the transfer + PWM output
     */
    if (current_led == 0) {

      current_led++;                                                               /* Go to next LED */
      led_fill_led_pwm_data(current_led, &tmp_led_data[WS2812_RAW_BYTES_PER_LED]); /* Prepare second LED too */
      hdma_tim1_ch1.Instance->CCR |= (DMA_CCR_CIRC);                               // set circular flag  for circular mode
      hdma_tim1_ch1.State = HAL_DMA_STATE_READY;
      HAL_DMA_Start_IT(&hdma_tim1_ch1, (uint32_t)tmp_led_data, (uint32_t)&htim1.Instance->CCR1, 2 * WS2812_RAW_BYTES_PER_LED);
      HAL_TIM_PWM_Start(&htim1, TIM_CHANNEL_1);
    }

    /*
     * When we reached all leds, we have to wait to transmit data for all leds before we can disable DMA and PWM:
     *
     *  - If TC event is enabled and we have EVEN number of LEDS (2, 4, 6, ...)
     *  - If HT event is enabled and we have ODD number of LEDS (1, 3, 5, ...)
     */
  } else if ((!tc && (WS2812_LED_COUNT & 0x01)) || (tc && !(WS2812_LED_COUNT & 0x01))) {
    HAL_TIM_PWM_Stop(&htim1, TIM_CHANNEL_1);
    HAL_DMA_Abort(&hdma_tim1_ch1);

    /* It is time to send final reset pulse, 50us at least */
    led_start_reset_pulse(2); /* Start reset pulse at the end */
  }
}

void WS2812::led_fill_led_pwm_data(size_t ledx, volatile uint16_t *ptr) {
  size_t   i;
  uint16_t OnOffValues[] = {2 * htim1.Instance->ARR / 3, (4 * htim1.Instance->ARR) / 3};

  if (ledx < WS2812_LED_COUNT) {
    for (i = 0; i < 8; i++) {
      // Also unmux RGB -> GRB in the index order here
      ptr[i]      = (leds_colors[WS2812_LED_CHANNEL_COUNT * ledx + 1] & (1 << (7 - i))) ? OnOffValues[1] : OnOffValues[0];
      ptr[8 + i]  = (leds_colors[WS2812_LED_CHANNEL_COUNT * ledx + 0] & (1 << (7 - i))) ? OnOffValues[1] : OnOffValues[0];
      ptr[16 + i] = (leds_colors[WS2812_LED_CHANNEL_COUNT * ledx + 2] & (1 << (7 - i))) ? OnOffValues[1] : OnOffValues[0];
#if WS2812_LED_CHANNEL_COUNT == 4
      ptr[24 + i] = (leds_colors[WS2812_LED_CHANNEL_COUNT * ledx + 3] & (1 << (7 - i))) ? OnOffValues[1] : OnOffValues[0];
#endif
    }
  } else {
    // Fill with zero?
  }
}
