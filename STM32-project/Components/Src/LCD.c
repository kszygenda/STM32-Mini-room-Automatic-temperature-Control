/**
 ******************************************************************************
 * @file LCD.c
 * @brief This file contains the implementation of the LCD module.
 ******************************************************************************
 */

/* Includes ------------------------------------------------------------------*/
#include "../Inc/LCD.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdint.h>
#include <ctype.h>

/* Typedef -------------------------------------------------------------------*/

/* Define --------------------------------------------------------------------*/

/* Macro ---------------------------------------------------------------------*/

/* Private variables ---------------------------------------------------------*/

/* Public variables ----------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/

/* Private function ----------------------------------------------------------*/

/* Public function -----------------------------------------------------------*/

/**
 * @brief Delays the execution by a specified number of ticks.
 * @param tick The number of ticks to delay.
 */
static void software_delay(uint32_t tick)
{
	uint32_t delay;
	while (tick-- > 0)
	{
		for (delay = 5; delay > 0; delay--)
		{
			asm("nop");
			asm("nop");
			asm("nop");
			asm("nop");
		}
	}
}

/**
 * @brief Initializes the LCD module.
 */
void LCD_init(void)
{
	software_delay(1000000);
	LCD_send_4bits(0x03, 0, 0);
	software_delay(1000000);
	LCD_send_4bits(0x03, 0, 0);
	software_delay(1000000);
	LCD_send_4bits(0x03, 0, 0);
	software_delay(400000);
	LCD_send_4bits(0x02, 0, 0);
	software_delay(400000);

	LCD_write_command(LCD_FUNCTION_INSTRUCTION | LCD_FUNCTION_DL_4BIT | LCD_FUNCTION_LINE_NUMBER_2 | LCD_FUNCTION_FONT_5x8);
	software_delay(50000);

	LCD_write_command(LCD_DISPLAY_INSTRUCTION | LCD_DISPLAY_ON | LCD_DISPLAY_CURSOR_OFF | LCD_DISPLAY_BLINK_OFF);
	software_delay(100000);

	LCD_write_command(LCD_CLEAR_INSTRUCTION);
	software_delay(100000);

	LCD_write_command(LCD_ENTRY_MODE_INSTRUCTION | LCD_ENTRY_MODE_INCREMENT | LCD_ENTRY_MODE_SHIFT_DISPLAY_OFF);
	software_delay(100000);

	LCD_write_command(LCD_HOME_INSTRUCTION);
	software_delay(100000);

	uint8_t custom_char1[] = LCD_CUSTOM_CHAR_ARROW_UP_PATERN;
	LCD_create_custom_character(custom_char1, 0);
	uint8_t custom_char2[] = LCD_CUSTOM_CHAR_ARROW_DOWN_PATERN;
	LCD_create_custom_character(custom_char2, 1);
	uint8_t custom_char3[] = LCD_CUSTOM_CHAR_ARROW_OUT_PATERN;
	LCD_create_custom_character(custom_char3, 2);
	uint8_t custom_char4[] = LCD_CUSTOM_CHAR_ARROW_INTO_PATERN;
	LCD_create_custom_character(custom_char4, 3);
	uint8_t custom_char5[] = LCD_CUSTOM_CHAR_ARROW_ENTER_PATERN;
	LCD_create_custom_character(custom_char5, 4);
	uint8_t custom_char6[] = LCD_CUSTOM_CHAR_ARROW_PLUS_MINUS_PATERN;
	LCD_create_custom_character(custom_char6, 5);
}

/**
 * @brief Sends 4 bits of data to the LCD module.
 * @param data_to_send The data to send.
 * @param RS The value of the RS pin.
 * @param RW The value of the RW pin.
 */
void LCD_send_4bits(uint8_t data_to_send, char RS, char RW)
{
	LCD_GPIO_SET_VALUE(LCD_GPIO_RS_Pin, RS, LCD_GPIO_RS_Port);

	if (data_to_send & (0x01 << 0))
	{
		LCD_DATABIT_ON(4);
	}
	else
	{
		LCD_DATABIT_OFF(4);
	}
	if (data_to_send & (0x01 << 1))
	{
		LCD_DATABIT_ON(5);
	}
	else
	{
		LCD_DATABIT_OFF(5);
	}
	if (data_to_send & (0x01 << 2))
	{
		LCD_DATABIT_ON(6);
	}
	else
	{
		LCD_DATABIT_OFF(6);
	}
	if (data_to_send & (0x01 << 3))
	{
		LCD_DATABIT_ON(7);
	}
	else
	{
		LCD_DATABIT_OFF(7);
	}
	software_delay(100);

	LCD_GPIO_ON(LCD_GPIO_E_Pin, LCD_GPIO_E_Port);
	software_delay(100);
	LCD_GPIO_OFF(LCD_GPIO_E_Pin, LCD_GPIO_E_Port);
	software_delay(100);
	LCD_GPIO_ON(LCD_GPIO_E_Pin, LCD_GPIO_E_Port);
	software_delay(1000);
}

/**
 * @brief Sends 8 bits of data to the LCD module using two 4-bit transmissions.
 * @param data The data to send.
 * @param RS The value of the RS pin.
 * @param RW The value of the RW pin.
 */
void LCD_send_8bits_twice_4bits(uint8_t data, char RS, char RW)
{
	LCD_send_4bits((data >> 4), RS, RW);
	LCD_send_4bits(data, RS, RW);
}

/**
 * @brief Writes a command to the LCD module.
 * @param command The command to write.
 */
void LCD_write_command(uint8_t command)
{
	LCD_send_8bits_twice_4bits(command, 0, 0);
	software_delay(10000);
}

/**
 * @brief Writes a data byte to the LCD module.
 * @param byte_data The data byte to write.
 */
void LCD_write_data(char byte_data)
{
	LCD_send_8bits_twice_4bits(byte_data, 1, 0);
}

/**
 * @brief Writes a character to the LCD module.
 * @param character The character to write.
 */
void LCD_write_char(char character)
{
	if (isprint(character))
	{
		LCD_write_data(character);
	}
}

/**
 * @brief Writes a null-terminated string to the LCD module.
 * @param pText The string to write.
 */
void LCD_write_text(char *pText)
{
	while (*pText != '\0')
	{
		LCD_write_char(*pText);
		pText++;
	}
}

/**
 * @brief Sets the cursor position on the LCD module.
 * @param line The line number (0 or 1).
 * @param y The column number.
 */
void LCD_goto_xy(uint8_t line, uint8_t y)
{
	switch (line)
	{
	case 0:
		line = 0x00;
		break;
	case 1:
		line = 0x40;
		break;
	default:
		line = 0;
	}
	LCD_write_command(LCD_DDRAM_ADDRESS | (line + y));
}

/**
 * @brief Sets the cursor position to the specified line on the LCD module.
 * @param line The line number (0 or 1).
 */
void LCD_goto_line(uint8_t line)
{
	LCD_goto_xy(line, 0);
}

/**
 * @brief Writes a formatted string to the LCD module, with automatic line wrapping and padding.
 * @param format The format string.
 * @param ... Additional arguments to format.
 */
void LCD_printf_line(const char *format, ...)
{
	#define LCD_BUFFER_SIZE (LCD_MAXIMUM_LINE_LENGTH + 1)
	char text_buffer[LCD_BUFFER_SIZE];
	uint8_t length = 0;
	va_list args;
	va_start(args, format);
	length = vsnprintf(text_buffer, LCD_BUFFER_SIZE, format, args);
	LCD_write_text(text_buffer);
	va_end(args);
	if (length >= 1 && length < LCD_MAXIMUM_LINE_LENGTH)
	{
		snprintf(text_buffer, LCD_BUFFER_SIZE, "%*c", LCD_MAXIMUM_LINE_LENGTH - length, ' ');
		LCD_write_text(text_buffer);
	}
}

/**
 * @brief Writes a formatted string to the LCD module.
 * @param format The format string.
 * @param ... Additional arguments to format.
 * @return The number of characters written.
 */
uint8_t LCD_printf(const char *format, ...)
{
	#define LCD_BUFFER_SIZE (LCD_MAXIMUM_LINE_LENGTH + 1)
	char text_buffer[LCD_BUFFER_SIZE];
	uint8_t length = 0;
	va_list args;
	va_start(args, format);
	length = vsnprintf(text_buffer, LCD_BUFFER_SIZE, format, args);
	LCD_write_text(text_buffer);
	va_end(args);
	return length;
}

/**
 * @brief Creates a custom character on the LCD module.
 * @param pPattern The pattern data for the custom character.
 * @param position The position of the custom character (0-7).
 */
void LCD_create_custom_character(uint8_t *pPattern, uint8_t position)
{
	LCD_write_command(LCD_CGRAM_ADDRESS | (position * 8));
	for (uint8_t i = 0; i < 8; i++)
	{
		LCD_write_data(pPattern[i]);
	}
}




