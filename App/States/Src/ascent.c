// Comment

#include "estimate_altitude.h"
#include "stm32f4xx_hal.h"
#include "bmi088.h"
#include "pyro.h"
#include "ascent.h"

float velocity = 0.0f;
uint32_t last_tick = 0; // Used to find dt for the velocity calculation
bmi088_data_t imu_data; // Global variable to hold the IMU data
uint32_t descending_counter = 0; // Counter to track how many consecutive times the altitude has decreased
uint32_t prev_altitude = 0; // Variable to hold the previous altitude for comparison

State update_ascent(State_Context *context) {
    context->status = update_bmi088(&imu_data);

    uint32_t curr_tick = HAL_GetTick();
    float dt = (curr_tick - last_tick) / 1000.0f;
    last_tick = curr_tick;

    velocity += imu_data.accel_z * dt; // Integrate acceleration to get velocity

    if (velocity < 200.0f) {
        return HAL_OK;
    }

    context->status = estimate_altitude(&context->altitude);

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