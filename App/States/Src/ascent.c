// Comment

/**
 * Notes:
 * - Still haven't decided what to do about errors in update_ascent.
 * - Some of these globals should be static. Keep in mind that pre_flight will also 
 *   use the imu. Maybe some things should be shared when pre_flight implementation is more defined.
 */

#include <stdbool.h>
#include "estimate_altitude.h"
#include "stm32f4xx_hal.h"
#include "bmi088.h"
#include "pyro.h"
#include "ascent.h"

float velocity = 0.0f;
uint32_t last_tick = 0;
bmi088_data_t imu_data; // Global variable to hold the IMU data
uint32_t descending_counter = 0; // Counter to track how many consecutive times the altitude has decreased
float prev_altitude = 0; // Variable to hold the previous altitude for comparison
bool ascent_initialized = false;

State update_ascent(State_Context *context) {
    if (!ascent_initialized) {
        last_tick = HAL_GetTick();
        ascent_initialized = true;
    }

    context->status = update_bmi088(&imu_data);
    
    uint32_t curr_tick = HAL_GetTick();
    float dt = (curr_tick - last_tick) / 1000.0f;
    last_tick = curr_tick;

    velocity += imu_data.accel_z * dt; // Integrate acceleration to get velocity

    if (velocity > 200.0f) {
        return ASCENT;
    }

    if (context->status != HAL_OK) {
        estimate_altitude(&context->altitude);
    } else {
        context->status = estimate_altitude(&context->altitude);
    }

    if (context->altitude < prev_altitude) {
        descending_counter++;
    } else if (descending_counter > 0){
        descending_counter--;
    }
    prev_altitude = context->altitude;

    if (descending_counter > 20) {
        context->apogee_time = HAL_GetTick();
        pyro_fire(APOGEE);

        return DESCENT;
    }

    return ASCENT;
}