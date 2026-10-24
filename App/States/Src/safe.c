#include <stdbool.h>
#include "config.h"
#include "pyro.h"
#include "safe.h"

#define TEN_SECOND_BUFFER_MS 10000

bool restart_needed = false;

void enter_safe_state(State_Context *context) {
    uint32_t curr_time;

    switch (context->prev_state) {
        case PRE_FLIGHT:
            buzzer_set_frequency_hz(4800);
            buzzer_on();
            restart_needed = true;
            break;

        case ASCENT:
            curr_time = HAL_getTick();
            if (curr_time - context->flight_start_time >= SIMULATED_TIME_TO_APOGEE_MS) {
                fire_pyro(APOGEE); // Fire apogee pyro charge at apogee if there is a critical error in the ascent state
                HAL_Delay(APOGEE_PYRO_BACKUP_DELAY_MS);
                fire_pyro(APOGEE_BACKUP);

                return DESCENT;
            }
            break;

        case DESCENT:
            curr_time = HAL_getTick();
            if (curr_time - context->flight_start_time >= (SIMULATED_TIME_TO_APOGEE_MS + APOGEE_TO_MAIN_PYRO_DELAY_MS)) {
                fire_pyro(MAIN);
                HAL_Delay(MAIN_PYRO_BACKUP_DELAY_MS);
                fire_pyro(MAIN_BACKUP);

                return IDLE_DESCENT;
            }
            break;

        case IDLE_DESCENT:
            if (curr_time - context->flight_start_time >= SIMULATED_FLIGHT_DURATION_MS + TEN_SECOND_BUFFER_MS) {
                return POST_FLIGHT;
            }
            break;

        case POST_FLIGHT:
            return POST_FLIGHT;

        default:
            break;
    }

    return SAFE;
}