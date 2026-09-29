#include "States/Inc/state_machine.h"
#include "States/Inc/pre_flight.h"
#include "States/Inc/ascent.h"
#include "States/Inc/descent.h"
#include "States/Inc/idle_descent.h"
#include "States/Inc/post_flight.h"

State update_state(State next_state, State_Context *context) {
    switch (next_state) {
        case PRE_FLIGHT: {
            next_state = update_pre_flight(context);

            return next_state;
        }
        case ASCENT: {
            next_state = update_ascent(context);

            return next_state;
        }
        case DESCENT: {
            next_state = update_descent(context);
            
            return next_state;
        }
        case IDLE_DESCENT: {
            next_state = update_idle_descent(context);

            return next_state;
        }
        case POST_FLIGHT: {
            next_state = update_post_flight(context);

            return next_state;
        }

        default: {
            return DESCENT; // Default to DESCENT if an invalid state is provided
        }
    }
}