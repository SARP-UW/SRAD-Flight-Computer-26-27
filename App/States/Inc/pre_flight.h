/**
 * State Operations:
 * - Toggle MCU LED at the beginning of the loop (so that it blinks)
 *     - Pull MCU LED high (so that it stops blinking) once switching states
 * - Collect IMU data, but don't write it to flash until ...
 * - Check if acceleration exceeds threshold for launch detection, if so, transition to the next state.
 * - Control the pyro LEDs
 * - Will we ever log to extern_flash in this state?
 */

#pragma once

#include "state_machine.h"

State update_pre_flight(State_Context *context);