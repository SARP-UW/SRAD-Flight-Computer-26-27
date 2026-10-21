/**
 * @file App/Devices/Inc/buzzer.h
 * @authors Michael Zheng
 * @brief Piezo buzzer driver
 */

/**
 * Notes:
 * - The buzzer is loudest at 4800 Hz, quietest at 3100 Hz
 * - Try to keep our output range within 4000 Hz to 5000 Hz if possible so we can hear it clearest
 * - The jump from 4000 Hz to 5000 Hz is roughly from a C note to E note
 * source: page 4, https://product.tdk.com/en/system/files/dam/doc/product/sw_piezo/sw_piezo/piezo-buzzer/catalog/piezoelectronic_buzzer_ps_en.pdf
 */

#pragma once

#include <stdint.h>
#include "stm32f4xx_hal.h"

/**
 * @brief Initializes the buzzer PWM output.
 */
// void init_buzzer(void);

/**
 * @brief Plays the buzzer tone indefinitely.
 */
void buzzer_on(void);

/**
 * @brief Sets the buzzer tone frequency in Hz.
 */
void buzzer_set_frequency(uint32_t frequency_hz);

/**
 * @brief Stops the buzzer tone.
 */
void buzzer_off(void);