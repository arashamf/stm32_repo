
#ifndef I2C_INI_H_
#define I2C_INI_H_

#define APB1_CLOCK 24000000L
// I2C1 - объ€влени€
#define GPIO_AFRL_AF4_I2C1_SCL 0x04000000
#define GPIO_AFRL_AF4_I2C1_SDA 0x40000000

#define I2C1_FREQ 400000 // частота шины I2C в fast mode, √ц
#define LIS3DSH_I2C_ADDRESS 0x1E // адрес микросхемы LIS3DSH - (0011110)

#define I2C_WR 0 // операци€ записи
#define I2C_RD 1 // операци€ чтени€

void I2C1_init ();
void I2C1_write_byte(uint8_t, uint8_t);
uint8_t I2C1_read_byte(uint8_t );
void I2C1_read_array(uint8_t , char *, uint8_t );

#endif /* I2C_INI_H_ */
