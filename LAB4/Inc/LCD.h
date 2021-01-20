#ifndef LCD_H
#define LCD_H

#include "stm32f1xx_hal.h"

#define LCD_CLEARDISPLAY        0x01
#define LCD_RETURNHOME          0x02
#define LCD_ENTRYMODESET        0x04
#define LCD_DISPLAYCONTROL      0x08
#define LCD_CURSORSHIFT         0x10
#define LCD_FUNCTIONSET         0x20
#define LCD_SETCGRAMADDR        0x40
#define LCD_SETDDRAMADDR        0x80
/* Flags for display entry mode */
#define LCD_ENTRYRIGHT          0x00
#define LCD_ENTRYLEFT           0x02
#define LCD_ENTRYSHIFTINCREMENT 0x01
#define LCD_ENTRYSHIFTDECREMENT 0x00
/* Flags for display on/off control */
#define LCD_DISPLAYON           0x04
#define LCD_CURSORON            0x02
#define LCD_BLINKON             0x01
/* Flags for display/cursor shift */
#define LCD_DISPLAYMOVE         0x08
#define LCD_CURSORMOVE          0x00
#define LCD_MOVERIGHT           0x04
#define LCD_MOVELEFT            0x00
/* Flags for function set */
#define LCD_8BITMODE            0x10
#define LCD_4BITMODE            0x00
#define LCD_2LINE               0x08
#define LCD_1LINE               0x00
#define LCD_5x10DOTS            0x04
#define LCD_5x8DOTS             0x00

 /********************************************* STRUCTURE *********************************************/
typedef struct // Struct for defining LCD pins
{
	GPIO_TypeDef* RS_PORT;
	uint16_t RS_PIN;
	
	GPIO_TypeDef* RW_PORT;
	uint16_t RW_PIN;
	
	GPIO_TypeDef* EN_PORT;
	uint16_t EN_PIN;
	
	GPIO_TypeDef* D0_PORT;
	uint16_t D0_PIN;
	
	GPIO_TypeDef* D1_PORT;
	uint16_t D1_PIN;
	
	GPIO_TypeDef* D2_PORT;
	uint16_t D2_PIN;
	
	GPIO_TypeDef* D3_PORT;
	uint16_t D3_PIN;
	
	GPIO_TypeDef* D4_PORT;
	uint16_t D4_PIN;
	
	GPIO_TypeDef* D5_PORT;
	uint16_t D5_PIN;
	
	GPIO_TypeDef* D6_PORT;
	uint16_t D6_PIN;
	
	GPIO_TypeDef* D7_PORT;
	uint16_t D7_PIN;
	
	uint8_t Port_Count;
	
	uint8_t No_Of_Lines;
	
	uint8_t Font;
	
	uint8_t Cursor;
	
	uint8_t Blink;
}LCD_STRUCT;

 /********************************************* FUNCTIONS *********************************************/
void E_Blink(LCD_STRUCT *lcd_st_input); // Turn Enable Pin On And Off
void lcd_write(LCD_STRUCT *lcd_st_input, uint8_t data); // Write To Lcd
void lcd_write_command(LCD_STRUCT *lcd_st_input, uint8_t command);// Send Command
void lcd_write_data(LCD_STRUCT *lcd_st_input, uint8_t data);	// Send Data
void LCD_Init(LCD_STRUCT *lcd_st_input);//Initialize Lcd with First Settings 
void Lcd_clear(LCD_STRUCT *lcd_st_input);//Clear Whole Lcd
void LCD_SetCursor(LCD_STRUCT *lcd_st_input, uint8_t row, uint8_t col);// Set X,Y for Writing
void LCD_PutChar(LCD_STRUCT *lcd_st_input, char input_char);// Put single CHARACTER on LCD
void LCD_PutString(LCD_STRUCT *lcd_st_input, char * string);// Put Sting on LCD
void LCD_CreateChar(LCD_STRUCT *lcd_st_input, uint8_t location, uint8_t *data);
void LCD_PutCustom(LCD_STRUCT *lcd_st_input, uint8_t location);
void LCD_SET_BLINK_CURSOR(LCD_STRUCT *lcd_st_input, _Bool CURSOR_STATE,_Bool BLINK_STATE);//Enable Cursor and bliking cursor

#endif