//������ � UART. ����� ������. �������� ���������� ������ ������.
#define stm32f4xx
#include "stm32f4xx.h" // �������� ���������
#include "stdio.h" // �������� ����������� �������
#include "string.h" // ���������� ������ �� ��������
#include "DELAY_TIM7.h" // ������� ��������
#define GPIO_AFRH_AF7_8 0x00000007
#define GPIO_AFRH_AF7_9 0x00000070
char msg[130]; // ������ ��� ������������ ��������� ��� ������ �� UART
char buffer[100]; // ������ ��� �������� �������� ������
unsigned char ykaz_buffer = 0; // ��������� �� ��������� ������ � �������
unsigned char receive_flag;  //���� �������� ������

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
//delay_ms(2000);
}

int main()
{
// ��������� ����� PD12 ��� ����������
RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; //��������� ���� IO D
GPIOD->MODER |= GPIO_MODER_MODER12_0; // �� ������� �����
// ��������� ������ PD8 � PD9 ��� UART
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
__enable_irq(); // �������� ���������� ���������

UART3_PutRAMString("HELLO!:\r\n");
UART3_PutRAMString("UART_RX_INT:\r\n");

while(1)
{
if (receive_flag)
	{
	for(unsigned char i = 0; i <= ykaz_buffer; i++)
	if ((*(buffer + i) == '\r') || (*(buffer + i) == '\n'))
			*(buffer + i) = '\0'; //������� �� ������ ������� \r � \n
	if(!strcmp(buffer, "led_on"))
		{
		receive_flag = 0; //�������� ���� �������� ������
		for(unsigned char i = 0; i <= ykaz_buffer; i++)
			*(buffer + i) = '\0'; // ������� �����
		ykaz_buffer = 0; // ������������� ��������� � ������ ������
		GPIOD->BSRRL |= GPIO_BSRR_BS_12; // �������� ���������
	}
/*������� strcmp �� ���������� string.h ���������� ������������� ��������, ���� � �������� ����� ������������ ������ sl
������� �� ������� s2, �������� �, ���� ������ ���������, � ������������� ��������, ���� ������ ������ ������������ ������*/
else
	if(!strcmp(buffer, "led_off"))
		{
		receive_flag = 0; //�������� ���� �������� ������
		for(unsigned char i = 0; i <= ykaz_buffer; i++)
			*(buffer+i) = '\0'; // ������� �����
		ykaz_buffer = 0; // ������������� ��������� � ������ ������
		GPIOD->BSRRH |= GPIO_BSRR_BS_12;  // ��������� ���������
		}
	else
		{
//			sprintf(msg,"Error\r\n");
		UART3_PutRAMString("error of command!\r\n");
		receive_flag = 0; //�������� ���� �������� ������
		for(unsigned char i = 0; i <= ykaz_buffer; i++)
			*(buffer+i) = '\0'; // ������� �����
		ykaz_buffer = 0; // ������������� ��������� � ������ ������
		}
	}
}
}
