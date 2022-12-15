#ifndef MAIN_LCD1602_H_
#define MAIN_LCD1602_H_
//---------------------------------------------------------------------
#include "main.h"
#include "i2c.h"
#include "delay.h"
//---------------------------------------------------------------------
#define e_set() LCD_WriteByteI2C(portlcd|=0x04) 		 //установка линии Е в 1
#define e_reset() LCD_WriteByteI2C(portlcd&=~0x04) 	//установка линии Е в 0
#define rs_set() LCD_WriteByteI2C(portlcd|=0x01)  	//установка линии RS в 1
#define rs_reset() LCD_WriteByteI2C(portlcd&=~0x01) 	//установка линии RS в 0
#define setled() LCD_WriteByteI2C(portlcd|=0x08) 	//установка линии BL в 1
#define setwrite() LCD_WriteByteI2C(portlcd&=~0x02) //установка линии RW в 0
#define setread() LCD_WriteByteI2C(portlcd|=0x02) 	//установка линии RW в 1
//---------------------------------------------------------------------
void LCD_WriteByteI2C(uint8_t bt);
void LCD_ini(void);
void LCD_SetPos(uint8_t x, uint8_t y);
void LCD_SendChar(char ch);
void LCD_String(char* st);
void clear_LCD1602 (void);
//---------------------------------------------------------------------
#endif /* MAIN_LCD1602_H_ */
