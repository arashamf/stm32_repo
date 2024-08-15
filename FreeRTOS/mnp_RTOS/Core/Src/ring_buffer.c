/*
* This is an independent project of an individual developer. Dear PVS-Studio, please check it.
* PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
*/
/* LINTLIBRARY */
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
  * @file ring_buffer.c
  * @brief ��������� �����
  * @version 1.0
  * @authors Zaikin Denis (ZD)
  * @authors Potorochin Andrew (PA)
  * 
  * @copyright GNU General Public License
  *********************************************************************************************************
  */
#include "ring_buffer.h"
#include <string.h> 
#include "usart.h"
#include "typedef.h"
#include <stdio.h>
/** 
 * @defgroup CRC ������� ���������� ����������� ����������� �����
 * @{
 */

/**
@function RING_CRC16ccitt ? ���������� ����������� �����.
@param RING_buffer_t *buf ? ��������� �� ��������� �����.
@param uint16_t lenght ? ������� ��������� ������ ��������� � ��������.
@param uint16_t position ? � ������ �������� ��������.
@return uint16_t ��������� ���������� ����������� �����.
 */
uint16_t RING_CRC16ccitt(const RING_buffer_t *buf, uint16_t lenght, uint16_t position)
{
	return RING_CRC16ccitt_Intermediate( buf, lenght, 0xFFFF, position);
}

/**
@function RING_CRC16ccitt_Intermediate ? ���������� ����������� ����� � ��������������.
@param RING_buffer_t *buf ? ��������� �� ��������� �����.
@param uint16_t lenght ? ������� ��������� ������ ��������� � ��������.
@param uint16_t tmpCrc ? ��������� �������� ����������� �����.
@param uint16_t position ? � ������ �������� ��������.
@return uint16_t ��������� ���������� ����������� �����.
 */
uint16_t RING_CRC16ccitt_Intermediate(const RING_buffer_t *buf, uint16_t lenght, uint16_t tmpCrc, uint16_t position)
{
	uint16_t crc = tmpCrc;
	uint16_t crctab;
	uint8_t byte;
	
    while (lenght--)
    {
        crctab = 0x0000;
        byte = (RING_ShowSymbol(buf, lenght + position))^( crc >> 8 );
        if( byte & 0x01 ) crctab ^= 0x1021;
        if( byte & 0x02 ) crctab ^= 0x2042;
        if( byte & 0x04 ) crctab ^= 0x4084;
        if( byte & 0x08 ) crctab ^= 0x8108;
        if( byte & 0x10 ) crctab ^= 0x1231;
        if( byte & 0x20 ) crctab ^= 0x2462;
        if( byte & 0x40 ) crctab ^= 0x48C4;
        if( byte & 0x80 ) crctab ^= 0x9188;
        
        crc = ( ( (crc & 0xFF)^(crctab >> 8) ) << 8 ) | ( crctab & 0xFF );
    }
    return crc;
}

/** @} */

/** 
 * @defgroup control ������� ���������� �������
 * @{
 */


/**
@function RING_Init ? ������������� ������.
@param RING_buffer_t *ring ? ��������� �� ��������� �����.
@param uint8_t *buf ? ��������� �� ����� ��������.
@param uint16_t size ? ������� ��������� � ������.
@return RING_ErrorStatus_t ��������� ������������� @ref RING_ErrorStatus_t
 */
RING_ErrorStatus_t RING_Init(RING_buffer_t *ring, uint8_t *buf, uint16_t size)
{
    ring->size = size; 
    ring->buffer = buf;
    RING_Clear( ring ); //������� ������

    return ( ring->buffer ? RING_SUCCESS : RING_ERROR ) ;
}

/**
@function RING_GetCount ? ���������� �������� ������ � ������.
@param RING_buffer_t *buf ? ��������� �� ��������� �����.
@return uint16_t ���������� �������� ������ � ������.
 */
uint16_t RING_GetCount(const RING_buffer_t *buf)
{
    uint16_t retval = 0;
    if (buf->idxIn < buf->idxOut) retval = buf->size + buf->idxIn - buf->idxOut;
    else retval = buf->idxIn - buf->idxOut;
    return retval;
}

/**
@function RING_Clear ? ������� �����.
@param RING_buffer_t *buf ? ��������� �� ��������� �����.
 */
void RING_Clear(RING_buffer_t* buf)
{
    buf->idxIn = 0; //buf->idxIn - ���������� ���������� ������
    buf->idxOut = 0; //buf->idxOut - ���������� ���������� ����
	//	sprintf (buffer_TX_UART2, "buffer_clear\r\n");
	//	UART2_PutString (buffer_TX_UART2);
		memset(buf->buffer, 0, buf->size);
}


/** @} */

/** 
 * @defgroup put ������� �������� ������ � �����
 * @{
 */

/**
@function RING_Put ? ��������� ������� � �����.
@param uint8_t symbol ? ������� ��� �������� � �����.
@param RING_buffer_t *buf ? ��������� �� ��������� �����.
 */
void RING_Put( RING_buffer_t* buf, uint8_t symbol)
{
    buf->buffer[buf->idxIn++] = symbol;
    if (buf->idxIn >= buf->size) 
		{
			buf->idxIn = 0;
		}
}

/**
@function RING_Put16 ? ��������� ������� � �����.
@param uint16_t symbol ? ������� ��� �������� � �����.
@param RING_buffer_t *buf ? ��������� �� ��������� �����.
 */
void RING_Put16( RING_buffer_t* buf, uint16_t symbol)
{
    buf->buffer[buf->idxIn++] = symbol >> 8;
    if (buf->idxIn >= buf->size) buf->idxIn = 0;
    buf->buffer[buf->idxIn++] = symbol & 0xFF;
    if (buf->idxIn >= buf->size) buf->idxIn = 0;
}

/**
@function RING_PutBuffr ? ��������� �������� �� �������-��������� � ��������� �����.
@param RING_buffer_t *ringbuf ? ��������� �� ��������� �����.
@param uint8_t *src ? ��������� �� ������-��������.
@param uint16_t len ? ���������� ����������� ���������.
 */
void RING_PutBuffr(RING_buffer_t *ringbuf, uint8_t *src, uint16_t len)
{
    while(len--) RING_Put(ringbuf, *(src++));
}

/** @} */

/** 
 * @defgroup pop ������� �������� ������ �� ������
 * @{
 */

/**
@function RING_Pop ? �������� �� ������ ����.
@param RING_buffer_t *buf ? ��������� �� ��������� �����.
@return uint8_t �������� ���������� ��������.
 */
uint8_t RING_Pop(RING_buffer_t *buf)
{
    uint8_t retval = buf->buffer[buf->idxOut++];
    if (buf->idxOut >= buf->size) 
			buf->idxOut = 0;
    return retval;
}

/**
@function RING_Pop16 ? �������� �� ������ 16-������ �����.
@param RING_buffer_t *buf ? ��������� �� ��������� �����.
@return uint16_t �������� ���������� ��������.
 */
uint16_t RING_Pop16(RING_buffer_t *buf)
{
    uint16_t retval = RING_Pop(buf) << 8;
    retval += RING_Pop(buf);
    return retval;
}

/**
@function RING_Pop32 ? �������� �� ������ 32-������ �����.
@param RING_buffer_t *buf ? ��������� �� ��������� �����.
@return uint32_t �������� ���������� ��������.
 */
uint32_t RING_Pop32(RING_buffer_t *buf)
{
    uint32_t retval  = RING_Pop(buf) << 8;
    retval += RING_Pop(buf) << 8;
    retval += RING_Pop(buf) << 8;
    retval += RING_Pop(buf);
    return retval;
}

/**
@function RING_PopBuffr ? ��������� ���������� ���������� ������ ������.
@param RING_buffer_t *ringbuf ? ��������� �� ��������� �����.
@param uint8_t *destination ? ��������� �� ������.
@param uint16_t len ? ���������� ���������� ���������.
 */
void RING_PopBuffr(RING_buffer_t *ringbuf, uint8_t *destination, uint16_t len)
{
    while(len--) *(destination++) = RING_Pop(ringbuf);
}

/**
@function RING_PopString ? ��������� ���������������� ������ �� ������ � string
@param RING_buffer_t *ringbuf ? ��������� �� ��������� �����.
@param char *string ? ��������� �� ������.
 */
void RING_PopString(RING_buffer_t *ringbuf, char *string)
{
    while(RING_ShowSymbol(ringbuf,0) > 0) *(string++) = RING_Pop(ringbuf);
}


/** @} */

/** 
 * @defgroup other ������ �������
 * @ingroup pop
 * @{
 */

/**
@function RING_ShowSymbol ? ���������� ���������� �������� ��� ��� �������� �� ������.
@param RING_buffer_t *ringbuf ? ��������� �� ��������� �����.
@param uint16_t symbolNumbe ? ����� ��������.
@return int32_t �������� ���������� ��������. -1 ���� ������.
 */
int32_t RING_ShowSymbol(const RING_buffer_t *buf, uint16_t symbolNumber)
{
    uint32_t pointer = buf->idxOut + symbolNumber;
    int32_t  retval = -1;
    if (symbolNumber < RING_GetCount(buf))
    {
        if (pointer > buf->size) pointer -= buf->size;
        retval = buf->buffer[ pointer ] ;
    }
    return retval;
}

/** @} */
