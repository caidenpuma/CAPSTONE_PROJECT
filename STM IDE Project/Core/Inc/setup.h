/* SoilSense
 * Header file for setup mode
 */

#include <stdint.h>
#include <plant_database.h>

#ifndef INC_SETUP_H_
#define INC_SETUP_H_

extern const uint32_t DRY_ADC_LEVEL;
extern Plant user_selected_plant;
extern uint16_t calibrated_wet_level;
extern uint16_t pot_depth;

uint32_t select_plant();
uint32_t calibrate_moisture(ADC_HandleTypeDef adc);
uint16_t select_pot_depth();
uint32_t change_setup();

extern volatile int *flags[5];
void toggle_flag(int button);
void set_flag(int button);
void clear_flags();


#endif /* INC_SETUP_H_ */
