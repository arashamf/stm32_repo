#define stm32f4xx
#include "stm32f4xx.h" // �������� ���������
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#define GPIO_AFRH_AF7_UART3_TX 0x7
#define GPIO_AFRH_AF7_UART3_RX 0x70
#define GPIO_AFRH_PIN12_AF2 0x20000
#define GPIO_AFRL_PIN7_AF2 0x20000000

char msg[100]; // ������ ��� ������ �� UART
volatile unsigned int Period = 0; // ��������� ������ ��������
volatile unsigned int Width = 0; // ��������� ������������ ��������
volatile unsigned int capture1 = 0, capture2; // �������� ��������
volatile unsigned char Period_Flag = 0; // ���� ��� ��������� �������

/* ������� �������� ����� ������ �� UART*/
void UART3_TX_byte(char byte)
{
while(!(USART3->SR & USART_SR_TC)) {}; //USART_SR - ������� ������� usart, 6 ��� ���������������, ���� �������� ����� ��������� � ����� TXE ����������.
USART3->DR = byte; // �������� ����
}

/*������� �������� ������ ������ �� UART*/
void UART3_TX_string(char *str)
{
while((*str) != '\0') // ��������� �� ��������� ������ (������ 0x00)
	{
	UART3_TX_byte(*str); // ������� ������� ����
	str++; //��������� �� ��������� ������
	}
}

/*��������� ���������� �� TIM3*/
void TIM3_IRQHandler(void)
{
TIM3->SR &= ~TIM_SR_CC2IF; /* ������� ���� ����������,
��� CC2IF ��� ��������� ������� � ����� capture ����� "1", ����� �������� �������� ���� �������� � �������� TIM�_CCR2*/
if(Period_Flag == 0) // ���� ��� ��������� ������� - ������ �������
	{
	capture1 = TIM3->CCR2; // ������ �������� ������� ������ 2
	Period_Flag = 1; // ���� ��� ��������� ������� - ������ �������
	TIM3->CCER |= TIM_CCER_CC2P ; //����������� ������ �� ���� ��������
	}
else
	if(Period_Flag == 1) // ���� ��� ��������� ������� - ������ �������
		{
		Period_Flag = 2;
		Width = TIM3->CCR2 - capture1; // ���������� ����� ��������
		TIM3->CCER &= ~TIM_CCER_CC2P ; //����������� ������ �� ����� ��������
		}
	else
			{
			Period = TIM3->CCR2 - capture1;
			TIM3->CNT = 0; // ������� �������
			Period_Flag = 0; // ���� ��� ��������� ������� - ������ �������
			}
}

int main(void)
{
unsigned long int delay_cnt; // ���������� ��� ������������ ��������

// ��������� UART
RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; // ��������� �������� ��������� GPIOD
GPIOD->MODER |= GPIO_MODER_MODER8_1 | GPIO_MODER_MODER9_1; // ����������� ����� PD8 � PD9 �� �������������� �������
/*��� ���������������� ����8 � ����� USART3_TX � ����9 � ����� USART3_RX ����� � �������� GPIOD->AFR[1] ������� ���������� AF7 (���.65 datasheet STM32F04xx)
��� ����� � ������� GPIOD->AFR[1] ���������� �������� 0�7 � 0�70 ��������������*/
GPIOD->AFR[1] |= GPIO_AFRH_AF7_UART3_TX;
GPIOD->AFR[1] |= GPIO_AFRH_AF7_UART3_RX;
RCC->APB1ENR |= RCC_APB1ENR_USART3EN; // ��������� �������� ��������� USART3
USART3->CR1 |= USART_CR1_TE; // �������� ����������
USART3->CR1 |= USART_CR1_RE; // �������� ��������
/*��� ������ �� ��������� ��������� ����������� (8N1 - 8 ���, ��� ��������, 1 ���� ���)*/
/* ������ ������������� ��� ��������� ��������: �������� ������� 16 ���. ��������� �������� 57600 ���/�.
USARTDIV = �������� �������/(16 * ��������� ��������) = 16000000/(16*57600) = 17.36
"DIV_Mantissa" = 17 = 0x21; "DIV_Fraction" = 16*0.36 = 5.76; ��������� �� ���������� ������ - 6 = 0x06*/
USART3->BRR = (17 << 4) + 6; // �������� 57600 ���/�.
USART3->CR1 |= USART_CR1_UE; // �������� USART
UART3_TX_string("Izmerenie Perioda i Dlitelnost Impulsa:\n\r");

// ��������� ������ PD12 �� �������������� ������� ������ ���������� TIM4_CH1
GPIOD->MODER |= GPIO_MODER_MODER12_1;
/* ��� ���������������� ����12 � ����� ������ TIM4_CH1, ����� � �������� GPIOD->AFR[1] ������� ���������� AF2
��� ����� � ������� GPIOD->AFR[1] ���������� �������� 0�20000*/
GPIOD->AFR[1] |= GPIO_AFRH_PIN12_AF2;

// ���� ����� ������� - ��� PA7 (TIM3_CH2)
RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; // �������� �������� ��������� ����� A
GPIOA->MODER |= GPIO_MODER_MODER7_1; // �� �������������� �������
 /* ��� ���������������� ����7 � ����� ����� TIM3_CH2, ����� � �������� GPIOD->AFR[0] ������� ���������� AF2
��� ����� � ������� GPIOD->AFR[0] ���������� �������� 0�20000000*/
GPIOA->AFR[0] |= GPIO_AFRL_PIN7_AF2;

// ��������� ������� TIM4
RCC->APB1ENR |= RCC_APB1ENR_TIM4EN; // �������� �������� ��������� TIM4
/* �������� ������� ������� 16000000 �� - ������ 0,0000000625
��� ��������� ������� 0,1 ���� (10 ��� ������ ���) ������� ��������� ���������: (1/16000000�� * 16 * 100 = 0,0001 ���*/
TIM4->PSC = 16-1; // ��������
TIM4->ARR = 200; // �������� ������������ TIM4
// ������/��������� ��� ������ �������� ��� ��������� - �� �����
TIM4->CCMR1 &= ~TIM_CCMR1_CC1S; // �����1 �������� ��� �����
TIM4->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1; //������� ����� PWM mode1 ������1. ���� ������� ������� �������� �� ��������: ��� CNT<CCR1 ������ OC1ref=1, ����� OC1ref=0.
// ���������� ������� ��� ������ ����������� - �������������
TIM4->CCER |= TIM_CCER_CC1E; // �������� ����� ������� �� ����� ��������� �� ����� ��
TIM4->CCR1 = 50; // ������������ ���, � ������ ������ ������� TIM4->CCR1 - ������� ���������
TIM4->CR1 |= TIM_CR1_CEN; // �������� ������ �������

// ��������� ������� TIM3
// ����� �������, CH2, 16 ��� /16 (��������) = 1 ��� (1 ���)
RCC->APB1ENR |= RCC_APB1ENR_TIM3EN; // �������� �������� ��������� TIM3
TIM3->ARR = 0xffff; // �������� ������������ (�������� �� �������� ������� ������) - ������������ �������� ��� ����������� ��������� �����
TIM3->CCMR1 |= TIM_CCMR1_CC2S_0; //���� �����2 �������� � ������ �������, ������ ������� � TI1;
TIM3->CCER &= ~TIM_CCER_CC2P;//�������� ������ �� ��������� ������ ��������
TIM3->CCMR1 &= ~(TIM_CCMR1_IC2F | TIM_CCMR1_IC2PSC); //�� ��������� � �������� �� ����������
TIM3->CCER |= TIM_CCER_CC2E; // ���� �����2 �������� �� ����, �� �������� ������ ����� �������
TIM3->DIER |= TIM_DIER_CC2IE; // �������� ���������� ��� �������/��������� 2 ������
TIM3->CR1 |= TIM_CR1_CEN;  // �������� ������ �������

NVIC_EnableIRQ(TIM3_IRQn); // ��������� NVIC ��� TIM3
Period_Flag = 0; // ���� ��� ��������� �������
__enable_irq(); // �������� ���������� ���������

while(1)
{
// ������� �������� ������� � ������������ ��������
sprintf(msg, "Pulse Width = %d mks, Period = %d mks\r\n", Width/16, (Period/16 - 1)); // 1 ���� ������� 1/16 ���
UART3_TX_string(msg);
for(delay_cnt = 0; delay_cnt < 720000; delay_cnt++) {}; // ��������
}
}