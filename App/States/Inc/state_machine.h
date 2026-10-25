#pragma once

#include <stdint.h>
#include "states.h"
#include "stm32f4xx_hal.h"

/**
 * @brief Structure to hold the context of the current state:
 *        - Flight start time (time when the rocket was launched)
 *        - Altitude (current altitude)
 *        - Apogee pyro time (time when apogee pyro was fired)
 *        - Main pyro time   (time when main pyro was fired)
 *        - Post flight start time (time when post flight state was entered)
 *        - Status           (current status: HAL_OK, HAL_ERROR, etc.)
 *        - Previous state    (the state before the current state)
 */
typedef struct {
    uint32_t flight_start_time;
    float altitude;
    uint32_t apogee_pyro_time;
    uint32_t main_pyro_time;
    uint32_t post_flight_start_time; // Time when the rocket is grounded
    HAL_StatusTypeDef status;
    State prev_state; // Previous state before the current state
} State_Context;

/**
 * @brief Updates the state machine based on the next state and the current context.
 * 
 * @param next_state The next state to transition to.
 * @param context Pointer to the State_Context structure that holds the current state information.
 * @return The updated state after processing the next state.
 */
State update_state(State next_state, State_Context *context);