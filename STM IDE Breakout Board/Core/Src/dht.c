/************** MAKE CHANGES HERE ********************/
#include "stm32f4xx_hal.h"

//#define TYPE_DHT11    // define according to your sensor
#define TYPE_DHT22

#define DHT_PORT GPIOA
#define DHT_PIN GPIO_PIN_6

/*******************************************     NO CHANGES AFTER THIS LINE      ****************************************************/

uint8_t Rh_byte1, Rh_byte2, Temp_byte1, Temp_byte2;
uint16_t SUM;
uint8_t Presence = 0;

#include "DHT.h"

/**
  * @brief Initialize DWT for precise delay
  * @retval 0 if successful, 1 otherwise
  */
uint32_t DWT_Delay_Init(void)
{
  CoreDebug->DEMCR &= ~CoreDebug_DEMCR_TRCENA_Msk; // Disable TRC
  CoreDebug->DEMCR |=  CoreDebug_DEMCR_TRCENA_Msk; // Enable TRC

  DWT->CTRL &= ~DWT_CTRL_CYCCNTENA_Msk; // Disable clock cycle counter
  DWT->CTRL |=  DWT_CTRL_CYCCNTENA_Msk; // Enable clock cycle counter

  DWT->CYCCNT = 0; // Reset the clock cycle counter value

  __ASM volatile ("NOP");
  __ASM volatile ("NOP");
  __ASM volatile ("NOP");

  return DWT->CYCCNT ? 0 : 1; // Check if clock cycle counter started
}

/**
  * @brief Microsecond delay using DWT
  * @param microseconds: Number of microseconds to delay
  */
__STATIC_INLINE void delay(volatile uint32_t microseconds)
{
  uint32_t clk_cycle_start = DWT->CYCCNT;
  microseconds *= (HAL_RCC_GetHCLKFreq() / 1000000);
  while ((DWT->CYCCNT - clk_cycle_start) < microseconds);
}

/**
  * @brief Set GPIO pin as output
  */
void Set_Pin_Output(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
	GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

/**
  * @brief Set GPIO pin as input
  */
void Set_Pin_Input(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
	GPIO_InitTypeDef GPIO_InitStruct = {0};
	GPIO_InitStruct.Pin = GPIO_Pin;
	GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
	GPIO_InitStruct.Pull = GPIO_NOPULL;
	HAL_GPIO_Init(GPIOx, &GPIO_InitStruct);
}

/**
  * @brief Start signal for DHT sensor
  */
void DHT_Start(void)
{
	DWT_Delay_Init();
	Set_Pin_Output(DHT_PORT, DHT_PIN);  // Set pin as output
	HAL_GPIO_WritePin(DHT_PORT, DHT_PIN, 0);   // Pull pin low

#if defined(TYPE_DHT11)
	delay(18000);   // Wait for 18ms
#endif

#if defined(TYPE_DHT22)
	delay(1200);  // >1ms delay
#endif

    HAL_GPIO_WritePin(DHT_PORT, DHT_PIN, 1);   // Pull pin high
    delay(20);   // Wait for 20 us
	Set_Pin_Input(DHT_PORT, DHT_PIN);    // Set pin as input
}

/**
  * @brief Check response from DHT sensor
  * @retval 1 if response received, -1 otherwise
  */
uint8_t DHT_Check_Response(void)
{
	uint8_t Response = 0;
	delay(40);
	if (!(HAL_GPIO_ReadPin(DHT_PORT, DHT_PIN)))
	{
		delay(80);
		if ((HAL_GPIO_ReadPin(DHT_PORT, DHT_PIN))) Response = 1;
		else Response = -1;
	}
	while ((HAL_GPIO_ReadPin(DHT_PORT, DHT_PIN)));   // Wait for pin to go low
	return Response;
}

/**
  * @brief Read data from DHT sensor
  * @retval 8-bit data read from sensor
  */
uint8_t DHT_Read(void)
{
	uint8_t i, j;
	for (j = 0; j < 8; j++)
	{
		while (!(HAL_GPIO_ReadPin(DHT_PORT, DHT_PIN)));   // Wait for pin to go high
		delay(40);   // Wait for 40 us
		if (!(HAL_GPIO_ReadPin(DHT_PORT, DHT_PIN)))   // If pin is low
		{
			i &= ~(1 << (7 - j));   // Write 0
		}
		else i |= (1 << (7 - j));  // If pin is high, write 1
		while ((HAL_GPIO_ReadPin(DHT_PORT, DHT_PIN)));  // Wait for pin to go low
	}
	return i;
}

/**
  * @brief Get temperature and humidity data from DHT sensor
  * @param DHT_Data: Pointer to DHT_DataTypedef structure to store temperature and humidity
  */
void DHT_GetData(DHT_DataTypedef *DHT_Data)
{
    DHT_Start();
	//uint8_t Presence = DHT_Check_Response();
	uint8_t Rh_byte1 = DHT_Read();
	uint8_t Rh_byte2 = DHT_Read();
	uint8_t Temp_byte1 = DHT_Read();
	uint8_t Temp_byte2 = DHT_Read();
	uint8_t SUM = DHT_Read();

	if (SUM == ((Rh_byte1 + Rh_byte2 + Temp_byte1 + Temp_byte2) & 0x00FF))
	{
		#if defined(TYPE_DHT11)
			DHT_Data->Temperature = (float)Temp_byte1;
			DHT_Data->Humidity = (float)Rh_byte1;
		#endif

		#if defined(TYPE_DHT22)
			DHT_Data->Temperature = ((Temp_byte1 << 8) | Temp_byte2) / 10.0;
			DHT_Data->Humidity = ((Rh_byte1 << 8) | Rh_byte2) / 10.0;
		#endif
	}
	//return Temp_byte2;
}

