#define stm32f4xx
#include "stm32f4xx.h" // описание периферии
#include "stdio.h" // описание стандартных функций
#include "UART_STM32.h" 

#define GPIO_AFRH_AF7_8 0x00000007
#define GPIO_AFRH_AF7_9 0x00000070

char buffer[100]; // массив для хранения принятых данных
char msg[100]; // массив для формирования сообщений для вывода по UART
/*unsigned char ykaz_buffer = 0; // указатель на следующий символ в массиве
unsigned char receive_flag;  //флаг принятия данных*/

//функция инициализации UART3
void UART3_init (unsigned int freq, unsigned int bandwith)
{
//настройка портов PD8 и PD9для UART3
RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; // запускаем тактовый генератор GPIOD
GPIOD->MODER |= GPIO_MODER_MODER8_1; // настраиваем вывод на альтернативную функцию
GPIOD->AFR[1] |= GPIO_AFRH_AF7_8; // зададим конкретную альтернативную функцию - USART3_TX
GPIOD->MODER |= GPIO_MODER_MODER9_1; // настраиваем вывод на альтернативную функцию
GPIOD->AFR[1] |= GPIO_AFRH_AF7_9; // зададим конкретную альтернативную функцию - USART3_RX
// настройка UART
RCC->APB1ENR |= RCC_APB1ENR_USART3EN; // запускаем тактовый генератор USART3
USART3->CR1 |= USART_CR1_TE; // включить передатчик
USART3->CR1 |= USART_CR1_RE; // включить приемник
/* USART_BRR = (fck + baudrate /2 ) / baudrate
расчет коэффициентов для требуемой скорости: тактовая частота 16 МГц. требуемая скорость 57600 бод/с.
USARTDIV = тактовая частота/(16 * требуемая скорость) = 16000000/(16*57600) = 17.36 
"DIV_Mantissa" = 17; "DIV_Fraction" = 16*0.36 = 5.76; округляем до ближайшего целого - 6 = 0x06
второй способ: USART_BRR=(fck + baudrate/2)/baudrate=278=0х116*/
//USART3->BRR = (17 << 4) + 6; //
USART3->BRR = koeff_brr (freq, bandwith);
USART3->CR1 |= USART_CR1_UE; // включить UART
}


//функция инициализации UART3 с прерыванием
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
USART3->SR &= ~USART_SR_RXNE; // сбросим флаг прерывания
NVIC_EnableIRQ(USART3_IRQn); // разрешаем прерывание в контроллере прерываний
}

//ф-я обработки прерывания от UART3, прием байта
/*void USART3_IRQHandler(void)
{
unsigned long USART_SR_COPY; // переменная для хранения копии регистра SR (при считывании SR флаги сбрасываются)

USART_SR_COPY = USART3->SR; // переменная для хранения копии регистра SR
// проверка аппаратных флагов ошибок
if(USART_SR_COPY & USART_SR_ORE) //Overrun error. Флаг устанавливается, когда новые данные не успеваются считыватся. Генерирует прерывание и сбрасывается, когда считывается регистр статуса,
	//а потом считывается регистр данных. Исчезают входящие данные только в сдвиговом регистре, т.е. пока не прочитаются данные из регистра данных, этим данным ничего не грозит.
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
//окончание проверки аппаратных флагов ошибок//
USART3->SR &= ~USART_SR_RXNE; // сбросим флаг прерывания
buffer[ykaz_buffer++] = USART3->DR; // записываем данные в буфер и увеличиваем указатель
if((buffer[ykaz_buffer-2] == 0x0D) && (buffer[ykaz_buffer-1] == 0x0A))
	receive_flag = 1; // 1- флаг принятия данных
}*/

// ф-я передачи байта по UART3
void UART3_PutByte(char c)
{
while(!(USART3->SR & USART_SR_TC)) {}; // проверяем готовность DR к приему (предыдущие данные переданы)
USART3->DR = c; // передаем байт
}

//ф-я отправки строки символов по UART3
void UART3_PutRAMString(char *str)
{
	char c;
	while((c = *str++)) // отправляем символы строки последовательно, пока не достигнут символ конца строки "\0"
		UART3_PutByte(c);
}

//ф-я расчёта  значения регистра USART_BRR
int koeff_brr (unsigned int freq, unsigned int bandwith)
{
int usart_brr = 0;
return usart_brr = (freq + (bandwith/2))/bandwith;
}
