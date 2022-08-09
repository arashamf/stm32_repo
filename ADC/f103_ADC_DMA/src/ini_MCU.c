/*
 * ini_MCU.c
 *
 *  Created on: 11 апр. 2020 г.
 *      Author: arashamf
 */

#include "ini_MCU.h"

uint8_t duty_ch1 = PERIOD/100; // длительность импульсов ШИМ канала 1
uint8_t duty_ch2 = PERIOD/100; // длительность импульсов ШИМ канала 2

//-----------------функция инициализации пинов МК для светодиодов---------------------//
void LEDS_ini ()
{
	RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOC, ENABLE);  //затактируем порт C

	GPIO_InitTypeDef 	GPIO_Init_LED1;
	GPIO_Init_LED1.GPIO_Pin = GPIO_Pin_13;
	GPIO_Init_LED1.GPIO_Mode = GPIO_Mode_Out_PP; //пины PC13 подтяжкой (push-pull)
	GPIO_Init_LED1.GPIO_Speed = GPIO_Speed_2MHz; //скорость порта самая низкая

	GPIO_Init (GPIOC, &GPIO_Init_LED1);
}

//-------------------------функция инициализации генератора PLL------------------------------------------//
uint8_t PLL_ini (uint8_t CPU_WS, uint8_t RCC_PLLMUL)
{
	SET_BIT (RCC->CR, RCC_CR_HSEON); // включаем генератор HSE
	while(!(RCC->CR & RCC_CR_HSERDY)) {}; // ожидаем готовности генератор HSE к работе
	CLEAR_BIT (RCC->CFGR, RCC_CFGR_PLLXTPRE_HSE_Div2); //очистим биты предделителя HSE
//	SET_BIT (RCC->CFGR, RCC_CFGR_PLLXTPRE_HSE_Div2); //включим предделитель HSE равным 2 перед PLL

	SET_BIT (FLASH->ACR, FLASH_ACR_PRFTBE); //Включаем буфер предвыборки FLASH
	CLEAR_BIT (FLASH->ACR, FLASH_ACR_LATENCY); //очистим старое значение задержки
	switch (CPU_WS) //Конфигурируем Flash на циклы ожидания (RM0008 стр. 58)
	{
		case 0: SET_BIT (FLASH->ACR, FLASH_ACR_LATENCY_0);
				break;
		case 1: SET_BIT (FLASH->ACR, FLASH_ACR_LATENCY_1);
				break;
		case 2: SET_BIT (FLASH->ACR, FLASH_ACR_LATENCY_2);
				break;
		default: break;
	}

	CLEAR_BIT (RCC->CR, RCC_CR_PLLON); //отключим генератор PLL
	CLEAR_BIT (RCC->CFGR, RCC_CFGR_PLLMULL); //Очистить PLLMULL
	SET_BIT (RCC->CFGR, RCC_CFGR_PLLSRC_HSE); //установим внешний генератор (HSE) в качестве входного сигнала для PLL
	SET_BIT (RCC->CFGR, (uint32_t)((RCC_PLLMUL-2) << 18));//сдвигаем биты RCC_PLLMUL, в данном случае  PLLMUL = 6; 8МГц*6 = 48МГц
	SET_BIT (RCC->CR, RCC_CR_PLLON); // включаем PLL
	while(!(RCC->CR & RCC_CR_PLLRDY)) {}; // ожидаем готовности PLL к работе

	CLEAR_BIT (RCC->CFGR, RCC_CFGR_HPRE); //установим AHB делитель=1(SYSCLK/AHB_DIV=HCLK)
	SET_BIT (RCC->CFGR, RCC_CFGR_PPRE1_2); // установим делитель шины APB1=2 (36 МГц макс. для APB1)
	SET_BIT (RCC->CFGR, RCC_CFGR_PPRE2_2);// установим делитель шины APB2=2 (64 МГц макс. для APB2)
	SET_BIT (RCC->CFGR, RCC_CFGR_USBPRE); //установим предделитель USB = 1. Этот бит должен быть установлен для включения USB

	CLEAR_BIT (RCC->CFGR, RCC_CFGR_SW); //Очистка битов выбора источника тактового сигнала
	SET_BIT (RCC->CFGR, RCC_CFGR_SW_1); //10: системная частота с PLL
	while((RCC->CFGR&RCC_CFGR_SWS)!=0x08){} //Ожидание переключения на PLL

	if (READ_BIT (RCC->CFGR, RCC_CFGR_SWS) == 0x8) //считаем флаг статуса источника тактирования. Если 0x8, то источник PLL
			return 1;
		else
			return 0;
	/*при тактирование от внешнего генератора HSE необходимо в файле system_stm32f10x.h раскомментить #define SYSCLK_FREQ_HSE    HSE_VALUE*/
}


//----------------------------функция инициализации таймера 3 для генерации ШИМ по 2 каналам------------------------------------------//
void PWM_timer3_ini ()
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM3, ENABLE); //включим таймер3
	RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOA, ENABLE);  //затактируем порт A

	//настройка GPIO для канала 1 и 2 ШИМ таймера 3//
	GPIO_InitTypeDef 	GPIO_PWM_tim3_init;
	GPIO_PWM_tim3_init.GPIO_Pin = (GPIO_Pin_6 | GPIO_Pin_7); //PA6 - channel 1, PA7 - channel 2 timer 3
	GPIO_PWM_tim3_init.GPIO_Mode = GPIO_Mode_AF_PP; //PA6 и PA7 на альт. выход с подтяжкой
	GPIO_PWM_tim3_init.GPIO_Speed = GPIO_Speed_2MHz; //скорость порта
	GPIO_Init (GPIOA, &GPIO_PWM_tim3_init);

	//базовые настройки таймер3//
	TIM_TimeBaseInitTypeDef timer3_base_ini;
	timer3_base_ini.TIM_Prescaler = (1600-1); //предделитель (частота таймера 16000000/1600 = 10000Hz)
	timer3_base_ini.TIM_CounterMode = TIM_CounterMode_Up ; //счёт на увеличение
	timer3_base_ini.TIM_Period = PERIOD; //значение перезагрузки TIM3, частота ШИМ = частота таймера/TIM_Period = 10000Hz/100 = 100 Hz. период ШИМ = 10 мс
	timer3_base_ini.TIM_ClockDivision = TIM_CKD_DIV1;
	timer3_base_ini.TIM_RepetitionCounter = 0;/*значение счетчика повторений RCR. Каждый раз, когда понижающий счетчик RCR достигает нуля, генерируется событие обновления, и подсчет перезапускается от значения RCR(N).
											Это означает, что в режиме ШИМ (N+1) соответствует или количеству периодов ШИМ в режиме выравнивания краев, или количеству половин периода ШИМ в режиме центрирования по центру*/
	TIM_TimeBaseInit(TIM3, &timer3_base_ini);

	//настройки для канала 1 ШИМ//
	TIM_OCInitTypeDef timer3_pwm_ini;
	timer3_pwm_ini.TIM_OCMode = TIM_OCMode_PWM1; //режим ШИМ1 (выравнивание по границе)
	timer3_pwm_ini.TIM_OutputState = TIM_OutputState_Enable; //активируем выход схемы сравнения канала 1 на GPIO
	timer3_pwm_ini.TIM_Pulse = duty_ch1; //длительность импульса ШИМ (период ШИМ*(TIM_Pulse/TIM_Period))
	timer3_pwm_ini.TIM_OCPolarity = TIM_OCPolarity_Low; //бит TIM_CCER_CC1P (active low, ШИМ будет инверсным)
	timer3_pwm_ini.TIM_OCIdleState = TIM_OCIdleState_Reset; //бит TIM_CR2_OIS1 (активация режима холостого хода)
	timer3_pwm_ini.TIM_OutputNState = TIM_OutputNState_Disable; //бит TIM_CCER_CC1NE (только для таймеров 1, 8)
	timer3_pwm_ini.TIM_OCNPolarity = TIM_OCNPolarity_High; //бит TIM_CCER_CC1NP (только для таймеров 1, 8-14)
	timer3_pwm_ini.TIM_OCNIdleState = TIM_OCNIdleState_Reset; //бит TIM_CR2_OIS1N (активация режима холостого хода OC1N)
	TIM_OC1Init(TIM3, &timer3_pwm_ini);

	//настройки для канала 2 ШИМ//
	timer3_pwm_ini.TIM_OCMode = TIM_OCMode_PWM1; //режим ШИМ1 (выравнивание по границе)
	timer3_pwm_ini.TIM_OutputState = TIM_OutputState_Enable; //активируем выход схемы сравнения канала 2 на GPIO
//	timer3_pwm_ini.TIM_OutputState = TIM_OutputState_Disable; //отключим выход схемы сравнения
	timer3_pwm_ini.TIM_Pulse = duty_ch2; //длительность импульса ШИМ (период ШИМ*(TIM_Pulse/TIM_Period))
	timer3_pwm_ini.TIM_OCPolarity = TIM_OCPolarity_High; //бит TIM_CCER_CC2P (active high)
	timer3_pwm_ini.TIM_OCIdleState = TIM_OCIdleState_Reset; //бит TIM_CR2_OIS2 (активация режима холостого хода)
	timer3_pwm_ini.TIM_OutputNState = TIM_OutputNState_Disable; //бит TIM_CCER_CC2NE (только для таймеров 1, 8)
	timer3_pwm_ini.TIM_OCNPolarity = TIM_OCNPolarity_High; //бит TIM_CCER_CC2NP (только для таймеров 1, 8, 9, 12)
	timer3_pwm_ini.TIM_OCNIdleState = TIM_OCNIdleState_Reset; //бит TIM_CR2_OIS2N (активация режима холостого хода OC2N)
	TIM_OC2Init(TIM3, &timer3_pwm_ini);

	TIM_Cmd(TIM3, ENABLE);
}

//--------------------------------------инициализация таймера 4 на отсчёт времени-----------------------------------------//
void timer4_ini ()
{
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4, ENABLE); //включим таймер4

	TIM_TimeBaseInitTypeDef timer4_base_ini;
	timer4_base_ini.TIM_Prescaler = (1600-1); //предделитель (частота таймера 16000000/1600 = 10000Гц)
	timer4_base_ini.TIM_CounterMode = TIM_CounterMode_Up ; //счёт на увеличение
	timer4_base_ini.TIM_Period = 100; //значение перезагрузки TIM4, прерывание TIM_Period/частота_TIM=100/10000=10 мс
	timer4_base_ini.TIM_ClockDivision = TIM_CKD_DIV1;
	timer4_base_ini.TIM_RepetitionCounter = 0;/*значение счетчика повторений RCR. Каждый раз, когда понижающий счетчик RCR достигает нуля, генерируется событие обновления, и подсчет перезапускается от значения RCR(N).
											Это означает, что в режиме ШИМ (N+1) соответствует или количеству периодов ШИМ в режиме выравнивания краев, или количеству половин периода ШИМ в режиме центрирования по центру*/
	TIM_TimeBaseInit(TIM4, &timer4_base_ini);

	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE); //включим прерывание от таймера 4 по переполнению
	NVIC_EnableIRQ(TIM4_IRQn);
	TIM_Cmd(TIM4, ENABLE);
}

//--------------------------------------инициализация ADC1-----------------------------------------//

void ADC1_ini ()
{
	RCC_ADCCLKConfig (RCC_PCLK2_Div2); //установка пределителя тактирования АЦП (ADC clock = PCLK2/2). Тактовая частота АЦП не более 14 МГц
	RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOA, ENABLE);  //затактируем порт A

	//настройка GPIO для канала 0 и 1 АЦП1
	GPIO_InitTypeDef 	GPIO_ADC_init;
	GPIO_ADC_init.GPIO_Pin = (GPIO_Pin_0); //PA0 - channel 0, PA1 - channel 1 ADC1
	GPIO_ADC_init.GPIO_Mode = GPIO_Mode_AIN; //на аналог. вход
	GPIO_ADC_init.GPIO_Speed = GPIO_Speed_2MHz; //скорость порта
	GPIO_Init (GPIOA, &GPIO_ADC_init);

	RCC_APB2PeriphClockCmd (RCC_APB2Periph_ADC1, ENABLE);
	ADC_InitTypeDef ADC1_ini;
	ADC1_ini.ADC_Mode = ADC_Mode_Independent; //независимый режим АЦП (для двух АЦП возможны варианты)
	ADC1_ini.ADC_ScanConvMode = DISABLE; //включение режим мультиканального опроса АЦП
	ADC1_ini.ADC_ContinuousConvMode = DISABLE; //включение режима циклического опроса канала АЦП
	ADC1_ini.ADC_ExternalTrigConv = ADC_ExternalTrigConv_None; //настраивает внешний триггер, используемый для запуска регулярных каналов АЦП
	ADC1_ini.ADC_DataAlign = ADC_DataAlign_Right; //выравнивание данных по левому или правому краю
	ADC1_ini.ADC_NbrOfChannel = 1; //количество каналов
	ADC_Init(ADC1, &ADC1_ini);

	ADC_ITConfig (ADC1, ADC_IT_EOC, ENABLE); //разрешение прерываний по окончанию преобразования АЦП
	NVIC_EnableIRQ (ADC1_2_IRQn); // разрешения прерываний АЦП1 в контроллере прерываний

	ADC_Cmd(ADC1, ENABLE); // включаем АЦП

}
