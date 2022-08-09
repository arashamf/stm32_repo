#define stm32f4xx
#include "stm32f4xx.h"
#include "I2C_ini.h"

void I2C1_init(void)
{

RCC_GetClocksFreq (&RCC_Freq);

// настройка портов для I2C1
SET_BIT (GPIOB->MODER, GPIO_MODER_MODER6_1 | GPIO_MODER_MODER7_1); // PB6 и PB7 на альтернативную функцию
SET_BIT (GPIOB->OTYPER, GPIO_OTYPER_OT_6 | GPIO_OTYPER_OT_7); // с открытым "стоком"
GPIOB->AFR[0] |= GPIO_AFRL_AF4_I2C1_SCL; //для PB6 альтернативная функция - I2C1_SCL
GPIOB->AFR[0] |= GPIO_AFRL_AF4_I2C1_SDA; //для PB7 альтернативная функция - I2C1_SDA
//SET_BIT (GPIOB->PUPDR, GPIO_PUPDR_PUPDR9_0); //PB9 на вход к ACC_INT1 c подтяжкой на +

SET_BIT (RCC->APB1ENR, RCC_APB1ENR_I2C1EN); // включим тактирование I2C1
//SET_BIT (I2C1->CR2, I2C_CR2_ITEVTEN); // включим прерывания от событий
SET_BIT (I2C1->CR1, I2C_CR1_ACK); // передавать бит подтверждения ACK
SET_BIT (I2C1->CR2, (RCC_Freq.PCLK1_Frequency/1000000)); // зададим частоту шины APB1=24 МГц

CLEAR_BIT (I2C1->CCR, I2C_CCR_CCR);
SET_BIT (I2C1->CCR, I2C_CCR_FS); //быстрый режим (Fast mode)
CLEAR_BIT (I2C1->CCR, I2C_CCR_DUTY); //скважность в быстрым режиме: tlow/thigh = 2
I2C1->CCR |= (RCC_Freq.PCLK1_Frequency/I2C1_FREQ); // коэффициент для тактирования шины. PCLK_APB1/I2C_SPEED
I2C1->TRISE = (uint8_t)((3./(10000000./(RCC_Freq.PCLK1_Frequency))) + 1); //max время спада импульса SCL. TRISE = RISE/tPCLK= RISE/1/PCLK_APB1, RISE — это max время нарастания сигнала, 1000 нс для Slow Mode и 300 нс для Fast mode

 SET_BIT (I2C1->CR1, I2C_CR1_PE); // включаем интерфейс I2C1
}

void I2C1_init_SPL ()
{
    // Включаем тактирование нужных модулей
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_I2C1, ENABLE);
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOB, ENABLE);

    GPIO_InitTypeDef gpio;
    I2C_InitTypeDef i2c;

    //настройка I2C
    i2c.I2C_ClockSpeed = 400000; //частота шины I2C (fast speed: 400 kHz)
    i2c.I2C_Mode = I2C_Mode_I2C; //режим I2C
    i2c.I2C_DutyCycle = I2C_DutyCycle_2; //скважность в быстрым режиме: tlow/thigh = 2
    i2c.I2C_OwnAddress1 = 0x1; // адрес я тут взял первый пришедший в голову
    i2c.I2C_Ack = I2C_Ack_Enable; // передавать бит подтверждения ACK
    i2c.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;  //7 битный режим
    I2C_Init(I2C1, &i2c); //настроим I2C1

    // настойка пинов PB6 и PB7 для I2C1
    gpio.GPIO_Pin = GPIO_Pin_6 | GPIO_Pin_7; //PB6 - SCL, PB7 - SDA
    gpio.GPIO_Mode = GPIO_Mode_AF; //альтернативный режим
    gpio.GPIO_Speed = GPIO_Fast_Speed; //быстрый режим (50 MHz)
    gpio.GPIO_OType = GPIO_OType_OD; //открытый коллектор
    gpio.GPIO_PuPd = GPIO_PuPd_UP; //подтяжка вверх
    GPIO_Init(GPIOB, &gpio); //порт B

 //   CLEAR_BIT (GPIOB->MODER, GPIO_MODER_MODER9); //PB9 на вход к slave
 //   SET_BIT (GPIOB->PUPDR, GPIO_PUPDR_PUPDR9_0); //PB9 c подтяжкой на +3.3V

    GPIO_PinAFConfig(GPIOB, GPIO_PinSource6, GPIO_AF_I2C1); //зададим альт. режим - I2C1
    GPIO_PinAFConfig(GPIOB, GPIO_PinSource7, GPIO_AF_I2C1); //зададим альт. режим - I2C1

    I2C_Cmd(I2C1, ENABLE);
}

// I2C1 - запись байта данных
void I2C1_write_byte(uint8_t device_adr, uint16_t registr_adr, uint8_t data)
{
SET_BIT (I2C1->CR1, I2C_CR1_ACK); //передавать бит подтверждения ACK

SET_BIT (I2C1->CR1, I2C_CR1_START); // Условие "Start"
while(!READ_BIT(I2C1->SR1, I2C_SR1_SB)) {}; // ожидаем выполнения операции
(void) I2C2->SR1;

WRITE_REG (I2C1->DR, (device_adr | I2C_WR)); // записываем адрес устройства и операцию записи
while(!READ_BIT(I2C1->SR1, I2C_SR1_ADDR)) {}; // ожидаем выполнения операции
(void)I2C1->SR2;
(void)I2C1->SR1; // сбросим флаг ADDR

WRITE_REG (I2C1->DR, ((registr_adr & 0xFF00) >> 8)); // записываем первую половину адреса регистра
while(!READ_BIT (I2C1->SR1, I2C_SR1_BTF)) {}; // ожидаем выполнения операции
WRITE_REG (I2C1->DR, (uint8_t)(registr_adr & 0x00FF)); // записываем вторую половину адреса регистра
while(!READ_BIT (I2C1->SR1, I2C_SR1_BTF)) {}; // ожидаем выполнения операции

WRITE_REG (I2C1->DR, data); // записываем данные
while(!READ_BIT (I2C1->SR1, I2C_SR1_BTF)) {}; // ожидаем выполнения операции
SET_BIT (I2C1->CR1, I2C_CR1_STOP); // Условие "Stop"
}

// I2C1 - запись нескольких байт данных
//
void I2C1_write_array(uint8_t device_adr, uint8_t registr_adr, uint8_t *data, uint8_t data_len)  //data_len - количество байт необходимых записать
{
SET_BIT (I2C1->CR1, I2C_CR1_ACK); // передавать бит подтверждения ACK

SET_BIT (I2C1->CR1, I2C_CR1_START); // Условие "Start"
while(!READ_BIT(I2C1->SR1, I2C_SR1_SB)) {}; // ожидаем выполнения операции "Start"
(void) I2C2->SR1;

WRITE_REG (I2C1->DR, (device_adr | I2C_WR)); // записываем адрес устройства и операцию записи
while(!READ_BIT(I2C1->SR1, I2C_SR1_ADDR)) {}; // ожидаем выполнения операции
(void)I2C1->SR2;
(void)I2C1->SR1; // сбросим флаг ADDR

WRITE_REG (I2C1->DR, ((registr_adr & 0xFF00) >> 8)); // записываем первую половину адреса регистра
while(!READ_BIT (I2C1->SR1, I2C_SR1_BTF)) {}; // ожидаем выполнения операции
WRITE_REG (I2C1->DR, (uint8_t)(registr_adr & 0x00FF)); // записываем вторую половину адреса регистра
while(!READ_BIT (I2C1->SR1, I2C_SR1_BTF)) {}; // ожидаем выполнения операции

for(uint8_t i = 0; i < data_len; i++)
	{
	while(!READ_BIT (I2C1->SR1, I2C_SR1_BTF)) {}; // ожидаем выполнения операции
	I2C1->DR = *data++; // записываем данные из массива
	}
SET_BIT (I2C1->CR1, I2C_CR1_STOP); // Условие "Stop"
}

// I2C1 - чтение одиного байта данных
//
uint8_t I2C1_read_byte(uint8_t device_adr, uint16_t registr_adr)
{
uint8_t data_I2C;

SET_BIT (I2C1->CR1, I2C_CR1_ACK); //передавать бит подтверждения ACK

SET_BIT (I2C1->CR1, I2C_CR1_START); // Условие "Start"
while(!READ_BIT(I2C1->SR1, I2C_SR1_SB)) {}; // ожидаем выполнения операции
(void) I2C2->SR1;

WRITE_REG (I2C1->DR, (device_adr | I2C_WR)); // записываем адрес устройства и операцию записи
while(!READ_BIT(I2C1->SR1, I2C_SR1_ADDR)) {}; // ожидаем выполнения операции
(void)I2C1->SR2;
(void)I2C1->SR1; // сбросим флаг ADDR

WRITE_REG (I2C1->DR, ((registr_adr & 0xFF00) >> 8)); // записываем адрес регистра
while(!READ_BIT (I2C1->SR1, I2C_SR1_BTF)) {}; // ожидаем выполнения операции
WRITE_REG (I2C1->DR, (uint8_t)(registr_adr & 0x00FF)); // записываем адрес регистра
while(!READ_BIT (I2C1->SR1, I2C_SR1_BTF)) {}; // ожидаем выполнения операции

SET_BIT (I2C1->CR1, I2C_CR1_START); // Условие повторный "Start"
while(!READ_BIT(I2C1->SR1, I2C_SR1_SB)) {}; // ожидаем выполнения операции
(void)I2C1->SR1;

WRITE_REG (I2C1->DR, (device_adr | I2C_RD)); // записываем адрес устройства и операцию чтения
while(!READ_BIT(I2C1->SR1, I2C_SR1_ADDR)) {}; // ожидаем выполнения операции
(void)I2C1->SR1;
(void)I2C1->SR2; // сбросим флаг ADDR

while(!READ_BIT (I2C1->SR1, I2C_SR1_RXNE)) {} // ожидание приема данных
CLEAR_BIT (I2C1->CR1, I2C_CR1_ACK);  //отправим сигнал NACK
SET_BIT (I2C1->CR1, I2C_CR1_STOP); // Условие "Stop"
return data_I2C = I2C1->DR;
}

// I2C1 - чтение нескольких байт данных

void I2C1_read_array(uint8_t device_adr, uint8_t registr_adr, uint8_t *data, uint8_t data_len)  //data_len - количество байт необходимых получить
{
SET_BIT (I2C1->CR1, I2C_CR1_ACK); // передавать бит подтверждения ACK

SET_BIT (I2C1->CR1, I2C_CR1_START); // Условие "Start"
while(!READ_BIT(I2C1->SR1, I2C_SR1_SB)) {}; // ожидаем выполнения операции "Start"

WRITE_REG (I2C1->DR, (device_adr + I2C_WR)); // записываем адрес устройства и операцию записи
while(!READ_BIT(I2C1->SR1, I2C_SR1_ADDR)) {}; // ожидаем выполнения операции
(void)I2C1->SR2;
(void)I2C1->SR1; // сбросим флаг ADDR

WRITE_REG (I2C1->DR, ((registr_adr & 0xFF00) >> 8)); // записываем адрес регистра
while(!READ_BIT (I2C1->SR1, I2C_SR1_BTF)) {}; // ожидаем выполнения операции
WRITE_REG (I2C1->DR, (uint8_t)(registr_adr & 0x00FF)); // записываем адрес регистра
while(!READ_BIT (I2C1->SR1, I2C_SR1_BTF)) {}; // ожидаем выполнения операции

SET_BIT (I2C1->CR1, I2C_CR1_START); // Условие повторный "Start"
while(!READ_BIT(I2C1->SR1, I2C_SR1_SB)) {}; // ожидаем выполнения операции "Start"

WRITE_REG (I2C1->DR, (device_adr + I2C_RD)); // записываем адрес устройства и операцию чтения
while(!READ_BIT(I2C1->SR1, I2C_SR1_ADDR)) {}; // ожидаем выполнения операции
(void)I2C1->SR2;
(void)I2C1->SR1; // сбросим флаг ADDR

for(uint8_t i = 0; i < data_len; i++)
	{
	if(i == (data_len-1))
		{
		CLEAR_BIT (I2C1->CR1, I2C_CR1_ACK); //отправим сигнал NACK
		SET_BIT (I2C1->CR1, I2C_CR1_STOP); // Условие "Stop"
		}
	while(!READ_BIT (I2C1->SR1, I2C_SR1_RXNE)) {}; // ожидание приема данных
	*data++ = I2C1->DR; // записываем данные в массив
	}
}
