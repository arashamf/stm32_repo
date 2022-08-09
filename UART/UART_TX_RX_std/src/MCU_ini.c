#define stm32f4xx
#include "stm32f4xx.h"
#include "MCU_ini.h"
char message1 [20];
char message2 [20];

//функция инициализации пинов МК для светодиодов
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

void UART2_ini ()

	{
	RCC_AHB1PeriphClockCmd (RCC_AHB1Periph_GPIOA, ENABLE); //затактируем порт А

	GPIO_InitTypeDef 	GPIO_Init_UART2;
	GPIO_Init_UART2.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3; //выберем пин 2 и 3 порта А для настройки
	GPIO_Init_UART2.GPIO_Mode = GPIO_Mode_AF; //порт А 2, 3 пин на альт. выход
	GPIO_Init_UART2.GPIO_Speed = GPIO_Fast_Speed; //скорость порта самая низкая
	GPIO_Init_UART2.GPIO_OType = GPIO_OType_PP; //выход с подтяжкой (push-pull)
	GPIO_Init_UART2.GPIO_PuPd = GPIO_PuPd_UP; //вход с подтяжкой вверх

	GPIO_Init (GPIOA, &GPIO_Init_UART2);

	GPIO_PinAFConfig (GPIOA, (GPIO_PinSource3), GPIO_AF_USART2);
	GPIO_PinAFConfig (GPIOA, (GPIO_PinSource2 ), GPIO_AF_USART2);

	RCC_APB1PeriphClockCmd (RCC_APB1Periph_USART2, ENABLE); //затактируем USART2

	USART_InitTypeDef ini_USART2;
	ini_USART2.USART_BaudRate = BAUDRATE;  //
	ini_USART2.USART_WordLength = USART_WordLength_8b; //1 стартовый бит, 8 бит данных, n стоп-бит
	ini_USART2.USART_StopBits = USART_StopBits_1; //1 стоп-бит
	ini_USART2.USART_Parity = USART_Parity_No; //без проверки бита чётности
	ini_USART2.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;  //включим приём/передачу по USART2
	ini_USART2.USART_HardwareFlowControl = USART_HardwareFlowControl_None; //аппаратное управление потоком отключено
	USART_Init(USART2, &ini_USART2); //запишем настройки USART2

//	USART_ITConfig(USART2, USART_IT_TXE, ENABLE); //включим прерывание  на передачу при пустом регистре данных
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE); //включим прерывание на приём при полном регистре данных
	NVIC_EnableIRQ(USART2_IRQn); //разрешим прерывание от USART2

	USART_Cmd(USART2, ENABLE);  //включим USART2
	}

void DMA1_ini ()
	{
	 RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_DMA1, ENABLE);

	 DMA_InitTypeDef ini_DMA1_UART2;
	 ini_DMA1_UART2.DMA_Channel = DMA_Channel_4; //номер канала
	 ini_DMA1_UART2.DMA_PeripheralBaseAddr = (uint32_t) &(USART2->DR); //указатель на регистр памяти перифирии: буффер USART2
	 ini_DMA1_UART2.DMA_Memory0BaseAddr = (uint32_t) message1; //указатель на массив передаваемых данных; указатель на массив есть название массива
	 ini_DMA1_UART2.DMA_DIR = DMA_DIR_MemoryToPeripheral; //направление копирования, из памяти в периферию
	 ini_DMA1_UART2.DMA_BufferSize = sizeof (message1); //размер буффера DMA1
	 ini_DMA1_UART2.DMA_PeripheralInc = DMA_PeripheralInc_Disable; //без увеличения (инкремента) указателя на регистр памяти перифирии
	 ini_DMA1_UART2.DMA_MemoryInc = DMA_MemoryInc_Enable;  //увеличение (инкремент) указателя на массив передаваемых данных
	 ini_DMA1_UART2.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte; //величина на которую увеличивается указатель на регистр данных USART2 - 1byte
	 ini_DMA1_UART2.DMA_MemoryDataSize = DMA_PeripheralDataSize_Byte; //величина на которую увеличивается указатель на массив передаваемых данных - 1byte
	 ini_DMA1_UART2.DMA_Mode = DMA_Mode_Normal; //нормальный режим передачи данных DMA, круговой режим выключен
	 ini_DMA1_UART2.DMA_Priority = DMA_Priority_High; //приоритет DMA1 высокий
	 ini_DMA1_UART2.DMA_FIFOMode = DMA_FIFOMode_Disable; //режим FIFO отключен
	 ini_DMA1_UART2.DMA_FIFOThreshold = DMA_FIFOThreshold_1QuarterFull; //Выбор порога FIFO, в данном случае не важен
	 ini_DMA1_UART2.DMA_MemoryBurst = DMA_MemoryBurst_Single; //конфигурация передачи пакетов памяти: пакетная передача отключена
	 ini_DMA1_UART2.DMA_PeripheralBurst = DMA_PeripheralBurst_Single; //конфигурация передачи пакетов перефирии: пакетная передача отключена

	 DMA_Init (DMA1_Stream6, &ini_DMA1_UART2); //инициализация DMA1 Stream6 (USART2_TX)
	 USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE); //включаем интерфейс DMA USART2_TX

	 DMA_DoubleBufferModeConfig (DMA1_Stream6, (uint32_t) message2, (uint32_t) message1);  //зададим адрес второго буффера
//	 DMA_DoubleBufferModeCmd(DMA1_Stream6, ENABLE); //включим режим второго буффера

	 DMA_ITConfig(DMA1_Stream6, DMA_IT_TC, ENABLE); //включаем прерывание при полной передачи потока 6 DMA1
	 NVIC_EnableIRQ (DMA1_Stream6_IRQn); //разрешим прерывание от потока 6 DMA1
	}

void PLL_ini (uint8_t CPU_WS, uint8_t RCC_PLLM, uint32_t RCC_PLLN, uint32_t RCC_PLLP)
{
	FLASH->ACR = CPU_WS; // настройка задержки для работы с Flash памятью (RM0090 стр. 81)
	READ_REG(FLASH->ACR);
	SET_BIT (RCC->CR, RCC_CR_HSEON); // включаем генератор HSE
	while(!(RCC->CR & RCC_CR_HSERDY)) {}; // ожидаем готовности генератор HSE к работе
	// сдвигаем биты RCC_PLLN и RCC_PLLP для размещения их в нужном месте регистра настройки
	RCC->PLLCFGR = RCC_PLLM + (RCC_PLLN << 6) + (RCC_PLLP << 16); 	// 16МГц*336/16/2=168МГц
	SET_BIT (RCC->PLLCFGR, RCC_PLLCFGR_PLLSRC_HSE); //установим внешний генератор (HSE) в качестве входного сигнала для PLL
	SET_BIT (RCC->CR, RCC_CR_PLLON); // включаем PLL
	while(!(RCC->CR & RCC_CR_PLLRDY)) {}; // ожидаем готовности PLL к работе
	SET_BIT (RCC->CFGR, RCC_CFGR_PPRE1_2 | RCC_CFGR_PPRE1_0);// установим делитель шины APB1 - 4 (168 МГц/ (42 МГц макс. для шины APB1) = 4)
	SET_BIT (RCC->CFGR, RCC_CFGR_PPRE2_2 | RCC_CFGR_PPRE2_0);// установим делитель шины APB2 - 4 (168 МГц/ 42 (84 МГц макс. для APB2) = 4)
	SET_BIT (RCC->CFGR, RCC_CFGR_SW_1); // установим системную частоту с PLL (при сбросе МК она устанавливается с HSI)
}


