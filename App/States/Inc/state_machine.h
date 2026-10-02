#pragma once

#include <stdint.h>
#include "states.h"

/**
 * @brief Structure to hold the context of the current state:
 *        - Apogee pyro time (time when apogee pyro was fired)
 *        - Main pyro time   (time when main pyro was fired)
 *        - Altitude         (current altitude)
 *        - Status           (current status: HAL_OK, HAL_ERROR, etc.)
 */
typedef struct {
    float altitude;
    uint32_t apogee_pyro_time;
    uint32_t main_pyro_time;
    HAL_StatusTypeDef status;
} State_Context;

/**
 * @brief Updates the state machine based on the next state and the current context.
 * 
 * @param next_state The next state to transition to.
 * @param context Pointer to the State_Context structure that holds the current state information.
 * @return The updated state after processing the next state.
 */
State update_state(State next_state, State_Context *context);