/**
 * @file App/Sensors/Inc/bmi088.c
 * @authors Jude Merritt
 * @brief BMI088 IMU driver
 */

/**
 * Notes:
 * - 
 */

#include "bmi088.h"
#include "spi.h"
#include "gpio.h"

/**************************************************************************************************
 * @section Type definitions
 **************************************************************************************************/

typedef enum {
    ACCEL,
    GYRO
} sensor_t;

/**************************************************************************************************
 * @section Definititions and global variables
 **************************************************************************************************/

// Basic Commands

// Configuration variables
#define BMI088_ACCEL_OSR 0x0A  // 400 Hz ODR. Can be changed if needed
#define BMI088_GYRO_OSR  0x02  // 1000 Hz ODR with 166 Hz filter bandwidth. Can be changed if needed

// SPI
extern SPI_HandleTypeDef hspi1;
static const uint8_t timeout = 10; // 10ms timeout for SPI transfers
#define ACCEL_CS_PORT GPIOA        // CHAGE ME WHEN YOU HAVE THE ACTUAL CS PORT
#define ACCEL_CS_PIN GPIO_PIN_4    // CHAGE ME WHEN YOU HAVE THE ACTUAL CS PORT
#define GYRO_CS_PORT GPIOA         // CHAGE ME WHEN YOU HAVE THE ACTUAL CS PORT
#define GYRO_CS_PIN GPIO_PIN_4     // CHAGE ME WHEN YOU HAVE THE ACTUAL CS PORT

// Accelerometer Registers
#define ACC_CHIP_ID  0x00 // Manufacturer ID
#define ACC_PWR_CTRL 0x7D // Toggle accelerometer power (0x00: off, 0x04: on)
#define ACC_CONF     0x40 // Accelerometer configuration
#define ACC_RANGE    0x41 // Accelerometer range (in g)
#define ACC_XYZ      0x12 // 0x12 - 0x17 is accel data: X, then Y, then Z; each two bytes (LSB first)

// Gyroscope Registers
#define GYRO_CHIP_ID   0x00 // Manufacturer ID
#define GYRO_RANGE     0x0F // Gyroscope range (in degrees / second)
#define GYRO_BANDWIDTH 0x10 // Basically a low pass filter. High frequency fluxuations can be filtered
#define GYRO_XYZ       0x02 // 0x02 - 0x07 is gyro data: X, then Y, then Z; each two bytes (LSB first)

/**************************************************************************************************
 * @section Private function definitions
 **************************************************************************************************/

static HAL_StatusTypeDef send_command(sensor_t sensor, uint8_t cmd) {
    GPIO_TypeDef *cs_port;
    uint16_t cs_pin;

    switch (sensor) {
        case ACCEL:
            cs_port = ACCEL_CS_PORT;
            cs_pin = ACCEL_CS_PIN;
            break;

        case GYRO:
            cs_port = GYRO_CS_PORT;
            cs_pin = GYRO_CS_PIN;
            break;

        default:
            return HAL_ERROR;
    }

    HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_PIN_RESET);
    HAL_StatusTypeDef status = HAL_SPI_Transmit(&hspi1, &cmd, 1, timeout);
    HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_PIN_SET);

    return status;
}

static HAL_StatusTypeDef write_reg(sensor_t sensor, uint8_t cmd, uint8_t value) {
    uint8_t tx[2] = {cmd, value};

    GPIO_TypeDef *cs_port;
    uint16_t cs_pin;

    switch (sensor) {
        case ACCEL:
            cs_port = ACCEL_CS_PORT;
            cs_pin = ACCEL_CS_PIN;
            break;

        case GYRO:
            cs_port = GYRO_CS_PORT;
            cs_pin = GYRO_CS_PIN;
            break;

        default:
            return HAL_ERROR;
    }

    HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_PIN_RESET);
    HAL_StatusTypeDef status = HAL_SPI_Transmit(&hspi1, tx, 2, timeout);
    HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_PIN_SET);

    return status;
}

static float convert_accel_data_ms2(int16_t raw_data) {
    // Convert raw accelerometer data to m/s^2
}

static float convert_gyro_data_rads(int16_t raw_data) {
    // Convert raw gyroscope data to rad/s
}

/**************************************************************************************************
 * @section Public function definitions
 **************************************************************************************************/

 HAL_StatusTypeDef init_BMI088(BMI088_Data *data) {
    HAL_StatusTypeDef status;

    status = send_command(ACC_CHIP_ID, ACCEL);




    return HAL_OK;
}

HAL_StatusTypeDef update_BMI088(BMI088_Data *data) {
    // Update code for the BMI088 sensor
    return HAL_OK;
}