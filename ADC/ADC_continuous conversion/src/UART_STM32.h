#ifndef LCD_STM32 
#define LCD_STM32
void UART3_init (unsigned int, unsigned int);
int koeff_brr (unsigned int, unsigned int );
void UART3_init_IRQ (unsigned int, unsigned int);
//void USART3_IRQHandler(void);
void UART3_PutByte(char);
void UART3_PutRAMString(char *);
#endif
