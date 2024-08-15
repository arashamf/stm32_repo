/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.h
  * @brief   This file contains all the function prototypes for
  *          the gpio.c file
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2024 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */
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
//Private typedef-----------------------------------------------------------------------//
#define LED_RED(x) ((x)? (LL_GPIO_SetOutputPin (LED_GPIO_Port, LED_Pin)) : (LL_GPIO_ResetOutputPin(LED_GPIO_Port, LED_Pin)))
#define TOOGLE_LED_RED() (LED_RED (!(LL_GPIO_IsOutputPinSet(LED_GPIO_Port, LED_Pin))))

#define LCD_RST(x) ((x)? (LL_GPIO_SetOutputPin(RESET_GPIO_Port, RESET_Pin)) : (LL_GPIO_ResetOutputPin(RESET_GPIO_Port, RESET_Pin)));  
#define LCD_DC(x) ((x)? (LL_GPIO_SetOutputPin(DC_GPIO_Port, DC_Pin)) : (LL_GPIO_ResetOutputPin(DC_GPIO_Port, DC_Pin)));  
#define LCD_CS(x) ((x)? (LL_GPIO_SetOutputPin(CS_GPIO_Port, CS_Pin)) : (LL_GPIO_ResetOutputPin(CS_GPIO_Port, CS_Pin)));  

#define IN1(x) ((x)? (LL_GPIO_SetOutputPin(IN1_GPIO_Port, IN1_Pin)) : (LL_GPIO_ResetOutputPin(IN1_GPIO_Port, IN1_Pin)));  
#define IN2(x) ((x)? (LL_GPIO_SetOutputPin(IN2_GPIO_Port, IN2_Pin)) : (LL_GPIO_ResetOutputPin(IN2_GPIO_Port, IN2_Pin)));  
#define IN3(x) ((x)? (LL_GPIO_SetOutputPin(IN3_GPIO_Port, IN3_Pin)) : (LL_GPIO_ResetOutputPin(IN3_GPIO_Port, IN3_Pin)));  
#define IN4(x) ((x)? (LL_GPIO_SetOutputPin(IN4_GPIO_Port, IN4_Pin)) : (LL_GPIO_ResetOutputPin(IN4_GPIO_Port, IN4_Pin)));  

//Private typedef-----------------------------------------------------------------------//
typedef struct 
{
	GPIO_TypeDef * PORTx;
	uint32_t PORT_Pin;		

}TPortPin;


extern const uint8_t number_check_pins;

extern const TPortPin outPin[];
/* USER CODE END Private defines */

void MX_GPIO_Init(void);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ GPIO_H__ */

