#define stm32f4xx
#include "stm32f4xx.h" // �������� ���������

// ���������� ��� ��������� �������� ��������� ������
#define KEY_Bounce_Time 400 // ����� �������� + ����������� � ��
static volatile unsigned int KEY_Bounce_Time_Cnt; // ������� ������� ��������
static volatile unsigned char KEY_Press_Flag = 0; // ���� ������� �� ������: 0 - �� ������; 1 - ������

#define PERIOD 100 //������ ��������� ���
#define GPIO_AFRH_PIN12_AF2 0x00020000
unsigned int duty_ch1 = 0; // ������������ ��������� ���
static volatile unsigned char flag = 0; //���� ���������� ��� �������� ���������� ���

// ��������� ���������� EXTI0 �� ������
void EXTI0_IRQHandler(void)
{

KEY_Bounce_Time_Cnt = KEY_Bounce_Time; // ������� ������� �������� - ��������
KEY_Press_Flag = 1; // ���� ������� �� ������ - ���������
duty_ch1 = PERIOD/100;
TIM4->CCR1 = duty_ch1;  //������� ���������� ��� = 100
EXTI->PR |= EXTI_PR_PR0; // ����� ����� ����������
EXTI->IMR &= ~EXTI_IMR_MR0; // �������� ���������� �� EXTI0
}

//�-� ��������� ���������� �� �������2, �������� ����� ������� ������
void TIM2_IRQHandler(void)
{
TIM2->SR &= ~TIM_SR_UIF; // ������� ���� ����������
if(KEY_Bounce_Time_Cnt)
	KEY_Bounce_Time_Cnt--; // ��������� �������
else //���� �����
	{
//	EXTI->PR |= EXTI_PR_PR0; // ����� ����� ����������
	EXTI->IMR |= EXTI_IMR_MR0; // �������� ���������� �� EXTI0
	}
}

//�-� ��������� ���������� �� �������3, ��������� ���������� ���
void TIM3_IRQHandler(void)
	{
	TIM3->SR &= ~TIM_SR_UIF; // ������� ���� ����������
	if (!flag)
		duty_ch1 += PERIOD/100; // ���������� ������������ �� 1 % �� �������
	else
		duty_ch1 -= PERIOD/100;
	if((duty_ch1 == (PERIOD - 1)) || (duty_ch1 == 1))
		flag ^= 1; //����������� �������� �����
	TIM4->CCR1 = duty_ch1; // ��������� ������������ ���1
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
GPIOD->MODER |= GPIO_MODER_MODER12_1; // ����������� ����� �� ����. �����
GPIOD->OSPEEDR = 0x0;  //�������� ����� ����� ������
//GPIOD->OTYPER - // ��� ������ ����� Push-Pull
GPIOD->AFR[1] |= GPIO_AFRH_PIN12_AF2; //������������� ���12 �� CH1 TIM4 (alt.mode)
GPIOD->BSRRH |= GPIO_BSRR_BS_12;

// ��������� ������� 2
RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; // ��������� �������� ��������� TIM2
TIM2->PSC = 16-1;
TIM2->ARR = 1000;
TIM2->DIER |= TIM_DIER_UIE; // �������� ���������� ��� ������������ �������2
TIM2->CR1 |= TIM_CR1_CEN; // �������� ���� �������2
NVIC_EnableIRQ(TIM2_IRQn); // ��������� NVIC ��� TIM2

// ��������� ������� 3
RCC->APB1ENR |= RCC_APB1ENR_TIM3EN; // ��������� �������� ��������� TIM3
TIM3->PSC = 8000-1; //������������ TIM3
TIM3->ARR = PERIOD; //�������� ������������ TIM3, ���������� 16000000/8000/100=100=50��
TIM3->DIER |= TIM_DIER_UIE; // �������� ���������� ��� ������������ TIM3
//TIM3->CR1 |= TIM_CR1_CEN; // �������� ���� ������� 3
NVIC_EnableIRQ(TIM3_IRQn); // ��������� NVIC ��� TIM3

/*������������� ������� 4*/
RCC->APB1ENR |= RCC_APB1ENR_TIM4EN; //��������� �������� ��������� TIM4
TIM4->CCER |= TIM_CCER_CC1E; //TIM4_CCER ������� ��������� ������ �������/��������� �������4; ��� 0 ��������� �������� ����� ��������� ������ 1 TIM4 �� ��� 12
TIM4->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1; /*TIM4_CCMR1 - �������1 ��������� ������ �������/��������� TIM4;  110����� ������ 1 TIM4 ����� ���1 - ������ ���*/
//TIM4->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_0; //��������� ���
// ��� ��������� ������� 0,2 ���� (5 ��� ������ ���) ������� ��������� ���������: (1/16000000�� * 16 * 100 = 0,0001�)
TIM4->CR1 |= TIM_CR1_CMS_1 | TIM_CR1_CMS_0; //������������ ���-������� �� ������, ������� ���������� ���������� ��� ����, �� ����� ����� ����� � ����, ������� ��� ��������� � 2 ����
//TIM4->CR1 |= TIM_CR1_CMS_1;
TIM4->PSC = 32-1; // ������������ TIM4
TIM4->ARR = PERIOD; //�������� ������������ TIM4, ������ �������� ���
//TIM4->CCR1 = duty_ch1; // ����� ���������� ���
//TIM4->CCR1 = PERIOD/2;  //����� ���������� ���
//TIM4->CR1 |= TIM_CR1_CEN; //������� ������4

__enable_irq(); // �������� ���������� ���������

while(1)
	{
	// ��������� ���� ������� ������
	if(KEY_Press_Flag)
		{
		TIM4->CR1 ^= TIM_CR1_CEN;
		TIM3->CR1 ^= TIM_CR1_CEN;
		KEY_Press_Flag = 0; // ���� ������� �� ������ - �������
		}
	}
}
