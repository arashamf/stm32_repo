#define stm32f4xx
#include "stm32f4xx.h" // �������� ���������
#include "stdio.h" // �������� ����������� �������
#include "UART_STM32.h" 

#define GPIO_AFRH_AF7_8 0x00000007 //��������� AF7 ��� ����8
#define GPIO_AFRH_AF7_9 0x00000070  //��������� AF7��� ����9
#define GPIO_AFRL_AF7_2 0x700   //��������� AF7 ��� ����2
#define GPIO_AFRL_AF7_3 0x7000   //��������� AF7 ��� ����3

char buffer[100]; // ������ ��� �������� �������� ������
char msg[100]; // ������ ��� ������������ ��������� ��� ������ �� UART
volatile unsigned char ykaz_buffer; // ��������� �� ��������� ������ � �������
volatile unsigned char receive_flag;  //���� �������� ������

//������� ������������� UART3
void UART3_init (unsigned int freq, unsigned int bandwith)
{
//��������� ������ PD8 � PD9��� UART3
RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; // ��������� �������� ��������� GPIOD
GPIOD->MODER |= GPIO_MODER_MODER8_1; // ����������� ����� �� �������������� �������
GPIOD->AFR[1] |= GPIO_AFRH_AF7_8; // ������� ���������� �������������� ������� - USART3_TX
GPIOD->MODER |= GPIO_MODER_MODER9_1; // ����������� ����� �� �������������� �������
GPIOD->AFR[1] |= GPIO_AFRH_AF7_9; // ������� ���������� �������������� ������� - USART3_RX
// ��������� UART
RCC->APB1ENR |= RCC_APB1ENR_USART3EN; // ��������� �������� ��������� USART3
USART3->CR1 |= USART_CR1_TE; // �������� ����������
USART3->CR1 |= USART_CR1_RE; // �������� ��������
/* USART_BRR = (fck + baudrate /2 ) / baudrate
������ ������������� ��� ��������� ��������: �������� ������� 16 ���. ��������� �������� 57600 ���/�.
USARTDIV = �������� �������/(16 * ��������� ��������) = 16000000/(16*57600) = 17.36 
"DIV_Mantissa" = 17; "DIV_Fraction" = 16*0.36 = 5.76; ��������� �� ���������� ������ - 6 = 0x06
������ ������: USART_BRR=(fck + baudrate/2)/baudrate=278=0�116*/
//USART3->BRR = (17 << 4) + 6; //
USART3->BRR = koeff_brr (freq, bandwith);
USART3->CR1 |= USART_CR1_UE; // �������� UART3
}


//������� ������������� UART3 � �����������
void UART3_init_IRQ (unsigned int freq, unsigned int bandwith)
{
// ��������� ������ PD8 � PD9 ��� UART3
RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; //��������� �������� ��������� ����� D
GPIOD->MODER |= GPIO_MODER_MODER8_1; // ����������� ����� �� �������������� �������
GPIOD->AFR[1] |= GPIO_AFRH_AF7_8; // ������� �������������� ������� - USART3_TX
GPIOD->MODER |= GPIO_MODER_MODER9_1; // ����������� ����� �� �������������� �������
GPIOD->AFR[1] |= GPIO_AFRH_AF7_9; // ������� �������������� ������� - USART3_RX
// ��������� UART � �����������
RCC->APB1ENR |= RCC_APB1ENR_USART3EN; // ��������� �������� ��������� USART3
USART3->CR1 |= USART_CR1_TE; // �������� ����������
USART3->CR1 |= USART_CR1_RE; // �������� ��������
//USART3->BRR = (17 << 4) + 6; //������ �������� �������� USART3->BRR
USART3->BRR = koeff_brr (freq, bandwith);
USART3->CR1 |= USART_CR1_RXNEIE; // ��������� ���������� ��� ������
USART3->CR1 |= USART_CR1_UE; // �������� UART3
USART3->SR &= ~USART_SR_RXNE; // ������� ���� ����������
NVIC_EnableIRQ(USART3_IRQn); // ��������� ���������� � ����������� ����������
}

//�-� ��������� ���������� �� UART3, ����� �����
void USART3_IRQHandler(void)
{
unsigned long USART_SR_COPY; // ���������� ��� �������� ����� �������� SR (��� ���������� SR ����� ������������)

USART_SR_COPY = USART3->SR; // ���������� ��� �������� ����� �������� SR
// �������� ���������� ������ ������
if(USART_SR_COPY & USART_SR_ORE) //Overrun error. ���� ���������������, ����� ����� ������ �� ���������� ����������. ���������� ���������� � ������������, ����� ����������� ������� �������,
//	� ����� ����������� ������� ������. �������� �������� ������ ������ � ��������� ��������, �.�. ���� �� ����������� ������ �� �������� ������, ���� ������ ������ �� ������.
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
//��������� �������� ���������� ������ ������//
USART3->SR &= ~USART_SR_RXNE; // ������� ���� ����������
buffer[ykaz_buffer++] = USART3->DR; // ���������� ������ � ����� � ����������� ���������
if((buffer[ykaz_buffer-2] == 0x0D) && (buffer[ykaz_buffer-1] == 0x0A))
	receive_flag = 1; // 1- ���� �������� ������
}

// �-� �������� ����� �� UART3
void UART3_PutByte(char c)
{
while(!(USART3->SR & USART_SR_TC)) {}; // ��������� ���������� DR � ������ (���������� ������ ��������)
USART3->DR = c; // �������� ����
}

//�-� �������� ������ �������� �� UART3
void UART3_PutRAMString(char *str)
{
	char c;
	while((c = *str++)) // ���������� ������� ������ ���������������, ���� �� ��������� ������ ����� ������ "\0"
		UART3_PutByte(c);
}

//������� ������������� UART2
void UART2_init (unsigned int freq, unsigned int bandwith)
{
//��������� ������ PD2 � PD3��� UART2
SET_BIT (RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN); //����������� ���� �
SET_BIT (GPIOA->MODER, GPIO_MODER_MODER2_1); // ����������� ����� �� �������������� �������
GPIOA->AFR[0] |= GPIO_AFRL_AF7_2; // ������� ���������� �������������� ������� - USART2_TX
SET_BIT (GPIOA->MODER, GPIO_MODER_MODER3_1); // ����������� ����� �� �������������� �������
GPIOA->AFR[0] |= GPIO_AFRL_AF7_3; // ������� ���������� �������������� ������� - USART2_RX
// ��������� UART
SET_BIT (RCC->APB1ENR, RCC_APB1ENR_USART2EN); // ��������� �������� ��������� USART2
SET_BIT (USART2->CR1, USART_CR1_TE); // �������� ����������
SET_BIT (USART2->CR1, USART_CR1_RE); // �������� ��������
/*  WRITE_REG (USART_BRR, (fck + baudrate /2 ) / baudrate);
������ ������������� ��� ��������� ��������: �������� ������� 16 ���. ��������� �������� 57600 ���/�.
USARTDIV = �������� �������/(16 * ��������� ��������) = 16000000/(16*57600) = 17.36
"DIV_Mantissa" = 17; "DIV_Fraction" = 16*0.36 = 5.76; ��������� �� ���������� ������ - 6 = 0x06
������ ������: USART_BRR=(fck + baudrate/2)/baudrate=278=0�116*/
//USART3->BRR = (17 << 4) + 6; //
WRITE_REG (USART2->BRR, koeff_brr (freq, bandwith));
SET_BIT (USART2->CR1, USART_CR1_UE); // �������� UART2
}

// �-� �������� ����� �� UART2
void UART2_PutByte(char c)
{
while(!(USART2->SR & USART_SR_TC)) {}; // ��������� ���������� DR � ������ (���������� ������ ��������)
USART2->DR = c; // �������� ����
}

//�-� �������� ������ �������� �� UART2
void UART2_PutRAMString(char *str)
{
	char c;
	while((c = *str++)) // ���������� ������� ������ ���������������, ���� �� ��������� ������ ����� ������ "\0"
		UART2_PutByte(c);
}

//�-� �������  �������� �������� USART_BRR
int koeff_brr (unsigned int freq, unsigned int bandwith)
{
int usart_brr = 0;
return usart_brr = (freq + (bandwith/2))/bandwith;
}
