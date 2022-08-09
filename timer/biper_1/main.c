// ������ � ���������. �����. ���������� �������, ������ ������������ �������.
//
#define stm32f4xx
#include "stm32f4xx.h" // �������� ���������
#include "DELAY_TIM7.h" // ������� ��������
#define FREQ_TIM3 100000
volatile unsigned int beep_time_cnt; // ������� ������� �������� ������, ��
unsigned int beep_freq; // ������� �������� ������, ��

int main()
{
//��������� ����� PD ��� ���������� ������������
RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; //��������� ���� IO D
GPIOD->MODER |= GPIO_MODER_MODER12_0 | GPIO_MODER_MODER13_0 | GPIO_MODER_MODER14_0; //���� D 12, 13, 14 ��� �� ������� �����
GPIOD->OTYPER = 0x0;  //����� � ��������� (push-pull)
GPIOD->OSPEEDR = 0x0;  //�������� ����� D ����� ������

//��������� ����� PE11 ��� ���������� �������
RCC->AHB1ENR |= RCC_AHB1ENR_GPIOEEN; // �������� �������� ��������� ����� E
GPIOE->MODER |= GPIO_MODER_MODER11_0; // �������� �� ����� ���� PE11

// ��������� ������� TIM4 ������������ �������� ������
RCC->APB1ENR |= RCC_APB1ENR_TIM4EN; // ������� ������������ �� TIM4
// ������������ ���������� TIM4 ����� 1 ���� (1 ���)
//�������� �������=16000000; 1000=16000000/(������������)*(�������� ���������)=>(������������)*(�������� ���������)=16000
TIM4->PSC = 160 - 1; // �������� ������������ (�� ���� TIM4 16000/160=100���)
TIM4->ARR = 100; // �������� ��������� (���������� TIM4 ��� � 1 ��)
TIM4->DIER |= TIM_DIER_UIE; // �������� ���������� ��� ���������� ������� TIM4
TIM4->CR1 |= TIM_CR1_CEN; // �������� ������ ������� TIM4
NVIC_EnableIRQ(TIM4_IRQn); // ��������� ���������� � ����������� NVIC

// ��������� ������� TIM3 ��������� ����� ������
RCC->APB1ENR |= RCC_APB1ENR_TIM3EN; // ������ ������������ �� TIM3
TIM3->PSC = 160 - 1; // �������� ������������ (�� ���� TIM3 16000/160=100���)
TIM3->ARR = 0; // ���������� TIM3 ��� � 0,01 ��
 TIM3->DIER |= TIM_DIER_UIE; // �������� ���������� ��� ���������� ������� TIM3
TIM3->CR1 |= TIM_CR1_CEN; // �������� ������ ������� TIM3
NVIC_EnableIRQ(TIM3_IRQn); // ��������� ���������� � ����������� NVIC

while(1)
	{
	beep_freq = 2500; // ������� ��������, ��
	TIM3->ARR = (float) FREQ_TIM3 / (beep_freq*2); // �������� ��������� (������� �� ����� �������/(��������� ������� ���������*2))
	//������������ ���������� �� TIM3: 1/(100000/TIM3->ARR) = 1/(100000/20)=1/5000=0,0002�=0,2��
	//������� ������������� ������� �����: 1/(����� ����������*2)
	beep_time_cnt = 750; // ������������ � ��
	GPIOD->BSRRH |= GPIO_BSRR_BS_14; //��������� 14 ���,
	GPIOD->BSRRL |= GPIO_BSRR_BS_12;  //�������� 12 ���
	delay_ms(1000);
	beep_freq = 400; // ������� ��������, ��
	TIM3->ARR = (float) FREQ_TIM3 /(beep_freq*2); // �������� ��������� TIM3
	beep_time_cnt = 500;
    GPIOD->BSRRH |= GPIO_BSRR_BS_12; //��������� 12 ���
    GPIOD->BSRRL |= GPIO_BSRR_BS_13;  //�������� 13 ���
	delay_ms(750);
	beep_freq = 1000; // ������� ��������, ��
	TIM3->ARR = (float) FREQ_TIM3 /(beep_freq*2); // �������� ��������� TIM3
	beep_time_cnt = 1000;
	GPIOD->BSRRH |= GPIO_BSRR_BS_13; //��������� 13 ���
	GPIOD->BSRRL |= GPIO_BSRR_BS_14;  //�������� 14 ���
	delay_ms(1250);
	}
}
// ������� ��������� ���������� TIM4
void TIM4_IRQHandler(void)
{
TIM4->SR &= ~TIM_SR_UIF; // ������� ���� ����������
if(beep_time_cnt) // �������� ������� �������
	{
	beep_time_cnt--; // ��������� ������� �������
	}
}

// ������� ��������� ���������� TIM3
void TIM3_IRQHandler(void)
{
TIM3->SR &= ~TIM_SR_UIF; // ������� ���� ����������
// �����
if(beep_time_cnt) // �������� ������� �������
	{
	GPIOE->ODR ^= GPIO_ODR_ODR_11; // ����������� ���� ��� ������
	}
else
	{
	GPIOE->ODR &= ~GPIO_ODR_ODR_11; // �������� ���� ��� ������
	}
}
