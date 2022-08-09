//��������������� ������. ������ �� �������� (������). ����� �����.
#define stm32f4xx
#include "stm32f4xx.h" // �������� ���������
#include "stdio.h" // �������� ����������� �������
// ����������� ������ ���
#define APB1_FREQUENCY 16000000L // ������� ����������
#define UART_BANDWITH 57600 //���������� ����������� UART

#include "DELAY_TIM7.h" // ������� ��������
#include "UART_STM32.h" // ���������� ������� ��� ������ � USART3
char msg[100]; // ������ ��� ������������ ��������� ��� ������ �� UART

//���
#define LENGTH 200

// ���
float Vref = 3.3; // ������� ���������� ��� ���
float ADC_float; // ��������� ��� � ���� ����������
int main()
{
	// ��������� ������ ���
SET_BIT (RCC->AHB1ENR, RCC_AHB1ENR_GPIOCEN); // ��������� �������� ��������� GPIOC
SET_BIT (GPIOC->MODER, GPIO_MODER_MODER2_0 | GPIO_MODER_MODER2_1); // ����������� ����� PC2 �� ���������� �����
SET_BIT (GPIOC->MODER, GPIO_MODER_MODER1_0 | GPIO_MODER_MODER1_1); // ����������� ����� PC1 �� ���������� �����

// ��������� ���
SET_BIT (RCC->APB2ENR, RCC_APB2ENR_ADC1EN); // �������� �������� ���������;
// � �������� �������� ���������� ������������ AVDD � AVSS
// ��������������� ������: ��� 1 ������ ����� ������ ���� ������� � JSQ4
// ��� 2 ������� ������ ������� ������������������ ������ ���� �������� � JSQ3 � JSQ4
// ��� 3 ������� ������ ������� ������������������ ������ ���� �������� � JSQ2 � JSQ3 � JSQ4
// ��� 4 ������� ������ ������� ������������������ ������ ���� �������� � JSQ1 � JSQ2 � JSQ3 � JSQ4
CLEAR_BIT (ADC1->CR1, (ADC_CR1_RES_0 | ADC_CR1_RES_1)); //12 ��� ���
CLEAR_BIT (ADC1->CR2, ADC_CR2_ALIGN); //������ ������������ �����
SET_BIT (ADC1->JSQR, ADC_JSQR_JL_0); //2 ��������������
SET_BIT (ADC1->JSQR, (ADC_JSQR_JSQ3_0 | ADC_JSQR_JSQ3_1 | ADC_JSQR_JSQ3_3)); //���� �������������� ���, �� ������ ����� (����� 11 � PC1) � JSQ_3, ������ � JSQ_4 (����� 12 � PC2)
SET_BIT (ADC1->JSQR, (ADC_JSQR_JSQ4_2 | ADC_JSQR_JSQ4_3));
SET_BIT (ADC1->CR1, ADC_CR1_JEOCIE); // �������� ���������� �� ��������� ��������������
SET_BIT (ADC1->CR1, ADC_CR1_SCAN); // ������ ��� �� ������������ ��������� �������

// ��������� ������� �� ��������
SET_BIT (ADC1->CR2, ADC_CR2_JEXTEN_1); // ������� ����������� �� ���������� ������
SET_BIT (ADC1->CR2, ADC_CR2_JEXTSEL_1); // � �������� �������� ������ ������ "Timer 2 ��1 event"
//SET_BIT (ADC1->CR2, (ADC_CR2_JEXTSEL_1 | ADC_CR2_JEXTSEL_0)); // � �������� �������� ������ ������ "Timer 2 TRGO event"
SET_BIT (ADC1->CR2, ADC_CR2_ADON); // �������� ���

UART3_init(APB1_FREQUENCY, UART_BANDWITH); // ������������� UART3
UART3_PutRAMString("ADC injected channels with :\r\n");

// ��������� ������� TIM2
SET_BIT (RCC->APB1ENR, RCC_APB1ENR_TIM2EN); // �������� �������� ��������� TIM2
WRITE_REG(TIM2->PSC, 8000-1); // ������������
WRITE_REG(TIM2->ARR, 2000-1); // �������� ������������
SET_BIT (TIM2->CCMR1, (TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1)); // ������� ����� ��� 1
WRITE_REG(TIM2->CCR1, LENGTH); // ������������ ���
SET_BIT (TIM2->CCER, TIM_CCER_CC1E); // �������� ����� ������� �� ����� ��������� �� ����� ��
SET_BIT (TIM2->CR1, TIM_CR1_CEN); // �������� ������ �������

NVIC_EnableIRQ(ADC_IRQn); // �������� ���������� ADC � NVIC

	while(1)
	{

	}
}

// ��������� ���������� �� ADC
void ADC_IRQHandler(void)
{
	if(ADC1->SR & ADC_SR_JEOC) // ���� ��������� ���������� �� ADC1_JEOC
	{
		ADC1->SR &= ~ADC_SR_JEOC; // ������� ���� ���������
		sprintf(msg,("ADC_result 11 = %f V, ADC_result 12 = %f V\r\n"), (Vref*ADC1->JDR1/4096),(Vref*ADC1->JDR2/4096)); // ������� �������� ��� � ������
		UART3_PutRAMString (msg); // ������� ������ �� UART
	}
}
