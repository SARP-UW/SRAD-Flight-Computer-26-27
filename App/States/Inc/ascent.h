/**
 * State Operations:
 * - Pull MCU LED high (so that it stops blinking)
 * - Collect IMU and barometer data and write it to external flash
 * - Detect apogee (implementation TBD)
 * - Fire apogee pyro channel at apogee.
 */

#pragma once

#include "stm32f4xx_hal.h"
#include "App/States/Inc/states.h"
#include "App/Sensors/Inc/bmi088.h"
#include "App/Sensors/Inc/bmp390.h"
#include "App/Devices/Inc/led.h"
#include "App/Utils/Inc/pyro.h"
#include "App/Devices/Inc/extern_flash.h"
#include "App/States/Inc/state_machine.h"

HAL_StatusTypeDef update_ascent(State_Context *context);