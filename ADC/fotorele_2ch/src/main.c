// АЦП. Программный запуск. Фотореле. 1 регулярный канал

#define stm32f4xx
#include "stm32f4xx.h" // описание периферии
#include "stdio.h" // описание стандартных функций

// определения частот шин МК
#define BANDWITH 57600 //пропускная способность шины
#define APB1_FREQUENCY 16000000L // частота шины APB1

#include "DELAY_TIM7.h" // функции задержки
#include "UART_STM32.h" // объявления функций для работы с USART3
char msg[130]; // массив для формирования сообщений для вывода по UART

// АЦП
float Vref = 3.3; // опорное напряжение для АЦП
float ADC_float; // результат АЦП в виде напряжения

// пороги переключения фотореле, В
float theshold_hi = 2.8; // верхний
float theshold_lo = 1.6; // нижний

int main()
{
//настройка вывода 12 для световой сигнализации
RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; // запускаем тактовый генератор GPIOD
GPIOD->MODER |= GPIO_MODER_MODER12_0; //пин 12 на обычный выход

// настройка порта С для АЦП
RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN; // запускаем тактовый генератор GPIOC
GPIOC->MODER |= GPIO_MODER_MODER2_1 | GPIO_MODER_MODER2_0; // настраиваем вывод PC2 на аналоговый режим
GPIOC->MODER |= GPIO_MODER_MODER1_1 | GPIO_MODER_MODER1_0; // настраиваем вывод PC1 на аналоговый режим

// настройка АЦП
RCC->APB2ENR |= RCC_APB2ENR_ADC1EN; // включаем тактовый генератор
// установлено при сбросе
// регистр CR1: RES[1:0] = 0: разрешение 12 бит
// регистр CR2: ALIGN = 0 - правое выравнивание
// в качестве опорного напряжения используется AVDD и AVSS
/*разряды SQx[4:0] задают номер канала, где х – это номер преобразования.*/
ADC1->SQR1 &= ~ADC_SQR1_L; //зададим количество каналов регулярной группы: 1
ADC1->SQR3 |= ADC_SQR3_SQ1_0 | ADC_SQR3_SQ1_1| ADC_SQR3_SQ1_3;; // канал 11 (PC1)
ADC1->CR2 |= ADC_CR2_ADON; // включаем АЦП
UART3_init(APB1_FREQUENCY, BANDWITH); // инициализация UART3
UART3_PutRAMString("ADC fotorele:\r\n");

while(1)
	{
	ADC1->CR2 |= ADC_CR2_SWSTART; // программный запуск АЦП, после каждого преобразования необходимо запускать повторно
	while(!(ADC1->SR & ADC_SR_EOC)) {}; // регистр ADC_SR - статусный регистр, проверяем флаг, сигнализирующий об окончании преобразования
	ADC_float = Vref*ADC1->DR/4096; // опорное напряжение*результат преобразования/максимальное число дискретов
	sprintf(msg,("ADC_result 11 = %f V\r\n"), ADC_float); // выводим значение АЦП в строку
	UART3_PutRAMString(msg); // выводим строку по UART
	// проверка порога
	if(ADC_float > theshold_hi)
		GPIOD->BSRRL |= GPIO_BSRR_BS_12; // включаем 12 пин
	else
		if(ADC_float < theshold_lo)
			GPIOD->BSRRH |= GPIO_BSRR_BS_12; // выключаем 12 пин
	delay_ms(1000); // задержка 1 с
	}
}
