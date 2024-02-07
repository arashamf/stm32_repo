#include "stm32f4xx_hal.h"
#include "main.h"
#include "lcd7735.h"
#include "gpio.h"
#include "font7x15.h"
#include "spi.h"

unsigned char Lcd_buf[ST7735_PANEL_WIDTH*ST7735_PANEL_HEIGHT];

//-----------------------------проверка работы библиотеки------------------------------------------//
//lcd7735_fillrect(0, 0, 128, 160, 0xF800);// заполним весь экран красным цветом
//если цвет не красный  ищем здесь void lcd7735_ini(void) там выбираем представление цветов

//------------------------------------------------------------------------------------------------------------------------//
void lcd7735_sendbyte(unsigned char data) //передача в режиме 8 бит
{
  HAL_SPI_Transmit(&hspi3, &data,1,0x1);
}

//------------------------------------------------------------------------------------------------------------------------//
void lcd7735_send2byte(uint8_t msb, uint8_t lsb) //передача в режиме 16 бит
{
	uint8_t masData[]={lsb,msb};
	HAL_SPI_Transmit(&hspi3,masData,1,0x1);
}

//---------------------------------------------отправка команды ------------------------------------------------//
void lcd7735_sendCmd(unsigned char cmd) 
{
   LCD_DC0;//ставим в ноль линию DC
   lcd7735_sendbyte(cmd);
}


//------------------------------------------------отправка данных---------------------------------------------//
void lcd7735_sendData(unsigned char data) 
{
   LCD_DC1;//ставим вывод DC в единицу т.к. передаем данные
   lcd7735_sendbyte (data);
}

//----------------------------------------инициализация дисплея -----------------------------------------//
void lcd7735_ini(void) 
{
   LCD_CS0;            // CS=0   
   LCD_RST0;           // RST=0 
   HAL_Delay(10);      
   LCD_RST1;           // RST=1
   HAL_Delay(10);      //    
   lcd7735_sendCmd(0x11); // 
   HAL_Delay(10);      // 
   lcd7735_sendCmd (0x3A); //    режим цвета:
   lcd7735_sendData(0x05); //     16  бит
	 lcd7735_sendCmd (0x36);// направление вывода изображения:
	 lcd7735_sendData(0x14); //0x1C-снизу вверх, справа на лево, порядок цветов RGB;0x14-снизу вверх, справа на лево, порядок цветов BGR
	 lcd7735_sendCmd (0x29);//Display on
}  

//----------------------------------------------вывод строки---------------------------------------------------------//
void lcd7735_putstr(unsigned char x, unsigned char y, const unsigned char str[], unsigned int charColor, unsigned int bkgColor) 
{
	while (*str!=0) 
	{
		lcd7735_putchar(x, y, *str, charColor, bkgColor);
		y=y+8;
		str++;
	}
}

//---------------------------определение области экрана для заполнения-------------------------------------//
void lcd7735_at(unsigned char startX, unsigned char startY, unsigned char stopX, unsigned char stopY)
{
	lcd7735_sendCmd(0x2A); //адресс колонки 
	LCD_DC1;
	lcd7735_sendbyte(0x00); //так как дисплей у нас маленький 
	lcd7735_sendbyte(startX);
	lcd7735_sendbyte(0x00); //так как дисплей у нас маленький 
	lcd7735_sendbyte(stopX);

	lcd7735_sendCmd (0x2B);//адрес ряда
	LCD_DC1;
	lcd7735_sendbyte (0x00);
	lcd7735_sendbyte (startY);
	lcd7735_sendbyte (0x00);
	lcd7735_sendbyte (stopY);
}

//------------------- процедура заполнения прямоугольной области экрана заданным цветом----------------------//
void lcd7735_fillrect(unsigned char startX, unsigned char startY, unsigned char stopX, unsigned char stopY, unsigned int color) 
{
	unsigned char y, x;
  unsigned char msb =((unsigned char)((color & 0xFF00)>>8));//старшие 8 бит
  unsigned char lsb =((unsigned char)(color & 0x00FF));	//младшие 8 бит
	
  LCD_CS0;// CS в 0 
  lcd7735_at(startX, startY, stopX, stopY);
	lcd7735_sendCmd(0x2C);//Memory write
  
	spi2_16b_init();// активация 16 битного режима
	LCD_DC1; //закоментить для 8 битного режима
	for (y=startY;y<stopY+1;y++)
	for (x=startX;x<stopX+1;x++) {	
   	lcd7735_send2byte (msb,lsb);
//  lcd7735_sendData(msb);//для 8 бит
//  lcd7735_sendData(lsb);//для 8 бит
		}
		while((SPI2->SR) & SPI_SR_BSY) {}; // ожидаем окончания передачи данных SPI2
		spi2_8b_init(); //активация 8 битного режима
 		LCD_CS1; //CS в 1 

}

//---------------------------------------------------вывод пиксела---------------------------------------------------//
void lcd7735_putpix(unsigned char x, unsigned char y, unsigned int Color) 
{
	LCD_CS0;
	lcd7735_at(x, y, x, y);
	lcd7735_sendCmd(0x2C);//запись в память
	lcd7735_sendbyte((unsigned char)((Color & 0xFF00)>>8)); //меняем старший и младший байты согласно протоколу прередачи
	lcd7735_sendbyte ((unsigned char) (Color & 0x00FF)); //меняем старший и младший байты согласно протоколу прередачи
  LCD_CS1;
}


//------------------------------------------печать десятичного числа-------------------------------------------------//
void LCD7735_dec(unsigned int numb, unsigned char dcount, unsigned char x, unsigned char y,unsigned int fntColor, unsigned int bkgColor) 
{
	unsigned int divid=10000;
	unsigned char i;
	for (i=5; i!=0; i--) 
	{
		unsigned char res=numb/divid;
		if (i<=dcount) 
		{
			lcd7735_putchar(x, y, res+'0', fntColor, bkgColor);
			y=y+6;
		}
		numb%=divid;
		divid/=10;
	}
}

//----------------------------------------вывод символа на экран по координатам------------------------------------------//
void lcd7735_putchar(unsigned char x, unsigned char y, unsigned char chr, unsigned int charColor, unsigned int bkgColor) 
{
	unsigned char i;
	unsigned char j;
	
	LCD_CS0;

	lcd7735_at(x, y, x+12, y+8);
	lcd7735_sendCmd(0x2C);//запись в память

	spi2_16b_init(); // переходим в 16 бит
	LCD_DC1; //закоментить для 8 бит
	unsigned char k, h; unsigned int color;
	for (i=0;i<7;i++)
	{
		for (k=2;k>0;k--) //к=1 верхняя половина символа,2 нижняя
		{
		  unsigned char chl=NewBFontLAT[ ( (chr-0x20)*14 + i+ 7*(k-1)) ];//вычилсляем нужный байт начала нужного символа
			//(chr-0x20)//у нас нету первых 20 символов поэтому в массиве на 20 символов меньше чтобы найти нужный нам в массиве соответсвенного
			//нужно вычесть 20 не существующих,*14 представление одного символа на экране занимает 14 байт.
			// i - служит для поиска нужного байта из 14 для обработки .
			// 7*(k-1) поиск нужного байта из 14 для нижней половины символа .т.е будет учитватся занчение либо 0 либо 7 т.е. пропускаем первые 7 байт 
		  chl=chl<<2*(k-1); // нижнюю половину символа сдвигаем на 1 позицию влево (убираем одну линию снизу)
		  if (k==2) 
				h=6; 
			else 
				h=7; // у нижней половины выведем только 6 точек вместо 7
		   for (j=0;j<h;j++) 
			{
				if (chl & 0x80) 
					color=charColor; 
				else 
					color=bkgColor;//если MSBit==0 тогда оставляем цвет бэграунда,если нет тогда цвет символа
				chl = chl<<1;//сдвигаем обработанный бит влево
				unsigned char msb =((unsigned char)((color & 0xFF00)>>8));//старшие 8 бит меняем местами MSB и LSB
				unsigned char lsb =((unsigned char)(color & 0x00FF));	//младшие 8 бит	 
     
				lcd7735_send2byte(msb,lsb);//надо переключаться в 16 бит прередачу
				//lcd7735_sendData(msb);//нужен 8 бит режим
				//lcd7735_sendData(lsb);//нужен 8 бит режим
			}
		}	
	}	
	// рисуем справо от символа пустую вертикальную линию для бокового интервала	
	unsigned char msb =((unsigned char)((bkgColor & 0xFF00)>>8));//старшие 8 бит
  unsigned char lsb =((unsigned char)(bkgColor & 0x00FF));	//младшие 8 бит	 
	for (j=0;j<13;j++) 
	{     
		lcd7735_send2byte (msb,lsb);//надо переключаться в 16 бит прередачу
		//	lcd7735_sendData(msb);//для 8 бит
		//	lcd7735_sendData(lsb);//старший байт первым
	}
	while((SPI2->SR) & SPI_SR_BSY); // ожидаем окончания передачи данных SPI2
	spi2_8b_init();
	LCD_CS1;
}

//---------------------------------------отрисовка линии-------------------------------------------//
void lcd7735_line(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned int color) 
{
	signed char   dx, dy, sx, sy;
	unsigned char  x,  y, mdx, mdy, l;

  if (x1==x2) 
	{ 
	  lcd7735_fillrect(x1,y1, x1,y2, color);//если линия прямая то просто чертим
	  return;
  }

  if (y1==y2) 
	{ 
	  lcd7735_fillrect(x1,y1, x2,y1, color);//если линия прямая то просто чертим
	  return;
  }
	
//здесь расчет черчения косой линии 
  dx=x2-x1; dy=y2-y1;

  if (dx>=0) 
		{ mdx=dx; sx=1; } else { mdx=x1-x2; sx=-1; }
  if (dy>=0) 
		{ mdy=dy; sy=1; } else { mdy=y1-y2; sy=-1; }

  x=x1; y=y1;

  if (mdx>=mdy) //если длинна линии  по х меньше чем по у
	{
     l=mdx;
		while (l>0) 
		{
			if (dy>0) 
				{ y=y1+mdy*(x-x1)/mdx; }
			else 
				{ y=y1-mdy*(x-x1)/mdx; }
			lcd7735_putpix(x,y,color);//расчитали текущий х,у и ресуем попиксельно задавая цвет
			x=x+sx;
			l--;
		} 
	} 
	else 
	{
		l=mdy;//если длинна линии  по у меньше чем по х
		while (l>0) 
		{
			if (dy>0) 
				{ x=x1+((mdx*(y-y1))/mdy); }
      else 
				{ x=x1+((mdx*(y1-y))/mdy); }
      lcd7735_putpix(x,y,color);//расчитали текущий х,у и ресуем попиксельно задавая цвет
      y=y+sy;
      l--;
		}
	}
  lcd7735_putpix(x2, y2, color);
}

//----------------------------рисование прямоугольника (не заполненного)---------------------------//

void lcd7735_rect(char x1,char y1,char x2,char y2, unsigned int color) 
{
	lcd7735_fillrect(x1,y1, x2,y1, color);
	lcd7735_fillrect(x1,y2, x2,y2, color);
	lcd7735_fillrect(x1,y1, x1,y2, color);
	lcd7735_fillrect(x2,y1, x2,y2, color);
}

void ClearLcdMemory(void)
{
	int tmp=0;
	for(tmp=0;tmp<20480;tmp++)
	{
		Lcd_buf[tmp]=0xFF;
	}
}
//------------------------------------------------------------------------------------------------------------------------//
/*uint16_t readDada(void){
						uint8_t data8[2];
						MAX6675_CS0
						HAL_SPI_Receive(&hspi1,data8,1,1000);
//	uint8_t   LSB = data8[0];
//	uint8_t  	MSB = data8[1];		
//	uint16_t	MSBLSB = MSB << 8 | LSB;
            if(data8[0] & 1<<2)	status=1;
	          else status=0;
	          MSBLSB = data8[1] << 8 | data8[0];
		        MSBLSB=(0X0FFF &(MSBLSB>>3));
						MAX6675_CS1
	return    (uint16_t)(MSBLSB/4);
}*/
