//��������� ������ ������ �� UART � ��������� �����

#define stm32f4xx
#include "stm32f4xx.h" // �������� ���������
#include "stdio.h" // �������� ����������� ������� �
#define GPIO_AFRH_AF7_8 0x00000007
#define GPIO_AFRH_AF7_9 0x00000070

//������� �������� ����� �� uart3
void UART3_PutByte(char c)
{
while(!(USART3->SR & USART_SR_TC)) {}; // ��������� ���������� DR � ������
USART3->DR = c; // �������� ����
}

//������� �������� ������ ��������, ����������� � RAM �� uart3
void UART3_PutRAMString(char *str)
{
char c;
while((c = *str++))		// ���������� ������� ������ ���������������, ���� �� ��������� ������ ����� ������ "0x00"
	UART3_PutByte(c);
}

char msg[130]; // ������ ��� ������������ ��������� ��� ������ �� UART

int main()
{
// ��������� ����� D
RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; //��������� �������� ��������� GPIOD
GPIOD->MODER |= GPIO_MODER_MODER8_1; //����������� ����� 8 �� �������������� �������
GPIOD->AFR[1] |= GPIO_AFRH_AF7_8; //������� ���������� �������������� ������� - USART3_TX
GPIOD->MODER |= GPIO_MODER_MODER9_1; // ����������� ����� 9 �� �������������� �������
GPIOD->AFR[1] |= GPIO_AFRH_AF7_9; //������� ���������� �������������� ������� - USART3_RX

// ��������� UART
RCC->APB1ENR |= RCC_APB1ENR_USART3EN; // ��������� �������� ��������� USART3
USART3->CR1 |= USART_CR1_TE | USART_CR1_RE; // �������� ���������� � �������
/* ������ ������������� ��� ��������� ��������: �������� ������� 16 ���. ��������� �������� 57600 ���/�.
USARTDIV = �������� �������/(16 * ��������� ��������) = 16000000/(16*57600) = 17.36
"DIV_Mantissa" = 17 = 0x21; "DIV_Fraction" = 16*0.36 = 5.76; ��������� �� ���������� ������ - 6 = 0x06*/
USART3->BRR = (17 << 4) + 6; //
USART3->CR1 |= USART_CR1_UE; // �������� USART
UART3_PutRAMString("HELLO!\r");
UART3_PutRAMString("UART_RX:\r\n");

while(1)
	{
	// ���� ������ ������ ��������� ��� ���������� �������� �USART3->DR�, ���� ������� �0� � ��� RXNE �������� USART_SR
	if(USART3->SR & USART_SR_RXNE) // �������� ����� ������ �����: 1 - ������, 0 - �� ������
		{
		sprintf(msg,"Byte = %c\r\n", USART3->DR);
		UART3_PutRAMString(msg);
		}
	}
}
