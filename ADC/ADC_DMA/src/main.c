//Регулярные каналы. Использование DMA. Запуск по таймеру.
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

// массив для передачи
#define HalfWords_NUM 20 // размерность массива значений
uint16_t HalfWords[HalfWords_NUM]; // массив значений


//ШИМ
#define LENGTH 200

// АЦП
float Vref = 3.3; // опорное напряжение для АЦП
float ADC_float; // результат АЦП в виде напряжения

// DMA2 - инициализация
//
void DMA2_init(void)
{
SET_BIT (RCC->AHB1ENR, RCC_AHB1ENR_DMA2EN); // включаем тактовый генератор DMA2
// настройка DMA производится при сброшенном бите "EN"
WRITE_REG (DMA2_Stream4->PAR, ((uint32_t)(&ADC1->DR))); // адрес периферийного устройства (регистр данных DR)
WRITE_REG (DMA2_Stream4->M0AR, ((uint32_t)(&HalfWords))); // адрес начала памяти
WRITE_REG (DMA2_Stream4->NDTR, HalfWords_NUM); // зададим число байт для передачи
CLEAR_BIT (DMA2_Stream4->CR, DMA_SxCR_CHSEL); //выберем канал 0
CLEAR_BIT (DMA2_Stream4->CR, DMA_SxCR_PFCTRL); //DMA является регулятором потока
SET_BIT (DMA2_Stream4->CR, DMA_SxCR_PL); //приоритет потока - very high
CLEAR_BIT (DMA2_Stream4->CR, DMA_SxCR_MBURST); //Конфигурация передачи пакетов памяти; единичная передача
CLEAR_BIT (DMA2_Stream4->CR, DMA_SxCR_PBURST); //Конфигурация передачи периферийных пакетов; единичная передача
CLEAR_BIT (DMA2_Stream4->CR, DMA_SxCR_DBM); //режим двойного буффера; нет переключения буфера в конце передачи
SET_BIT (DMA2_Stream4->CR, DMA_SxCR_PSIZE_0); //размер периферийных данных; полуслово (16 бит)
SET_BIT (DMA2_Stream4->CR, DMA_SxCR_MSIZE_0); //размер данных памяти; полуслово (16 бит)
CLEAR_BIT (DMA2_Stream4->CR, DMA_SxCR_PINC); //0 - адрес периферийного устройства не изменяется
SET_BIT (DMA2_Stream4->CR, DMA_SxCR_MINC); //1 - указатель адреса памяти увеличивается после каждой передачи данных на значение указанное в MSIZE
CLEAR_BIT (DMA2_Stream4->CR, DMA_SxCR_CIRC);  //круговой режим - выключен
CLEAR_BIT (DMA2_Stream4->CR, DMA_SxCR_TCIE);  //0 - прерывание по завершению передачи выключено
CLEAR_BIT (DMA2_Stream4->CR, DMA_SxCR_HTIE); //0 - прерывание после половины передачи выключено
CLEAR_BIT (DMA2_Stream4->CR, DMA_SxCR_TEIE); //0 - прерывание после ошибок во время передачи выключено
CLEAR_BIT (DMA2_Stream4->CR, DMA_SxCR_DMEIE); // 0 - прерывание после ошибок в Direct mode
CLEAR_BIT (DMA2_Stream4->CR, DMA_SxCR_DIR); // передача из периферийного устройства в память
//DMA2->LIFCR - регистр сброса флагов состояний прерываний DMA-потоков от 0 до 3, бит CTCIF0 сбрасывает флаг TCIF0
//SET_BIT (DMA2->LIFCR, DMA_LIFCR_CTCIF0); // сбросим флаг прерывания завершения передачи потока 0
SET_BIT (DMA2->HIFCR, DMA_HIFCR_CTCIF4); // сбросим флаг прерывания завершения передачи потока 4
//SET_BIT (DMA2_Stream4->CR, DMA_SxCR_EN); // разрешим работу DMA
//UART3_PutRAMString("DMA_completed\r\n");
}

int main()
{
// настройка портов АЦП
SET_BIT (RCC->AHB1ENR, RCC_AHB1ENR_GPIOCEN); // запускаем тактовый генератор GPIOC
SET_BIT (GPIOC->MODER, GPIO_MODER_MODER2_0 | GPIO_MODER_MODER2_1); // настраиваем вывод PC2 на аналоговый режим
SET_BIT (GPIOC->MODER, GPIO_MODER_MODER1_0 | GPIO_MODER_MODER1_1); // настраиваем вывод PC1 на аналоговый режим

// настройка АЦП
SET_BIT (RCC->APB2ENR, RCC_APB2ENR_ADC1EN); // включаем тактовый генератор;
// регулярные каналы:
CLEAR_BIT (ADC1->CR1, (ADC_CR1_RES_0 | ADC_CR1_RES_1)); //12 бит АЦП
CLEAR_BIT (ADC1->CR2, ADC_CR2_ALIGN); //правое выравнивание битов
SET_BIT (ADC1->SQR1, ADC_SQR1_L_0); // преобразование 2-х каналов
SET_BIT (ADC1->SQR3, (ADC_SQR3_SQ1_0 | ADC_SQR3_SQ1_1 | ADC_SQR3_SQ1_3)); // канал 11 (PC1)
SET_BIT (ADC1->SQR3, (ADC_SQR3_SQ2_2 | ADC_SQR3_SQ2_3)); // канал 12 (PC2)
//SET_BIT (ADC1->CR2, ADC_CR2_EOCS); //если установлен бит EOCS: бит EOC устанавливается в конце каждого регулярного преобразования. Включено обнаружение переполнения.
SET_BIT (ADC1->CR1, ADC_CR1_EOCIE); //разрешим прерывания по окончанию преобразования
SET_BIT (ADC1->CR1, ADC_CR1_OVRIE); //// разрешим прерывания при перезаписи
SET_BIT (ADC->CCR, ADC_CCR_ADCPRE);  //ADC prescaler. делитель входной частоты АЦП на 8
SET_BIT (ADC1->CR1, ADC_CR1_SCAN); // запуск АЦП на сканирование выбранных каналов
SET_BIT (ADC1->CR2, ADC_CR2_DMA); // разрешим работу DMA
// настройка запуска по триггеру
SET_BIT (ADC1->CR2, ADC_CR2_EXTEN_1); // триггер срабатывает по спадающему фронту
SET_BIT (ADC1->CR2, (ADC_CR2_EXTSEL_2 | ADC_CR2_EXTSEL_1)); // в качестве триггера выбран сигнал "Timer 2 TRGO event"


//UART3_PutRAMString("ADC with DMA:\r\n");
SET_BIT (ADC1->CR2, ADC_CR2_ADON); // включаем АЦП
UART3_init(APB1_FREQUENCY, UART_BANDWITH); // инициализация UART3
UART3_PutRAMString("ADC with DMA:\r\n");

// настройка таймера TIM2
SET_BIT (RCC->APB1ENR, RCC_APB1ENR_TIM2EN); // включаем тактовый генератор TIM2
WRITE_REG(TIM2->PSC, 8000-1); // предделитель
WRITE_REG(TIM2->ARR, 2000-1); // значение перезагрузки
SET_BIT (TIM2->CCER, TIM_CCER_CC1E); // разрешим вывод сигнала со схемы сравнения на вывод МК*/
SET_BIT (TIM2->CR2, TIM_CR2_MMS_1); // в качестве TRGO будет использоваться событие "Update"
SET_BIT (TIM2->CR1, TIM_CR1_CEN); // разрешим работу таймера

DMA2_init(); // инициализация DMA2
NVIC_EnableIRQ(ADC_IRQn); // разрешим прерывания ADC в NVIC
//delay_ms (1000);

SET_BIT (DMA2_Stream4->CR, DMA_SxCR_EN); // разрешим работу DMA
while (!(READ_BIT(DMA2->HISR, DMA_HISR_TCIF4))) //если флаг прерывания завершения передачи Stream4 не установлен
//while (!(READ_BIT(DMA2->LISR, DMA_LISR_TCIF0))) //если флаг прерывания завершения передачи Stream0 не установлен
	{
	if(is_OVR) // проверка перезаписи
		{
		UART3_PutRAMString ("ADC_SR_OVR\r\n"); // выводим строку по UART
		is_OVR = 0; // флаг: 1 - есть перезапись результата, 0 - нет
		}
	} // ожидаем установку флага окончания передачи
sprintf(msg,"ADC1_DMA2_test_programm_end\r\n");
UART3_PutRAMString (msg); // тест
for(byte i = 0; i < HalfWords_NUM; i++)
	{
	sprintf(msg,"i = %u, ", i);
	UART3_PutRAMString (msg); // <a>
	sprintf(msg,"HalfWords[%u] = %5.2f, ", i, (Vref*HalfWords[i]/4096));
	UART3_PutRAMString (msg); //тест
	i++;
	sprintf(msg,"HalfWords[%u] = %5.2f\r\n", i, (Vref*HalfWords[i]/4096));
	UART3_PutRAMString (msg); //тест
	}

while(1)
	{

	}
}

// обработка прерываний от ADC
void ADC_IRQHandler(void)
{
if(READ_BIT(ADC1->SR, ADC_SR_EOC)) // если сработало прерывание от ADC1 EOC
	{
//	ADC_DR_tmp[ADC_DR_tmp_ptr++] = ADC1->DR; // сохраним
	CLEAR_BIT (ADC1->SR, ADC_SR_EOC); // сбросим флаг прерывания
	}
if(READ_BIT (ADC1->SR, ADC_SR_OVR)) // если сработало прерывание от ADC1 OVR - перезапись значения
	{
	is_OVR = 1; // флаг: 1 - есть перезапись результата, 0 - нет
	CLEAR_BIT (ADC1->SR, ADC_SR_OVR); // сбросим флаг перывания
	}
}

