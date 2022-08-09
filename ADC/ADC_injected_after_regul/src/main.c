//Автоматический запуск измерений инжектированных каналов после регулярных.
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

/*сохранение результатов преобразования регулярных каналов в массив, т.к. непосредственный вывод их по терминалу в прерывании приводит
 к перезаписи результата (установке флага «OVR»). Добавим также контроль флага перезаписи результата и уменьшим тактовую частоту АЦП*/
float ADC_DR_tmp[16]; // массив для хранения результатов АЦП регулярных каналов
__IO byte ADC_DR_tmp_ptr = 0; // указатель на элемент в массиве ADC_DR_tmp
__IO byte measure_good = 0; // флаг: 1 - измерение готово, 0 - нет
__IO byte is_OVR = 0; // флаг: 1 - есть перезапись результата, 0 - нет
			
//ШИМ
#define LENGTH 200

// АЦП
float Vref = 3.3; // опорное напряжение для АЦП
float ADC_float; // результат АЦП в виде напряжения

// датчик температуры (данные из даташита для расчета значения температуры по формуле)
#define V25 0.76 // VSENSE value for 25° C
#define Avg_Slope 2.5 // mV/°C

int main()
{
// настройка портов АЦП
SET_BIT (RCC->AHB1ENR, RCC_AHB1ENR_GPIOCEN); // запускаем тактовый генератор GPIOC
SET_BIT (GPIOC->MODER, GPIO_MODER_MODER2_0 | GPIO_MODER_MODER2_1); // настраиваем вывод PC2 на аналоговый режим
SET_BIT (GPIOC->MODER, GPIO_MODER_MODER1_0 | GPIO_MODER_MODER1_1); // настраиваем вывод PC1 на аналоговый режим

// настройка АЦП
SET_BIT (RCC->APB2ENR, RCC_APB2ENR_ADC1EN); // включаем тактовый генератор;
// регулярные каналы:
SET_BIT (ADC1->SQR1, ADC_SQR1_L_0); // преобразование 2-х каналов
SET_BIT (ADC1->SQR3, (ADC_SQR3_SQ1_0 | ADC_SQR3_SQ1_1 | ADC_SQR3_SQ1_3)); // канал 11 (PC1)
SET_BIT (ADC1->SQR3, (ADC_SQR3_SQ2_2 | ADC_SQR3_SQ2_3)); // канал 12 (PC2)
SET_BIT (ADC1->CR2, ADC_CR2_EOCS); //если установлен бит EOCS: бит EOC устанавливается в конце каждого регулярного преобразования. Включено обнаружение переполнения.
SET_BIT (ADC1->CR1, ADC_CR1_EOCIE); //разрешим прерывания по окончанию преобразования
// инжектированные каналы: для 1 канала номер должен быть записан в JSQ4
// для 2 каналов номера каналов последовательности должны быть записаны в JSQ3 и JSQ4
// для 3 каналов номера каналов последовательности должны быть записаны в JSQ2 и JSQ3 и JSQ4
// для 4 каналов номера каналов последовательности должны быть записаны в JSQ1 и JSQ2 и JSQ3 и JSQ4
CLEAR_BIT (ADC1->CR1, (ADC_CR1_RES_0 | ADC_CR1_RES_1)); //12 бит АЦП
CLEAR_BIT (ADC1->CR2, ADC_CR2_ALIGN); //правое выравнивание битов
SET_BIT (ADC1->JSQR, ADC_JSQR_JL_1); //3 преобразования
SET_BIT (ADC1->JSQR, (ADC_JSQR_JSQ2_4)); //встроенный датчик температуры: канал 16 в JSQ2
SET_BIT (ADC1->JSQR, (ADC_JSQR_JSQ3_4 | ADC_JSQR_JSQ3_0)); //встроенный датчик опорного напр. VREFINT: канал 17 в JSQ3
SET_BIT (ADC->CCR, ADC_CCR_TSVREFE); // разрешим измерение температурного сенсора и опорного напряжения
SET_BIT (ADC1->JSQR, (ADC_JSQR_JSQ4_4 | ADC_JSQR_JSQ4_1)); //внутр. датчик напр-я батарейки: канал 18 в JSQ4
SET_BIT (ADC->CCR, ADC_CCR_VBATE); // разрешим измерение Vbat
SET_BIT (ADC1->CR1, ADC_CR1_JEOCIE); // разрешим прерывания по окончанию преобразования инжектированных каналов
SET_BIT (ADC1->CR1, ADC_CR1_JAUTO); // разрешим запуск по окончанию преобразования регулярных каналов
SET_BIT (ADC1->CR1, ADC_CR1_OVRIE); //// разрешим прерывания при перезаписи
SET_BIT (ADC->CCR, ADC_CCR_ADCPRE);  //ADC prescaler. делитель входной частоты АЦП на 8
//SET_BIT (ADC->CCR, ADC_CCR_ADCPRE_0); //ADC prescaler. делитель входной частоты АЦП на 4
SET_BIT (ADC1->CR1, ADC_CR1_SCAN); // запуск АЦП на сканирование выбранных каналов

// настройка запуска по триггеру
SET_BIT (ADC1->CR2, ADC_CR2_JEXTEN_1); // триггер срабатывает по спадающему фронту
SET_BIT (ADC1->CR2, ADC_CR2_JEXTSEL_1); // в качестве триггера выбран сигнал "Timer 2 СС1 event"
//SET_BIT (ADC1->CR2, (ADC_CR2_JEXTSEL_1 | ADC_CR2_JEXTSEL_0)); // в качестве триггера выбран сигнал "Timer 2 TRGO event"
SET_BIT (ADC1->CR2, ADC_CR2_ADON); // включаем АЦП

UART3_init(APB1_FREQUENCY, UART_BANDWITH); // инициализация UART3
UART3_PutRAMString("ADC injected channels with :\r\n");

// настройка таймера TIM2
SET_BIT (RCC->APB1ENR, RCC_APB1ENR_TIM2EN); // включаем тактовый генератор TIM2
WRITE_REG(TIM2->PSC, 8000-1); // предделитель
WRITE_REG(TIM2->ARR, 2000-1); // значение перезагрузки
SET_BIT (TIM2->CCMR1, (TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1)); // зададим режим ШИМ 1
WRITE_REG(TIM2->CCR1, LENGTH); // длительность ШИМ
SET_BIT (TIM2->CCER, TIM_CCER_CC1E); // разрешим вывод сигнала со схемы сравнения на вывод МК
SET_BIT (TIM2->CR1, TIM_CR1_CEN); // разрешим работу таймера

NVIC_EnableIRQ(ADC_IRQn); // разрешим прерывания ADC в NVIC

while(1)
	{
	ADC_DR_tmp_ptr = 0; // указатель на элемент в массиве ADC_DR_tmp
	SET_BIT (ADC1->CR2, ADC_CR2_SWSTART); // программный запуск АЦП
	while(!measure_good) {}; // ждем окончания преобразования
	// Регулярные каналы
	UART3_PutRAMString ("ADC_regular_channels:\r\n"); // выводим строку по UART
	sprintf(msg,("ADC_result 1 = %f V\r\n"), Vref*ADC_DR_tmp[0]/4096); // выводим значение АЦП в строку
	UART3_PutRAMString(msg); // выводим строку по UART
	sprintf(msg,("ADC_result 2 = %f V\r\n"), Vref*ADC_DR_tmp[1]/4096); // выводим значение АЦП в строку
	UART3_PutRAMString(msg); // выводим строку по UART
	// Инжектированные каналы
	UART3_PutRAMString("ADC_injected_channels:\r\n"); // выводим строку по UART
	sprintf(msg,("ADC_Temp = %f C, ADC_Vref = %f, ADC_Vbat = %f V\r\n"), (((Vref*ADC1->JDR1/4096)-V25)/Avg_Slope+25.),
	(Vref*ADC1->JDR2/4096), (Vref*ADC1->JDR3/4096*2)); // выводим значение АЦП в строку
	UART3_PutRAMString (msg); // выводим строку по UART
	measure_good = 0; // флаг: 1 - измерение готово, 0 - нет

	// проверка перезаписи
	if(is_OVR)
		{
		UART3_PutRAMString ("ADC_SR_OVR\r\n"); // выводим строку по UART
		is_OVR = 0; // флаг: 1 - есть перезапись результата, 0 - нет
		}
	delay_ms(1000);
	}
}

// обработка прерываний от ADC
void ADC_IRQHandler(void)
{
if(READ_BIT(ADC1->SR, ADC_SR_EOC)) // если сработало прерывание от ADC1 EOC
	{
	ADC_DR_tmp[ADC_DR_tmp_ptr++] = ADC1->DR; // сохраним
	CLEAR_BIT (ADC1->SR, ADC_SR_EOC); // сбросим флаг перывания
	}
if(READ_BIT(ADC1->SR, ADC_SR_JEOC)) // если сработало прерывание от ADC1 JEOC
	{
	measure_good = 1; // флаг: 1 - измерение готово, 0 - нет
	CLEAR_BIT (ADC1->SR, ADC_SR_JEOC); // сбросим флаг перывания
	}
if(READ_BIT (ADC1->SR, ADC_SR_OVR)) // если сработало прерывание от ADC1 OVR - перезапись значения
	{
	is_OVR = 1; // флаг: 1 - есть перезапись результата, 0 - нет
	CLEAR_BIT (ADC1->SR, ADC_SR_OVR); // сбросим флаг перывания
	}
}
