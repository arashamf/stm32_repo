/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.h
  * @brief          : Header for main.c file.
  *                   This file contains the common defines of the application.
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
#ifndef __MAIN_H
#define __MAIN_H

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "stm32f1xx_hal.h"

#include "stm32f1xx_ll_rcc.h"
#include "stm32f1xx_ll_bus.h"
#include "stm32f1xx_ll_system.h"
#include "stm32f1xx_ll_exti.h"
#include "stm32f1xx_ll_cortex.h"
#include "stm32f1xx_ll_utils.h"
#include "stm32f1xx_ll_pwr.h"
#include "stm32f1xx_ll_dma.h"
#include "stm32f1xx_ll_tim.h"
#include "stm32f1xx_ll_usart.h"
#include "stm32f1xx_ll_gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */

/* USER CODE END ET */

/* Exported constants --------------------------------------------------------*/
/* USER CODE BEGIN EC */

/* USER CODE END EC */

/* Exported macro ------------------------------------------------------------*/
/* USER CODE BEGIN EM */

/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

/* USER CODE BEGIN EFP */

/* USER CODE END EFP */

/* Private defines -----------------------------------------------------------*/
#define LED_Pin LL_GPIO_PIN_13
#define LED_GPIO_Port GPIOC
#define COL_0_Pin LL_GPIO_PIN_1
#define COL_0_GPIO_Port GPIOA
#define COL_1_Pin LL_GPIO_PIN_2
#define COL_1_GPIO_Port GPIOA
#define COL_2_Pin LL_GPIO_PIN_3
#define COL_2_GPIO_Port GPIOA
#define ROW_0_Pin LL_GPIO_PIN_4
#define ROW_0_GPIO_Port GPIOA
#define ROW_1_Pin LL_GPIO_PIN_5
#define ROW_1_GPIO_Port GPIOA
#define ROW_2_Pin LL_GPIO_PIN_6
#define ROW_2_GPIO_Port GPIOA
#define ROW_3_Pin LL_GPIO_PIN_7
#define ROW_3_GPIO_Port GPIOA
#define EN_DRIVE_Pin LL_GPIO_PIN_1
#define EN_DRIVE_GPIO_Port GPIOB
#define DIR_Pin LL_GPIO_PIN_10
#define DIR_GPIO_Port GPIOB
#define STEP_Pin LL_GPIO_PIN_11
#define STEP_GPIO_Port GPIOB
#define LCD_CS_Pin LL_GPIO_PIN_12
#define LCD_CS_GPIO_Port GPIOA
#define LCD_RESET_Pin LL_GPIO_PIN_15
#define LCD_RESET_GPIO_Port GPIOA
#define LCD_A0_Pin LL_GPIO_PIN_4
#define LCD_A0_GPIO_Port GPIOB
/* USER CODE BEGIN Private defines */
#define ON 1
#define OFF 0

#define FORWARD 1
#define BACKWARD 0
/* USER CODE END Private defines */

#ifdef __cplusplus
}
#endif

#endif /* __MAIN_H */
