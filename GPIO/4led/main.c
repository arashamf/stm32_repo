#include "stm32f4xx.h"
int main(void)
{

RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; //тактируем порт IO D, стр.265 reference manual
//RCC->AHB1ENR |= 0x8; //можно попробывать и так
GPIOD->MODER |= GPIO_MODER_MODER12_0; //порт D 12 пин на обычный выход
GPIOD->MODER |= GPIO_MODER_MODER13_0; //порт D 13 пин на обычный выход
GPIOD->MODER |= GPIO_MODER_MODER14_0; //порт D 14 пин на обычный выход
GPIOD->OTYPER = 0x0;  //выход с подтяжкой (push-pull)
GPIOD->OSPEEDR = 0x0;  //скорость порта самая низкая

    while(1)
    {
    	GPIOD->ODR &= ~GPIO_ODR_ODR_14; //'0' на выход, гасим 14 светодиод
    	GPIOD->ODR |= GPIO_ODR_ODR_12;  //'1' на выход, зажигаем 12 светодиод
    	for(unsigned int i = 0; i < 0xFFFFF; i++) {}
    	GPIOD->ODR &= ~GPIO_ODR_ODR_12;
    	GPIOD->ODR |= GPIO_ODR_ODR_13;
    	for(unsigned int i = 0; i < 0xFFFFF; i++) {}
    	GPIOD->ODR &= ~GPIO_ODR_ODR_13;
    	GPIOD->ODR |= GPIO_ODR_ODR_14;
    	for(unsigned int i = 0; i < 0xFFFFF; i++) {}
    }
}
