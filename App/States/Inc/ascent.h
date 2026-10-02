/**
 * @file App/States/Inc/ascent.h
 * @authors Jude Merritt
 * @brief Ascent state draft
 */

#pragma once

#include "state_machine.h"

/**
 * @brief Runs the ascent state operations, and returns either the ASCENT, or DESCENT
 *        state based on the current conditions.
 * 
 * @param context pointer to the State_Context structure that holds the current state information.
 * @return The updated state (ASCENT or DESCENT).
 */
State update_ascent(State_Context *context);