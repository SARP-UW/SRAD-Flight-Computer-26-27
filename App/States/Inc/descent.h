/**
 * State Operations:
 * - Take the apogee_time from the state machine as a parameter, then check to see when two seconds has elapsed. Once it has, fire the backup apogee pyro charge. 
 * - ...
 */

#pragma once

#include "App/States/Inc/state_machine.h"

HAL_StatusTypeDef update_descent(State_Context *context);