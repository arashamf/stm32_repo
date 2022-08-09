#include "stm32f4xx.h"
int main(void)
{

//RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; //тактируем порт IO D, стр.265  reference manual
RCC->AHB1ENR |= 0x8; //можно попробывать и так
//GPIOD->MODER |= GPIO_MODER_MODER13_0; //порт D 13 пин на обычный выход
GPIOD->MODER = 0x4000000; //0b00000100000000000000000000000000
GPIOD->OTYPER = 0x0;  //выход с подтяжкой (push-pull)
GPIOD->OSPEEDR = 0x0;  //скорость порта самая низкая

    while(1)
    {
    	GPIOD->ODR |= GPIO_ODR_ODR_13;  //'1' на выход, зажигаем светодиод
    	for(unsigned int i = 0; i < 0xFFFFF; i++) {}
    	GPIOD->ODR &= ~GPIO_ODR_ODR_13; //'0' на выход, гасим светодиод
    	for(unsigned int i = 0; i < 0xFFFFF; i++) {}
    }
}
