#include "LCD.h"

//############################################################################################
void E_Blink(LCD_STRUCT *lcd_st_input)
{
	HAL_GPIO_WritePin(lcd_st_input->EN_PORT, lcd_st_input->EN_PIN, GPIO_PIN_SET);
	HAL_Delay(1);
	HAL_GPIO_WritePin(lcd_st_input->EN_PORT, lcd_st_input->EN_PIN, GPIO_PIN_RESET);
}
//############################################################################################
void lcd_write(LCD_STRUCT *lcd_st_input, uint8_t data)
{
	if(lcd_st_input->Port_Count == 8)
	{
		HAL_GPIO_WritePin(lcd_st_input->D7_PORT, lcd_st_input->D7_PIN, (GPIO_PinState)(data & 0x80));
		HAL_GPIO_WritePin(lcd_st_input->D6_PORT, lcd_st_input->D6_PIN, (GPIO_PinState)(data & 0x40));
		HAL_GPIO_WritePin(lcd_st_input->D5_PORT, lcd_st_input->D5_PIN, (GPIO_PinState)(data & 0x20));
		HAL_GPIO_WritePin(lcd_st_input->D4_PORT, lcd_st_input->D4_PIN, (GPIO_PinState)(data & 0x10));
		HAL_GPIO_WritePin(lcd_st_input->D3_PORT, lcd_st_input->D3_PIN, (GPIO_PinState)(data & 0x08));
		HAL_GPIO_WritePin(lcd_st_input->D2_PORT, lcd_st_input->D2_PIN, (GPIO_PinState)(data & 0x04));
		HAL_GPIO_WritePin(lcd_st_input->D1_PORT, lcd_st_input->D1_PIN, (GPIO_PinState)(data & 0x02));
		HAL_GPIO_WritePin(lcd_st_input->D0_PORT, lcd_st_input->D0_PIN, (GPIO_PinState)(data & 0x01));
	}
	else
	{
		HAL_GPIO_WritePin(lcd_st_input->D7_PORT, lcd_st_input->D7_PIN, (GPIO_PinState)(data & 0x08));
		HAL_GPIO_WritePin(lcd_st_input->D6_PORT, lcd_st_input->D6_PIN, (GPIO_PinState)(data & 0x04));
		HAL_GPIO_WritePin(lcd_st_input->D5_PORT, lcd_st_input->D5_PIN, (GPIO_PinState)(data & 0x02));
		HAL_GPIO_WritePin(lcd_st_input->D4_PORT, lcd_st_input->D4_PIN, (GPIO_PinState)(data & 0x01));
	}
	E_Blink(lcd_st_input);
}
//############################################################################################
void lcd_write_command(LCD_STRUCT *lcd_st_input, uint8_t command)
{
	HAL_GPIO_WritePin(lcd_st_input->RS_PORT, lcd_st_input->RS_PIN, GPIO_PIN_RESET);		// Write to command register

	if(lcd_st_input->Port_Count == 4)
	{
		lcd_write(lcd_st_input, command >> 4);
		lcd_write(lcd_st_input, command & 0x0F);
	}
	else
	{
		lcd_write(lcd_st_input, command);
	}

}
//############################################################################################
void lcd_write_data(LCD_STRUCT *lcd_st_input, uint8_t data)
{
	HAL_GPIO_WritePin(lcd_st_input->RS_PORT, lcd_st_input->RS_PIN, GPIO_PIN_SET);			// Write to data register

	if(lcd_st_input->Port_Count == 4)
	{
		lcd_write(lcd_st_input, data >> 4);
		lcd_write(lcd_st_input, data & 0x0F);
	}
	else
	{
		lcd_write(lcd_st_input, data);
	}

}
//############################################################################################
void LCD_Init(LCD_STRUCT *lcd_st_input)
{
	uint8_t command;
	command = LCD_FUNCTIONSET;
	if(lcd_st_input->Port_Count == 4)
		command |= LCD_4BITMODE;
	else
		command |= LCD_8BITMODE;
	
	if(lcd_st_input->Font == 10)
		command |= LCD_5x10DOTS ;
	else
		command |= LCD_5x8DOTS;
	
	if(lcd_st_input->No_Of_Lines == 1)
		command |= LCD_1LINE ;
	else
		command |= LCD_2LINE;

		if(lcd_st_input->Port_Count == 4)
	{
			lcd_write_command(lcd_st_input, 0x33);
			lcd_write_command(lcd_st_input, 0x32);
			lcd_write_command(lcd_st_input, command);				// 4-bit mode
	}
	else
		lcd_write_command(lcd_st_input, command);


	lcd_write_command(lcd_st_input, LCD_CLEARDISPLAY);						          // Clear screen
	
	command = LCD_DISPLAYCONTROL | LCD_DISPLAYON;
	if(lcd_st_input->Cursor == 1)
		command |= LCD_CURSORON;

	
	if(lcd_st_input->Blink == 1)
		command |= LCD_BLINKON;
	
	lcd_write_command(lcd_st_input, command);
	
	
	lcd_write_command(lcd_st_input, LCD_ENTRYMODESET | LCD_ENTRYLEFT);
}
//############################################################################################
void Lcd_clear(LCD_STRUCT *lcd_st_input) {
	lcd_write_command(lcd_st_input, LCD_CLEARDISPLAY);
}
//############################################################################################
void LCD_SetCursor(LCD_STRUCT *lcd_st_input, uint8_t row, uint8_t col)
{
	const uint8_t ROW_16[] = {0x00, 0x40, 0x10, 0x50};
	lcd_write_command(lcd_st_input, LCD_SETDDRAMADDR  + ROW_16[row] + col);
}
//############################################################################################
void LCD_PutChar(LCD_STRUCT *lcd_st_input, char input_char)
{
		lcd_write_data(lcd_st_input, input_char);
}
//############################################################################################
void LCD_PutString(LCD_STRUCT *lcd_st_input, char * string)
{
	for(uint8_t i = 0; string[i]; i++)
	{
		lcd_write_data(lcd_st_input, string[i]);
	}
}
//############################################################################################
void LCD_CreateChar(LCD_STRUCT *lcd_st_input, uint8_t location, uint8_t *data)
{
	uint8_t i;
	/* We have 8 locations available for custom characters */
	location &= 0x07;
	lcd_write_command(lcd_st_input, LCD_SETCGRAMADDR | (location << 3));
	for (i = 0; i < 8; i++)
		lcd_write_data(lcd_st_input,data[i]);
}
//############################################################################################
void LCD_PutCustom(LCD_STRUCT *lcd_st_input, uint8_t location)
{
	lcd_write_data(lcd_st_input,location);
}
//############################################################################################
void LCD_SET_BLINK_CURSOR(LCD_STRUCT *lcd_st_input, _Bool CURSOR_STATE,_Bool BLINK_STATE)
{
	uint8_t command;
	command = LCD_DISPLAYCONTROL | LCD_DISPLAYON;
	if(CURSOR_STATE == 1)
		command |= LCD_CURSORON;

	
	if(BLINK_STATE == 1)
		command |= LCD_BLINKON;
	
	lcd_write_command(lcd_st_input, command);
}


