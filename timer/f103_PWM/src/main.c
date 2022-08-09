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
#include "stdio.h"

extern uint8_t duty_ch1; // длительность импульсов ШИМ канала 1
extern uint8_t duty_ch2;

uint16_t delay_count = 0;

#define msg_SIZE 60
static char msg[msg_SIZE]; // массив для формирования сообщений для вывода по UART

//-----------------------ф-я обработки прерывания от таймера4------------------------//
void TIM4_IRQHandler(void)
{
	TIM_ClearITPendingBit(TIM4, TIM_IT_Update); // сбросим флаг прерывания
	duty_ch1 += PERIOD/100; // прибавляем длительность по 1 % от периода
	duty_ch2 += PERIOD/100; // прибавляем длительность по 2 % от периода
	if(duty_ch1 > PERIOD)
		duty_ch1 = PERIOD/100; // ограничиваем диапазон изменения - от 0 до 100%
	if(duty_ch2 > PERIOD)
		duty_ch2 = PERIOD/100; // ограничиваем диапазон изменения - от 0 до 100%
	TIM3->CCR1 = duty_ch1; // обновляем длительность ШИМ1
	TIM3->CCR2 = duty_ch2; // обновляем длительность ШИМ2
}

//-----------------функция обработки прерывания от таймера SysTick--------------------//
void SysTick_Handler ()
{
	if (delay_count > 0) //пока значение delay_count больше 0,
		{
		delay_count--; //уменьшаем значение delay_count
		}
}

//----------------------------------------функция задержки-------------------------------------------------//
void delay_miliseconds (uint16_t delay_temp)
	{
	delay_count = delay_temp;
	while (delay_count) {} //пока значение delay_count не равно 0, продолжаем ждать
	}

//---------------------------------функция передачи данных по UART2----------------------------------------//
void UART2_PutRAMString(char *str)
{
	char c;
	while((c = *str++)) // отправляем символы строки последовательно, пока не достигнут символ конца строки "\0"
		{
		while (!USART_GetFlagStatus(USART2, USART_FLAG_TC)) {}; //ждём выставления флага "transmission complete"
		USART_SendData(USART2, c); // передаем байт
		}
}

int main(void)
{
	// объявления для PLL (8/8х192/2=96 MHz)
	uint8_t RCC_PLLMUL = 8; // умножитель частоты PLL
	uint8_t CPU_WS = 1; //врем¤ задержки для работы с Flash памятью (для 32 МГц равно 1)
	uint8_t status_PLL = 0; //флаг готовности PLL
	status_PLL = PLL_ini (CPU_WS, RCC_PLLMUL); //настройка генератора PLL (тактирование от HSE = 8MHz, SYSCLK = 96MHz, APB1=APB2=24MHz)
	SystemCoreClockUpdate ();  //обновим системную частоту
	SysTick_Config (SystemCoreClock/1000);  //прерывания раз в 1 мс
	LEDS_ini ();
	UART2_ini ();
	if (status_PLL)
		UART2_PutRAMString("PLL ok!\r\n");
	else
		UART2_PutRAMString("PLL error!\r\n");
	RCC_ClocksTypeDef RCC_freq;
	RCC_GetClocksFreq(&RCC_freq);
	sprintf (msg, "SYSCLK=%ld, HCLK=%ld, PCLK1=%ld, PCLK2=%ld\r\n", RCC_freq.SYSCLK_Frequency, RCC_freq.HCLK_Frequency,
			RCC_freq.PCLK1_Frequency, RCC_freq.PCLK2_Frequency);
	UART2_PutRAMString(msg);
	PWM_timer3_ini ();
	timer4_ini ();

	RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOC, ENABLE);  //затактируем порт C

	GPIO_InitTypeDef 	GPIO_Init_LED1;
	GPIO_Init_LED1.GPIO_Pin = GPIO_Pin_13;
	GPIO_Init_LED1.GPIO_Mode = GPIO_Mode_Out_PP; //пины PC13 подтяжкой (push-pull)
	GPIO_Init_LED1.GPIO_Speed = GPIO_Speed_2MHz; //скорость порта самая низкая

	GPIO_Init (GPIOC, &GPIO_Init_LED1);

	while (1)
	{
		GPIO_SetBits(GPIOA, GPIO_Pin_1);
		GPIO_ResetBits(GPIOA, GPIO_Pin_0);
		GPIO_SetBits(GPIOC, GPIO_Pin_13);
		sprintf (msg, "System clock switch = %lx\r\n", READ_BIT (RCC->CFGR, RCC_CFGR_SWS));
		UART2_PutRAMString(msg);
		UART2_PutRAMString ("led red!\r\n");
		delay_miliseconds (1000);
		GPIO_SetBits(GPIOA, GPIO_Pin_0);
		GPIO_ResetBits(GPIOA, GPIO_Pin_1);
		GPIO_ResetBits(GPIOC, GPIO_Pin_13);
		UART2_PutRAMString ("led yellow!\r\n");
		delay_miliseconds (1000);
		sprintf (msg, "SYSCLK=%ld, HCLK=%ld, PCLK1=%ld, PCLK2=%ld\r\n", RCC_freq.SYSCLK_Frequency, RCC_freq.HCLK_Frequency,
					RCC_freq.PCLK1_Frequency, RCC_freq.PCLK2_Frequency);
		UART2_PutRAMString(msg);
	}
}
