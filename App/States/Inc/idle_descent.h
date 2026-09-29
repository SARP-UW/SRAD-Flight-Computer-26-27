/**
 * State Operations:
 * - Take the apogee_time from the state machine as a parameter, then check to see when two seconds has elapsed. Once it has, fire the backup main pyro charge. 
 * - ...
 */

#pragma once

#include "stm32f4xx_hal.h"
#include "state_machine.h"

State update_idle_descent(State_Context *context);