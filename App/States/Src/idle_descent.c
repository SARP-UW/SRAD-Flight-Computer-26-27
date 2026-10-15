/**
 * @file App/States/Inc/idle_descent.c
 * @authors Michael Zheng
 * @brief Idle descent state draft
 */

/** State Operations:
 * 1) Fire main backup charge
 *   - Time-based based using the param in config.h, based on when the main charge fires in descent.c
 * 2) Determine when rocket has landed then switch to post_flight
 *   - Could use barometer or accel
 */

/** Notes
 * - Likely NEEDS secondary check for landing detection,
 * proably will analyze acceleration data
 */

#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include "state_machine.h"
#include "estimate_altitude.h"
#include "pyro.h"
#include "config.h"
#include "idle_descent.h"

static bool main_backup_fired = false; // Flag to track if the main backup pyro has been fired

State update_idle_descent(State_Context *context) {
    // Fire main backup pyro after the specified delay
    if (!main_backup_fired) {
        uint32_t current_time = HAL_GetTick();
        if (current_time - context->main_pyro_time >= MAIN_PYRO_BACKUP_DELAY_MS) {
            fire_pyro(MAIN_BACKUP);
            main_backup_fired = true;
        }
    }
    
    // Check if the rocket has landed, ref: 
    // https://docs.google.com/document/d/14AMPyc_z-sSQZw2hGkxxA7nLwaDc4ogyx1ahcZhJtiA/edit?tab=t.0
    // Check for constant vertical velocity from baro to determine landing
    

    return IDLE_DESCENT;
}
