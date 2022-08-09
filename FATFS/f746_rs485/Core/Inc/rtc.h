#ifndef __RTC_H__
#define __RTC_H__

#ifdef __cplusplus
extern "C" {
#endif
#include "main.h"	
#include "i2c.h"
#include "usart.h"
#include "fatfs.h"	
	
void edit_RTC_data (I2C_HandleTypeDef hi, uint8_t adress);
void GetTime (I2C_HandleTypeDef hi, uint8_t , uint8_t );
void SetTime (I2C_HandleTypeDef hi, uint8_t , char *);
uint8_t RTC_ConvertToDec(uint8_t );
uint8_t RTC_ConvertToBinDec(uint8_t );
void read_reg_RTC (I2C_HandleTypeDef hi, uint8_t );
	
#ifdef __cplusplus
}
#endif

#endif /* RTC_H */
