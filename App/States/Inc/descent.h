/**
 * State Operations:
 * - Take the apogee_time from the state machine as a parameter, then check to see when two seconds has elapsed. Once it has, fire the backup apogee pyro charge. 
 * - ...
 */

#pragma once

#include "state_machine.h"

State update_descent(State_Context *context);