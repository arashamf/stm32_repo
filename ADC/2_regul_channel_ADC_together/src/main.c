//Последовательный запуск АЦП для регулярных каналов. Запуск по таймеру.
#define stm32f4xx
#include "stm32f4xx.h" // описание периферии
#include "stdio.h" // описание стандартных функций
// определения частот шин
#define APB1_FREQUENCY 16000000L // частота процессора
#define UART_BANDWITH 57600 //пропускная способность UART
typedef unsigned char byte;

#include "DELAY_TIM7.h" // функции задержки
#include "UART_STM32.h" // объявления функций для работы с USART3
char msg[100]; // массив для формирования сообщений для вывода по UART

//ШИМ
#define LENGTH 200

// АЦП
float Vref = 3.3; // опорное напряжение для АЦП
float ADC_float; // результат АЦП в виде напряжения

int main()
{
// настройка портов АЦП
SET_BIT (RCC->AHB1ENR, RCC_AHB1ENR_GPIOCEN); // запускаем тактовый генератор GPIOC
SET_BIT (GPIOC->MODER, GPIO_MODER_MODER2_0 | GPIO_MODER_MODER2_1); // настраиваем вывод PC2 на аналоговый режим
SET_BIT (GPIOC->MODER, GPIO_MODER_MODER1_0 | GPIO_MODER_MODER1_1); // настраиваем вывод PC1 на аналоговый режим

// настройка АЦП для канала 11
SET_BIT (RCC->APB2ENR, RCC_APB2ENR_ADC1EN); // включаем тактовый генератор АЦП1;
//настройка регулярных каналов АЦП
CLEAR_BIT (ADC1->CR1, (ADC_CR1_RES_0 | ADC_CR1_RES_1)); //12 бит АЦП
CLEAR_BIT (ADC1->CR2, ADC_CR2_ALIGN); //правое выравнивание битов
CLEAR_BIT (ADC1->SQR1, ADC_SQR1_L); // преобразование 1 канала
SET_BIT (ADC1->SQR3, (ADC_SQR3_SQ1_0 | ADC_SQR3_SQ1_1 | ADC_SQR3_SQ1_3)); // канал 11 (PC1)
CLEAR_BIT (ADC1->CR2, ADC_CR2_EOCS); //если бит EOCS=0: бит EOC (регистр ADC_CSR) устанавливается в конце каждой последовательности регулярных преобразований. Обнаружение переполнения включено, если DMA=1.
//если бит EOCS=1: бит EOC устанавливается в конце каждого регулярного преобразования. Включено обнаружение переполнения.
SET_BIT (ADC1->CR1, ADC_CR1_EOCIE); //бит EOCIE включает/выключает прерывания после окончания преобразования АЦП. Прерывание генерируется, когда установлен бит EOC (регистр ADC_CSR)
SET_BIT (ADC1->CR1, ADC_CR1_OVRIE); // разрешим прерывания при перезаписи
//SET_BIT (ADC->CCR, ADC_CCR_ADCPRE);  //ADC prescaler. делитель входной частоты АЦП на 8
SET_BIT (ADC1->CR1, ADC_CR1_SCAN); // запуск АЦП на сканирование выбранных каналов
SET_BIT (ADC1->CR2, ADC_CR2_EXTEN_0); // триггер срабатывает по нарастающему фронту
SET_BIT (ADC1->CR2, (ADC_CR2_EXTSEL_2 | ADC_CR2_EXTSEL_1)); // в качестве триггера выбран сигнал "Timer 2 TRGO event"

// настройка АЦП для канала 12
SET_BIT (RCC->APB2ENR, RCC_APB2ENR_ADC2EN); // включаем тактовый генератор АЦП2;
SET_BIT (ADC2->SQR3, (ADC_SQR3_SQ2_2 | ADC_SQR3_SQ2_3)); //канал 12 (PC2)
//SET_BIT (ADC->CCR, (ADC_CCR_MULTI_2 | ADC_CCR_MULTI_1 | ADC_CCR_MULTI_0)); //Настройка режима мульти-АЦП. В данном случае: двойной режим, только режим чередования
SET_BIT (ADC->CCR, (ADC_CCR_MULTI_1)); //Настройка режима мульти-АЦП. В данном случае: двойной режим, комбинированный регулярный одновременный + альтернативный запуск от триггера
SET_BIT (ADC->CCR, (ADC_CCR_DELAY_0 | ADC_CCR_DELAY_1)); //задержка запуска 2-го АЦП - 8 тактов. Эти биты используются в двойных или тройных чередующихся режимах
SET_BIT (ADC1->CR2, ADC_CR2_ADON); // включаем АЦП1
SET_BIT (ADC2->CR2, ADC_CR2_ADON); // включаем АЦП2

UART3_init(APB1_FREQUENCY, UART_BANDWITH); // инициализация UART3
UART3_PutRAMString("Multi ADC mode:\r\n");

// настройка таймера TIM2
SET_BIT (RCC->APB1ENR, RCC_APB1ENR_TIM2EN); // включаем тактовый генератор TIM2
WRITE_REG(TIM2->PSC, 8000-1); // предделитель
WRITE_REG(TIM2->ARR, 2000-1); // значение перезагрузки
SET_BIT (TIM2->CCER, TIM_CCER_CC1E); // разрешим вывод сигнала со схемы сравнения на вывод МК*/
SET_BIT (TIM2->CR2, TIM_CR2_MMS_1); // в качестве TRGO будет использоваться событие "Update"
SET_BIT (TIM2->CR1, TIM_CR1_CEN); // разрешим работу таймера

NVIC_EnableIRQ(ADC_IRQn); // разрешим прерывания ADC в NVIC
//delay_ms (1000);

while(1)
	{
	}
}

// обработка прерываний от ADC
void ADC_IRQHandler(void)
{
if(ADC1->SR & ADC_SR_EOC) // если сработало прерывание от ADC1 EOC
	{
	sprintf(msg,("ADC1_result = %5.2f V\r\n"), Vref*ADC1->DR/4096); // выводим значение АЦП1 в строку
	UART3_PutRAMString (msg); // выводим строку по UART
	ADC1->SR &= ~ADC_SR_EOC; // сбросим флаг перывания
	}
if(ADC2->SR & ADC_SR_EOC) // если сработало прерывание от ADC1 EOC
	{
	sprintf(msg,("ADC2_result = %5.2f V\r\n"), Vref*ADC2->DR/4096); // выводим значение АЦП2 в строку
	UART3_PutRAMString (msg); // выводим строку по UART
	ADC2->SR &= ~ADC_SR_EOC; // сбросим флаг перывания
	}
}

