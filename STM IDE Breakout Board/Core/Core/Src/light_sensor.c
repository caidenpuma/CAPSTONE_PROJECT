/* SoilSense F4 Debug
 * Functions for operation of light sensor VEML7700
 */

#include "stm32f4xx_hal.h"
#include "user_interaction.h"
#include "setup.h"
#include "veml7700.h"
#include <stdint.h>
#include <math.h>

#include "light_sensor.h"

uint32_t light_int_24_hrs[144]; 		// stores 24 hours worth of 6 light intensity readings/hour
uint32_t daily_optimal_time[3]; 		// stores last 3 days of time in optimal light, in minutes
uint32_t daily_max_ints[3]; 			// stores last 3 days of max hourly intensities, in lux

uint32_t light_24h_counter = 0;			// GLOBAL: 0-143 for light_int_24_hrs, value updated in main
uint32_t light_3day_counter = 0;		// GLOBAL: 0-2 for both daily_x arrays, value updated in main

/* Converts VEML7700 output to lux value (formula in application note)
 * Parameters:
 * - veml_raw: raw output of VEML7700 I2C read
 * Returns: lux value corresponding to input
 * Note: correction formula output doesn't match what is in datasheet but does match exactly with calculator
 */
static uint32_t convert_to_lux(uint16_t veml_raw){
	uint32_t lux;
	// for ALS gain = 1/4 and integration time = 100ms
	lux = veml_raw * 0.2688;
	if(lux>1000){ // correction formula
		double lux_a = pow(6.0135,-13);
		double lux_b = pow(-9.3924,-9);
		double lux_c = pow(8.1488,-5);
		double lux_d = 1.0023;
		lux = lux_a*pow((double)lux,4) + lux_b*pow((double)lux,3) + lux_c*pow((double)lux,2) + lux_d*lux;
	}
	return lux;
}

/* Reads values of light sensor
 * Parameters:
 * - veml: pointer to initialized veml
 * Returns: raw ADC reading
 */
static uint16_t light_i2c_read(veml7700 *veml){
	uint16_t als_raw;
	veml7700_power_on(veml);
	// according to datasheet should need delay of 2.5 ms after wakeup
	HAL_Delay(115); // experimentally: doesn't work at 112, does work at 115
	als_raw = veml7700_read_als(veml);
	// turn off to save power
	veml7700_shutdown(veml);
	return als_raw;

}

/* Takes light level category of plant and outputs high end of light range
 * Parameters:
 * - light_level: light level parameter of user's selected plant
 * Returns: int corresponding to upper bound of acceptable lux range
 */
static uint32_t light_level_to_lux_high(uint8_t light_level){
    switch(light_level) {
        case 1:
            return 1076; // low light
            break;
        case 2:
            return 5382; // medium light
            break;
        case 3:
            return 10764; // bright indirect light
            break;
        case 4:
            return 99999; // direct light
            break;
        default:
        	return 0;
        	break;
    }
}

/* Takes light level category of plant and outputs low end of light range
 * Parameters:
 * - light_level: light level parameter of user's selected plant
 * Returns: int corresponding to lower bound of acceptable lux range
 */
static uint32_t light_level_to_lux_low(uint8_t light_level){
    switch(light_level) {
        case 1:
            return 270; // low light
            break;
        case 2:
            return 1076; // medium light
            break;
        case 3:
            return 5382; // bright indirect light
            break;
        case 4:
            return 10750; // direct light
            break;
        default:
        	return 0;
        	break;
    }
}

/* Calculates the daily time spent in optimal light range
 * Parameters:
 * - light_level: light level parameter of user's selected plant
 */
void mins_in_optimal_light(int light_level){
	uint32_t optimal_time, high, low;
	optimal_time = 0;
	high = light_level_to_lux_high(light_level);
	low = light_level_to_lux_low(light_level);
    for(int i=0; i<144; i++){
    	uint32_t light = light_int_24_hrs[i];
        if((light<high) && (light>low)){
            optimal_time++;
        }
    }
    daily_optimal_time[light_3day_counter]=optimal_time*10;
}

/* Calculates average daily time in optimal light
 * Returns: average daily time in optimal light range over last 3 days in minutes
 */
static uint32_t average_optimal_light_time(){
	uint32_t sum, avg;
	sum = 0;
    for(int i=0; i<3; i++){
        sum += daily_optimal_time[i];
    }
    avg = sum/3;
    return avg;
}

/* Calculates average max light intensity in optimal light range
 * Returns: average max light intensity in optimal light range over last 3 days, in lux
 */
static uint32_t average_optimal_light_ints(){
	uint32_t sum, avg;
	sum = 0;
    for(int i=0; i<3; i++){
        sum += daily_max_ints[i];
    }
    avg = sum/3;
    return avg;
}

/* Takes 3 readings of light meter 2 seconds apart and averages them to get value
 * Stores values in array light_int_24_hrs
 */
void get_light_value(veml7700 *veml){
    uint32_t sum, avg, lux;
    uint16_t reading;
    sum = 0;
    for(int i=0; i<3; i++) {
        reading = light_i2c_read(veml);
        lux = convert_to_lux(reading);
        sum += lux;
        HAL_Delay(2000);
    }
    avg = sum/3;
    light_int_24_hrs[light_24h_counter] = avg;
}

/* Calculates peak average hourly light intensity of one day by
 * checking if current hourly average higher than previous max
 */
void calc_high_intensity(){
	uint32_t sum, intensity, last_hour_intensity;
	sum=0;
    static uint32_t max_hourly_intensity = 0;
    // average last hour of intensities
    if(light_24h_counter>4) { // wait for at least one hour of readings
        for(int i=0; i<6; i++){
            intensity = light_int_24_hrs[light_24h_counter-i];
            sum += intensity;
        }
        last_hour_intensity = sum/6;
        // if higher than current average update as new max
        if (last_hour_intensity > max_hourly_intensity){
            max_hourly_intensity = last_hour_intensity;
        }
    }
    if (light_24h_counter==143){ // at end of day store daily max and reset max
        daily_max_ints[light_3day_counter] = max_hourly_intensity;
        max_hourly_intensity = 0;
    }
}

/* Check if 3-day average light intensity in acceptable range
 */
void check_light_intensity(){
	uint32_t max_ok_light, min_ok_light, avg_intensity;
	max_ok_light = light_level_to_lux_high(user_selected_plant.light_level);
	min_ok_light = light_level_to_lux_low(user_selected_plant.light_level);
	avg_intensity = average_optimal_light_ints();
    if (avg_intensity>max_ok_light){ // too bright
        update_alert(3,1);
    } else {
    	update_alert(3,0);
    }
    if (avg_intensity<min_ok_light){ // too dark
        update_alert(2,1);
    }
    else {
        update_alert(2,0);
    }
}

/* Demo version of check_light_intensity
 * Check only if current light reading is within acceptable range
 */
void DEMO_check_light_intensity(){
	uint32_t max_ok_light, min_ok_light, intensity;
	max_ok_light = light_level_to_lux_high(user_selected_plant.light_level);
	min_ok_light = light_level_to_lux_low(user_selected_plant.light_level);
	intensity = light_int_24_hrs[light_24h_counter];
    if (intensity>max_ok_light){ // too bright
        update_alert(3,1);
    } else {
    	update_alert(3,0);
    }
    if (intensity<min_ok_light){ // too dark
        update_alert(2,1);
    }
    else {
        update_alert(2,0);
    }
}

/* Check if 3-day average light time in acceptable range (within 1 hr of ideal)
 */
void check_light_time(){
	uint32_t measured_light_mins, ideal_light_mins;
	measured_light_mins = average_optimal_light_time();
	ideal_light_mins = user_selected_plant.light_time * 60;
    if (measured_light_mins<(ideal_light_mins-60)){ // not enough light
        update_alert(0,1);
    }
    else{
    	update_alert(0,0);
    }
    if (measured_light_mins>(ideal_light_mins+60)){ // too much light
        update_alert(1,1);
    }
    else {
        update_alert(1,0);
    }
}
