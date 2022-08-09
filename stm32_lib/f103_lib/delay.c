/*
 * delay.c
 *
 *  Created on: 22 нояб. 2020 г.
 *      Author: arashamf
 */
#include "delay.h"

//---------------------------------функция передачи данных по UART2----------------------------------------//
void UART2_PutString(char *str)
{
	char c;
	while((c = *str++)) // отправляем символы строки последовательно, пока не достигнут символ конца строки "\0"
		{
		while (!USART_GetFlagStatus(USART2, USART_FLAG_TC)) {}; //ждём выставления флага "transmission complete"
		USART_SendData(USART2, c); // передаем байт
		}
}

void ini_TIMER6 ()
{
	RCC_APB1PeriphClockCmd (RCC_APB1Periph_TIM6, ENABLE);  //включаем тактирование
	TIM_TimeBaseInitTypeDef TIM6_ini;
	TIM6_ini.TIM_Prescaler = 48000 - 1; //предделитель
	TIM6_ini.TIM_CounterMode = TIM_CounterMode_Up; //счёт на увеличение
	TIM6_ini.TIM_Period = 1000 - 1;  //значение перезагрузки
	TIM6_ini.TIM_ClockDivision = TIM_CKD_DIV1; //показывает отношение деления между частотой таймера (CK_INT) и тактовой частотой дискретизации, используемой цифровыми фильтрами (ETR, TIx). 00: tDTS = tCK_INT
	TIM6_ini.TIM_RepetitionCounter = 0x000;//Если указать число N>0, то прерывание будет генерироваться не при каждом переполнение счётчика, а на каждые N переполнений. В режиме ШИМ (N+1) соответствует:
	 	 	 	 	 	 	 	 	 	//количество периодов ШИМ в режиме выравнивания по краям или количество полупериодов ШИМ в режиме центрирования;
	TIM_TimeBaseInit(TIM6, &TIM6_ini);
//	TIM_ITConfig(TIM6, TIM_IT_Update, ENABLE);  //разрешим прерывания по обновления таймера 6
//	NVIC_EnableIRQ(TIM_IRQn); //разрешим прерывания от таймера 6 в контроллере прерывани1
}

void delay_us(uint32_t us)
{
	if (us <= 1)
			return;
	RCC_ClocksTypeDef RCC_Clocks; //объявим структуру
	RCC_GetClocksFreq (&RCC_Clocks); //возвращает частоты SYSCLK, HCLK, PCLK1 и PCLK2
	TIM_DeInit(TIM6); //устанавливает в регистры TIM6 значения сброса по умолчанию
	RCC_APB1PeriphClockCmd (RCC_APB1Periph_TIM6, ENABLE);
	TIM_TimeBaseInitTypeDef TIM6_ini;
	TIM6_ini.TIM_Prescaler =  (2*RCC_Clocks.PCLK1_Frequency/1000000) - 1; //предделитель
	TIM6_ini.TIM_CounterMode = TIM_CounterMode_Up; //счёт на увеличение
	TIM6_ini.TIM_Period =  us-1;  //значение перезагрузки
	TIM6_ini.TIM_ClockDivision = TIM_CKD_DIV1; //показывает отношение деления между частотой таймера (CK_INT) и тактовой частотой дискретизации, используемой цифровыми фильтрами (ETR, TIx). 00: tDTS = tCK_INT
	TIM6_ini.TIM_RepetitionCounter = 0x000;//Если указать число N>0, то прерывание будет генерироваться не при каждом переполнение счётчика, а на каждые N переполнений.
	TIM_TimeBaseInit(TIM6, &TIM6_ini);

	TIM_ClearFlag(TIM6, TIM_FLAG_Update); // сбросим флаг обновления
	TIM_Cmd(TIM6, ENABLE); //включим таймер 5
	while (!(TIM_GetFlagStatus(TIM6, TIM_FLAG_Update))) {} //ждём установки флага обновления таймера 6
	TIM_Cmd(TIM6, DISABLE);  //отключаем таймер 6
	TIM_ClearFlag(TIM6, TIM_FLAG_Update); // сбросим флаг обновления
}

void delay_ms(uint32_t ms)
{
	if (ms <= 1)
		return;
	RCC_GetClocksFreq (&RCC_Clocks); //возвращает частоты SYSCLK, HCLK, PCLK1 и PCLK2
	TIM_DeInit(TIM5); //устанавливает в регистры TIM6 значения сброса по умолчанию
	RCC_APB1PeriphClockCmd (RCC_APB1Periph_TIM5, ENABLE);
	TIM_TimeBaseInitTypeDef TIM5_ini;
	TIM5_ini.TIM_Prescaler = (2*RCC_Clocks.PCLK1_Frequency/1000) - 1; //предделитель
//	TIM5_ini.TIM_Prescaler = (16000 - 1);
	TIM5_ini.TIM_CounterMode = TIM_CounterMode_Up; //счёт на увеличение
	TIM5_ini.TIM_Period = ms-1;  //значение перезагрузки
	TIM5_ini.TIM_ClockDivision = TIM_CKD_DIV1; //показывает отношение деления между частотой таймера (CK_INT) и тактовой частотой дискретизации, используемой цифровыми фильтрами (ETR, TIx). 00: tDTS = tCK_INT
	TIM5_ini.TIM_RepetitionCounter = 0x000;//Если указать число N>0, то прерывание будет генерироваться не при каждом переполнение счётчика, а на каждые N переполнений.
	TIM_TimeBaseInit(TIM5, &TIM5_ini);

	TIM_ClearFlag(TIM5, TIM_FLAG_Update); // сбросим флаг обновления
	TIM_Cmd(TIM5, ENABLE); //включим таймер 5
//	UART2_PutString("tim6-ok\r\n");
	while (!(TIM_GetFlagStatus(TIM5, TIM_FLAG_Update))) {} //ждём установки флага обновления таймера 6
//	UART2_PutString("tim6-ok-ok\r\n");
	TIM_Cmd(TIM5, DISABLE);  //отключаем таймер 5
	TIM_ClearFlag(TIM5, TIM_FLAG_Update); // сбросим флаг обновления
}
