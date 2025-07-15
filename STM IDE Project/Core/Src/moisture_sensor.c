/* SoilSense
 * Functions for operation of moisture sensor
 */

#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_adc.h"
#include "adc.h"
#include "setup.h"
#include "user_interaction.h"
#include <stdint.h>

#include "moisture_sensor.h"

uint16_t moisture_sensor_readings[2];	// stores last value of averaged raw ADC readings for the two moisture sensors

uint16_t moisture_12h_counter = 0;		// GLOBAL: 0-71 to time moisture readings, value updated in main

/* Converts raw ADC value to a percent soil moisture
 * Parameters:
 * - adc_raw: corresponds to raw ADC output
 * Returns: % soil moisture reading from moisture sensor
 */
static uint16_t adc_to_pct_moisture(uint16_t adc_raw){
	uint16_t percent_moisture;
	uint32_t step, mvolt;
	mvolt = adc_raw*1000*3.3/4095;
	if(mvolt>DRY_ADC_LEVEL){
		return 100;
	} else if (mvolt<calibrated_wet_level){
		return 0;
	} else {
		step = DRY_ADC_LEVEL-calibrated_wet_level;
		percent_moisture = (mvolt-calibrated_wet_level)*100/step;
		return percent_moisture;
	}
}

/* Estimates the moisture at a given soil depth
 * Parameters:
 * - pct_depth: ranges from 0 (top) to 1 (bottom), fraction of pot depth where
 *              critical depth is found
 * - ms1: reading from higher (0"-2.5") moisture sensor, in raw ADC reading
 * - ms2: reading from lower (1"-3.5") moisture sensor, in raw ADC reading
 * Returns: estimate of moisture at given soil depth, in % moisture
 */
static uint16_t moisture_at_depth(float pct_depth, uint16_t ms1_raw, uint16_t ms2_raw){
	uint16_t moisture_at_cd;
	float crit_depth, k;
	uint32_t x = 2.5*(0.7*ms1_raw-0.3*ms2_raw);
	uint32_t y = (ms1_raw+ms2_raw)/2;
	uint32_t z = 2.5*(0.7*ms2_raw-0.3*ms1_raw);
	int diff = x-y;
    crit_depth = pct_depth*pot_depth;
    if (crit_depth>3.5) { // have to extrapolate
        k = crit_depth - 3.5;
        moisture_at_cd = z-k*diff;
    }
    else if(crit_depth>2.5) { // critical depth is in z range
        moisture_at_cd = z;
    }
    else if(crit_depth>1) { // critical depth is in y range
        moisture_at_cd = y;
    }
    else { // critical depth is in x range
        moisture_at_cd = x;
    }
    return adc_to_pct_moisture(moisture_at_cd);
}

/* Store averaged raw ADC values from 5 reads in array moisture_sensor_readings
 * Parameters:
 * - adc: initialized ADC
 * Returns: averaged raw ADC value for higher moisture sensor
 * TODO: update return value (extra parameter to chose which? don't return anything?)
 */
uint16_t get_moisture_value(ADC_HandleTypeDef adc){
    uint32_t sum1, sum2;
    uint16_t s1, s2, avg1, avg2;
    sum1 = 0; sum2 = 0;
    for(int i=0; i<5; i++) {
    	s1 = adc_channel_conv(1, adc);
        sum1 += s1;
        s2 = adc_channel_conv(3, adc);
        sum2 += s2;
        HAL_Delay(200);
    }
    avg1 = sum1/5;
    avg2 = sum2/5;

    moisture_sensor_readings[0] = avg1;
    moisture_sensor_readings[1] = avg2;

    return avg1;
}

/* Checks if plant needs watering based on pot height and plant type
 * (algorithm is still a work in progress)
 */
void check_moisture_level(){
	uint16_t high_moisture, low_moisture, cd_moisture;
	uint16_t ms1_raw, ms2_raw;
	float pct_depth;
	ms1_raw = moisture_sensor_readings[0];
	ms2_raw = moisture_sensor_readings[1];
	high_moisture = user_selected_plant.high_moisture;
	low_moisture = user_selected_plant.low_moisture;
    pct_depth = 0;
    if (high_moisture<21){ // low moisture plants
        pct_depth = 0.90;
    }
    else if (high_moisture<61){ // medium moisture plants
        pct_depth = 0.33;
    }
    else { // high moisture plants
        pct_depth = 0.15;
    }
    cd_moisture = moisture_at_depth(pct_depth,ms1_raw,ms2_raw);
    if (cd_moisture<low_moisture){
        update_alert(8,1);
    }
    else {
        update_alert(8,0); // clears alert if in proper range
    }
}
