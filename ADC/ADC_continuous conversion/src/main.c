
 // ������ � ���.  ����������� ��������������.
#define stm32f4xx
#include "stm32f4xx.h" // �������� ���������
#include "stdio.h" // �������� ����������� �������
#include "string.h" // ���������� C ������ �� ��������
#include "DELAY_TIM7.h" // ������� ��������
#include "UART_STM32.h" // ���������� ������� ��� ������ � USART3

typedef unsigned char byte;

// ����������� ������ ��� ��
#define BANDWITH 57600 //���������� ����������� ����
#define APB1_FREQUENCY 16000000 // ������� ���� APB1

char msg[100]; // ������ ��� ������������ ��������� ��� ������ �� UART
char buffer [50]; //������ ��� ���������� ������ ���������� �� UART

__I float Vref = 3.3; // ������� ���������� ��� ���; __I = volatile const
__IO float ADC_float = 0; // ��������� ��� � ���� ����������; __IO = volatile
__IO byte ADC_count = 0; //������� ���������� ��������������

#define GPIO_AFRH_PIN13_AF2 0x200000 //����. ����� AF2 ���� 13
#define COMMAND_ADC "start" //������� ������� ���
__IO byte receive_flag = 0; //���� ��������� ������ �� UART
__IO byte ykaz_buffer = 0; //��������� �� ������ UART

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
if((buffer[ykaz_buffer-2] == 0x0D) && (buffer[ykaz_buffer-1] == 0x0A)) //�������� ��������� ���������� ������
	{
	receive_flag = 1; // 1- ���� �������� ������
	UART3_PutRAMString("get!\r");
	}
}

int main()
{

// ��������� ����� � ��� ���
SET_BIT(RCC->AHB1ENR, RCC_AHB1ENR_GPIOCEN); // ��������� �������� ��������� GPIOC
SET_BIT(GPIOC->MODER, GPIO_MODER_MODER1_1 | GPIO_MODER_MODER1_0); // ����������� ����� PC1 �� ���������� �����

//��������� UART
UART3_init_IRQ(APB1_FREQUENCY, BANDWITH); // ������������� UART3
UART3_PutRAMString("ADC continuous conversion:\r\n");

// ��������� ���
SET_BIT(RCC->APB2ENR, RCC_APB2ENR_ADC1EN); // �������� �������� ���������
/*������� SQx[4:0] ������ ����� ������, ��� � � ��� ����� ��������������.*/
CLEAR_BIT(ADC1->SQR1, ADC_SQR1_L); //������� ���������� ������� ���������� ������: 1
SET_BIT(ADC1->SQR3, ADC_SQR3_SQ1_0 | ADC_SQR3_SQ1_1| ADC_SQR3_SQ1_3); // ����� 11 (PC1)
SET_BIT(ADC1->CR2, ADC_CR2_ADON); // �������� ���

__enable_irq(); // �������� ���������� ���������

while(1)
	{
		if (receive_flag)
			{
		if (!strncmp (buffer,  COMMAND_ADC, 5)) //�������� ������������ �������
			{
			SET_BIT(ADC1->CR2, ADC_CR2_CONT); // ������� ����������� ��������������
			ADC1->CR2 |= ADC_CR2_SWSTART; // ����������� ������ ���
			UART3_PutRAMString ("ADC conversion enabled\n\r");
			for (ADC_count = 0; ADC_count <=10; ADC_count++)
				{
				ADC_float = Vref*ADC1->DR/4096; // ������� ����������/������������ ����� ��������� * ��������� ��������������
				sprintf(msg,("ADC_count = %d, ADC_result 12 = %f V\r\n") ,ADC_count, ADC_float); // ������� �������� ��� � ������
				UART3_PutRAMString (msg); // ������� ������ �� UART
				TIM4->CCR2 = ADC1->DR/4.096; // ����� ������������ ���
				delay_ms(500); // ��������
				}
			CLEAR_BIT(ADC1->CR2, ADC_CR2_CONT); // �������� ����������� ��������������
			UART3_PutRAMString("ADC conversion disabled\r\n");
			}
		else
			{
			UART3_PutRAMString("ERROR\r\n"); //������������ �������
			UART3_PutRAMString("write 'start'\r\n");
			}
		memset(buffer, '\0', sizeof(buffer)); // ������� �������
		receive_flag = 0;  //�������� ����
		ykaz_buffer = 0; //�������� ���������
		}
	}
}
