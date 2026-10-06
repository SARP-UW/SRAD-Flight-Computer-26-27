/**
 * @file App/States/Inc/descent.h
 * @authors Jude Merritt
 * @brief Descent state draft
 */

#pragma once

#include "state_machine.h"

/**
 * @brief Runs the descent state operations, and returns either the DESCENT, or IDLE_DESCENT
 *        state based on the current conditions.
 * 
 * @param context pointer to the State_Context structure that holds the current state information.
 * @return The updated state (DESCENT or IDLE_DESCENT).
 */
State update_descent(State_Context *context);