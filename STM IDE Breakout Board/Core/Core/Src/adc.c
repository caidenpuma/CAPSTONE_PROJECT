/* SoilSense F4 Debug
 * Enables ADC single conversion on specified channel:
 * For demo:
 * ADC_CHANNEL_0: Higher moisture sensor
 * ADC_CHANNEL_1: Lower moisture sensor
 */
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_adc.h"
#include <stdint.h>

#include "adc.h"

ADC_ChannelConfTypeDef ADC_CH_Cfg = {0};
uint32_t ADC_Channels[5] = {ADC_CHANNEL_0, ADC_CHANNEL_1, ADC_CHANNEL_2,ADC_CHANNEL_3, ADC_CHANNEL_4};


/* Performs ADC single conversion on selected channel
 * Parameters:
 * - chan: channel number
 * - adc: initialized ADC
 * Returns: raw ADC reading from selected channel (0-4)
 */
uint16_t adc_channel_conv(uint8_t chan, ADC_HandleTypeDef adc){
	uint16_t adc_raw;
	ADC_CH_Cfg.Rank =  1;
	ADC_CH_Cfg.SamplingTime = ADC_SAMPLETIME_3CYCLES;
	ADC_CH_Cfg.Channel = ADC_Channels[chan];        // select ADC channel
	HAL_ADC_ConfigChannel(&adc, &ADC_CH_Cfg);   	// configure selected channel
	HAL_ADC_Start(&adc);                        	// start ADC conversion
	HAL_ADC_PollForConversion(&adc, 1);         	// poll ADC channel with 1ms timeout
	adc_raw = HAL_ADC_GetValue(&adc);
	return adc_raw;
}
