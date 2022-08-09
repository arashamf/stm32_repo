#include "stm32f4xx.h"

int main(void)
{
RCC->APB1ENR |= RCC_APB1ENR_TIM4EN; //включаем таймер 4
TIM4->PSC = 16000-1; // установка предделителя таймера4
TIM4->ARR = 1200; // значение перезагрузки таймера4
TIM4->CR1 |= TIM_CR1_CEN; // разрешим работу таймера
RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; //тактируем порт IO D
GPIOD->MODER |= GPIO_MODER_MODER12_0 | GPIO_MODER_MODER13_0 | GPIO_MODER_MODER14_0 | GPIO_MODER_MODER15_0; //порт D 12, 13, 14, 15 пин на обычный выход
GPIOD->OTYPER = 0x0;  //выход с подтяжкой (push-pull)
GPIOD->OSPEEDR = 0x0;  //скорость порта D самая низкая

    while(1)
    {
    	for (unsigned short i = 0; i <= TIM4->ARR; i++)
    	{
    		if (TIM4->CNT == 0) //если значение в регистре счётчика таймера4 равно о, то:
    			{
    		    GPIOD->BSRRH |= GPIO_BSRR_BS_15; //выключаем 15 пин
				//BSRRH - старшая половина регистра BSRR, импользуется для сброса бита в 0
    		    GPIOD->BSRRL |= GPIO_BSRR_BS_12;  //включаем 12 пин
				//BSRRL - младшая половина регистра BSRR, используется для установки бита в 1
    		    }
    		if (TIM4->CNT == TIM4->ARR/4)
    			{
    		    GPIOD->BSRRH |= GPIO_BSRR_BS_12; //выключаем 12 пин
    		    GPIOD->BSRRL |= GPIO_BSRR_BS_13;  //включаем 13 пин
    		    }
    		if (TIM4->CNT == TIM4->ARR/2)
    		    {
    		    GPIOD->BSRRH |= GPIO_BSRR_BS_13; //выключаем 13 пин
    		    GPIOD->BSRRL |= GPIO_BSRR_BS_14;  //включаем 14 пин
    		    }
    		if (TIM4->CNT/3 == TIM4->ARR/4)
    			{
    		    GPIOD->BSRRH |= GPIO_BSRR_BS_14; //выключаем 14 пин
    		    GPIOD->BSRRL |= GPIO_BSRR_BS_15;  //включаем 15 пин
    		    }
    	}
    }
}
