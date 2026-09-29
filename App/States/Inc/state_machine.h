#pragma once

#include "states.h"

typedef struct {
    float altitude;
    uint32_t apogee_time;
    uint32_t main_pyro_time;
    HAL_StatusTypeDef status;
} State_Context;

State update_state(State next_state, State_Context *context);