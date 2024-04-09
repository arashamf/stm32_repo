/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    gpio.h
  * @brief   This file contains all the function prototypes for
  *          the gpio.c file
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
#define LED_RED(x) ((x)? (LL_GPIO_SetOutputPin (LED_GPIO_Port, LED_Pin)) : (LL_GPIO_ResetOutputPin(LED_GPIO_Port, LED_Pin)))
#define TOOGLE_LED_RED() (LED_RED (!(LL_GPIO_IsOutputPinSet(LED_GPIO_Port, LED_Pin))))

#define STEP(x) ((x)? (LL_GPIO_SetOutputPin (STEP_GPIO_Port, STEP_Pin)) : (LL_GPIO_ResetOutputPin(STEP_GPIO_Port, STEP_Pin)))
#define DIR_DRIVE(x) ((x)? (LL_GPIO_ResetOutputPin (DIR_GPIO_Port, DIR_Pin)) : (LL_GPIO_SetOutputPin(DIR_GPIO_Port, DIR_Pin)))
#define DRIVE_ENABLE(x) ((x)? (LL_GPIO_SetOutputPin (EN_DRIVE_GPIO_Port, EN_DRIVE_Pin)) : (LL_GPIO_ResetOutputPin(EN_DRIVE_GPIO_Port, EN_DRIVE_Pin)))

#define SCAN_ROW1(x) ((x)? (LL_GPIO_SetOutputPin(ROW_0_GPIO_Port, ROW_0_Pin)) : (LL_GPIO_ResetOutputPin(ROW_0_GPIO_Port, ROW_0_Pin)))
#define SCAN_ROW2(x) ((x)? (LL_GPIO_SetOutputPin(ROW_1_GPIO_Port, ROW_1_Pin)) : (LL_GPIO_ResetOutputPin(ROW_1_GPIO_Port, ROW_1_Pin)))
#define SCAN_ROW3(x) ((x)? (LL_GPIO_SetOutputPin(ROW_2_GPIO_Port, ROW_2_Pin)) : (LL_GPIO_ResetOutputPin(ROW_2_GPIO_Port, ROW_2_Pin)))
#define SCAN_ROW4(x) ((x)? (LL_GPIO_SetOutputPin(ROW_3_GPIO_Port, ROW_3_Pin)) : (LL_GPIO_ResetOutputPin(ROW_3_GPIO_Port, ROW_3_Pin)))
/* USER CODE END Private defines */

void MX_GPIO_Init(void);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ GPIO_H__ */

