/* SoilSense
 * Header file for moisture sensor operation
 */

#include <stdint.h>

#ifndef INC_MOISTURE_SENSOR_H_
#define INC_MOISTURE_SENSOR_H_

extern uint16_t moisture_12h_counter;

uint16_t get_moisture_value();
void check_moisture_level();

#endif /* INC_MOISTURE_SENSOR_H_ */
