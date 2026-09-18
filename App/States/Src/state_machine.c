#include "States/Inc/state_machine.h"
#include "States/Inc/pre_flight.h"
#include "States/Inc/ascent.h"
#include "States/Inc/descent.h"
#include "States/Inc/idle_descent.h"
#include "States/Inc/post_flight.h"

State update_state(State next_state, State_Context *context) {
    switch (next_state) {
        case PRE_FLIGHT: {
            HAL_StatusTypeDef status = update_pre_flight();

            if (status != HAL_OK) {
                //...
            }

            break;
        }
        case ASCENT: {
            HAL_StatusTypeDef status = update_ascent(context);

            if (status != HAL_OK) {
                //...
            }

            break;
        }
        case DESCENT: {
            HAL_StatusTypeDef status = update_descent(context);

            if (status != HAL_OK) {
                //...
            }
            
            break;
        }
        case IDLE_DESCENT: {
            HAL_StatusTypeDef status = update_idle_descent(context);

            if (status != HAL_OK) {
                //...
            }

            break;
        }
        case POST_FLIGHT: {
            HAL_StatusTypeDef status = update_post_flight();

            if (status != HAL_OK) {
                //...
            }

            break;
        }
    }
}