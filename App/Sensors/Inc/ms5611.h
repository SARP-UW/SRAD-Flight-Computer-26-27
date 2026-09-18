/**
 * @file App/Sensors/Inc/ms5611.h
 * @authors Jude Merritt
 * @brief MS561101BA03 Barometer driver
 */

 #pragma once

#include <stdint.h>
#include "stm32f4xx_hal.h"

/**************************************************************************************************
 * @section Type definitions
 **************************************************************************************************/

/**
 * @brief Oversampling ratios
 */
typedef enum {
    OSR_256  = 0x00, /**< 256 samples per measurement */
    OSR_512  = 0x02, /**< 512 samples per measurement */
    OSR_1024 = 0x04, /**< 1024 samples per measurement */
    OSR_2048 = 0x06, /**< 2048 samples per measurement */
    OSR_4096 = 0x08  /**< 4096 samples per measurement */
} ms5611_osr_t;

/**
 * @brief Barometer results
 */
typedef struct {
    float pressure;    // Pressure in mbar
    float temperature; // Temperature in degrees Celsius
} ms5611_data_t;

/**************************************************************************************************
 * @section Function Definitions
 **************************************************************************************************/

/**
 * @brief Initializes the MS561101BA03 barometer. 
 *
 * @param data pointer to the ms5611_data structure to store the barometer data.
 * @param osr Oversampling ratio to use for measurements. A higher oversampling
 *            ratio will result in more accurate measurements, but will take longer to complete.
 * @return HAL_StatusTypeDef HAL_OK if the initialization was successful, or an error code otherwise.
 */
HAL_StatusTypeDef init_ms5611(ms5611_osr_t osr);

/**
 * @brief Updates the data structure with pressure, temperature, and altitude values.
 * @param data pointer to the ms5611_data structure to store the barometer data.
 * @return HAL_StatusTypeDef HAL_OK if the update was successful, or an error code otherwise.
 */
HAL_StatusTypeDef update_ms5611(ms5611_data_t *data);