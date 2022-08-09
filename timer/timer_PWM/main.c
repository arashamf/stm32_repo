#include <stm32f4xx.h>
#define GPIO_AFRH_PIN12_AF2 0x00020000
#define GPIO_AFRH_PIN13_AF2 0x00200000
#define PAUSE 32000
#define PERIOD 100 //������ ��������� ���

unsigned int duty_ch1 = PERIOD/100; // ������������ ��������� ��� ������ 1 ����12
unsigned int duty_ch2 = PERIOD/50; //������������ ��������� ��� ������ 2 ����13

void TIM3_IRQHandler(void)
{
	TIM3->SR &= ~TIM_SR_UIF; // ������� ���� ����������
	duty_ch1 += PERIOD/100; // ���������� ������������ �� 1 % �� �������
	duty_ch2 += PERIOD/50; // ���������� ������������ �� 2 % �� �������
	if(duty_ch1 > PERIOD)
		duty_ch1 = PERIOD/100; // ������������ �������� ��������� - �� 0 �� 100%
	if(duty_ch2 > PERIOD)
		duty_ch2 = PERIOD/50; // ������������ �������� ��������� - �� 0 �� 100%
	TIM4->CCR1 = duty_ch1; // ��������� ������������ ���1
	TIM4->CCR2 = duty_ch2; // ��������� ������������ ���2
}

int main(void)
{

/*������������� ����12 ����� D*/
RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; //��������� ���� D
GPIOD->MODER |= GPIO_MODER_MODER12_1 | GPIO_MODER_MODER13_1; //�������������� ��� 12 �� �������������� ����� �����/������
GPIOD->OTYPER &= ~GPIO_OTYPER_ODR_12 & ~GPIO_OTYPER_ODR_13;  //����� � ��������� (push-pull)
GPIOD->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR12 &  ~GPIO_OSPEEDER_OSPEEDR13;  //�������� ����� ����� ������

/*������������� ������� 4*/
RCC->APB1ENR |= RCC_APB1ENR_TIM4EN | RCC_APB1ENR_TIM3EN;; //��������� ������4 � ������ 3
// ��� ��������� ������� 0,1 ���� (10 ��� ������ ���) ������� ��������� ���������: (1/16000000�� * 16 * 100 = 0,0001 ���
TIM4->PSC = 16-1; // ������������ TIM4
TIM4->ARR = PERIOD; //�������� ������������ TIM4, � ������ ������ ��� �������� ����� ������� �������� ���
TIM4->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2; /*TIM4_CCMR1 - �������1 ��������� ������
�������/��������� TIM4; ���� 4-6 ������1 � ���� 14-12 ������2 ������������� �������� ����� ���������, � ������ ������� � ��� ���� �������� 110,
��� ����� ������� 1 � 2 TIM4 ����� ���1; ���� ������� ������� �������� �� ��������: ��� CNT<CCR1 ������ OC1ref=1, ����� OC1ref=0.*/
TIM4->CCER |= TIM_CCER_CC1E | TIM_CCER_CC2E; /*TIM4_CCER ������� ��������� ������ �������/��������� �������4; ��� 0 � 5 ��������� ��������
											����� ��������� ������ 1 � 2 TIM4 �� ���� 12 � 13 ����� D*/
TIM4->CCR1 = duty_ch1; // TIM4_CCR1 �������1 �������/��������� TIM4, ����� ������� ������������ ���
TIM4->CCR2 = duty_ch2;
TIM4->CR1 |= TIM_CR1_CEN; //������� ������4

/*������������� ������� 3*/
TIM3->PSC = 3200-1;
TIM3->ARR = 100;
TIM3->DIER |= TIM_DIER_UIE; //�������� ���������� ��� ���������� (������������) ������� 3
TIM3->CR1 |= TIM_CR1_CEN; //��������� ������3
NVIC_EnableIRQ(TIM3_IRQn); //��������� ���������� � ����������� ����������. ���.218 PM0214
__enable_irq(); // �������� ���������� ���������

GPIOD->AFR[1] |= GPIO_AFRH_PIN12_AF2 | GPIO_AFRH_PIN13_AF2; /*������������� ���12 �� CH1 TIM4 (alt.mode), ������������� ���13 �� CH2 TIM4,
��� ����� ���������� "1" � 17 � 21 ��� �������� GPIO_AFRH. ��� ����� ��������������� ������ ��� 62-70 datasheet STM32F407xx*/

    while(1)
    {
 /*  	for (unsigned int cnt_delay = 0; cnt_delay < PAUSE; cnt_delay++) {};

    	duty_ch1 += Period/100; // ���������� ������������ �� 1 % �� �������
    	duty_ch2 += Period/50; // ���������� ������������ �� 2 % �� �������
    	if(duty_ch1 > Period)
    		duty_ch1 = 0; // ������������ �������� ��������� - �� 0 �� 100%
    	if(duty_ch2 > Period)
    	    duty_ch2 = 0; // ������������ �������� ��������� - �� 0 �� 100%
    	TIM4->CCR1 = duty_ch1; // ��������� ������������ ���1
    	TIM4->CCR2 = duty_ch2; // ��������� ������������ ���2*/
    }
}



