/* SoilSense F4 Debug
 * Driver for I2C communication with VEML7700 ALS
 * Modified from https://github.com/belyalov/stm32-hal-libraries/blob/master/veml6030.c
 */

#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_i2c.h"
#include <stdint.h>

#include "veml7700.h"

// define registers
#define REG_ALS_CONF            0x00
#define REG_ALS_WH              0x01
#define REG_ALS_WL              0x02
#define REG_POWER_SAVING        0x03
#define REG_ALS                 0x04
#define REG_WHITE               0x05
#define REG_ALS_INT             0x06
#define REG_ID					0x07

// Register 0x00: ALS_CONF_0
// ALS gain selection
#define REG_ALS_CONF_GAIN_1		(0x00 << 11) // x1
#define REG_ALS_CONF_GAIN_2		(0x01 << 11) // x2
#define REG_ALS_CONF_GAIN_8TH	(0x02 << 11) // x 1/8
#define REG_ALS_CONF_GAIN_4TH	(0x03 << 11) // x 1/4
// ALS integration times - all bits
#define REG_ALS_CONF_IT_CLEAR   (0x0f << 6)
#define REG_ALS_CONF_IT25       (0x0C << 6)
#define REG_ALS_CONF_IT50       (0x08 << 6)
#define REG_ALS_CONF_IT100      (0x00 << 6)
#define REG_ALS_CONF_IT200      (0x01 << 6)
#define REG_ALS_CONF_IT400      (0x02 << 6)
#define REG_ALS_CONF_IT800      (0x03 << 6)
// ALS persistent protect number
#define REG_ALS_CONF_PERS_1     (0x00 << 4)
#define REG_ALS_CONF_PERS_2     (0x01 << 4)
#define REG_ALS_CONF_PERS_4     (0x02 << 4)
#define REG_ALS_CONF_PERS_8     (0x03 << 4)
// ALS interrupt enable
#define REG_ALS_CONF_IT_ENABLE  (0x01 << 1)
// ALS shutdown setting
#define REG_ALS_CONF_SHUTDOWN   0x01

// Register 0x03: POWER_SAVING
// Power saving modes
#define REG_POWER_SAVING_PSM_1  (0x00 << 1)
#define REG_POWER_SAVING_PSM_2  (0x01 << 1)
#define REG_POWER_SAVING_PSM_3  (0x02 << 1)
#define REG_POWER_SAVING_PSM_4  (0x03 << 1)
#define REG_POWER_SAVING_ENABLE  0x01

static uint32_t _write_register(veml7700 *veml, uint8_t reg, uint16_t value){
  uint8_t payload[3] = {reg, value & 0xff, value >> 8};

  return HAL_I2C_Master_Transmit(veml->i2c, veml->write_addr, &payload[0], 3, 500);
}

static uint16_t _read_register(veml7700 *veml, uint8_t reg){
  uint8_t payload[2] = {0};

  int err = HAL_I2C_Mem_Read(veml->i2c, veml->write_addr, reg, 1, &payload[0], 2, 500);
  if (err != HAL_OK) {
    return 0;
  }
  return (payload[1] << 8) | payload[0];
}

uint32_t veml7700_init(veml7700 *veml, I2C_HandleTypeDef *i2c, uint8_t addr){
  veml->read_addr  = (addr << 1) | 0x01;
  veml->write_addr = (addr << 1);
  veml->i2c        = i2c;

  // Reset VEML configuration (in order to check device)
  return _write_register(veml, REG_ALS_CONF, 0);
}

uint32_t veml7700_power_on(veml7700 *veml){
  // Get current config and clear shutdown bit
  uint16_t config = _read_register(veml, REG_ALS_CONF);
  config &= ~REG_ALS_CONF_SHUTDOWN;

  return _write_register(veml, REG_ALS_CONF, config);
}

uint32_t veml7700_shutdown(veml7700 *veml){
  // Get current config and set shutdown bit
  uint16_t config = _read_register(veml, REG_ALS_CONF);
  config |= REG_ALS_CONF_SHUTDOWN;

  return _write_register(veml, REG_ALS_CONF, config);
}

uint32_t veml7700_set_als_integration_time(veml7700 *veml, uint16_t it){
  uint16_t config = _read_register(veml, REG_ALS_CONF);
  config &= ~REG_ALS_CONF_IT_CLEAR;
  config |= it;
  return _write_register(veml, REG_ALS_CONF, config);
}

uint16_t veml7700_get_als_integration_time(veml7700 *veml)
{
  uint16_t config = _read_register(veml, REG_ALS_CONF);
  return (config & REG_ALS_CONF_IT_CLEAR) >> 6;
}

uint32_t veml7700_set_als_gain(veml7700 *veml, uint16_t gain){
  uint16_t config = _read_register(veml, REG_ALS_CONF);
  // Clear all gain bits
  config &= ~REG_ALS_CONF_GAIN_4TH;
  config |= gain;
  return _write_register(veml, REG_ALS_CONF, gain);
}

uint16_t veml7700_get_als_gain(veml7700 *veml){
  uint16_t config = _read_register(veml, REG_ALS_CONF);
  return (config & REG_ALS_CONF_GAIN_4TH) >> 11;
}

uint16_t veml7700_read_als(veml7700 *veml){
  return _read_register(veml, REG_ALS);
}

uint16_t veml7700_read_white(veml7700 *veml){
  return _read_register(veml, REG_WHITE);
}
