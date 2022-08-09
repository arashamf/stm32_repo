#include "stm32f4xx.h"
int main(void)
{

//RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; //��������� ���� IO D, ���.265  reference manual
RCC->AHB1ENR |= 0x8; //����� ����������� � ���
//GPIOD->MODER |= GPIO_MODER_MODER13_0; //���� D 13 ��� �� ������� �����
GPIOD->MODER = 0x4000000; //0b00000100000000000000000000000000
GPIOD->OTYPER = 0x0;  //����� � ��������� (push-pull)
GPIOD->OSPEEDR = 0x0;  //�������� ����� ����� ������

    while(1)
    {
    	GPIOD->ODR |= GPIO_ODR_ODR_13;  //'1' �� �����, �������� ���������
    	for(unsigned int i = 0; i < 0xFFFFF; i++) {}
    	GPIOD->ODR &= ~GPIO_ODR_ODR_13; //'0' �� �����, ����� ���������
    	for(unsigned int i = 0; i < 0xFFFFF; i++) {}
    }
}
