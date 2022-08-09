//программа приема данных по UART по прерыванию
#define stm32f4xx
#include "stm32f4xx.h" // описание периферии
#include "stdio.h" // описание стандартных функций
#define GPIO_AFRH_AF7_8 0x00000007
#define GPIO_AFRH_AF7_9 0x00000070
char msg[130]; // массив для формирования сообщений для вывода по UART
char buffer[99]; // массив для хранения принятых данных
unsigned char ykaz_buffer = 0; // указатель на следующий символ в массиве

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
GPIOD->BSRRL |= GPIO_BSRR_BS_12; // включаем светодиод
buffer [ykaz_buffer++] = USART3->DR;  

if ((buffer [ykaz_buffer-2] == 0x0D) && (buffer [ykaz_buffer-1] == 0x0A)) // 0x0D и 0xOA - окончание данных
	{
	sprintf(msg,"You say - %s", buffer);
	UART3_PutRAMString(msg);
	for (char i = 0; i <= ykaz_buffer; i++)
		buffer [i] = 0;
	ykaz_buffer = 0;
	}
GPIOD->BSRRH |= GPIO_BSRR_BS_12;  // выключаем светодиод
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
{}

}
