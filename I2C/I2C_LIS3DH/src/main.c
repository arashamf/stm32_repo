/**
  ******************************************************************************
  * @file    main.c
  * @author  Ac6
  * @version V1.0
  * @date    01-December-2013
  * @brief   Default main function.
  ******************************************************************************
*/
#include "stm32f4xx.h"
#include "I2C_ini.h"
#include "MCU_ini.h"
#include "stdio.h"
#include "stdlib.h"
			
#define DELAY 500
uint16_t delay_count = 0; //задержка

char OUT_XYZ[6]; // массив данных с акселерометра
uint8_t status = 0;
// регистры LIS3DSH
#define LIS3DSH_CTRL_REG4 0x20
#define LIS3DSH_CTRL_REG1 0x21
#define LIS3DSH_CTRL_REG2 0x22
#define LIS3DSH_CTRL_REG3 0x23
#define LIS3DSH_CTRL_REG5 0x24
#define LIS3DSH_CTRL_REG6 0x25
#define LIS3DSH_REG_STATUS 0x27
#define LIS3DSH_REG_OUT_X 0x28
#define LIS3DSH_REG_WHOAMI 0x0F

// режимы по частоте (настраиваютс€ в регистре 4)
#define LIS3DSH_F_3HZ_NORMAL 0x17 // 00010111
#define LIS3DSH_F_6HZ_NORMAL 0x27 // 00100111
#define LIS3DSH_F_800HZ_NORMAL 0x87 // 10000111
#define LIS3DSH_F_1600HZ_NORMAL 0x97 // 10010111

#define msg_SIZE 50
char msg [msg_SIZE]; // массив дл€ формировани€ сообщений дл€ вывода по UART

//функци€ обработки прерывани€ от таймера SysTick
void SysTick_Handler ()
{
	if (delay_count > 0) //пока значение delay_count больше 0,
		{
		delay_count--; //уменьшаем значение delay_count
		}
}

//функци€ передачи данных по UART2
void UART2_PutRAMString(char *str)
{
	char c;
	while((c = *str++)) // отправл€ем символы строки последовательно, пока не достигнут символ конца строки "\0"
		{
		while (!USART_GetFlagStatus(USART2, USART_FLAG_TC)) {}; //ждЄм выставлени€ флага "transmission complete"
		USART_SendData(USART2, c); // передаем байт
		}
}

//функци€ задержки
void delay_ms (uint16_t delay_temp)
	{
	delay_count = delay_temp;
	while (delay_count) {} //пока значение delay_count не равно 0, продолжаем ждать
	}


void LIS3DH_init(void)
{
// настройка акселерометра,
I2C1_write_byte(LIS3DSH_CTRL_REG4, LIS3DSH_F_3HZ_NORMAL); // Normal mode (3.125 Hz), все оси включены
I2C1_write_byte(LIS3DSH_CTRL_REG3, 0x88); //"0b10001000" сигнал готовности на INT1, сигнал готовности на INT1 - активный "0"
I2C1_write_byte(LIS3DSH_CTRL_REG5, 0x18); //"0b00011000" измер€емый диапазон +-8g
I2C1_write_byte(LIS3DSH_CTRL_REG6, 0x10); //включим инкремент адресов регистров
}

uint8_t LIS3DH_get_status (void)
{

return 0;
}

int main(void)
{
	// объ€влени€ дл€ PLL
	uint8_t RCC_PLLM = 8; // предделитель PLL
	uint32_t  RCC_PLLN = 192 ; // умножитель PLL
	uint32_t  RCC_PLLP = 0; // постделитель PLL = 2
	uint8_t CPU_WS = 3; //врем€ задержки дл€ работы с Flash пам€тью
	PLL_ini (CPU_WS, RCC_PLLM, RCC_PLLN, RCC_PLLP); //настройка генератора PLL (тактирование от HSE = 8MHz, SYSCLK = 96MHz, APB1=APB2=24MHz)
	SystemCoreClockUpdate ();  //обновим системную частоту (SYSCLK/8=12MHz)
	LEDS_ini (); //инициализируем LEDs
	UART2_ini (); //инициализируем UART2
	SysTick_Config (SystemCoreClock/1000);  //прерывани€ раз в 1 мс
	I2C1_init(); // инициализаци€ I2C1
	sprintf(msg,"I2C programm start\r\n");
	UART2_PutRAMString (msg);
	status = I2C1_read_byte (LIS3DSH_REG_WHOAMI);
	sprintf(msg,"Who_I_am=%x\r\n", status);
	UART2_PutRAMString (msg);
	status = 0;
	LIS3DH_init(); // инициализаци€ акселерометра LIS3DH
	delay_ms (DELAY);

	while (1)
	{
		status = I2C1_read_byte(LIS3DSH_REG_STATUS); //получим данные регистра статуса LIS3DSH
		sprintf(msg,"LIS3DSH status = %x\r\n", status);
		UART2_PutRAMString (msg);
		// провер€ем готовность данных акселерометра
		if(!(GPIOB->IDR & GPIO_IDR_IDR_9)) //если на PB9 в режиме входа логический "0"
		{
		I2C1_read_array(LIS3DSH_REG_OUT_X, OUT_XYZ, 6);
		sprintf(msg,"X - %.2x %.2x : Y - %2.2x %2.2x : Z - %2.2x %2.2x\r\n", OUT_XYZ[1], OUT_XYZ[0], OUT_XYZ[3], OUT_XYZ[2], OUT_XYZ[5], OUT_XYZ[4]);
		UART2_PutRAMString (msg); // тест
		}
		delay_ms (1000);
	}
}

