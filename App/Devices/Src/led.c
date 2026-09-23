/**
 * @file App/Devices/Src/led.c
 * @authors Michael Zheng
 * @brief LED driver
 */

/**
 * Notes:
 * - 
 */

#include "led.h"
#include "pinout.h"
#include "stm32f4xx_hal.h"

/**************************************************************************************************
 * @section Definitions and global variables
 **************************************************************************************************/

#define MCU_LED_ON GPIO_PIN_SET
#define MCU_LED_OFF GPIO_PIN_RESET

/**************************************************************************************************
 * @section Public function definitions
 **************************************************************************************************/

void led_on(void) {
    HAL_GPIO_WritePin(MCU_LED_PORT, MCU_LED_PIN, MCU_LED_ON);
}

void led_off(void) {
    HAL_GPIO_WritePin(MCU_LED_PORT, MCU_LED_PIN, MCU_LED_OFF);
}

void led_toggle(void) {
    HAL_GPIO_TogglePin(MCU_LED_PORT, MCU_LED_PIN);
}
