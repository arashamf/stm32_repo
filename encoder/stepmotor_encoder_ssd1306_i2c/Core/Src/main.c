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
#include "i2c.h"
#include "tim.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "typedef.h"
#include "ssd1306.h"
#include "button.h"
#include "drive.h"
#include "eeprom.h"
#include "angle_calc.h"
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

angular_data_t curr_rotation = {0}; //��������� � �������� ������� �������
encoder_data_t encoder_data = {0}; //��������� � ������� ��������
milling_data_t milling_mode = {0};
STATUS_FLAG_t status_flag;

__IO uint16_t key_code = NO_KEY;
/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
/* USER CODE BEGIN PFP */
void SetupRotationMode (encoder_data_t *, angular_data_t *, uint8_t * );
void SetupMillingMode (milling_data_t *, encoder_data_t *, angular_data_t *, uint8_t *);
void init_setup (milling_data_t * , angular_data_t * , STATUS_FLAG_t * , uint8_t * );
/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */
//---------------------------------------------------------------------------------=-------------------//

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
  MX_TIM3_Init();
  MX_I2C1_Init();
  /* USER CODE BEGIN 2 */
	encoder_init();
	timers_ini ();
	
	ssd1306_Init();
	delay_us (5000); //5 �� ��������
	
	init_setup (&milling_mode , &curr_rotation, &status_flag, eeprom_tx_buffer); //��������� �� EEPROM � ��������� ���� �������� 
	
	DRIVE_ENABLE(OFF); //���������� �������
	STEP(OFF); //
	delay_us (10000); //10 �� ��������
  /* USER CODE END 2 */

  /* Infinite loop */
  /* USER CODE BEGIN WHILE */
  while (1)
  {
		if ((key_code = scan_keys()) != NO_KEY) //���� ���� ������ ������
		{
			if (status_flag.tool_mode == MODE_DEFAULT) //���� ������� ����� 1
			{	
				switch (key_code) //��������� ���� ������� ������
				{					
					case KEY_LEFT:	//������� ����� ������
						default_screen_mode1 (&curr_rotation); //������� ���� ������ 1 
						break;
					
					case KEY_CENTER_SHORT: //�������� ������� �����. ������ - ������� � �������� �������� ���� � ������� �������
						return_mode_screen (&curr_rotation); //�������� ������ �������� ����
						while(1)
						{
							if ((key_code = scan_keys()) == NO_KEY) //���� �� ������ �����-������ ������
							{	continue;	} //�������� ������� ����� ������
							else
							{
								switch (key_code)
								{
									case KEY_LEFT:
										left_rotate_to_zero ( &curr_rotation); //������� ������ ������� ������� � ������� �������
										curr_rotation.mode1_error = 0; //��������� ����������� ������ ������ 1 ��� �������� � ������� �������
										break;
								
									case KEY_CENTER_SHORT:
										one_full_turn(); //������ �� 360 ��������
										break;
								
									case KEY_RIGHT: 
										right_rotate_to_zero (&curr_rotation); //������� �� ������� ������� � ������� �������
										curr_rotation.mode1_error = 0; //��������� ����������� ������ ������ 1 ��� �������� � ������� �������
										break;	

									default:
										key_code = NO_KEY; //����� �� ��������� 
										break;									
								}
								default_screen_mode1 (&curr_rotation);	//������� ���� ������ 1 
								break;
							}
						}
						break;
						
					case KEY_CENTER_LONG:	//������� ������� �����. ������ - ������� ��������� ���� ��������������� �������
						curr_rotation.mode1_error = 0;//��������� ����������� ������ ������ 1 ��� �������� � ������� �������
						AngleShaftReset (&curr_rotation); //����� �������� ��������� ����		
						angle_to_EEPROMbuf (&curr_rotation, eeprom_tx_buffer); //c��������� � ������ EEPROM ������� ������ ���� ����
						EEPROM_WriteBytes (EEPROM_MEMORY_PAGE, eeprom_tx_buffer, (EEPROM_NUMBER_BYTES-6)); //������ 8 ����
						default_screen_mode1 (&curr_rotation); //������� ���� ������ 1 
						break;
						
						
					case KEY_RIGHT: //������� ������ ������
						default_screen_mode1 (&curr_rotation); //������� ���� ������ 1 
						break;
					
					case KEY_ENC_SHORT: //�������� ������� ������ �������� - �������� ��������� ���� ���� ����
						curr_rotation.mode1_error = 0;//��������� ����������� ������ ������ 1 
						SetupRotationMode (&encoder_data, &curr_rotation, eeprom_tx_buffer); //�������� ����� ������ �������� ���� ���� ����
						angle_to_EEPROMbuf (&curr_rotation, eeprom_tx_buffer);  //���������� � EEPROM	
						EEPROM_WriteBytes (EEPROM_MEMORY_PAGE, eeprom_tx_buffer, (EEPROM_NUMBER_BYTES-10)); //4 ����		
						break;
					
					case KEY_ENC_LONG: //������� ������� ������ �������� - ����� �������������� ���� ���� ���� � ���� ������ �������� ���� 
						curr_rotation.mode1_error = 0;//��������� ����������� ������ ������ 1
						SetAngleReset (&curr_rotation); //����� �������������� ���� �������� �� ������������ ��������					
						SetupRotationMode (&encoder_data, &curr_rotation, eeprom_tx_buffer); //�������� ����� ������ �������� ���� ���� ����
						angle_to_EEPROMbuf (&curr_rotation, eeprom_tx_buffer);  //���������� � EEPROM	
						EEPROM_WriteBytes (EEPROM_MEMORY_PAGE, eeprom_tx_buffer, (EEPROM_NUMBER_BYTES-10)); //4 ����		
						break;
				
					case KEY_MODE_SHORT: 
						default_screen_mode1 (&curr_rotation);	//�������� ������� - ����� 1 �� ���������
						break;
					
					case KEY_MODE_LONG: 
						status_flag.tool_mode = MODE_MILLING; //����� - ����������
						angle_to_EEPROMbuf (&curr_rotation, eeprom_tx_buffer);  //���������� ������� ������ ��������� ����
						teeth_angle_to_EEPROMbuf (&milling_mode, eeprom_tx_buffer, &status_flag); //���������� ������ ������ ����������
						EEPROM_WriteBytes (EEPROM_MEMORY_PAGE, eeprom_tx_buffer, (EEPROM_NUMBER_BYTES-3)); //������ 11 ����
						default_screen_mode2 (&milling_mode, &status_flag);	//�������� ������� - ����� ���������� �� ���������
						break;
			
					default:
						key_code = NO_KEY;
						break;	
				}
			}
			else
			{
				if (status_flag.tool_mode == MODE_MILLING) //���� ������� ������ ����� == MODE_MILLING) //����� ����������
				{
					switch (key_code) 
					{	
						case KEY_LEFT: //������� ����� ������
							if (status_flag.left_flag == ON)
							{
								left_teeth_rotation (&milling_mode, &curr_rotation); //������� ����� ������ ������� �������
							//	angle_to_EEPROMbuf (&curr_rotation, eeprom_tx_buffer);  //���������� ������� ������ ��������� ����
								teeth_angle_to_EEPROMbuf (&milling_mode, eeprom_tx_buffer, &status_flag); //���������� ������ ������ ����������
								EEPROM_WriteBytes (EEPROM_MEMORY_PAGE, eeprom_tx_buffer, (EEPROM_NUMBER_BYTES-3)); //������ 11 ����
								default_screen_mode2 (&milling_mode, &status_flag); //�������� ������� - ����� 2 �� ���������
							}
							break;
					
						case KEY_CENTER_SHORT: //������� � �������� �������� ����
							return_mode_screen (&curr_rotation); //�������� ������� - ������ �������� ����
							while(1)
							{
								if ((key_code = scan_keys()) == NO_KEY) //���� �� ������ �����-������ ������
								{	continue;	} 	//�������� � ����������� �����
								else
								{
									switch (key_code) //���� ������ ���� ������
									{
										case KEY_LEFT:
											left_rotate_to_zero (&curr_rotation); //������� ������ ������� ������� � ������� �������
											RemainTeethReset (&milling_mode); //����� ���������� ���������� ������
											MilingFlagReset (&milling_mode, &status_flag); //����� ������ � ����������� ������
											break;							
								
										case KEY_RIGHT: 
											right_rotate_to_zero (&curr_rotation); //������� �� ������� ������� � ������� �������
											RemainTeethReset (&milling_mode); //����� ���������� ���������� ������
											MilingFlagReset (&milling_mode, &status_flag); //����� ������ � ����������� ������
											break;	

										default:
											key_code = NO_KEY;
											break;									
									}
									default_screen_mode2 (&milling_mode, &status_flag);	//������� ���� ������ ����������
									break;
								}
							}
							break;
						
						case KEY_CENTER_LONG:	
							default_screen_mode2 (&milling_mode, &status_flag); //������� ���� ������ ����������
							break;
											
						case KEY_RIGHT: //������ ������ ������
							if (status_flag.right_flag == ON) 
							{
								right_teeth_rotation	(&milling_mode, &curr_rotation); //������� ����� �� ������� �������
								//angle_to_EEPROMbuf (&curr_rotation, eeprom_tx_buffer);  //���������� ������� ������ ��������� ����
								teeth_angle_to_EEPROMbuf (&milling_mode, eeprom_tx_buffer, &status_flag); //���������� ������� �������� ������ ����������
								EEPROM_WriteBytes (EEPROM_MEMORY_PAGE, eeprom_tx_buffer, (EEPROM_NUMBER_BYTES-3)); //������ 11 ����
								default_screen_mode2 	(&milling_mode, &status_flag); //�������� ������� - ����� 2 �� ���������
							}
							break;
					
						case KEY_ENC_SHORT: //�������� ������� ��������	- ��������� ���������� ������ ������
							MilingFlagReset (&milling_mode, &status_flag); //����� ������ � ����������� ������
							SetupMillingMode (&milling_mode, &encoder_data, &curr_rotation, eeprom_tx_buffer); //�������� ��������� ���������� ������ ������
							teeth_angle_to_EEPROMbuf (&milling_mode, eeprom_tx_buffer, &status_flag); //���������� ������� �������� ������ ����������
							EEPROM_WriteBytes (EEPROM_MEMORY_PAGE, eeprom_tx_buffer, (EEPROM_NUMBER_BYTES-3)); //������ 11 ����
							default_screen_mode2 (&milling_mode, &status_flag); //������� ���� ������ ����������
							break;
					
						case KEY_ENC_LONG: 		//������� ������� �������� - ����� ���� �������� ������ ���������� � ��������� ���������� ������ ������			
							MilAngleTeethReset (&milling_mode, &status_flag); //����� ���� ��������� ������ ����������
							SetupMillingMode (&milling_mode, &encoder_data, &curr_rotation, eeprom_tx_buffer); //�������� ��������� ���������� ������ ������
							teeth_angle_to_EEPROMbuf (&milling_mode, eeprom_tx_buffer, &status_flag); //���������� ������� �������� ������ ����������
							EEPROM_WriteBytes (EEPROM_MEMORY_PAGE, eeprom_tx_buffer, (EEPROM_NUMBER_BYTES-3)); //������ 11 ����
							default_screen_mode2 (&milling_mode, &status_flag); //������� ���� ������ ����������
							break;
				
						case KEY_MODE_SHORT: 
							default_screen_mode2 (&milling_mode, &status_flag); //�������� ������� - ����� 1
							break;
						
						case KEY_MODE_LONG: 
							status_flag.tool_mode = MODE_DEFAULT; //���� ������� ������ ����� = MODE_DEFAULT; //��������� ������ �� ���������
							angle_to_EEPROMbuf (&curr_rotation, eeprom_tx_buffer);  //���������� ������� ������ ��������� ����
							teeth_angle_to_EEPROMbuf (&milling_mode, eeprom_tx_buffer, &status_flag); //���������� ������ ������ ����������
							EEPROM_WriteBytes (EEPROM_MEMORY_PAGE, eeprom_tx_buffer, (EEPROM_NUMBER_BYTES-3)); //������ 11 ����
							GetAngleShaft_from_Seconds (&curr_rotation); //��������� �������� ��������� ���� � ������� ��/���/�
							encoder_reset (&encoder_data); 	//����� ��������� �������� (���� ������� ������� �� ������ ������)
							default_screen_mode1 (&curr_rotation); //�������� ������� - ����� 1
							break;
				
						default:
							key_code = NO_KEY;
							break;	
					}
				}		
			}	
		}
		
		if (status_flag.tool_mode == MODE_DEFAULT) //���� ������� ������ �����
		{	enc_shaft_rotation (&curr_rotation, &encoder_data);	} //������������ ��������� �������� � �������	���� � ������ ��������� ��� ���������
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
  RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSI;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.HSICalibrationValue = RCC_HSICALIBRATION_DEFAULT;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSI;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL12;
  RCC_OscInitStruct.PLL.PREDIV = RCC_PREDIV_DIV1;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_1) != HAL_OK)
  {
    Error_Handler();
  }
  PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_I2C1;
  PeriphClkInit.I2c1ClockSelection = RCC_I2C1CLKSOURCE_HSI;
  if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK)
  {
    Error_Handler();
  }
}

/* USER CODE BEGIN 4 */
//---------------------------������� ����� ������� ����������� ������ ������ 1---------------------------//
void SetupRotationMode (encoder_data_t * handle_enc, angular_data_t * handle_ang, uint8_t * TXbuffer)
{
	setangle_mode_screen (handle_ang); //���� �������� ������ ��������� ����
	handle_enc->prevCounter_SetAngle = handle_enc->prevCounter_ShaftRotation; //����� ������� ��������� �������� 
	while(1) //��������� ������� ������ ���� ����
	{
		if ((key_code = scan_keys()) != KEY_ENC_SHORT) //���� �� ����� ���������� ������� ������ ��������
		{	set_angle(handle_ang, handle_enc);	}		//�������� ��������� ��������								
		else
		{	
			angle_to_EEPROMbuf (handle_ang, TXbuffer); //������� ������ ���� � ������
			EEPROM_WriteBytes (EEPROM_MEMORY_PAGE, TXbuffer, (EEPROM_NUMBER_BYTES-10)); //������ 4 ���� (�������� ���� ���� ����)			
			default_screen_mode1 (handle_ang); //�������� ������� - ����� 1 �� ���������
			handle_enc->prevCounter_ShaftRotation = handle_enc->prevCounter_SetAngle; //����� ��������� ��������� �������� 
			break;	
		}
	}
}

//---------------------------������� ����� ����������� ������ ���������� � ������ 2---------------------------//
void SetupMillingMode (milling_data_t * handle_mil, encoder_data_t * handle_enc, angular_data_t * handle_ang, uint8_t * TXbuffer)
{
	encoder_reset (handle_enc); 	//����� ��������� ��������
	setteeth_mode_screen (handle_mil); //�������� ������� - ����� ����� ����������� ������ ����������
	while(1) //����� ��������� ���������� ����� ���������
	{
		if ((key_code = scan_keys()) != KEY_ENC_SHORT) //���� ��� ��������� ������� ������ ��������
		{	set_teeth_gear(handle_mil, handle_enc);	} //����� �������� - ��������� ���������� ������ ���������
		else
		{	
			GetMilAngleTeeth (handle_mil); //������ ���� �������� ����� ����� ���������� �����
			handle_mil->remain_teeth_gear = handle_mil->teeth_gear_numbers; //����� ���������� ���������� �����	
			angle_to_EEPROMbuf (handle_ang, TXbuffer); //���������� �������� ��������� ���� � EEPROM �������
			teeth_angle_to_EEPROMbuf (handle_mil, TXbuffer, &status_flag); //���������� ������ ���������� � EEPROM �������
			EEPROM_WriteBytes (EEPROM_MEMORY_PAGE, TXbuffer, EEPROM_NUMBER_BYTES-3 ); //�������� � EEPROM 11 ����
			handle_enc->prevCounter_ShaftRotation = handle_enc->prevCounter_SetAngle; //����� ��������� ��������
			break;
		}
	}
	
	select_rotation_mode_screen(); //�������� ��������� ��������� ����������� ����������
	while (1) //��������� ����������� ����������
	{
		key_code = scan_keys();
		if ((key_code == KEY_RIGHT) || (key_code == KEY_LEFT))
		{
			if (key_code == KEY_RIGHT) 
			{	status_flag.right_flag = ON;	}
			else
			{
				if (key_code == KEY_LEFT) 
				{	status_flag.left_flag = ON;	}
			}
			break;
		}
	}
}

//-------------------------------------------------------------------------------------------------------//
void init_setup (milling_data_t * handle_mil, angular_data_t * handle_ang, STATUS_FLAG_t * status_flag, uint8_t * TXbuffer)
{
	EEPROM_ReadBytes (EEPROM_MEMORY_PAGE, TXbuffer, EEPROM_NUMBER_BYTES); //��������� ������� ������ ��������� �������� �� EEPROM
	angle_from_EEPROMbuf (handle_ang, TXbuffer); //������������� � ����������� ������� ������ ��������� �������� � ���������
	teeth_angle_from_EEPROMbuf (handle_mil, TXbuffer, status_flag); //������������� � ����������� ������� ������ ������ ����������
	
	//�������� ������������� ���������� ������� ������ ������ 1
	if ((handle_ang->ShaftAngleInSec > CIRCLE_IN_SEC) || (handle_ang->StepAngleInSec >= CIRCLE_IN_SEC)) 
	{
		SetAngleReset (handle_ang); //���� ������ ������������� - ����� ��������� ���� ���� ����
		AngleShaftReset (handle_ang); //���� ������ ������������� - ������� ��������� ���� �������
	}
	GetSetAngle_from_Seconds (handle_ang); 		//������� ������� ������ ��������� ���� � ������ ��/���/�
	GetAngleShaft_from_Seconds (handle_ang);	//������� ������� ������ ��������� ���� � ������ ��/���/�
	
	GetMilAngleTeeth (handle_mil); //������� ���� ���� ���� ��� ������ ����������	
	if ((handle_mil->AngleTeethInSec > CIRCLE_IN_SEC))//�������� ������������� ���������� ������� ������ ������ ����������
	{	MilAngleTeethReset (handle_mil, status_flag);	}//���� ������ ������������� - ����� �������� ������ ����������
	MilAngleTeeth_from_Seconds (handle_mil); //������� ������� ������ ���� ���� ���������� � ������ ��/���/�
	
	if (status_flag->tool_mode == MODE_DEFAULT) 
	{ default_screen_mode1 (handle_ang); } //�������� �� ��������� ������ 1
	else
	{ default_screen_mode2 (handle_mil, status_flag); } //�������� �� ��������� ������ ����������
}

//-------------------------------------------------------------------------------------------------------//
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
