//Temp
/**
 * @file App/States/Src/pre_flight.c
 * @authors Shivani Palanikumar
 * @brief Pre-flight state draft
 */


 /*
 NOTES:
 - Currently, the check to move into ascent state checks for altitude
    - We could add a check for acceleration, but would make testing more complicated
 - We should also eventually add USB compatibility once we have the device drivers
 - Error handling updated to implement safe state
    - can use bluetooth to check which sensor is failing to help debug
 */


#include "pre_flight.h"
#include <stdbool.h>
#include "led.h"
#include "bmi088.h"
#include "ms5611.h"
#include "pyro.h"
#include "stm32f4xx_hal_gpio.h"
#include <pinout.h>
#include "stm32f411xe.h"


bmi088_data_t imu_data;
bool preflight_init = false;

void toggle_continuity_leds(PyroChannel channel, GPIO_TypeDef* led_port, uint16_t led_pin, led_t led){
    if (check_continuity(channel)){
        if (HAL_GPIO_ReadPin(led_port, led_pin) != GPIO_PIN_SET){ //check if led is already on
            led_on(led); 
        }
    } else if (HAL_GPIO_ReadPin(led_port, led_pin) != GPIO_PIN_RESET){ // check if led is off
        led_off(led);
    }
}

State update_pre_flight(State_Context *context) {
    
    if (!preflight_init){

        HAL_StatusTypeDef imu_init_status = init_bmi088();
        HAL_StatusTypeDef baro_init_status = init_ms5611(OSR_4096);

        if (imu_init_status != HAL_OK || baro_init_status != HAL_OK){
            return SAFE;
        } 
        
        led_on(LED_MCU);  
        baro_configure_ground_pressure();
        preflight_init = true;
        
    }

    if (preflight_init){
    
        //check continuity of all pyro channels
        toggle_continuity_leds(MAIN, LED_MAIN_PORT, LED_MAIN_PIN, LED_MAIN);
        toggle_continuity_leds(MAIN_BACKUP, LED_MAIN_BACKUP_PORT, LED_MAIN_BACKUP_PIN, LED_MAIN_BACKUP);
        toggle_continuity_leds(APOGEE, LED_APOGEE_PORT, LED_APOGEE_PIN, LED_APOGEE);
        toggle_continuity_leds(APOGEE_BACKUP, LED_APOGEE_BACKUP_PORT, LED_APOGEE_BACKUP_PIN, LED_APOGEE_BACKUP);


        // collect baro and imu data
        // if there is an error with any of the sensors, goes to safe state
        context->status = update_bmi088(&imu_data); //collect data from imu
        if (context->status != HAL_OK){
            return SAFE;
        }

        context->status = baro_estimate_altitude(&context->altitude); // collect data from baro
        if (context->status != HAL_OK){
            return SAFE;
        }


        // ascent state if the altitude is greater than 10 
        if(context -> altitude >= 10){  
            led_off(LED_MCU); 
            return ASCENT;
        }   
    }

    return PRE_FLIGHT;
    
}
