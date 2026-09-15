/**
 * State Operations:
 * - Toggle MCU LED at the beginning of the loop (so that it blinks)
 * - Collect IMU data, but don't write it to flash until ...
 * - Check if acceleration exceeds threshold for launch detection, if so, transition to the next state.
 * - Control the pyro LEDs
 * - Will we ever log to extern_flash in this state?
 */

#pragma once

HAL_StatusTypeDef update_pre_flight(void);