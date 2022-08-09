#include "stm32f4xx.h"

int main(void)
{
RCC->APB1ENR |= RCC_APB1ENR_TIM4EN; //включаем таймер 4
TIM4->PSC = 16000-1; // установка предделителя таймера4
TIM4->ARR = 1400; // значение перезагрузки
TIM4->CR1 |= TIM_CR1_CEN; // разрешим работу таймера
RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; //тактируем порт IO D
//GPIOD->MODER |= GPIO_MODER_MODER12_0 | GPIO_MODER_MODER13_0 | GPIO_MODER_MODER14_0 | GPIO_MODER_MODER15_0; //порт D 12, 13, 14, 15 пин на обычный выход
GPIOD->MODER = 0x55000000; //0b01010101000000000000000000000000
GPIOD->OTYPER = 0x0;  //выход с подтяжкой (push-pull)
GPIOD->OSPEEDR = 0x0;  //скорость порта самая низкая

    while(1)
    {
    	if (TIM4->CNT < TIM4->ARR/2) //истина, если значение в регистре TIM4_CNT меньше, чем половина значения в регистре TIM4_ARR
    		{
    		GPIOD->ODR &= ~GPIO_ODR_ODR_14 & ~GPIO_ODR_ODR_15; //выключаем 14, 15 пин
    		GPIOD->ODR |= GPIO_ODR_ODR_12 | GPIO_ODR_ODR_13;  //включаем 12, 13 пин
    		}
    		else
    		{
    		GPIOD->ODR &= ~GPIO_ODR_ODR_12 & ~GPIO_ODR_ODR_13; //выключаем 12, 13 пин
    		GPIOD->ODR |= GPIO_ODR_ODR_14 | GPIO_ODR_ODR_15;  //включаем 14, 15 пин
    		}
    }
}
