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
 */

#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include "state_machine.h"
#include "pyro.h"
#include "descent.h"

#define TWO_SECONDS 2000 // 2 seconds in milliseconds
#define FIVE_HUNDRED_METERS 500.0f // 500 meters

static bool backup_apogee_fired = false; // Flag to track if the backup apogee pyro has been fired

State update_descent(State_Context *context) {
    // What does this do
    if (!backup_apogee_fired) {
        uint32_t current_time = HAL_GetTick();
        if (current_time - context->apogee_pyro_time >= TWO_SECONDS) {
            pyro_fire(APOGEE_BACKUP);
            backup_apogee_fired = true;
        }

        return DESCENT;
    }

    context->status = estimate_altitude(&context->altitude);
    if (context->status != HAL_OK) {
        // Do something
    }

    if (context->altitude <= FIVE_HUNDRED_METERS) {
        context->main_pyro_time = HAL_GetTick();
        pyro_fire(MAIN);

        return IDLE_DESCENT;
    }

    return DESCENT;
}
