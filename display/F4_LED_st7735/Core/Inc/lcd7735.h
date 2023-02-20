#ifndef LCD7735_H
#define LCD7735_H

#define ST7735_PANEL_WIDTH  160
#define ST7735_PANEL_HEIGHT 128

void lcd7735_ini(void); //инициализация дисплея
void lcd7735_sendData(unsigned char data); //отправка данных
void lcd7735_sendbyte(unsigned char data);
void lcd7735_send2byte(unsigned char msb,unsigned char lsb);
void lcd7735_sendCmd(unsigned char cmd); //отправка команды
void ClearLcdMemory(void);
void lcd7735_fillrect(unsigned char startX, unsigned char startY, unsigned char stopX, unsigned char stopY, unsigned int color); // процедура заполнения прямоугольной области экрана заданным цветом
void lcd7735_putpix(unsigned char x, unsigned char y, unsigned int Color); // вывод пиксела
void lcd7735_line(unsigned char x1, unsigned char y1, unsigned char x2, unsigned char y2, unsigned int color); // отрисовка линии 
void lcd7735_rect(char x1,char y1,char x2,char y2, unsigned int color); // рисование прямоугольника (не заполненного)	 
void lcd7735_putchar(unsigned char x, unsigned char y, unsigned char chr, unsigned int charColor, unsigned int bkgColor); //вывод символа на экран по координатам	  
void lcd7735_putstr(unsigned char x, unsigned char y, const unsigned char str[], unsigned int charColor, unsigned int bkgColor); //вывод строки	
void LCD7735_dec(unsigned int numb, unsigned char dcount, unsigned char x, unsigned char y,unsigned int fntColor, unsigned int bkgColor); //печать десятичного числа		
void lcd7735_at(unsigned char startX, unsigned char startY, unsigned char stopX, unsigned char stopY) ; // определение области экрана для заполнения
#endif
