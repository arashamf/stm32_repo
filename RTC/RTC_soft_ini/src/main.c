//-----------------------------------инициализация RTC CMSIS--------------------------------------//
#include "stdio.h"
#include "stm32f4xx.h"
#include "delay_lib.h"
#include "MCU_ini.h"

#define DELAY 1000 //задержка

#define msg_SIZE 50  //размер массива для формирования сообщений для отправки по UART
char msg [msg_SIZE]; // массив для формирования сообщений для отправки по UART

uint8_t seconds = 0; // секунды
uint16_t minuts = 0; // минуты
uint32_t hours = 0; // часы

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
	seconds = 55; // секунды
	minuts = 59; // минуты
	hours = 12; // часы
	WRITE_REG (RTC->TR, ((seconds - (seconds/10)*10) + ((seconds/10)<<4) + ((minuts - (minuts/10)*10)<<8) //где ((minuts - ((minuts/10)*10))<<8) - минутные единицы, записываемые в 8-11 бит, ((minuts/10)<<12) - минутные десятки, записываемые в 12-14 бит
	+ ((minuts/10)<<12) + ((hours - (hours/10)*10)<<16) + ((hours/10)<<20))); // можно записать регистр одним числом или собрать его из составляющих
	CLEAR_BIT (RTC->ISR, RTC_ISR_INIT); // сбрасываем бит инициализации
	while (READ_BIT (RTC->ISR, RTC_ISR_INITF)) {}; // ожидаем готовности
	WRITE_REG(RTC->WPR, 0xFF); //сбросим ключ защиты
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
//	LEDS_ini (); //инициализируем LEDs
	UART2_ini (); //инициализируем UART2
	if (status_PLL) //если генератор PLL завёлся правильно
		UART2_PutRAMString ("PLL ok\r\n");
	else
		UART2_PutRAMString ("PLL error\r\n");
	RTC_clock_ini ();
	UART2_PutRAMString ("RTC programm start\r\n");


	while (1)
	{
		seconds = ((READ_BIT (RTC->TR, 0x0070))>>4)*10 + (READ_BIT (RTC->TR, 0xF)); // получаем десятки и единицы секунд из соответсвующего регистра RTC
		minuts = ((READ_BIT (RTC->TR, 0x7000))>>12)*10 + ((READ_BIT(RTC->TR, 0xF00))>>8); // получаем десятки и единицы минут из соответсвующего регистра RTC
		hours = ((READ_BIT (RTC->TR, 0x300000))>>20)*10 + ((READ_BIT (RTC->TR, 0xF0000))>>16); // получаем десятки и единицы минут из соответсвующего регистра RTC
		sprintf(msg,"time: %2d-%2d-%2d\r\n", hours, minuts, seconds);
		UART2_PutRAMString (msg); // выводим строку по UART
		delay_ms (DELAY); // задержка 1 c
	}
}
