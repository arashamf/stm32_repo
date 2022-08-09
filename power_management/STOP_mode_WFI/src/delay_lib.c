/*
 * delay_lib.c
 *
 *  Created on: 17 февр. 2020 г.
 *      Author: ALEXEY
 */
#include "delay_lib.h"
#include "stm32f4xx.h"

void delay_us(uint32_t us)
{
	if (us <= 1)
			return;
	RCC_ClocksTypeDef RCC_Clocks; //объявим структуру
	RCC_GetClocksFreq (&RCC_Clocks); //возвращает частоты SYSCLK, HCLK, PCLK1 и PCLK2
	TIM_DeInit(TIM5); //устанавливает в регистры TIM5 значения сброса по умолчанию

	RCC_APB1PeriphClockCmd (RCC_APB1Periph_TIM5, ENABLE);

	TIM_TimeBaseInitTypeDef TIM5_ini;
	TIM5_ini.TIM_Prescaler =  (2*RCC_Clocks.PCLK1_Frequency/1000000) - 1; //предделитель
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
	TIM5_ini.TIM_Prescaler = (2*RCC_Clocks.PCLK1_Frequency/1000) - 1; //предделитель
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

//-----------------------------ф-я инициализации таймера 4 на срабатывание по внешнему тригеру (Gated mode)-------------------------------//
void ini_TIMER4_trigger ()
{
	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOB, ENABLE); //затактируем порт В

	GPIO_InitTypeDef 	GPIO_ini_trigger_Tim4; //настроим входы для таймер4
	GPIO_ini_trigger_Tim4.GPIO_Pin = GPIO_Pin_7 ; //PВ7 - канал 2 таймера 4
	GPIO_ini_trigger_Tim4.GPIO_Mode =  GPIO_Mode_AF; //порт В пин 7 в альт. режим
	GPIO_ini_trigger_Tim4.GPIO_Speed = GPIO_Low_Speed; //скорость порта низкая
	GPIO_ini_trigger_Tim4.GPIO_OType = GPIO_OType_PP; //выход с подтяжкой (push-pull) (неважно)
	GPIO_ini_trigger_Tim4.GPIO_PuPd =  GPIO_PuPd_NOPULL; //вход без подтяжки (неважно)
	GPIO_Init (GPIOB, &GPIO_ini_trigger_Tim4);

	GPIO_PinAFConfig (GPIOB, (GPIO_PinSource7), GPIO_AF_TIM4); //установим альт. функцию - TIM4

	RCC_APB1PeriphClockCmd (RCC_APB1Periph_TIM4, ENABLE);  //включаем тактирование

	RCC_ClocksTypeDef RCC_Clocks; //объявим структуру
	RCC_GetClocksFreq (&RCC_Clocks); //возвращает частоты SYSCLK, HCLK, PCLK1 и PCLK2

	TIM_TimeBaseInitTypeDef TIM4_ini;
	TIM4_ini.TIM_Prescaler =  (2*RCC_Clocks.PCLK1_Frequency/1000) - 1; //предделитель
	TIM4_ini.TIM_CounterMode = TIM_CounterMode_Up; //счёт на увеличение
	TIM4_ini.TIM_Period = 1000 - 1;  //значение перезагрузки, прерывание раз в 1 с
	TIM4_ini.TIM_ClockDivision = TIM_CKD_DIV1; //показывает отношение деления между частотой таймера (CK_INT) и тактовой частотой дискретизации, используемой цифровыми фильтрами (ETR, TIx). 00: tDTS = tCK_INT
	TIM4_ini.TIM_RepetitionCounter = 0x000;//Если указать число N>0, то прерывание будет генерироваться не при каждом переполнение счётчика, а на каждые N переполнений.
	TIM_TimeBaseInit(TIM4, &TIM4_ini);

	TIM_ClearITPendingBit (TIM4, TIM_IT_Update); //сбросим флаг прерывания по переполнению
	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE);  //разрешим прервыния по обновления таймера 4

	SET_BIT (TIM4->CCMR1, TIM_CCMR1_CC2S_0);  //канал СС2 на вход, Ti2 - источник входного захвата
//	SET_BIT (TIM4->CCER, TIM_CCER_CC2P); //проверка полярности и обнаружения только низкого уровня
	CLEAR_BIT (TIM4->CCER, TIM_CCER_CC2P); //проверка полярности и обнаружения только высокого уровня
	SET_BIT (TIM4->SMCR, (TIM_SMCR_SMS_2 | TIM_SMCR_SMS_1)); //активируем режим триггера
	SET_BIT (TIM4->SMCR, (TIM_SMCR_TS_2 | TIM_SMCR_TS_1)); //выберем в качестве триггера timer input 2 (TI2)

	NVIC_EnableIRQ(TIM4_IRQn); //разрешим прерывания от таймера 5 в контроллере прерывани1
}
