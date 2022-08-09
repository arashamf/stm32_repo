#include "stm32f4xx.h"

int main(void)
{
RCC->APB1ENR |= RCC_APB1ENR_TIM4EN; //�������� ������ 4
TIM4->PSC = 16000-1; // ��������� ������������ �������4
TIM4->ARR = 1200; // �������� ������������ �������4
TIM4->CR1 |= TIM_CR1_CEN; // �������� ������ �������
RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; //��������� ���� IO D
GPIOD->MODER |= GPIO_MODER_MODER12_0 | GPIO_MODER_MODER13_0 | GPIO_MODER_MODER14_0 | GPIO_MODER_MODER15_0; //���� D 12, 13, 14, 15 ��� �� ������� �����
GPIOD->OTYPER = 0x0;  //����� � ��������� (push-pull)
GPIOD->OSPEEDR = 0x0;  //�������� ����� D ����� ������

    while(1)
    {
    	for (unsigned short i = 0; i <= TIM4->ARR; i++)
    	{
    		if (TIM4->CNT == 0) //���� �������� � �������� �������� �������4 ����� �, ��:
    			{
    		    GPIOD->BSRRH |= GPIO_BSRR_BS_15; //��������� 15 ���
				//BSRRH - ������� �������� �������� BSRR, ������������ ��� ������ ���� � 0
    		    GPIOD->BSRRL |= GPIO_BSRR_BS_12;  //�������� 12 ���
				//BSRRL - ������� �������� �������� BSRR, ������������ ��� ��������� ���� � 1
    		    }
    		if (TIM4->CNT == TIM4->ARR/4)
    			{
    		    GPIOD->BSRRH |= GPIO_BSRR_BS_12; //��������� 12 ���
    		    GPIOD->BSRRL |= GPIO_BSRR_BS_13;  //�������� 13 ���
    		    }
    		if (TIM4->CNT == TIM4->ARR/2)
    		    {
    		    GPIOD->BSRRH |= GPIO_BSRR_BS_13; //��������� 13 ���
    		    GPIOD->BSRRL |= GPIO_BSRR_BS_14;  //�������� 14 ���
    		    }
    		if (TIM4->CNT/3 == TIM4->ARR/4)
    			{
    		    GPIOD->BSRRH |= GPIO_BSRR_BS_14; //��������� 14 ���
    		    GPIOD->BSRRL |= GPIO_BSRR_BS_15;  //�������� 15 ���
    		    }
    	}
    }
}
