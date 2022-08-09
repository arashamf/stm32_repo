//��������� ������ ������ �� UART �� ����������
#define stm32f4xx
#include "stm32f4xx.h" // �������� ���������
#include "stdio.h" // �������� ����������� �������
#define GPIO_AFRH_AF7_8 0x00000007
#define GPIO_AFRH_AF7_9 0x00000070
char msg[130]; // ������ ��� ������������ ��������� ��� ������ �� UART
char buffer[99]; // ������ ��� �������� �������� ������
unsigned char ykaz_buffer = 0; // ��������� �� ��������� ������ � �������

// �������� ����� �� UART3
void UART3_PutByte(char c)
{
while(!(USART3->SR & USART_SR_TC)) {}; // ��������� ���������� DR � ������ (���������� ������ ��������)
USART3->DR = c; // �������� ����
}

// ������� �������� ������ ��������, ����������� � RAM �� UART3
void UART3_PutRAMString(char *str)
{
char c;
// ���������� ������� ������ ���������������, ���� �� ��������� ������ ����� ������ "0x00"
while(c = *str++)
	UART3_PutByte(c);
}

//������ � UART �� ����, ��������� ���������� �� USART3, ����� �����
void USART3_IRQHandler(void)
{
USART3->SR &= ~USART_SR_RXNE; // ������� ���� ����������
GPIOD->BSRRL |= GPIO_BSRR_BS_12; // �������� ���������
buffer [ykaz_buffer++] = USART3->DR;  

if ((buffer [ykaz_buffer-2] == 0x0D) && (buffer [ykaz_buffer-1] == 0x0A)) // 0x0D � 0xOA - ��������� ������
	{
	sprintf(msg,"You say - %s", buffer);
	UART3_PutRAMString(msg);
	for (char i = 0; i <= ykaz_buffer; i++)
		buffer [i] = 0;
	ykaz_buffer = 0;
	}
GPIOD->BSRRH |= GPIO_BSRR_BS_12;  // ��������� ���������
}

int main()
{
// ��������� ����� D
RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; // ��������� �������� ��������� GPIOD
GPIOD->MODER |= GPIO_MODER_MODER8_1 | GPIO_MODER_MODER9_1; // ����������� ����� 8 � 9 �� �������������� �������
GPIOD->AFR[1] |= GPIO_AFRH_AF7_8; // ������� ���� 8 �������������� ������� - USART3_TX
GPIOD->AFR[1] |= GPIO_AFRH_AF7_9; // ������� ���� 9 �������������� ������� - USART3_RX
GPIOD->MODER |= GPIO_MODER_MODER12_0; //��� 12 �� ������� �����
GPIOD->OTYPER &= ~GPIO_OTYPER_OT_12;  //��� 12 �� ����� � ��������� (push-pull)
GPIOD->OSPEEDR &= ~GPIO_OSPEEDER_OSPEEDR12;  //�������� ���� 12 ����� ������
// ��������� UART
RCC->APB1ENR |= RCC_APB1ENR_USART3EN; // ��������� �������� ��������� USART3
USART3->CR1 |= USART_CR1_TE | USART_CR1_RE; // �������� ���������� � �������
USART3->BRR = (17 << 4) + 6; //
USART3->CR1 |= USART_CR1_RXNEIE; // ��������� ���������� ��� ������
USART3->CR1 |= USART_CR1_UE; // �������� USART
USART3->SR &= ~USART_SR_RXNE; // ������� ���� ���������� �� ������ ������
NVIC_EnableIRQ(USART3_IRQn); // ��������� ���������� � ����������� ����������
__enable_irq(); // �������� ���������� ���������

UART3_PutRAMString("HELLO!:\r\n");
UART3_PutRAMString("UART_RX_INT:\r\n");

while(1)
{}

}
