/**
 * @file App/States/Src/post_flight.c
 * @authors Shivani Palanikumar
 * @brief Post-flight state draft
 */

#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include "config.h"
#include "post_flight.h"
#include "buzzer.h"
#include "state_machine.h"

bool buzzer_stopped = false;
bool still_collecting_data = true;
bool post_flight_initialized = false;

State update_post_flight(State_Context *context) {

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

    if(!buzzer_stopped){
        uint32_t curr_time = HAL_GetTick();

        if (curr_time - context->post_flight_start_time >= 2000){
            buzzer_off();
            buzzer_stopped = true;
           }
    }

    return POST_FLIGHT;
}