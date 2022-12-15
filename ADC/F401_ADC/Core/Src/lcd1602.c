#include "lcd1602.h"
//------------------------------------------------
uint8_t buf[1]={0};
char str1[20];
uint8_t portlcd; //ячейка для хранения данных порта микросхемы расширения
//----------------------------------------------------------------------------------------//
inline void LCD_WriteByteI2C(uint8_t bt)
{
	buf[0]=bt;
	HAL_I2C_Master_Transmit(&hi2c1,(uint16_t) 0x4E,buf,1,1000);
}

//----------------------------------------------------------------------------------------//
void sendhalfbyte(uint8_t c)
{
  c<<=4;
  /*LCD_WriteByteI2C(portlcd|c);
  LCD_WriteByteI2C((portlcd|=0x04)|c);//включаем линию E
  delay_us (2);
	LCD_WriteByteI2C ((portlcd&=~0x04)|c);//выключаем линию E
  delay_us (50);*/
	LCD_WriteByteI2C((portlcd|=0x04)|c);	//включаем линию E
  delay_us (3);
  LCD_WriteByteI2C (portlcd|c);
  LCD_WriteByteI2C((portlcd&=~0x04)|c);	//выключаем линию E
  delay_us (3);
}

//----------------------------------------------------------------------------------------//
void sendbyte(uint8_t c, uint8_t mode)
{
	uint8_t hc=0;
	if (mode==0)
		rs_reset();
	else
		rs_set();
	hc=c>>4;
	sendhalfbyte(hc);
	sendhalfbyte(c);
}

//-----------------------------------установка позиции курсора-----------------------------------//
void LCD_SetPos(uint8_t x, uint8_t y)
{
  switch(y)
  {
    case 0:
      sendbyte(x|0x80,0);
      break;
    case 1:
      sendbyte((0x40+x)|0x80,0);
      break;
    case 2:
      sendbyte((0x14+x)|0x80,0);
      break;
    case 3:
      sendbyte((0x54+x)|0x80,0);
      break;
  }
}

//------------------------------------------------
void LCD_SendChar(char ch)
{
	sendbyte(ch,1);
}

//----------------------------------------------------------------------------------------//
void LCD_String(char* st)
{
  uint8_t i=0;

  while (*(st+i) != 0)
  {
    sendbyte(*(st+i), 1);
    i++;
  }
}

//----------------------------------------------------------------------------------------//
void LCD_ini(void)
{
  delay_ms(50);
  LCD_WriteByteI2C(0);
  delay_ms(50);
  sendhalfbyte(0x03);
  delay_us (200);
  sendhalfbyte(0x03);
	delay_us (200);
  sendhalfbyte(0x03);
  delay_us (200);
  sendhalfbyte(0x02);
  sendbyte(0x28,0);//режим 4 бит, 2 линии (для нашего большого дисплея это 4 линии, шрифт 5х8
	delay_us(100);
  sendbyte(0x0C,0);//дисплей включаем (D=1), курсоры никакие не нужны
	delay_us (100);
  sendbyte(0x01,0);// уберем мусор
  delay_us (1500);
  sendbyte(0x06,0);// пишем влево
  delay_us (1000);
  sendbyte(0x02,0);//курсор на место
  delay_us (1500);
  setled();//подсветка
  setwrite();//запись
}

//-----------------------------------очистка дисплея-----------------------------------//
void clear_LCD1602 (void)
{
	char buffer[16] ;
	uint8_t count = 0;

	for (count = 0; count < 16; count++)
	{
		buffer[count] = ' '; //заполнение буффера символами пробела
	}

	for (count = 0; count < 2; count++)
	{
		LCD_SetPos(0, count);
		LCD_String (buffer);
	}
//	sendbyte(0x01,0);
//	HAL_Delay(2);
}
