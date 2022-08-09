#include "stm32f4xx.h"

unsigned char LED_status;

void TIM4_IRQHandler(void)
{
	TIM4->SR &= ~TIM_SR_UIF; // ������� ���� ���������� �� ������������ ������� 4
	if (GPIOD->ODR == GPIO_ODR_ODR_15) //���� �� 15 ���  "1", ��
		GPIOD->ODR ^= GPIO_ODR_ODR_15 ^ GPIO_ODR_ODR_12; //� ������� ������������ ��� ������� 15 ��� � ������� "1" �� 12 ���
	else
		GPIOD->ODR = GPIOD->ODR << 1; //���������� led ����������� �������
	}

int main(void)
{

RCC->APB1ENR |= RCC_APB1ENR_TIM4EN | RCC_APB1ENR_TIM3EN; //�������� ������ 4 � 3
TIM4->PSC = 16000-1; // ������������ ������� 4
TIM4->ARR = 600; // �������� ������������ ������4
TIM4->DIER |= TIM_DIER_UIE;  //�������� ���������� ��� ���������� (������������) ������� 4
TIM4->CR1 |= TIM_CR1_CEN; // �������� ������ ������� 4
NVIC_EnableIRQ(TIM4_IRQn); //��������� ���������� � ����������� ����������
__enable_irq(); // �������� ���������� ���������

RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; //������� ���� IO D
GPIOD->MODER |= GPIO_MODER_MODER12_0 | GPIO_MODER_MODER13_0 | GPIO_MODER_MODER14_0 | GPIO_MODER_MODER15_0; //������������� ��� 12,13,14,15 �� ������� �����
GPIOD->OTYPER = 0x0;  //����� � ��������� (push-pull)
GPIOD->OSPEEDR = 0x0;  //�������� ����� ����� ������
GPIOD->ODR |= GPIO_ODR_ODR_12; //������� "1" �� ��� 12

    while(1)
    {

    }
}
