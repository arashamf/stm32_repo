#include "stm32f4xx.h"
int main(void)
{

RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; //��������� ���� IO D, ���.265 reference manual
//RCC->AHB1ENR |= 0x8; //����� ����������� � ���
GPIOD->MODER |= GPIO_MODER_MODER12_0; //���� D 12 ��� �� ������� �����
GPIOD->MODER |= GPIO_MODER_MODER13_0; //���� D 13 ��� �� ������� �����
GPIOD->MODER |= GPIO_MODER_MODER14_0; //���� D 14 ��� �� ������� �����
GPIOD->OTYPER = 0x0;  //����� � ��������� (push-pull)
GPIOD->OSPEEDR = 0x0;  //�������� ����� ����� ������

    while(1)
    {
    	GPIOD->ODR &= ~GPIO_ODR_ODR_14; //'0' �� �����, ����� 14 ���������
    	GPIOD->ODR |= GPIO_ODR_ODR_12;  //'1' �� �����, �������� 12 ���������
    	for(unsigned int i = 0; i < 0xFFFFF; i++) {}
    	GPIOD->ODR &= ~GPIO_ODR_ODR_12;
    	GPIOD->ODR |= GPIO_ODR_ODR_13;
    	for(unsigned int i = 0; i < 0xFFFFF; i++) {}
    	GPIOD->ODR &= ~GPIO_ODR_ODR_13;
    	GPIOD->ODR |= GPIO_ODR_ODR_14;
    	for(unsigned int i = 0; i < 0xFFFFF; i++) {}
    }
}
