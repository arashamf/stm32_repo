/*���.���������� ������.������ �� �������� �� �������*/
#define stm32f4xx
#include "stm32f4xx.h" // �������� ���������
#include "stdio.h" // �������� ����������� �������

// ����������� ������ ��� ��
#define BANDWITH 57600 //���������� ����������� ����
#define APB1_FREQUENCY 16000000 // ������� ���� APB1

#include "DELAY_TIM7.h" // ������� ��������
#include "UART_STM32.h" // ���������� ������� ��� ������ � USART3
char msg[130]; // ������ ��� ������������ ��������� ��� ������ �� UART

__I float Vref = 3.3; // ������� ���������� ��� ���; __I = volatile const
__IO float ADC_float = 0; // ��������� ��� � ���� ����������; __IO = volatile

// ������ ������������ ��������, �
/*const float theshold_hi = 2.8; // ������� �����
const float theshold_lo = 1.6; // ������*/

#define GPIO_AFRH_PIN13_AF2 0x200000 //
#define PERIOD 1000

// ��������� ���������� �� ADC
void ADC_IRQHandler(void)
{
if(ADC1->SR & ADC_SR_EOC) // ���� ��������� ���������� �� ADC1 EOC
	{
	ADC_float = Vref*ADC1->DR/4096; // ������� ����������/������������ ����� ��������� * ��������� ��������������
	sprintf(msg,("ADC_result = %f V\r\n"), ADC_float); // ������� �������� ��� � ������
	UART3_PutRAMString(msg); // ������� ������ �� UART
	ADC1->SR &= ~ADC_SR_EOC; // ������� ���� ���������
	}
}

int main()
{
//��������� ������ 13 ��� �������� ������������
SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIODEN); // ��������� �������� ��������� GPIOD
SET_BIT(GPIOD->MODER, GPIO_MODER_MODER13_1); //��� 13 �� �������������� �����
GPIOD->AFR[1] |= GPIO_AFRH_PIN13_AF2; //����� ����. ������ ��� ���� 13 (AF2, TIM4_CH2)

// ��������� ����� � ��� ���
SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOCEN); // ��������� �������� ��������� GPIOC
SET_BIT(GPIOC->MODER, GPIO_MODER_MODER1_1 | GPIO_MODER_MODER1_0); // ����������� ����� PC1 �� ���������� �����

// ��������� ���
SET_BIT(RCC->APB2ENR, RCC_APB2ENR_ADC1EN); // �������� �������� ���������
/*������� SQx[4:0] ������ ����� ������, ��� � � ��� ����� ��������������.*/
CLEAR_BIT(ADC1->SQR1, ADC_SQR1_L); //������� ���������� ������� ���������� ������: 1
SET_BIT(ADC1->SQR3, ADC_SQR3_SQ1_0 | ADC_SQR3_SQ1_1| ADC_SQR3_SQ1_3); // ����� 11 (PC1)
SET_BIT(ADC1->CR2, ADC_CR2_EXTEN_0); // ������� ����������� �� ������������ ������
SET_BIT(ADC1->CR2, ADC_CR2_EXTSEL_1 | ADC_CR2_EXTSEL_2); // � �������� �������� ������ ������ "Timer 2 TRGO event".
SET_BIT(ADC1->CR2, ADC_CR2_ADON); // �������� ���

// ��������� ������� TIM2
SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM2EN); // �������� �������� ��������� TIM2
TIM2->PSC = 16000-1; // ������������ (������� ������������ ������� ��� � 2 �)
TIM2->ARR = 2000; // �������� ������������
SET_BIT(TIM2->CR2, TIM_CR2_MMS_1); // � �������� TRGO ����� �������������� ������� "Update"
SET_BIT(TIM2->CR1, TIM_CR1_CEN); // �������� ������ �������

//��������� TIM4 ��� ���
/*SET_BIT(RCC->APB1ENR, RCC_APB1ENR_TIM4EN); //������� �������� ��������� �������4
TIM4->PSC = 160-1; // ������������ TIM4
TIM4->ARR = PERIOD; // �������� ������������ TIM4
SET_BIT(TIM4->CCMR1, TIM_CCMR1_OC2M_2 | TIM_CCMR1_OC2M_1); //������ ������ 1 ���
SET_BIT(TIM4->CCER, TIM_CCER_CC2E);   //���������� ����� 2 TIM4
TIM4->CCR2 = 0; //������������ ���
SET_BIT(TIM4->CR1, TIM_CR1_CEN); //������� ������4*/

//��������� UART
UART3_init(APB1_FREQUENCY, BANDWITH); // ������������� UART3
UART3_PutRAMString("ADC_External_Trigger:\r\n");

// ��������� ���������� �� ���
ADC1->CR1 |= ADC_CR1_EOCIE; // �������� ���������� ADC �� ��������� ��������������
NVIC_EnableIRQ(ADC_IRQn); // �������� ���������� ADC � ����������� NVIC
__enable_irq(); // �������� ���������� ���������

while(1)
	{
/*	ADC1->CR2 |= ADC_CR2_SWSTART; // ����������� ������ ���, ����� ������� �������������� ���������� ��������� ��������
	while(!(ADC1->SR & ADC_SR_EOC)) {}; // ������� ADC_SR - ��������� �������, ��������� ����, ��������������� �� ��������� ��������������
	ADC_float = Vref*ADC1->DR/4096; //������� ����������*��������� ��������������/������������ ����� ���������
	TIM4->CCR2 = ADC1->DR/4.096; // ����� ������������ ���
	sprintf(msg,("ADC_result 11 = %f V\r\n"), ADC_float); // ������� �������� ��� � ������
	UART3_PutRAMString(msg); // ������� ������ �� UART
	delay_ms(500); // �������� 1 �*/
	}
}

