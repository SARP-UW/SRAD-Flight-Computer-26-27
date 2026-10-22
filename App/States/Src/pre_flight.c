/**
 * @file App/States/Src/pre_flight.c
 * @authors Shivani Palanikumar
 * @brief Pre-flight state draft
 */


 /*
 NOTES:
 - Currently, the check to move into ascent state checks for altitude and acceleration. This makes the algorithm 
   more reliable, but might make software testing more complicated. 
    - Could maybe switch to just 1 check later?
 - We should also eventually add USB compatibility once we have the device drivers

 - Error handling is also a little complicated. Not sure how much we want to do with it (maybe an idea for bluetooth?)
    - Not sure if I overcomplicated it...
    - Currently, if the sensors fail while initializing, the rest of the state won't run until fixed + buzzer will sound
        - also not sure if it's ok to keep trying to initialize the sensors repeatedly if they don't work the first time
    - If the sensors fail while collecting data, buzzer sounds and pre-flight continues to loop until sensors are fixed
 */


#include "pre_flight.h"
#include <stdbool.h>
#include "led.h"
#include "bmi088.h"
#include "ms5611.h"
#include "pyro.h"


bmi088_data_t imu_data;
bool preflight_init = false;
bool sensor_error;

State update_pre_flight(State_Context *context) {
    sensor_error = false; //resets flag every cycle to keep checking if errors have been fixed
    
    if (!preflight_init){

        HAL_StatusTypeDef imu_init_status = init_bmi088();
        HAL_StatusTypeDef imu_calibrate_status = calibrate_accel_offset(&imu_data);  
        HAL_StatusTypeDef baro_init_status = init_ms5611(OSR_4096);

        if (imu_init_status != HAL_OK || imu_calibrate_status != HAL_OK || baro_init_status != HAL_OK){
            // if any of the sensors fail, pre-flight doesn't initialize and buzzer will sound until fixed
            buzzer_set_frequency(4000); 
            buzzer_on();
            
        } else {
            buzzer_off();
            led_on(LED_MCU);   // if all sensors initialize, turn on LED and configure baro 
            baro_configure_ground_pressure();
            preflight_init = true;
        }
    }

    if (preflight_init){
    
        //check continuity of all pyro channels
        if (check_continuity(MAIN)){ 
            led_on(LED_MAIN); //if continuity, turn on LED
        } else {
            led_off(LED_MAIN);
        }

        if (check_continuity(MAIN_BACKUP)){ 
            led_on(LED_MAIN_BACKUP);
        } else {
            led_off(LED_MAIN_BACKUP);
        }

        if (check_continuity(APOGEE)){ 
            led_on(LED_APOGEE);
        } else {
            led_off(LED_APOGEE);
        }

        if (check_continuity(APOGEE_BACKUP)){ 
            led_on(LED_APOGEE_BACKUP);
        } else {
            led_off(LED_APOGEE_BACKUP);
        }


        // collect baro and imu data
        // if there is an error with any of the sensors, the buzzer will sound at loudest output until all errors are fixed
        context->status = update_bmi088(&imu_data); //collect data from imu
        if (context->status != HAL_OK){
            sensor_error = true;
        }

        context->status = baro_estimate_altitude(&context->altitude); // collect data from baro
        if (context->status != HAL_OK){
            sensor_error = true;
        }

        if (sensor_error){
            buzzer_set_frequency(4000); // set buzzer to loudest output
            buzzer_on();
        } else {
            buzzer_off();
        }

        // ascent state if the altitude is greater than 10 and accel is over 2.5 gs 
        if(!sensor_error && context -> altitude >= 10 && imu_data.accel_z >= 24.5){  // accel in m/s^2
            led_off(LED_MCU); 
            return ASCENT;
        }   
    }

    return PRE_FLIGHT;
    
}