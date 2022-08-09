
#define stm32f4xx
#include "stm32f4xx.h"
#include "MCU_ini.h"

//-----------------функция инициализации пинов МК для светодиодов---------------------//
void LEDS_ini ()
{
	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOD, ENABLE); //затактируем порт D

	GPIO_InitTypeDef 	GPIO_Init_LED;
	GPIO_Init_LED.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_Init_LED.GPIO_Mode = GPIO_Mode_OUT; //порт D 12, 13, 14, 15 пин на обычный выход
	GPIO_Init_LED.GPIO_Speed = GPIO_Low_Speed; //скорость порта самая низкая
	GPIO_Init_LED.GPIO_OType = GPIO_OType_PP; //выход с подтяжкой (push-pull)
	GPIO_Init_LED.GPIO_PuPd = GPIO_PuPd_NOPULL; //вход без подтяжки

	GPIO_Init (GPIOD, &GPIO_Init_LED);
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
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); //включим прерывание на приём при полном регистре данных
	USART_ClearITPendingBit(USART2, USART_IT_RXNE); //очистим флаг

	NVIC_EnableIRQ(USART2_IRQn); //разрешим прерывание от USART2
	__enable_irq ();  //разрешим прерывания глобально
	USART_Cmd(USART2, ENABLE);  //включим USART2
	}

//-------------------------функция инициализации UART3-------------------------------//
void UART3_ini ()

	{
	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOB, ENABLE); //затактируем порт B

	GPIO_InitTypeDef 	GPIO_Init_UART3;
	GPIO_Init_UART3.GPIO_Pin = GPIO_Pin_10 | GPIO_Pin_11; //PB10 - Tx, PB11 - Rx
	GPIO_Init_UART3.GPIO_Mode = GPIO_Mode_AF; //порт B пин 11, 12 на альт. выход
	GPIO_Init_UART3.GPIO_Speed = GPIO_Fast_Speed; //скорость порта высокая
	GPIO_Init_UART3.GPIO_OType = GPIO_OType_PP; //выход с подтяжкой (push-pull)
	GPIO_Init_UART3.GPIO_PuPd = GPIO_PuPd_UP; //вход с подтяжкой вверх
	GPIO_Init (GPIOB, &GPIO_Init_UART3);

	GPIO_PinAFConfig (GPIOB, (GPIO_PinSource10), GPIO_AF_USART3); //установим альт. функцию UART3-TX
	GPIO_PinAFConfig (GPIOB, (GPIO_PinSource11), GPIO_AF_USART3); //установим альт. функцию UART3-RX

	RCC_APB1PeriphClockCmd (RCC_APB1Periph_USART3, ENABLE); //затактируем USART3

	USART_InitTypeDef ini_USART3;
	ini_USART3.USART_BaudRate = BAUDRATE; //пропускная способность канала UART
	ini_USART3.USART_WordLength = USART_WordLength_8b; //1 стартовый бит, 8 бит данных, n стоп-бит
	ini_USART3.USART_StopBits = USART_StopBits_1; //1 стоп-бит
	ini_USART3.USART_Parity = USART_Parity_No; //без проверки бита чётности
	ini_USART3.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;  //включим приём/передачу по USART3
	ini_USART3.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //аппаратное управление потоком отключено
	USART_Init(USART3, &ini_USART3); //запишем настройки USART3

/*	USART_ITConfig(USART3, USART_IT_TXE, ENABLE); //включим прерывание  на передачу при пустом регистре данных
	USART_ITConfig(USART3, USART_IT_RXNE, ENABLE); //включим прерывание на приём при полном регистре данных
	USART_ClearITPendingBit(USART3, USART_IT_RXNE); //очистим флаг

	NVIC_EnableIRQ(USART3_IRQn); //разрешим прерывание от USART3
	__enable_irq ();  //разрешим прерывания глобально*/
	USART_Cmd(USART3, ENABLE);  //включим USART3
	}

//-------------------------функция инициализации генератора PLL------------------------------------------//
uint8_t PLL_ini (uint8_t CPU_WS, uint8_t RCC_PLLM, uint32_t RCC_PLLN, uint32_t RCC_PLLP)
{
	SET_BIT (RCC->CR, RCC_CR_HSEON); // включаем генератор HSE
	while(!(RCC->CR & RCC_CR_HSERDY)) {}; // ожидаем готовности генератор HSE к работе
	FLASH->ACR |= CPU_WS; // настройка задержки для работы с Flash памятью (RM0090 стр. 81)
	READ_REG(FLASH->ACR); //прочитаем регистр FLASH->ACR
	SET_BIT (RCC->CFGR, RCC_CFGR_SW_1); //10: системная частота с PLL
	SET_BIT (RCC->CFGR, RCC_CFGR_PPRE1_2 | RCC_CFGR_PPRE1_0 );// установим делитель шины APB1 - 4 (42 МГц макс. для APB1)
	SET_BIT (RCC->CFGR, RCC_CFGR_PPRE2_2 | RCC_CFGR_PPRE2_0 );// установим делитель шины APB2 - 4 (84 МГц макс. для APB2)
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

//-------------------------функция инициализации внешнего генератора HSE------------------------------------------//
uint8_t HSE_ini ()
{
	SET_BIT (RCC->CR, RCC_CR_HSEON); // включаем генератор HSE
	while(!(RCC->CR & RCC_CR_HSERDY)) {}; // ожидаем готовности генератор HSE к работе
	CLEAR_BIT (RCC->CFGR, RCC_CFGR_SW);
	SET_BIT (RCC->CFGR, RCC_CFGR_SW_0); //01: системная частота с HSE
	CLEAR_BIT (RCC->CFGR, RCC_CFGR_PPRE1);//без делителя шины APB1
	CLEAR_BIT (RCC->CFGR, RCC_CFGR_PPRE2 );//без делителя шины APB2
	if (READ_BIT (RCC->CFGR, RCC_CFGR_SWS) == 0x4) //считаем флаг статуса источника тактирования. Если 0x4, то источник HSE
			return 1;
		else
			return 0;
}

//-------------------------функция инициализации внунтреннего генератора HSI------------------------------------------//
uint8_t HSI_ini ()
{
	SET_BIT (RCC->CR, RCC_CR_HSION); // включаем генератор HSI
	while(!(RCC->CR & RCC_CR_HSIRDY)) {}; // ожидаем готовности генератор HSI к работе
	CLEAR_BIT (RCC->CFGR, RCC_CFGR_SW); //00: системная частота с HSI
	CLEAR_BIT (RCC->CFGR, RCC_CFGR_PPRE1);//без делителя шины APB1
	CLEAR_BIT (RCC->CFGR, RCC_CFGR_PPRE2 );//без делителя шины APB2
	if (READ_BIT (RCC->CFGR, RCC_CFGR_SWS) == 0x0) //считаем флаг статуса источника тактирования. Если 0x0, то источник HSI
			return 1;
		else
			return 0;
}

//------------------------ф-я инициализации ЦАП1 си формирования на его выходе синусоиды-----------------------------//
void DAC_ini ()
{
	RCC_APB1PeriphClockCmd (RCC_APB1Periph_DAC, ENABLE); //затактируем ЦАП

	DAC_InitTypeDef ini_DAC1;
	ini_DAC1.DAC_Trigger = DAC_Trigger_Software; //програмный триггер запуска ЦАП1
	ini_DAC1.DAC_WaveGeneration = DAC_WaveGeneration_None;  //без формирования шума/треугольника
	ini_DAC1.DAC_LFSRUnmask_TriangleAmplitude = DAC_TriangleAmplitude_1; //выбор амплитуды/маски треугольника/шума. В данном случае неважно
	ini_DAC1.DAC_OutputBuffer = DAC_OutputBuffer_Enable; //включим выходной буффер ЦАП1

	DAC_Init(DAC_Channel_1, &ini_DAC1);

	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOA, ENABLE); //затактируем порт А

	GPIO_InitTypeDef 	GPIO_Init_DAC; //настроим выходы для ЦАП1
	GPIO_Init_DAC.GPIO_Pin = GPIO_Pin_4 | GPIO_Pin_5; //PA4 - DAC_OUT1, PA5 - DAC_OUT2
	GPIO_Init_DAC.GPIO_Mode =  GPIO_Mode_AN; //порт А пин 4, 5 в аналоговый режим
	GPIO_Init_DAC.GPIO_Speed = GPIO_Low_Speed; //скорость порта низкая
	GPIO_Init_DAC.GPIO_OType = GPIO_OType_PP; //выход с подтяжкой (push-pull) (неважно)
	GPIO_Init_DAC.GPIO_PuPd =  GPIO_PuPd_NOPULL; //вход без подтяжки (неважно)

	GPIO_Init (GPIOA, &GPIO_Init_DAC);

	DAC_Cmd (DAC_Channel_1, ENABLE); //запустим ЦАП1
}

//-----------------------------ф-я настройки кнопки B1, подключённой к пину PA0 и вектора прерывания EXTI0------------------------------------------//
void ini_button_B1 ()
{
	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOA, ENABLE); //затактируем порт А

	GPIO_InitTypeDef 	GPIO_ini_button;
	GPIO_ini_button.GPIO_Pin = GPIO_Pin_0; //PA0
	GPIO_ini_button.GPIO_Mode = GPIO_Mode_IN; //порт А пин 2, 3 на альт. выход
	GPIO_ini_button.GPIO_Speed = GPIO_Low_Speed; //скорость порта низкая
	GPIO_ini_button.GPIO_OType = GPIO_OType_PP; //выход с подтяжкой (push-pull), не важно
	GPIO_ini_button.GPIO_PuPd = GPIO_PuPd_DOWN; //вход без подтяжки, т.к. есть внешняя
	GPIO_Init (GPIOA, &GPIO_ini_button);

	 RCC_APB2PeriphClockLPModeCmd(RCC_APB2Periph_SYSCFG, ENABLE); //подадим тактирование на SYSCFG

	EXTI_InitTypeDef ini_EXTI_button;
	ini_EXTI_button.EXTI_Line = EXTI_Line0; //настриваемая линия - EXTI0
	ini_EXTI_button.EXTI_Mode = EXTI_Mode_Interrupt; //режим прерывания
	ini_EXTI_button.EXTI_Trigger = EXTI_Trigger_Rising; //по нарастающему фронту
	ini_EXTI_button.EXTI_LineCmd = ENABLE; //включим линию EXTI0
	EXTI_Init (&ini_EXTI_button); //настроим прерывание по вектору EXTI0

	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource0); //настроим линию EXTI0 к пину PA0

	NVIC_EnableIRQ(EXTI0_IRQn); //разрешим прерывание по вектору EXTI0
	__enable_irq ();  //разрешим прерывания глобально*/
}

//------------------------------функция запуска режима останова----------------------------------//
void init_stop_mode ()
{
	SET_BIT (PWR->CR, PWR_CR_CWUF); // сбросим флаг WUF на всякий случай (WUF = 1 событие пробуждения было получена от WKUP-пина или от Аларма, событий НСД, событий получения метки времени или событий RTC пробуждений)
	SET_BIT (SCB->SCR, SCB_SCR_SLEEPDEEP_Msk); //установка бита SLEEPDEEP (стр.230 PM0214)
	PWR->CR &= ~PWR_CR_PDDS; //0: вход в режим останова. Состояние регулятора напряжения зависит от бита LPDS
	PWR->CR &= ~PWR_CR_LPDS; //0: регулятор напряжения включен в режиме останова*/
//	PWR_EnterSTOPMode(PWR_MainRegulator_ON, PWR_STOPEntry_WFI);
}
