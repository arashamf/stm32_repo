//-----------------------------------инициализация RTC c установкой по UART--------------------------------------//
#include "stdio.h"
#include "stm32f4xx.h"
#include "delay_lib.h"
#include "MCU_ini.h"

#define DELAY 1000 //задержка

#define msg_SIZE 50  //размер массива для формирования сообщений для отправки по UART
char msg [msg_SIZE]; // массив для формирования сообщений для отправки по UART
char recieve_msg [msg_SIZE]; // массив с принятыми данными по UART
uint8_t receive_flag = 0;
uint8_t recieve_msg_count = 0;
static RTC_TimeTypeDef RTC_set_time; //инициализируем шаблон структуры

//функци¤ передачи данных по UART2
void UART2_PutRAMString(char *str)
{
	char c;
	while((c = *str++)) // последовательно отправлем символы массива, пока не достигнут символ конца строки "\0"
		{
		while (!USART_GetFlagStatus(USART2, USART_FLAG_TC)) {}; //ждём выставлени¤ флага "transmission complete" (передача завершена)
		USART_SendData(USART2, c); // передаем байт
		}
}

//функция обработки прерывания от UART2 при приёме
void USART2_IRQHandler ()
{
	if (USART_GetITStatus(USART2, USART_IT_RXNE)) //если установлен флаг USART_IT_RXNE
	{
		recieve_msg [recieve_msg_count] = USART_ReceiveData(USART2);  //записываем в массив принятые данные
		recieve_msg_count++;  //увеличиваем указатель
		USART_ClearITPendingBit(USART2, USART_IT_RXNE); //очистим флаг
		if((recieve_msg[recieve_msg_count-2] == 0x0D) && (recieve_msg[recieve_msg_count-1] == 0x0A)) //проверка наличия символов '\r' и '\n'
		{
			receive_flag = 1; // 1- сообщение принято полностью
			recieve_msg_count = 0; //обнулим указатель на элемент массива с принятым сообщением
		}
	}
}
//---------------------------ф-ия инициализации часов RTC CMSIS-------------------------------------//
void RTC_clock_ini ()
	{
	SET_BIT (RCC->CSR, RCC_CSR_LSION); // включим внутренний низкочастотный генератор LSI 32кГц
	while(!(READ_BIT (RCC->CSR, RCC_CSR_LSIRDY))) {}; // ожидаем готовности генератора LSI к работе*/
	/*SET_BIT (RCC->BDCR, RCC_BDCR_LSEON); //включим внешний часовой кварц LSE 32768 Гц
	while(!(READ_BIT (RCC->BDCR, RCC_BDCR_LSERDY))) {}; //ожидаем готовности генератора LSI к работе*/
	//----------снимем защиту от записи регистра RTC-------------//
	SET_BIT (RCC->APB1ENR, RCC_APB1ENR_PWREN); //включаем тактирование интерфейса питания
	SET_BIT (PWR->CR, PWR_CR_DBP); // разрешим запись в регистры RTC
	SET_BIT (RCC->BDCR, RCC_BDCR_BDRST); //установка бита сброса резервного домена RTC
	RCC_BackupResetCmd(DISABLE); //сброс бит сброса резервного домена RTC
	//CLEAR_BIT (RCC->BDCR, RCC_BDCR_BDRST); //если сбрасывать через макрос, то программа зависает после отключения питания
	//SET_BIT (RCC->BDCR, RCC_BDCR_RTCSEL_0); //выберем в качестве источника тактирования генератор LSE
	SET_BIT (RCC->BDCR, RCC_BDCR_RTCSEL_1); //выберем в качестве источника тактирования генератор LSI (внутренний)
	SET_BIT (RCC->BDCR, RCC_BDCR_RTCEN); //включим тактирование RTC
	// специальная последовательность снятия запрета обновления регистров RTC
	WRITE_REG(RTC->WPR, 0xCA); // ключ защиты 1
	WRITE_REG(RTC->WPR, 0x53); // ключ защиты 2
	// инициализация RTC
	while(!(RTC->ISR & RTC_ISR_RSF)) {}; //прочитаем бит RSF (бит устанавливается аппаратно, когда календарные регистры копируются в теневые регистры (RTC_SSRx, RTC_TRx и RTC_DRx). Этот бит сбрасывается аппаратно в режиме инициализации, в то время как операция сдвига находится в состоянии ожидания (SHPF=1), или в режиме обхода блокировки теневого регистра (BYPSHAD=1). Этот бит также может быть очищен ПО. 0: теневые регистры календаря еще не синхронизированы. 1: синхронизация теневых регистров календаря)
	SET_BIT (RTC->ISR, RTC_ISR_INIT); //входим в режим инициализации часов
	while(!(RTC->ISR & RTC_ISR_INITF)) {}; // ожидаем готовности
	// синхронный делитель на 15999 ck_spre frequency = ck_apre frequency/(PREDIV_S+1), асинхронный делитель - на 2,
	// !асинхронный делитель не может быть равен 0, выбрано значение 1 (деление на 2 - ck_apre frequency = RTCCLK frequency/(PREDIV_A+1))
	WRITE_REG (RTC->PRER, (0x3E7F + 0x1000));  //где  PREDIV_A = 15999 (0x3E7F), PREDIV_S = 1 (0x10000)
	CLEAR_BIT (RTC->CR, RTC_CR_FMT); // формат 24 часа
	CLEAR_BIT (RTC->TR, RTC_TR_PM); //0: 0-23-hour format; 1: 0-12 hour format
	uint8_t seconds = 55; // секунды
	uint16_t minuts = 59; // минуты
	uint32_t hours = 12; // часы
	WRITE_REG (RTC->TR, ((seconds - (seconds/10)*10) + ((seconds/10)<<4) + ((minuts - (minuts/10)*10)<<8) //где ((minuts - ((minuts/10)*10))<<8) - минутные единицы, записываемые в 8-11 бит, ((minuts/10)<<12) - минутные десятки, записываемые в 12-14 бит
	+ ((minuts/10)<<12) + ((hours - (hours/10)*10)<<16) + ((hours/10)<<20))); // можно записать регистр одним числом или собрать его из составляющих
	CLEAR_BIT (RTC->ISR, RTC_ISR_INIT); // сбрасываем бит инициализации
	while (READ_BIT (RTC->ISR, RTC_ISR_INITF)) {}; // ожидаем готовности
	WRITE_REG(RTC->WPR, 0xFF); //сбросим ключ защиты
	}

//---------------------------ф-ия инициализации часов RTC в SPL-------------------------------------//
void RTC_clock_ini_SPL ()
{
	RCC_LSICmd(ENABLE); //включим внутренний низкочастотный генератор LSI 32кГц
	while((RCC_GetFlagStatus (RCC_FLAG_LSIRDY) == RESET)) {}; // ожидаем готовности генератора LSI к работе
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE); //включаем тактирование интерфейса питания
	PWR_BackupAccessCmd(ENABLE); // разрешим запись в регистры RTC
	RCC_RTCCLKConfig (RCC_RTCCLKSource_LSI); //выберем в качестве источника тактирования генератор LSI (внутренний)
	RCC_RTCCLKCmd(ENABLE); //включим тактирование RTC
//	RCC_BackupResetCmd(ENABLE); //установим бит RCC_BDCR_BDRST, который обнуляет значения регистров RTC
//	RCC_BackupResetCmd(DISABLE); //сбросим бит RCC_BDCR_BDRST
	RTC_EnterInitMode(); //входим в режим инициализации часов

	RTC_InitTypeDef RTC_clock24_ini;
	RTC_clock24_ini.RTC_HourFormat = RTC_HourFormat_24; //Specifies the RTC Hour Format
	RTC_clock24_ini.RTC_AsynchPrediv = 0x1; //!асинхронный делитель не может быть равен 0, выбрано значение 2-1=1
	RTC_clock24_ini.RTC_SynchPrediv = 0x3E7F;  //синхронный делитель 16000-1
	RTC_Init(&RTC_clock24_ini);

	RTC_TimeStructInit (&RTC_set_time); //заполним значения времени нулями
	RTC_SetTime(RTC_Format_BIN, &RTC_set_time); //установим значения времени RTC

	RTC_ExitInitMode(); // выходим из режима инициализации
	while((RCC_GetFlagStatus (RTC_FLAG_INITF) == SET)) {}; //ожидаем готовности
}

//---------------------------ф-ия установки времени часов RTC в SPL-------------------------------------//
char UART_RTC_set_time (char *buffer_RX)
{
	uint8_t flag_error = 1; //флаг ошибки. 0: ошибка данных, 1: данные корректны
	uint8_t seconds = 0, minuts = 0,  hours = 0;
	if ((hours = (buffer_RX [0]-0x30)*10 + (buffer_RX [1]-0x30)) > 23) //вычтем 0x30, чтобы перевести знаки в цифры и проверим корректность полученных значений времени
		flag_error = 0;													//если данные некорректны, вернём флаг ошибки = 0
	if ((minuts = (buffer_RX [2]-0x30)*10 + (buffer_RX [3]-0x30)) > 59)
		flag_error = 0;
	if ((seconds = (buffer_RX [4]-0x30)*10 + (buffer_RX [5]-0x30)) > 59)
		flag_error = 0;
	if (flag_error) //если флаг ошибки = 1, т.е. данные для установки времени корректны
		{			//заполним этими данными регистры времени RTC
		RTC_EnterInitMode(); //входим в режим инициализации часов
		RTC_set_time.RTC_Hours = hours; //установим часы
		RTC_set_time.RTC_Minutes = minuts;  //установим минуты
		RTC_set_time.RTC_Seconds = seconds; //установим секунды
		RTC_set_time.RTC_H12 = RTC_H12_AM; //формат 24 часа
		RTC_SetTime(RTC_Format_BIN, &RTC_set_time); //установим значения времени RTC
		RTC_ExitInitMode(); // выходим из режима инициализации
		while((RCC_GetFlagStatus (RTC_FLAG_INITF) == SET)) {}; //ожидаем готовности
		}
	return flag_error; //вернём флаг ошибки
}

//---------------------------ф-ия получения и проверки настроек времени RTC-------------------------------------//
void check_and_set_time ()
{
	uint8_t count = 0; //значение счётчика
	while (!receive_flag) //цикл опроса пользователя, до тех пока пока не приняты корректные данные
	{
		if (((count++) % 4) == 0) //выводим сообщения при count = 4, т.е. раз в 2 с
			{
			UART2_PutRAMString ("please, set time\r\n");
			UART2_PutRAMString ("example: 121543\r\n");
			}
		GPIO_WriteBit(GPIOD, (GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15), Bit_SET); //помигаем светодиодами
		delay_ms (DELAY/4); // задержка 1/4 c
		GPIO_WriteBit(GPIOD, (GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15), Bit_RESET);
		delay_ms (DELAY/4); // задержка 1/4 c
		if (receive_flag) //если флаг принятия данных по UART установлен
			{
			if (UART_RTC_set_time (recieve_msg)) //если функция установки не вернула ошибку
				{
				receive_flag = 0; //обнулим флаг принятия данных
				break;			 //выйдем из цикла опроса
				}
			else    			//если функция установки времени вернуло 0
				{
				UART2_PutRAMString ("data time error!\r\n"); //предупредим об ошибке и продолжим цикл опроса
				}
			receive_flag = 0; //обнулим флаг принятия данных
			}
		if (count == 4) //если счётчик равен 4
			count = 0; //сбросим значение счётчика
	}
}


int main(void)
{
	// объявления для PLL (8/8х192/2=96 MHz)
	uint8_t RCC_PLLM = 8; // предделитель PLL
	uint16_t  RCC_PLLN = 192 ; // умножитель PLL
	uint32_t  RCC_PLLP = 0; // постделитель PLL = 2
	uint8_t CPU_WS = 3; //врем¤ задержки для работы с Flash памятью
	uint8_t status_PLL; //флаг готовности PLL
	status_PLL = PLL_ini (CPU_WS, RCC_PLLM, RCC_PLLN, RCC_PLLP); //настройка генератора PLL (тактирование от HSE = 8MHz, SYSCLK = 96MHz, APB1=APB2=24MHz)
	SystemCoreClockUpdate ();  //обновим системную частоту (SYSCLK/8=12MHz)
	LEDS_ini (); //инициализируем LEDs
	UART2_ini (); //инициализируем UART2
	if (status_PLL) //если генератор PLL завёлся правильно
		UART2_PutRAMString ("PLL ok\r\n");
	else
		UART2_PutRAMString ("PLL error\r\n");
	RTC_clock_ini_SPL (); //инициализация RTC
	UART2_PutRAMString ("RTC programm start\r\n");
	check_and_set_time (); //висим в этой функции, пока не будут получены корректные данные
	NVIC_DisableIRQ(USART2_IRQn); //отключим прерывание от USART2

	while (1)
	{
		RTC_GetTime (RTC_Format_BIN, &RTC_set_time); //заполним структуры текущими данными времени (ч, м, с)
		sprintf(msg,"time: %2d:%2d:%2d\r\n", RTC_set_time.RTC_Hours, RTC_set_time.RTC_Minutes, RTC_set_time.RTC_Seconds);
		UART2_PutRAMString (msg); // выводим строку по UART
		delay_ms (DELAY); // задержка 1 c
	}
}
