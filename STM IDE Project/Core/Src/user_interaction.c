/* SoilSense F4 Debug
 * Functions for user interaction with system
 */

#include "stm32l4xx_hal.h"
#include "adc.h"
#include "setup.h"
#include "light_sensor.h"
#include "moisture_sensor.h"
#include "temp_rh_sensor.h"
#include "liquidcrystal_i2c.h"
#include <stdbool.h>

#include "user_interaction.h"

bool display_is_on = 1; // screen defaults to on (e.g. turn on when powered up)

// Alerts:
uint16_t A_more_light    = 0;
uint16_t A_less_light    = 0;
uint16_t A_brighter      = 0;
uint16_t A_darker        = 0;
uint16_t A_too_hot       = 0;
uint16_t A_too_cold      = 0;
uint16_t A_too_humid     = 0;
uint16_t A_too_dry       = 0;
uint16_t A_water         = 0;
uint16_t A_fertilize     = 0;
uint16_t A_dust          = 0;
uint16_t A_low_battery   = 0;
                                        	// index (for update_alert):
uint16_t *alert_array[12] = {&A_more_light,  //0
                        &A_less_light,  	//1
                        &A_brighter,    	//2
                        &A_darker,      	//3
                        &A_too_hot,     	//4
                        &A_too_cold,    	//5
                        &A_too_humid,   	//6
                        &A_too_dry,     	//7
                        &A_water,       	//8
                        &A_fertilize,   	//9
                        &A_dust,			//10
						&A_low_battery};    //11

/* Displays alert message for specified alert
 * Parameters:
 * - index: specifies which alert to display (see above indices in *alert_array)
 */
static void display_alert(uint16_t index){
	switch(index){
	case 0:
		display_message_left("needs more", "light");
		break;
	case 1:
		display_message_left("needs less", "light");
		break;
	case 2:
		display_message_left("needs brighter", "light");
		break;
	case 3:
		display_message_left("needs less", "bright light");
		break;
	case 4:
		display_message_left("too hot", "");
		break;
	case 5:
		display_message_left("too cold", "");
		break;
	case 6:
		display_message_left("too humid", "");
		break;
	case 7:
		display_message_left("too dry", "");
		break;
	case 8:
		display_message_left("time to water", "");
		break;
	case 9:
		display_message_left("time to", "fertilize");
		break;
	case 10:
		display_message_left("time to", "dust");
		break;
	case 11:
		display_message_left("battery low", "");
		break;
	}
}

/* Changes whether alert is active or not
 * Parameters:
 * - index: specifies which alert to set (see above indices in *alert_array)
 * - set: 0 to clear alert, 1 to set alert
 */
void update_alert(uint16_t index, uint16_t set){
    *alert_array[index]=set;
}

/* Counts number of active alerts and sets alert LED
 * Returns: number of active alerts
 * TODO: unit test GPIO ONLY (tested count is correct)
 */
uint16_t count_alerts(){
	uint16_t count = 0;
    for (int i=0; i<12; i++){
        if (*alert_array[i]==1){
            count++;
        }
    }
    if(count==0){
    	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, GPIO_PIN_RESET);
    } else {
    	HAL_GPIO_WritePin(GPIOC, GPIO_PIN_15, GPIO_PIN_SET);
    }
    return count;
}

/* Show alert messages, cycle through with button press
 */
uint16_t show_alerts(){
    int i = 0;
    while(1){
        if (count_alerts()==0){
        	clear_flags();
            display_message_left("no alerts", "for now :)");
            return 0;   // exit if no more alerts
        } else {
        	if (*alert_array[i]==1){
        		clear_flags();
        		display_alert(i);
        		if (*flags[2]){ // if "clear alert" pressed
        			clear_flags();
        			update_alert(i,0); // clear current alert
        			i++;
        		}
        		if (*flags[4]){ // if "next" pressed
        			if(i==11){i=0;} // wrap to first index
        			else {i++;}
        		}
        		if (*flags[3]){ // if "back" pressed
        			if(i==0) {i=11;}  // wrap to last index
        			else {i--;}
        		}
        	} else if(*flags[4]) {
        		if(i<12){i++;}
        		else{i=0;}
        	} else{
        		if(i>0){i--;}
        		else{i=11;}
        	}
        }
    }
}

/* Toggles display_is_on indicator
 */
void display_toggle(){
    display_is_on = (!display_is_on);
    if (display_is_on){display_on();}
    else {display_off();}
}

/* Checks if external AA battery supply running low
 * TODO: unit test - need to finish GPIO setup
 */
void check_battery_level(ADC_HandleTypeDef adc){
	uint16_t battery_level;	// raw ADC reading
	uint32_t mvolts; 		// battery level in mV
    // TODO: switch to ADC from GPIO (with disconnected internal pullup)
	battery_level = adc_channel_conv(2, adc);
	mvolts = battery_level * 3300 / 4095;
	if (mvolts < (1200*2)){ // ~20% capacity at 1.2V for alkaline AA batteries, 2 in series
		update_alert(11, 1);
	}
	else {
		update_alert(11, 0);
	}
}
