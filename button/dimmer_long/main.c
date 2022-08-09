//��������� ���������� �������� ���������� � ������� ���� ������ c ���������� ���������� � ������� ��������
#define stm32f4xx
#include "stm32f4xx.h" // �������� ���������

// ���������� ��� ��������� �������� ��������� ������
#define KEY_BOUNCE_TIME 200 // ����� �������� � ��
#define KEY_REPEAT_TIME 500 // ����� ����������� � ��
static volatile unsigned int key_bounce_time_cnt; // ������� ������� ��������
static volatile unsigned int key_repeat_time_cnt; // ������� ������� �����������
static volatile unsigned char key_press_flag_short = 0; // ���� ��������� ������� �� ������: 0 - �� ������; 1 - ������
static volatile unsigned char key_press_flag_middle = 0; // ���� �������� ������� �� ������
static volatile unsigned char key_press_flag_long = 0; // ���� ������� ������� �� ������

#define GPIO_AFRH_PIN12_AF2 0x00020000  //������� ����� ��������� ���� 12 �� �������������� �������
#define PERIOD 1000 //������ ��������� ���
short duty_ch1 = PERIOD/2; // ������������ ��������� ���
static volatile unsigned char regul_flag = 0; //���� ���������� ��� ���������� �������, 1 - ��������� �������, 0 - ���������

short regul_led_brightness (unsigned char, short, short); //�������� �������

// ��������� ���������� EXTI0 �� ������
void EXTI0_IRQHandler(void)
{
key_bounce_time_cnt = KEY_BOUNCE_TIME; // ������� ������� �������� - ��������
key_press_flag_short = 1; // ���� ������� �� ������ - ���������
regul_flag = 1; //���� ���������� �������
EXTI->PR |= EXTI_PR_PR0; // ����� ����� ����������
EXTI->IMR &= ~EXTI_IMR_MR0 & ~EXTI_IMR_MR1; // �������� ���������� �� EXTI0 � EXTI1
}

// ��������� ���������� EXTI1 �� ������
void EXTI1_IRQHandler(void)
{
key_bounce_time_cnt = KEY_BOUNCE_TIME; // ������� ������� �������� - ��������
key_press_flag_short = 1; // ���� ������� �� ������ - ���������
regul_flag = 0; //���� ���������� �������
EXTI->PR |= EXTI_PR_PR1; // ����� ����� ����������
EXTI->IMR &= ~EXTI_IMR_MR0 & ~EXTI_IMR_MR1; // �������� ���������� �� EXTI0 � EXTI1
}

//�-� ��������� ���������� �� �������3, �������� ����� ������� ������
void TIM3_IRQHandler(void)
{
TIM3->SR &= ~TIM_SR_UIF; // ������� ���� ����������
if(key_bounce_time_cnt)
	key_bounce_time_cnt--; // ��������� �������
else //���� �����
	{
	if (key_press_flag_short)
		{
		GPIOD->BSRRH |= GPIO_BSRR_BS_14; //��������� 14 ���
		GPIOD->BSRRH |= GPIO_BSRR_BS_13; //��������� 13 ���
		duty_ch1 = regul_led_brightness (regul_flag, 100, duty_ch1);
		if ((!(GPIOA->IDR & GPIO_IDR_IDR_0)) || (!(GPIOA->IDR & GPIO_IDR_IDR_1)))  //���� ���� �� ������ ���������� ������
			{
			key_press_flag_middle = 1; //��������� ���� �������� �������
			key_repeat_time_cnt = KEY_REPEAT_TIME; //��������� ����� �������� ��� �������� �������
			GPIOD->BSRRL |= GPIO_BSRR_BS_13;  //�������� 13 ���
//			GPIOD->BSRRH |= GPIO_BSRR_BS_14; //��������� 14 ���
			}
		else
			{
			EXTI->IMR |= EXTI_IMR_MR0 | EXTI_IMR_MR1; // �������� ���������� �� EXTI0 � EXTI1
			}
		key_press_flag_short = 0;  //������� ���� ��������� �������
		TIM4->CCR1 = duty_ch1;
		}
	if (key_press_flag_middle)
		{
		if (key_repeat_time_cnt)
			{
			key_repeat_time_cnt--;
			}
		else
			{
			duty_ch1 = regul_led_brightness (regul_flag, 50, duty_ch1);
			key_press_flag_middle = 0;
			TIM4->CCR1 = duty_ch1;
			if ((!(GPIOA->IDR & GPIO_IDR_IDR_0)) || (!(GPIOA->IDR & GPIO_IDR_IDR_1)))  //���� ���� �� ������ ���������� ������
				{
				key_press_flag_long = 1; //��������� ���� �������� �������
				key_repeat_time_cnt = KEY_REPEAT_TIME; //��������� ����� �������� ��� �������� �������
				GPIOD->BSRRL |= GPIO_BSRR_BS_14;  //�������� 14 ���
				GPIOD->BSRRH |= GPIO_BSRR_BS_13; //��������� 13 ���
				}
			else
				{
				EXTI->IMR |= EXTI_IMR_MR0 | EXTI_IMR_MR1; // �������� ���������� �� EXTI0 � EXTI1
				}
			}
		}
	if (key_press_flag_long)
		{
		if (key_repeat_time_cnt)
			{
			key_repeat_time_cnt--;
			}
		else
			{
			duty_ch1 = regul_led_brightness (regul_flag, 25, duty_ch1);
			key_press_flag_long = 0;
			TIM4->CCR1 = duty_ch1;
			if ((!(GPIOA->IDR & GPIO_IDR_IDR_0)) || (!(GPIOA->IDR & GPIO_IDR_IDR_1)))  //���� ���� �� ������ ���������� ������
				{
				key_press_flag_long = 1; //��������� ���� �������� �������
				key_repeat_time_cnt = KEY_REPEAT_TIME; //��������� ����� �������� ��� �������� �������
				}
			else
				{
				EXTI->IMR |= EXTI_IMR_MR0 | EXTI_IMR_MR1; // �������� ���������� �� EXTI0 � EXTI1
				}
			}
		}
	}
}


int main()
{
// ���� ��� ������ ���������� �������� ����������
RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; // ��������� �������� ��������� GPIOA
GPIOA->MODER &= ~GPIO_MODER_MODER0 & ~GPIO_MODER_MODER1; // ����������� ����� 0 � 1 �� ����
GPIOA->PUPDR |= GPIO_PUPDR_PUPDR0_0 | GPIO_PUPDR_PUPDR1_0; // ���������� �������� �����

// �������� ���� �� "������� ����������"
RCC->APB2ENR |= RCC_APB2ENR_SYSCFGEN; // �������� ������������
SYSCFG->EXTICR[0] &= ~SYSCFG_EXTICR1_EXTI0 & ~SYSCFG_EXTICR1_EXTI1; // �������� ���������� EXTI0 � EXTI1 ��� ����� PA0 � PA1
EXTI->IMR |= EXTI_IMR_MR0 | EXTI_IMR_MR1; // �������� ���������� �� EXTI0 � EXTI1
//EXTI->RTSR |= EXTI_RTSR_TR0 | EXTI_RTSR_TR1; // ������������� �� ������������ ������ (rise) �� EXTI0 � EXTI1
EXTI->FTSR |= EXTI_FTSR_TR0 | EXTI_FTSR_TR1; // ������������� ���������� �� ���������� ������ (fall) �� EXTI0 � EXTI1
NVIC_EnableIRQ(EXTI0_IRQn); // ��������� NVIC ��� EXTI
NVIC_EnableIRQ(EXTI1_IRQn); // ��������� NVIC ��� EXTI

// ����� �� ���������
RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; // ��������� �������� ��������� GPIOD
GPIOD->MODER |= GPIO_MODER_MODER13_0 | GPIO_MODER_MODER14_0;
GPIOD->MODER |= GPIO_MODER_MODER12_1; // ����������� ����� �� ����. �����
GPIOD->OSPEEDR = 0x0;  //�������� ����� ����� ������
//GPIOD->OTYPER - // ��� ������ ����� Push-Pull
GPIOD->AFR[1] |= GPIO_AFRH_PIN12_AF2; //������������� ���12 �� CH1 TIM4 (alt.mode)

// ��������� ������� 3
RCC->APB1ENR |= RCC_APB1ENR_TIM3EN; // ��������� �������� ��������� TIM3
TIM3->PSC = 16-1; //������������ TIM3
TIM3->ARR = 1000; //�������� ������������ TIM3, ���������� 16000000/16/1000=1000=1��
TIM3->DIER |= TIM_DIER_UIE; // �������� ���������� ��� ������������ TIM3
TIM3->CR1 |= TIM_CR1_CEN; // �������� ���� ������� 3
NVIC_EnableIRQ(TIM3_IRQn); // ��������� NVIC ��� TIM3

/*������������� ������� 4 ��� ���*/
RCC->APB1ENR |= RCC_APB1ENR_TIM4EN; //��������� �������� ��������� TIM4
TIM4->CCER |= TIM_CCER_CC1E; //TIM4_CCER ������� ��������� ������ �������/��������� �������4; ��� 0 ��������� �������� ����� ��������� ������ 1 TIM4 �� ��� 12
TIM4->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1; /*TIM4_CCMR1 - �������1 ��������� ������ �������/��������� TIM4;  110����� ������ 1 TIM4 ����� ���1 - ������ ���*/
//TIM4->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1 | TIM_CCMR1_OC1M_0; //��������� ���
// ��� ��������� ������� 1 �� (1 ��� ������ ���) ������� ��������� ���������: (1/16000000�� * 16 * 1000 = 0,001�)
TIM4->CR1 |= TIM_CR1_CMS_1 | TIM_CR1_CMS_0; //������������ ���-������� �� ������, ������� ���������� ���������� ��� ����, �� ����� ����� ����� � ����, ������� ��� ��������� � 2 ����
TIM4->PSC = 16-1; // ������������ TIM4
TIM4->ARR = PERIOD; //�������� ������������ TIM4, ������ �������� ���
TIM4->CCR1 = duty_ch1;  //����� ���������� ���
TIM4->CR1 |= TIM_CR1_CEN; //������� ������4

__enable_irq(); // �������� ���������� ���������

while(1)
	{

	}
}

/*������� ����������� ������� ����������*/
short regul_led_brightness (unsigned char button_flag, short pulse_divider, short pulse_duration)
{
if (button_flag) //���� ���������� ������ �� ������ ������
	{
	if (pulse_duration >= PERIOD) //���� ������������ �������� ��������� ������ ���
		pulse_duration = 0; //����� ���������
	else
		pulse_duration += PERIOD/pulse_divider; //���� ���, �� ����������� ������� �� ���
	}
else //���� ���������� ������ �� ������ ������
	{
	if (pulse_duration <= 0) //���� ������������ �������� ������ ��� ����� 0
		pulse_duration = PERIOD; //��, ��������� �������� � ������ ����
	else
		pulse_duration -= PERIOD/pulse_divider; //���� ���, �� ��������� ������� �� ���
	}
return pulse_duration;
}
