/**
 * @file App/Utils/Inc/estimate_altitude.h
 * @authors Jude Merritt
 * @brief Altitude estimation algorithm based only on barometer readings
 */

/**
 * Notes:
 * - This algorithm only uses barometer readings to estimate altitude. This means that it has
 *   a lot of room for improvement. If time allows, it may be good to build off this to implement
 *   a more robust algorithm.
 * - Maybe replace the magic numbers in the altitude calculation with constants that are defined near the top of the file.
 */

#include "estimate_altitude.h"
#include "ms5611.h"
#include <stdint.h>
#include <math.h>


static float ground_pressure      = 1013.25f; // Default ground pressure in mbar (sea level standard atmospheric pressure)
static float ground_temperature   = 288.15f;  // Default ground temperature in Kelvin (sea level standard temperature)
static uint32_t config_data_count = 0;        // Counter for the number of configuration data points
static ms5611_data_t ms5611_data;             // Global variable to hold the barometer data

HAL_StatusTypeDef configure_ground_reference(void) {
    HAL_StatusTypeDef status = update_ms5611(&ms5611_data);
    if (status != HAL_OK) {
        return status;
    }

    if (ms5611_data.pressure <=0.0f) {
        return HAL_ERROR;
    }

    config_data_count++;
    ground_pressure += (ms5611_data.pressure - ground_pressure) / config_data_count;
    ground_temperature += ((ms5611_data.temperature + 273.15f) - ground_temperature) / config_data_count;

    return HAL_OK;
}

HAL_StatusTypeDef estimate_altitude(float *altitude) {
    HAL_StatusTypeDef status = update_ms5611(&ms5611_data);
    if (status != HAL_OK) {
        return status;
    }

    if (ms5611_data.pressure <=0.0f) {
        return HAL_ERROR;
    }

    float p = ms5611_data.pressure; // Current pressure in mbar
    float p0 = ground_pressure;     // Ground pressure in mbar
    float t0 = ground_temperature;  // Ground temperature in Kelvin

    // ref: https://ntrs.nasa.gov/citations/19770009539 (Page 12, equation 33a, solve for delta H)
    // The constants are derived from the International Standard Atmosphere (ISA) model,
    // which assumes a standard lapse rate of 6.5 K/km in the troposphere.
    float h = (t0 / 0.0065f) * (1.0f - powf((p / p0), 0.190284f)); // Altitude in meters

    *altitude = h;
    return HAL_OK;
}