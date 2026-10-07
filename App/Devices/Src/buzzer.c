/**
 * @file App/Devices/Src/buzzer.c
 * @authors Michael Zheng
 * @brief Piezo buzzer driver
 */

/**
 * Notes: 
 * - PS1240P02BT buzzer, with optimal input frequency of 4 kHz, and should have a 50% duty cycle
 * - Needs to be a non-blocking function, so TIM2 directly drives the buzzer pin
 * - In CubeMX, the output pin is PA0 (TIM2_CH1)

#include "buzzer.h"
#include "pinout.h"
#include "tim.h"

/**************************************************************************************************
 * @section Definitions and global variables
 **************************************************************************************************/

#define BUZZER_TIMER htim2
#define BUZZER_CHANNEL TIM_CHANNEL_1 // Hardcoded for buzzer to be wired to PA0
#define TIMER_CLOCK_HZ 1000000

/**************************************************************************************************
 * @section Public function definitions
 **************************************************************************************************/

void buzzer_on(void) {
    HAL_TIM_PWM_Start(&BUZZER_TIMER, BUZZER_CHANNEL);
}

void buzzer_set_frequency(uint32_t frequency_hz) {
    uint32_t period = TIMER_CLOCK_HZ / frequency_hz;

    __HAL_TIM_SET_AUTORELOAD(&BUZZER_TIMER, period - 1); // Sets ARR register to count to specified number of ticks (controls frequency)
    __HAL_TIM_SET_COMPARE(&BUZZER_TIMER, BUZZER_CHANNEL, period / 2); // Target 50% duty cycle
}

void buzzer_off(void) {
    HAL_TIM_PWM_Stop(&BUZZER_TIMER, BUZZER_CHANNEL);
}