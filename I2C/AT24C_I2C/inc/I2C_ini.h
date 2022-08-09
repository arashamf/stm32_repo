
#ifndef I2C_INI_H_
#define I2C_INI_H_

// I2C1 - ����������
#define GPIO_AFRL_AF4_I2C1_SCL 0x04000000
#define GPIO_AFRL_AF4_I2C1_SDA 0x40000000

#define I2C1_FREQ 400000 // ������� ���� I2C � fast mode, ��
#define I2C_WR 0x0 // �������� ������
#define I2C_RD 0x1 // �������� ������

RCC_ClocksTypeDef RCC_Freq;  //������� ���������

void I2C1_init ();
void I2C1_init_SPL ();
void I2C1_write_byte(uint8_t, uint16_t, uint8_t);
uint8_t I2C1_read_byte(uint8_t, uint16_t );
void I2C1_write_array (uint8_t, uint8_t, uint8_t *, uint8_t);
void I2C1_read_array (uint8_t ,uint8_t , uint8_t *, uint8_t );

#endif /* I2C_INI_H_ */
