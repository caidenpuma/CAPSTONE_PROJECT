/* SoilSense
 * Header file for global configuration settings
 */

#include <stdint.h>

#ifndef INC_ADC_H_
#define INC_ADC_H_

extern ADC_ChannelConfTypeDef ADC_CH_Cfg;

uint16_t adc_channel_conv(uint8_t chan, ADC_HandleTypeDef adc);

#endif /* INC_ADC_H_ */
