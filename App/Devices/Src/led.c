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

void led_init(void) {
    GPIO_InitTypeDef gpio_init = {0};

    __HAL_RCC_GPIOA_CLK_ENABLE();   // CHANGE ME WHEN YOU HAVE THE ACTUAL LED PORT
    HAL_GPIO_WritePin(MCU_LED_PORT, MCU_LED_PIN, MCU_LED_OFF);

    gpio_init.Pin = MCU_LED_PIN;
    gpio_init.Mode = GPIO_MODE_OUTPUT_PP; // Drives both high (VDD) and low (GND)
    gpio_init.Pull = GPIO_NOPULL; // No need for internal pull up/down resistors
    gpio_init.Speed = GPIO_SPEED_FREQ_LOW; // Not expecting switching faster than a few Hz
    HAL_GPIO_Init(MCU_LED_PORT, &gpio_init);
}

void led_on(void) {
    HAL_GPIO_WritePin(MCU_LED_PORT, MCU_LED_PIN, MCU_LED_ON);
}

void led_off(void) {
    HAL_GPIO_WritePin(MCU_LED_PORT, MCU_LED_PIN, MCU_LED_OFF);
}

void led_toggle(void) {
    HAL_GPIO_TogglePin(MCU_LED_PORT, MCU_LED_PIN);
}
