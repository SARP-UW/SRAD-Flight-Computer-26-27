// Comment

#pragma once

#include <stdint.h>
#include <stdbool.h>
#include "stm32f4xx_hal.h"
#include "bmi088.h"
#include "ms5611.h"
#include "states.h"

typedef struct {
    uint32_t timestamp;        // Timestamp of the data packet
    State state;               // Current state of the system
    bmi088_data_t bmi088_data; // Data from the BMI088 sensor
    ms5611_data_t ms5611_data; // Data from the MS5611 sensor
    bool pyro_fire;            // Was a pyro fired during this packet? True if yes, false if no
    bool backup_pyro_fire;     // Was a backup pyro fired during this packet? True if yes, false if no
    HAL_StatusTypeDef status;  // Status of the data packet
} Packet;

HAL_StatusTypeDef init_extern_flash(void);

HAL_StatusTypeDef write_packet_to_extern_flash(Packet* packet);