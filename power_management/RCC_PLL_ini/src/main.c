
#include "stm32f4xx.h"
#include "MCU_ini.h"
#include "stdio.h"

#define DELAY 500
uint32_t delay_count = 0;
//extern char message [15];

void flashing (uint16_t led)
{
	GPIO_SetBits (GPIOD, led);
	for(delay_count = 0; delay_count<0x7FFFF; delay_count++) {}
	GPIO_ResetBits (GPIOD, led);
	for(delay_count = 0; delay_count<0x7FFFF; delay_count++) {}
	GPIO_SetBits (GPIOD, led);
	for(delay_count = 0; delay_count<0x7FFFF; delay_count++) {}
	GPIO_ResetBits (GPIOD, led);
	for(delay_count = 0; delay_count<0x7FFFF; delay_count++) {}
	GPIO_SetBits (GPIOD, led);
	for(delay_count = 0; delay_count<0x7FFFF; delay_count++) {}
	GPIO_ResetBits (GPIOD, led);
	for(delay_count = 0; delay_count<0x7FFFF; delay_count++) {}
	GPIO_SetBits (GPIOD,led);
	for(delay_count = 0; delay_count<0x7FFFF; delay_count++) {}
	GPIO_ResetBits (GPIOD, led);
	for(delay_count = 0; delay_count<0x7FFFF; delay_count++) {}
}

//функция обработки прерывания от таймера SysTick
void SysTick_Handler ()
{
	if (delay_count > 0) //пока значение delay_count больше 0,
		{
		delay_count--; //уменьшаем значение delay_count
		}
}

//функция передачи данных по UART2
void UART2_PutRAMString(char *str)
{
	char c;
	while((c = *str++)) // отправляем символы строки последовательно, пока не достигнут символ конца строки "\0"
		{
		while (!USART_GetFlagStatus(USART2, USART_FLAG_TC)) {}; //ждём выставления флага "transmission complete"
		USART_SendData(USART2, c); // передаем байт
		}
}

//функция задержки
void delay_ms (uint16_t delay_temp)
	{
	delay_count = delay_temp;
	while (delay_count) {} //пока значение delay_count не равно 0, продолжаем ждать
	}



int main(void)
{
	uint8_t count = 0;
	uint8_t WS [] = {1, 1, 2, 5};
	// объявления для PLL. Настройка PLL для работы с HSE; HSE - 8 МГц, выход - 168 МГц
	uint8_t RCC_PLLM = 16; // предделитель PLL
	uint32_t RCC_PLLN = 192; // умножитель PLL
	uint32_t RCC_PLLP [] = {3, 2, 1, 0}; // постделитель PLL
	uint8_t flag_PLL = PLL_ini (WS [2], RCC_PLLM, RCC_PLLN, RCC_PLLP [3]);  //постделитель PLLP равен 2
	LEDS_ini ();
	UART2_ini (); //инициализируем UART2
	if (!flag_PLL)
		UART2_PutRAMString ("init PLL ERROR!\r\n");
	else
		UART2_PutRAMString ("init PLL complete\r\n");
	SysTick_Config (SystemCoreClock/1000);  //прерывания раз в 1 мс

	while (1)
	{
	PLL_ini (WS [count], RCC_PLLM, RCC_PLLN, RCC_PLLP [count]);
	GPIO_SetBits (GPIOD, GPIO_Pin_12);
	GPIO_ResetBits (GPIOD, GPIO_Pin_15);
	for(delay_count = 0; delay_count<0xFFFFF; delay_count++) {} // задержка
	UART2_PutRAMString ("green led\r\n");
	delay_ms (DELAY);
	GPIO_ResetBits (GPIOD, GPIO_Pin_12);
	GPIO_SetBits (GPIOD, GPIO_Pin_13);
	for(delay_count = 0; delay_count<0xFFFFF; delay_count++) {} // задержка
	UART2_PutRAMString ("orange led\r\n");
	delay_ms (DELAY);
	GPIO_ResetBits (GPIOD, GPIO_Pin_13);
	GPIO_SetBits (GPIOD, GPIO_Pin_14);
	for(delay_count = 0; delay_count<0xFFFFF; delay_count++) {} // задержка
	UART2_PutRAMString ("red led\r\n");
	delay_ms (DELAY);
	GPIO_SetBits (GPIOD, GPIO_Pin_15);
	GPIO_ResetBits (GPIOD, GPIO_Pin_14);
	for(delay_count = 0; delay_count<0xFFFFF; delay_count++) {} // задержка
	UART2_PutRAMString ("blue led\r\n");
	delay_ms (DELAY);
	switch (count)
		{
		case 0: flashing (GPIO_Pin_12);  // мигаем зелёным
				break;
		case 1: flashing (GPIO_Pin_13); //мигаем оранжевым
				break;
		case 2: flashing (GPIO_Pin_14); //мигаем красным
				break;
		case 3: flashing (GPIO_Pin_15); //мигаем синим
				break;
		default: break;
		}
	count++;
	if (count > 3)
		{
		count = 0;
		}

	}
}

/*
Для настройки PLL служит регистр «RCC_PLLCFGR». Частоту на выходе PLL, которая будет использоваться для основной тактовой частоты
(кроме этого выход PLL может быть использован для работы некоторых периферийных устройств, в частности USB) можно посчитать по формуле:
f(VCO clock) = f (PLL clock input) × (PLLN / PLLM)
f(PLL general clock output) = f (VCO clock) / PLLP
f(USB OTG FS, SDIO, RNG clock output) = f (VCO clock) / PLLQ
Fвых = Fвх* PLLN/PLLM/PLLP
где Fвых – выходная частота PLL, Fвх – входная частота PLL, PLLM – делитель входной частоты (предделитель),
PLLN – коэффициент умножения VCO в PLL, PLLP – делитель выходной частоты (постделитель)
Таким образом, имеется возможность широкой настройки выходной частоты PLL.
Для настройки конфигурации PLL также служат определенные биты в регистрах «RCC_CR» и «RCC_CFGR».
Алгоритм настройки работы PLL следующий:
1. Включить нужный генератор (HSI или HSE) и дождаться его готовности к работе.
2. Установить требуемые коэффициенты умножителя и делителей для получения требуемой частоты.
3. Установить соответствующий генератор как источник тактовых импульсов для PLL.
4. Включить PLL и дождаться его готовности к работе.
5. Изменить при необходимость задержки при работе с Flash памятью.
6. Установить выход с PLL как источник системной частоты микроконтроллера.

Необходимо учесть следующие ограничения и рекомендации производителя [2]:
- частота на выходе предделителя рекомендуется 1-2 МГц
- PLL работает нормально, если выходная частота умножителя VCO находится в пределах 192-432МГц
- постделитель имеет минимальное значение 2 (чтобы получить на выходе меандр).
Можно выбрать следующие коэффициенты: Fвых = 16МГц/16*336/2 = 168МГц
*/

/*Чтобы правильно считывать данные из флэш-памяти, количество состояний ожидания (латентность) должно быть правильно запрограммировано
в регистре управления доступом к флэш-памяти (FLASH_ACR) в соответствии с частотой тактовых импульсов процессора (HCLK) и напряжением
питания устройства.
 После сброса тактовая частота процессора составляет 16 МГц, а в регистре FLASH_ACR настроено 0 состояний ожидания (WS).
Настоятельно рекомендуется использовать следующие программные последовательности для настройки количества состояний ожидания, необходимых
для доступа к флэш-памяти с частотой процессора.
Увеличение частоты процессора
1. Запрограммируйте новое число состояний ожидания для битов задержки в регистре FLASH_ACR
2. Проверьте, что новое число состояний ожидания учитывается для доступа к флэш-памяти, прочитав регистр FLASH_ACR
3. Измените источник тактовой частоты процессора, записав SW-биты в регистр RCC_CFGR
4. При необходимости измените прескалер часов процессора, записав биты HPRE в RCC_CFGR
5. Проверьте, что новый источник тактовой частоты процессора или/и новое значение прескалера тактовой частоты процессора/приняты во внимание,
считывая состояние источника тактовой частоты (биты SWS) или/и значение прескалера AHB (биты HPRE), соответственно, в регистре RCC_CFGR.
Уменьшение частоты процессора
1. Измените источник тактовой частоты процессора, записав SW-биты в регистр RCC_CFGR
2. При необходимости измените прескалер часов процессора, записав биты HPRE в RCC_CFGR
3. Проверьте, что новый источник тактовой частоты процессора или/и новое значение прескалера тактовой частоты процессора/приняты во внимание путем чтения состояния источника тактовой частоты (SWS бит) или / и AHB
значение прескалера (бит HPRE), соответственно, в регистре RCC_CFGR
4. Запрограммируйте новое число состояний ожидания для битов задержки в FLASH_ACR
5. Проверьте, что новое число состояний ожидания используется для доступа к флэш-памяти, прочитав регистр FLASH_ACR*/
