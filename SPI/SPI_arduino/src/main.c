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
#include "stdio.h"
#include "MCU_ini.h"
#include "SPI_lib.h"
#include "UART_lib.h"
#include "delay_lib.h"

#define msg_SIZE 50 //размер массива дл¤ формировани¤ сообщений дл¤ отправки по UART
char msg_UART_Tx [msg_SIZE]; // массив дл¤ формировани¤ сообщений дл¤ отправки по UART

uint8_t SPI_data [4];
uint8_t point = 0xCA;
float temp = 0;

int main(void)
{

	// объявления для PLL
	uint8_t RCC_PLLM = 8; // предделитель PLL
	uint32_t  RCC_PLLN = 192 ; // умножитель PLL
	uint32_t  RCC_PLLP = 2; // постделитель PLL
	uint32_t RCC_PLLQ = 7;
	uint8_t CPU_WS = 2; //время задержки для работы с Flash памятью
	uint8_t status_PLL = PLL_ini (CPU_WS, RCC_PLLM, RCC_PLLN, RCC_PLLP, RCC_PLLQ); //инициализируем генератор PLL

	LEDS_ini (); //инициализируем LED
	GPIO_SetBits (LED_Port, LED3_Pin);
	GPIO_ResetBits(LED_Port, LED4_Pin);
	GPIO_ResetBits(LED_Port, LED5_Pin);
	GPIO_ResetBits(LED_Port, LED6_Pin);

	UART2_ini_spl (57600); //инициализируем UART1
	if (status_PLL)
		UART2_PutString("init PLL ok\r\n");
	delay_us (100);

	RCC_GetClocksFreq (&RCC_Clocks); //возвращает частоты SYSCLK, HCLK, PCLK1 и PCLK2
	sprintf (msg_UART_Tx, "%lu, %lu, %lu, %lu\r\n", RCC_Clocks.SYSCLK_Frequency, RCC_Clocks.HCLK_Frequency, RCC_Clocks.PCLK1_Frequency, RCC_Clocks.PCLK2_Frequency);
	UART2_PutString (msg_UART_Tx);

	SPI2_ini ();

	UART2_PutString ("Arduino_connect\r\n");

	while (1)
	{
		GPIO_ToggleBits (LED_Port, LED4_Pin);
		GPIO_ToggleBits (LED_Port, LED3_Pin);
		SPI2_read_array_ARDUINO (point, SPI_data, 4);
		sprintf (msg_UART_Tx, "data = %u %u %u %u\r\n", SPI_data[0], SPI_data[1], SPI_data[2], SPI_data[3]);
		UART2_PutString (msg_UART_Tx);
		delay_ms (2500);
	}

}
