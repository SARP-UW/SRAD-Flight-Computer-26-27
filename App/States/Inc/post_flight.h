/**
 * @file App/States/Inc/post_flight.h
 * @authors Shivani Palanikumar
 * @brief Post-flight state draft
 */

/**
 * State Operations: 
 * - Sound the piezo buzzer
 * - Continue logging data for 15 seconds after we first switch to the state then stop
 * - Write data to flash memory (but not for draft)
 * - + other stuff?
 */

#pragma once

#include "state_machine.h"

State update_post_flight(State_Context *context);