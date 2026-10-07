#include "state_machine.h"
#include "pre_flight.h"
#include "ascent.h"
#include "descent.h"
#include "idle_descent.h"
#include "post_flight.h"

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