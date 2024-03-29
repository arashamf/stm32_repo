#include "stm32f4xx.h"
#include "DELAY_TIM7.h" 
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
