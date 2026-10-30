/**
 * @file App/States/Src/idle_descent.c
 * @authors Michael Zheng
 * @brief Idle descent state draft
 */

/** State Operations:
 * 1) Fire main backup charge
 *   - Fires x seconds after the main charge fires in descent.c
 * 2) Determine when rocket has landed, then switch to post_flight
 *   - Checks for 1) low vertical speed and 2) [backup condition]
 */

/** Notes
 * - Likely NEEDS secondary check for landing detection,
 *     probably will analyze acceleration data
 * JUDE: 
 * - I don't love that the under_threshold_timer resets to zero if
 *   a single measurment is above the threshold. I worry that noise, or a strong gust
 *   of wind could prevent the state machine from switching to the post_flight state. 
 * - I also realize that due to the speed of the processor we may be working with 
 *   altitude values that are incredibly close to one another, and very small differences 
 *   in time. Maybe this is not inherently bad, but it is something to keep in mind.
 * - Maybe we could get away with using just one flag instead of two?
 * - This state makes me think that an entry function for each state may be useful. 
 */

#include <math.h>
#include <stdbool.h>
#include <stdint.h>

#include "stm32f4xx_hal.h"
#include "state_machine.h"
#include "config.h"
#include "idle_descent.h"

#include "baro_estimate_altitude.h"
#include "pyro.h"
#include "low_pass_filter.h"

static bool main_backup_fired = false;  // Flag to track if the main backup pyro has been fired
static bool landing_detection_started = false;  // Flag to track if landing detection has started
static float prev_altitude_m = 0;  // Variable to hold the previous altitude for comparison, 
static uint32_t prev_altitude_time_ms = 0;  // Timestamp of previous altitude reading, ms
static uint32_t under_threshold_timer_ms = 0;  // How long the vertical speed has continuously under set threshold, ms

static low_pass_filter_t vertical_speed_lpf;  // Low-pass filter for vertical speed

State update_idle_descent(State_Context *context) {
    context->prev_state = IDLE_DESCENT;
    
    // Fire main backup pyro after the specified delay
    if (!main_backup_fired) {
        uint32_t current_time = HAL_GetTick();
        if (current_time - context->main_pyro_time >= MAIN_PYRO_BACKUP_DELAY_MS) {
            fire_pyro(MAIN_BACKUP);
            main_backup_fired = true;
        }
    }
    
    // Check for constant vertical velocity from baro to determine landing
    //   ref: https://docs.google.com/document/d/14AMPyc_z-sSQZw2hGkxxA7nLwaDc4ogyx1ahcZhJtiA/edit?tab=t.0
    
    // Calculates vertical velocity
    context->status = baro_estimate_altitude(&context->altitude);
    if (context->status != HAL_OK) {
        return SAFE;
    }

    if(!landing_detection_started) {
        prev_altitude_m = context->altitude;
        prev_altitude_time_ms = HAL_GetTick();

        low_pass_filter_init(&vertical_speed_lpf, 1.0f);

        landing_detection_started = true;
        return IDLE_DESCENT;
    } // Gets initial value for prev_altitude and prev_altitude_time

    uint32_t current_time = HAL_GetTick();
    uint32_t time_interval_ms = (current_time - prev_altitude_time_ms);

    if (time_interval_ms >= 20) {
        float vertical_speed = (context->altitude - prev_altitude_m) / (time_interval_ms / 1000.0f);
        // low pass filter
        vertical_speed = fabsf(low_pass_filter_update(&vertical_speed_lpf, vertical_speed, time_interval_ms));
        
        if (vertical_speed < LANDING_DETECTION_MAX_VELOCITY) {
            under_threshold_timer_ms += time_interval_ms;
        } else {
            if (under_threshold_timer_ms > time_interval_ms) {
                under_threshold_timer_ms -= time_interval_ms; // it would be bad if we set a uint32 negative
            } else {
                under_threshold_timer_ms = 0;
            }
        }

        prev_altitude_m = context->altitude;
        prev_altitude_time_ms = current_time; // Update previous altitude time for next iteration
    }

    if (under_threshold_timer_ms >= LANDING_DETECTION_INTERVAL_MS) {
        // Revise how this secondary check is implemented
        return POST_FLIGHT;
    }

    return IDLE_DESCENT;
}
