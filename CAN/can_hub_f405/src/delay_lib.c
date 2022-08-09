/*
 * delay_lib.c
 *
 *  Created on: 17 пїЅпїЅпїЅпїЅ. 2020 пїЅ.
 *      Author: ALEXEY
 */

#include "delay_lib.h"

void ini_TIMER5 ()
{
	RCC_APB1PeriphClockCmd (RCC_APB1Periph_TIM5, ENABLE);  //включаем тактирование
	TIM_TimeBaseInitTypeDef TIM5_ini;
	TIM5_ini.TIM_Prescaler = 48000 - 1; //предделитель
	TIM5_ini.TIM_CounterMode = TIM_CounterMode_Up; //счёт на увеличение
	TIM5_ini.TIM_Period = 1000 - 1;  //значение перезагрузки
	TIM5_ini.TIM_ClockDivision = TIM_CKD_DIV1; //показывает отношение деления между частотой таймера (CK_INT) и тактовой частотой дискретизации, используемой цифровыми фильтрами (ETR, TIx). 00: tDTS = tCK_INT
	TIM5_ini.TIM_RepetitionCounter = 0x000;//Если указать число N>0, то прерывание будет генерироваться не при каждом переполнение счётчика, а на каждые N переполнений. В режиме ШИМ (N+1) соответствует:
	 	 	 	 	 	 	 	 	 	//количество периодов ШИМ в режиме выравнивания по краям или количество полупериодов ШИМ в режиме центрирования;
	TIM_TimeBaseInit(TIM5, &TIM5_ini);
	TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);  //разрешим прерывания по обновления таймера 5
	NVIC_EnableIRQ(TIM5_IRQn); //разрешим прерывания от таймера 5 в контроллере прерывани1

	TIM_Cmd(TIM5, ENABLE); //включим таймер 5
}

void delay_us(uint32_t us)
{
	if (us <= 1)
			return;
	RCC_ClocksTypeDef RCC_Clocks; //объявим структуру
	RCC_GetClocksFreq (&RCC_Clocks); //возвращает частоты SYSCLK, HCLK, PCLK1 и PCLK2
	TIM_DeInit(TIM5); //устанавливает в регистры TIM5 значения сброса по умолчанию
	RCC_APB1PeriphClockCmd (RCC_APB1Periph_TIM5, ENABLE);
	TIM_TimeBaseInitTypeDef TIM5_ini;
	TIM5_ini.TIM_Prescaler =  (RCC_Clocks.PCLK1_Frequency/1000000) - 1; //предделитель
	TIM5_ini.TIM_CounterMode = TIM_CounterMode_Up; //счёт на увеличение
	TIM5_ini.TIM_Period =  2*us-1;  //значение перезагрузки
	TIM5_ini.TIM_ClockDivision = TIM_CKD_DIV1; //показывает отношение деления между частотой таймера (CK_INT) и тактовой частотой дискретизации, используемой цифровыми фильтрами (ETR, TIx). 00: tDTS = tCK_INT
	TIM5_ini.TIM_RepetitionCounter = 0x000;//Если указать число N>0, то прерывание будет генерироваться не при каждом переполнение счётчика, а на каждые N переполнений.
	TIM_TimeBaseInit(TIM5, &TIM5_ini);

	TIM_ClearFlag(TIM5, TIM_FLAG_Update); // сбросим флаг обновления
	TIM_Cmd(TIM5, ENABLE); //включим таймер 5
	while (!(TIM_GetFlagStatus(TIM5, TIM_FLAG_Update))) {} //ждём установки флага обновления таймера 5
	TIM_Cmd(TIM5, DISABLE);  //отключаем таймер 5
	TIM_ClearFlag(TIM5, TIM_FLAG_Update); // сбросим флаг обновления
}

void delay_ms(uint32_t ms)
{
	if (ms <= 1)
		return;
	RCC_GetClocksFreq (&RCC_Clocks); //возвращает частоты SYSCLK, HCLK, PCLK1 и PCLK2
	TIM_DeInit(TIM5); //устанавливает в регистры TIM5 значения сброса по умолчанию
	RCC_APB1PeriphClockCmd (RCC_APB1Periph_TIM5, ENABLE);
	TIM_TimeBaseInitTypeDef TIM5_ini;
	TIM5_ini.TIM_Prescaler = (RCC_Clocks.PCLK1_Frequency/1000) - 1; //предделитель
	TIM5_ini.TIM_CounterMode = TIM_CounterMode_Up; //счёт на увеличение
	TIM5_ini.TIM_Period = 2*ms-1;  //значение перезагрузки
	TIM5_ini.TIM_ClockDivision = TIM_CKD_DIV1; //показывает отношение деления между частотой таймера (CK_INT) и тактовой частотой дискретизации, используемой цифровыми фильтрами (ETR, TIx). 00: tDTS = tCK_INT
	TIM5_ini.TIM_RepetitionCounter = 0x000;//Если указать число N>0, то прерывание будет генерироваться не при каждом переполнение счётчика, а на каждые N переполнений.
	TIM_TimeBaseInit(TIM5, &TIM5_ini);

	TIM_ClearFlag(TIM5, TIM_FLAG_Update); // сбросим флаг обновления
	TIM_Cmd(TIM5, ENABLE); //включим таймер 5
	while (!(TIM_GetFlagStatus(TIM5, TIM_FLAG_Update))) {} //ждём установки флага обновления таймера 5
	TIM_Cmd(TIM5, DISABLE);  //отключаем таймер 5
	TIM_ClearFlag(TIM5, TIM_FLAG_Update); // сбросим флаг обновления
}

