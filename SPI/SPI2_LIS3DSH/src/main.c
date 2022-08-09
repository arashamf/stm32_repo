#include "MCU_ini.h"
#include "SPI.h"
#include "lisdsh.h"
#include "stm32f4xx.h"
#include "delay_lib.h"
#include "stdio.h"

#define DELAY 500 //��������

char OUT_XYZ[6]; // ������ ������ � �������������
uint8_t status = 0;
uint8_t status_XYZ = 0;


#define msg_SIZE 50  //������ ������� ��� ������������ ��������� ��� �������� �� UART
char msg [msg_SIZE]; // ������ ��� ������������ ��������� ��� �������� �� UART

//������� �������� ������ �� UART2
void UART2_PutRAMString(char *str)
{
	char c;
	while((c = *str++)) // ��������������� ��������� ������� �������, ���� �� ��������� ������ ����� ������ "\0"
		{
		while (!USART_GetFlagStatus(USART2, USART_FLAG_TC)) {}; //���� ����������� ����� "transmission complete" (�������� ���������)
		USART_SendData(USART2, c); // �������� ����
		}
}

//---------------------�-�� ������������� ������� LIS3DH----------------------------//
void LIS3DH_init()
{
// ��������� �������������,
SPI2_write_byte (LIS3DSH_CTRL_REG4, LIS3DSH_F_800HZ_NORMAL); // Normal mode (3.125 Hz), ��� ��� ��������
delay_us (20);
SPI2_write_byte(LIS3DSH_CTRL_REG5, 0x8); //"0b00011000" ���������� �������� +-4g
delay_us (20);
//SPI2_write_byte(LIS3DSH_CTRL_REG6, 0x10); //������� ��������� ������� ���������
}

int main(void)
{
	// ���������� ��� PLL
	uint8_t RCC_PLLM = 8; // ������������ PLL
	uint32_t  RCC_PLLN = 192 ; // ���������� PLL
	uint32_t  RCC_PLLP = 0; // ������������ PLL = 2
	uint8_t CPU_WS = 3; //����� �������� ��� ������ � Flash �������
	uint8_t status_PLL; //���� ���������� PLL
	status_PLL = PLL_ini (CPU_WS, RCC_PLLM, RCC_PLLN, RCC_PLLP); //��������� ���������� PLL (������������ �� HSE = 8MHz, SYSCLK = 96MHz, APB1=APB2=24MHz)
	SystemCoreClockUpdate ();  //������� ��������� ������� (SYSCLK/8=12MHz)
	LEDS_ini (); //�������������� LEDs
	UART2_ini (); //�������������� UART2
	if (status_PLL) //���� ��������� PLL ������� ���������
		UART2_PutRAMString ("PLL ok\r\n");
	else
		UART2_PutRAMString ("PLL error\r\n");
	UART2_PutRAMString ("SPI2 programm start\r\n");
	SPI2_ini (); //������������� SPI2
	delay_ms(DELAY);
	status = SPI2_read_byte (LIS3DSH_REG_WHOAMI); //������� ���������� �������� "WHO AM I"
	sprintf(msg,"Who_I_am=%x\r\n", status);
	UART2_PutRAMString (msg);
	LIS3DH_init(); //������������� ������������
	delay_us (DELAY/10); //�������� 50 ���
	status = SPI2_read_byte (LIS3DSH_CTRL_REG4); //��������� ������� ���������� �4
	sprintf(msg,"CTRL_REG4=%x\r\n", status);
	UART2_PutRAMString (msg);
	status = SPI2_read_byte (LIS3DSH_CTRL_REG5);  //��������� ������� ���������� �5
	sprintf(msg,"CTRL_REG5=%x\r\n", status);
	UART2_PutRAMString (msg);

	while (1)
	{
		status_XYZ = SPI2_read_byte (LIS3DSH_REG_STATUS); //��������� ������� ������� ��������������
		sprintf(msg,"status_LISDSH=%x\r\n", status_XYZ);
		UART2_PutRAMString (msg);
		if (READ_BIT(status_XYZ, 1 << 4)) //���� ������ �� ���� XYZ ����������
			{
			GPIO_SetBits (GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
			SPI2_read_array(LIS3DSH_REG_OUT_X, OUT_XYZ, 6); //��������� 6 ��������� � ������� �� ���� XYZ
			sprintf(msg,"X - %.2x %.2x : Y - %2.2x %2.2x : Z - %2.2x %2.2x\r\n", OUT_XYZ[1], OUT_XYZ[0], OUT_XYZ[3], OUT_XYZ[2], OUT_XYZ[5], OUT_XYZ[4]);
			UART2_PutRAMString (msg);
			delay_ms (DELAY);
			GPIO_ResetBits (GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
			}
		status = SPI2_read_byte (LIS3DSH_REG_WHOAMI); //������� ���������� �������� "WHO AM I"
		sprintf(msg,"Who_I_am=%x\r\n", status);
		UART2_PutRAMString (msg);
		delay_ms (DELAY);
	}
}

