// ������ � LCD CHAR.

#define stm32f4xx
#include "stm32f4xx.h" // �������� ���������
#include "stdio.h" // �������� ����������� �������
#include "string.h" // ���������� C ������ �� ��������
#include "LCD_STM32.h"
#define GPIO_AFRH_AF7_8 0x00000007
#define GPIO_AFRH_AF7_9 0x00000070

unsigned char msg[30]; // ������ ��� ������������ ��������� ��� ������ �� UART
char buffer[20]; // ������ ��� �������� �������� ������
unsigned char ykaz_buffer = 0; // ��������� �� ��������� ������ � �������
unsigned char receive_flag = 0;  //���� �������� ������

//������� ������ � UART �� ����, ��������� ���������� �� USART3, ����� �����
void USART3_IRQHandler(void)
{
/*��������� �������� ���������� ������ ������*/
USART3->SR &= ~USART_SR_RXNE; // ������� ���� ����������
buffer[ykaz_buffer++] = USART3->DR; // ���������� ������ � ����� � ����������� ���������
if((buffer[ykaz_buffer-2] == 0x0D) && (buffer[ykaz_buffer-1] == 0x0A))
	receive_flag = 1; // 1- ���� �������� ������
}

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

int main()
{
// ��������� ������ PD8 � PD9 ��� UART
RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; // ��������� �������� ��������� GPIOD
GPIOD->MODER |= GPIO_MODER_MODER8_1; // ����������� ����� �� �������������� �������
GPIOD->AFR[1] |= GPIO_AFRH_AF7_8;  //������� �������������� ������� - USART3_TX
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

//��������� LCD
LCD_STM_INIT (); //�������� ������ ��, ������������ � LCD
LCD_CHAR_init(16); // �������������	16 ���������� ������
LCD_CHAR_gotoxy(0,0); // ��������� ������� ���������� ��� ������
LCD_CHAR_puts("Hello,"); // ������� ������ �� RAM
LCD_CHAR_gotoxy(0,1); // ��������� ������� ���������� ��� ������
LCD_CHAR_puts("UART:"); // ������� ������ �� RAM

UART3_PutRAMString("HELLO!:\r\n");

// �������� ���� ���������
while(1)
	{
	if (receive_flag)
	{
		UART3_PutRAMString(buffer);
		for(unsigned char i = 0; i <= ykaz_buffer; i++)
		if ((*(buffer + i) == '\r') || (*(buffer + i) == '\n'))
				*(buffer + i) = '\0'; //������� �� ������ ������� \r � \n
		LCD_CHAR_wr(0x01, 0); //�������� ������� � ���������� ������ � ��������� �������
		delay_ms(2);
		LCD_CHAR_gotoxy(0,0); //���������� ������ � ������ ������� ������
		LCD_CHAR_puts("get_message:");
		LCD_CHAR_gotoxy(0,1); //���������� ������ � ������ ������ ������
		LCD_CHAR_puts(buffer);
		receive_flag = 0; //���� ������ ������ (�������): 1 - �������, 0 - ���
		memset(buffer, '\0', sizeof(buffer));  //������� ������
		ykaz_buffer = 0; //������������� ��������� �� ������ ������
		}
	}
}
