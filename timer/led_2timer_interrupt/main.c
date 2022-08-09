#include "stm32f4xx.h"

#define LED_off 0 // ��������
#define LED_on 1 // �������
#define LED_blink 2 // ������
//#define PAUSE 0x4FFFFF
unsigned char LED_status;

void TIM4_IRQHandler(void)
{
	TIM4->SR &= ~TIM_SR_UIF; // ������� ���� ����������
	if(LED_status == LED_off)
		GPIOD->ODR &= ~GPIO_ODR_ODR_12; // ��������� ���������
	else
	{
		if(LED_status == LED_on)
			GPIOD->ODR |= GPIO_ODR_ODR_12; // �������� ���������
		else
		{
			if(LED_status == LED_blink)
				GPIOD->ODR ^= GPIO_ODR_ODR_12; // ����������� ���������. ^ - ����������� ���  (0 ���� 0^0 � 1^1. 1 ���� 1^0 � 0^1)
		}
	}
}

int main(void)
{

RCC->APB1ENR |= RCC_APB1ENR_TIM4EN | RCC_APB1ENR_TIM3EN; //�������� ������ 4 � 3
TIM4->PSC = 16000-1; // ������������ ������� 4
TIM3->PSC = 64000-1; // ������������ ������� 3
TIM4->ARR = 500; // �������� ������������ ������4
TIM3->ARR = 4800; // �������� ������������ ������3
TIM4->DIER |= TIM_DIER_UIE;  //�������� ���������� ��� ���������� (������������) ������� 4
TIM4->CR1 |= TIM_CR1_CEN; // �������� ������ ������� 4
TIM3->CR1 |= TIM_CR1_CEN;
NVIC_EnableIRQ(TIM4_IRQn); //��������� ���������� � ����������� ����������
__enable_irq(); // �������� ���������� ���������

RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; //������� ���� IO D
GPIOD->MODER |= GPIO_MODER_MODER12_0; //������������� ��� 12 �� ������� �����
GPIOD->OTYPER = 0x0;  //����� � ��������� (push-pull)
GPIOD->OSPEEDR = 0x0;  //�������� ����� ����� ������

    while(1)
    {
    	for (unsigned short i = 0; i <= TIM3->ARR; i++)
    	{
    	if (TIM3->CNT == 0)
    		LED_status = LED_on;
    	if (TIM3->CNT == TIM3->ARR/4)
    		LED_status = LED_off;
    	if (TIM3->CNT == TIM3->ARR/2)
    		LED_status = LED_blink;
    	if (TIM3->CNT/3 == TIM3->ARR/4)
    		LED_status = LED_off;
    	}
 /*   	unsigned long i = 0;
    	LED_status = LED_on;
    	for(i = 0; i < PAUSE; i++) {} // ��������
    	LED_status = LED_off;
    	for(i = 0; i < PAUSE; i++) {} // ��������
    	LED_status = LED_blink;
    	for(i = 0; i < PAUSE; i++) {} // ��������
    	LED_status = LED_off;
    	for(i = 0; i < PAUSE; i++) {} // ��������*/
    }
}
