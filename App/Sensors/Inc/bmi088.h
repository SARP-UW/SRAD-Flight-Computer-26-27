/**
 * @file App/Sensors/Inc/bmi088.h
 * @authors Jude Merritt
 * @brief BMI088 IMU driver
 */

#pragma once

#include <stdint.h>
#include "stm32f4xx_hal.h"

typedef struct {
    float accel_x; // Acceleration in the X direction (m/s^2)
    float accel_y; // Acceleration in the Y direction (m/s^2)
    float accel_z; // Acceleration in the Z direction (m/s^2)
    float gyro_x;  // Angular velocity around the X axis (rad/s)
    float gyro_y;  // Angular velocity around the Y axis (rad/s)
    float gyro_z;  // Angular velocity around the Z axis (rad/s)
} bmi088_data_t;

/**
 * @brief Initializes the BMI088 IMU. 
 * 
 * @return HAL_StatusTypeDef HAL_OK if the initialization was successful, or an error code otherwise.
 */
HAL_StatusTypeDef init_bmi088(void);

/**
 * @brief Updates the bmi088_data_t structure with linear acceleration and angular velocity vaules.
 *
 * @param data pointer to the bmi088_data_t structure to store the IMU data.
 * @return HAL_StatusTypeDef HAL_OK if the initialization was successful, or an error code otherwise.
 */
HAL_StatusTypeDef update_bmi088(bmi088_data_t *data);