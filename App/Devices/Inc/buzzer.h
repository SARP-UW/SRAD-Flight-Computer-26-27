/**
 * @file App/Devices/Inc/buzzer.h
 * @authors Michael Zheng
 * @brief ...
 */

#pragma once

#include <stdint.h>
#include "stm32f4xx_hal.h"

//comment
void init_buzzer(void);

//comment
void buzzer_on(void);

//comment
void buzzer_off(void);