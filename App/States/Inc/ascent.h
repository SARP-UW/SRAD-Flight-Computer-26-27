/**
 * State Operations:
 * - Collect IMU and barometer data and write it to external flash
 * - Detect apogee (implementation TBD)
 * - Fire apogee pyro channel at apogee.
 */

#pragma once

#include "stm32f4xx_hal.h"
#include "state_machine.h"

State update_ascent(State_Context *context);