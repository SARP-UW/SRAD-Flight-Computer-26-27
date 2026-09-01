// Comment

#pragma once

#include <stdint.h>
#include "stm32f4xx_hal.h"

typedef struct {
    float pressure;    // Pressure in Pascals
    float temperature; // Temperature in degrees Celsius
    float altitude;    // Altitude in meters
} BMP390_Data;

HAL_StatusTypeDef init_BMP390(BMP390_Data *data);

HAL_StatusTypeDef update_BMP390(BMP390_Data *data);