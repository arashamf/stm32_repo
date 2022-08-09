
 //���. ��������������� ������. ����������� ������ 2 ������ (����� � ����������).
#define stm32f4xx
#include "stm32f4xx.h" // �������� ���������
#include "stdio.h" // �������� ����������� �������
// ����������� ������ ��� ����������������
#define APB1_FREQUENCY 16000000L // ������� ����������
#define UART_BANDWITH 57600 //���������� ����������� UART
#include "UART_STM32.h"
#include "DELAY_TIM7.h"
char msg[100]; // ������ ��� ������������ ��������� ��� ������ �� UART
//���
#define LENGTH 500
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
SET_BIT (RCC->APB2ENR, RCC_APB2ENR_ADC1EN); // �������� �������� ���������
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
SET_BIT (ADC1->CR2, ADC_CR2_ADON); // �������� ���

UART3_init (APB1_FREQUENCY, UART_BANDWITH); // ������������� UART3
UART3_PutRAMString ("injected channels:\r\n");

SET_BIT (ADC1->CR2, ADC_CR2_JSWSTART); // ����������� ������ ���

while(1)
	{
	if(READ_BIT (ADC1->SR, ADC_SR_JEOC)) // �������� ��������� ����� ��������� ��������������
		{
		sprintf(msg,("ADC_result 11 = %f V, ADC_result 12 = %f V\r\n"), (Vref*ADC1->JDR1/4096), (Vref*ADC1->JDR2/4096)); //������� �������� ��� � ������
		UART3_PutRAMString (msg); //������� ������ �� UART
		CLEAR_BIT (ADC1->SR, ~ADC_SR_JEOC); //������� ���� ���������
//		ADC1->CR2 |= ADC_CR2_JSWSTART;
		}
	delay_ms (800);
	}
}
