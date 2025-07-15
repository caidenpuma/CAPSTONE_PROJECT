/* SoilSense
 * Header file for user interaction
 */

#include <stdbool.h>

#ifndef INC_USER_INTERACTION_H_
#define INC_USER_INTERACTION_H_

extern bool display_is_on;
extern bool new_data;
extern uint8_t alerts_set;

extern uint16_t A_more_light;
extern uint16_t A_less_light;
extern uint16_t A_brighter;
extern uint16_t A_darker;
extern uint16_t A_too_hot;
extern uint16_t A_too_cold;
extern uint16_t A_too_humid;
extern uint16_t A_too_dry ;
extern uint16_t A_water;
extern uint16_t A_fertilize;
extern uint16_t A_dust;
extern uint16_t A_low_battery;

void update_alert(uint16_t index, uint16_t set);
void count_alerts();
void display_message(char* line_1, char* line_2);
uint16_t show_alerts(ADC_HandleTypeDef adc);
void display_toggle();

#endif /* INC_USER_INTERACTION_H_ */
