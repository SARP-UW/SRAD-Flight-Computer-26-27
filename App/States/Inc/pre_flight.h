/**
 * State Operations:
 * - Toggle MCU LED at the beginning of the loop (so that it blinks)
 * - Collect IMU data, but don't write it to flash
 * - Check if acceleration exceeds threshold for launch detection, if so, transition to the next state.
 * - Control the pyro LEDs
 * - Will we ever log to extern_flash in this state?
 */

#pragma once

#include "stm32f4xx_hal.h"
#include "App/States/Inc/states.h"
#include "App/Sensors/Inc/bmi088.h"
#include "App/Sensors/Inc/bmp390.h"
#include "App/Devices/Inc/led.h"
#include "App/Utils/Inc/pyro.h"

HAL_StatusTypeDef update_pre_flight();