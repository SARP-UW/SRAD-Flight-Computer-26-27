// Comment

#pragma once

#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "bmi088.h"
#include "ms5611.h"
#include "states.h"

typedef struct {
    uint32_t timestamp;        // Timestamp of the data packet
    State state;               // Current state of the system
    bmi088_data_t bmi088_data; // Data from the BMI088 sensor
    bmp390_data_t bmp390_data; // Data from the BMP390 sensor
    bool pyro_fire;            // Was a pyro fired during this packet? True if yes, false if no
} Packet;

HAL_StatusTypeDef init_extern_flash(void);

HAL_StatusTypeDef write_packet_to_extern_flash(Packet* packet);