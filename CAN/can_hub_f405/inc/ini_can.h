/*
 * ini_cah.h
 *
 *  Created on: 18 мар. 2021 г.
 *      Author: Пользователь
 */

#ifndef INI_CAH_H_
#define INI_CAH_H_

#include "stm32f4xx.h"

#define CAN_CMD_Test_Send			0x11		// Команда отправки тестового сообщения
#define CAN_CMD_Test_Ok				0x12		// Команда подтверждения тестового сообщения

void init_GPIO_CAN1 ();
void init_CAN1 ();
uint8_t CAN1_Send_Test();
void CAN1_Send_Ok ( );
uint8_t CAN1_TxMessage (CanTxMsg* );

void init_GPIO_CAN2 ();
void init_CAN2 ();
uint8_t CAN2_Send_Test();
void CAN2_Send_Ok ( );
uint8_t CAN2_TxMessage (CanTxMsg* );

#endif /* INI_CAH_H_ */
