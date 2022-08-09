/*
 * delay_lib.c
 *
 *  Created on: 17 февр. 2020 г.
 *      Author: ALEXEY
 */
#include "delay_lib.h"

#include "stm32f4xx.h"
#define stm32f4xx

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
	TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);  //разрешим прервыния по обновления таймера 5
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
	TIM5_ini.TIM_Prescaler =  (RCC_Clocks.SYSCLK_Frequency/1000000) - 1; //предделитель
	TIM5_ini.TIM_CounterMode = TIM_CounterMode_Up; //счёт на увеличение
	TIM5_ini.TIM_Period =  us-1;  //значение перезагрузки
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
	TIM5_ini.TIM_Prescaler = (2*RCC_Clocks.PCLK1_Frequency/1000) - 1; //предделитель, PCLK1_Frequency - тактовая частота шины АРВ1, тактирующей таймер 5 с умножителем 2
	TIM5_ini.TIM_CounterMode = TIM_CounterMode_Up; //счёт на увеличение
	TIM5_ini.TIM_Period = ms-1;  //значение перезагрузки
	TIM5_ini.TIM_ClockDivision = TIM_CKD_DIV1; //показывает отношение деления между частотой таймера (CK_INT) и тактовой частотой дискретизации, используемой цифровыми фильтрами (ETR, TIx). 00: tDTS = tCK_INT
	TIM5_ini.TIM_RepetitionCounter = 0x000;//Если указать число N>0, то прерывание будет генерироваться не при каждом переполнение счётчика, а на каждые N переполнений.
	TIM_TimeBaseInit(TIM5, &TIM5_ini);
	TIM_ClearFlag(TIM5, TIM_FLAG_Update); // сбросим флаг обновления
	TIM_Cmd(TIM5, ENABLE); //включим таймер 5
	while (!(TIM_GetFlagStatus(TIM5, TIM_FLAG_Update))) {} //ждём установки флага обновления таймера 5
	TIM_Cmd(TIM5, DISABLE);  //отключаем таймер 5
	TIM_ClearFlag(TIM5, TIM_FLAG_Update); // сбросим флаг обновления
}

//---------------------------------------функция настройки таймера 4--------------------------------------------//
void ini_TIMER4 (uint32_t duration)
{
	//----------------настройка вывода PВ7 для внешнего триггера по каналу 2 таймера 4---------------------//
	/*RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOB, ENABLE); //затактируем порт В

	GPIO_InitTypeDef 	GPIO_Ini_Tim4; //настроим входы для таймер4
	GPIO_Ini_Tim4.GPIO_Pin = GPIO_Pin_7 ; //PВ7 - канал 2 таймера 4
	GPIO_Ini_Tim4.GPIO_Mode =  GPIO_Mode_AF; //порт В пин 7 в альт. режим
	GPIO_Ini_Tim4.GPIO_Speed = GPIO_Low_Speed; //скорость порта низкая
	GPIO_Ini_Tim4.GPIO_OType = GPIO_OType_PP; //выход с подтяжкой (push-pull) (неважно)
	GPIO_Ini_Tim4.GPIO_PuPd =  GPIO_PuPd_NOPULL; //вход без подтяжки (неважно)
	GPIO_Init (GPIOB, &GPIO_Ini_Tim4);

	GPIO_PinAFConfig (GPIOB, (GPIO_PinSource7), GPIO_AF_TIM4); //установим для вывода PВ7 альт. функцию - канал 2 TIM4*/

	//--------------------настройка непосредственно таймера на счёт-------------------------//
	RCC_APB1PeriphClockCmd (RCC_APB1Periph_TIM4, ENABLE);  //включаем тактирование

	RCC_GetClocksFreq (&RCC_Clocks); //возвращает частоты SYSCLK, HCLK, PCLK1 и PCLK2
	TIM_DeInit(TIM4); //устанавливает в регистры TIM4 значения сброса по умолчанию

	TIM_TimeBaseInitTypeDef TIM4_ini;
	TIM4_ini.TIM_Prescaler = (2*RCC_Clocks.PCLK1_Frequency/1000) - 1; //предделитель
	TIM4_ini.TIM_CounterMode = TIM_CounterMode_Up; //счёт на увеличение
	TIM4_ini.TIM_Period = duration - 1;  //значение перезагрузки
	TIM4_ini.TIM_ClockDivision = TIM_CKD_DIV1; //показывает отношение деления между частотой таймера (CK_INT) и тактовой частотой дискретизации, используемой цифровыми фильтрами (ETR, TIx). 00: tDTS = tCK_INT
	TIM4_ini.TIM_RepetitionCounter = 0x000;//Если указать число N>0, то прерывание будет генерироваться не при каждом переполнение счётчика, а на каждые N переполнений.
	TIM_TimeBaseInit(TIM4, &TIM4_ini);

	TIM_ClearFlag(TIM4, TIM_FLAG_Update); // сбросим флаг обновления
//	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);  //разрешим прервыния по обновления таймера 4

//	NVIC_EnableIRQ(TIM4_IRQn); //разрешим прерывания от таймера 5 в контроллере прерывани1

	TIM_Cmd(TIM4, ENABLE); //включим таймер 4
}
