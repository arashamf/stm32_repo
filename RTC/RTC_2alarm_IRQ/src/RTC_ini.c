#include "stm32f4xx.h"
#include "RTC_ini.h"
 //---------------------------ф-ия инициализации часов RTC CMSIS-------------------------------------//
void RTC_clock_ini ()
	{
	SET_BIT (RCC->CSR, RCC_CSR_LSION); // включим внутренний низкочастотный генератор LSI 32кГц
	while(!(READ_BIT (RCC->CSR, RCC_CSR_LSIRDY))) {}; // ожидаем готовности генератора LSI к работе*/
	//----------снимем защиту от записи регистра RTC-------------//
	SET_BIT (RCC->APB1ENR, RCC_APB1ENR_PWREN); //включаем тактирование интерфейса питания
	SET_BIT (PWR->CR, PWR_CR_DBP); // разрешим запись в регистры RTC
	SET_BIT (RCC->BDCR, RCC_BDCR_BDRST); //установка бита сброса резервного домена RTC
	RCC_BackupResetCmd(DISABLE); //сброс бит сброса резервного домена RTC
	//CLEAR_BIT (RCC->BDCR, RCC_BDCR_BDRST); //если сбрасывать через макрос, то программа зависает после отключения питания
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
uint8_t UART_RTC_set_time_SPL (char *buffer_RX)
{
	uint8_t flag_error = 1; //флаг ошибки. 0: ошибка данных, 1: данные корректны
	uint8_t second = 0, minut = 0,  hour = 0;
	if ((hour = (buffer_RX [0]-0x30)*10 + (buffer_RX [1]-0x30)) > 23) //вычтем 0x30, чтобы перевести знаки в цифры и проверим корректность полученных значений времени
		flag_error = 0;													//если данные некорректны, вернём флаг ошибки = 0
	if ((minut = (buffer_RX [2]-0x30)*10 + (buffer_RX [3]-0x30)) > 59)
		flag_error = 0;
	if ((second = (buffer_RX [4]-0x30)*10 + (buffer_RX [5]-0x30)) > 59)
		flag_error = 0;
	if (flag_error) //если флаг ошибки = 1, т.е. данные для установки времени корректны
		{			//заполним этими данными регистры времени RTC
		RTC_EnterInitMode(); //входим в режим инициализации часов

		RTC_set_time.RTC_Hours = hour; //установим часы
		RTC_set_time.RTC_Minutes = minut;  //установим минуты
		RTC_set_time.RTC_Seconds = second; //установим секунды
		RTC_set_time.RTC_H12 = RTC_H12_AM; //формат 24 часа
		RTC_SetTime(RTC_Format_BIN, &RTC_set_time); //установим значения времени RTC

		RTC_ExitInitMode(); // выходим из режима инициализации
		while((RCC_GetFlagStatus (RTC_FLAG_INITF) == SET)) {}; //ожидаем готовности
		}
	return flag_error; //вернём флаг ошибки
}

//---------------------------ф-ия установки времени даты RTC в SPL-------------------------------------//
void RTC_set_date_SPL ()
{
	uint8_t flag_error = 1; //флаг ошибки. 0: ошибка данных, 1: данные корректны
	uint8_t day = 03, month = 03,  year = 20;

	RTC_EnterInitMode(); //входим в режим инициализации часов

	RTC_set_date.RTC_WeekDay = RTC_Weekday_Monday; //установим день недели
	RTC_set_date.RTC_Month = month;  //установим месяц
	RTC_set_date.RTC_Date = day; //установим день
	RTC_set_date.RTC_Year = year; //год должен быть меньше или равен 99
	RTC_SetDate( RTC_Format_BIN , &RTC_set_date); //установим дату

	RTC_ExitInitMode(); // выходим из режима инициализации
	while((RCC_GetFlagStatus (RTC_FLAG_INITF) == SET)) {}; //ожидаем готовности
}


//---------------------------ф-ия инициализации будильника А RTC  с прерыванием в CMSIS-------------------------------------//
void ALARMA_ini_IRQ (uint8_t seconds)
{
WRITE_REG (RTC->WPR, 0xCA); // ключ защиты 1
WRITE_REG (RTC->WPR, 0x53); // ключ защиты 2
CLEAR_BIT (RTC->CR, RTC_CR_ALRAE); //отключим будильник А
while (!READ_BIT (RTC->ISR, RTC_ISR_ALRAWF));  //Этот бит устанавливается АО, когда значения Alarm A могут быть изменены
WRITE_REG (RTC->ALRMAR, (seconds -((seconds/10)*10) + ((seconds/10) << 4))); //запишем значения секунд
SET_BIT (RTC->ALRMAR, (RTC_ALRMAR_MSK2 | RTC_ALRMAR_MSK3 | RTC_ALRMAR_MSK4)); //замаскируем минуты, часы, дату
SET_BIT (RTC->CR, RTC_CR_ALRAE); //включим будильник А
SET_BIT (RTC->CR, RTC_CR_ALRAIE); //включим прерывание от аларма А
WRITE_REG(RTC->WPR, 0xFF); //сбросим ключ защиты
//настройки прерывания от Аларма А
SET_BIT (EXTI->IMR, EXTI_IMR_MR17); //включим линию EXTI №17 для Аларма
SET_BIT (EXTI->RTSR, EXTI_RTSR_TR17); //срабатывание линии 17 по нарастающему фронту
NVIC_EnableIRQ (RTC_Alarm_IRQn); //включим глобальное прерывание от аларма
}

//---------------------------ф-ия инициализации будильника B RTC  с прерыванием в CMSIS-------------------------------------//
void ALARMB_ini_IRQ (uint8_t seconds)
{
WRITE_REG (RTC->WPR, 0xCA); // ключ защиты 1
WRITE_REG (RTC->WPR, 0x53); // ключ защиты 2
CLEAR_BIT (RTC->CR, RTC_CR_ALRBE); //отключим будильник B
while (!READ_BIT (RTC->ISR, RTC_ISR_ALRBWF));  //Этот бит устанавливается АО, когда значения Alarm B могут быть изменены
WRITE_REG (RTC->ALRMBR, (seconds -((seconds/10)*10) + ((seconds/10) << 4))); //запишем значения секунд
SET_BIT (RTC->ALRMBR, (RTC_ALRMAR_MSK2 | RTC_ALRMAR_MSK3 | RTC_ALRMAR_MSK4)); //замаскируем минуты, часы, дату
SET_BIT (RTC->CR, RTC_CR_ALRBE); //включим будильник B
SET_BIT (RTC->CR, RTC_CR_ALRBIE); //включим прерывание от аларма B
WRITE_REG(RTC->WPR, 0xFF); //сбросим ключ защиты
//настройки прерывания от Аларма А
SET_BIT (EXTI->IMR, EXTI_IMR_MR17); //включим линию EXTI №17 для Аларма
SET_BIT (EXTI->RTSR, EXTI_RTSR_TR17); //срабатывание линии 17 по нарастающему фронту
NVIC_EnableIRQ (RTC_Alarm_IRQn); //включим глобальное прерывание от аларма
}

//---------------------------ф-ия инициализации будильника А RTC в SPL-------------------------------------//
void ALARM_ini_SPL (uint8_t seconds)
{

	RTC_TimeTypeDef RTC_ALARM_A_time; //структура для настройки времени
	RTC_ALARM_A_time.RTC_Hours = 0; //установим часы
	RTC_ALARM_A_time.RTC_Minutes = 0; //установим минуты
	RTC_ALARM_A_time.RTC_Seconds = seconds; //установим секунды
	RTC_ALARM_A_time.RTC_H12 = RTC_H12_AM; //формат 24 часа

	RTC_AlarmTypeDef RTC_ALARM_A_ini; //структуры для настройки АЛАРМА
	RTC_ALARM_A_ini.RTC_AlarmTime = RTC_ALARM_A_time; //выберем АЛАРМ А
	RTC_ALARM_A_ini.RTC_AlarmMask = RTC_AlarmMask_Minutes + RTC_AlarmMask_Hours + RTC_AlarmMask_DateWeekDay; //замаскируем дату, часы, минуты
	RTC_ALARM_A_ini.RTC_AlarmDateWeekDaySel = RTC_AlarmDateWeekDaySel_Date; //выбор: или день недели, или конкретную дату.В данном случае дата замаскирована
	RTC_ALARM_A_ini.RTC_AlarmDateWeekDay = RTC_Weekday_Monday;  //выбор конректного дня недели. В данном случае неважно
	RTC_SetAlarm (RTC_Format_BIN, RTC_Alarm_A, &RTC_ALARM_A_ini); //установим настройки АЛАРМ А

	RTC_AlarmCmd(RTC_Alarm_A, ENABLE); //включим АЛАРМ А

}
