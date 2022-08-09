/**
  ******************************************************************************
  * @file    gpio.h
  * @brief   This file contains all the function prototypes for
  *          the gpio.c file
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
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
#define LED_RED(x) ((x)? (HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, GPIO_PIN_SET)) : (HAL_GPIO_WritePin(LED_R_GPIO_Port, LED_R_Pin, GPIO_PIN_RESET))); 
#define LED_GREEN(x) ((x)? (HAL_GPIO_WritePin(LED_G_GPIO_Port, LED_G_Pin, GPIO_PIN_SET)) : (HAL_GPIO_WritePin(LED_G_GPIO_Port, LED_G_Pin, GPIO_PIN_RESET))); 

#define DE(x) ((x)? (HAL_GPIO_WritePin (DE_485_GPIO_Port, DE_485_Pin, GPIO_PIN_SET)) : (HAL_GPIO_WritePin (DE_485_GPIO_Port, DE_485_Pin, GPIO_PIN_RESET))); 
#define RE(x) ((x)? (HAL_GPIO_WritePin (RE_485_GPIO_Port, RE_485_Pin, GPIO_PIN_SET)) : (HAL_GPIO_WritePin (RE_485_GPIO_Port, RE_485_Pin, GPIO_PIN_RESET))); 
#define RS485_TX DE(1);RE(1);
#define RS485_RX DE(0);RE(0);
	
#define ENABLE_SD_CARD  (HAL_GPIO_WritePin (SDMMC_PSO_GPIO_Port, SDMMC_PSO_Pin, GPIO_PIN_RESET))
#define ENABLE_LED_SD  (HAL_GPIO_WritePin(SDMMC_OC_GPIO_Port, SDMMC_OC_Pin, GPIO_PIN_RESET))	
#define ENABLE_12V  (HAL_GPIO_WritePin (EN12V_GPIO_Port, EN12V_Pin, GPIO_PIN_SET))

//LCD
#define TFT_C_D(x)   ((x)? (GPIOD->BSRR =(1<<7))  : (GPIOD->BSRR =(1<<23)));     		//PD7_LCD_A0
#define TFT_RESET(x) ((x)? (GPIOB->BSRR =(1<<4))  : (GPIOB->BSRR =(1<<20)));     	 //PB4_LCD_RESET
#define TFT_CS(x)    ((x)? (GPIOB->BSRR =(1<<5))  : (GPIOB->BSRR =(1<<21)));      //PB5_LCD_CS
#define TFT_SCK(x)   ((x)? (GPIOB->BSRR =(1<<3))  : (GPIOB->BSRR =(1<<19)));     //PB3_LCD_SCK
#define TFT_SDA(x)   ((x)? (GPIOD->BSRR =(1<<6))  : (GPIOD->BSRR =(1<<22))); 		//PD6_LCD_SDA
#define SDA_PIN HAL_GPIO_ReadPin(GPIOD,GPIO_PIN_6)
/* USER CODE END Private defines */

void MX_GPIO_Init(void);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ GPIO_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
