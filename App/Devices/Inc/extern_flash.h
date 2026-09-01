// Comment

#pragma once

#include <stdint.h>
#include "stm32f4xx_hal.h"
#include "bmi088.h"
#include "bmp390.h"
#include "states.h"

typedef struct {
    uint32_t timestamp;      // Timestamp of the data packet
    State state;             // Current state of the system
    BMI088_Data bmi088_data; // Data from the BMI088 sensor
    BMP390_Data bmp390_data; // Data from the BMP390 sensor
    bool pyro_fire;          // Was a pyro fired during this packet? True if yes, false if no
} Packet;

HAL_StatusTypeDef init_extern_flash();

HAL_StatusTypeDef write_packet_to_extern_flash(Packet* packet);