
/* Includes ------------------------------------------------------------------*/
#include "ssd1306_interface.h"
#include "i2c.h"

/* Private define ------------------------------------------------------------*/
#define SSD1306_I2C_TIMEOUT                                	0xFF

/* Private typedef -----------------------------------------------------------*/
extern I2C_HandleTypeDef hi2c1;
extern SSD1306_State SSD1306_state;

/* Private variables ---------------------------------------------------------*/
static uint8_t temp[2];

/* Functions -----------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
void ssd1306_SendCommand(uint8_t type_command)
{
	temp[0] = SSD1306_BYTE_COMMAND;
	temp[1] = type_command;
	i2c_write_byte (SSD1306_I2C_ADDRESS, temp, 2);
}

/*----------------------------------------------------------------------------*/
void ssd1306_SendByteData(uint8_t data)
{
	temp[0] = SSD1306_BYTE_DATA; 
	temp[1] = data;
	i2c_write_byte (SSD1306_I2C_ADDRESS, temp, 2);
}

/*----------------------------------------------------------------------------*/
void ssd1306_SendDataBuffer(uint8_t *data, uint16_t data_size)
{
	i2c_write_buffer_8bit_registr(SSD1306_I2C_ADDRESS, SSD1306_BYTE_DATA, data, data_size);
}

/*----------------------------------------------------------------------------*/
void HAL_I2C_MemTxCpltCallback(I2C_HandleTypeDef *hi2c)
{
 // SSD1306_state = SSD1306_READY;
}

/*----------------------------------------------------------------------------*/
