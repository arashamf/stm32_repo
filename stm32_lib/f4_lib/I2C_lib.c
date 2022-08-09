#define stm32f4xx
#include "stm32f4xx.h"
#include "I2C_ini.h"

void I2C1_init(void)
{

RCC_GetClocksFreq (&RCC_Freq);

// ��������� ������ ��� I2C1
SET_BIT (GPIOB->MODER, GPIO_MODER_MODER6_1 | GPIO_MODER_MODER7_1); // PB6 � PB7 �� �������������� �������
SET_BIT (GPIOB->OTYPER, GPIO_OTYPER_OT_6 | GPIO_OTYPER_OT_7); // � �������� "������"
GPIOB->AFR[0] |= GPIO_AFRL_AF4_I2C1_SCL; //��� PB6 �������������� ������� - I2C1_SCL
GPIOB->AFR[0] |= GPIO_AFRL_AF4_I2C1_SDA; //��� PB7 �������������� ������� - I2C1_SDA
//SET_BIT (GPIOB->PUPDR, GPIO_PUPDR_PUPDR9_0); //PB9 �� ���� � ACC_INT1 c ��������� �� +

SET_BIT (RCC->APB1ENR, RCC_APB1ENR_I2C1EN); // ������� ������������ I2C1
//SET_BIT (I2C1->CR2, I2C_CR2_ITEVTEN); // ������� ���������� �� �������
SET_BIT (I2C1->CR1, I2C_CR1_ACK); // ���������� ��� ������������� ACK
SET_BIT (I2C1->CR2, (RCC_Freq.PCLK1_Frequency/1000000)); // ������� ������� ���� APB1=24 ���

CLEAR_BIT (I2C1->CCR, I2C_CCR_CCR);
SET_BIT (I2C1->CCR, I2C_CCR_FS); //������� ����� (Fast mode)
CLEAR_BIT (I2C1->CCR, I2C_CCR_DUTY); //���������� � ������� ������: tlow/thigh = 2
I2C1->CCR |= (RCC_Freq.PCLK1_Frequency/I2C1_FREQ); // ����������� ��� ������������ ����. PCLK_APB1/I2C_SPEED
I2C1->TRISE = (uint8_t)((3./(10000000./(RCC_Freq.PCLK1_Frequency))) + 1); //max ����� ����� �������� SCL. TRISE = RISE/tPCLK= RISE/1/PCLK_APB1, RISE � ��� max ����� ���������� �������, 1000 �� ��� Slow Mode � 300 �� ��� Fast mode

 SET_BIT (I2C1->CR1, I2C_CR1_PE); // �������� ��������� I2C1
}

void I2C1_init_SPL ()
{
    // �������� ������������ ������ �������
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

    GPIO_InitTypeDef gpio;
    I2C_InitTypeDef i2c;

    //��������� I2C
    i2c.I2C_ClockSpeed = 400000; //������� ���� I2C (fast speed: 400 kHz)
    i2c.I2C_Mode = I2C_Mode_I2C; //����� I2C
    i2c.I2C_DutyCycle = I2C_DutyCycle_2; //���������� � ������� ������: tlow/thigh = 2
    i2c.I2C_OwnAddress1 = 0x1; // ����� � ��� ���� ������ ��������� � ������
    i2c.I2C_Ack = I2C_Ack_Enable; // ���������� ��� ������������� ACK
    i2c.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;  //7 ������ �����
    I2C_Init(I2C1, &i2c); //�������� I2C1

    // �������� ����� PB6 � PB7 ��� I2C1
    gpio.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; //PB6 - SCL, PB7 - SDA
    gpio.GPIO_Mode = GPIO_Mode_AF; //�������������� �����
    gpio.GPIO_Speed = GPIO_Fast_Speed; //������� ����� (50 MHz)
    gpio.GPIO_OType = GPIO_OType_OD; //�������� ���������
    gpio.GPIO_PuPd = GPIO_PuPd_UP; //�������� �����
    GPIO_Init(GPIOB, &gpio); //���� B

 //   CLEAR_BIT (GPIOB->MODER, GPIO_MODER_MODER9); //PB9 �� ���� � slave
 //   SET_BIT (GPIOB->PUPDR, GPIO_PUPDR_PUPDR9_0); //PB9 c ��������� �� +3.3V

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_I2C1); //������� ����. ����� - I2C1
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_I2C1); //������� ����. ����� - I2C1

    I2C_Cmd(I2C1, ENABLE);
}

// I2C1 - ������ ����� ������
void I2C1_write_byte(uint8_t device_adr, uint16_t registr_adr, uint8_t data)
{
SET_BIT (I2C1->CR1, I2C_CR1_ACK); //���������� ��� ������������� ACK

SET_BIT (I2C1->CR1, I2C_CR1_START); // ������� "Start"
while(!READ_BIT(I2C1->SR1, I2C_SR1_SB)) {}; // ������� ���������� ��������
(void) I2C2->SR1;

WRITE_REG (I2C1->DR, (device_adr | I2C_WR)); // ���������� ����� ���������� � �������� ������
while(!READ_BIT(I2C1->SR1, I2C_SR1_ADDR)) {}; // ������� ���������� ��������
(void)I2C1->SR2;
(void)I2C1->SR1; // ������� ���� ADDR

WRITE_REG (I2C1->DR, ((registr_adr & 0xFF00) >> 8)); // ���������� ������ �������� ������ ��������
while(!READ_BIT (I2C1->SR1, I2C_SR1_BTF)) {}; // ������� ���������� ��������
WRITE_REG (I2C1->DR, (uint8_t)(registr_adr & 0x00FF)); // ���������� ������ �������� ������ ��������
while(!READ_BIT (I2C1->SR1, I2C_SR1_BTF)) {}; // ������� ���������� ��������

WRITE_REG (I2C1->DR, data); // ���������� ������
while(!READ_BIT (I2C1->SR1, I2C_SR1_BTF)) {}; // ������� ���������� ��������
SET_BIT (I2C1->CR1, I2C_CR1_STOP); // ������� "Stop"
}

// I2C1 - ������ ���������� ���� ������
//
void I2C1_write_array(uint8_t device_adr, uint8_t registr_adr, uint8_t *data, uint8_t data_len)  //data_len - ���������� ���� ����������� ��������
{
SET_BIT (I2C1->CR1, I2C_CR1_ACK); // ���������� ��� ������������� ACK

SET_BIT (I2C1->CR1, I2C_CR1_START); // ������� "Start"
while(!READ_BIT(I2C1->SR1, I2C_SR1_SB)) {}; // ������� ���������� �������� "Start"
(void) I2C2->SR1;

WRITE_REG (I2C1->DR, (device_adr | I2C_WR)); // ���������� ����� ���������� � �������� ������
while(!READ_BIT(I2C1->SR1, I2C_SR1_ADDR)) {}; // ������� ���������� ��������
(void)I2C1->SR2;
(void)I2C1->SR1; // ������� ���� ADDR

WRITE_REG (I2C1->DR, ((registr_adr & 0xFF00) >> 8)); // ���������� ������ �������� ������ ��������
while(!READ_BIT (I2C1->SR1, I2C_SR1_BTF)) {}; // ������� ���������� ��������
WRITE_REG (I2C1->DR, (uint8_t)(registr_adr & 0x00FF)); // ���������� ������ �������� ������ ��������
while(!READ_BIT (I2C1->SR1, I2C_SR1_BTF)) {}; // ������� ���������� ��������

for(uint8_t i = 0; i < data_len; i++)
	{
	while(!READ_BIT (I2C1->SR1, I2C_SR1_BTF)) {}; // ������� ���������� ��������
	I2C1->DR = *data++; // ���������� ������ �� �������
	}
SET_BIT (I2C1->CR1, I2C_CR1_STOP); // ������� "Stop"
}

// I2C1 - ������ ������� ����� ������
//
uint8_t I2C1_read_byte(uint8_t device_adr, uint16_t registr_adr)
{
uint8_t data_I2C;

SET_BIT (I2C1->CR1, I2C_CR1_ACK); //���������� ��� ������������� ACK

SET_BIT (I2C1->CR1, I2C_CR1_START); // ������� "Start"
while(!READ_BIT(I2C1->SR1, I2C_SR1_SB)) {}; // ������� ���������� ��������
(void) I2C2->SR1;

WRITE_REG (I2C1->DR, (device_adr | I2C_WR)); // ���������� ����� ���������� � �������� ������
while(!READ_BIT(I2C1->SR1, I2C_SR1_ADDR)) {}; // ������� ���������� ��������
(void)I2C1->SR2;
(void)I2C1->SR1; // ������� ���� ADDR

WRITE_REG (I2C1->DR, ((registr_adr & 0xFF00) >> 8)); // ���������� ����� ��������
while(!READ_BIT (I2C1->SR1, I2C_SR1_BTF)) {}; // ������� ���������� ��������
WRITE_REG (I2C1->DR, (uint8_t)(registr_adr & 0x00FF)); // ���������� ����� ��������
while(!READ_BIT (I2C1->SR1, I2C_SR1_BTF)) {}; // ������� ���������� ��������

SET_BIT (I2C1->CR1, I2C_CR1_START); // ������� ��������� "Start"
while(!READ_BIT(I2C1->SR1, I2C_SR1_SB)) {}; // ������� ���������� ��������
(void)I2C1->SR1;

WRITE_REG (I2C1->DR, (device_adr | I2C_RD)); // ���������� ����� ���������� � �������� ������
while(!READ_BIT(I2C1->SR1, I2C_SR1_ADDR)) {}; // ������� ���������� ��������
(void)I2C1->SR1;
(void)I2C1->SR2; // ������� ���� ADDR

while(!READ_BIT (I2C1->SR1, I2C_SR1_RXNE)) {} // �������� ������ ������
CLEAR_BIT (I2C1->CR1, I2C_CR1_ACK);  //�������� ������ NACK
SET_BIT (I2C1->CR1, I2C_CR1_STOP); // ������� "Stop"
return data_I2C = I2C1->DR;
}

// I2C1 - ������ ���������� ���� ������

void I2C1_read_array(uint8_t device_adr, uint8_t registr_adr, uint8_t *data, uint8_t data_len)  //data_len - ���������� ���� ����������� ��������
{
SET_BIT (I2C1->CR1, I2C_CR1_ACK); // ���������� ��� ������������� ACK

SET_BIT (I2C1->CR1, I2C_CR1_START); // ������� "Start"
while(!READ_BIT(I2C1->SR1, I2C_SR1_SB)) {}; // ������� ���������� �������� "Start"

WRITE_REG (I2C1->DR, (device_adr + I2C_WR)); // ���������� ����� ���������� � �������� ������
while(!READ_BIT(I2C1->SR1, I2C_SR1_ADDR)) {}; // ������� ���������� ��������
(void)I2C1->SR2;
(void)I2C1->SR1; // ������� ���� ADDR

WRITE_REG (I2C1->DR, ((registr_adr & 0xFF00) >> 8)); // ���������� ����� ��������
while(!READ_BIT (I2C1->SR1, I2C_SR1_BTF)) {}; // ������� ���������� ��������
WRITE_REG (I2C1->DR, (uint8_t)(registr_adr & 0x00FF)); // ���������� ����� ��������
while(!READ_BIT (I2C1->SR1, I2C_SR1_BTF)) {}; // ������� ���������� ��������

SET_BIT (I2C1->CR1, I2C_CR1_START); // ������� ��������� "Start"
while(!READ_BIT(I2C1->SR1, I2C_SR1_SB)) {}; // ������� ���������� �������� "Start"

WRITE_REG (I2C1->DR, (device_adr + I2C_RD)); // ���������� ����� ���������� � �������� ������
while(!READ_BIT(I2C1->SR1, I2C_SR1_ADDR)) {}; // ������� ���������� ��������
(void)I2C1->SR2;
(void)I2C1->SR1; // ������� ���� ADDR

for(uint8_t i = 0; i < data_len; i++)
	{
	if(i == (data_len-1))
		{
		CLEAR_BIT (I2C1->CR1, I2C_CR1_ACK); //�������� ������ NACK
		SET_BIT (I2C1->CR1, I2C_CR1_STOP); // ������� "Stop"
		}
	while(!READ_BIT (I2C1->SR1, I2C_SR1_RXNE)) {}; // �������� ������ ������
	*data++ = I2C1->DR; // ���������� ������ � ������
	}
}
