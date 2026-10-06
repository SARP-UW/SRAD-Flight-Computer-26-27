/**
 * @file App/States/Src/descent.c
 * @authors Jude Merritt
 * @brief Descent state draft
 */

 /**
 * Notes:
 * - Still haven't decided what to do about errors in update_descent.
 * - We'll fire backup main pyro in the idle-descent state (I know, the name seems somewhat counterintuitive now),
 *   after two seconds have elapsed since an altitude of XXXX meters (currently 500m) was detected.
 * - descent.h documentation should probably be improved to better describe the state operations.
 */

#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include "state_machine.h"
#include "pyro.h"
#include "config.h"
#include "descent.h"

static bool backup_apogee_fired = false; // Flag to track if the backup apogee pyro has been fired

State update_descent(State_Context *context) {
    // What does this do
    if (!backup_apogee_fired) {
        uint32_t current_time = HAL_GetTick();
        if (current_time - context->apogee_pyro_time >= APOGEE_PYRO_BACKUP_DELAY_MS) {
            pyro_fire(APOGEE_BACKUP);
            backup_apogee_fired = true;
        }

        return DESCENT;
    }

    context->status = estimate_altitude(&context->altitude);
    if (context->status != HAL_OK) {
        // Do something
    }

    if (context->altitude <= MAIN_PYRO_BACKUP_ALTITUDE_M) {
        context->main_pyro_time = HAL_GetTick();
        pyro_fire(MAIN);

        return IDLE_DESCENT;
    }

    return DESCENT;
}
