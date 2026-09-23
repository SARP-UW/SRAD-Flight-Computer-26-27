/**
 * @file App/Sensors/Src/ms5611.c
 * @authors Jude Merritt
 * @brief MS561101BA03 Barometer driver
 */

/**
 * Notes:
 * - Could improve the driver by verifying the PROM values with the CRC value.
 * - Altitude will be calculated in a higher-level file.
 * - May be good to abstract the convertion math from ms5611_update so that you can
 *   easily conver to different units.
 */

#include "ms5611.h"
#include "spi.h"
#include "gpio.h"
#include "pinout.h"

/**************************************************************************************************
 * @section Type definitions
 **************************************************************************************************/

typedef struct {
    uint16_t sens;     // C1 Pressure sensitivity
    uint16_t off;      // C2 Pressure offset
    uint16_t tcs;      // C3 Temperature coefficient of pressure sensitivity
    uint16_t tco;      // C4 Temperature coefficient of pressure offset
    uint16_t t_ref;    // C5 Reference temperature
    uint16_t tempsens; // C6 Temperature coefficient of the temperature
}ms5611_calibration_data_t; 

/**************************************************************************************************
 * @section Definitions and global variables
 **************************************************************************************************/

// Basic Commands
#define D1_BASE_CMD 0x40 // D1 conversion at 256 OSR. Add desired OSR value to this base command
#define D2_BASE_CMD 0x50 // D2 conversion at 256 OSR. Add desired OSR value to this base command
#define ADC_READ    0x00
#define RESET       0x1E

// Calibration commands
#define PROM_ADDR_MANUFACTURER 0xA0
#define PROM_ADDR_C1           0xA2
#define PROM_ADDR_C2           0xA4
#define PROM_ADDR_C3           0xA6
#define PROM_ADDR_C4           0xA8
#define PROM_ADDR_C5           0xAA
#define PROM_ADDR_C6           0xAC
#define PROM_ADDR_CRC          0xAE

// SPI
extern SPI_HandleTypeDef hspi1;
static const uint8_t timeout = 10;       // 10ms timeout for SPI transfers

// Global and static variables
#define RELOAD_DELAY 10 // (ms)
static ms5611_calibration_data_t calibration_data; // Static variable to hold calibration data
static ms5611_osr_t osr;                           // Static variable to hold the oversampling ratio

/**************************************************************************************************
 * @section Private function definitions
 **************************************************************************************************/

// Used to provide correct delay based on selected osr
 static void ms5611_delay(ms5611_osr_t osr) {
    uint8_t conversion_time;

   /*
    * The conversion times are based on the following table:
    * OSR   Min.   Max.
    * 4096  7.40   9.04
    * 2048  3.72   4.54
    * 1024  1.88   2.28
    * 512   0.95   1.17
    * 256   0.48   0.60
    */
    switch (osr) {
        case OSR_256:  conversion_time = 1;  break;
        case OSR_512:  conversion_time = 2;  break;
        case OSR_1024: conversion_time = 3;  break;
        case OSR_2048: conversion_time = 5;  break;
        case OSR_4096: conversion_time = 10; break;
        default: return;
    }

    HAL_Delay(conversion_time);
}

// Used to send a single command
static HAL_StatusTypeDef send_command(uint8_t cmd) {
    uint8_t tx[1] = {cmd};
    HAL_GPIO_WritePin(MS5611_CS_PORT, MS5611_CS_PIN, GPIO_PIN_RESET);
    HAL_StatusTypeDef status = HAL_SPI_Transmit(&hspi1, tx, 1, timeout);
    HAL_GPIO_WritePin(MS5611_CS_PORT, MS5611_CS_PIN, GPIO_PIN_SET);
    return status;
}

// Used to read from the adc
static HAL_StatusTypeDef read_adc(uint32_t *res) {
    uint8_t tx[1] = {ADC_READ};
    uint8_t rx[3] = {0};

    HAL_GPIO_WritePin(MS5611_CS_PORT, MS5611_CS_PIN, GPIO_PIN_RESET);
    HAL_StatusTypeDef status = HAL_SPI_Transmit(&hspi1, tx, 1, timeout);
    if (status != HAL_OK) {
        HAL_GPIO_WritePin(MS5611_CS_PORT, MS5611_CS_PIN, GPIO_PIN_SET);
        return status;
    }

    status = HAL_SPI_Receive(&hspi1, rx, 3, timeout);
    HAL_GPIO_WritePin(MS5611_CS_PORT, MS5611_CS_PIN, GPIO_PIN_SET);
    if (status == HAL_OK) {
        *res = (rx[0] << 16) | (rx[1] << 8) | rx[2];
    }

    return status;
}

// Used to read from the prom
static HAL_StatusTypeDef read_prom(uint8_t prom_addr, uint16_t *res) {
    uint8_t tx[1] = {prom_addr};
    uint8_t rx[2] = {0};

    HAL_GPIO_WritePin(MS5611_CS_PORT, MS5611_CS_PIN, GPIO_PIN_RESET);
    HAL_StatusTypeDef status = HAL_SPI_Transmit(&hspi1, tx, 1, timeout);
    if (status != HAL_OK) {
        HAL_GPIO_WritePin(MS5611_CS_PORT, MS5611_CS_PIN, GPIO_PIN_SET);
        return status;
    }

    status = HAL_SPI_Receive(&hspi1, rx, 2, timeout);
    HAL_GPIO_WritePin(MS5611_CS_PORT, MS5611_CS_PIN, GPIO_PIN_SET);
    if (status == HAL_OK) {
        *res = (rx[0] << 8) | rx[1];
    }

    return status;
}

/**************************************************************************************************
 * @section Public function definitions
 **************************************************************************************************/

HAL_StatusTypeDef init_ms5611(ms5611_osr_t selected_osr) {
    osr = selected_osr;

    HAL_StatusTypeDef status = send_command(RESET); // Send reset command
    if (status != HAL_OK) {
        return status;
    }

    HAL_Delay(RELOAD_DELAY); // Wait for the sensor to reset

    uint16_t res;

    status = read_prom(PROM_ADDR_C1, &res); // Read C1
    if (status != HAL_OK) return status;
    calibration_data.sens = res;

    status = read_prom(PROM_ADDR_C2, &res); // Read C2
    if (status != HAL_OK) return status;
    calibration_data.off = res;

    status = read_prom(PROM_ADDR_C3, &res); // Read C3
    if (status != HAL_OK) return status;
    calibration_data.tcs = res;

    status = read_prom(PROM_ADDR_C4, &res); // Read C4
    if (status != HAL_OK) return status;
    calibration_data.tco = res;

    status = read_prom(PROM_ADDR_C5, &res); // Read C5
    if (status != HAL_OK) return status;
    calibration_data.t_ref = res;

    status = read_prom(PROM_ADDR_C6, &res); // Read C6
    if (status != HAL_OK) return status;
    calibration_data.tempsens = res;

    return HAL_OK;
}

HAL_StatusTypeDef update_ms5611(ms5611_data_t *data) {
    uint32_t d1, d2;
    HAL_StatusTypeDef status;

    status = send_command(D1_BASE_CMD + osr); // Start pressure conversion
    if (status != HAL_OK) return status;
    ms5611_delay(osr); // Wait for conversion to complete
    status = read_adc(&d1);
    if (status != HAL_OK) return status;

    status = send_command(D2_BASE_CMD + osr); // Start temperature conversion
    if (status != HAL_OK) return status;
    ms5611_delay(osr); // Wait for conversion to complete
    status = read_adc(&d2);
    if (status != HAL_OK) return status;

    // Calculate temperature difference
    int32_t delta_t = d2 - ((int32_t)calibration_data.t_ref << 8);

    // Calculate actual temperature
    int32_t temp = 2000 + (((int64_t)delta_t * calibration_data.tempsens) >> 23);

    // Calculate initial offset and sensitivity
    int64_t off  = ((int64_t)calibration_data.off << 16) + (((int64_t)calibration_data.tco * delta_t) >> 7);
    int64_t sens = ((int64_t)calibration_data.sens << 15) + (((int64_t)calibration_data.tcs * delta_t) >> 8);

    // Second order temperature compensation
    int64_t temp_2 = 0;
    int64_t off_2  = 0;
    int64_t sens_2 = 0;

    // If temperature is below 20°C
    if (temp < 2000) {
        temp_2 = ((int64_t)delta_t * delta_t) >> 31;
        off_2  = 5 * ((int64_t)(temp - 2000) * (temp - 2000)) >> 1;
        sens_2 = 5 * ((int64_t)(temp - 2000) * (temp - 2000)) >> 2;

        // If temperature if below -15°C
        if (temp < -1500) {
            off_2  = off_2 + (7 * ((int64_t)(temp + 1500) * (temp + 1500)));
            sens_2 = sens_2 + (11 * ((int64_t)(temp + 1500) * (temp + 1500)) >> 1);
        }
    }

    temp -= temp_2;
    off  -= off_2;
    sens -= sens_2;

    // Calculate temperature compensated pressure
    int32_t pressure = (((d1 * sens) >> 21) - off) >> 15;

    data->temperature = temp / 100.0f; // Convert to degrees Celsius
    data->pressure    = pressure / 100.0f; // Convert to mbar

    return HAL_OK;
}