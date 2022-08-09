// Работа с АЦП. Фоторегулятор c ШИМ.
#define stm32f4xx
#include "stm32f4xx.h" // описание периферии
#include "stdio.h" // описание стандартных функций

// определения частот шин МК
#define BANDWITH 57600 //пропускная способность шины
#define APB1_FREQUENCY 16000000 // частота шины APB1

#include "DELAY_TIM7.h" // функции задержки
#include "UART_STM32.h" // объявления функций для работы с USART3
char msg[130]; // массив для формирования сообщений для вывода по UART

__I float Vref = 3.3; // опорное напряжение для АЦП; __I = volatile const
__IO float ADC_float = 0; // результат АЦП в виде напряжения; __IO = volatile

// пороги переключения фотореле, В
/*const float theshold_hi = 2.8; // верхний порог
const float theshold_lo = 1.6; // нижний*/

#define GPIO_AFRH_PIN13_AF2 0x200000 //

#define PERIOD 1000
//__IO unsigned char duty = PERIOD/2;

int main()
{
//настройка вывода 13 для световой сигнализации
SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIODEN); // запускаем тактовый генератор GPIOD
SET_BIT(GPIOD->MODER, GPIO_MODER_MODER13_1); //пин 13 на альтернативный выход
GPIOD->AFR[1] |= GPIO_AFRH_PIN13_AF2; //выбор альт. режима для пина 13 (AF2, TIM4_CH2)

// настройка порта С для АЦП
SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOCEN); // запускаем тактовый генератор GPIOC
SET_BIT(GPIOC->MODER, GPIO_MODER_MODER1_1 | GPIO_MODER_MODER1_0); // настраиваем вывод PC1 на аналоговый режим

// настройка АЦП
SET_BIT(RCC->APB2ENR, RCC_APB2ENR_ADC1EN); // включаем тактовый генератор
/*разряды SQx[4:0] задают номер канала, где х – это номер преобразования.*/
CLEAR_BIT(ADC1->SQR1, ADC_SQR1_L); //зададим количество каналов регулярной группы: 1
SET_BIT(ADC1->SQR3, ADC_SQR3_SQ1_0 | ADC_SQR3_SQ1_1| ADC_SQR3_SQ1_3); // канал 11 (PC1)
SET_BIT(ADC1->CR2, ADC_CR2_ADON); // включаем АЦП

//настройка Analog watchdog
/*ADC1->HTR = theshold_hi*4096/Vref; // верхний порог срабатывания
ADC1->LTR = theshold_lo*4096/Vref; // нижний порог срабатывания
SET_BIT(ADC1->CR1, ADC_CR1_AWDSGL); //AWD  enabled on a single channel
SET_BIT(ADC1->CR1, ADC_CR1_AWDCH_0 | ADC_CR1_AWDCH_1 | ADC_CR1_AWDCH_4); //сторожим 11 канал
SET_BIT(ADC1->CR1, ADC_CR1_AWDEN); // AWD enable on regular channels
SET_BIT(ADC1->CR1, ADC_CR1_AWDIE);  //Analog watchdog interrupt enabled
NVIC_EnableIRQ(WWDG_IRQn); //включим прерывание от AWD от контроллера прерываний*/

//настройка таймера4 для ШИМ
SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM4EN); //включим тактовый генератор таймера4
TIM4->PSC = 160-1; // предделитель TIM4
TIM4->ARR = PERIOD; // значение перезагрузки TIM4
SET_BIT(TIM4->CCMR1, TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1); //запуск режима 1 ШИМ
SET_BIT(TIM4->CCER, TIM_CCER_CC2E);   //активируем канал 2 TIM4
TIM4->CCR2 = 0; //длительность ШИМ
SET_BIT(TIM4->CR1, TIM_CR1_CEN); //включим таймер4

//настройка UART
UART3_init(APB1_FREQUENCY, BANDWITH); // инициализация UART3
//UART3_init(16000000, 57600);
UART3_PutRAMString("ADC fotoregulator with PWM:\r\n");

__enable_irq(); // разрешим прерывания глобально

while(1)
	{
	ADC1->CR2 |= ADC_CR2_SWSTART; // программный запуск АЦП, после каждого преобразования необходимо запускать повторно
	while(!(ADC1->SR & ADC_SR_EOC)) {}; // регистр ADC_SR - статусный регистр, проверяем флаг, сигнализирующий об окончании преобразования
	/* опорное напряжение*результат преобразования/максимальное число дискретов*/
	ADC_float = Vref*ADC1->DR/4096;
	TIM4->CCR2 = ADC1->DR/4.096; // новая длительность ШИМ
	sprintf(msg,("ADC_result 11 = %f V\r\n"), ADC_float); // выводим значение АЦП в строку
	UART3_PutRAMString(msg); // выводим строку по UART
	delay_ms(500); // задержка 1 с
	}
}

