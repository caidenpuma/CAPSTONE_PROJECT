/* SoilSense
 * Header file for light sensor operation
 * Sensor: VEML7700
 */

#include <stdint.h>

#ifndef INC_LIGHT_SENSOR_H_
#define INC_LIGHT_SENSOR_H_

extern uint32_t light_24h_counter;
extern uint32_t light_3day_counter;

void mins_in_optimal_light(int light_level);
void get_light_value();
void calc_high_intensity();
void check_light_intensity();
void check_light_time();

// TODO: remove these functions after verifying light sensor functionality:
#include "veml7700.h"
uint16_t light_i2c_read(veml7700 *veml);
uint32_t convert_to_lux(uint16_t veml_raw);


#endif /* INC_LIGHT_SENSOR_H_ */
