
#define stm32f4xx
#include "stm32f4xx.h"
#include "MCU_ini.h"

//-----------------функция инициализации пинов МК для светодиодов---------------------//
void LEDS_ini ()
{
	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOD, ENABLE); //затактируем порт D

	GPIO_InitTypeDef 	GPIO_Init_LED;
	GPIO_Init_LED.GPIO_Pin = GPIO_Pin_11 | GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init_LED.GPIO_Mode = GPIO_Mode_OUT; //порт D 12, 13, 14, 15 пин на обычный выход
	GPIO_Init_LED.GPIO_Speed = GPIO_Low_Speed; //скорость порта самая низкая
	GPIO_Init_LED.GPIO_OType = GPIO_OType_PP; //выход с подтяжкой (push-pull)
	GPIO_Init_LED.GPIO_PuPd = GPIO_PuPd_NOPULL; //вход без подтяжки

	GPIO_Init (GPIOD, &GPIO_Init_LED);
}

//------------------------функия инициализация порта IO ------------------------------//
void GPIO_trigger_init ()
{
	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOE, ENABLE); //затактируем порт E

	GPIO_InitTypeDef 	GPIO_Init_GPIO_trigger;
	GPIO_Init_GPIO_trigger.GPIO_Pin = GPIO_Pin_0;
	GPIO_Init_GPIO_trigger.GPIO_Mode = GPIO_Mode_OUT; //порт E 0 пин на обычный выход
	GPIO_Init_GPIO_trigger.GPIO_Speed = GPIO_Low_Speed; //скорость порта самая низкая
	GPIO_Init_GPIO_trigger.GPIO_OType = GPIO_OType_PP; //выход с подтяжкой (push-pull)
	GPIO_Init_GPIO_trigger.GPIO_PuPd = GPIO_PuPd_NOPULL; //вход без подтяжки

	GPIO_Init (GPIOE, &GPIO_Init_GPIO_trigger);
}

//-------------------------функция инициализации UART2-------------------------------//
void UART2_ini ()

	{
	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOA, ENABLE); //затактируем порт А

	GPIO_InitTypeDef 	GPIO_Init_UART2;
	GPIO_Init_UART2.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; //PA2 - Tx, PA3 - Rx
	GPIO_Init_UART2.GPIO_Mode = GPIO_Mode_AF; //порт А пин 2, 3 на альт. выход
	GPIO_Init_UART2.GPIO_Speed = GPIO_Fast_Speed; //скорость порта высокая
	GPIO_Init_UART2.GPIO_OType = GPIO_OType_PP; //выход с подтяжкой (push-pull)
	GPIO_Init_UART2.GPIO_PuPd = GPIO_PuPd_UP; //вход с подтяжкой вверх

	GPIO_Init (GPIOA, &GPIO_Init_UART2);

	GPIO_PinAFConfig (GPIOA, (GPIO_PinSource2), GPIO_AF_USART2); //установим альт. функцию UART2-TX
	GPIO_PinAFConfig (GPIOA, (GPIO_PinSource3), GPIO_AF_USART2); //установим альт. функцию UART2-RX

	RCC_APB1PeriphClockCmd (RCC_APB1Periph_USART2, ENABLE); //затактируем USART2

	USART_InitTypeDef ini_USART2;
	ini_USART2.USART_BaudRate = BAUDRATE; //пропускная способность канала UART
	ini_USART2.USART_WordLength = USART_WordLength_8b; //1 стартовый бит, 8 бит данных, n стоп-бит
	ini_USART2.USART_StopBits = USART_StopBits_1; //1 стоп-бит
	ini_USART2.USART_Parity = USART_Parity_No; //без проверки бита чётности
	ini_USART2.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;  //включим приём/передачу по USART2
	ini_USART2.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //аппаратное управление потоком отключено
	USART_Init(USART2, &ini_USART2); //запишем настройки USART2

//	USART_ITConfig(USART2, USART_IT_TXE, ENABLE); //включим прерывание  на передачу при пустом регистре данных
//	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); //включим прерывание на приём при полном регистре данных
//	USART_ClearITPendingBit(USART2, USART_IT_RXNE); //очистим флаг

//	NVIC_EnableIRQ(USART2_IRQn); //разрешим прерывание от USART2
//	__enable_irq ();  //разрешим прерывания глобально
	USART_Cmd(USART2, ENABLE);  //включим USART2
	}

//-------------------------функция инициализации генератора PLL------------------------------------------//
uint8_t PLL_ini (uint8_t CPU_WS, uint8_t RCC_PLLM, uint32_t RCC_PLLN, uint32_t RCC_PLLP)
{
	SET_BIT (RCC->CR, RCC_CR_HSEON); // включаем генератор HSE
	while(!(RCC->CR & RCC_CR_HSERDY)) {}; // ожидаем готовности генератор HSE к работе
	FLASH->ACR |= CPU_WS; // настройка задержки для работы с Flash памятью (RM0090 стр. 81)
	READ_REG(FLASH->ACR); //прочитаем регистр FLASH->ACR
	SET_BIT (RCC->CFGR, RCC_CFGR_SW_1); //10: системная частота с PLL
	SET_BIT (RCC->CFGR, RCC_CFGR_PPRE1_2 | RCC_CFGR_PPRE1_1 );// установим делитель шины APB1 - 8 (42 МГц макс. для APB1)
	SET_BIT (RCC->CFGR, RCC_CFGR_PPRE2_2 | RCC_CFGR_PPRE2_1 );// установим делитель шины APB2 - 8 (84 МГц макс. для APB2)
	RCC->PLLCFGR = RCC_PLLM + (RCC_PLLN << 6) + (RCC_PLLP << 16);//сдвигаем биты RCC_PLLN и RCC_PLLP, в данном случае: 8МГц*192/8/2=96МГц
	SET_BIT (RCC->PLLCFGR, RCC_PLLCFGR_PLLQ_3); //USB OTG FS clock frequency = VCO frequency / PLLQ, где 2 ≤ PLLQ ≤ 15 (48 МГц макс.)
	SET_BIT (RCC->PLLCFGR, RCC_PLLCFGR_PLLSRC_HSE); //установим внешний генератор (HSE) в качестве входного сигнала для PLL
	SET_BIT (RCC->CR, RCC_CR_PLLON); // включаем PLL
	while(!(RCC->CR & RCC_CR_PLLRDY)) {}; // ожидаем готовности PLL к работе
	if (READ_BIT (RCC->CFGR, RCC_CFGR_SWS) == 0x8) //считаем флаг статуса источника тактирования. Если 0x8, то источник PLL
			return 1;
		else
			return 0;
}
/*при тактирование от внешнего генератора HSE необходимо в файле stm32f4xx.h изменить величину #define HSE_VALUE    ((uint32_t)ХХХХХХХ),
где ХХХХХХХ - величина тактовой частоты внешнего генератора (кварца)*/

//---------------------------------------------------------------------------------//
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
