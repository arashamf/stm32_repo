//���������������� ������ ��� ��� ���������� �������. ������ �� �������.
#define stm32f4xx
#include "stm32f4xx.h" // �������� ���������
#include "stdio.h" // �������� ����������� �������
// ����������� ������ ���
#define APB1_FREQUENCY 16000000L // ������� ����������
#define UART_BANDWITH 57600 //���������� ����������� UART
typedef unsigned char byte;

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

// ��������� ��� ��� ������ 11
SET_BIT (RCC->APB2ENR, RCC_APB2ENR_ADC1EN); // �������� �������� ��������� ���1;
//��������� ���������� ������� ���
CLEAR_BIT (ADC1->CR1, (ADC_CR1_RES_0 | ADC_CR1_RES_1)); //12 ��� ���
CLEAR_BIT (ADC1->CR2, ADC_CR2_ALIGN); //������ ������������ �����
CLEAR_BIT (ADC1->SQR1, ADC_SQR1_L); // �������������� 1 ������
SET_BIT (ADC1->SQR3, (ADC_SQR3_SQ1_0 | ADC_SQR3_SQ1_1 | ADC_SQR3_SQ1_3)); // ����� 11 (PC1)
CLEAR_BIT (ADC1->CR2, ADC_CR2_EOCS); //���� ��� EOCS=0: ��� EOC (������� ADC_CSR) ��������������� � ����� ������ ������������������ ���������� ��������������. ����������� ������������ ��������, ���� DMA=1.
//���� ��� EOCS=1: ��� EOC ��������������� � ����� ������� ����������� ��������������. �������� ����������� ������������.
SET_BIT (ADC1->CR1, ADC_CR1_EOCIE); //��� EOCIE ��������/��������� ���������� ����� ��������� �������������� ���. ���������� ������������, ����� ���������� ��� EOC (������� ADC_CSR)
SET_BIT (ADC1->CR1, ADC_CR1_OVRIE); // �������� ���������� ��� ����������
//SET_BIT (ADC->CCR, ADC_CCR_ADCPRE);  //ADC prescaler. �������� ������� ������� ��� �� 8
SET_BIT (ADC1->CR1, ADC_CR1_SCAN); // ������ ��� �� ������������ ��������� �������
SET_BIT (ADC1->CR2, ADC_CR2_EXTEN_0); // ������� ����������� �� ������������ ������
SET_BIT (ADC1->CR2, (ADC_CR2_EXTSEL_2 | ADC_CR2_EXTSEL_1)); // � �������� �������� ������ ������ "Timer 2 TRGO event"

// ��������� ��� ��� ������ 12
SET_BIT (RCC->APB2ENR, RCC_APB2ENR_ADC2EN); // �������� �������� ��������� ���2;
SET_BIT (ADC2->SQR3, (ADC_SQR3_SQ2_2 | ADC_SQR3_SQ2_3)); //����� 12 (PC2)
//SET_BIT (ADC->CCR, (ADC_CCR_MULTI_2 | ADC_CCR_MULTI_1 | ADC_CCR_MULTI_0)); //��������� ������ ������-���. � ������ ������: ������� �����, ������ ����� �����������
SET_BIT (ADC->CCR, (ADC_CCR_MULTI_1)); //��������� ������ ������-���. � ������ ������: ������� �����, ��������������� ���������� ������������� + �������������� ������ �� ��������
SET_BIT (ADC->CCR, (ADC_CCR_DELAY_0 | ADC_CCR_DELAY_1)); //�������� ������� 2-�� ��� - 8 ������. ��� ���� ������������ � ������� ��� ������� ������������ �������
SET_BIT (ADC1->CR2, ADC_CR2_ADON); // �������� ���1
SET_BIT (ADC2->CR2, ADC_CR2_ADON); // �������� ���2

UART3_init(APB1_FREQUENCY, UART_BANDWITH); // ������������� UART3
UART3_PutRAMString("Multi ADC mode:\r\n");

// ��������� ������� TIM2
SET_BIT (RCC->APB1ENR, RCC_APB1ENR_TIM2EN); // �������� �������� ��������� TIM2
WRITE_REG(TIM2->PSC, 8000-1); // ������������
WRITE_REG(TIM2->ARR, 2000-1); // �������� ������������
SET_BIT (TIM2->CCER, TIM_CCER_CC1E); // �������� ����� ������� �� ����� ��������� �� ����� ��*/
SET_BIT (TIM2->CR2, TIM_CR2_MMS_1); // � �������� TRGO ����� �������������� ������� "Update"
SET_BIT (TIM2->CR1, TIM_CR1_CEN); // �������� ������ �������

NVIC_EnableIRQ(ADC_IRQn); // �������� ���������� ADC � NVIC
//delay_ms (1000);

while(1)
	{
	}
}

// ��������� ���������� �� ADC
void ADC_IRQHandler(void)
{
if(ADC1->SR & ADC_SR_EOC) // ���� ��������� ���������� �� ADC1 EOC
	{
	sprintf(msg,("ADC1_result = %5.2f V\r\n"), Vref*ADC1->DR/4096); // ������� �������� ���1 � ������
	UART3_PutRAMString (msg); // ������� ������ �� UART
	ADC1->SR &= ~ADC_SR_EOC; // ������� ���� ���������
	}
if(ADC2->SR & ADC_SR_EOC) // ���� ��������� ���������� �� ADC1 EOC
	{
	sprintf(msg,("ADC2_result = %5.2f V\r\n"), Vref*ADC2->DR/4096); // ������� �������� ���2 � ������
	UART3_PutRAMString (msg); // ������� ������ �� UART
	ADC2->SR &= ~ADC_SR_EOC; // ������� ���� ���������
	}
}

