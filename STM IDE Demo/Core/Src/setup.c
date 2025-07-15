/* SoilSense F4 Debug
 * Control setup mode (calibration and plant selection)
 */

#include "stm32l4xx_hal.h"
#include "stm32l4xx_hal_adc.h"
#include "adc.h"
#include "user_interaction.h"
#include "plant_database.h"
#include "light_sensor.h"
#include "moisture_sensor.h"
#include "liquidcrystal_i2c.h"
#include <stdio.h>

#include "setup.h"

const uint32_t DRY_ADC_LEVEL = 2650;	// GLOBAL in mV (dry level around 2.64V)
Plant user_selected_plant = {"null", 0,0,0,0,0,0,0,0};				// GLOBAL
uint16_t calibrated_wet_level = 0;		// GLOBAL in mV
uint16_t pot_depth = 0;					// GLOBAL

volatile int rtc_flag = 0; // GLOBAL
volatile int flag_1 = 0; // GLOBAL
volatile int flag_2 = 0; // GLOBAL
volatile int flag_3 = 0; // GLOBAL
volatile int flag_4 = 0; // GLOBAL

/* Buttons in setup, operation modes
 * Button 1: calibrate, recalibrate
 * Button 2: select/ok, clear alert
 * Button 3: back, screen on/off
 * Button 4: next, toggle alert
 */
volatile int *flags[5] = {&rtc_flag, &flag_1, &flag_2, &flag_3, &flag_4};


void toggle_flag(int button){
	if(*flags[button] == 0){*flags[button]=1;}
	else if (*flags[button] == 1){*flags[button]=0;}
}

void set_flag(int button){
	*flags[button] = 1;
}

void clear_flag(int button){
	*flags[button] = 0;
}

/* Helper function for select_plant
 * controls display of plant list
 * NOTE: need to update wrapping index when size of plant_list changed
 */
static Plant scroll_plant(){
  int i = 0;
  while(1){
    display_message_left(plant_list[i]->plant_name,"");
    if(*flags[2]){ // if "select" is pressed
    	clear_flag(2);
        return *plant_list[i];
    }
    if(*flags[4]){ // if "next" pressed
        clear_flag(4);
        if(i==2){i=0;} // wrap to first index
        else {i++;}
    }
    if(*flags[3]){ // if "back" pressed
    	clear_flag(3);
        if(i==0) {i=2;}  // wrap to last index
        else {i--;}
    }
  }
}

/* User plant selection
 * Displays plant options and sets user_selected_plant
 */
uint32_t select_plant(){
    display_message_left("to select plant", "press next");
    while(!*flags[4]){ // wait for "next" press
        if(*flags[3] && user_selected_plant.high_humidity!=0){ // exit if "back" pressed and already plant selected
        	clear_flag(1); clear_flag(2); clear_flag(3); clear_flag(4);
            return 0;
        }
    };
    clear_flag(1); clear_flag(2); clear_flag(3); clear_flag(4);
    user_selected_plant = scroll_plant();
    return 0;
}

/* User calibration of moisture sensor reading for wet soil
 * Want wet level to be higher sensor, run 5 mins after watering
 * Parameters:
 * - adc: initialized ADC
 */
uint32_t calibrate_moisture(ADC_HandleTypeDef adc){
    display_message_left("water plant then", "press calibrate");
    while(!*flags[1]){ // wait for "calibrate" press
        if(*flags[3] && calibrated_wet_level!=0){ // exit if "back" pressed and already calibrated
        	display_message_left("exiting", "calibration");
        	HAL_Delay(2000);
        	HD44780_Clear();
        	clear_flag(1); clear_flag(2); clear_flag(3); clear_flag(4);
            return 0;
        }
    };
    clear_flag(1); clear_flag(2); clear_flag(3); clear_flag(4);
    display_message_left("calibrating", "");
    calibrated_wet_level = get_moisture_value(adc);
    display_message_left("calibration", "saved");
    HAL_Delay(2000);
    HD44780_Clear();
    return 0;
}

/* Helper function for select_pot_depth
 * Controls display of depth options
 */
static uint16_t scroll_depth(){
	uint32_t min_depth = 4;
	uint32_t max_depth = 24;
	uint16_t depth = min_depth;
	char buf[16];
    while(1){
    	sprintf(buf, "%d in",depth);
    	display_message_left("pot depth:", buf);
        if(*flags[2]){ // if "select" is pressed
        	clear_flag(2); // clear "select" flag
        	display_message_left("depth", "selected");
        	HAL_Delay(2000);
        	HD44780_Clear();
            return depth;
        }
        if (*flags[4]){ // if "next" pressed
        	clear_flag(4); // clear "next" flag
            if(depth==max_depth){depth=min_depth;} // wrap
            else {depth++;}
        }
        if(*flags[3]){ // if "back" pressed
        	clear_flag(3); // clear "back" flag
            if(depth==min_depth) {depth=max_depth;}  // wrap
            else {depth--;}
        }
    }
}

/* User pot depth selection
 */
uint16_t select_pot_depth(){
    display_message_left("to select pot", "depth press next");
    while(!*flags[4]){ // wait for "next" press
        if(*flags[3] && pot_depth!=0){ // exit if "back" pressed and already set depth
        	clear_flag(4); // clear "next" and "back" flags
        	clear_flag(3);
            return 0;
        }
    };
    clear_flag(4); // clear "next" flag
    pot_depth = scroll_depth();
    display_message_left("pot depth saved", "");
    HAL_Delay(2000);
    HD44780_Clear();
    return 0;
}

/* User recalibration (change one of setup values)
 */
uint32_t change_setup(ADC_HandleTypeDef adc){
    int i = 0;
    while(1){
        if(*flags[2]){ // if "select" is pressed
        	clear_flag(2); // clear "select" flag
            switch (i) {
                case (0):
                    select_plant();
                    break;
                case (1):
                    calibrate_moisture(adc);
                    break;
                case (2):
                    select_pot_depth();
                    break;
            }
            return i;
        }
        if (*flags[4]){ // if "next" pressed
        	clear_flag(4); // clear "next" flag
            if(i==2){i=0;} // wrap
            else {i++;}
        }
        if(*flags[3]){ // if "back" pressed
        	clear_flag(3); // clear "back" flag
            if(i==0) {i=2;}  // wrap
            else {i--;}
        }
        switch (i) {
            case (0):
                display_message_left("select new","plant");
                break;
            case (1):
                display_message_left("recalibrate","moisture");
                break;
            case (2):
                display_message_left("reset pot","depth");
                break;
        }
    }
    return 0;
};
