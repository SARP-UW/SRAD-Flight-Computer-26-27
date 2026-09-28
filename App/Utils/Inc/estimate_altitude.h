/**
 * @file App/Utils/Inc/estimate_altitude.h
 * @authors Jude Merritt
 * @brief Altitude estimation algorithm based only on barometer readings
 */

#pragma once

#include "stm32f4xx_hal.h"

/**
 * @brief Configures the ground reference pressure and temperature for altitude estimation. 
 * 
 * IMPORTANT: This function should be called throughout the pre-flight state, and
 * should not be called during any other state.
 * 
 * @return HAL_StatusTypeDef HAL_OK if the configuration was successful, or an error code otherwise.
 */
HAL_StatusTypeDef configure_ground_pressure(void);

/**
 * @brief Estimates the altitude based on the current barometer readings and the configured ground reference.
 * 
 * @param altitude pointer to a float variable to store the estimated altitude in meters.
 * @return HAL_StatusTypeDef HAL_OK if the estimation was successful, or an error code otherwise.
 */
HAL_StatusTypeDef estimate_altitude(float *altitude);