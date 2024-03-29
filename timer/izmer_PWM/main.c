#define stm32f4xx
#include "stm32f4xx.h" // �������� ���������
#include "stdio.h" // �������� ����������� �������
#include "string.h" // ���������� ������ �� ��������
#define CPU_CLOCK 1600000L // ������� ����������
#define APB1_CLOCK 16000000L // ������� ���� APB1
#define APB2_CLOCK 16000000L // ������� ���� APB2
#define GPIO_AFRH_AF7_UART3_TX 0x7
#define GPIO_AFRH_AF7_UART3_RX 0x70
#include "DELAY_TIM7.h" // ������� ��������
//#include "UART3_drv.h" // ���������� ������� ��� ������ � USART3
char msg[130]; // ������ ��� ������������ ��������� ��� ������ �� UART
volatile unsigned int Period; // ��������� ������
volatile unsigned int Dlit; // ���������� ������������
volatile unsigned long int measure_good = 0; // ����: >= 2 - ��������� ������, 0 - ���

/* ������� �������� ����� ������ �� UART*/
void UART3_TX_byte(char byte)
{
while(!(USART3->SR & USART_SR_TC)) {}; //USART_SR - ������� ������� usart, 6 ��� ���������������, ���� �������� ����� ��������� � ����� TXE ����������.
USART3->DR = byte; // �������� ����
}

/*������� �������� ������ ������ �� UART*/
void UART3_TX_string(char *str)
{
while((*str) != '\0') // ��������� �� ��������� ������
	{
	UART3_TX_byte(*str); // ������� ������� ����
	str++; //��������� �� ��������� ������
	}
}

int main(void)
{
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
/*��� ������ �� ��������� ��������� ����������� 8N1 - 8 ���, ��� ��������, 1 ���� ���*/
/* ������ ������������� ��� ��������� ��������: �������� ������� 16 ���. ��������� �������� 57600 ���/�.
USARTDIV = �������� �������/(16 * ��������� ��������) = 16000000/(16*57600) = 17.36
"DIV_Mantissa" = 17 = 0x21; "DIV_Fraction" = 16*0.36 = 5.76; ��������� �� ���������� ������ - 6 = 0x06*/
USART3->BRR = (17 << 4) + 6; // �������� 57600 ���/�.
USART3->CR1 |= USART_CR1_UE; // �������� USART
UART3_TX_string("Izmerenie Perioda i Dlitelnost Impulsa:\n\r");

// ��������� ����� D - ����� ����
//RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; // �������� �������� ��������� ����� D
GPIOD->MODER |= GPIO_MODER_MODER12_1; //��������� ����� �� �������������� ������� TIM4, ����� ���������� - PD12
GPIOD->AFR[1] |= 0x00020000; // TIM4 - �������������� ������� AF2 ��� ���� 12, 0010 � 16-19 ����

RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN; // �������� �������� ��������� ����� �
GPIOC->MODER |= GPIO_MODER_MODER6_1;//��������� ����� �� �������������� ������� TIM3, ����� ���������� - PC6
GPIOC->AFR[0] |= 0x2000000; // TIM3 - �������������� ������� AF2 ��� ���� 6 ����� �, 0010 � 24-27 ���

// ��������� ���������� ��� �� TIM4
RCC->APB1ENR |= RCC_APB1ENR_TIM4EN; // �������� �������� ��������� TIM4
// �������� ������� ������� 16000000 �� - ������ 0,0000000625
// ��� ��������� ������� 0,1 ���� (10 ��� ������� ���) ������� ��������� ���������: 1/16000000�� * 8 * 200 = 0,0001 ���
TIM4->PSC = 8-1; // ��������
TIM4->ARR = 200; // �������� ������������ TIM4
TIM4->CCMR1 &= ~TIM_CCMR1_CC1S; // �����1 �������� ��� �����
TIM4->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1; // ������� ����� ��� - 1
// ���������� ������� ��� ������ ����������� - �������������
TIM4->CCER |= TIM_CCER_CC1E; // �������� ����� ������� �� ����� ��������� �� ����� ��
TIM4->CCR1 = 50; // ������������ ���
TIM4->CR1 |= TIM_CR1_CEN; // �������� ������ �������

// ��������� ���������� ��� �� TIM3
RCC->APB1ENR |= RCC_APB1ENR_TIM3EN; // �������� �������� ��������� TIM3
// �������� ������� ������� 16000000 �� - ������=0,0000000625 �
// ��� ��������� ������� 125 ����� (8 ��� ������� ���) ������� ��������� ���������: 1/16000000�� * 8 * 250 = 0,000125 ���
TIM3->PSC = 8-1; // ��������
TIM3->ARR = 250; // �������� ������������ TIM3
TIM3->CCMR1 &= ~TIM_CCMR1_CC1S; // �����1 �������� ��� �����
TIM3->CCMR1 |= TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1; // ������� ����� ��� - 1
// ���������� ������� ��� ������ ����������� - �������������
TIM3->CCER |= TIM_CCER_CC1E; // �������� ����� ������� �� ����� ��������� �� ����� ��
TIM3->CCR1 = 51; // ������������ ���
TIM3->CR1 |= TIM_CR1_CEN; // �������� ������ �������

// ���� ����� ������� - PA0 � PA5 - TIM2 � TIM5
RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN;// �������� �������� ��������� ����� A
GPIOA->MODER |= GPIO_MODER_MODER5_1; // �� �������������� �������
GPIOA->AFR[0] |= 0x100000; // TIM2 - �������������� ������� AF1 ��� ���� 5
GPIOA->MODER |= GPIO_MODER_MODER0_1; // �� �������������� �������
GPIOA->AFR[0] |= 0x2; // TIM5 - �������������� ������� AF2 ��� ���� 0
//GPIOA->AFR[0] |= 0x1; // TIM2 - �������������� ������� AF1 ��� ���� 5, 0001 � 0-3 ���

// ��������� ������� TIM2 - ���������� ���
RCC->APB1ENR |= RCC_APB1ENR_TIM2EN; // �������� �������� ��������� TIM2
TIM2->ARR = 0xffffffff; // �������� ������������ - ������������ �������� ��� ����������� ��������� �����
TIM2->CCMR1 |= TIM_CCMR1_CC1S_0; // //���� �����1 �������� � ������ �������, ������ ������� � TI1;
TIM2->CCMR1 |= TIM_CCMR1_CC2S_1; // //���� �����2 �������� � ������ �������, ������ ������� � TI1;
TIM2->CCER &= ~(TIM_CCER_CC1P | TIM_CCER_CC1NP); // �������� ������ ������ 1 �� ��������� ������ ��������
TIM2->CCER &= ~TIM_CCER_CC2NP; //�������� ������ ������ 2 �� ���������� ������
TIM2->CCER |= TIM_CCER_CC2P; //��� ����� � ��� 5 ���������� 1, � ��� 7 - 0
TIM2->SMCR |= TIM_SMCR_TS_2 | TIM_SMCR_TS_0; // ������� ������� "Filtered Timer Input 1"- ������ TI1 (����� ��������� �������); TIMx_SMCR - ������� ���������� ����������� ������� �������;
TIM2->SMCR |= TIM_SMCR_SMS_2; // ������� ����� �������� - Reset Mode, ����� ������ - ����������� ����� ������� �� ��������� ���������� ����� (TRGI) ���������� ������� � ��������� �������� (100)
TIM2->DIER |= TIM_DIER_CC1IE; // �������� ���������� ��� �������/��������� 1 ������
TIM2->DIER |= TIM_DIER_CC2IE; // �������� ���������� ��� �������/��������� 2 ������
TIM2->CCER |= TIM_CCER_CC1E; // �������� ������ ����� ������� 1
TIM2->CCER |= TIM_CCER_CC2E; // �������� ������ ����� ������� 2
TIM2->CR1 |= TIM_CR1_CEN; // �������� ������ �������
NVIC_EnableIRQ(TIM2_IRQn); // ��������� NVIC ��� TIM2

// ��������� ������� TIM5 - ���������� ���
RCC->APB1ENR |= RCC_APB1ENR_TIM5EN; // �������� �������� ��������� TIM5
TIM5->ARR = 0xffffffff; // �������� ������������ - ������������ �������� ��� ����������� ��������� �����
TIM5->CCMR1 |= TIM_CCMR1_CC1S_0; // //���� �����1 �������� � ������ �������, ������ ������� � TI1;
TIM5->CCMR1 |= TIM_CCMR1_CC2S_1; // //���� �����2 �������� � ������ �������, ������ ������� � TI1;
TIM5->CCER &= ~(TIM_CCER_CC1P | TIM_CCER_CC1NP); // �������� ������ ������ 1 �� ��������� ������ ��������
TIM5->CCER &= ~TIM_CCER_CC2NP; //�������� ������ ������ 2 �� ���������� ������
TIM5->CCER |= TIM_CCER_CC2P; //��� ����� � ��� 5 ���������� 1, � ��� 7 - 0
TIM5->SMCR |= TIM_SMCR_TS_2 | TIM_SMCR_TS_0; // ������� ������� "Filtered Timer Input 1"- ������ TI1 (����� ��������� �������); TIMx_SMCR - ������� ���������� ����������� ������� �������;
TIM5->SMCR |= TIM_SMCR_SMS_2; // ������� ����� �������� - Reset Mode, ����� ������ - ����������� ����� ������� �� ��������� ���������� ����� (TRGI) ���������� ������� � ��������� �������� (100)
TIM5->DIER |= TIM_DIER_CC1IE; // �������� ���������� ��� �������/��������� 1 ������
TIM5->DIER |= TIM_DIER_CC2IE; // �������� ���������� ��� �������/��������� 2 ������
TIM5->CCER |= TIM_CCER_CC1E; // �������� ������ ����� ������� 1
TIM5->CCER |= TIM_CCER_CC2E; // �������� ������ ����� ������� 2
TIM5->CR1 |= TIM_CR1_CEN; // �������� ������ �������
NVIC_EnableIRQ(TIM5_IRQn); // ��������� NVIC ��� TIM5


// �������� ���� ���������
while(1)
{
if(measure_good > 2) // ���� ��������� ��������������
	{
	sprintf(msg, "Period = %d mks, Length = %d mks\r\n", (Period/16), (Dlit/16)); // 1 ���� ������� 1/16 ���
	UART3_TX_string(msg);
	delay_ms(1000);
	}
}
}

// ��������� ���������� �� TIM2
void TIM2_IRQHandler(void)
{
// ��������, ��� �������� ������ ����
if(TIM2->SR & TIM_SR_CC1IF)
	{
	TIM2->SR &= ~TIM_SR_CC1IF; // ������� ���� ����������
	Period = TIM2->CCR1; // ��������� ������
	// ����������� - ��������� ������� ������
	measure_good++; // ����: 2 >= ��������� ������, 0 - ���
	}
if(TIM2->SR & TIM_SR_CC2IF)
	{
	TIM2->SR &= ~TIM_SR_CC2IF; // ������� ���� ����������
	Dlit = TIM2->CCR2; // ���������� ������������
	// ����������� - ��������� ������������ ������
	measure_good++; // ����: 2 >= ��������� ������, 0 - ���
	}
}

// ��������� ���������� �� TIM5
void TIM5_IRQHandler(void)
{
// ��������, ��� �������� ������ ����
if(TIM5->SR & TIM_SR_CC1IF)
	{
	TIM5->SR &= ~TIM_SR_CC1IF; // ������� ���� ����������
	Period = TIM5->CCR1; // ��������� ������
	// ����������� - ��������� ������� ������
	measure_good++; // ����: 2 >= ��������� ������, 0 - ���
	}
if(TIM5->SR & TIM_SR_CC2IF)
	{
	TIM5->SR &= ~TIM_SR_CC2IF; // ������� ���� ����������
	Dlit = TIM5->CCR2; // ���������� ������������
	// ����������� - ��������� ������������ ������
	measure_good++; // ����: 2 >= ��������� ������, 0 - ���
	}
}
