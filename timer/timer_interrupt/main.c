#include "stm32f4xx.h"

unsigned char LED_status;

void TIM4_IRQHandler(void)
{
	TIM4->SR &= ~TIM_SR_UIF; // сбросим флаг прерывания по переполнения таймера 4
	if (GPIOD->ODR == GPIO_ODR_ODR_15) //если на 15 пин  "1", то
		GPIOD->ODR ^= GPIO_ODR_ODR_15 ^ GPIO_ODR_ODR_12; //с помощью исключающего или обнулим 15 пин и подадим "1" на 12 пин
	else
		GPIOD->ODR = GPIOD->ODR << 1; //переключим led поразрядным сдвигом
	}

int main(void)
{

RCC->APB1ENR |= RCC_APB1ENR_TIM4EN | RCC_APB1ENR_TIM3EN; //включаем таймер 4 и 3
TIM4->PSC = 16000-1; // предделитель таймера 4
TIM4->ARR = 600; // значение перезагрузки таймер4
TIM4->DIER |= TIM_DIER_UIE;  //разрешим прерывание при обновлении (переполнении) таймера 4
TIM4->CR1 |= TIM_CR1_CEN; // разрешим работу таймера 4
NVIC_EnableIRQ(TIM4_IRQn); //разрешаем прерывание в контроллере прерываний
__enable_irq(); // разрешим прерывания глобально

RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; //включим порт IO D
GPIOD->MODER |= GPIO_MODER_MODER12_0 | GPIO_MODER_MODER13_0 | GPIO_MODER_MODER14_0 | GPIO_MODER_MODER15_0; //сконфигурирум пин 12,13,14,15 на обычный выход
GPIOD->OTYPER = 0x0;  //выход с подтяжкой (push-pull)
GPIOD->OSPEEDR = 0x0;  //скорость порта самая низкая
GPIOD->ODR |= GPIO_ODR_ODR_12; //подадим "1" на пин 12

    while(1)
    {

    }
}
