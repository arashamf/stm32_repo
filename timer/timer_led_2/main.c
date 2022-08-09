#include "stm32f4xx.h"

int main(void)
{
RCC->APB1ENR |= RCC_APB1ENR_TIM4EN; //�������� ������ 4
TIM4->PSC = 16000-1; // ��������� ������������ �������4
TIM4->ARR = 1400; // �������� ������������
TIM4->CR1 |= TIM_CR1_CEN; // �������� ������ �������
RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; //��������� ���� IO D
//GPIOD->MODER |= GPIO_MODER_MODER12_0 | GPIO_MODER_MODER13_0 | GPIO_MODER_MODER14_0 | GPIO_MODER_MODER15_0; //���� D 12, 13, 14, 15 ��� �� ������� �����
GPIOD->MODER = 0x55000000; //0b01010101000000000000000000000000
GPIOD->OTYPER = 0x0;  //����� � ��������� (push-pull)
GPIOD->OSPEEDR = 0x0;  //�������� ����� ����� ������

    while(1)
    {
    	if (TIM4->CNT < TIM4->ARR/2) //������, ���� �������� � �������� TIM4_CNT ������, ��� �������� �������� � �������� TIM4_ARR
    		{
    		GPIOD->ODR &= ~GPIO_ODR_ODR_14 & ~GPIO_ODR_ODR_15; //��������� 14, 15 ���
    		GPIOD->ODR |= GPIO_ODR_ODR_12 | GPIO_ODR_ODR_13;  //�������� 12, 13 ���
    		}
    		else
    		{
    		GPIOD->ODR &= ~GPIO_ODR_ODR_12 & ~GPIO_ODR_ODR_13; //��������� 12, 13 ���
    		GPIOD->ODR |= GPIO_ODR_ODR_14 | GPIO_ODR_ODR_15;  //�������� 14, 15 ���
    		}
    }
}
