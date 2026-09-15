/**
 * State Operations:
 * - Pull MCU LED high (so that it stops blinking)
 * - Collect IMU and barometer data and write it to external flash
 * - Detect apogee (implementation TBD)
 * - Fire apogee pyro channel at apogee.
 */

#pragma once

#include "App/States/Inc/state_machine.h"

HAL_StatusTypeDef update_ascent(State_Context *context);