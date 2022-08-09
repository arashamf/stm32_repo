// Работа с таймерами. Бипер. Переменная частота, разная длительность сигнала.
//
#define stm32f4xx
#include "stm32f4xx.h" // описание периферии
#include "DELAY_TIM7.h" // функции задержки
#define FREQ_TIM3 100000
volatile unsigned int beep_time_cnt; // счетчик времени звучания бипера, мс
unsigned int beep_freq; // частота звучания бипера, Гц

int main()
{
//настройка порта PD для управления светодиодами
RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; //тактируем порт IO D
GPIOD->MODER |= GPIO_MODER_MODER12_0 | GPIO_MODER_MODER13_0 | GPIO_MODER_MODER14_0; //порт D 12, 13, 14 пин на обычный выход
GPIOD->OTYPER = 0x0;  //выход с подтяжкой (push-pull)
GPIOD->OSPEEDR = 0x0;  //скорость порта D самая низкая

//настройка порта PE11 для управления бипером
RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN; // включаем тактовый генератор порта E
GPIOE->MODER |= GPIO_MODER_MODER11_0; // настроим на выход порт PE11

// настройка таймера TIM4 длительности звучания бипера
RCC->APB1ENR |= RCC_APB1ENR_TIM4EN; // подадим тактирование на TIM4
// срабатывание прерывания TIM4 через 1 мсек (1 кГц)
//тактовая частота=16000000; 1000=16000000/(предделитель)*(значение сравнения)=>(предделитель)*(значение сравнения)=16000
TIM4->PSC = 160 - 1; // значение предделителя (на вход TIM4 16000/160=100кГц)
TIM4->ARR = 100; // значение сравнения (прерывание TIM4 раз в 1 мс)
TIM4->DIER |= TIM_DIER_UIE; // разрешим прерывание при обновлении таймера TIM4
TIM4->CR1 |= TIM_CR1_CEN; // разрешим работу таймера TIM4
NVIC_EnableIRQ(TIM4_IRQn); // разрешаем прерывание в контроллере NVIC

// настройка таймера TIM3 генерации звука бипера
RCC->APB1ENR |= RCC_APB1ENR_TIM3EN; // подали тактирование на TIM3
TIM3->PSC = 160 - 1; // значение предделителя (на вход TIM3 16000/160=100кГц)
TIM3->ARR = 0; // прерывание TIM3 раз в 0,01 мс
 TIM3->DIER |= TIM_DIER_UIE; // разрешим прерывание при обновлении таймера TIM3
TIM3->CR1 |= TIM_CR1_CEN; // разрешим работу таймера TIM3
NVIC_EnableIRQ(TIM3_IRQn); // разрешаем прерывание в контроллере NVIC

while(1)
	{
	beep_freq = 2500; // частота звучания, Гц
	TIM3->ARR = (float) FREQ_TIM3 / (beep_freq*2); // значение сравнения (частота на входе таймера/(требуемую частоту генерации*2))
	//срабатывание прерывания на TIM3: 1/(100000/TIM3->ARR) = 1/(100000/20)=1/5000=0,0002с=0,2мс
	//частота генерируемого сигнала равна: 1/(время прерывания*2)
	beep_time_cnt = 750; // длительность в мс
	GPIOD->BSRRH |= GPIO_BSRR_BS_14; //выключаем 14 пин,
	GPIOD->BSRRL |= GPIO_BSRR_BS_12;  //включаем 12 пин
	delay_ms(1000);
	beep_freq = 400; // частота звучания, Гц
	TIM3->ARR = (float) FREQ_TIM3 /(beep_freq*2); // значение сравнения TIM3
	beep_time_cnt = 500;
    GPIOD->BSRRH |= GPIO_BSRR_BS_12; //выключаем 12 пин
    GPIOD->BSRRL |= GPIO_BSRR_BS_13;  //включаем 13 пин
	delay_ms(750);
	beep_freq = 1000; // частота звучания, Гц
	TIM3->ARR = (float) FREQ_TIM3 /(beep_freq*2); // значение сравнения TIM3
	beep_time_cnt = 1000;
	GPIOD->BSRRH |= GPIO_BSRR_BS_13; //выключаем 13 пин
	GPIOD->BSRRL |= GPIO_BSRR_BS_14;  //включаем 14 пин
	delay_ms(1250);
	}
}
// функция обработки прерывания TIM4
void TIM4_IRQHandler(void)
{
TIM4->SR &= ~TIM_SR_UIF; // сбросим флаг прерывания
if(beep_time_cnt) // проверим счетчик времени
	{
	beep_time_cnt--; // уменьшаем счетчик времени
	}
}

// функция обработки прерывания TIM3
void TIM3_IRQHandler(void)
{
TIM3->SR &= ~TIM_SR_UIF; // сбросим флаг прерывания
// бипер
if(beep_time_cnt) // проверим счетчик времени
	{
	GPIOE->ODR ^= GPIO_ODR_ODR_11; // переключаем порт для бипера
	}
else
	{
	GPIOE->ODR &= ~GPIO_ODR_ODR_11; // выключим порт для бипера
	}
}
