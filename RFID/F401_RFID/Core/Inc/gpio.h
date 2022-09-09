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
  * This software component is licensed by ST under BSD 3-Clause license,
  * the "License"; You may not use this file except in compliance with the
  * License. You may obtain a copy of the License at:
  *                        opensource.org/licenses/BSD-3-Clause
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
//LCD_RESET
#define LCD_RST1  GPIOB->BSRR=GPIO_BSRR_BS_7 //GPIO_PIN_SET  
#define LCD_RST0  GPIOB->BSRR=GPIO_BSRR_BR_7 //GPIO_PIN_RESET 
	
//   LCD_DC
#define LCD_DC1  GPIOB->BSRR=GPIO_BSRR_BS_6 //GPIO_PIN_SET 
#define LCD_DC0  GPIOB->BSRR=GPIO_BSRR_BR_6 //GPIO_PIN_RESET 

//  LCD_CS
#define LCD_CS1   GPIOB->BSRR=GPIO_BSRR_BS_4  //GPIO_PIN_SET
#define LCD_CS0   GPIOB->BSRR=GPIO_BSRR_BR_4 //GPIO_PIN_RESET

#define LED(x) ((x)? (GPIOC->BSRR=GPIO_BSRR_BR_13) : (GPIOC->BSRR=GPIO_BSRR_BS_13)); 
/* USER CODE END Private defines */

void MX_GPIO_Init(void);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ GPIO_H__ */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
