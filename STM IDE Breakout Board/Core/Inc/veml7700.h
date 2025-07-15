/* SoilSense F4 Debug
 * Header for VEML7700 driver
 * Modified from; https://github.com/belyalov/stm32-hal-libraries/blob/master/veml6030.h
 */

#include "stm32f4xx_hal_i2c.h"

#ifndef INC_VEML7700_H_
#define INC_VEML7700_H_


#define EXPORT

enum {
  VEML7700_OK = 0,
  VEML7700_ERROR = 1,
};

typedef struct {
  I2C_HandleTypeDef *i2c;
  uint8_t            read_addr;
  uint8_t            write_addr;
} veml7700;

// Initialize VEML7700 sensor
// Params:
//  - `veml`: VEML7700 definition to be initialized
//  - `i2c` I2C HAL bus (`hi2c1`, `hi2c2`, etc)
//  - `addr` VEML7700 address (fixed 7 bit slave address 0x10 )
// Returns:
//  - `VEML7700_OK` - device initialized successfully
//  - `VEML7700_ERROR` - initialization failed (e.g. device not found on the i2c bus)
EXPORT uint32_t veml7700_init(veml7700 *veml, I2C_HandleTypeDef *i2c, uint8_t addr);

// Power control //

// Power on veml7700
// Params:
//  - `veml` initialized instance
// Returns:
//  - `VEML7700_OK` - device powered on
//  - `VEML7700_ERROR` - I2C error
EXPORT uint32_t veml7700_power_on(veml7700 *veml);

// Shutdown veml7700
// Params:
//  - `veml` initialized instance
// Returns:
//  - `VEML7700_OK` - device put into sleep
//  - `VEML7700_ERROR` - I2C error
EXPORT uint32_t veml7700_shutdown(veml7700 *veml);

// ALS integration time configuration //

// Set Integration Time
// Params:
//  - `veml` initialized instance
//  - it - REG_ALS_CONF_ITx
// Returns:
//  - `VEML7700_OK` - device put into sleep
//  - `VEML7700_ERROR` - I2C error
EXPORT uint32_t veml7700_set_als_integration_time(veml7700 *veml, uint16_t it);

// Get current Integration Time
// Params:
//  - `veml` initialized instance
// Returns current integration time (REG_ALS_CONF_ITx)
EXPORT uint16_t veml7700_get_als_integration_time(veml7700 *veml);

// ALS gain configuration //

// Set ALS gain
// Params:
//  - `veml` initialized instance
//  - gain - REG_ALS_CONF_GAIN_x
// Returns:
//  - `VEML7700_OK` - device put into sleep
//  - `VEML7700_ERROR` - I2C error
EXPORT uint32_t veml7700_set_als_gain(veml7700 *veml, uint16_t gain);

// Get current ALS gain value
// Params:
//  - `veml` initialized instance
// Returns current gain value (REG_ALS_CONF_GAIN_x)
EXPORT uint16_t veml7700_get_als_gain(veml7700 *veml);

// Read current sensor values //

// Read previous measurement of ALS
// Params:
//  - `veml` initialized instance
// Returns current ALS
EXPORT uint16_t veml7700_read_als(veml7700 *veml);

// Read previous measurement of WHITE
// Params:
//  - `veml` initialized instance
// Returns current WHITE value
EXPORT uint16_t veml7700_read_white(veml7700 *veml);

#endif /* INC_VEML7700_H_ */
