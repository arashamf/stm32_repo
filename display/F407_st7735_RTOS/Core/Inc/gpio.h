/**
  ******************************************************************************
  * @file    gpio.h
  * @brief   This file contains all the function prototypes for
  *          the gpio.c file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2022 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */
/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __GPIO_H__
#define __GPIO_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* USER CODE BEGIN Private defines */
	
#define LCD_RST1  HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_SET); //LCD_RESET
#define LCD_RST0  HAL_GPIO_WritePin(LCD_RESET_GPIO_Port, LCD_RESET_Pin, GPIO_PIN_RESET);
	
//   LCD_DC
//#define LCD_DC1  GPIOD->BSRR=GPIO_BSRR_BS_7 //GPIO_PIN_SET A0
//#define LCD_DC0  GPIOD->BSRR=GPIO_BSRR_BR_7 //GPIO_PIN_RESET A0
#define LCD_DC1  HAL_GPIO_WritePin(LCD_A0_GPIO_Port, LCD_A0_Pin, GPIO_PIN_SET); //GPIO_PIN_SET A0
#define LCD_DC0  HAL_GPIO_WritePin(LCD_A0_GPIO_Port, LCD_A0_Pin, GPIO_PIN_RESET); //GPIO_PIN_SET A0

//  LCD_CS
//#define LCD_CS1   GPIOB->BSRR=GPIO_BSRR_BS_5  //GPIO_PIN_SET
//#define LCD_CS0   GPIOB->BSRR=GPIO_BSRR_BR_5 //GPIO_PIN_RESET
#define LCD_CS1  HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_SET); 
#define LCD_CS0  HAL_GPIO_WritePin(LCD_CS_GPIO_Port, LCD_CS_Pin, GPIO_PIN_RESET); 
	
#define LED1(x) ((x)? (HAL_GPIO_WritePin(LED1_GPIO_Port,  LED1_Pin, GPIO_PIN_SET)) : (HAL_GPIO_WritePin(LED1_GPIO_Port,  LED1_Pin, GPIO_PIN_RESET))); 
#define LED2(x) ((x)? (HAL_GPIO_WritePin(LED2_GPIO_Port,  LED2_Pin, GPIO_PIN_SET)) : (HAL_GPIO_WritePin(LED2_GPIO_Port,  LED2_Pin, GPIO_PIN_RESET)));
#define LED3(x) ((x)? (HAL_GPIO_WritePin(LED3_GPIO_Port,  LED3_Pin, GPIO_PIN_SET)) : (HAL_GPIO_WritePin(LED3_GPIO_Port,  LED3_Pin, GPIO_PIN_RESET))); 
#define LED4(x) ((x)? (HAL_GPIO_WritePin(LED4_GPIO_Port,  LED4_Pin, GPIO_PIN_SET)) : (HAL_GPIO_WritePin(LED4_GPIO_Port,  LED4_Pin, GPIO_PIN_RESET))); 
#define TOOGGLE_LED1 HAL_GPIO_TogglePin (LED1_GPIO_Port, LED1_Pin);
#define TOOGGLE_LED2 HAL_GPIO_TogglePin (LED2_GPIO_Port, LED2_Pin);
#define TOOGGLE_LED3 HAL_GPIO_TogglePin (LED3_GPIO_Port, LED3_Pin);
#define TOOGGLE_LED4 HAL_GPIO_TogglePin (LED4_GPIO_Port, LED4_Pin);
/* USER CODE END Private defines */

void MX_GPIO_Init(void);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ GPIO_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
