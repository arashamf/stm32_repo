// Работа с LCD CHAR.

#define stm32f4xx
#include "stm32f4xx.h" // описание периферии
#include "stdio.h" // описание стандартных функций
#include "string.h" // библиотека C работы со строками
#include "LCD_STM32.h"
#define GPIO_AFRH_AF7_8 0x00000007
#define GPIO_AFRH_AF7_9 0x00000070

unsigned char msg[30]; // массив для формирования сообщений для вывода по UART
char buffer[20]; // массив для хранения принятых данных
unsigned char ykaz_buffer = 0; // указатель на следующий символ в массиве
unsigned char receive_flag = 0;  //флаг принятия данных

//функция работы с UART на ввод, обработка прерываний от USART3, прием байта
void USART3_IRQHandler(void)
{
/*окончание проверки аппаратных флагов ошибок*/
USART3->SR &= ~USART_SR_RXNE; // сбросим флаг прерывания
buffer[ykaz_buffer++] = USART3->DR; // записываем данные в буфер и увеличиваем указатель
if((buffer[ykaz_buffer-2] == 0x0D) && (buffer[ykaz_buffer-1] == 0x0A))
	receive_flag = 1; // 1- флаг принятия данных
}

// Передача байта по UART3
void UART3_PutByte(char c)
{
while(!(USART3->SR & USART_SR_TC)) {}; // проверяем готовность DR к приему (предыдущие данные переданы)
USART3->DR = c; // передаем байт
}

// Отправка строки символов, размещенной в RAM по UART3
void UART3_PutRAMString(char *str)
{
	char c;
	while((c = *(str++))) // отправляем символы строки последовательно, пока не достигнут символ конца строки "0x00"
		UART3_PutByte(c);
}

int main()
{
// настройка портов PD8 и PD9 под UART
RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; // запускаем тактовый генератор GPIOD
GPIOD->MODER |= GPIO_MODER_MODER8_1; // настраиваем вывод на альтернативную функцию
GPIOD->AFR[1] |= GPIO_AFRH_AF7_8;  //зададим альтернативную функцию - USART3_TX
GPIOD->MODER |= GPIO_MODER_MODER9_1; // настраиваем вывод на альтернативную функцию
GPIOD->AFR[1] |= GPIO_AFRH_AF7_9; // зададим альтернативную функцию - USART3_RX
// настройка UART
RCC->APB1ENR |= RCC_APB1ENR_USART3EN; // запускаем тактовый генератор USART3
USART3->CR1 |= USART_CR1_TE; // включить передатчик
USART3->CR1 |= USART_CR1_RE; // включить приемник
USART3->BRR = (17 << 4) + 6; //
USART3->CR1 |= USART_CR1_RXNEIE; // разрешаем прерывания при приеме
USART3->CR1 |= USART_CR1_UE; // включить USART
USART3->SR &= ~USART_SR_RXNE; // сбросим флаг прерывания
NVIC_EnableIRQ(USART3_IRQn); // разрешаем прерывание в контроллере прерываний
__enable_irq(); // разрешим прерывания глобально

//настройка LCD
LCD_STM_INIT (); //настрока портов МК, подключаемых к LCD
LCD_CHAR_init(16); // инициализация	16 символьной строки
LCD_CHAR_gotoxy(0,0); // установим позицию индикатора для вывода
LCD_CHAR_puts("Hello,"); // выведем строку из RAM
LCD_CHAR_gotoxy(0,1); // установим позицию индикатора для вывода
LCD_CHAR_puts("UART:"); // выведем строку из RAM

UART3_PutRAMString("HELLO!:\r\n");

// основной цикл программы
while(1)
	{
	if (receive_flag)
	{
		UART3_PutRAMString(buffer);
		for(unsigned char i = 0; i <= ykaz_buffer; i++)
		if ((*(buffer + i) == '\r') || (*(buffer + i) == '\n'))
				*(buffer + i) = '\0'; //убираем из буфера символы \r и \n
		LCD_CHAR_wr(0x01, 0); //очистить дисплей и установить курсор в начальную позицию
		delay_ms(2);
		LCD_CHAR_gotoxy(0,0); //перемещаем курсор в начало верхней строки
		LCD_CHAR_puts("get_message:");
		LCD_CHAR_gotoxy(0,1); //перемещаем курсор в начало нижней строки
		LCD_CHAR_puts(buffer);
		receive_flag = 0; //флаг приема данных (команды): 1 - принято, 0 - нет
		memset(buffer, '\0', sizeof(buffer));  //очищаем буффер
		ykaz_buffer = 0; //устанавливаем указатель на начало буфера
		}
	}
}
