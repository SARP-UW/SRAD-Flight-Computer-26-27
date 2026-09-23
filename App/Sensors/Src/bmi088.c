/**
 * @file App/Sensors/Inc/bmi088.c
 * @authors Jude Merritt
 * @brief BMI088 IMU driver
 * 
* Datasheet: Bosch BMI088 — JLCPCB C194919
 * https://jlcpcb.com/partdetail/BoschSensortec-BMI088/C194919
 */

/**
 * Notes:
 * - 
 */

#include "bmi088.h"
#include "spi.h"
#include "gpio.h"
#include "pinout.h"

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

// Accelerometer Registers
// ref: Section 5.2 and 5.3 in the datasheet
#define ACC_CHIP_ID   0x00 // Manufacturer ID
#define ACC_SOFTRESET 0x7E // Reset accelerometer
#define ACC_CONF      0x40 // Accelerometer configuration
#define ACC_RANGE     0x41 // Accelerometer range (in g)
#define ACC_PWR_CTRL  0x7D // Toggle accelerometer power (0x00: off, 0x04: on)
#define ACC_XYZ       0x12 // 0x12 - 0x17 is accel data: X, then Y, then Z; each two bytes (LSB first)

// Accelerometer configuration variables
// ref: Section 5.2 and 5.3 in the datasheet
#define ACCEL_RESET  0xB6 // Reset accelerometer
#define ACCEL_ODR    0x0A // 400 Hz ODR
#define ACCEL_BWP    0x0A // Normal bandwidth
#define ACCEL_RANGE  0x03 // +- 24g
#define ACCEL_PWR_ON 0x04 // Accelerometer on

// Gyroscope Registers
// ref: Section 5.4 and 5.5 in the datasheet
#define GYRO_CHIP_ID   0x00 // Manufacturer ID
#define GYRO_SOFTRESET 0x14 // Reset gyroscope
#define GYRO_RANGE     0x0F // Gyroscope range (in degrees / second)
#define GYRO_BANDWIDTH 0x10 // Basically a low pass filter. High frequency fluxuations can be filtered
#define GYRO_XYZ       0x02 // 0x02 - 0x07 is gyro data: X, then Y, then Z; each two bytes (LSB first)

// Gyroscope configuration variables
// ref: Section 5.4 and 5.5 in the datasheet
#define GYRO_RESET_VAL     0xB6 // Reset gyroscope
#define GYRO_BANDWIDTH_VAL 0x06 // 200 Hz ODR, 64 Hz filter bandwidth
#define GYRO_RANGE_VAL     0x00 // +- 2000 degrees / second

// SPI
extern SPI_HandleTypeDef hspi1;
static const uint8_t timeout = 10; // 10ms timeout for SPI transfers

/**************************************************************************************************
 * @section Private function definitions
 **************************************************************************************************/

// This function is used to send a single command to the BMI088. 
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

// This function is used to write data to a register on the BMI088.
static HAL_StatusTypeDef write_reg(sensor_t sensor, uint8_t cmd, const uint8_t *data, uint8_t length) {
    uint8_t tx[3];

    tx[0] = cmd;

    for (uint8_t i = 0; i < length; i++) {
        tx[i + 1] = data[i];
    }

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
    HAL_StatusTypeDef status = HAL_SPI_Transmit(&hspi1, tx, length + 1, timeout);
    HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_PIN_SET);

    return status;
}

// This function is used to read data from a register on the BMI088.
static HAL_StatusTypeDef read_reg(sensor_t sensor, uint8_t cmd, uint8_t *rx, uint8_t length) {
    uint8_t tx[7] = {0};

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

    tx[0] = cmd | 0x80;

    HAL_StatusTypeDef status;

    HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_PIN_RESET);
    if (sensor == ACCEL) {
        status = HAL_SPI_TransmitReceive(&hspi1, tx, rx, length + 1, timeout);

        if (status == HAL_OK) {
            for (uint8_t i = 0; i < length; i++) {
                rx[i] = rx[i + 1];
            }
        }
    }
    else {
        status = HAL_SPI_TransmitReceive(&hspi1, tx, rx, length, timeout);
    }
    HAL_GPIO_WritePin(cs_port, cs_pin, GPIO_PIN_SET);

    return status;
}

// This function is used to convert raw accelerometer data to m/s^2.
static float convert_accel_data_ms2(int16_t raw_data) {
    // At +-24g, the sensitivity is 1365 LSB/g. 
    // ref: Section 1.2 in the datasheet

    // LSB / (LSB/g) = g, g * (9.80665 m/s^2 / 1g) = m/s^2
    return ((float)raw_data / 1365.0f) * 9.80665f;
}

// This function is used to convert raw gyroscope data to radians per second.
static float convert_gyro_data_rads(int16_t raw_data) {
    // At +-2000 degrees/second, the sensitivity is 16.384 LSB/(degrees/second).
    // ref: Section 1.3 in the datasheet
    
    // LSB / (LSB/(degrees/second)) = degrees/second, degrees/second * (pi radians / 180 degrees) = radians/second
    return ((float)raw_data / 16.384f) * (3.14159265f / 180.0f);
}

/**************************************************************************************************
 * @section Public function definitions
 **************************************************************************************************/

 HAL_StatusTypeDef init_bmi088(void) {
    HAL_StatusTypeDef status;

    // Random transfer to select spi (instead of i2c)for the accelerometer
    status = send_command(ACCEL, ACC_CHIP_ID); 
    if (status != HAL_OK) {
        return status;
    }

    // Varify accelerometer manufacturer ID
    uint8_t acc_id;
    status = read_reg(ACCEL, ACC_CHIP_ID, &acc_id, 1);
    if (status != HAL_OK) {
        return status;
    } else if (acc_id != 0x1E) {
        return HAL_ERROR;
    }

    // Varify gyroscope manufacturer ID
    uint8_t gyro_id;
    status = read_reg(GYRO, GYRO_CHIP_ID, &gyro_id, 1);
    if (status != HAL_OK) {
        return status;
    } else if (gyro_id != 0x0F) {
        return HAL_ERROR;
    }

    // Accelerometer configuration
    // ref: Section 4 of the datasheet covers the configuration of the accelerometer.
    uint8_t accel_reset = ACCEL_RESET;
    status = write_reg(ACCEL, ACC_SOFTRESET, &accel_reset, 1);
    if (status != HAL_OK) {
        return status;
    }

    uint8_t accel_conf_data = (ACCEL_ODR << 4) | ACCEL_BWP;
    status = write_reg(ACCEL, ACC_CONF, &accel_conf_data, 1);
    if (status != HAL_OK) {
        return status;
    }

    uint8_t accel_range = ACCEL_RANGE;
    status = write_reg(ACCEL, ACC_RANGE, &accel_range, 1);
    if (status != HAL_OK) {
        return status;
    }

    uint8_t accel_pwr = ACCEL_PWR_ON;
    status = write_reg(ACCEL, ACC_PWR_CTRL, &accel_pwr, 1);
    if (status != HAL_OK) {
        return status;
    }

    // Gyroscope configuration
    // ref: Section 4 of the datasheet covers the configuration of the gyroscope.
    uint8_t gyro_reset = GYRO_RESET_VAL;
    status = write_reg(GYRO, GYRO_SOFTRESET, &gyro_reset, 1);
    if (status != HAL_OK) {
        return status;
    }

    uint8_t gyro_bandwidth = GYRO_BANDWIDTH_VAL;
    status = write_reg(GYRO, GYRO_BANDWIDTH, &gyro_bandwidth, 1);
    if (status != HAL_OK) {
        return status;
    }

    uint8_t gyro_range = GYRO_RANGE_VAL;
    status = write_reg(GYRO, GYRO_RANGE, &gyro_range, 1);
    if (status != HAL_OK) {
        return status;
    }

    return HAL_OK;
}

HAL_StatusTypeDef update_BMI088(bmi088_data_t *data) {
    uint8_t accel_data[6];
    uint8_t gyro_data[6];

    HAL_StatusTypeDef status;

    // Read accelerometer X, Y, Z
    status = read_reg(ACCEL, ACC_XYZ, accel_data, 6);
    if (status != HAL_OK) {
        return status;
    }

    // Read gyroscope X, Y, Z
    status = read_reg(GYRO, GYRO_XYZ, gyro_data, 6);
    if (status != HAL_OK) {
        return status;
    }

    // Convert accelerometer data
    // ref: Section 5.3.4 in the datasheet
    int16_t accel_x = (int16_t)((accel_data[1] << 8) | accel_data[0]);
    int16_t accel_y = (int16_t)((accel_data[3] << 8) | accel_data[2]);
    int16_t accel_z = (int16_t)((accel_data[5] << 8) | accel_data[4]);

    data->accel_x = convert_accel_data_ms2(accel_x);
    data->accel_y = convert_accel_data_ms2(accel_y);
    data->accel_z = convert_accel_data_ms2(accel_z);

    // Convert gyroscope data
    // ref: Section 5.5.2 in the datasheet
    int16_t gyro_x = (int16_t)((gyro_data[1] << 8) | gyro_data[0]);
    int16_t gyro_y = (int16_t)((gyro_data[3] << 8) | gyro_data[2]);
    int16_t gyro_z = (int16_t)((gyro_data[5] << 8) | gyro_data[4]);

    data->gyro_x = convert_gyro_data_rads(gyro_x);
    data->gyro_y = convert_gyro_data_rads(gyro_y);
    data->gyro_z = convert_gyro_data_rads(gyro_z);

    return HAL_OK;
}