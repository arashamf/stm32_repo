/*
 * lisdsh.h
 *
 *  Created on: 19 февр. 2020 г.
 *      Author: ALEXEY
 */

#ifndef LISDSH_H_
#define LISDSH_H_

// регистры LIS3DSH
#define LIS3DSH_CTRL_REG4 0x20
#define LIS3DSH_CTRL_REG1 0x21
#define LIS3DSH_CTRL_REG2 0x22
#define LIS3DSH_CTRL_REG3 0x23
#define LIS3DSH_CTRL_REG5 0x24
#define LIS3DSH_CTRL_REG6 0x25
#define LIS3DSH_REG_STATUS 0x27
#define LIS3DSH_REG_OUT_X 0x28
#define LIS3DSH_REG_WHOAMI 0x0F

// режимы по частоте (настраиваются в регистре 4)
#define LIS3DSH_F_3HZ_NORMAL 0x17 // 00010111
#define LIS3DSH_F_6HZ_NORMAL 0x27 // 00100111
#define LIS3DSH_F_800HZ_NORMAL 0x87 // 10000111
#define LIS3DSH_F_1600HZ_NORMAL 0x97 // 10010111

#endif /* LISDSH_H_ */
