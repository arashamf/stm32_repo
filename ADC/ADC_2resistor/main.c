//����������� ������ ���
//
#define stm32f4xx
#include "stm32f4xx.h" // �������� ���������
#include "stdio.h" // �������� ����������� �������
#include "LCD_STM32.h"
#include "UART_STM32.h"
#include  "string.h"

char buffer [16];
char msg[20]; // ������ ��� ������������ ��������� ��� ������ �� UART
const float Uref=3.3;

int main()
{
// ��������� ����� ���
RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN; // ��������� �������� ��������� GPIOC
GPIOC->MODER |=GPIO_MODER_MODER1 | GPIO_MODER_MODER2; // ����������� ���� PC1 � PC2 �� ���������� ����� (ADC123_IN11 � ADC123_IN12)
// ��������� ���
RCC->APB2ENR |= RCC_APB2ENR_ADC1EN; // �������� �������� ���������
//����������� ��� ������: ������� CR1-RES[1:0] = 0: ���������� 12 ���; ������� CR2: ALIGN = 0 - ������ ������������;
// � �������� �������� ���������� ������������ AVDD � AVSS
ADC1->SQR1 &= ~ADC_SQR1_L; // ����� ������������������ ���������� ������� = 1;
/*ADC1->SQR3 |= ADC_SQR3_SQ1_0 | ADC_SQR3_SQ1_1| ADC_SQR3_SQ1_3; // ����� 11 ��� ������������������ ���������� ������� = 1;
ADC1->SQR3 |= ADC_SQR3_SQ2_2 | ADC_SQR3_SQ2_3; // ����� 12 ��� ������������������ ���������� ������� = 2;*/
ADC1->CR2 |= ADC_CR2_ADON; // �������� ���

UART3_init (); //������������� UART
UART3_PutRAMString("ADC:\r\n");

LCD_STM_INIT (); //�������� ������ ��, ������������ � LCD
LCD_CHAR_init(16); // �������������	16 ���������� ������

while(1)
	{
	float voltage = 0;
	ADC1->SQR3 |= ADC_SQR3_SQ1_0 | ADC_SQR3_SQ1_1| ADC_SQR3_SQ1_3; // ����� 11 ��� ������������������ ���������� ������� = 1;
	ADC1->CR2 |= ADC_CR2_SWSTART; // ����������� ������ ���
	while(!(ADC1->SR & ADC_SR_EOC)) {}; // ���� ���� �������������� ����������
	voltage = Uref * ADC1->DR / 4096;
	sprintf(msg,("ADC_channel_11=%f_V\r\n"), voltage); // ������� �������� ��� � ������
	UART3_PutRAMString(msg); // ������� ������ �� UART
	sprintf(buffer,("%ld; U=%.2fV"),ADC1->DR, voltage);
	LCD_CHAR_wr(0x01, 0); //�������� ������� � ���������� ������ � ��������� �������
	delay_ms(1);
	LCD_CHAR_gotoxy(0,0); //���������� ������ � ������ ������� ������
	LCD_CHAR_puts("ADC_channel_11:");
	LCD_CHAR_gotoxy(0,1); //���������� ������ � ������ ������ ������
	LCD_CHAR_puts (buffer);
	memset(buffer, '\0', sizeof(buffer));
	memset(msg, '\0', sizeof(msg));
	ADC1->SQR3 &= ~ADC_SQR3_SQ1;
	delay_ms (1000); // ��������

	ADC1->SQR3 |= ADC_SQR3_SQ1_2 | ADC_SQR3_SQ1_3; // ����� 12 ��� ������������������ ���������� ������� = 1;
	ADC1->CR2 |= ADC_CR2_SWSTART; // ����������� ������ ���
	while(!(ADC1->SR & ADC_SR_EOC)) {}; // ���� ���� �������������� ����������
	voltage = Uref * ADC1->DR / 4096;
	sprintf(msg,("ADC_channel_12=%f_V\r\n"), voltage); // ������� �������� ��� � ������
	UART3_PutRAMString(msg); // ������� ������ �� UART
	sprintf(buffer,("%ld; U=%.2fV"),ADC1->DR, voltage);
	LCD_CHAR_wr(0x01, 0); //�������� ������� � ���������� ������ � ��������� �������
	delay_ms(1);
	LCD_CHAR_gotoxy(0,0); //���������� ������ � ������ ������� ������
	LCD_CHAR_puts("ADC_channel_12:");
	LCD_CHAR_gotoxy(0,1); //���������� ������ � ������ ������ ������
	LCD_CHAR_puts (buffer);
	memset(buffer, '\0', sizeof(buffer));
	memset(msg, '\0', sizeof(msg));
	ADC1->SQR3 &= ~ADC_SQR3_SQ1;
	delay_ms (1000); // ��������
	}
}
