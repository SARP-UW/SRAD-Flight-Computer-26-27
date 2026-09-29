/**
 * @file App/Devices/Inc/led.h
 * @authors Michael Zheng
 * @brief LED driver
 */

#pragma once

typedef enum {
	LED_MCU,
	LED_APOGEE,
	LED_APOGEE_BACKUP,
	LED_MAIN,
	LED_MAIN_BACKUP
} led_t;

/**
 * @brief Turns the selected LED on
 */
void led_on(led_t led);

/**
 * @brief Turns the selected LED off
 */
void led_off(led_t led);

/**
 * @brief Toggles the selected LED state
 */
void led_toggle(led_t led);