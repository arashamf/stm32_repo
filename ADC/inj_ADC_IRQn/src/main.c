
 //АЦП. Инжектированные каналы. Программный запуск 2 канала (опрос и прерывания).
#define stm32f4xx
#include "stm32f4xx.h" // описание периферии
#include "stdio.h" // описание стандартных функций
// определения частот шин микроконтроллера
#define APB1_FREQUENCY 16000000L // частота процессора
#define UART_BANDWITH 57600 //пропускная способность UART
#include "UART_STM32.h"
#include "DELAY_TIM7.h"
char msg[100]; // массив для формирования сообщений для вывода по UART
//ШИМ
#define LENGTH 500
// АЦП
float Vref = 3.3; // опорное напряжение для АЦП
float ADC_float; // результат АЦП в виде напряжения

int main()
{
// настройка портов АЦП
SET_BIT (RCC->AHB1ENR, RCC_AHB1ENR_GPIOCEN); // запускаем тактовый генератор GPIOC
SET_BIT (GPIOC->MODER, GPIO_MODER_MODER2_0 | GPIO_MODER_MODER2_1); // настраиваем вывод PC2 на аналоговый режим
SET_BIT (GPIOC->MODER, GPIO_MODER_MODER1_0 | GPIO_MODER_MODER1_1); // настраиваем вывод PC1 на аналоговый режим

// настройка АЦП
SET_BIT (RCC->APB2ENR, RCC_APB2ENR_ADC1EN); // включаем тактовый генератор
// в качестве опорного напряжения используется AVDD и AVSS
// инжектированные каналы: для 1 канала номер должен быть записан в JSQ4
// для 2 каналов номера каналов последовательности должны быть записаны в JSQ3 и JSQ4
// для 3 каналов номера каналов последовательности должны быть записаны в JSQ2 и JSQ3 и JSQ4
// для 4 каналов номера каналов последовательности должны быть записаны в JSQ1 и JSQ2 и JSQ3 и JSQ4
CLEAR_BIT (ADC1->CR1, (ADC_CR1_RES_0 | ADC_CR1_RES_1)); //12 бит АЦП
CLEAR_BIT (ADC1->CR2, ADC_CR2_ALIGN); //правое выравнивание битов
SET_BIT (ADC1->JSQR, ADC_JSQR_JL_0); //2 преобразования
SET_BIT (ADC1->JSQR, (ADC_JSQR_JSQ3_0 | ADC_JSQR_JSQ3_1 | ADC_JSQR_JSQ3_3)); //если преобразований два, то первый канал (канал 11 в PC1) в JSQ_3, второй в JSQ_4 (канал 12 в PC2)
SET_BIT (ADC1->JSQR, (ADC_JSQR_JSQ4_2 | ADC_JSQR_JSQ4_3));
SET_BIT (ADC1->CR1, ADC_CR1_JEOCIE); // разрешим прерывания по окончанию преобразования
SET_BIT (ADC1->CR1, ADC_CR1_SCAN); // запуск АЦП на сканирование выбранных каналов
SET_BIT (ADC1->CR2, ADC_CR2_ADON); // включаем АЦП

UART3_init (APB1_FREQUENCY, UART_BANDWITH); // инициализация UART3
UART3_PutRAMString ("injected channels:\r\n");

SET_BIT (ADC1->CR2, ADC_CR2_JSWSTART); // программный запуск АЦП

while(1)
	{
	if(READ_BIT (ADC1->SR, ADC_SR_JEOC)) // проверим установку флага окончания преобразования
		{
		sprintf(msg,("ADC_result 11 = %f V, ADC_result 12 = %f V\r\n"), (Vref*ADC1->JDR1/4096), (Vref*ADC1->JDR2/4096)); //выводим значение АЦП в строку
		UART3_PutRAMString (msg); //выводим строку по UART
		CLEAR_BIT (ADC1->SR, ~ADC_SR_JEOC); //сбросим флаг перывания
//		ADC1->CR2 |= ADC_CR2_JSWSTART;
		}
	delay_ms (800);
	}
}
