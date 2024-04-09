/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "spi.h"
#include "tim.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "string.h"
#include "st7735.h"
#include "typedef.h"
#include "Font.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */
char LCD_str_buffer[16];
__IO int32_t prevCounter = 0;
__IO int32_t currCounter = 0;

const uint8_t ROWS = 4; //число строк у нашей клавиатуры
const uint8_t COLS = 3; //число столбцов у нашей клавиатуры

uint32_t key_status = 0;
uint32_t tmp_count = 0;
	
struct KEY_MACHINE_t
{
	KEY_CODE_t 		key_code;
	KEY_STATE_t 	key_state;
} ;

struct KEY_MACHINE_t KEY_MACHINE;

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void encoder_init(void);
void loop(void);
uint32_t scan_keys (void);
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_SPI1_Init();
  MX_TIM1_Init();
  /* USER CODE BEGIN 2 */
	
	encoder_init();
	
	lcdInit();
	ClearLcdMemory();
	LCD_SetFont (Arial_15x17, black);
	LCD_ShowString (3,0,"start...");
	LCD_Refresh();
	
	LED_RED(OFF);
	
	DIR_DRIVE(BACKWARD);
	DRIVE_ENABLE(OFF);
	
	KEY_MACHINE.key_code = NO_KEY;
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {		
		{
			for (uint32_t count = 0; count < 0x4FFFF; count++)			
			{
				loop();
				if ((tmp_count = scan_keys()) != NO_KEY)
				{
					key_status = tmp_count;
				}	
			}
			snprintf(LCD_str_buffer, sizeof(LCD_str_buffer), "key=%x", key_status);
			ClearLcdMemory();
			LCD_ShowString(3, 10, LCD_str_buffer);
			LCD_Refresh();
			TOOGLE_LED_RED();
		}
    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  LL_FLASH_SetLatency(LL_FLASH_LATENCY_2);
  while(LL_FLASH_GetLatency()!= LL_FLASH_LATENCY_2)
  {
  }
  LL_RCC_HSE_Enable();

   /* Wait till HSE is ready */
  while(LL_RCC_HSE_IsReady() != 1)
  {

  }
  LL_RCC_PLL_ConfigDomain_SYS(LL_RCC_PLLSOURCE_HSE_DIV_1, LL_RCC_PLL_MUL_9);
  LL_RCC_PLL_Enable();

   /* Wait till PLL is ready */
  while(LL_RCC_PLL_IsReady() != 1)
  {

  }
  LL_RCC_SetAHBPrescaler(LL_RCC_SYSCLK_DIV_1);
  LL_RCC_SetAPB1Prescaler(LL_RCC_APB1_DIV_2);
  LL_RCC_SetAPB2Prescaler(LL_RCC_APB2_DIV_1);
  LL_RCC_SetSysClkSource(LL_RCC_SYS_CLKSOURCE_PLL);

   /* Wait till System clock is ready */
  while(LL_RCC_GetSysClkSource() != LL_RCC_SYS_CLKSOURCE_STATUS_PLL)
  {

  }
  LL_SetSystemCoreClock(72000000);

   /* Update the time base */
  if (HAL_InitTick (TICK_INT_PRIORITY) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
//--------------------------------------------------------------------------------------------------------//
void encoder_init(void) 
{
    
  LL_TIM_SetCounter(TIM1, 32760); // начальное значение счетчика:
	
	LL_TIM_CC_EnableChannel(TIM1,LL_TIM_CHANNEL_CH1); //Enable the encoder interface channels 
	LL_TIM_CC_EnableChannel(TIM1,LL_TIM_CHANNEL_CH2);

  LL_TIM_EnableCounter(TIM1);     // включение таймера
}

//--------------------------------------------------------------------------------------------------------//
void loop(void) 
{
	currCounter = LL_TIM_GetCounter(TIM1); //текущее показание энкодера
	currCounter = (32767 - ((currCounter-1) & 0x0FFFF))/2; //деление на 2, чтобы считать щелчки (щелчок = 2 импульса)
	int32_t need_step = 0;
	int32_t delta = 0;
	
	if(currCounter != prevCounter) 
	{
		delta = (currCounter-prevCounter); //разница между текущим и предыдущим показанием энкодера
    prevCounter = currCounter; //сохранение текущего показанаи€ энкодера
    // защита от дребезга контактов и переполнени€ счетчика (переполнение будет случатьс€ очень редко)
   // if((delta > -20) && (delta < 20)) 
		if (delta != 0)
		{    
			/*snprintf(LCD_buff, sizeof(LCD_buff), "%06d %03d", currCounter, delta);
			ClearLcdMemory();
			LCD_ShowString(3, 10, LCD_buff);
			LCD_Refresh();*/
			if (delta < 0)
			{
				DRIVE_ENABLE(ON); //сигнал разрешени€ контроллеру
				DIR_DRIVE(BACKWARD); //направление вращени€
				HAL_Delay(5);
				need_step = 5*delta; //количество шагов
				
				while (need_step < 0)
				{
					STEP(ON); 
					HAL_Delay(3);
					STEP(OFF);
					HAL_Delay(3);
					need_step++;
				}
				STEP(ON);
				DIR_DRIVE(FORWARD);
				HAL_Delay(3);
				STEP(OFF);
				DRIVE_ENABLE(OFF);
			}
			else
			{
				if (delta > 0)
				{
					DRIVE_ENABLE(ON);
					DIR_DRIVE(FORWARD);
					HAL_Delay(5);
					need_step = 5*delta;
					while (need_step > 0)
					{
						STEP(ON);
						HAL_Delay(3);
						STEP(OFF);
						HAL_Delay(3);
						need_step--;
					}
					STEP(ON);
					DIR_DRIVE(BACKWARD);
					HAL_Delay(3);
					STEP(OFF);
					DRIVE_ENABLE(OFF);
				}
			}
		}
	}
}

//--------------------------------------------------------------------------------------------------------//
uint32_t scan_keys (void)
{
	static __IO uint8_t key_state = KEY_STATE_OFF; // начальное состо€ние кнопки - не нажата
	static __IO uint16_t key_repeat_time_cnt; // счетчик времени повтора
	static __IO uint8_t kscan_step = 0; // шаг (фаза) сканировани€
	
	if(key_state == KEY_STATE_OFF) //если кнопка была отпущена - ожидание нажати€
	{
		if(LL_GPIO_IsInputPinSet(COL_0_GPIO_Port,  COL_0_Pin) == 1)	//если кнопка была нажата - получение кода нажатой кнопки
		{
			key_state =  KEY_STATE_ON; //установка режима - кнопка нажата
			switch (kscan_step)
			{
				case 0:
					KEY_MACHINE.key_code = KEY_s0_d0;
					break;
				case 1:
					KEY_MACHINE.key_code = KEY_s1_d0;
					break;
				case 2:
					KEY_MACHINE.key_code = KEY_s2_d0;
					break;
				case 3:
					KEY_MACHINE.key_code = KEY_s3_d0;
					break;
				default:
					break;
			}
		}
		else
		{
			if (LL_GPIO_IsInputPinSet(COL_1_GPIO_Port, COL_1_Pin) == 1)
			{
				key_state =  KEY_STATE_ON;
				switch (kscan_step)
				{
					case 0:
						KEY_MACHINE.key_code =KEY_s0_d1;
						break;
					case 1:
						KEY_MACHINE.key_code =KEY_s1_d1;
						break;
					case 2:
						KEY_MACHINE.key_code =KEY_s2_d1;
						break;
					case 3:
						KEY_MACHINE.key_code =KEY_s3_d1;
						break;
					default:
						break;
				}
			}
			else
			{
				if (LL_GPIO_IsInputPinSet(COL_2_GPIO_Port, COL_2_Pin) == 1)
				{
					key_state =  KEY_STATE_ON;
					switch (kscan_step)
					{
						case 0:
							KEY_MACHINE.key_code = KEY_s0_d2;
							break;
						case 1:
							KEY_MACHINE.key_code = KEY_s1_d2;
							break;
						case 2:
							KEY_MACHINE.key_code = KEY_s2_d2;
							break;
						case 3:
							KEY_MACHINE.key_code = KEY_s3_d2;
							break;
						default:
							break;
					}
				}
			}
		}
	}
	
	if (key_state ==  KEY_STATE_ON)
	{
		key_state = KEY_STATE_BOUNCE; // состо€ние кнопки - дребезг
		key_repeat_time_cnt = KEY_BOUNCE_TIME; // счетчик времени дребезга - устанавливаем
	}

	if(key_state == KEY_STATE_BOUNCE) // пропускаем интервал дребезга
	{
		if(key_repeat_time_cnt > 0)
		{key_repeat_time_cnt--;} // счетчик времени дребезга - уменьшаем
		
		else // дребезг кончилс€
		{
			key_state = KEY_STATE_AUTOREPEAT; // кнопка нажата
			key_repeat_time_cnt = KEY_AUTOREPEAT_TIME; // счетчик времени автоповтора - устанавливаем
		}
	}
	if (key_state == KEY_STATE_AUTOREPEAT)
	{
		if(key_repeat_time_cnt > 0)
			{key_repeat_time_cnt--;} // уменьшаем счетчик автоповтора
		
		else
		{
			if(((LL_GPIO_IsInputPinSet(COL_0_GPIO_Port, COL_0_Pin)) || (LL_GPIO_IsInputPinSet(COL_1_GPIO_Port, COL_1_Pin)) || (LL_GPIO_IsInputPinSet(COL_2_GPIO_Port, COL_2_Pin))) == 0) // если кнопка отпущена
			{
				key_state = KEY_STATE_OFF; //возврат в начальное состо€ние ожидани€ нажати€ кнопки
				return KEY_MACHINE.key_code;
			}			
			else // кнопка продолжает удерживатьс€
				{key_repeat_time_cnt = KEY_AUTOREPEAT_TIME;} // установим счетчик автоповтора
		}
	}
	switch(kscan_step) // сканирование клавиатуры
	{
		case 0:
			kscan_step = 1; // следующий шаг (фаза) сканировани€
			SCAN_ROW2(ON); // лини€ активирована
			SCAN_ROW1(OFF); // лини€ не активирована
			break;
		
		case 1:
			kscan_step = 2; // шаг (фаза) сканировани€
			SCAN_ROW3(ON); // лини€ активирована
			SCAN_ROW2(OFF); // лини€ не активирована
			break;
		
		case 2:
			kscan_step = 3; // шаг (фаза) сканировани€
			SCAN_ROW4(ON); // лини€ активирована
			SCAN_ROW3(OFF); // лини€ не активирована
			break;
		
		case 3:
			kscan_step = 0; // шаг (фаза) сканировани€
			SCAN_ROW1(ON); // лини€ активирована
			SCAN_ROW4(OFF); // лини€ не активирована
			break;
		
		default:
			break;
	}
	return NO_KEY;
}
/* USER CODE END 4 */

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
