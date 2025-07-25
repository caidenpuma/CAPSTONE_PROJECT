/* SoilSense F4 Debug
 * Functions for operation of LCD screen
 */

#include "liquidcrystal_i2c.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

extern I2C_HandleTypeDef hi2c1;

uint8_t dpFunction;
uint8_t dpControl;
uint8_t dpMode;
uint8_t dpRows;
uint8_t dpBacklight;

static void SendCommand(uint8_t);
static void SendChar(uint8_t);
static void Send(uint8_t, uint8_t);
static void Write4Bits(uint8_t);
static void ExpanderWrite(uint8_t);
static void PulseEnable(uint8_t);
static void DelayInit(void);
static void DelayUS(uint32_t);

uint8_t special1[8] = {
    0b00000, 0b00000, 0b00000, 0b00000,
    0b00000, 0b10101, 0b01110, 0b00100
};

uint8_t special2[8] = {
    0b00100, 0b01110, 0b10101, 0b00000,
    0b00000, 0b10101, 0b01110, 0b00100
};

uint8_t special3[8] = {
    0b00100, 0b01110, 0b10101, 0b00000,
    0b00000, 0b00000, 0b00000, 0b00000
};

void HD44780_Init(uint8_t rows) {
  dpRows = rows;
  dpBacklight = LCD_BACKLIGHT;
  dpFunction = LCD_4BITMODE | LCD_1LINE | LCD_5x8DOTS;

  if (dpRows > 1)
    dpFunction |= LCD_2LINE;
  else
    dpFunction |= LCD_5x10DOTS;

  DelayInit();
  HAL_Delay(50);
  ExpanderWrite(dpBacklight);
  HAL_Delay(1000);

  Write4Bits(0x03 << 4); DelayUS(4500);
  Write4Bits(0x03 << 4); DelayUS(4500);
  Write4Bits(0x03 << 4); DelayUS(4500);
  Write4Bits(0x02 << 4); DelayUS(100);

  SendCommand(LCD_FUNCTIONSET | dpFunction);
  dpControl = LCD_DISPLAYON | LCD_CURSOROFF | LCD_BLINKOFF;
  HD44780_Display();
  HD44780_Clear();

  dpMode = LCD_ENTRYLEFT | LCD_ENTRYSHIFTDECREMENT;
  SendCommand(LCD_ENTRYMODESET | dpMode);
  DelayUS(4500);

  HD44780_CreateSpecialChar(0, special1);
  HD44780_CreateSpecialChar(1, special2);
  HD44780_CreateSpecialChar(2, special3);

  HD44780_Home();

}

void HD44780_Clear() {
  SendCommand(LCD_CLEARDISPLAY);
  DelayUS(2000);
}

void HD44780_Home() {
  SendCommand(LCD_RETURNHOME);
  DelayUS(2000);
}

void HD44780_SetCursor(uint8_t col, uint8_t row) {
  int row_offsets[] = { 0x00, 0x40, 0x14, 0x54 };
  if (row >= dpRows) row = dpRows - 1;
  SendCommand(LCD_SETDDRAMADDR | (col + row_offsets[row]));
}

void HD44780_Display()         { dpControl |= LCD_DISPLAYON; SendCommand(LCD_DISPLAYCONTROL | dpControl); }
void HD44780_NoDisplay()       { dpControl &= ~LCD_DISPLAYON; SendCommand(LCD_DISPLAYCONTROL | dpControl); }
void HD44780_Cursor()          { dpControl |= LCD_CURSORON; SendCommand(LCD_DISPLAYCONTROL | dpControl); }
void HD44780_NoCursor()        { dpControl &= ~LCD_CURSORON; SendCommand(LCD_DISPLAYCONTROL | dpControl); }
void HD44780_Blink()           { dpControl |= LCD_BLINKON; SendCommand(LCD_DISPLAYCONTROL | dpControl); }
void HD44780_NoBlink()         { dpControl &= ~LCD_BLINKON; SendCommand(LCD_DISPLAYCONTROL | dpControl); }
void HD44780_ScrollDisplayLeft()  { SendCommand(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVELEFT); }
void HD44780_ScrollDisplayRight() { SendCommand(LCD_CURSORSHIFT | LCD_DISPLAYMOVE | LCD_MOVERIGHT); }
void HD44780_LeftToRight()     { dpMode |= LCD_ENTRYLEFT; SendCommand(LCD_ENTRYMODESET | dpMode); }
void HD44780_RightToLeft()     { dpMode &= ~LCD_ENTRYLEFT; SendCommand(LCD_ENTRYMODESET | dpMode); }
void HD44780_AutoScroll()      { dpMode |= LCD_ENTRYSHIFTINCREMENT; SendCommand(LCD_ENTRYMODESET | dpMode); }
void HD44780_NoAutoScroll()    { dpMode &= ~LCD_ENTRYSHIFTINCREMENT; SendCommand(LCD_ENTRYMODESET | dpMode); }

void HD44780_CreateSpecialChar(uint8_t location, uint8_t charmap[]) {
  location &= 0x7;
  SendCommand(LCD_SETCGRAMADDR | (location << 3));
  for (int i = 0; i < 8; i++) SendChar(charmap[i]);
}

void HD44780_PrintSpecialChar(uint8_t index) {
  SendChar(index);
}

void HD44780_LoadCustomCharacter(uint8_t char_num, uint8_t *rows) {
  HD44780_CreateSpecialChar(char_num, rows);
}

void HD44780_PrintStr(const char c[]) {
  while (*c) SendChar(*c++);
}

void HD44780_SetBacklight(uint8_t new_val) {
  if (new_val) HD44780_Backlight();
  else HD44780_NoBacklight();
}

void HD44780_NoBacklight(void) {
  dpBacklight = LCD_NOBACKLIGHT;
  ExpanderWrite(0);
}

void HD44780_Backlight(void) {
  dpBacklight = LCD_BACKLIGHT;
  ExpanderWrite(0);
}

static void SendCommand(uint8_t cmd) { Send(cmd, 0); }
static void SendChar(uint8_t ch)     { Send(ch, RS); }
static void Send(uint8_t value, uint8_t mode) {
  uint8_t highnib = value & 0xF0;
  uint8_t lownib = (value << 4) & 0xF0;
  Write4Bits(highnib | mode);
  Write4Bits(lownib | mode);
}

static void Write4Bits(uint8_t value) {
  ExpanderWrite(value);
  PulseEnable(value);
}

static void ExpanderWrite(uint8_t _data) {
  uint8_t data = _data | dpBacklight;
  HAL_I2C_Master_Transmit(&hi2c1, DEVICE_ADDR, (uint8_t*)&data, 1, 10);
}

static void PulseEnable(uint8_t _data) {
  ExpanderWrite(_data | ENABLE);
  DelayUS(20);
  ExpanderWrite(_data & ~ENABLE);
  DelayUS(20);
}

static void DelayInit(void) {
  CoreDebug->DEMCR |= CoreDebug_DEMCR_TRCENA_Msk;
  DWT->CTRL |= DWT_CTRL_CYCCNTENA_Msk;
  DWT->CYCCNT = 0;
  __ASM volatile ("NOP\nNOP\nNOP");
}

static void DelayUS(uint32_t us) {
  uint32_t cycles = (SystemCoreClock / 1000000L) * us;
  uint32_t start = DWT->CYCCNT;
  while ((DWT->CYCCNT - start) < cycles);
}

char line1[17] = "";
char line2[17] = "";

int update(const char *string1, const char *string2) {
    return strcmp(string1, line1) != 0 || strcmp(string2, line2) != 0;
}

void display_message_left(const char *string1, const char *string2) {
    if (update(string1, string2)) {
        HD44780_Clear();
        HD44780_SetCursor(0, 0);
        HD44780_PrintStr(string1);
        HD44780_SetCursor(0, 1);
        HD44780_PrintStr(string2);
        strncpy(line1, string1, 16); line1[16] = '\0';
        strncpy(line2, string2, 16); line2[16] = '\0';
    }
}

void display_message_center(const char *string1, const char *string2) {
    if (update(string1, string2)) {
        int i = ceil((16 - strlen(string1)) / 2.0);
        int j = ceil((16 - strlen(string2)) / 2.0);
        HD44780_Clear();
        HD44780_SetCursor(i, 0);
        HD44780_PrintStr(string1);
        HD44780_SetCursor(j, 1);
        HD44780_PrintStr(string2);
        strncpy(line1, string1, 16); line1[16] = '\0';
        strncpy(line2, string2, 16); line2[16] = '\0';
    }
}

void convert_number_to_string(int num, char *buffer) {
    sprintf(buffer, "%d", num);
}

void convert_float_to_string(float num, char *buffer) {
    //sprintf(buffer, "%.2f", num);
}

void display_message_left_int(int num1, int num2) {
    char buffer1[16], buffer2[16];
    convert_number_to_string(num1, buffer1);
    convert_number_to_string(num2, buffer2);
    display_message_left(buffer1, buffer2);
}

void display_message_left_float(float num1, float num2) {
    char buffer1[16], buffer2[16];
    convert_float_to_string(num1, buffer1);
    convert_float_to_string(num2, buffer2);
    display_message_left(buffer1, buffer2);
}

void display_message_center_int(int num1, int num2) {
    char buffer1[16], buffer2[16];
    convert_number_to_string(num1, buffer1);
    convert_number_to_string(num2, buffer2);
    display_message_center(buffer1, buffer2);
}

void display_message_center_float(float num1, float num2) {
    char buffer1[16], buffer2[16];
    convert_float_to_string(num1, buffer1);
    convert_float_to_string(num2, buffer2);
    display_message_center(buffer1, buffer2);
}

void display_off(void) {
  HD44780_Clear();
  HD44780_NoBacklight();
}

void display_on(void) {
  HD44780_Backlight();
}
