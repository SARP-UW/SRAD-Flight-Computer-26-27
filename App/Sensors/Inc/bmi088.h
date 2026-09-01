// Comment

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
} BMI088_Data;

HAL_StatusTypeDef init_BMI088(BMI088_Data *data);

/**
 * @brief  Update the BMI088 sensor data.
 */
HAL_StatusTypeDef update_BMI088(BMI088_Data *data);