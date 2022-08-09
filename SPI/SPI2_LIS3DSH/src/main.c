#include "MCU_ini.h"
#include "SPI.h"
#include "lisdsh.h"
#include "stm32f4xx.h"
#include "delay_lib.h"
#include "stdio.h"

#define DELAY 500 //задержка

char OUT_XYZ[6]; // массив данных с акселерометра
uint8_t status = 0;
uint8_t status_XYZ = 0;


#define msg_SIZE 50  //размер массива для формирования сообщений для отправки по UART
char msg [msg_SIZE]; // массив для формирования сообщений для отправки по UART

//функци€ передачи данных по UART2
void UART2_PutRAMString(char *str)
{
	char c;
	while((c = *str++)) // последовательно отправлем символы массива, пока не достигнут символ конца строки "\0"
		{
		while (!USART_GetFlagStatus(USART2, USART_FLAG_TC)) {}; //ждём выставлени€ флага "transmission complete" (передача завершена)
		USART_SendData(USART2, c); // передаем байт
		}
}

//---------------------ф-я€ инициализации датчика LIS3DH----------------------------//
void LIS3DH_init()
{
// настройка акселерометра,
SPI2_write_byte (LIS3DSH_CTRL_REG4, LIS3DSH_F_800HZ_NORMAL); // Normal mode (3.125 Hz), все оси включены
delay_us (20);
SPI2_write_byte(LIS3DSH_CTRL_REG5, 0x8); //"0b00011000" измер€емый диапазон +-4g
delay_us (20);
//SPI2_write_byte(LIS3DSH_CTRL_REG6, 0x10); //включим инкремент адресов регистров
}

int main(void)
{
	// объявления для PLL
	uint8_t RCC_PLLM = 8; // предделитель PLL
	uint32_t  RCC_PLLN = 192 ; // умножитель PLL
	uint32_t  RCC_PLLP = 0; // постделитель PLL = 2
	uint8_t CPU_WS = 3; //врем€ задержки дл€ работы с Flash пам€тью
	uint8_t status_PLL; //флаг готовности PLL
	status_PLL = PLL_ini (CPU_WS, RCC_PLLM, RCC_PLLN, RCC_PLLP); //настройка генератора PLL (тактирование от HSE = 8MHz, SYSCLK = 96MHz, APB1=APB2=24MHz)
	SystemCoreClockUpdate ();  //обновим системную частоту (SYSCLK/8=12MHz)
	LEDS_ini (); //инициализируем LEDs
	UART2_ini (); //инициализируем UART2
	if (status_PLL) //если генератор PLL завёлся правильно
		UART2_PutRAMString ("PLL ok\r\n");
	else
		UART2_PutRAMString ("PLL error\r\n");
	UART2_PutRAMString ("SPI2 programm start\r\n");
	SPI2_ini (); //инициализация SPI2
	delay_ms(DELAY);
	status = SPI2_read_byte (LIS3DSH_REG_WHOAMI); //считаем содержимое регистра "WHO AM I"
	sprintf(msg,"Who_I_am=%x\r\n", status);
	UART2_PutRAMString (msg);
	LIS3DH_init(); //инициализация акселеометра
	delay_us (DELAY/10); //подождём 50 мкс
	status = SPI2_read_byte (LIS3DSH_CTRL_REG4); //прочитаем регистр управления №4
	sprintf(msg,"CTRL_REG4=%x\r\n", status);
	UART2_PutRAMString (msg);
	status = SPI2_read_byte (LIS3DSH_CTRL_REG5);  //прочитаем регистр управления №5
	sprintf(msg,"CTRL_REG5=%x\r\n", status);
	UART2_PutRAMString (msg);

	while (1)
	{
		status_XYZ = SPI2_read_byte (LIS3DSH_REG_STATUS); //прочитаем регистр статуса преобразования
		sprintf(msg,"status_LISDSH=%x\r\n", status_XYZ);
		UART2_PutRAMString (msg);
		if (READ_BIT(status_XYZ, 1 << 4)) //если данные по осям XYZ обновились
			{
			GPIO_SetBits (GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
			SPI2_read_array(LIS3DSH_REG_OUT_X, OUT_XYZ, 6); //прочитаем 6 регистров с данными по осям XYZ
			sprintf(msg,"X - %.2x %.2x : Y - %2.2x %2.2x : Z - %2.2x %2.2x\r\n", OUT_XYZ[1], OUT_XYZ[0], OUT_XYZ[3], OUT_XYZ[2], OUT_XYZ[5], OUT_XYZ[4]);
			UART2_PutRAMString (msg);
			delay_ms (DELAY);
			GPIO_ResetBits (GPIOD, GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15);
			}
		status = SPI2_read_byte (LIS3DSH_REG_WHOAMI); //считаем содержимое регистра "WHO AM I"
		sprintf(msg,"Who_I_am=%x\r\n", status);
		UART2_PutRAMString (msg);
		delay_ms (DELAY);
	}
}

