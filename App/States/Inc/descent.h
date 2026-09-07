/**
 * State Operations:
 * - Take the apogee_time from the state machine as a parameter, then check to see when two seconds has elapsed. Once it has, fire the backup apogee pyro charge. 
 * - ...
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

HAL_StatusTypeDef update_descent(State_Context *context);