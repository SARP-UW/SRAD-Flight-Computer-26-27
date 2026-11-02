//This could probably just be included in state_machine.h.

#pragma once

typedef enum {
    PRE_FLIGHT,
    ASCENT,
    DESCENT,
    IDLE_DESCENT,
    POST_FLIGHT,
    SAFE
} State;