/**
 * @file App/Devices/Src/buzzer.c
 * @authors Michael Zheng & Shivani Palanikumar
 * @brief Piezo buzzer driver
 */

/**
 * Notes: 
 * - PS1240P02BT buzzer, with optimal input frequency of 4 kHz, and should have a 50% duty cycle
 * - Needs to be a non-blocking function, so TIM2 directly drives the buzzer pin
 * - In CubeMX, the output pin is PA0 (TIM2_CH1)
 */

 /**
  * Notes (Shivani): Added, tim3 for buzzer interrupts
  * - Right now, the ARR is set to 35999 with a 72 mHz clock, so the interrupt counts using 0.5 ms ticks,
  * - Possible changes: we can change clock mHz to 36 mHz so that the ticks would be every ms. 
  *   
  */

#include "buzzer.h"
#include "pinout.h"
#include "tim.h"

/**************************************************************************************************
 * @section Definitions and global variables
 **************************************************************************************************/

#define BUZZER_TIMER htim2
#define BUZZER_CHANNEL TIM_CHANNEL_1 // Hardcoded for buzzer to be wired to PA0
#define TIMER_CLOCK_HZ 1000000 // tim 2
#define INTERRUPT_TIMER htim3

#define BEEP_TICKS 1000


typedef enum {
    BUZZER_IDLE = 0, // buzzer is off
    BUZZER_BEEPING, // buzzer beeps on/off indefinitely until buzzer_stop_beeping() is called 
    BUZZER_SOUND_FOR // on for a fixed duration
} buzzer_state_t;

volatile buzzer_state_t buzzer_state = BUZZER_IDLE;
volatile bool buzzer_is_on = false;
volatile uint32_t ticks_remaining = 0;


/**************************************************************************************************
 * @section Public function definitions
 **************************************************************************************************/

void buzzer_set_frequency_hz(uint32_t frequency_hz) {
    uint32_t period = TIMER_CLOCK_HZ / frequency_hz;

    __HAL_TIM_SET_AUTORELOAD(&BUZZER_TIMER, period - 1); // Sets ARR register to count to specified number of ticks (controls frequency)
    __HAL_TIM_SET_COMPARE(&BUZZER_TIMER, BUZZER_CHANNEL, period / 2); // Target 50% duty cycle
}

 void buzzer_on(void) {
    HAL_TIM_PWM_Start(&BUZZER_TIMER, BUZZER_CHANNEL);
}

void buzzer_off(void) {
    HAL_TIM_PWM_Stop(&BUZZER_TIMER, BUZZER_CHANNEL);
}


void buzzer_beep(void) {
    buzzer_set_frequency_hz(4000);

    buzzer_state = BUZZER_BEEPING;
    buzzer_is_on = true;
    ticks_remaining = BEEP_TICKS;
    buzzer_on();

    __HAL_TIM_SET_COUNTER(&INTERRUPT_TIMER, 0);
    HAL_TIM_Base_Start_IT(&INTERRUPT_TIMER);
}

void buzzer_sound_for(uint32_t duration_ms){
    buzzer_set_frequency_hz(4000);

    buzzer_state = BUZZER_SOUND_FOR;
    buzzer_is_on = true;
    ticks_remaining = duration_ms * 2;
    buzzer_on();

    __HAL_TIM_SET_COUNTER(&INTERRUPT_TIMER, 0);
    HAL_TIM_Base_Start_IT(&INTERRUPT_TIMER);
}

void buzzer_stop_beeping(void){
    buzzer_state = BUZZER_IDLE;
    buzzer_is_on = false;
    ticks_remaining = 0;

    HAL_TIM_Base_Stop_IT(&INTERRUPT_TIMER);
    buzzer_off();
}

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
    if (htim->Instance == TIM3) {
        switch (buzzer_state) {

            case BUZZER_BEEPING:
                if (--ticks_remaining == 0) {
                    buzzer_is_on = !buzzer_is_on;
                    if (buzzer_is_on) {
                        buzzer_on();
                        ticks_remaining = BEEP_TICKS;
                    } else {
                        buzzer_off();
                        ticks_remaining = BEEP_TICKS;
                    }
                }
                break;
        
            case BUZZER_SOUND_FOR:
                if (--ticks_remaining == 0) {
                    buzzer_off();
                    buzzer_is_on = false;
                    buzzer_state = BUZZER_IDLE;
                    HAL_TIM_Base_Stop_IT(&INTERRUPT_TIMER);
                }
                break;
        
            case BUZZER_IDLE:
            default:
                buzzer_off();
                HAL_TIM_Base_Stop_IT(&INTERRUPT_TIMER);
                break;
        }
    }
}