//��������� ������ ������ �� ���������� �������
#define stm32f4xx
#include "stm32f4xx.h" // �������� ���������
// ��� ��������� �������� ��������� ������
#define KEY_Bounce_Time 400 // ����� �������� + ����������� (� ��)
static volatile unsigned int KEY_Bounce_Time_Cnt; // ������� ������� ��������
static volatile unsigned char KEY_Press_Flag = 0; // ���� ������� �� ������: 0 - �� ������; 1 - ������

// ��������� ���������� �� TIM4
void TIM4_IRQHandler(void)
{
TIM4->SR &= ~TIM_SR_UIF; // ������� ���� ����������
// ���������, ����� �� ���������� ������
if(KEY_Bounce_Time_Cnt) //����  ������
	KEY_Bounce_Time_Cnt--; // ��������� �������
else // ���� �����
	{
	if(!(GPIOA->IDR & GPIO_IDR_IDR_0)) // ���� ������ ������ (�� ����� ���������� 0)
		{
		KEY_Bounce_Time_Cnt = KEY_Bounce_Time; // ������� ������� �������� - ��������
		KEY_Press_Flag = 1; // ���� ������� �� ������ - ���������
		}
	}
}

int main()
{
// ���� ��� ������
RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; // ��������� �������� ��������� GPIOA
GPIOA->MODER &= ~GPIO_MODER_MODER0; // ����������� ����� 0 �� ����, ��� ������ ���� �������� �� ����
GPIOA->PUPDR |= GPIO_PUPDR_PUPDR0_0; // ���������� �������� �����

// ���� ����� �� ���������
RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; // ��������� �������� ��������� GPIOD
GPIOD->MODER |= GPIO_MODER_MODER12_0; // ����������� ����� 12 �� �����
//GPIOD->OTYPER - // ��� ������ ����� Push-Pull

// ��������� ������� TIM4 �� 1 ��
// 16000000/16/1000 = 1000�� (1��)
RCC->APB1ENR |= RCC_APB1ENR_TIM4EN; // ��������� �������� ��������� TIM4
TIM4->PSC = 16-1;
TIM4->ARR = 1000;
TIM4->DIER |= TIM_DIER_UIE; // �������� ���������� ��� ������������ �������
TIM4->CR1 |= TIM_CR1_CEN; // �������� ���� �������
// ��������� NVIC ��� TIM4
NVIC_EnableIRQ(TIM4_IRQn);
__enable_irq(); // �������� ���������� ���������

while(1)
	{
	// ��������� ���� ������� ������
	if(KEY_Press_Flag)
		{
		// ����������� ���������
		if(GPIOD->ODR & GPIO_ODR_ODR_12)
			GPIOD->BSRRH |= GPIO_BSRR_BS_12;  //��������� ����� 12 
		else
			GPIOD->BSRRL |= GPIO_BSRR_BS_12; //�������� ����� 12
		KEY_Press_Flag = 0; // ���� ������� �� ������ - �������
		}
	}
}


