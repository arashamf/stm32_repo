/* Пример 8. Функции задержки с использованием таймера*/

#define stm32f4xx
#include "stm32f4xx.h" // описание периферии
#include "DELAY_TIM7.h" // функции задержки

int main()
{
RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; // включаем тактовый генератор порта D
GPIOD->MODER |= GPIO_MODER_MODER13_0; //пин 13 на выход
GPIOD->OTYPER = 0x0;  //выход с подтяжкой (push-pull)
GPIOD->OSPEEDR = 0x0;  //скорость порта D самая низкая

while(1)
	{
	GPIOD->ODR |= GPIO_ODR_ODR_13; // включаем
	delay_ms(1000); // задержка
	GPIOD->ODR &= ~GPIO_ODR_ODR_13; // выключаем
	delay_ms(500); // задержка
	}
}


