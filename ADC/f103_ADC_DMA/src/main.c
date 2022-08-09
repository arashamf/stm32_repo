/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/


#include "stm32f10x.h"
#include "ini_MCU.h"
#include "delay.h"
#include "stdio.h"
#include "string.h"
#include "UART_f103.h"

#define DELAY 500

RCC_ClocksTypeDef RCC_freq; //струтктура с тактовыми частотами
char msg_UART_Tx [55];

uint16_t value_adc = 0;
volatile uint8_t flag = 0; //флаг окончания преобразования АЦП

//---------------------------------функция передачи данных по UART2----------------------------------------//
void UART1_PutRAMString(char *str)
{
	char c;
	while((c = *str++)) // отправляем символы строки последовательно, пока не достигнут символ конца строки "\0"
		{
		while (!USART_GetFlagStatus(USART1, USART_FLAG_TC)) {}; //ждём выставления флага "transmission complete"
		USART_SendData(USART1, c); // передаем байт
		}
}

void ADC1_2_IRQHandler ()
{
	ADC_ClearITPendingBit (ADC1, ADC_IT_EOC);
	flag = 1;
	ADC_ClearFlag(ADC1, ADC_FLAG_EOC);
}

int main(void)
{

	uint8_t RCC_PLLMUL = 6; // умножитель частоты PLL
	uint8_t CPU_WS = 1; //врем¤ задержки для работы с Flash памятью (для 32 МГц равно 1)
	uint8_t status_PLL = 0; //флаг готовности PLL
	status_PLL = PLL_ini (CPU_WS, RCC_PLLMUL); //настройка генератора PLL
	LEDS_ini ();
	GPIO_WriteBit(GPIOC, GPIO_Pin_13, 1);
	UART1_ini ();
	UART1_PutRAMString("start\r\n");
	if (status_PLL)
		UART1_PutRAMString("PLL ok!\r\n");
	else
		UART1_PutRAMString("PLL error!\r\n");
	RCC_GetClocksFreq(&RCC_freq);
	sprintf (msg_UART_Tx, "SYSCLK=%ld, HCLK=%ld, PCLK1=%ld, PCLK2=%ld\r\n", RCC_freq.SYSCLK_Frequency, RCC_freq.HCLK_Frequency,
					RCC_freq.PCLK1_Frequency, RCC_freq.PCLK2_Frequency);
	UART1_PutRAMString(msg_UART_Tx);
	__enable_irq();
	ADC1_ini ();
	ADC_ResetCalibration(ADC1);
	while(ADC_GetResetCalibrationStatus(ADC1)) {}
	ADC_StartCalibration(ADC1);
	while(ADC_GetCalibrationStatus(ADC1)) {}
	ADC_RegularChannelConfig(ADC1, ADC_Channel_0, 1, ADC_SampleTime_1Cycles5); //Configures for the selected ADC regular channel its corresponding rank in the sequencer and its sample time.
	ADC_SoftwareStartConvCmd (ADC1, ENABLE);

	while (1)
	{
		if (flag)
		 {
			 GPIO_WriteBit(GPIOC, GPIO_Pin_13, 1);
			 flag = 0;
	//		 HAL_GPIO_TogglePin (LED_GPIO_Port, LED_Pin);
	//		 ADC_SoftwareStartConvCmd (ADC1, DISABLE); // останавливаем АЦП1
			 value_adc = ADC_GetConversionValue (ADC1);
			 sprintf(msg_UART_Tx,  "ADC = %hd \n\r", (uint16_t)value_adc);
			 UART1_PutRAMString (msg_UART_Tx);
			 value_adc = 0;
			 delay_ms (500);
			 ADC_SoftwareStartConvCmd (ADC1, ENABLE);
		 }
	}
}
