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
#define LED_GREEN(x) ((x)? (SET_BIT (GPIOD->BSRR, GPIO_BSRR_BS12)) : (SET_BIT (GPIOD->BSRR, GPIO_BSRR_BR12)))
#define TOOGLE_LED_GREEN ((READ_BIT (GPIOD->ODR, GPIO_ODR_OD12)) ? (LED_GREEN(0)) : (LED_GREEN(1)))

#define LED_ORANGE(x) ((x)? (SET_BIT (GPIOD->BSRR, GPIO_BSRR_BS13)) : (SET_BIT (GPIOD->BSRR, GPIO_BSRR_BR13)))
#define TOOGLE_LED_ORANGE ((READ_BIT (GPIOD->ODR, GPIO_ODR_OD13)) ? (LED_ORANGE(0)) : (LED_ORANGE(1)))

#define LED_RED(x) ((x)? (SET_BIT (GPIOD->BSRR, GPIO_BSRR_BS14)) : (SET_BIT (GPIOD->BSRR, GPIO_BSRR_BR14)))
#define TOOGLE_LED_RED ((READ_BIT (GPIOD->ODR, GPIO_ODR_OD14)) ? (LED_RED(0)) : (LED_RED(1)))

#define LED_BLUE(x) ((x)? (SET_BIT (GPIOD->BSRR, GPIO_BSRR_BS15)) : (SET_BIT (GPIOD->BSRR, GPIO_BSRR_BR15)))
#define TOOGLE_LED_BLUE ((READ_BIT (GPIOD->ODR, GPIO_ODR_OD15)) ? (LED_BLUE(0)) : (LED_BLUE(1)))

/* USER CODE END Private defines */

void MX_GPIO_Init(void);

/* USER CODE BEGIN Prototypes */

/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif
#endif /*__ GPIO_H__ */

