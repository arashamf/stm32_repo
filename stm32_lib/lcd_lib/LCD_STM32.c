#define stm32f4xx
#include "stm32f4xx.h" // �������� ���������
#include "LCD_STM32.h" 
#include "stdio.h"

// ����������� �������, � ������� ������������ LCD: D4-D7, R/S, R/W, E
#define LCD_CHAR_D4_PORT GPIOE->ODR
#define LCD_CHAR_D4_PIN GPIO_ODR_ODR_7 //����� PE7
#define LCD_CHAR_D5_PORT GPIOE->ODR
#define LCD_CHAR_D5_PIN GPIO_ODR_ODR_10  //����� PE10
#define LCD_CHAR_D6_PORT GPIOE->ODR
#define LCD_CHAR_D6_PIN GPIO_ODR_ODR_14 //����� PE14
#define LCD_CHAR_D7_PORT GPIOE->ODR
#define LCD_CHAR_D7_PIN GPIO_ODR_ODR_15 //����� PE15
#define LCD_CHAR_RS_PORT GPIOA->ODR
#define LCD_CHAR_RS_PIN GPIO_ODR_ODR_2 //����� PA2
#define LCD_CHAR_RW_PORT GPIOA->ODR
#define LCD_CHAR_RW_PIN GPIO_ODR_ODR_3 //����� PA3
#define LCD_CHAR_E_PORT GPIOA->ODR
#define LCD_CHAR_E_PIN GPIO_ODR_ODR_1 //����� PA1

// ������� ������ ��� � �����
#define SetBit(PORT, BIT) PORT|=BIT // ��������� ����
#define ClrBit(PORT, BIT) PORT&=~BIT  // ����� ����

//������� �������� ������ ��, ������������ � LCD
void LCD_STM_INIT () //�-� ������������� ������ IO �� ��� ����������� LCD
{
RCC->AHB1ENR |= RCC_AHB1ENR_GPIOAEN; // ��������� �������� ��������� GPIOA
RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN; // ��������� �������� ��������� GPIOE

// ��������� ������ �� ����� 
GPIOE->MODER |= GPIO_MODER_MODER7_0; // D4
GPIOE->MODER |= GPIO_MODER_MODER10_0; // D5
GPIOE->MODER |= GPIO_MODER_MODER14_0; // D6
GPIOE->MODER |= GPIO_MODER_MODER15_0; // D7
GPIOA->MODER |= GPIO_MODER_MODER2_0; // RS
GPIOA->MODER |= GPIO_MODER_MODER3_0; // RW
GPIOA->MODER |= GPIO_MODER_MODER1_0; // E	
}

// ������� ������ �������/������
void LCD_CHAR_wr(char LCD_CHAR_data, unsigned char LCD_CHAR_RS) //LCD_CHAR_RS==1 - ������� ������, 0 - �������
{
// ����������� �������
if(LCD_CHAR_RS)
	SetBit(LCD_CHAR_RS_PORT, LCD_CHAR_RS_PIN); // R/S = 1 (������)
else
	ClrBit(LCD_CHAR_RS_PORT, LCD_CHAR_RS_PIN); // R/S = 0 (�������)
ClrBit(LCD_CHAR_RW_PORT, LCD_CHAR_RW_PIN); // R/W = 0 (������)
// �������� ������� ������� ������
if(LCD_CHAR_data & 0x10)
	SetBit(LCD_CHAR_D4_PORT, LCD_CHAR_D4_PIN);
else
	ClrBit(LCD_CHAR_D4_PORT, LCD_CHAR_D4_PIN);
if(LCD_CHAR_data & 0x20)
	SetBit(LCD_CHAR_D5_PORT, LCD_CHAR_D5_PIN);
else
	ClrBit(LCD_CHAR_D5_PORT, LCD_CHAR_D5_PIN);
if(LCD_CHAR_data & 0x40)
	SetBit(LCD_CHAR_D6_PORT, LCD_CHAR_D6_PIN);
else
	ClrBit(LCD_CHAR_D6_PORT, LCD_CHAR_D6_PIN);
if(LCD_CHAR_data & 0x80)
	SetBit(LCD_CHAR_D7_PORT, LCD_CHAR_D7_PIN);
else
	ClrBit(LCD_CHAR_D7_PORT, LCD_CHAR_D7_PIN);
// ����������� �������
SetBit(LCD_CHAR_E_PORT, LCD_CHAR_E_PIN); // E = 1
delay_us(2);
ClrBit(LCD_CHAR_E_PORT, LCD_CHAR_E_PIN); // E = 0
// �������� ������� ������� ������
if(LCD_CHAR_data & 0x01)
	SetBit(LCD_CHAR_D4_PORT, LCD_CHAR_D4_PIN);
else
	ClrBit(LCD_CHAR_D4_PORT, LCD_CHAR_D4_PIN);
if(LCD_CHAR_data & 0x02)
	SetBit(LCD_CHAR_D5_PORT, LCD_CHAR_D5_PIN);
else
	ClrBit(LCD_CHAR_D5_PORT, LCD_CHAR_D5_PIN);
if(LCD_CHAR_data & 0x04)
	SetBit(LCD_CHAR_D6_PORT, LCD_CHAR_D6_PIN);
else
	ClrBit(LCD_CHAR_D6_PORT, LCD_CHAR_D6_PIN);
if(LCD_CHAR_data & 0x08)
	SetBit(LCD_CHAR_D7_PORT, LCD_CHAR_D7_PIN);
else
	ClrBit(LCD_CHAR_D7_PORT, LCD_CHAR_D7_PIN);
// ����������� �������
SetBit(LCD_CHAR_E_PORT, LCD_CHAR_E_PIN); // E = 1
delay_us(2); //��������
ClrBit(LCD_CHAR_E_PORT, LCD_CHAR_E_PIN); // E = 0
}


// ������� ���� ����������������� ������������������
void LCD_CHAR_init_step(char LCD_CHAR_data) 
{
// ����������� �������
ClrBit(LCD_CHAR_RS_PORT, LCD_CHAR_RS_PIN); // R/S = 0 (�������)
ClrBit(LCD_CHAR_RW_PORT, LCD_CHAR_RW_PIN); // R/W = 0 (������)
// �������� ������� ������� ������
if(LCD_CHAR_data & 0x10)
	SetBit(LCD_CHAR_D4_PORT, LCD_CHAR_D4_PIN);
else
	ClrBit(LCD_CHAR_D4_PORT, LCD_CHAR_D4_PIN);
if(LCD_CHAR_data & 0x20)
	SetBit(LCD_CHAR_D5_PORT, LCD_CHAR_D5_PIN);
else
	ClrBit(LCD_CHAR_D5_PORT, LCD_CHAR_D5_PIN);
if(LCD_CHAR_data & 0x40)
	SetBit(LCD_CHAR_D6_PORT, LCD_CHAR_D6_PIN);
else
	ClrBit(LCD_CHAR_D6_PORT, LCD_CHAR_D6_PIN);
if(LCD_CHAR_data & 0x80)
	SetBit(LCD_CHAR_D7_PORT, LCD_CHAR_D7_PIN);
else
	ClrBit(LCD_CHAR_D7_PORT, LCD_CHAR_D7_PIN);
// ����������� �������
SetBit(LCD_CHAR_E_PORT, LCD_CHAR_E_PIN); // E = 1
delay_us(2); 
ClrBit(LCD_CHAR_E_PORT, LCD_CHAR_E_PIN); // E = 0
}

// ������� ������������� LCD
void LCD_CHAR_init(unsigned char columns) //columns - ����� �������� � ������
{
// ��������� - 4-������, ����� - 2, ����� - 5�7 �����
// ���������������� ������������������
delay_ms(30); 
LCD_CHAR_init_step(0x30);  //��������� 8 ��������� ���� �������
delay_us(40); 
LCD_CHAR_init_step(0x20);
delay_us(40); 
// ��������� ����������
 LCD_CHAR_wr(0x28, 0); // 4-������ ���������
delay_us(40);
// ��������� �����������
LCD_CHAR_wr(0x0C, 0);  //�������� �������, ��������� ������
delay_us(40); 
LCD_CHAR_wr(0x06, 0); //������� �� ��������
delay_us(40); 
LCD_CHAR_wr(0x01, 0); //�������� ������� � ���������� ������ � ��������� �������
delay_ms(2);
}

// ������� ��������� ������� ���������� ��� ������
void LCD_CHAR_gotoxy(unsigned char column, unsigned char row) //column - �������, row - ������
{
if (row == 0)
	LCD_CHAR_wr(0x80 + 0x00 + column, 0);
else
	if (row == 1)
		LCD_CHAR_wr(0x80 + 0x40 + column, 0);
delay_us(40); 
}

// ������� ������ ������ str �� RAM �� ��������� � �������� �������
void LCD_CHAR_puts(char *str)
{
char c;
// ���������� ������� ������ ���������������, ���� �� ��������� ������ ����� ������ "0x00"
while((c = *(str++)))
	{
	LCD_CHAR_wr(c, 1); // ������ ������
	delay_us(40); 
	}
}

// ������� �������� � ���
void delay_us(unsigned int us)
{
RCC->APB1ENR |= RCC_APB1ENR_TIM7EN; // �������� �������� ���������
TIM7->CNT = 0; // ������� ������� �������
TIM7->PSC = 16-1; //��������, 16000000/16 = 1000000 = 1 ���
TIM7->ARR = us;
TIM7->EGR |= TIM_EGR_UG; /* ��� ��������������� ����������, ������������ ������������� ���������; ������������� ������� �������,
������� ���������� � ��������� �������� � ������������� (��������� ������� �������� ���������� �� ��������������� ���������).
���� ��� UDIS=1, �������� ������������, �� ������� �������� �� �����������.*/
TIM7->SR &= ~TIM_SR_UIF; // ������� ���� ����������
TIM7->CR1 |= TIM_CR1_CEN; // �������� ������ �� ����
while(!(TIM7->SR & TIM_SR_UIF)) {} //  ���� ���������� ���� ���������� ������� � "1", �� ���� �������������
TIM7->CR1 &= ~TIM_CR1_CEN; // ��������� ������
TIM7->SR &= ~TIM_SR_UIF; // ������� ���� ������� ����������
}

// ������� �������� � ��
void delay_ms(unsigned int ms)
{
RCC->APB1ENR |= RCC_APB1ENR_TIM7EN; // �������� �������� ���������
TIM7->CNT = 0; // ������� ������� �������
TIM7->PSC = 16000-1; // ��������, 16000000/16000 = 1000 = 1 ��
TIM7->ARR = ms;
TIM7->EGR |= TIM_EGR_UG; // ����������������� �������
TIM7->SR &= ~TIM_SR_UIF; // ������� ���� ����������
TIM7->CR1 |= TIM_CR1_CEN; // �������� ������ �� ����
while(!(TIM7->SR & TIM_SR_UIF)) {} //����� ���: while((TIM7->SR & TIM_SR_UIF)==0){};
TIM7->CR1 &= ~TIM_CR1_CEN; // ��������� ������
TIM7->SR &= ~TIM_SR_UIF; // ������� ���� ������� ����������
}
