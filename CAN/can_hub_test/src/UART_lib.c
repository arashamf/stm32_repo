#define stm32f4xx
#include "stm32f4xx.h" // описание периферии
#include "stdio.h" // описание стандартных функций
#include "UART_lib.h" 

#define GPIO_AFRH_AF7_8 0x00000007 //настройка AF7 для пина8
#define GPIO_AFRH_AF7_9 0x00000070  //настройка AF7для пина9
#define GPIO_AFRL_AF7_2 0x700   //настройка AF7 для пина2
#define GPIO_AFRL_AF7_3 0x7000   //настройка AF7 для пина3
char buffer_UART_RX [100]; // массив для хранения принятых данных
char buffer_UART_TX [100]; // массив для формирования сообщений для вывода по UART
volatile unsigned char ptr_array; // указатель на следующий символ в массиве
volatile unsigned char receive_flag;  //флаг принятия данных

//-------------------------функция инициализации UART1 spl-------------------------------//
void UART1_ini_spl (unsigned int baudrate)

	{
	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOA, ENABLE); //затактируем порт А

	GPIO_InitTypeDef 	GPIO_Init_UART1;
	GPIO_Init_UART1.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10; //PA9 - Tx, PA10 - Rx
	GPIO_Init_UART1.GPIO_Mode = GPIO_Mode_AF; //порт А пин 9, 10 на альт. выход
	GPIO_Init_UART1.GPIO_Speed = GPIO_Fast_Speed; //скорость порта высокая
	GPIO_Init_UART1.GPIO_OType = GPIO_OType_PP; //выход с подтяжкой (push-pull)
	GPIO_Init_UART1.GPIO_PuPd = GPIO_PuPd_UP; //вход с подтяжкой вверх
	GPIO_Init (GPIOA, &GPIO_Init_UART1);

	GPIO_PinAFConfig (GPIOA, (GPIO_PinSource9), GPIO_AF_USART1); //установим альт. функцию UART1-TX
	GPIO_PinAFConfig (GPIOA, (GPIO_PinSource10), GPIO_AF_USART1); //установим альт. функцию UART1-RX

	RCC_APB2PeriphClockCmd (RCC_APB2Periph_USART1, ENABLE); //затактируем USART1

	USART_InitTypeDef ini_USART1;
	ini_USART1.USART_BaudRate = baudrate; //пропускная способность канала UART
	ini_USART1.USART_WordLength = USART_WordLength_8b; //1 стартовый бит, 8 бит данных, n стоп-бит
	ini_USART1.USART_StopBits = USART_StopBits_1; //1 стоп-бит
	ini_USART1.USART_Parity = USART_Parity_No; //без проверки бита чётности
	ini_USART1.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;  //включим приём/передачу по USART1
	ini_USART1.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //аппаратное управление потоком отключено
	USART_Init(USART1, &ini_USART1); //запишем настройки USART1

/*	USART_ITConfig(USART1, USART_IT_TXE, ENABLE); //включим прерывание  на передачу при пустом регистре данных
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE); //включим прерывание на приём при полном регистре данных
	USART_ClearITPendingBit(USART1, USART_IT_RXNE); //очистим флаг

	NVIC_EnableIRQ(USART1_IRQn); //разрешим прерывание от USART1
	__enable_irq ();  //разрешим прерывания глобально*/
	USART_Cmd(USART1, ENABLE);  //включим USART2
	}

//-------------------------------ф-я передачи байта по UART1-----------------------------------//
void UART1_PutByte(char c)
{
while(!(USART1->SR & USART_SR_TC)) {}; // проверяем готовность DR к приему (предыдущие данные переданы)
USART1->DR = c; // передаем байт
}

//-------------------------------ф-я отправки строки символов по UART1-----------------------------------//
void UART1_PutString(char *str)
{
	char c;
	while((c = *str++)) // отправляем символы строки последовательно, пока не достигнут символ конца строки "\0"
		UART1_PutByte(c);
}

//-------------------------функция инициализации UART2 spl-------------------------------//
void UART2_ini_spl (unsigned int baudrate)

	{
	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOA, ENABLE); //затактируем порт А

	GPIO_InitTypeDef 	GPIO_Init_UART2;
	GPIO_Init_UART2.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; //PA2 - Tx, PA3 - Rx
	GPIO_Init_UART2.GPIO_Mode = GPIO_Mode_AF; //порт А пин 2, 3 на альт. выход
	GPIO_Init_UART2.GPIO_Speed = GPIO_Fast_Speed; //скорость порта высокая
	GPIO_Init_UART2.GPIO_OType = GPIO_OType_PP; //выход с подтяжкой (push-pull)
	GPIO_Init_UART2.GPIO_PuPd = GPIO_PuPd_UP; //вход с подтяжкой вверх
	GPIO_Init (GPIOA, &GPIO_Init_UART2);

	GPIO_PinAFConfig (GPIOA, (GPIO_PinSource2), GPIO_AF_USART2); //установим альт. функцию UART2-TX
	GPIO_PinAFConfig (GPIOA, (GPIO_PinSource3), GPIO_AF_USART2); //установим альт. функцию UART2-RX

	RCC_APB1PeriphClockCmd (RCC_APB1Periph_USART2, ENABLE); //затактируем USART2

	USART_InitTypeDef ini_USART2;
	ini_USART2.USART_BaudRate = baudrate; //пропускная способность канала UART
	ini_USART2.USART_WordLength = USART_WordLength_8b; //1 стартовый бит, 8 бит данных, n стоп-бит
	ini_USART2.USART_StopBits = USART_StopBits_1; //1 стоп-бит
	ini_USART2.USART_Parity = USART_Parity_No; //без проверки бита чётности
	ini_USART2.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;  //включим приём/передачу по USART2
	ini_USART2.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //аппаратное управление потоком отключено
	USART_Init(USART2, &ini_USART2); //запишем настройки USART2

/*	USART_ITConfig(USART2, USART_IT_TXE, ENABLE); //включим прерывание  на передачу при пустом регистре данных
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); //включим прерывание на приём при полном регистре данных
	USART_ClearITPendingBit(USART2, USART_IT_RXNE); //очистим флаг

	NVIC_EnableIRQ(USART2_IRQn); //разрешим прерывание от USART2
	__enable_irq ();  //разрешим прерывания глобально*/
	USART_Cmd(USART2, ENABLE);  //включим USART2
	}

//-------------------------------функция инициализации UART2 -----------------------------------//
void UART2_init (unsigned int freq, unsigned int bandwith)
{
//--------------------------настройка портов PA2 и PA3 для UART2------------------------------//
SET_BIT (RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN); //затактируем порт А
SET_BIT (GPIOA->MODER, GPIO_MODER_MODER2_1); // настраиваем вывод на альтернативную функцию
GPIOA->AFR[0] |= GPIO_AFRL_AF7_2; // зададим конкретную альтернативную функцию - USART2_TX
SET_BIT (GPIOA->MODER, GPIO_MODER_MODER3_1); // настраиваем вывод на альтернативную функцию
GPIOA->AFR[0] |= GPIO_AFRL_AF7_3; // зададим конкретную альтернативную функцию - USART2_RX
//--------------------------настройка UART------------------------------//
SET_BIT (RCC->APB1ENR, RCC_APB1ENR_USART2EN); // запускаем тактовый генератор USART2
SET_BIT (USART2->CR1, USART_CR1_TE); // включить передатчик
SET_BIT (USART2->CR1, USART_CR1_RE); // включить приемник
//USART3->BRR = (17 << 4) + 6; //USARTDIV = тактовая частота/(16 * скорость) = 16*10^6/(16*57600) = 17.36. "DIV_Mantissa" = 17; "DIV_Fraction" = 16*0.36 = 5.76; округляем до целого ~0x06
//WRITE_REG (USART_BRR, (fck + baudrate /2 ) / baudrate);  //второй способ: USART_BRR=(fck + baudrate/2)/baudrate=278=0х116
WRITE_REG (USART2->BRR, koeff_brr (freq, bandwith)); //
SET_BIT (USART2->CR1, USART_CR1_UE); // включить UART2
}

//-------------------------------ф-я передачи байта по UART2-----------------------------------//
void UART2_PutByte(char c)
{
while(!(USART2->SR & USART_SR_TC)) {}; // проверяем готовность DR к приему (предыдущие данные переданы)
USART2->DR = c; //передаем байт
}

//-------------------------------ф-я отправки строки символов по UART2-----------------------------------//
void UART2_PutString(char *str)
{
	char c;
	while((c = *str++)) // отправляем символы строки последовательно, пока не достигнут символ конца строки "\0"
		UART2_PutByte(c);
}

//-------------------------------функция инициализации UART3 с прерыванием-----------------------------------//
void UART3_init_IRQ (unsigned int freq, unsigned int bandwith)
{
// настройка портов PD8 и PD9 для UART3
RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; //запускаем тактовый генератор порта D
GPIOD->MODER |= GPIO_MODER_MODER8_1; // настраиваем вывод на альтернативную функцию
GPIOD->AFR[1] |= GPIO_AFRH_AF7_8; // зададим альтернативную функцию - USART3_TX
GPIOD->MODER |= GPIO_MODER_MODER9_1; // настраиваем вывод на альтернативную функцию
GPIOD->AFR[1] |= GPIO_AFRH_AF7_9; // зададим альтернативную функцию - USART3_RX
// настройка UART с прерыванием
RCC->APB1ENR |= RCC_APB1ENR_USART3EN; // запускаем тактовый генератор USART3
USART3->CR1 |= USART_CR1_TE; // включить передатчик
USART3->CR1 |= USART_CR1_RE; // включить приемник
//USART3->BRR = (17 << 4) + 6; //расчёт значения регистра USART3->BRR
USART3->BRR = koeff_brr (freq, bandwith);
USART3->CR1 |= USART_CR1_RXNEIE; // разрешаем прерывания при приеме
USART3->CR1 |= USART_CR1_UE; // включить UART3
/*USART3->SR &= ~USART_SR_RXNE; // сбросим флаг прерывания
NVIC_EnableIRQ(USART3_IRQn); // разрешаем прерывание в контроллере прерываний*/
}

//-------------------------функция инициализации UART3 spl-------------------------------//
void UART3_ini_spl (unsigned int baudrate)

	{
	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOC, ENABLE); //затактируем порт C

	GPIO_InitTypeDef 	GPIO_Init_UART3;
	GPIO_Init_UART3.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11; //PC10 - Tx, PC11 - Rx
	GPIO_Init_UART3.GPIO_Mode = GPIO_Mode_AF; //порт C пин 10, 11 на альт. выход
	GPIO_Init_UART3.GPIO_Speed = GPIO_Fast_Speed; //скорость порта высокая
	GPIO_Init_UART3.GPIO_OType = GPIO_OType_PP; //выход с подтяжкой (push-pull)
	GPIO_Init_UART3.GPIO_PuPd = GPIO_PuPd_UP; //вход с подтяжкой вверх
	GPIO_Init (GPIOC, &GPIO_Init_UART3);

	GPIO_PinAFConfig (GPIOC, (GPIO_PinSource10), GPIO_AF_USART3); //установим альт. функцию UART3-TX
	GPIO_PinAFConfig (GPIOC, (GPIO_PinSource11), GPIO_AF_USART3); //установим альт. функцию UART3-RX

	RCC_APB1PeriphClockCmd (RCC_APB1Periph_USART3, ENABLE); //затактируем USART3

	USART_InitTypeDef ini_USART3;
	ini_USART3.USART_BaudRate = baudrate; //пропускная способность канала UART
	ini_USART3.USART_WordLength = USART_WordLength_8b; //1 стартовый бит, 8 бит данных, n стоп-бит
	ini_USART3.USART_StopBits = USART_StopBits_1; //1 стоп-бит
	ini_USART3.USART_Parity = USART_Parity_No; //без проверки бита чётности
	ini_USART3.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;  //включим приём/передачу по USART3
	ini_USART3.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //аппаратное управление потоком отключено
	USART_Init(USART3, &ini_USART3); //запишем настройки USART3

/*	USART_ITConfig(USART3, USART_IT_TXE, ENABLE); //включим прерывание  на передачу при пустом регистре данных
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); //включим прерывание на приём при полном регистре данных
	USART_ClearITPendingBit(USART3, USART_IT_RXNE); //очистим флаг

	NVIC_EnableIRQ(USART3_IRQn); //разрешим прерывание от USART3
	__enable_irq ();  //разрешим прерывания глобально*/
	USART_Cmd(USART3, ENABLE);  //включим USART3
	}

//-------------------------------ф-я передачи байта по UART3-----------------------------------//
void UART3_PutByte(char c)
{
while(!(USART3->SR & USART_SR_TC)) {}; // проверяем готовность DR к приему (предыдущие данные переданы)
USART3->DR = c; // передаем байт
}

//-------------------------------ф-я отправки строки символов по UART3-----------------------------------//
void UART3_PutString(char *str)
{
	char c;
	while((c = *str++)) // отправляем символы строки последовательно, пока не достигнут символ конца строки "\0"
		UART3_PutByte(c);
}

//-------------------------------ф-я обработки прерывания от UART3, прием байта-----------------------------------//
/*void USART3_IRQHandler(void)
{
unsigned long USART_SR_COPY; // переменная для хранения копии регистра SR (при считывании SR флаги сбрасываются)

USART_SR_COPY = USART3->SR; // переменная для хранения копии регистра SR
// проверка аппаратных флагов ошибок
if(USART_SR_COPY & USART_SR_ORE) //Overrun error. Флаг устанавливается, когда новые данные не успеваются считыватся. Генерирует прерывание и сбрасывается, когда считывается регистр статуса,
//	а потом считывается регистр данных. Исчезают входящие данные только в сдвиговом регистре, т.е. пока не прочитаются данные из регистра данных, этим данным ничего не грозит.
	{
	sprintf(buffer_UART_TX, "Error ORE\r\n"); UART3_PutRAMString(buffer_UART_TX);
	}
if(USART_SR_COPY & USART_SR_NE) // Noise error flag. Шум в линии, вызывает прерывание. Сбрасывается также как и ORE.
	{
	sprintf(buffer_UART_TX, "Error NE\r\n"); UART3_PutRAMString(buffer_UART_TX);
	}
if(USART_SR_COPY & USART_SR_FE) // Frame Error. Данные побились при передачи. Например, частота плавает у передатчика и в линию идут некорректные данные. Дает прерывание, сбрасывается также как и ORE.
	{
	sprintf (buffer_UART_TX, "Error FE\r\n"); UART3_PutRAMString(buffer_UART_TX);
	}
if(USART_SR_COPY & USART_SR_PE) //Parity Error-ошибка контроля четности. Очищается аналогично, но тут надо перед сборосом подождать пока установится бит RXNE. Дает прерывание.
	{
	sprintf(buffer_UART_TX, "Error PE\r\n"); UART3_PutRAMString (buffer_UART_TX);
	}
//окончание проверки аппаратных флагов ошибок//
USART3->SR &= ~USART_SR_RXNE; // сбросим флаг прерывания
buffer_UART_RX [ptr_array++] = USART3->DR; // записываем данные в буфер и увеличиваем указатель
if((buffer_UART_RX [ptr_array-2] == 0x0D) && (ptr_array == 0x0A))
	receive_flag = 1; // 1- флаг принятия данных
}*/

//-------------------------------ф-я расчёта  значения регистра USART_BRR-----------------------------------//
int koeff_brr (unsigned int freq, unsigned int bandwith)
{
int usart_brr = 0;
return usart_brr = (freq + (bandwith/2))/bandwith;
}
