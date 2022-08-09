//программа отправки и декодирования команд с выводом сообщения об ошибках
#define stm32f4xx
#include "stm32f4xx.h" // описание периферии
#include "stdio.h" // описание стандартных функций
#include "string.h"
#define GPIO_AFRH_AF7_8 0x00000007
#define GPIO_AFRH_AF7_9 0x00000070
//char msg[100]; // массив для формирования сообщений для вывода по UART
char buffer[90]; // массив для хранения принятых данных
unsigned char ykaz_buffer = 0; // указатель на следующий символ в массиве
unsigned char receive_flag;

// Передача байта по UART3
void UART3_PutByte(char c)
{
while(!(USART3->SR & USART_SR_TC)) {}; // проверяем готовность DR к приему (предыдущие данные переданы)
USART3->DR = c; // передаем байт
}

// функция отправки строки символов, размещенной в RAM по UART3
void UART3_PutRAMString(char *str)
{
char c;
// отправляем символы строки последовательно, пока не достигнут символ конца строки "0x00"
while(c = *str++)
	UART3_PutByte(c);
}

//работа с UART на ввод, обработка прерываний от USART3, прием байта
void USART3_IRQHandler(void)
{
USART3->SR &= ~USART_SR_RXNE; // сбросим флаг прерывания
buffer [ykaz_buffer++] = USART3->DR;  // записываем данные в буфер и увеличиваем указатель
// 0x0D и 0xOA - код окончания данных, перевод каретки и новая строка соответсвенно
if ((buffer [ykaz_buffer-2] == 0x0D) && (buffer [ykaz_buffer-1] == 0x0A))
	{
	receive_flag = 1;
	UART3_PutRAMString("get!\r");
	}
}

int main()
{
// настройка порта D
RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; // запускаем тактовый генератор GPIOD
GPIOD->MODER |= GPIO_MODER_MODER8_1 | GPIO_MODER_MODER9_1; // настраиваем вывод 8 и 9 на альтернативную функцию
GPIOD->AFR[1] |= GPIO_AFRH_AF7_8; // зададим пину 8 альтернативную функцию - USART3_TX
GPIOD->AFR[1] |= GPIO_AFRH_AF7_9; // зададим пину 9 альтернативную функцию - USART3_RX
GPIOD->MODER |= GPIO_MODER_MODER12_0; //пин 12 на обычный выход
GPIOD->OTYPER &= ~GPIO_OTYPER_OT_12;  //пин 12 на выход с подтяжкой (push-pull)
GPIOD->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR12;  //скорость пина 12 самая низкая
// настройка UART
RCC->APB1ENR |= RCC_APB1ENR_USART3EN; // запускаем тактовый генератор USART3
USART3->CR1 |= USART_CR1_TE | USART_CR1_RE; // включить передатчик и приёмник
USART3->BRR = (17 << 4) + 6; //
USART3->CR1 |= USART_CR1_RXNEIE; // разрешаем прерывания при приеме
USART3->CR1 |= USART_CR1_UE; // включаем USART
USART3->SR &= ~USART_SR_RXNE; // сбросим флаг прерывания на всякий случай
NVIC_EnableIRQ(USART3_IRQn); // разрешаем прерывание в контроллере прерываний
__enable_irq(); // разрешим прерывания глобально

UART3_PutRAMString("HELLO!:\r\n");
UART3_PutRAMString("UART_RX_INT:\r\n");

while(1)
	{
	if (receive_flag)
		{
		for(unsigned char i = 0; i <= ykaz_buffer; i++)
		if ((*(buffer + i) == '\r') || (*(buffer + i) == '\n'))
				*(buffer + i) = '\0'; //убираем из буфера символы \r и \n
		if(!strcmp(buffer, "led_on"))
			{
			receive_flag = 0; //обнуляем флаг принятия данных
			for(unsigned char i = 0; i <= ykaz_buffer; i++)
				*(buffer + i) = '\0'; // очищаем буфер
			ykaz_buffer = 0; // устанавливаем указатель в начало буфера
			GPIOD->BSRRL |= GPIO_BSRR_BS_12; // включаем светодиод
		}
/*функция strcmp из библиотеки string.h возвращает положительное значение, если в машинной схеме упорядочения строка sl
следует за строкой s2, значение О, если строки идентичны, и отрицательное значение, если первая строка предшествует второй*/
	else
		if(!strcmp(buffer, "led_off"))
			{
			receive_flag = 0; //обнуляем флаг принятия данных
			for(unsigned char i = 0; i <= ykaz_buffer; i++)
				*(buffer+i) = '\0'; // очищаем буфер
			ykaz_buffer = 0; // устанавливаем указатель в начало буфера
			GPIOD->BSRRH |= GPIO_BSRR_BS_12;  // выключаем светодиод
			}
		else
			{
//			sprintf(msg,"Error\r\n");
			UART3_PutRAMString("error of command!\r\n");
			receive_flag = 0; //обнуляем флаг принятия данных
			for(unsigned char i = 0; i <= ykaz_buffer; i++)
				*(buffer+i) = '\0'; // очищаем буфер
			ykaz_buffer = 0; // устанавливаем указатель в начало буфера
			}
		}
	}
}
