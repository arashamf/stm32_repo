/*
 * SPI.h
 *
 *  Created on: 18 ����. 2020 �.
 *      Author: ALEXEY
 */

#ifndef SPI_H_
#define SPI_H_
#define LIS3DSH_Cmd_Read 0x80 // ������� ������
#define LIS3DSH_Cmd_Write 0x00 // ������� ������
uint8_t SPI2_read_byte (uint8_t );
void SPI2_write_byte (uint8_t , uint8_t );
void SPI2_read_array(uint8_t, char *, uint8_t);
void SPI2_ini ();
void SPI1_ini ();

#endif /* SPI_H_ */
