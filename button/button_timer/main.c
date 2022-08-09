//программа опроса кнопки по прерыванию таймера
#define stm32f4xx
#include "stm32f4xx.h" // описание периферии
// для обработки дребезга контактов кнопки
#define KEY_Bounce_Time 400 // время дребезга + автоповтора (в мс)
static volatile unsigned int KEY_Bounce_Time_Cnt; // счетчик времени дребезга
static volatile unsigned char KEY_Press_Flag = 0; // флаг нажатия на кнопку: 0 - не нажата; 1 - нажата

// обработка прерываний от TIM4
void TIM4_IRQHandler(void)
{
TIM4->SR &= ~TIM_SR_UIF; // сбросим флаг прерывания
// проверяем, можно ли опрашивать кнопку
if(KEY_Bounce_Time_Cnt) //если  нельзя
	KEY_Bounce_Time_Cnt--; // уменьшаем счетчик
else // если можно
	{
	if(!(GPIOA->IDR & GPIO_IDR_IDR_0)) // если кнопка нажата (на входе логический 0)
		{
		KEY_Bounce_Time_Cnt = KEY_Bounce_Time; // счетчик времени дребезга - запустим
		KEY_Press_Flag = 1; // флаг нажатия на кнопку - установим
		}
	}
}

int main()
{
// порт для кнопки
RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; // запускаем тактовый генератор GPIOA
GPIOA->MODER &= ~GPIO_MODER_MODER0; // настраиваем вывод 0 на вход, при сбросе порт настроен на вход
GPIOA->PUPDR |= GPIO_PUPDR_PUPDR0_0; // внутренния подтяжка вверх

// порт выход на светодиод
RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; // запускаем тактовый генератор GPIOD
GPIOD->MODER |= GPIO_MODER_MODER12_0; // настраиваем вывод 12 на выход
//GPIOD->OTYPER - // при сбросе стоит Push-Pull

// настройка таймера TIM4 на 1 мс
// 16000000/16/1000 = 1000Гц (1мс)
RCC->APB1ENR |= RCC_APB1ENR_TIM4EN; // запускаем тактовый генератор TIM4
TIM4->PSC = 16-1;
TIM4->ARR = 1000;
TIM4->DIER |= TIM_DIER_UIE; // разрешим прерывание при перезагрузке таймера
TIM4->CR1 |= TIM_CR1_CEN; // разрешим счет таймера
// настройка NVIC для TIM4
NVIC_EnableIRQ(TIM4_IRQn);
__enable_irq(); // разрешим прерывания глобально

while(1)
	{
	// проверяем флаг нажатия кнопки
	if(KEY_Press_Flag)
		{
		// переключаем светодиод
		if(GPIOD->ODR & GPIO_ODR_ODR_12)
			GPIOD->BSRRH |= GPIO_BSRR_BS_12;  //выключаем вывод 12 
		else
			GPIOD->BSRRL |= GPIO_BSRR_BS_12; //включаем вывод 12
		KEY_Press_Flag = 0; // флаг нажатия на кнопку - сбросим
		}
	}
}


