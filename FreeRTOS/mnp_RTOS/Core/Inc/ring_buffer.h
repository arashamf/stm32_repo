/**
  *********************************************************************************************************
  *
  *	This file is part of Devprodest Lib.
  *	
  *	  Devprodest Lib is free software: you can redistribute it and/or modify
  *	  it under the terms of the GNU General Public License as published by
  *	  the Free Software Foundation, either version 3 of the License, or
  *	  (at your option) any later version.
  *	
  *	  Devprodest Lib is distributed in the hope that it will be useful,
  *	  but WITHOUT ANY WARRANTY; without even the implied warranty of
  *	  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  *	  GNU General Public License for more details.
  *	
  *	  You should have received a copy of the GNU General Public License
  *	  along with Devprodest Lib.  If not, see <http://www.gnu.org/licenses/>.
  *	
  * @file ring_buffer.h
  * @brief ��������� �����
  * @version 1.0
  * @authors Zaikin Denis (ZD)
  * @authors Potorochin Andrew (PA)
  * 
  * @copyright GNU General Public License
  *********************************************************************************************************
  */
#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "main.h"

// ������ ���������, ����������� ��������� �����
typedef struct
{
    uint8_t *buffer;           ///< ��������� �� �����-���������
    uint16_t idxIn;            ///< ����� ������ ��� ������
    uint16_t idxOut;           ///< ����� ������ ��� ������
    uint16_t size;             ///< ������ ������-���������
} RING_buffer_t;


/// ��������� ��������� ���������� ������� �������������
typedef enum 
{
    RING_ERROR = 0,             ///< �������� ������������� ������
    RING_SUCCESS = !RING_ERROR  ///< �������� ���� �� ������ ������
} RING_ErrorStatus_t;

/**
@function RING_CRC16ccitt ? ���������� ����������� �����.
@param RING_buffer_t *buf ? ��������� �� ��������� �����.
@param uint16_t lenght ? ������� ��������� ������ ��������� � ��������.
@param uint16_t position ? � ������ �������� ��������.
@return uint16_t ��������� ���������� ����������� �����.
 */
uint16_t RING_CRC16ccitt(const RING_buffer_t *buf, uint16_t lenght, uint16_t position);

/**
@function RING_CRC16ccitt_Intermediate ? ���������� ����������� ����� � ��������������.
@param RING_buffer_t *buf ? ��������� �� ��������� �����.
@param uint16_t lenght ? ������� ��������� ������ ��������� � ��������.
@param uint16_t tmpCrc ? ��������� �������� ����������� �����.
@param uint16_t position ? � ������ �������� ��������.
@return uint16_t ��������� ���������� ����������� �����.
 */
uint16_t RING_CRC16ccitt_Intermediate(const RING_buffer_t *buf, uint16_t lenght, uint16_t tmpCrc, uint16_t position);

/**
@function RING_Init ? ������������� ������.
@param RING_buffer_t *ring ? ��������� �� ��������� �����.
@param uint8_t *buf ? ��������� �� ����� ��������.
@param uint16_t size ? ������� ��������� � ������.
@return RING_ErrorStatus_t ��������� ������������� @ref RING_ErrorStatus_t
 */
RING_ErrorStatus_t RING_Init(RING_buffer_t *ring, uint8_t *buf, uint16_t size);

/**
@function RING_GetCount ? ���������� �������� ������ � ������.
@param RING_buffer_t *buf ? ��������� �� ��������� �����.
@return uint16_t ���������� �������� ������ � ������.
 */
uint16_t RING_GetCount(const RING_buffer_t *buf);

/**
@function RING_Clear ? ������� �����.
@param RING_buffer_t *buf ? ��������� �� ��������� �����.
 */
void RING_Clear(RING_buffer_t* buf);

/**
@function RING_Put ? ��������� ������� � �����.
@param uint8_t symbol ? ������� ��� �������� � �����.
@param RING_buffer_t *buf ? ��������� �� ��������� �����.
 */
void RING_Put( RING_buffer_t* buf, uint8_t symbol);

/**
@function RING_Put16 ? ��������� ������� � �����.
@param uint16_t symbol ? ������� ��� �������� � �����.
@param RING_buffer_t *buf ? ��������� �� ��������� �����.
 */
void RING_Put16( RING_buffer_t* buf, uint16_t symbol);

/**
@function RING_PutBuffr ? ��������� �������� �� �������-��������� � ��������� �����.
@param RING_buffer_t *ringbuf ? ��������� �� ��������� �����.
@param uint8_t *src ? ��������� �� ������-��������.
@param uint16_t len ? ���������� ����������� ���������.
 */
void RING_PutBuffr(RING_buffer_t *ringbuf, uint8_t *src, uint16_t len);

/**
@function RING_Pop ? �������� �� ������ ����.
@param RING_buffer_t *buf ? ��������� �� ��������� �����.
@return uint8_t �������� ���������� ��������.
 */
uint8_t RING_Pop(RING_buffer_t *buf);

/**
@function RING_Pop16 ? �������� �� ������ 16-������ �����.
@param RING_buffer_t *buf ? ��������� �� ��������� �����.
@return uint16_t �������� ���������� ��������.
 */
uint16_t RING_Pop16(RING_buffer_t *buf);

/**
@function RING_Pop32 ? �������� �� ������ 32-������ �����.
@param RING_buffer_t *buf ? ��������� �� ��������� �����.
@return uint32_t �������� ���������� ��������.
 */
uint32_t RING_Pop32(RING_buffer_t *buf);

/**
@function RING_PopBuffr ? ��������� ���������� ���������� ������ ������.
@param RING_buffer_t *ringbuf ? ��������� �� ��������� �����.
@param uint8_t *destination ? ��������� �� ������.
@param uint16_t len ? ���������� ���������� ���������.
 */
void RING_PopBuffr(RING_buffer_t *ringbuf, uint8_t *destination, uint16_t len);


/**
@function RING_PopString ? ��������� ���������������� ������ �� ������ � string
@param RING_buffer_t *ringbuf ? ��������� �� ��������� �����.
@param char *string ? ��������� �� ������.
 */
void RING_PopString(RING_buffer_t *ringbuf, char *string);
    
/**
@function RING_ShowSymbol ? ���������� ���������� �������� ��� ��� �������� �� ������.
@param RING_buffer_t *ringbuf ? ��������� �� ��������� �����.
@param uint16_t symbolNumbe ? ����� ��������.
@return int32_t �������� ���������� ��������. -1 ���� ������.
 */
int32_t RING_ShowSymbol(const RING_buffer_t *buf, uint16_t symbolNumber);


#ifdef __cplusplus
}
#endif

#endif /* RING_BUFFER_H */
