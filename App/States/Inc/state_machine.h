#pragma once

#include <stdint.h>
#include "states.h"

typedef struct {
    uint32_t apogee_time;
    uint32_t main_pyro_time;
} State_Context;

State update_state(State next_state, State_Context *context);