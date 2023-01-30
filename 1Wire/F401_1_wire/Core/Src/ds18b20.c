#include "ds18b20.h"
#include "delay.h"

//----------------------------------------------------------------------------//
void port_init(void)
{
  HAL_GPIO_DeInit(GPIOB, GPIO_PIN_13);
  GPIOB->MODER |= GPIO_MODER_MODER13_0; //General purpose output mode
	GPIOB->MODER &= (~GPIO_MODER_MODER13_1); //General purpose output mode
	GPIOB->OSPEEDR |= GPIO_OSPEEDR_OSPEED13 ; //Very high speed
  GPIOB->OTYPER |= GPIO_OTYPER_OT13; //Output open-drain	
}

//----------------------------------------------------------------------------//
uint8_t ds18b20_Reset(void)
{
  uint16_t status;
	GPIOB->ODR &= ~GPIO_ODR_OD13 ; //установка нуля на шине
  delay_us(485); //задержка минимум 480 мкс
  GPIOB->ODR |= GPIO_ODR_OD13; //установка единице на шине
  delay_us(65); //задержка минимум 60 мкс
  status = (GPIOB->IDR & GPIO_IDR_ID13 ? 1 : 0); //чтение уровня
	delay_us(500); //задержка минимум 480 мкс
  return status; 
}

//----------------------------------------------------------------------------//
uint8_t ds18b20_ReadBit(void)
{
  uint8_t bit = 0;
  GPIOB->ODR &= ~GPIO_ODR_OD13;
  delay_us(2); //задержка минимум 1 мкс
	GPIOB->ODR |= GPIO_ODR_OD13;
	delay_us(13);
	bit = (GPIOB->IDR & GPIO_IDR_ID13 ? 1 : 0); //чтение уровня
	delay_us(45);
  return bit;
}
//-----------------------------------------------
uint8_t ds18b20_ReadByte(void)
{
  uint8_t data = 0;
  for (uint8_t i = 0; i <= 7; i++)
  data += ds18b20_ReadBit() << i;
  return data;
}
//-----------------------------------------------
void ds18b20_WriteBit(uint8_t bit)
{
  GPIOB->ODR &= ~GPIO_ODR_OD13;
  delay_us (bit ? 3 : 65); //задержка 
  GPIOB->ODR |= GPIO_ODR_OD13;
  delay_us(bit ? 65 : 3);
}
//-----------------------------------------------
void ds18b20_WriteByte(uint8_t dt)
{
  for (uint8_t i = 0; i < 8; i++)
  {
    ds18b20_WriteBit(dt >> i & 1);   
    delay_us(5); //Delay Protection
  }
}
//-----------------------------------------------
uint8_t ds18b20_init(uint8_t mode)
{
	if(ds18b20_Reset()) return 1;
  if(mode==SKIP_ROM)
  {	
		ds18b20_WriteByte(0xCC); //SKIP ROM		
		ds18b20_WriteByte(0x4E); //WRITE SCRATCHPAD	
		ds18b20_WriteByte(0x64); //TH REGISTER 100 градусов		
		ds18b20_WriteByte(0x9E); //TL REGISTER - 30 градусов		
		ds18b20_WriteByte(RESOLUTION_12BIT); //Resolution 12 bit
  }
  return 0;
}
//----------------------------------------------------------
void ds18b20_MeasureTemperCmd(uint8_t mode, uint8_t DevNum)
{
  ds18b20_Reset();
  if(mode==SKIP_ROM)
  {    
    ds18b20_WriteByte(0xCC); //SKIP ROM
  }
  ds18b20_WriteByte(0x44); //CONVERT T
}
//----------------------------------------------------------ф-я считывания памяти----------------------------------------------------------//
void ds18b20_ReadStratcpad(uint8_t mode, uint8_t *Data, uint8_t DevNum)
{
  uint8_t i;
  ds18b20_Reset();
  if(mode==SKIP_ROM)
  {  
    ds18b20_WriteByte(0xCC);  //SKIP ROM
  }
  ds18b20_WriteByte(0xBE);   //READ SCRATCHPAD
  for(i=0;i<8;i++)
  {
    Data[i] = ds18b20_ReadByte();
  }
}
//----------------------------------------------------------
uint8_t ds18b20_GetSign(uint16_t dt)
{
  if (dt&(1<<11)) return 1; //проверка 11 бита
  else return 0;
}
//----------------------------------------------------------
float ds18b20_Convert(uint16_t dt)
{
  float t;
  t = (float) ((dt&0x07FF)>>4); //знаковые и дробные биты сброшены 
  t += (float)(dt&0x000F) / 16.0f; //прибавка дробной части
  return t;
}
