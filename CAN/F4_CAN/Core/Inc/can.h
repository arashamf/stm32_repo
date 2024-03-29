/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file    can.h
  * @brief   This file contains all the function prototypes for
  *          the can.c file
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
#ifndef __CAN_H__
#define __CAN_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Includes ------------------------------------------------------------------*/
#include "main.h"

/* USER CODE BEGIN Includes */

/* USER CODE END Includes */

extern CAN_HandleTypeDef hcan1;

extern CAN_HandleTypeDef hcan2;

/* USER CODE BEGIN Private defines */
/*#define ID_C1 0b010000
#define ID_adress 0b01010*/

#define CAN_MSG_TYPE_A1_ID	0x01
#define CAN_MSG_TYPE_B_ID	0x08
#define CAN_MSG_TYPE_C_ID	0x10
#define CAN_MSG_TYPE_D_ID	0x20

#define MY_MODULE_TYPE 0x0C	// ��� ���� ������ - ���3
#define MY_MODULE_ADDR 0x1F	//��������� ����� ������

#define MAKE_FRAME_ID( msg_type_id, board_addr) ((((uint32_t)msg_type_id) << 5) | board_addr) 
/* USER CODE END Private defines */

void MX_CAN1_Init(void);
void MX_CAN2_Init(void);

/* USER CODE BEGIN Prototypes */
void CAN1_Send_Message (uint8_t * );
void CAN1_Send_C1 ();
void CAN2_Send_Message (uint8_t * );
/* USER CODE END Prototypes */

#ifdef __cplusplus
}
#endif

#endif /* __CAN_H__ */

