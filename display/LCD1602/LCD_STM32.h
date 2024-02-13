#ifndef LCD_STM32 // если не определен
#define LCD_STM32
void LCD_STM_INIT (void);
void LCD_CHAR_wr(char , unsigned char ); 
void LCD_CHAR_init_step(char);
void LCD_CHAR_init(unsigned char);
void LCD_CHAR_gotoxy(unsigned char, unsigned char);
void LCD_CHAR_puts(char *);
void delay_us(unsigned int us); //прототип ф-ии задержки в мкс
void delay_ms(unsigned int ms); //прототип ф-ии задержки в мс
#endif