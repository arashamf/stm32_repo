/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
  *
  ******************************************************************************
  */
/* USER CODE END Header */
/* Includes ------------------------------------------------------------------*/
#include "main.h"
#include "fatfs.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "stdio.h"
#include "string.h"
#include "stm32f7xx_hal.h"
/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define LED(x) ((x)? (GPIOI->BSRR=GPIO_BSRR_BS_1) : (GPIOI->BSRR=GPIO_BSRR_BR_1));
/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

SD_HandleTypeDef hsd1;

UART_HandleTypeDef huart6;

char UART_msg_TX [50]; //буффер сообщений UARTS

FATFS log_fs ;    // рабочая область (file system object) для логических диска
FIL logfile;     //файловый объект 
//char logSDPath;  // User logical drive path 

unsigned int byteswritten, bytesread; //количество байт которые необходимо прочитать
uint8_t result; //код возврата функций FatFs

char filename [50];
char ver_in_sd[9];
char ver_in_flash[8];

char rtext[100]; // File read buffer 
	
//char read_file [] = "bootloader.txt"; //название файла для логгирования	
/* USER CODE BEGIN PV */

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
static void MX_GPIO_Init(void);
static void MX_SDMMC1_SD_Init(void);
static void MX_USART6_UART_Init(void);
/* USER CODE BEGIN PFP */
void UART6_SendByte(char );
void UART6_SendString (const char * );
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
/**********************************************************************************************************************/
uint8_t mount_card (FATFS* fs)
{
	uint8_t result = 0;
	if(BSP_SD_Init() != MSD_OK)
	{
		sprintf (UART_msg_TX,"bsp_sd_init error\r\n");
		UART6_SendString (UART_msg_TX);
		result = 1;
	}
	else
	{
		if((result = f_mount(&log_fs, (TCHAR const*)SDPath, 1)) != FR_OK) //монтирование рабочей области (0 - отложенное, 1 - немедленное монтирование)
		{
			sprintf (UART_msg_TX, "SD_card_error=%u\r\n", result);
			UART6_SendString (UART_msg_TX);
		}
	}
	return	result;
}

//****************************************************************************************************************************************************//
void EraseFlash(void)
{
	
	UART6_SendString("Start flash erasing\r\n"); 
	
	__disable_irq();
	HAL_FLASH_Unlock();		
	
	FLASH_Erase_Sector(FLASH_SECTOR_2, FLASH_VOLTAGE_RANGE_3);
	FLASH_WaitForLastOperation(500);
	FLASH_Erase_Sector(FLASH_SECTOR_3, FLASH_VOLTAGE_RANGE_3);
	FLASH_WaitForLastOperation(500);
	FLASH_Erase_Sector(FLASH_SECTOR_4, FLASH_VOLTAGE_RANGE_3);
	FLASH_WaitForLastOperation(500);
	//UART6_SendString("FLASH_SECTOR_4 erased\r\n");
	FLASH_Erase_Sector(FLASH_SECTOR_5, FLASH_VOLTAGE_RANGE_3);
	FLASH_WaitForLastOperation(500);
	//UART6_SendString("FLASH_SECTOR_5 erased\r\n");
	FLASH_Erase_Sector(FLASH_SECTOR_6, FLASH_VOLTAGE_RANGE_3);
	FLASH_WaitForLastOperation(500);
	//UART6_SendString("FLASH_SECTOR_6 erased\r\n");
	FLASH_Erase_Sector(FLASH_SECTOR_7, FLASH_VOLTAGE_RANGE_3);
	FLASH_WaitForLastOperation(500);
	//UART6_SendString("FLASH_SECTOR_7 erased\r\n");*/
	UART6_SendString("Finish flash erasing\r\n");
	
	HAL_FLASH_Lock();
	__enable_irq();	
}

//****************************************************************************************************************************************************//
unsigned int ReadFirmwareVersion(void)
{
	static uint32_t bytesread;	
	if(f_open(&logfile,filename, FA_READ) != FR_OK)
	{
		f_close(&logfile);
		return 0;	
	}
	if(f_lseek(&logfile,0x10000)!= FR_OK) //расширение файла до размера 0x10000 байт
	{
		f_close(&logfile);
		return 0;	
	}	
	if(f_read(&logfile,ver_in_sd,8,(void *)&bytesread)!= FR_OK)
	{
		f_close(&logfile);
		return 0;	
	}		
	f_close(&logfile);		
	return 1;		
}

//****************************************************************************************************************************************************//
void ReadCurrentVersion (void)
{
	unsigned int * src;
	unsigned int word=0,word2=0;
	
	src  = (unsigned int *)VERSION_ADDRESS;
	word=*src++;
	word2=*src;
	
	ver_in_flash[0]=word;
	ver_in_flash[1]=word>>8;
	ver_in_flash[2]=word>>16;
	ver_in_flash[3]=word>>24;
	
	ver_in_flash[4]=word2;
	ver_in_flash[5]=word2>>8;
	ver_in_flash[6]=word2>>16;
	ver_in_flash[7]=word2>>24;
}

//****************************************************************************************************************************************************//
unsigned int scan_files (char* path )
{
	FRESULT result;
  DIR dir;
  static FILINFO fno;
//	char *fn;
	                   
	if ((result = f_opendir(&dir, path)) == FR_OK)   // Open the directory 
	{
		for (;;) 
		{
			result = f_readdir(&dir, &fno);                   // Read a directory item 
      if (result != FR_OK || fno.fname[0] == 0)  // Останов цикла при ошибке или при достижении конца списка директории
				break; 
//			fn = fno.fname;
			if (!(fno.fattrib & AM_DIR)) //если найдена не директория
			{ 
				if((strstr(fno.fname,"BOOT"))&&(strstr(fno.fname,"BIN")))
				{
					sprintf(UART_msg_TX,"Found file %s \r\n", fno.fname);
					memcpy(filename, fno.fname, sizeof(fno.fname));
					UART6_SendString(UART_msg_TX);
					if(ReadFirmwareVersion())
					{
						sprintf(UART_msg_TX,"Version of file %s \r\n", ver_in_sd);
						UART6_SendString(UART_msg_TX);
					}										
					/*ReadCurrentVersion();
					sprintf(UART_msg_TX,"Current Version %s \r\n", ver_in_flash);
					UART6_SendString(UART_msg_TX);
					if(memcmp(ver_in_flash,ver_in_sd,8)==0)
					{
						return FOUND_SAME_FIRMWARE;
					}
					else
					{
						return FOUND_NEW_FIRMWARE;
					}*/
					return FOUND_NEW_FIRMWARE;
				}	
      } 			 
		}
		if ((result = f_closedir(&dir)) != FR_OK)
		{
				sprintf (UART_msg_TX,"incorrect_close_directory. code=%u\r\n", result);
				UART6_SendString (UART_msg_TX);
		}			
	}
	return FILE_NOT_FOUND;	
}

//****************************************************************************************************************************************************//
void UpgradeFirmware(void)
{
	char buffer[512];
//~	unsigned int i=0,x=0,size=0,per=0,per_done=0;
	unsigned int offset=0;
	static uint32_t bytesread;
	FRESULT result;
			
	if(f_open(&logfile, filename, FA_READ) == FR_OK)
  {
		sprintf (UART_msg_TX, "Found firmware file-%s\r\n", filename);
		UART6_SendString(UART_msg_TX);			 		 			 		 				 				 
		EraseFlash();			 				 
		UART6_SendString("Start flash programming\r\n"); 					
		HAL_FLASH_Unlock();	
				 
		for(unsigned int i=0; i<65536; i++)
		{
			result = f_read(&logfile, buffer, sizeof(buffer), (void *)&bytesread);						
			if(result != FR_OK) 
			{
				UART6_SendString("Error reading file\r\n");
			}							
			if(bytesread==0) //если файл окончился
				{break;}		
												
			for (unsigned int x=0; x<bytesread; x++)
			{
				HAL_FLASH_Program(TYPEPROGRAM_BYTE, APPLICATION_ADDRESS+offset, buffer[x]);
				offset=offset+1;
			}		
		}															
		HAL_FLASH_Lock();					
		UART6_SendString("Finish flash programming\r\n"); 				 	 
			
		f_close(&logfile);					
//		LED_OK(0);
//		NVIC_SystemReset();
	}	
}

//****************************************************************************************************************************************************//
void Go_To_User_App(void)
{
    uint32_t app_jump_address;
		
		UART6_SendString("jump_to_address\r\n"); 
	
    typedef void(*pFunction)(void);
    pFunction Jump_To_Application;

     __disable_irq();
		
    app_jump_address = *( uint32_t*) (APPLICATION_ADDRESS + 4);    
    Jump_To_Application = (pFunction)app_jump_address;            
	
		SCB->VTOR =  APPLICATION_ADDRESS; 
	
		//   Initialize user application's Stack Pointer
      __set_MSP(*(__IO uint32_t*) APPLICATION_ADDRESS);                                                
    Jump_To_Application();		                       
}

//****************************************************************************************************************************************************//
void ExitBootloader(void)
{
	Go_To_User_App();
}

//****************************************************************************************************************************************************//
/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */
	unsigned int timer=0;
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
  MX_SDMMC1_SD_Init();
  MX_USART6_UART_Init();
  MX_FATFS_Init();
  /* USER CODE BEGIN 2 */

  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
	/*	if((result = f_open(&logfile, "logfile.txt", FA_READ)) != FR_OK)
    {
			sprintf (UART_msg_TX,"incorrect_open_readfile. code=%u\r\n", result);
			UART6_SendString (UART_msg_TX);
		}
		else
		{
			f_gets (rtext, sizeof (rtext), &logfile); //копирование одной строки
			sprintf (UART_msg_TX,"%s\r\n", rtext);
			UART6_SendString (UART_msg_TX);
			if ((result = f_close(&logfile)) != FR_OK)
			{
				sprintf (UART_msg_TX,"incorrect_close_readfile. code=%u\r\n", result);
				UART6_SendString (UART_msg_TX);
			}				
		}*/
		if(mount_card (&log_fs) == MSD_OK)
		{
			result =	scan_files ("/");
//			sprintf (UART_msg_TX,"code=%u\r\n", result);
//			UART6_SendString (UART_msg_TX);
			switch (result)
			{
				case FILE_NOT_FOUND:
					//NVIC_SystemReset();
					ExitBootloader();
					break;
				case NO_SD_CARD:
					ExitBootloader();
					break;
				case FOUND_SAME_FIRMWARE:
					LED(1);
					UpgradeFirmware();
					LED(0);
					NVIC_SystemReset();
				//	ExitBootloader();
					break;
				case FOUND_NEW_FIRMWARE:
					LED(1);
					UpgradeFirmware();
					HAL_SD_Abort(&hsd1);
					LED(0);
					//NVIC_SystemReset();
					ExitBootloader();
					break;
			}			
	/*		timer++;
			if(timer>100000)
			{
				ExitBootloader();
			}*/
		}	
		HAL_Delay (1000);
		ExitBootloader();
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
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
  RCC_PeriphCLKInitTypeDef PeriphClkInitStruct = {0};

  /** Configure the main internal regulator output voltage
  */
  __HAL_RCC_PWR_CLK_ENABLE();
  __HAL_PWR_VOLTAGESCALING_CONFIG(PWR_REGULATOR_VOLTAGE_SCALE1);
  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLM = 25;
  RCC_OscInitStruct.PLL.PLLN = 384;
  RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
  RCC_OscInitStruct.PLL.PLLQ = 8;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
  /** Activate the Over-Drive mode
  */
  if (HAL_PWREx_EnableOverDrive() != HAL_OK)
  {
    Error_Handler();
  }
  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV4;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV2;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_6) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInitStruct.PeriphClockSelection = RCC_PERIPHCLK_USART6|RCC_PERIPHCLK_SDMMC1
                              |RCC_PERIPHCLK_CLK48;
  PeriphClkInitStruct.PLLSAI.PLLSAIN = 384;
  PeriphClkInitStruct.PLLSAI.PLLSAIR = 5;
  PeriphClkInitStruct.PLLSAI.PLLSAIQ = 2;
  PeriphClkInitStruct.PLLSAI.PLLSAIP = RCC_PLLSAIP_DIV8;
  PeriphClkInitStruct.PLLSAIDivQ = 1;
  PeriphClkInitStruct.PLLSAIDivR = RCC_PLLSAIDIVR_8;
  PeriphClkInitStruct.Usart6ClockSelection = RCC_USART6CLKSOURCE_PCLK2;
  PeriphClkInitStruct.Clk48ClockSelection = RCC_CLK48SOURCE_PLLSAIP;
  PeriphClkInitStruct.Sdmmc1ClockSelection = RCC_SDMMC1CLKSOURCE_CLK48;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInitStruct) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief SDMMC1 Initialization Function
  * @param None
  * @retval None
  */
static void MX_SDMMC1_SD_Init(void)
{

  /* USER CODE BEGIN SDMMC1_Init 0 */

  /* USER CODE END SDMMC1_Init 0 */

  /* USER CODE BEGIN SDMMC1_Init 1 */

  /* USER CODE END SDMMC1_Init 1 */
  hsd1.Instance = SDMMC1;
  hsd1.Init.ClockEdge = SDMMC_CLOCK_EDGE_RISING;
  hsd1.Init.ClockBypass = SDMMC_CLOCK_BYPASS_DISABLE;
  hsd1.Init.ClockPowerSave = SDMMC_CLOCK_POWER_SAVE_DISABLE;
  hsd1.Init.BusWide = SDMMC_BUS_WIDE_1B;
  hsd1.Init.HardwareFlowControl = SDMMC_HARDWARE_FLOW_CONTROL_DISABLE;
  hsd1.Init.ClockDiv = 8;
  /* USER CODE BEGIN SDMMC1_Init 2 */

  /* USER CODE END SDMMC1_Init 2 */

}

/**
  * @brief USART6 Initialization Function
  * @param None
  * @retval None
  */
static void MX_USART6_UART_Init(void)
{

  /* USER CODE BEGIN USART6_Init 0 */

  /* USER CODE END USART6_Init 0 */

  /* USER CODE BEGIN USART6_Init 1 */

  /* USER CODE END USART6_Init 1 */
  huart6.Instance = USART6;
  huart6.Init.BaudRate = 115200;
  huart6.Init.WordLength = UART_WORDLENGTH_8B;
  huart6.Init.StopBits = UART_STOPBITS_1;
  huart6.Init.Parity = UART_PARITY_NONE;
  huart6.Init.Mode = UART_MODE_TX_RX;
  huart6.Init.HwFlowCtl = UART_HWCONTROL_NONE;
  huart6.Init.OverSampling = UART_OVERSAMPLING_16;
  huart6.Init.OneBitSampling = UART_ONE_BIT_SAMPLE_DISABLE;
  huart6.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_NO_INIT;
  if (HAL_UART_Init(&huart6) != HAL_OK)
  {
    Error_Handler();
  }
  /* USER CODE BEGIN USART6_Init 2 */

  /* USER CODE END USART6_Init 2 */

}

/**
  * @brief GPIO Initialization Function
  * @param None
  * @retval None
  */
static void MX_GPIO_Init(void)
{
  GPIO_InitTypeDef GPIO_InitStruct = {0};

  /* GPIO Ports Clock Enable */
  __HAL_RCC_GPIOC_CLK_ENABLE();
  __HAL_RCC_GPIOD_CLK_ENABLE();
  __HAL_RCC_GPIOI_CLK_ENABLE();
  __HAL_RCC_GPIOH_CLK_ENABLE();

  /*Configure GPIO pin Output Level */
  HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);

  /*Configure GPIO pin : uSD_Detect_Pin */
  GPIO_InitStruct.Pin = uSD_Detect_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_INPUT;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  HAL_GPIO_Init(uSD_Detect_GPIO_Port, &GPIO_InitStruct);

  /*Configure GPIO pin : LED_Pin */
  GPIO_InitStruct.Pin = LED_Pin;
  GPIO_InitStruct.Mode = GPIO_MODE_OUTPUT_PP;
  GPIO_InitStruct.Pull = GPIO_NOPULL;
  GPIO_InitStruct.Speed = GPIO_SPEED_FREQ_LOW;
  HAL_GPIO_Init(LED_GPIO_Port, &GPIO_InitStruct);

}

/* USER CODE BEGIN 4 */
//****************************************************************************************************************************************************//
void UART6_SendByte(char b)
{
	int timeout = 300000;
	while ((USART6->ISR & UART_FLAG_TXE) == (uint16_t)RESET)
	{
		if(timeout --== 0){
			return;}
	}
	if ((USART6->ISR & USART_ISR_TC) == USART_ISR_TC) {
		USART6->TDR = (b & (uint16_t)0x01FF);} /// Transmit Data 
	while ((USART6->ISR & UART_FLAG_TC) == (uint16_t)RESET){}	//wait for trasmit
}

//****************************************************************************************************************************************************//
void UART6_SendString (const char * text)
{
	while(*text)
	{
		UART6_SendByte(*text);
		text++;
	}
}
//****************************************************************************************************************************************************//
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

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
