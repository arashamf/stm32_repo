//программа приема данных по UART с проверкой флага

#define stm32f4xx
#include "stm32f4xx.h" // описание периферии
#include "stdio.h" // описание стандартных функций С
#define GPIO_AFRH_AF7_8 0x00000007
#define GPIO_AFRH_AF7_9 0x00000070

//функция передачи байта по uart3
void UART3_PutByte(char c)
{
while(!(USART3->SR & USART_SR_TC)) {}; // проверяем готовность DR к приему
USART3->DR = c; // передаем байт
}

//функция отправки строки символов, размещенной в RAM по uart3
void UART3_PutRAMString(char *str)
{
char c;
while((c = *str++))		// отправляем символы строки последовательно, пока не достигнут символ конца строки "0x00"
	UART3_PutByte(c);
}

char msg[130]; // массив для формирования сообщений для вывода по UART

int main()
{
// настройка порта D
RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; //запускаем тактовый генератор GPIOD
GPIOD->MODER |= GPIO_MODER_MODER8_1; //настраиваем вывод 8 на альтернативную функцию
GPIOD->AFR[1] |= GPIO_AFRH_AF7_8; //зададим конкретную альтернативную функцию - USART3_TX
GPIOD->MODER |= GPIO_MODER_MODER9_1; // настраиваем вывод 9 на альтернативную функцию
GPIOD->AFR[1] |= GPIO_AFRH_AF7_9; //зададим конкретную альтернативную функцию - USART3_RX

// настройка UART
RCC->APB1ENR |= RCC_APB1ENR_USART3EN; // запускаем тактовый генератор USART3
USART3->CR1 |= USART_CR1_TE | USART_CR1_RE; // включить передатчик и приёмник
/* расчет коэффициентов для требуемой скорости: тактовая частота 16 МГц. требуемая скорость 57600 бод/с.
USARTDIV = тактовая частота/(16 * требуемая скорость) = 16000000/(16*57600) = 17.36
"DIV_Mantissa" = 17 = 0x21; "DIV_Fraction" = 16*0.36 = 5.76; округляем до ближайшего целого - 6 = 0x06*/
USART3->BRR = (17 << 4) + 6; //
USART3->CR1 |= USART_CR1_UE; // включить USART
UART3_PutRAMString("HELLO!\r");
UART3_PutRAMString("UART_RX:\r\n");

while(1)
	{
	// Флаг приема данных очищается при считывании регистра «USART3->DR», либо записью «0» в бит RXNE регистра USART_SR
	if(USART3->SR & USART_SR_RXNE) // проверка флага приема байта: 1 - принят, 0 - не принят
		{
		sprintf(msg,"Byte = %c\r\n", USART3->DR);
		UART3_PutRAMString(msg);
		}
	}
}
