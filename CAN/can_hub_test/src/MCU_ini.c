
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

//-------------------------функция инициализации генератора PLL------------------------------------------//
uint8_t PLL_ini (uint8_t CPU_WS, uint8_t RCC_PLLM, uint32_t RCC_PLLN, uint32_t RCC_PLLP, uint32_t RCC_PLLQ)
{
//	RCC_DeInit(); //сбросим настройки RCC

	CLEAR_REG (RCC->CFGR); //очистим регистр CFGR
	RCC->CR &= (uint32_t)0xEAF6FFFF; //сброс HSEON, CSSON, PLLON, PLLI2S and PLLSAI bits
	CLEAR_REG (RCC->PLLCFGR); //очистим регистр настройки PLL

	SET_BIT (RCC->CR, RCC_CR_HSEON); // включаем генератор HSE
	while(!(RCC->CR & RCC_CR_HSERDY)) {}; // ожидаем готовности генератор HSE к работе

	SET_BIT (RCC->PLLCFGR, RCC_PLLCFGR_PLLSRC_HSE); //установим внешний генератор (HSE) в качестве входного сигнала для PLL
	RCC->PLLCFGR |= RCC_PLLM | (RCC_PLLN << 6) | (((RCC_PLLP >> 1) -1) << 16) | ( (RCC_PLLQ << 24))  ;//сдвигаем биты RCC_PLLN и RCC_PLLP, в данном случае: 8МГц*192/8/2=96МГц
//	SET_BIT (RCC->PLLCFGR, RCC_PLLCFGR_PLLQ_2); //PLLQ=2, USB_freq = Fin/PLLM*PLLN/PLLQ(48 МГц макс.)
	SET_BIT (RCC->CR, RCC_CR_PLLON); // включаем PLL
	while(!(RCC->CR & RCC_CR_PLLRDY)) {}; // ожидаем готовности PLL к работе

	SET_BIT (FLASH->ACR, CPU_WS); // настройка задержки для работы с Flash памятью (RM0090 стр. 81)
	READ_REG(FLASH->ACR); //прочитаем регистр FLASH->ACR

	SET_BIT (RCC->CFGR, RCC_CFGR_HPRE_3); // делитель AHB = 2, SYSClk/AHB_Div = HClk
	SET_BIT (RCC->CFGR, RCC_CFGR_PPRE1_2 ); // делитель шины APB1 = 2 (42 МГц макс. для APB1)
	SET_BIT (RCC->CFGR, RCC_CFGR_PPRE2_2); // делитель шины APB2 = 2 (84 МГц макс. для APB2)

	SET_BIT (RCC->CFGR, RCC_CFGR_SW_1); // системная частота с PLL

	if (READ_BIT (RCC->CFGR, RCC_CFGR_SWS) == 0x8) //считаем флаг статуса источника тактирования. Если 0x8, то источник PLL
			return 1;
		else
			return 0;
}
/*при тактирование от внешнего генератора HSE необходимо в файле stm32f4xx.h изменить величину #define HSE_VALUE    ((uint32_t)ХХХХХХХ),
где ХХХХХХХ - величина тактовой частоты внешнего генератора (кварца)*/

//-------------------------функция инициализации генератора PLL------------------------------------------//
uint8_t PLL_ini_SPL (uint8_t CPU_WS, uint8_t RCC_PLLM, uint32_t RCC_PLLN, uint32_t RCC_PLLP, uint8_t RCC_PLLQ)
{
	RCC_DeInit(); //сбросим настройки RCC
	RCC_HSEConfig(RCC_HSE_ON); // включаем генератор HSE
	while (!RCC_WaitForHSEStartUp()) {}; // ожидаем готовности генератор HSE к работе

	RCC_PLLConfig(RCC_PLLSource_HSE, (uint32_t) RCC_PLLM, (uint32_t) RCC_PLLN, (uint32_t) RCC_PLLP, (uint32_t) RCC_PLLQ); //настроим делители PLL, установим генератор HSE в качестве входного для PLL
	RCC_PLLCmd(ENABLE); // включаем PLL
	while(!(RCC_GetFlagStatus(RCC_FLAG_PLLRDY))) {}; // ожидаем готовности генератор PLL к работе

	SET_BIT (FLASH->ACR, CPU_WS); // настройка задержки для работы с Flash памятью (RM0090 стр. 81)
	READ_REG(FLASH->ACR); //прочитаем регистр FLASH->ACR

	RCC_HCLKConfig (RCC_SYSCLK_Div2); // установим делитель AHB = 2, SYSClk/AHB_Div = HClk
	RCC_PCLK1Config (RCC_HCLK_Div2); // делитель шины APB1 (42 МГц макс. для APB1)
	RCC_PCLK2Config (RCC_HCLK_Div2); // делитель шины APB2 (84 МГц макс. для APB2)

	RCC_SYSCLKConfig (RCC_SYSCLKSource_PLLCLK); // системная частота с PLL

	if ((RCC_GetSYSCLKSource()) == 0x8) //считаем флаг статуса источника тактирования. Если 0x8, то источник PLL
			return 1;
		else
			return 0;

}

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

//-------------------------функция инициализации внутреннего генератора HSI------------------------------------------//
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

//------------------------ф-я инициализации ЦАП1 и формирования на его выходе синусоиды-----------------------------//
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
	SET_BIT (PWR->CR, PWR_CR_CWUF); // сбросим флаг WUF (WUF = 1 событие пробуждения было получена от WKUP-пина или от Аларма, событий НСД, событий получения метки времени или событий RTC пробуждений)
	SET_BIT (SCB->SCR, SCB_SCR_SLEEPDEEP_Msk); //установка бита SLEEPDEEP (стр.230 PM0214)
	PWR->CR &= ~PWR_CR_PDDS; //0: вход в режим останова. Состояние регулятора напряжения зависит от бита LPDS
	PWR->CR &= ~PWR_CR_LPDS; //0: регулятор напряжения включен в режиме останова
//	PWR_EnterSTOPMode(PWR_MainRegulator_ON, PWR_STOPEntry_WFI);
}

/*--------------------------------------------------------------тайминги для памяти--------------------------------------------------------------------------------------
				    2.7 V - 3.6 V   2.4 V - 2.7 V     2.1 V - 2.4 V   1.8 V - 2.1 V
0 WS (1 CPU cycle)  0 <HCLK ≤ 30    0 <HCLK ≤ 24     0 <HCLK ≤ 22     0 < HCLK ≤ 20
1 WS (2 CPU cycles) 30 <HCLK ≤ 60   24 < HCLK ≤ 48   22 <HCLK ≤ 44    20 <HCLK ≤ 40
2 WS (3 CPU cycles) 60 <HCLK ≤ 90   48 < HCLK ≤ 72   44 < HCLK ≤ 66   40 < HCLK ≤ 60
3 WS (4 CPU cycles) 90 <HCLK ≤ 120  72 < HCLK ≤ 96   66 <HCLK ≤ 88    60 < HCLK ≤ 80
4 WS (5 CPU cycles) 120 <HCLK ≤ 150 96 < HCLK ≤ 120  88 < HCLK ≤ 110  80 < HCLK ≤ 100
5 WS (6 CPU cycles) 150 <HCLK ≤ 180 120 <HCLK ≤ 144  110 < HCLK ≤ 132 100 < HCLK ≤ 120
6 WS (7 CPU cycles) 				144 <HCLK ≤ 168  132 < HCLK ≤ 154 120 < HCLK ≤ 140
7 WS (8 CPU cycles) 				168 <HCLK ≤ 180  154 <HCLK ≤ 176  140 < HCLK ≤ 160
8 WS (9 CPU cycles) 								 176 <HCLK ≤ 180  160 < HCLK ≤ 168
-------------------------------------------------------------------------------------------------------------------------------------------------------*/
