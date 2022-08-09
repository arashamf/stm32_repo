#include "stm32f4xx.h"

unsigned char LED_status;

void TIM4_IRQHandler(void)
{
	TIM4->SR &= ~TIM_SR_UIF; // ������� ���� ����������
	GPIOD->ODR ^= GPIO_ODR_ODR_12 | GPIO_ODR_ODR_13 | GPIO_ODR_ODR_14 | GPIO_ODR_ODR_15; //���������� led
	//^ - ����������� ���  (0 ���� 0^0 � 1^1; 1 ���� 1^0 � 0^1)
}

int main(void)
{

RCC->APB1ENR |= RCC_APB1ENR_TIM4EN; //�������� ������ 4 � 3
TIM4->PSC = 16000-1; // ������������ ������� 4; ����� ������ ���� ������� 1/(32000000/16000)=0,0005c
TIM4->ARR = 1000; // �������� ������������ ������4; 1000x0,0005=0,5c
TIM4->DIER |= TIM_DIER_UIE;  //�������� ���������� ��� ���������� (������������) ������� 4
TIM4->CR1 |= TIM_CR1_CEN; // �������� ������ ������� 4
NVIC_EnableIRQ(TIM4_IRQn); //��������� ���������� � ����������� ����������
__enable_irq(); // �������� ���������� ���������

RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; //������� ���� IO D
GPIOD->MODER |= GPIO_MODER_MODER12_0 | GPIO_MODER_MODER13_0 | GPIO_MODER_MODER14_0 | GPIO_MODER_MODER15_0; //������������� ��� 12,13,14,15 �� ������� �����
GPIOD->OTYPER = 0x0;  //����� � ��������� (push-pull)
GPIOD->OSPEEDR = 0x0;  //�������� ����� ����� ������
GPIOD->BSRRL |= GPIO_BSRR_BS_12 | GPIO_BSRR_BS_13; //������� "1" �� ���� 12, 13

    while(1)
    {

    }
}
