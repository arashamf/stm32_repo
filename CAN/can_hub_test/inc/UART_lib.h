#ifndef UART_lib
#define UART_lib
#define GPIO_AFRH_AF7_8 0x00000007 //настройка AF7 для пина8
#define GPIO_AFRH_AF7_9 0x00000070  //настройка AF7для пина9
#define GPIO_AFRL_AF7_2 0x700   //настройка AF7 для пина2
#define GPIO_AFRL_AF7_3 0x7000   //настройка AF7 для пина3

void UART1_ini_spl (unsigned int );
void UART1_PutByte(char );
void UART1_PutString(char *);

void UART2_ini_spl (unsigned int);
void UART2_init (unsigned int , unsigned int );
void UART2_PutByte(char );
void UART2_PutString(char *);

void UART3_init (unsigned int , unsigned int );
void UART3_ini_spl (unsigned int);
void UART3_PutByte(char);
void UART3_PutString(char *);
void USART3_IRQHandler(void);

int koeff_brr (unsigned int, unsigned int );
#endif
