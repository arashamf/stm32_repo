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
#include "string.h"

#include "../../NRF_F4/inc/delay_lib.h"
#include "../../NRF_F4/inc/MCU_ini.h"
#include "../../NRF_F4/inc/SPI_lib.h"
#include "../../NRF_F4/inc/uart_lib.h"
#include "nrf24l01.h"

#define buffer_SIZE 48 //размер буффера

extern RCC_ClocksTypeDef RCC_Clocks;
char msg_UART_TX [buffer_SIZE]; // буфер передачи по UART
static unsigned char NRF_buffer [20]; //буффер приёма данных по SPI


int main(void)
{

	// объявления для PLL
	uint8_t RCC_PLLQ = 4; // предделитель PLLQ для USB
	uint8_t RCC_PLLM = 8; // предделитель PLL
	uint32_t  RCC_PLLN = 192 ; // умножитель PLL
	uint32_t  RCC_PLLP = 2; // постделитель PLL = 2
	uint8_t CPU_WS = 2; //время задержки для работы с Flash памятью
	uint8_t status = PLL_ini_SPL (CPU_WS, RCC_PLLM, RCC_PLLN, RCC_PLLP, RCC_PLLQ); //настройка генератора PLL (тактирование от HSE = 8MHz, SYSCLK = 96MHz, APB1=APB2=24MHz)
	LEDS_ini (); //инициализируем LEDs
	UART3_ini (); //инициализируем UART2
	SPI1_ini (); //инициализируем SPI2
	UART3_PutString ("NRF programm start\r\n");
	if (status)
		UART3_PutString("PLL ok\r\n");
	RCC_GetClocksFreq (&RCC_Clocks); //возвращает частоты SYSCLK, HCLK, PCLK1 и PCLK2
	sprintf (msg_UART_TX, "%lu, %lu, %lu, %lu\r\n", RCC_Clocks.SYSCLK_Frequency, RCC_Clocks.HCLK_Frequency, RCC_Clocks.PCLK1_Frequency, RCC_Clocks.PCLK2_Frequency);
	UART3_PutString(msg_UART_TX);
	SET_BIT (GPIOA->MODER, GPIO_MODER_MODER3_0); // PA3 (CE) на выход
	NRF24_ini ();

	while (1)
	{

		sprintf (msg_UART_TX,"CONFIG: 0x%02X\r\n", SPI1_read_byte (CONFIG, READ_REG));
		UART3_PutString (msg_UART_TX);

		sprintf (msg_UART_TX,"EN_AA: 0x%02X\r\n", SPI1_read_byte (EN_AA, READ_REG));
		UART3_PutString (msg_UART_TX);

		sprintf (msg_UART_TX,"EN_RXADDR: 0x%02X\r\n", SPI1_read_byte (EN_RXADDR, READ_REG));
		UART3_PutString (msg_UART_TX);

		sprintf (msg_UART_TX,"STATUS: 0x%02X\r\n", SPI1_read_byte (STATUS,  READ_REG));
		UART3_PutString (msg_UART_TX);

		sprintf (msg_UART_TX,"RF_SETUP: 0x%02X\r\n", SPI1_read_byte (RF_SETUP, READ_REG));
		UART3_PutString (msg_UART_TX);

		SPI1_read_array (TX_ADDR, NRF_buffer, TX_ADR_WIDTH, READ_REG);
		sprintf (msg_UART_TX,"TX_ADDR: 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X\r\n", NRF_buffer[0], NRF_buffer[1], NRF_buffer[2], NRF_buffer[3], NRF_buffer[4]);
		UART3_PutString (msg_UART_TX);

		SPI1_read_array (RX_ADDR_P1, NRF_buffer, TX_ADR_WIDTH, READ_REG);
		sprintf (msg_UART_TX,"RX1_ADDR: 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X\r\n", NRF_buffer[0], NRF_buffer[1], NRF_buffer[2], NRF_buffer[3], NRF_buffer[4]);
		UART3_PutString (msg_UART_TX);

		SPI1_read_array (RX_ADDR_P2, NRF_buffer, TX_ADR_WIDTH, READ_REG);
		sprintf (msg_UART_TX,"RX2_ADDR: 0x%02X, 0x%02X, 0x%02X, 0x%02X, 0x%02X\r\n", NRF_buffer[0], NRF_buffer[1], NRF_buffer[2], NRF_buffer[3], NRF_buffer[4]);
		UART3_PutString (msg_UART_TX);

		delay_ms (1000);
	}
}
