//-----------------------------------������������� RTC CMSIS--------------------------------------//
#include "stdio.h"
#include "stm32f4xx.h"
#include "delay_lib.h"
#include "MCU_ini.h"

#define DELAY 1000 //��������

#define msg_SIZE 50  //������ ������� ��� ������������ ��������� ��� �������� �� UART
char msg [msg_SIZE]; // ������ ��� ������������ ��������� ��� �������� �� UART

uint8_t seconds = 0; // �������
uint16_t minuts = 0; // ������
uint32_t hours = 0; // ����

//������ �������� ������ �� UART2
void UART2_PutRAMString(char *str)
{
	char c;
	while((c = *str++)) // ��������������� ��������� ������� �������, ���� �� ��������� ������ ����� ������ "\0"
		{
		while (!USART_GetFlagStatus(USART2, USART_FLAG_TC)) {}; //��� ���������� ����� "transmission complete" (�������� ���������)
		USART_SendData(USART2, c); // �������� ����
		}
}

//---------------------------�-�� ������������� ����� RTC CMSIS-------------------------------------//
void RTC_clock_ini ()
	{
	SET_BIT (RCC->CSR, RCC_CSR_LSION); // ������� ���������� �������������� ��������� LSI 32���
	while(!(READ_BIT (RCC->CSR, RCC_CSR_LSIRDY))) {}; // ������� ���������� ���������� LSI � ������*/
	/*SET_BIT (RCC->BDCR, RCC_BDCR_LSEON); //������� ������� ������� ����� LSE 32768 ��
	while(!(READ_BIT (RCC->BDCR, RCC_BDCR_LSERDY))) {}; //������� ���������� ���������� LSI � ������*/
	//----------������ ������ �� ������ �������� RTC-------------//
	SET_BIT (RCC->APB1ENR, RCC_APB1ENR_PWREN); //�������� ������������ ���������� �������
	SET_BIT (PWR->CR, PWR_CR_DBP); // �������� ������ � �������� RTC
	SET_BIT (RCC->BDCR, RCC_BDCR_BDRST); //��������� ���� ������ ���������� ������ RTC
	RCC_BackupResetCmd(DISABLE); //����� ��� ������ ���������� ������ RTC
	//CLEAR_BIT (RCC->BDCR, RCC_BDCR_BDRST); //���� ���������� ����� ������, �� ��������� �������� ����� ���������� �������
	//SET_BIT (RCC->BDCR, RCC_BDCR_RTCSEL_0); //������� � �������� ��������� ������������ ��������� LSE
	SET_BIT (RCC->BDCR, RCC_BDCR_RTCSEL_1); //������� � �������� ��������� ������������ ��������� LSI (����������)
	SET_BIT (RCC->BDCR, RCC_BDCR_RTCEN); //������� ������������ RTC
	// ����������� ������������������ ������ ������� ���������� ��������� RTC
	WRITE_REG(RTC->WPR, 0xCA); // ���� ������ 1
	WRITE_REG(RTC->WPR, 0x53); // ���� ������ 2
	// ������������� RTC
	while(!(RTC->ISR & RTC_ISR_RSF)) {}; //��������� ��� RSF (��� ��������������� ���������, ����� ����������� �������� ���������� � ������� �������� (RTC_SSRx, RTC_TRx � RTC_DRx). ���� ��� ������������ ��������� � ������ �������������, � �� ����� ��� �������� ������ ��������� � ��������� �������� (SHPF=1), ��� � ������ ������ ���������� �������� �������� (BYPSHAD=1). ���� ��� ����� ����� ���� ������ ��. 0: ������� �������� ��������� ��� �� ����������������. 1: ������������� ������� ��������� ���������)
	SET_BIT (RTC->ISR, RTC_ISR_INIT); //������ � ����� ������������� �����
	while(!(RTC->ISR & RTC_ISR_INITF)) {}; // ������� ����������
	// ���������� �������� �� 15999 ck_spre frequency = ck_apre frequency/(PREDIV_S+1), ����������� �������� - �� 2,
	// !����������� �������� �� ����� ���� ����� 0, ������� �������� 1 (������� �� 2 - ck_apre frequency = RTCCLK frequency/(PREDIV_A+1))
	WRITE_REG (RTC->PRER, (0x3E7F + 0x1000));  //���  PREDIV_A = 15999 (0x3E7F), PREDIV_S = 1 (0x10000)
	CLEAR_BIT (RTC->CR, RTC_CR_FMT); // ������ 24 ����
	CLEAR_BIT (RTC->TR, RTC_TR_PM); //0: 0-23-hour format; 1: 0-12 hour format
	seconds = 55; // �������
	minuts = 59; // ������
	hours = 12; // ����
	WRITE_REG (RTC->TR, ((seconds - (seconds/10)*10) + ((seconds/10)<<4) + ((minuts - (minuts/10)*10)<<8) //��� ((minuts - ((minuts/10)*10))<<8) - �������� �������, ������������ � 8-11 ���, ((minuts/10)<<12) - �������� �������, ������������ � 12-14 ���
	+ ((minuts/10)<<12) + ((hours - (hours/10)*10)<<16) + ((hours/10)<<20))); // ����� �������� ������� ����� ������ ��� ������� ��� �� ������������
	CLEAR_BIT (RTC->ISR, RTC_ISR_INIT); // ���������� ��� �������������
	while (READ_BIT (RTC->ISR, RTC_ISR_INITF)) {}; // ������� ����������
	WRITE_REG(RTC->WPR, 0xFF); //������� ���� ������
	}
int main(void)
{
	// ���������� ��� PLL (8/8�192/2=96 MHz)
	uint8_t RCC_PLLM = 8; // ������������ PLL
	uint16_t  RCC_PLLN = 192 ; // ���������� PLL
	uint32_t  RCC_PLLP = 0; // ������������ PLL = 2
	uint8_t CPU_WS = 3; //���� �������� ��� ������ � Flash �������
	uint8_t status_PLL; //���� ���������� PLL
	status_PLL = PLL_ini (CPU_WS, RCC_PLLM, RCC_PLLN, RCC_PLLP); //��������� ���������� PLL (������������ �� HSE = 8MHz, SYSCLK = 96MHz, APB1=APB2=24MHz)
	SystemCoreClockUpdate ();  //������� ��������� ������� (SYSCLK/8=12MHz)
//	LEDS_ini (); //�������������� LEDs
	UART2_ini (); //�������������� UART2
	if (status_PLL) //���� ��������� PLL ������ ���������
		UART2_PutRAMString ("PLL ok\r\n");
	else
		UART2_PutRAMString ("PLL error\r\n");
	RTC_clock_ini ();
	UART2_PutRAMString ("RTC programm start\r\n");


	while (1)
	{
		seconds = ((READ_BIT (RTC->TR, 0x0070))>>4)*10 + (READ_BIT (RTC->TR, 0xF)); // �������� ������� � ������� ������ �� ��������������� �������� RTC
		minuts = ((READ_BIT (RTC->TR, 0x7000))>>12)*10 + ((READ_BIT(RTC->TR, 0xF00))>>8); // �������� ������� � ������� ����� �� ��������������� �������� RTC
		hours = ((READ_BIT (RTC->TR, 0x300000))>>20)*10 + ((READ_BIT (RTC->TR, 0xF0000))>>16); // �������� ������� � ������� ����� �� ��������������� �������� RTC
		sprintf(msg,"time: %2d-%2d-%2d\r\n", hours, minuts, seconds);
		UART2_PutRAMString (msg); // ������� ������ �� UART
		delay_ms (DELAY); // �������� 1 c
	}
}
