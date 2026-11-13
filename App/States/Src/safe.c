/**
 * @file App/States/Src/safe.c
 * @authors Jude Merritt
 * @brief Safe state draft
 */

/**
 * Notes:
 * - Need to add a significant buffer to each of these events to guarentee that they do not happen too early. 
 * - Not every error should send the state machine to the safe state. For example, if one sensor fails, we should try to use 
 *   the other sensor before sending the state machine to the safe state.
 */

#include <stdbool.h>
#include "baro_estimate_altitude.h"
#include "stm32f4xx_hal.h"
#include "bmi088.h"
#include "safe.h"

static bool restart_needed = false;
bmi088_data_t imu_data;

void enter_safe_state(State_Context *context) {

    switch (context->prev_state) {
        case PRE_FLIGHT:
            if (restart_needed) {
                buzzer_set_frequency_hz(4800);
                buzzer_on();
                restart_needed = true;
            }
            break;

        case POST_FLIGHT:
            return POST_FLIGHT;

        default:
            context->baro_status = baro_estimate_altitude(&context->altitude);
            context->imu_status = update_bmi088(&imu_data);
            // log to flash memory
            break;
    }

    return SAFE;
}