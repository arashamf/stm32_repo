//������ � UART. ����� ������. �������� ���������� ������ ������.
#define stm32f4xx
#include "stm32f4xx.h" // �������� ���������
#include "stdio.h" // �������� ����������� ������� C
#include "stdlib.h"
#include "string.h" // ���������� C ������ �� ��������
#include "ctype.h" // ���������� C ������ � ���������
#include "DELAY_TIM7.h" // ������� ��������
#define GPIO_AFRH_AF7_8 0x00000007
#define GPIO_AFRH_AF7_9 0x00000070

char msg[130]; // ������ ��� ������������ ��������� ��� ������ �� UART
char buffer[100]; // ������ ��� �������� �������� ������
unsigned char ykaz_buffer = 0; // ��������� �� ��������� ������ � �������
unsigned char receive_flag;  //���� �������� ������
char device_name[10]; // ��� ����������
char device_num_str[10]; // ����� ���������� (������)
unsigned int device_num; // ����� ���������� (�����)
char device_funk[10]; // ������� ����������
unsigned char error_flag; // ���� ������: 1,2,4 - ������ (������ ��� ���������� ���� ������), 0 - ��� ������
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
//delay_ms(2000);
}

int main()
{
// ��������� ������ PD12, 13, 14, 15 ��� �����������
RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; //��������� ���� IO D
GPIOD->MODER |= GPIO_MODER_MODER12_0 | GPIO_MODER_MODER13_0 | GPIO_MODER_MODER14_0 | GPIO_MODER_MODER15_0; // �� ������� �����
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
		// �������������� ������ (�������)
		// ������� ��������
		memset(device_name, '\0', sizeof(device_name));
		/*�-� memset �������� ������� ���� ������� ch � ������ count �������� ������� buf. ���������� ��������� �� ������.
		� ������ ������ ������������, ����� �������� � ������ ����*/
		memset(device_num_str, '\0', sizeof(device_num_str));
		memset(device_funk, '\0', sizeof(device_funk));

		//��������� �������. ���������� � ������ ��������
		ptr = strtok(buffer , "_"); // ������� ��������� �� 1 �������
		/*�-� strtok ���������� ��������� �� ��������� ������� � ������ str1. �������, ���������� ������ str2, �������� �������������,
		������������� �������. ���� ������� �� ���������� ������������ ����. ���� ����� ������ ������. �� �� ���������� \0.
		���� �-� strtok ������� �� ����, ����� ���������� ��������� ������ str �� �����, ��� ���� �������� ���������� �-� strtok,
		�� ������ ��������� �� ����������� ������ � �������� ������� ��������� ����������� \0. �-� strtok ��������� ��������� � ������������ ������.*/
		strcpy(device_name, ptr); // �������� ������� � ������
		/*�-� strcpy �������� ���������� ������ str2 � ������ str1. ��������� str1 ������ ��������� �� �� ������ str2,
		����������� ������� ��������.*/
		for(unsigned char i=0; i < strlen(device_name); i++) //�-� strlen ���������� ����� ������ ��� ����� ������� /0
			device_name[i] = tolower(device_name[i]); //��������� ������� � ������ �������
		sprintf(msg, "device_name - %s\r\n", device_name); UART3_PutRAMString(msg); //�������� ������

		//���������� � ������ ����� ����������
		ptr = strtok('\0' , "_"); // ������� ��������� �� 2 �������
		strcpy(device_num_str, ptr); // �������� ������� � ������
		for(unsigned char i=0; i < strlen(device_num_str); i++)
			device_num_str[i] = tolower(device_num_str[i]);// ��������� ������� � ������ �������
		device_num = atoi(device_num_str); // ����������� ����� ���������� �� �������� � �����
		sprintf(msg, "device_num - %d\r\n", device_num); UART3_PutRAMString(msg); //�������� ������

		//���������� � ������ ������� ����������
		ptr = strtok('\0' , "_"); // ������� ��������� �� 3 �������
		strcpy(device_funk, ptr); // �������� ������� � ������
		for(unsigned char i=0; i < strlen(device_funk); i++)
			device_funk[i] = tolower(device_funk[i]); //��������� ������� � ������ �������
		sprintf(msg, "device_funk - %s\r\n", device_funk); UART3_PutRAMString(msg); //�������� ������
		error_flag = 0; // ���� ������: >0 - ������, 0 - ���

		// ���������� �������� �������
		if(strncmp(device_name, "led", 3) == 0) // �������� 1 �������
			/*�-� strncmp ���������� ������ n �������� �����. ���������� "0", ���� ������ ���������, "������ 0" -
			 * ���� ������1 ������ �����2, "������ 0" - ���� ������1 ������ ������ 2*/
			{
			if(strncmp(device_funk, "on", 2) == 0) // �������� 3 �������
				{
				switch(device_num) // �������� 2 �������
					{
					case 1: GPIOD->BSRRL |= GPIO_BSRR_BS_12;
							break; //�������� 1
					case 2: GPIOD->BSRRL |= GPIO_BSRR_BS_13;
							break; //�������� 2
					case 3: GPIOD->BSRRL |= GPIO_BSRR_BS_14;
							break; //�������� 3
					case 4: GPIOD->BSRRL |= GPIO_BSRR_BS_15;
							break; //�������� 4
					default:error_flag += 2; // ���� ������: >0 - ������, 0 - ���
							break;
					}
				}
			else
				if(strncmp(device_funk, "off", 3) == 0) // �������� 3 �������
					{
					switch(device_num) // �������� 2 �������
						{
						case 1: GPIOD->BSRRH |= GPIO_BSRR_BS_12;
								break; //��������� 1
						case 2: GPIOD->BSRRH |= GPIO_BSRR_BS_13;
								break; //��������� 2
						case 3: GPIOD->BSRRH |= GPIO_BSRR_BS_14;
								break; //��������� 3
						case 4: GPIOD->BSRRH |= GPIO_BSRR_BS_15;
								break; //��������� 4
						default:error_flag += 2; // ���� ������: >0 - ������, 0 - ���
								break;
						}
					}
				else
					{
					error_flag += 4; // ���� ������: >0 - ������, 0 - ���
					}
				}
		else
			{
			error_flag += 1; // ���� ������: >0 - ������, 0 - ���
			}
	if(error_flag)
		{
		sprintf(msg, "Error %d\r\n", error_flag); UART3_PutRAMString(msg); // ������ ������
		}
	receive_flag = 0; //���� ������ ������ (�������): 1 - �������, 0 - ���
	for(unsigned char i=0; i < ykaz_buffer; i++)
		*(buffer + i) = '\0'; //������� �����
	ykaz_buffer = 0; //������������� ��������� �� ������ ������
		}
	}
}
