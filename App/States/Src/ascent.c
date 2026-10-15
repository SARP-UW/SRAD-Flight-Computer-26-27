/**
 * @file App/States/Inc/ascent.c
 * @authors Jude Merritt
 * @brief Ascent state draft
 */

/**
 * Notes:
 * - Still haven't decided what to do about errors in update_ascent.
 * - We'll fire backup apogee pyro in the descent state, after two seconds have elapsed since apogee was detected.
 * - ascent.h documentation should probably be improved to better describe the state operations.
 * - Write data to flash memory
 */

#include <stdbool.h>
#include <stdint.h>
#include "baro_estimate_altitude.h"
#include "stm32f4xx_hal.h"
#include "bmi088.h"
#include "pyro.h"
#include "ascent.h"

uint32_t descending_counter = 0; // Counter to track how many consecutive times the altitude has decreased
float prev_altitude = 0; // Variable to hold the previous altitude for comparison

State update_ascent(State_Context *context) {
    context->status = baro_estimate_altitude(&context->altitude);
    if (context->status != HAL_OK) {
        // Do something
    }

    if (context->altitude < prev_altitude) {
        descending_counter++;
    } else if (descending_counter > 0){
        descending_counter--;
    }

    prev_altitude = context->altitude;

    // APOGEE!!
    if (descending_counter > 20) {
        context->apogee_pyro_time = HAL_GetTick();
        fire_pyro(APOGEE);

        return DESCENT;
    }

    return ASCENT;
}