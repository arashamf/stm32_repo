#define stm32f4xx
#include "stm32f4xx.h" // �������� ���������

// ���������� ��� ��������� �������� ��������� ������
#define KEY_Bounce_Time 400 // ����� �������� + ����������� � ��
static volatile unsigned int KEY_Bounce_Time_Cnt; // ������� ������� ��������
static volatile unsigned char KEY_Press_Flag = 0; // ���� ������� �� ������: 0 - �� ������; 1 - ������

static volatile unsigned char flag = 0; //����� ����������, ������� ���������� ������

// ��������� ���������� �� TIM4
void TIM4_IRQHandler(void)
{
TIM4->SR &= ~TIM_SR_UIF; // ������� ���� ����������
if(KEY_Bounce_Time_Cnt)
	KEY_Bounce_Time_Cnt--; // ��������� �������
else //���� �����
	{
	EXTI->PR |= EXTI_PR_PR0; // ����� ����� ����������
	EXTI->IMR |= EXTI_IMR_MR0; // �������� ���������� �� EXTI0
	}
}

// ��������� ���������� EXTI0
void EXTI0_IRQHandler(void)
{
KEY_Bounce_Time_Cnt = KEY_Bounce_Time; // ������� ������� �������� - ��������
KEY_Press_Flag = 1; // ���� ������� �� ������ - ���������
EXTI->PR |= EXTI_PR_PR0; // ����� ����� ����������
EXTI->IMR &= ~EXTI_IMR_MR0; // �������� ���������� �� EXTI0
}

int main()
{
// ���� ��� ������
RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; // ��������� �������� ��������� GPIOA
GPIOA->MODER &= ~GPIO_MODER_MODER0; // ����������� ����� 0 �� ����, ��� ������ ���� �������� �� ����
GPIOA->PUPDR |= GPIO_PUPDR_PUPDR0_0; // ���������� �������� �����

// �������� ���� �� "������� ����������"
RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN; // �������� ������������
//SYSCFG_EXTICR1 = 0; // ��� ������ EXTI0 ��������� � PA0
EXTI->IMR |= EXTI_IMR_MR0; // �������� ���������� �� EXTI0
EXTI->RTSR |= EXTI_RTSR_TR0; // ������������� �� ������������ ������ (rise)
NVIC_EnableIRQ(EXTI0_IRQn); // ��������� NVIC ��� EXTI
// ����� �� ���������
RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; // ��������� �������� ��������� GPIOD
GPIOD->MODER |= GPIO_MODER_MODER12_0 | GPIO_MODER_MODER13_0 | GPIO_MODER_MODER14_0 | GPIO_MODER_MODER15_0; // ����������� ������ �� �����
//GPIOD->OTYPER - // ��� ������ ����� Push-Pull

// ��������� ������� TIM4
// �� 1 ��: 16000000/16/1000 = 1000�� (1��)
RCC->APB1ENR |= RCC_APB1ENR_TIM4EN; // ��������� �������� ��������� TIM4
TIM4->PSC = 16-1; //������������ TIM4
TIM4->ARR = 1000; //�������� ������������ TIM4
TIM4->DIER |= TIM_DIER_UIE; // �������� ���������� ��� ������������ �������
TIM4->CR1 |= TIM_CR1_CEN; // �������� ���� �������
NVIC_EnableIRQ(TIM4_IRQn); // ��������� NVIC ��� TIM4
__enable_irq(); // �������� ���������� ���������

while(1)
	{
	// ��������� ���� ������� ������
	if(KEY_Press_Flag)
		{
		switch (flag)
			{
			case 0: GPIOD->BSRRH |= GPIO_BSRR_BS_15;  //��������� ����� 15
					GPIOD->BSRRL |= GPIO_BSRR_BS_12; //�������� ����� 12
					flag++;
					break;
			case 1: GPIOD->BSRRH |= GPIO_BSRR_BS_12;  //��������� ����� 12
					GPIOD->BSRRL |= GPIO_BSRR_BS_13; //�������� ����� 13
					flag++;
					break;
			case 2: GPIOD->BSRRH |= GPIO_BSRR_BS_13;  //��������� ����� 13
					GPIOD->BSRRL |= GPIO_BSRR_BS_14; //�������� ����� 14
					flag++;
					break;
			case 3: GPIOD->BSRRH |= GPIO_BSRR_BS_14;  //��������� ����� 14
					GPIOD->BSRRL |= GPIO_BSRR_BS_15; //�������� ����� 15
					flag = 0;  //�������� ����, ��� ��������� ������� ������ ���������� 1 ���������
					break;
			default: break;
			}
		KEY_Press_Flag = 0; // ���� ������� �� ������ - �������
		}
	}
}
