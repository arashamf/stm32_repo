#ifndef UART_STM32
#define UART_STM32
#define GPIO_AFRH_AF7_8 0x00000007 //настройка AF7 для пина8
#define GPIO_AFRH_AF7_9 0x00000070  //настройка AF7для пина9
#define GPIO_AFRL_AF7_2 0x700   //настройка AF7 для пина2
#define GPIO_AFRL_AF7_3 0x7000   //настройка AF7 для пина3
void UART3_init (unsigned int freq, unsigned int bandwith);
int koeff_brr (unsigned int, unsigned int );
void UART3_init_IRQ (unsigned int, unsigned int);
void USART3_IRQHandler(void);
void UART3_PutByte(char);
void UART3_PutRAMString(char *);
void UART2_init (unsigned int freq, unsigned int bandwith);
void UART2_PutByte(char c);
void UART2_PutRAMString(char *str);
#endif
