/* SoilSense
 * Functions for operation of temp/humidity sensor
 */

#include "stm32l4xx_hal.h"
#include "setup.h"
#include "user_interaction.h"
#include "dht.h"
#include <stdint.h>

#include "temp_rh_sensor.h"

uint16_t temp_1_hr[6];	// stores last 6 (1 hour) temperature readings in degrees F
uint16_t rh_1_hr[6];	// stores last 6 (1 hour) humidity readings in %RH

DHT_DataTypedef dht_data; // struct to store temp and rh values

/* Calculates average temperature over last hour of readings
 * Returns: average temperature
 */
static uint16_t hourly_avg_temp(){
	uint32_t sum;
	uint16_t t_avg;
	sum = 0;
    for(int i=0; i<6; i++){
        sum += temp_1_hr[i];
    }
    t_avg = sum/6;
    return t_avg;
}

/* Calculates average rel. humidity over last hour of readings
 * Returns: average rel. humidity
 */
static uint16_t hourly_avg_rh(){
	uint32_t sum;
	uint16_t rh_avg;
	sum = 0;
    for(int i=0; i<6; i++){
        sum += rh_1_hr[i];
    }
    rh_avg = sum/6;
    return rh_avg;
}

/* Takes 5 readings of temp and rh and averages them to get value
 * Stores values in temp/rh_1_hr array
 * Called in RTC ISR
 */
void get_temp_and_rh_value(){
    float t_sum, rh_sum;
    uint16_t t_avg, rh_avg;
	static uint16_t trh_index = 0;
    t_sum=0; rh_sum=0;
    float t_read, rh_read;
    for(int i=0; i<5; i++) {
    	DHT_GetData(&dht_data);
    	t_read = dht_data.Temperature;
    	rh_read = dht_data.Humidity;
        t_sum += t_read;
        rh_sum += rh_read;
        //HAL_Delay(1000); // NOTE: probably want to put this back?
    }
    t_avg = t_sum/5;
    rh_avg = rh_sum/5;
    temp_1_hr[trh_index] = t_avg;
    rh_1_hr[trh_index] = rh_avg;
    (trh_index == 5) ? (trh_index=0) : (trh_index++); //++ or wrap index
}

/* Checks if current average temperature in acceptable range, triggers alert if not
 */
void check_temp(){
    if (hourly_avg_temp()<user_selected_plant.low_temp){ // too cold
        update_alert(5,1);
    }
    else {
    	update_alert(5,0);
    }
    if (hourly_avg_temp()>user_selected_plant.high_temp){ // too hot
        update_alert(4,1);
    }
    else {
        update_alert(4,0);
    }
}

/* Checks if RH in acceptable range, triggers alert if not
 */
void check_rh(){
    if (hourly_avg_rh()<user_selected_plant.low_humidity){ // too dry
        update_alert(7,1);
    }
    else {
    	update_alert(7,0);
    }
    if (hourly_avg_rh()>user_selected_plant.high_humidity){ // too wet
        update_alert(6,1);
    }
    else {
        update_alert(6,0);
    }
}
