#include "stm32f4xx.h"

unsigned char LED_status;

void TIM4_IRQHandler(void)
{
	TIM4->SR &= ~TIM_SR_UIF; // сбросим флаг прерывания
	GPIOD->ODR ^= GPIO_ODR_ODR_12 | GPIO_ODR_ODR_13 | GPIO_ODR_ODR_14 | GPIO_ODR_ODR_15; //переключим led
	//^ - исключающее или  (0 если 0^0 и 1^1; 1 если 1^0 и 0^1)
}

int main(void)
{

RCC->APB1ENR |= RCC_APB1ENR_TIM4EN; //включаем таймер 4 и 3
TIM4->PSC = 16000-1; // предделитель таймера 4; время одного тика таймера 1/(32000000/16000)=0,0005c
TIM4->ARR = 1000; // значение перезагрузки таймер4; 1000x0,0005=0,5c
TIM4->DIER |= TIM_DIER_UIE;  //разрешим прерывание при обновлении (переполнении) таймера 4
TIM4->CR1 |= TIM_CR1_CEN; // разрешим работу таймера 4
NVIC_EnableIRQ(TIM4_IRQn); //разрешаем прерывание в контроллере прерываний
__enable_irq(); // разрешим прерывания глобально

RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; //включим порт IO D
GPIOD->MODER |= GPIO_MODER_MODER12_0 | GPIO_MODER_MODER13_0 | GPIO_MODER_MODER14_0 | GPIO_MODER_MODER15_0; //сконфигурирум пин 12,13,14,15 на обычный выход
GPIOD->OTYPER = 0x0;  //выход с подтяжкой (push-pull)
GPIOD->OSPEEDR = 0x0;  //скорость порта самая низкая
GPIOD->BSRRL |= GPIO_BSRR_BS_12 | GPIO_BSRR_BS_13; //подадим "1" на пины 12, 13

    while(1)
    {

    }
}
