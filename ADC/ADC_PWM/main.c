//����������� ������ ���
#define stm32f4xx
#include "stm32f4xx.h" // �������� ���������
#include "stdio.h" // �������� ����������� �������
#include "LCD_STM32.h"
#include "UART_STM32.h"
#include  "string.h"
#define PERIOD 4096 //������ ��������� ���
#define GPIO_AFRH_PIN12_AF2 0x00020000

char buffer [16];
char msg[20]; // ������ ��� ������������ ��������� ��� ������ �� UART
const float Uref=3.3;
unsigned int duty = 4096;

int main()
{
// ��������� ����� ���
RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN; // ��������� �������� ��������� GPIOC
GPIOC->MODER |=GPIO_MODER_MODER1; // ����������� ���� PC1 �� ���������� ����� (ADC123_IN11)
// ��������� ���
RCC->APB2ENR |= RCC_APB2ENR_ADC1EN; // �������� �������� ���������
//����������� ��� ������: ������� CR1-RES[1:0] = 0: ���������� 12 ���; ������� CR2: ALIGN = 0 - ������ ������������;
// � �������� �������� ���������� ������������ AVDD � AVSS
ADC1->SQR1 &= ~ADC_SQR1_L; // ����� ������������������ ���������� ������� = 1;
ADC1->SQR3 |= ADC_SQR3_SQ1_0 | ADC_SQR3_SQ1_1| ADC_SQR3_SQ1_3; // ����� 11 ��� ������������������ ���������� ������� = 1;
ADC1->CR2 |= ADC_CR2_ADON; // �������� ���

UART3_init (); //������������� UART
UART3_PutRAMString("ADC:\r\n");

LCD_STM_INIT (); //�������� ������ ��, ������������ � LCD
LCD_CHAR_init(16); // �������������	16 ���������� ������

/*������������� ������� 4*/
RCC->APB1ENR |= RCC_APB1ENR_TIM4EN; //��������� ������4
// ��� ��������� ������� 0,1 �� (10 ��� ������ ���) ������� ��������� ���������: (1/16000000�� * 16 * 100 = 0,1 ��)
TIM4->PSC = 16-1; // ������������ TIM4
TIM4->ARR = PERIOD; //�������� ������������ TIM4 ����� ������� �������� ���
TIM4->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1;
/*TIM4_CCMR1 - �������1 ��������� ������ �������/��������� TIM4; ���� 4-6 ������1 � ���� 14-12 ������2 ������������� �������� ����� ���������, � ������ ������� � ��� ���� �������� 110,
��� ����� ������� 1 � 2 TIM4 ����� ���1; ���� ������� ������� �������� �� ��������: ��� CNT<CCR1 ������ OC1ref=1, ����� OC1ref=0*/
TIM4->CCER |= TIM_CCER_CC1E;
//TIM4_CCER ������� ��������� ������ �������/��������� �������4; ��� 0 � 5 ��������� �������� ����� ��������� ������ 1 � 2 TIM4 �� ���� 12 � 13 ����� D
TIM4->CCR1 = duty; //TIM4_CCR1 �������1 �������/��������� TIM4, ����� ������� ������������ ���
TIM4->CR1 |= TIM_CR1_CEN; //������� ������4

/*������������� ����12 ����� D*/
RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; //��������� ���� D
GPIOD->MODER |= GPIO_MODER_MODER12_1; //�������������� ��� 12 �� �������������� ����� �����/������
GPIOD->OTYPER &= ~GPIO_OTYPER_ODR_12;  //����� � ��������� (push-pull)
GPIOD->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR12;  //�������� ����� ����� ������
GPIOD->AFR[1] |= GPIO_AFRH_PIN12_AF2;

while(1)
	{
	float voltage = 0;
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
	duty = ADC1->DR;
	TIM4->CCR1 = duty;
	memset(buffer, '\0', sizeof(buffer));
	memset(msg, '\0', sizeof(msg));
	delay_ms (1000); // ��������
	}
}
