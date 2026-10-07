/**
 * @file App/Devices/Src/led.c
 * @authors Michael Zheng
 * @brief LED driver
 */

/**
 * Notes:
 */

#include "led.h"
#include "pinout.h"
#include "stm32f4xx_hal.h"

/**************************************************************************************************
 * @section Definitions and global variables
 **************************************************************************************************/

static GPIO_TypeDef *led_port(led_t led) {
    switch (led) {
        case LED_MCU:
            return LED_MCU_PORT;
        case LED_APOGEE:
            return LED_APOGEE_PORT;
        case LED_APOGEE_BACKUP:
            return LED_APOGEE_BACKUP_PORT;
        case LED_MAIN:
            return LED_MAIN_PORT;
        case LED_MAIN_BACKUP:
            return LED_MAIN_BACKUP_PORT;
        default:
            return NULL;
    }
}

static uint16_t led_pin(led_t led) {
    switch (led) {
        case LED_MCU:
            return LED_MCU_PIN;
        case LED_APOGEE:
            return LED_APOGEE_PIN;
        case LED_APOGEE_BACKUP:
            return LED_APOGEE_BACKUP_PIN;
        case LED_MAIN:
            return LED_MAIN_PIN;
        case LED_MAIN_BACKUP:
            return LED_MAIN_BACKUP_PIN;
        default:
            return 0;
    }
}

/**************************************************************************************************
 * @section Public function definitions
 **************************************************************************************************/

void led_on(led_t led) {
    HAL_GPIO_WritePin(led_port(led), led_pin(led), GPIO_PIN_SET);
}

void led_off(led_t led) {
    HAL_GPIO_WritePin(led_port(led), led_pin(led), GPIO_PIN_RESET);
}

void led_toggle(led_t led) {
    HAL_GPIO_TogglePin(led_port(led), led_pin(led));
}
