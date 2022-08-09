//������ � UART. ���������� �������
#define stm32f4xx
#include "stm32f4xx.h" // �������� ���������
#include "stdio.h" // �������� ����������� ������� C
#include "stdlib.h"
#include "string.h" // ���������� C ������ �� ��������
#include "ctype.h" // ���������� C ������ � ���������
#include "DELAY_TIM7.h" // ������� ��������
#define GPIO_AFRH_AF7_8 0x00000007
#define GPIO_AFRH_AF7_9 0x00000070
#define FREQ_TIM3 100000 //������� ������� 3, ��

char msg[130]; // ������ ��� ������������ ��������� ��� ������ �� UART
char buffer[100]; // ������ ��� �������� �������� ������
unsigned char ykaz_buffer = 0; // ��������� �� ��������� ������ � �������
unsigned char receive_flag;  //���� �������� ������
char beeper_freq[10];
short freq;
char beeper_length[10];
short length = 0;
short leght_beeper;
unsigned char error_flag; // ���� ������: 1,2,4 - ������ (������ ��� ���������� ���� ������), 0 -
char * ptr; // ���������

// �������� ����� �� UART3
void UART3_PutByte(char c)
{
while(!(USART3->SR & USART_SR_TC)) {}; // ��������� ���������� DR � ������ (���������� ������ ��������)
USART3->DR = c; // �������� ����
}

// �������� ������ ��������, ����������� � RAM �� UART3
void UART3_PutRAMString(char *str)
{
	char c;
	while((c = *(str++))) // ���������� ������� ������ ���������������, ���� �� ��������� ������ ����� ������ "0x00"
		UART3_PutByte(c);
}

//������ � UART �� ����, ��������� ���������� �� USART3, ����� �����
void USART3_IRQHandler(void)
{
unsigned long USART_SR_COPY; // ���������� ��� �������� ����� �������� SR (��� ���������� SR ����� ������������)

USART_SR_COPY = USART3->SR; // ���������� ��� �������� ����� �������� SR
/* �������� ���������� ������ ������*/
if(USART_SR_COPY & USART_SR_ORE) /*Overrun error. ���� ���������������, ����� ����� ������ �� ���������� ����������. ���������� ���������� � ������������, ����� ����������� ������� �������,
	� ����� ����������� ������� ������. �������� �������� ������ ������ � ��������� ��������, �.�. ���� �� ����������� ������ �� �������� ������, ���� ������ ������ �� ������.*/
	{
	sprintf(msg, "Error ORE\r\n"); UART3_PutRAMString(msg);
	}
if(USART_SR_COPY & USART_SR_NE) // Noise error flag. ��� � �����, �������� ����������. ������������ ����� ��� � ORE.
	{
	sprintf(msg, "Error NE\r\n"); UART3_PutRAMString(msg);
	}
if(USART_SR_COPY & USART_SR_FE) // Frame Error. ������ �������� ��� ��������. ��������, ������� ������� � ����������� � � ����� ���� ������������ ������. ���� ����������, ������������ ����� ��� � ORE.
	{
	sprintf(msg, "Error FE\r\n"); UART3_PutRAMString(msg);
	}
if(USART_SR_COPY & USART_SR_PE) //Parity Error-������ �������� ��������. ��������� ����������, �� ��� ���� ����� �������� ��������� ���� ����������� ��� RXNE. ���� ����������.
	{
	sprintf(msg, "Error PE\r\n"); UART3_PutRAMString(msg);
	}
/*��������� �������� ���������� ������ ������*/
USART3->SR &= ~USART_SR_RXNE; // ������� ���� ����������
buffer[ykaz_buffer++] = USART3->DR; // ���������� ������ � ����� � ����������� ���������
if((buffer[ykaz_buffer-2] == 0x0D) && (buffer[ykaz_buffer-1] == 0x0A))
	{
	receive_flag = 1; // 1- ���� �������� ������
	UART3_PutRAMString("get!\r");
	}
}

int main()
{
// ��������� ������ PD12, 13, 14, 15 ��� �����������
//GPIOD->MODER |= GPIO_MODER_MODER12_0 | GPIO_MODER_MODER13_0 | GPIO_MODER_MODER14_0 | GPIO_MODER_MODER15_0; // �� ������� �����*/
// ��������� ������ PD8 � PD9 ��� UART
RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; //��������� ���� IO D
GPIOD->MODER |= GPIO_MODER_MODER8_1; // ����������� ����� �� �������������� �������
GPIOD->AFR[1] |= GPIO_AFRH_AF7_8;
GPIOD->MODER |= GPIO_MODER_MODER9_1; // ����������� ����� �� �������������� �������
GPIOD->AFR[1] |= GPIO_AFRH_AF7_9; // ������� �������������� ������� - USART3_RX
// ��������� UART
RCC->APB1ENR |= RCC_APB1ENR_USART3EN; // ��������� �������� ��������� USART3
USART3->CR1 |= USART_CR1_TE; // �������� ����������
USART3->CR1 |= USART_CR1_RE; // �������� ��������
USART3->BRR = (17 << 4) + 6; //
USART3->CR1 |= USART_CR1_RXNEIE; // ��������� ���������� ��� ������
USART3->CR1 |= USART_CR1_UE; // �������� USART
USART3->SR &= ~USART_SR_RXNE; // ������� ���� ����������
NVIC_EnableIRQ(USART3_IRQn); // ��������� ���������� � ����������� ����������

//��������� ����� PE11 ��� ���������� �������
RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN; // �������� �������� ��������� ����� E
GPIOE->MODER |= GPIO_MODER_MODER11_0; // �������� �� ����� ���� PE11

// ��������� ������� TIM3 ��������� ����� ������
RCC->APB1ENR |= RCC_APB1ENR_TIM3EN; // ������ ������������ �� TIM3
TIM3->PSC = 160 - 1; // �������� ������������ (�� ���� TIM3 16000/160=100���)
TIM3->ARR = 0; // ���������� TIM3 ��� � 0,01 ��
 TIM3->DIER |= TIM_DIER_UIE; // �������� ���������� ��� ���������� ������� TIM3
//TIM3->CR1 |= TIM_CR1_CEN; // �������� ������ ������� TIM3
NVIC_EnableIRQ(TIM3_IRQn); // ��������� ���������� � ����������� NVIC

// ��������� ������� TIM4 ������������ �������� ������
RCC->APB1ENR |= RCC_APB1ENR_TIM4EN; // ������� ������������ �� TIM4
// ������������ ���������� TIM4 ����� 1 ���� (1 ���)
//�������� �������=16000000; 1000=16000000/(������������)*(�������� ���������)=>(������������)*(�������� ���������)=16000
TIM4->PSC = 160 - 1; // �������� ������������ (�� ���� TIM4 16000/160=100���)
TIM4->ARR = 100; // ���������� TIM4 ��� � 1 ��
TIM4->DIER |= TIM_DIER_UIE; // �������� ���������� ��� ���������� ������� TIM4
//TIM4->CR1 |= TIM_CR1_CEN; // �������� ������ ������� TIM4
NVIC_EnableIRQ(TIM4_IRQn); // ��������� ���������� � ����������� NVIC

__enable_irq(); // �������� ���������� ���������

UART3_PutRAMString("HELLO!:\r\n");
UART3_PutRAMString("Vvedite chastoty v Hz i dlitelnost zvyka v sekundax:\r\n");

while(1)
	{
	if (receive_flag)
		{
		// �������������� ������ (�������)
		memset(beeper_freq, '\0', sizeof(beeper_freq)); // ������� ��������
		/*�-� memset �������� ������� ���� ������� ch � ������ count �������� ������� buf. ���������� ��������� �� ������.
		� ������ ������ ������������, ����� �������� � ������ ����*/
		memset(beeper_length, '\0', sizeof(beeper_length));

		//��������� �������. ���������� � ������ �������
		ptr = strtok(buffer, " _,."); // ������� ��������� �� 1 �������
		/*�-� strtok ���������� ��������� �� ��������� ������� � ������ str1. �������, ���������� ������ str2, �������� �������������,
		������������� �������. ���� ������� �� ���������� ������������ ����. ���� ����� ������ ������. �� �� ���������� \0.
		���� �-� strtok ������� �� ����, ����� ���������� ��������� ������ str �� �����, ��� ���� �������� ���������� �-� strtok,
		�� ������ ��������� �� ����������� ������ � �������� ������� ��������� ����������� \0. �-� strtok ��������� ��������� � ������������ ������.*/
		strcpy(beeper_freq, ptr); // �������� ������� � ������
		/*�-� strcpy �������� ���������� ������ str2 � ������ str1. ��������� str1 ������ ��������� �� �� ������ str2,
		����������� ������� ��������.*/
		for(unsigned char i=0; i < strlen(beeper_freq); i++) //�-� strlen ���������� ����� ������ ��� ����� ������� /0
			beeper_freq[i] = tolower(beeper_freq[i]); //��������� ������� � ������ �������
		freq = atoi(beeper_freq); // ����������� ����� ���������� �� �������� � �����
		sprintf(msg, "beeper frequency  - %d Hz\r\n", freq); UART3_PutRAMString(msg); //�������� ������

		//���������� � ������ ������������
		ptr = strtok('\0' , " _,."); // ������� ��������� �� 2 �������
		strcpy(beeper_length, ptr); // �������� ������� � ������
		for(unsigned char i=0; i < strlen(beeper_length); i++)
			beeper_length[i] = tolower(beeper_length[i]);// ��������� ������� � ������ �������
		length = atoi(beeper_length); // ����������� ����� ���������� �� �������� � �����
		leght_beeper = length*1000;
		sprintf(msg, "time beeper - %d sek\r\n", length); UART3_PutRAMString(msg); //�������� ������

		TIM3->ARR = (float) FREQ_TIM3 / (freq*2);
		TIM3->CR1 |= TIM_CR1_CEN; // �������� ������ ������� TIM3
		TIM4->CR1 |= TIM_CR1_CEN; // �������� ������ ������� TIM3

		receive_flag = 0; //���� ������ ������ (�������): 1 - �������, 0 - ���
		/*for(unsigned char i=0; i < ykaz_buffer; i++)
			*(buffer + i) = '\0'; //������� �����*/
		memset(buffer, '\0', sizeof(buffer));
		ykaz_buffer = 0; //������������� ��������� �� ������ ������
		}
	}
}

void TIM4_IRQHandler(void)
{
TIM4->SR &= ~TIM_SR_UIF; // ������� ���� ����������
if(leght_beeper) // �������� ������� �������
	{
	leght_beeper--; // ��������� ������� �������
	}
}

// ������� ��������� ���������� TIM3
void TIM3_IRQHandler(void)
{
TIM3->SR &= ~TIM_SR_UIF; // ������� ���� ����������

if(leght_beeper) // �������� ������� �������
	{
	GPIOE->ODR ^= GPIO_ODR_ODR_11; // ����������� ���� ��� ������
	}
else
	{
	GPIOE->ODR &= ~GPIO_ODR_ODR_11; // �������� ���� ��� ������
	}
}
