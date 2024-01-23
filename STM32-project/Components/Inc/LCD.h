/**
 * @file LCD.h
 * @brief Header file for LCD module
 *
 * This file contains the declarations of functions and macros for controlling an LCD module.
 * The module supports 4-bit mode and provides functions for sending commands, writing data,
 * writing characters, and creating custom characters. It also includes macros for controlling
 * the GPIO pins of the LCD module.
 *
 * @author [Your Name]
 */


#ifndef __LCD_H__
#define __LCD_H__

#include "main.h"
#include <stdint.h>

#define LCD_GPIO_RS_Pin 	LCD_RS_Pin

#define LCD_GPIO_E_Pin 		LCD_E_Pin
#define LCD_GPIO_RS_Port	LCD_RS_GPIO_Port

#define LCD_GPIO_E_Port 	LCD_E_GPIO_Port
#define LCD_GPIO_DB4_Pin LCD_DB4_Pin
#define LCD_GPIO_DB5_Pin LCD_DB5_Pin
#define LCD_GPIO_DB6_Pin LCD_DB6_Pin
#define LCD_GPIO_DB7_Pin LCD_DB7_Pin

#define LCD_GPIO_DB4_Port LCD_DB4_GPIO_Port
#define LCD_GPIO_DB5_Port LCD_DB5_GPIO_Port
#define LCD_GPIO_DB6_Port LCD_DB6_GPIO_Port
#define LCD_GPIO_DB7_Port LCD_DB7_GPIO_Port


//LCD GPIO macros
#define LCD_GPIO_TOGGLE(pin, port) 				HAL_GPIO_TogglePin(port, pin)
#define LCD_GPIO_ON(pin, port) 					HAL_GPIO_WritePin(port, pin, GPIO_PIN_SET)
#define LCD_GPIO_OFF(pin, port) 				HAL_GPIO_WritePin(port, pin, GPIO_PIN_RESET)
#define LCD_GPIO_SET_VALUE(pin, value, port) 	(value==0?LCD_GPIO_OFF(pin, port):LCD_GPIO_ON(pin, port))

#define LCD_DATABIT_TOGGLE(number) 				LCD_GPIO_TOGGLE(LCD_GPIO_DB ## number ## _Pin, LCD_GPIO_DB ## number ## _Port)
#define LCD_DATABIT_ON(number) 					LCD_GPIO_ON(LCD_GPIO_DB ## number ## _Pin, LCD_GPIO_DB ## number ## _Port)
#define LCD_DATABIT_OFF(number) 				LCD_GPIO_OFF(LCD_GPIO_DB ## number ## _Pin, LCD_GPIO_DB ## number ## _Port)
#define LCD_DATABIT_SET_VALUE(number, value) 	(value==0?LCD_DATABIT_OFF(number):LCD_DATABIT_ON(number))


//LCD commands code
#define LCD_CLEAR_INSTRUCTION 	0x01
#define LCD_HOME_INSTRUCTION 	0x02

#define LCD_ENTRY_MODE_INSTRUCTION 			0x04
#define LCD_ENTRY_MODE_INCREMENT  			0x02
#define LCD_ENTRY_MODE_DECREMENT  			0x00
#define LCD_ENTRY_MODE_SHIFT_DISPLAY_ON 	0x01
#define LCD_ENTRY_MODE_SHIFT_DISPLAY_OFF 	0x00

#define LCD_DISPLAY_INSTRUCTION 0x08
#define LCD_DISPLAY_ON 			0x04
#define LCD_DISPLAY_OFF 		0x00
#define LCD_DISPLAY_CURSOR_ON 	0x02
#define LCD_DISPLAY_CURSOR_OFF 	0x00
#define LCD_DISPLAY_BLINK_ON 	0x01
#define LCD_DISPLAY_BLINK_OFF 	0x00

#define LCD_SHIFT_INSTRUCTION 	0x10
#define LCD_SHIFT_COURSOR_LEFT 	0x00
#define LCD_SHIFT_COURSOR_RIGTH (0x01<<2)
#define LCD_SHIFT_ALL_LEFT 		(0x01<<3)
#define LCD_SHIFT_ALL_RIGHT 	((0x01<<3)|(0x01<<2))

#define LCD_FUNCTION_INSTRUCTION 	0x20
#define LCD_FUNCTION_DL_8BIT 		0x10
#define LCD_FUNCTION_DL_4BIT 		0x00
#define LCD_FUNCTION_LINE_NUMBER_1 	0x00
#define LCD_FUNCTION_LINE_NUMBER_2 	0x08
#define LCD_FUNCTION_FONT_5x8 		0x00
#define LCD_FUNCTION_FONT_5x11 		0x04

#define LCD_CGRAM_ADDRESS 		0x40
#define LCD_DDRAM_ADDRESS 		0x80

#define LCD_MAXIMUM_LINE_LENGTH 16

//Char available in CGROM
#define LCD_CHAR_ARROW_LEFT 	0x7F
#define LCD_CHAR_ARROW_RIGHT 	0x7E
//Custom char available in CGRAM
#define LCD_CHAR_ARROW_UP		0
#define LCD_CHAR_ARROW_DOWN		1
#define LCD_CHAR_ARROW_OUT		2
#define LCD_CHAR_ARROW_INTO		3
#define LCD_CHAR_ARROW_ENTER	4
#define LCD_CHAR_PLUS_MINUS		5

//#define LCD_CUSTOM_CHAR_ARROW_UP_PATERN {0x04, 0x0E,  0x15,  0x04,  0x04,  0x04,  0x04,  0x00}
//#define LCD_CUSTOM_CHAR_ARROW_DOWN_PATERN {  0x00,  0x04,  0x04,  0x04,  0x15,  0x0E,  0x04,  0x00}
#define LCD_CUSTOM_CHAR_ARROW_UP_PATERN 	{0x00,  0x00,  0x04,  0x04,  0x0E,  0x0E,  0x1F,  0x00}
#define LCD_CUSTOM_CHAR_ARROW_DOWN_PATERN 	{0x00,  0x00,  0x1F,  0x0E,  0x0E,  0x04,  0x04,  0x00}
#define LCD_CUSTOM_CHAR_ARROW_OUT_PATERN 	{0x01,  0x05,  0x09,  0x1F,  0x09,  0x05,  0x01,  0x00}
#define LCD_CUSTOM_CHAR_ARROW_INTO_PATERN  	{0x01,  0x09,  0x05,  0x1F,  0x05,  0x09,  0x01,  0x00}
#define LCD_CUSTOM_CHAR_ARROW_ENTER_PATERN 	{0x01,  0x01,  0x05,  0x09,  0x1F,  0x08,  0x04,  0x00}
#define LCD_CUSTOM_CHAR_ARROW_PLUS_MINUS_PATERN {0x04,  0x04,  0x1F,  0x04,  0x04,  0x00,  0x1F,  0x00}

/**
* Initialization of LCD GPIO
*/
void LCD_init(void);

void LCD_send_4bits(uint8_t data, char RS, char RW);

/**
* Send data/command to LCD
* @param[in] date Data/command to send
* @param[in] RS 1-data, 0-command
* @param[in] RW 1-read, 0-write
*/
void LCD_send_8bits_twice_4bits(uint8_t data_to_send, char RS, char RW);

void LCD_write_command(uint8_t command);
void LCD_write_data(char byte_data);
void LCD_write_char(char character);
void LCD_write_text(char* pText);
void LCD_goto_xy(uint8_t line, uint8_t y);
void LCD_goto_line(uint8_t line);
uint8_t LCD_printf(const char * format, ... );
void LCD_create_custom_character(uint8_t* pPattern, uint8_t position);

/**
* Write one line of LCD
*/
void LCD_printf_line(const char * format, ... );

#endif
