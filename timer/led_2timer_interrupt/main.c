#include "stm32f4xx.h"

#define LED_off 0 // выключен
#define LED_on 1 // включен
#define LED_blink 2 // мигает
//#define PAUSE 0x4FFFFF
unsigned char LED_status;

void TIM4_IRQHandler(void)
{
	TIM4->SR &= ~TIM_SR_UIF; // сбросим флаг прерывания
	if(LED_status == LED_off)
		GPIOD->ODR &= ~GPIO_ODR_ODR_12; // выключаем светодиод
	else
	{
		if(LED_status == LED_on)
			GPIOD->ODR |= GPIO_ODR_ODR_12; // включаем светодиод
		else
		{
			if(LED_status == LED_blink)
				GPIOD->ODR ^= GPIO_ODR_ODR_12; // переключаем светодиод. ^ - исключающее или  (0 если 0^0 и 1^1. 1 если 1^0 и 0^1)
		}
	}
}

int main(void)
{

RCC->APB1ENR |= RCC_APB1ENR_TIM4EN | RCC_APB1ENR_TIM3EN; //включаем таймер 4 и 3
TIM4->PSC = 16000-1; // предделитель таймера 4
TIM3->PSC = 64000-1; // предделитель таймера 3
TIM4->ARR = 500; // значение перезагрузки таймер4
TIM3->ARR = 4800; // значение перезагрузки таймер3
TIM4->DIER |= TIM_DIER_UIE;  //разрешим прерывание при обновлении (переполнении) таймера 4
TIM4->CR1 |= TIM_CR1_CEN; // разрешим работу таймера 4
TIM3->CR1 |= TIM_CR1_CEN;
NVIC_EnableIRQ(TIM4_IRQn); //разрешаем прерывание в контроллере прерываний
__enable_irq(); // разрешим прерывания глобально

RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; //включим порт IO D
GPIOD->MODER |= GPIO_MODER_MODER12_0; //сконфигурирум пин 12 на обычный выход
GPIOD->OTYPER = 0x0;  //выход с подтяжкой (push-pull)
GPIOD->OSPEEDR = 0x0;  //скорость порта самая низкая

    while(1)
    {
    	for (unsigned short i = 0; i <= TIM3->ARR; i++)
    	{
    	if (TIM3->CNT == 0)
    		LED_status = LED_on;
    	if (TIM3->CNT == TIM3->ARR/4)
    		LED_status = LED_off;
    	if (TIM3->CNT == TIM3->ARR/2)
    		LED_status = LED_blink;
    	if (TIM3->CNT/3 == TIM3->ARR/4)
    		LED_status = LED_off;
    	}
 /*   	unsigned long i = 0;
    	LED_status = LED_on;
    	for(i = 0; i < PAUSE; i++) {} // задержка
    	LED_status = LED_off;
    	for(i = 0; i < PAUSE; i++) {} // задержка
    	LED_status = LED_blink;
    	for(i = 0; i < PAUSE; i++) {} // задержка
    	LED_status = LED_off;
    	for(i = 0; i < PAUSE; i++) {} // задержка*/
    }
}
