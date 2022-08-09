//Работа с UART. Прием данных. Проверка аппаратных флагов ошибок.
#define stm32f4xx
#include "stm32f4xx.h" // описание периферии
#include "stdio.h" // описание стандартных функций
#include "string.h" // библиотека работы со строками
#include "DELAY_TIM7.h" // функции задержки
#define GPIO_AFRH_AF7_8 0x00000007
#define GPIO_AFRH_AF7_9 0x00000070
char msg[130]; // массив для формирования сообщений для вывода по UART
char buffer[100]; // массив для хранения принятых данных
unsigned char ykaz_buffer = 0; // указатель на следующий символ в массиве
unsigned char receive_flag;  //флаг принятия данных

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

//работа с UART на ввод, обработка прерываний от USART3, прием байта
void USART3_IRQHandler(void)
{
unsigned long USART_SR_COPY; // переменная для хранения копии регистра SR (при считывании SR флаги сбрасываются)

USART_SR_COPY = USART3->SR; // переменная для хранения копии регистра SR
/* проверка аппаратных флагов ошибок*/
if(USART_SR_COPY & USART_SR_ORE) /*Overrun error. Флаг устанавливается, когда новые данные не успеваются считыватся. Генерирует прерывание и сбрасывается, когда считывается регистр статуса,
	а потом считывается регистр данных. Исчезают входящие данные только в сдвиговом регистре, т.е. пока не прочитаются данные из регистра данных, этим данным ничего не грозит.*/
	{
	sprintf(msg, "Error ORE\r\n"); UART3_PutRAMString(msg);
	}
if(USART_SR_COPY & USART_SR_NE) // Noise error flag. Шум в линии, вызывает прерывание. Сбрасывается также как и ORE.
	{
	sprintf(msg, "Error NE\r\n"); UART3_PutRAMString(msg);
	}
if(USART_SR_COPY & USART_SR_FE) // Frame Error. Данные побились при передачи. Например, частота плавает у передатчика и в линию идут некорректные данные. Дает прерывание, сбрасывается также как и ORE.
	{
	sprintf(msg, "Error FE\r\n"); UART3_PutRAMString(msg);
	}
if(USART_SR_COPY & USART_SR_PE) //Parity Error-ошибка контроля четности. Очищается аналогично, но тут надо перед сборосом подождать пока установится бит RXNE. Дает прерывание.
	{
	sprintf(msg, "Error PE\r\n"); UART3_PutRAMString(msg);
	}
/*окончание проверки аппаратных флагов ошибок*/
USART3->SR &= ~USART_SR_RXNE; // сбросим флаг прерывания
buffer[ykaz_buffer++] = USART3->DR; // записываем данные в буфер и увеличиваем указатель
if((buffer[ykaz_buffer-2] == 0x0D) && (buffer[ykaz_buffer-1] == 0x0A))
	{
	receive_flag = 1; // 1- флаг принятия данных
	UART3_PutRAMString("get!\r");
	}
//delay_ms(2000);
}

int main()
{
// настройка порта PD12 для светодиода
RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; //тактируем порт IO D
GPIOD->MODER |= GPIO_MODER_MODER12_0; // на обычный выход
// настройка портов PD8 и PD9 под UART
GPIOD->MODER |= GPIO_MODER_MODER8_1; // настраиваем вывод на альтернативную функцию
GPIOD->AFR[1] |= GPIO_AFRH_AF7_8;
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
