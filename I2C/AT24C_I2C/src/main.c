/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/


#include "stm32f4xx.h"
#include "I2C_ini.h"
#include "MCU_ini.h"
#include "stdio.h"
#include "string.h"
#include "delay_lib.h"

#define EEPROM_ADDRESS 0xA0
#define buffer_SIZE 32

extern RCC_ClocksTypeDef RCC_Clocks;
char msg_UART_TX [buffer_SIZE]; // ����� �������� �� UART
uint8_t I2C_TX_buffer [buffer_SIZE] = 	{0xB2, 0xB3, 0xB4, 0xB5, 0xB6, 0xAC, 0xA2, 0xA3,
										0x14, 0x25, 0x6, 0xA7, 0x1A, 0x3B, 0x23, 0x42,
										0xC6, 0xCC, 0xC2, 0xC3, 0xC4, 0xC5, 0xC6, 0xBC,
										0xE6, 0xEC, 0xE2, 0xE3, 0xE4, 0xE5, 0xE7, 0xEE};
uint8_t I2C_RX_buffer [buffer_SIZE]; //������ ����� �� I2C

//������� �������� ������ �� UART3
void UART3_PutString(char *str)
{
	char c;
	while((c = *str++)) // ��������� ������� ������ ���������������, ���� �� ��������� ������ ����� ������ "\0"
		{
		while (!USART_GetFlagStatus(USART3, USART_FLAG_TC)) {}; //��� ���������� ����� "transmission complete"
		USART_SendData(USART3, c); // �������� ����
		}
}

int main(void)
{
	// ���������� ��� PLL
//	uint8_t RCC_PLLQ = 4; // ������������ PLLQ ��� USB
	uint8_t RCC_PLLM = 8; // ������������ PLL
	uint32_t  RCC_PLLN = 192 ; // ���������� PLL
	uint32_t  RCC_PLLP = 2; // ������������ PLL = 2
	uint8_t CPU_WS = 2; //����� �������� ��� ������ � Flash �������
	uint8_t status = PLL_ini (CPU_WS, RCC_PLLM, RCC_PLLN, RCC_PLLP); //��������� ���������� PLL (������������ �� HSE = 8MHz, SYSCLK = 96MHz, APB1=APB2=24MHz)
//	uint8_t status = PLL_ini_SPL (CPU_WS, RCC_PLLM, RCC_PLLN, RCC_PLLP, RCC_PLLQ);
//	SystemCoreClockUpdate ();  //������� ��������� ������� (SYSCLK/8=12MHz)
	LEDS_ini (); //�������������� LEDs
	UART3_ini (); //�������������� UART2
	UART3_PutString ("I2C programm start\r\n");
	if (status)
		UART3_PutString("PLL ok\r\n");
	RCC_GetClocksFreq (&RCC_Clocks); //���������� ������� SYSCLK, HCLK, PCLK1 � PCLK2
	sprintf (msg_UART_TX, "%lu, %lu, %lu, %lu\r\n", RCC_Clocks.SYSCLK_Frequency, RCC_Clocks.HCLK_Frequency, RCC_Clocks.PCLK1_Frequency, RCC_Clocks.PCLK2_Frequency);
	UART3_PutString(msg_UART_TX);
	I2C1_init(); // ������������� I2C1
//	I2C1_init_SPL ();

//	I2C1_write_byte(EEPROM_ADDRESS, 0x1F3F, 0xCB);
/*	I2C1_write_array(EEPROM_ADDRESS, 0, I2C_TX_buffer, buffer_SIZE); //������� ������ � ������ �������� ������ EEPROM
	delay_ms(5); //�������� ����� ������ �� ����� 4 ��
	uint8_t number = 0xA;
	for (uint8_t count = 0; count < buffer_SIZE; count++)
		{
		I2C_TX_buffer [count] = number++;
		}
	I2C1_write_array(EEPROM_ADDRESS, 0x20, I2C_TX_buffer, buffer_SIZE);
	delay_ms(5); //�������� ����� ������ �� ����� 4 ��*/

	while (1)
	{
	RCC_GetClocksFreq (&RCC_Clocks); //���������� ������� SYSCLK, HCLK, PCLK1 � PCLK2
	sprintf (msg_UART_TX, "%lu, %lu, %lu, %lu\r\n", RCC_Clocks.SYSCLK_Frequency, RCC_Clocks.HCLK_Frequency, RCC_Clocks.PCLK1_Frequency, RCC_Clocks.PCLK2_Frequency);
	UART3_PutString(msg_UART_TX);
	memset(msg_UART_TX, '\0', sizeof(msg_UART_TX)); //������� ����� ��������

	/*sprintf (msg_UART_TX, "%x\r\n", I2C1_read_byte (EEPROM_ADDRESS, 0x1F3F));
	UART3_PutString(msg_UART_TX);*/

	I2C1_read_array (EEPROM_ADDRESS, 0x0, I2C_RX_buffer, buffer_SIZE);
	for (uint8_t count = 0; count < buffer_SIZE; count++)
		{
		if (count == (buffer_SIZE-1))
			{
			sprintf (msg_UART_TX, "%x\r\n ", I2C_RX_buffer [count]);
			UART3_PutString(msg_UART_TX);
			}
		else
			{
			sprintf (msg_UART_TX, "%x ", I2C_RX_buffer [count]);
			UART3_PutString(msg_UART_TX);
			}
		}

	I2C1_read_array (EEPROM_ADDRESS, 0x20, I2C_RX_buffer, buffer_SIZE);
	for (uint8_t count = 0; count < buffer_SIZE; count++)
		{
		if (count == (buffer_SIZE-1))
			{
			sprintf (msg_UART_TX, "%x\r\n ", I2C_RX_buffer [count]);
			UART3_PutString(msg_UART_TX);
			}
		else
			{
			sprintf (msg_UART_TX, "%x ", I2C_RX_buffer [count]);
			UART3_PutString(msg_UART_TX);
			}
		}

	delay_ms(1000);
	}
}
