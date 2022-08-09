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
	RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOA, ENABLE);  //затактируем порт A

	GPIO_InitTypeDef 	GPIO_Init_LED;
	GPIO_Init_LED.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1;
	GPIO_Init_LED.GPIO_Mode = GPIO_Mode_Out_PP; //пины PA0 и PA1 на обычный выход с подтяжкой (push-pull)
	GPIO_Init_LED.GPIO_Speed = GPIO_Speed_2MHz; //скорость порта самая низкая

	GPIO_Init (GPIOA, &GPIO_Init_LED);
}

//-------------------------функция инициализации UART2-------------------------------//
void UART2_ini ()

	{
	RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOA, ENABLE);  //затактируем порт B

	GPIO_InitTypeDef 	GPIO_Init_UART2;
	GPIO_Init_UART2.GPIO_Pin = GPIO_Pin_2; //PA2 - Tx
	GPIO_Init_UART2.GPIO_Mode = GPIO_Mode_AF_PP; //PA2 на альт. выход с подтяжкой
	GPIO_Init_UART2.GPIO_Speed = GPIO_Speed_10MHz; //скорость порта
	GPIO_Init (GPIOA, &GPIO_Init_UART2);

	GPIO_Init_UART2.GPIO_Pin =  GPIO_Pin_3; //PA3 - Rx
	GPIO_Init_UART2.GPIO_Mode = GPIO_Mode_IN_FLOATING; //PA3 на плавающий выход
	GPIO_Init_UART2.GPIO_Speed = GPIO_Speed_10MHz; //скорость порта
	GPIO_Init (GPIOA, &GPIO_Init_UART2);

	GPIO_PinRemapConfig(GPIO_Remap_USART2, ENABLE);

	RCC_APB1PeriphClockCmd (RCC_APB1Periph_USART2, ENABLE); //затактируем USART2

	USART_InitTypeDef ini_USART2;
	ini_USART2.USART_BaudRate = BAUDRATE; //пропускная способность UART2
	ini_USART2.USART_WordLength = USART_WordLength_8b; //1 стартовый бит, 8 бит данных, n стоп-бит
	ini_USART2.USART_StopBits = USART_StopBits_1; //1 стоп-бит
	ini_USART2.USART_Parity = USART_Parity_No; //без проверки бита чётности
	ini_USART2.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;  //включим приём/передачу по USART2
	ini_USART2.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //аппаратное управление потоком отключено
	USART_Init(USART2, &ini_USART2); //запишем настройки USART2

/*	USART_ITConfig(USART2, USART_IT_TXE, ENABLE); //включим прерывание  на передачу при пустом регистре данных
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); //включим прерывание на приём при полном регистре данных
	NVIC_EnableIRQ(USART2_IRQn); //разрешим прерывание от USART2*/

	USART_Cmd(USART2, ENABLE);  //включим USART2
	}

//-------------------------функция инициализации генератора PLL------------------------------------------//
uint8_t PLL_ini (uint8_t CPU_WS, uint8_t RCC_PLLMUL)
{
	SET_BIT (RCC->CR, RCC_CR_HSEON); // включаем генератор HSE
	while(!(RCC->CR & RCC_CR_HSERDY)) {}; // ожидаем готовности генератор HSE к работе
//	CLEAR_BIT (RCC->CFGR, RCC_CFGR_PLLXTPRE_HSE_Div2); //очистим биты предделителя HSE
	SET_BIT (RCC->CFGR, RCC_CFGR_PLLXTPRE_HSE_Div2); //включим предделитель HSE равным 2 перед PLL

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
	SET_BIT (RCC->CFGR, (uint32_t)((RCC_PLLMUL-2) << 18));//сдвигаем биты RCC_PLLMUL, в данном случае  PLLMUL = 8; 8МГц/2*8=32МГц
	SET_BIT (RCC->CR, RCC_CR_PLLON); // включаем PLL
	while(!(RCC->CR & RCC_CR_PLLRDY)) {}; // ожидаем готовности PLL к работе

	SET_BIT (RCC->CFGR, RCC_CFGR_HPRE_3); //установим AHB делитель=2(SYSCLK/AHB_DIV=HCLK)
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
	RCC_APB2PeriphClockCmd (RCC_APB2Periph_GPIOA, ENABLE);  //затактируем порт B

	//настройка GPIO для канала 1 и 2 ШИМ таймера 3//
	GPIO_InitTypeDef 	GPIO_PWM_tim3_init;
	GPIO_PWM_tim3_init.GPIO_Pin = (GPIO_Pin_6 | GPIO_Pin_7); //PA6 - channel 1, PA7 - channel 2 timer 3
	GPIO_PWM_tim3_init.GPIO_Mode = GPIO_Mode_AF_PP; //PA6 на альт. выход с подтяжкой
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
	timer4_base_ini.TIM_Period = 100; //значение перезагрузки TIM3, прерывание TIM_Period/частота_TIM=100/10000=10 мс
	timer4_base_ini.TIM_ClockDivision = TIM_CKD_DIV1;
	timer4_base_ini.TIM_RepetitionCounter = 0;/*значение счетчика повторений RCR. Каждый раз, когда понижающий счетчик RCR достигает нуля, генерируется событие обновления, и подсчет перезапускается от значения RCR(N).
											Это означает, что в режиме ШИМ (N+1) соответствует или количеству периодов ШИМ в режиме выравнивания краев, или количеству половин периода ШИМ в режиме центрирования по центру*/
	TIM_TimeBaseInit(TIM4, &timer4_base_ini);

	TIM_ITConfig(TIM4, TIM_IT_Update, ENABLE); //включим прерывание от таймера 4 по переполнению
	NVIC_EnableIRQ(TIM4_IRQn);
	TIM_Cmd(TIM4, ENABLE);
}
