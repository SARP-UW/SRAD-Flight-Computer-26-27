/**
 * @file App/Devices/Inc/buzzer.h
 * @authors Michael Zheng
 * @brief Piezo buzzer driver
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