/*���.���������� ������.������ �� �������� �� �������� ����������*/
#define stm32f4xx
#include "stm32f4xx.h" // �������� ���������
#include "stdio.h" // �������� ����������� �������

// ����������� ������ ��� ��
#define BANDWITH 57600 //���������� ����������� ����
#define APB1_FREQUENCY 16000000 // ������� ���� APB1

#include "DELAY_TIM7.h" // ������� ��������
#include "UART_STM32.h" // ���������� ������� ��� ������ � USART3
char msg[130]; // ������ ��� ������������ ��������� ��� ������ �� UART
typedef unsigned char byte;

__I float Vref = 3.3; // ������� ���������� ��� ���; __I = volatile const
__IO float ADC_float = 0; // ��������� ��� � ���� ����������; __IO = volatile

#define GPIO_AFRH_PIN13_AF2 0x200000 // ����������� ��� ����. ������� AF2 ��� PD13
//#define GPIO_AFRL_PIN6_AF2 0x200000//

#define PERIOD 10000  //�������� ������� ���

// ���������� ��� ��������� �������� ��������� ������
#define KEY_BOUNCE_TIME 500// ����� �������� + ����������� � ��
__IO byte flag_ADC = 0;
static volatile unsigned int KEY_Bounce_Time_Cnt;
static volatile byte KEY_Press_Flag = 0; // ���� ������� �� ������: 0 - �� ������; 1 - ������

// ��������� ���������� EXTI0
void EXTI15_10_IRQHandler(void)
{
	if(EXTI->PR & EXTI_PR_PR11)
	{
		UART3_PutRAMString("hello\r\n"); // ������� ������ �� UART
		KEY_Bounce_Time_Cnt = KEY_BOUNCE_TIME; // ������� ������� �������� - ��������
		KEY_Press_Flag = 1; // ���� ������� �� ������ - ���������
		EXTI->PR |= EXTI_PR_PR11; // ����� ����� ����������
		EXTI->IMR &= ~EXTI_IMR_MR11; // �������� ���������� �� EXTI0
		flag_ADC = 0;
	}
}

// ��������� ���������� �� TIM3
void TIM4_IRQHandler(void)
{
TIM4->SR &= ~TIM_SR_UIF; // ������� ���� ����������
if(KEY_Bounce_Time_Cnt)
	{
	KEY_Bounce_Time_Cnt--; // ��������� �������
	}
else //���� �����
	{
//	EXTI->PR |= EXTI_PR_PR11; // ����� ����� ����������
	if (!READ_BIT(EXTI->IMR, EXTI_IMR_MR11))
		{
		flag_ADC = 1;
		EXTI->IMR |= EXTI_IMR_MR11; // �������� ���������� �� EXTI0
		}
	}
}

// ��������� ���������� �� ADC
void ADC_IRQHandler(void)
{

//if(ADC1->SR & ADC_SR_EOC) // ���� ��������� ���������� �� ADC1 EOC
if (flag_ADC)
	{
	ADC_float = Vref*ADC1->DR/4096; // ������� ����������/������������ ����� ��������� * ��������� ��������������
	sprintf(msg,("ADC_result = %f V\r\n"), ADC_float); // ������� �������� ��� � ������
	UART3_PutRAMString(msg); // ������� ������ �� UART
//	WRITE_REG (TIM4->CCR2, PERIOD/2); //������������ ���
	ADC1->SR &= ~ADC_SR_EOC; // ������� ���� ���������
	}
}

int main()
{
//��������� ������ 13 ��� �������� ������������
SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIODEN); // ��������� �������� ��������� GPIOD
SET_BIT(GPIOD->MODER, GPIO_MODER_MODER13_1); //��� 13 �� �������������� �����
GPIOD->AFR[1] |= GPIO_AFRH_PIN13_AF2; //����� ����. ������ ��� ���� 13 (AF2, TIM4_CH2)

// ��������� ����� ��� ������
//SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIODEN); // ��������� �������� ��������� GPIOD
//CLEAR_BIT(GPIOD->MODER, GPIO_MODER_MODER11); // ����������� ����� 11 �� ����, ��� ������ ���� �������� �� ����
//SET_BIT(GPIOD->PUPDR, GPIO_PUPDR_PUPDR11_0); // ���������� �������� �����
SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOCEN); // ��������� �������� ��������� GPIOD
CLEAR_BIT(GPIOC->MODER, GPIO_MODER_MODER11); // ����������� ����� 11 �� ����, ��� ������ ���� �������� �� ����
SET_BIT(GPIOC->PUPDR, GPIO_PUPDR_PUPDR11_0); // ���������� �������� �����

// �������� ���� �� "������� ����������"
SET_BIT(RCC->APB2ENR, RCC_APB2ENR_SYSCFGEN); // �������� ������������
//SET_BIT(SYSCFG->EXTICR[2], SYSCFG_EXTICR3_EXTI11_PD); // ��������� EXTI11 � PD11
SET_BIT(SYSCFG->EXTICR[2], SYSCFG_EXTICR3_EXTI11_PC); // ��������� EXTI11 � PC11
//SET_BIT(EXTI->RTSR, EXTI_RTSR_TR11); // ������������� �� ������������ ������ (rise)
SET_BIT(EXTI->FTSR, EXTI_FTSR_TR11); //������������ �� ���������� ������
SET_BIT(EXTI->IMR, EXTI_IMR_MR11); // �������� ���������� �� EXTI11

// ��������� ����� � ��� ���
SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOCEN); // ��������� �������� ��������� GPIOC
SET_BIT(GPIOC->MODER, GPIO_MODER_MODER1_1 | GPIO_MODER_MODER1_0); // ����������� ����� PC1 �� ���������� �����

// ��������� ���
SET_BIT(RCC->APB2ENR, RCC_APB2ENR_ADC1EN); // �������� �������� ���������
//������� SQx[4:0] ������ ����� ������, ��� � � ��� ����� ��������������
CLEAR_BIT(ADC1->SQR1, ADC_SQR1_L); //������� ���������� ������� ���������� ������: 1
SET_BIT(ADC1->SQR3, ADC_SQR3_SQ1_0 | ADC_SQR3_SQ1_1| ADC_SQR3_SQ1_3); // ����� 11 (PC1)
//SET_BIT(ADC1->CR2, ADC_CR2_EXTEN_0); // ������� ����������� �� ������������ ������
SET_BIT(ADC1->CR2, ADC_CR2_EXTEN_1); // ������� ����������� �� ���������� ������
//SET_BIT(ADC1->CR2, ADC_CR2_EXTSEL_1 | ADC_CR2_EXTSEL_2); // � �������� �������� ������ ������ "Timer 2 TRGO event"
//SET_BIT(ADC1->CR2, ADC_CR2_EXTSEL_2 | ADC_CR2_EXTSEL_1 | ADC_CR2_EXTSEL_0); // � �������� �������� ������ ������ " Timer 3 CC1 event"
//SET_BIT(ADC1->CR2, ADC_CR2_EXTSEL_3); // � �������� �������� ������ ������ "Timer 3 TRGO event"
SET_BIT(ADC1->CR2, ADC_CR2_EXTSEL_3 | ADC_CR2_EXTSEL_2 | ADC_CR2_EXTSEL_1 | ADC_CR2_EXTSEL_0); // � �������� �������� ������ ������ �� ����. ����������
SET_BIT(ADC1->CR1, ADC_CR1_EOCIE); // �������� ���������� ADC �� ��������� ��������������
SET_BIT(ADC1->CR2, ADC_CR2_ADON); // �������� ���

//��������� TIM4 ��� ���
/*SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM4EN); //������� �������� ��������� �������4
WRITE_REG (TIM4->PSC, 16-1); // ������������ TIM4
WRITE_REG (TIM4->ARR, PERIOD); // �������� ������������ TIM4
SET_BIT(TIM4->CCMR1, TIM_CCMR1_OC2M_1 | TIM_CCMR1_OC2M_2); //������ ������ 1 ��� ������ 2
SET_BIT(TIM4->CCER, TIM_CCER_CC2E); //���������� ����� 2 TIM4
WRITE_REG (TIM4->CCR2, PERIOD/2); //������������ �������� ���
SET_BIT(TIM4->CR1, TIM_CR1_CEN); //������� ������4*/

// ��������� ������� TIM4
SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM4EN); // �������� �������� ��������� TIM3
WRITE_REG (TIM4->PSC, 16-1); //������������ TIM3
WRITE_REG (TIM4->ARR, 1000);  //�������� ������������ TIM3
SET_BIT(TIM4->DIER, TIM_DIER_UIE); // �������� ���������� ��� ������������ TIM3
SET_BIT(TIM4->CR1, TIM_CR1_CEN); // �������� ���� TIM3

//��������� UART
UART3_init(APB1_FREQUENCY, BANDWITH); // ������������� UART3
UART3_PutRAMString("ADC_External_Trigger_EXTI:\r\n");

NVIC_EnableIRQ(EXTI15_10_IRQn); // ��������� NVIC ��� EXTI11
NVIC_EnableIRQ(TIM4_IRQn); // ��������� NVIC ��� TIM4
NVIC_EnableIRQ(ADC_IRQn); // �������� ���������� ADC � ����������� NVIC

__enable_irq(); // �������� ���������� ���������

while(1)
	{
	}
}
