/**
 * @file App/States/Src/post_flight.c
 * @authors Shivani Palanikumar
 * @brief Post-flight state draft
 */

/**
 * Notes:
 * JUDE:
 * - In the future, let's implement a buzzer beep method that takes a frequency and a duration as parameters.
 *   the function would be contained in buzzer.c, and would be called in place of the buzzer_on() function. 
 *   The would save energy by allowing us to turn the buzzer off after a set duration, instead of leaving
 *   it on indefinitely.
 */

#include <stdbool.h>
#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "config.h"
#include "post_flight.h"
#include "buzzer.h"
#include "state_machine.h"

bool buzzer_stopped = false;
bool still_collecting_data = true;
bool post_flight_initialized = false;

State update_post_flight(State_Context *context) {
    context->prev_state = POST_FLIGHT;

    if(!post_flight_initialized){
        uint32_t freq = 4000; 
        buzzer_set_frequency(freq);
        buzzer_on();
        
        post_flight_initialized = true;
    }

    if (still_collecting_data){
       uint32_t curr_time = HAL_GetTick();
       //save data?
       
       if (curr_time - context->post_flight_start_time >= POST_FLIGHT_LOGGING_DURATION_MS){
        still_collecting_data = false;
       }
    }

    return POST_FLIGHT;
}