//���. ����

#define stm32f4xx
#include "stm32f4xx.h" // �������� ���������


int main()
{
uint16_t cnt = 0; // ������� ��� ������ ������� ���

// ��������� ����� �����-������ ��� DAC1
SET_BIT (RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN);// ��������� �������� ��������� GPIOA
SET_BIT (GPIOA->MODER, GPIO_MODER_MODER4); //��� PA4 � ���������� ����� ��� DAC_OUT1, ����� �������� ����������� �����������
SET_BIT (GPIOA->MODER, GPIO_MODER_MODER5); //��� PA5 � ���������� ����� ����� ��� DAC_OUT2, ����� �������� ����������� �����������

// ��������� DAC
SET_BIT (RCC->APB1ENR, RCC_APB1ENR_DACEN); // ��������� �������� ��������� DAC
SET_BIT (DAC->CR, DAC_CR_TEN1); //������� ���1 �������, ������ �� �������� DAC->DHR�, ���������� ����� 3 ����� APB1 � ������� DAC_DOR1 (��� ���������� ������� ����� ���� ����)
SET_BIT (DAC->CR, DAC_CR_TEN2); //������� ���1 �������, ������ �� �������� DAC->DHR�, ���������� ����� 3 ����� APB1 � ������� DAC_DOR2 (��� ���������� ������� ����� ���� ����)
SET_BIT (DAC->CR, (DAC_CR_TSEL1_2 | DAC_CR_TSEL1_1 | DAC_CR_TSEL1_0)); // 111 - ������� ������ �������� ����������� ������ ���1
SET_BIT (DAC->CR, (DAC_CR_TSEL2_2 | DAC_CR_TSEL2_1 | DAC_CR_TSEL2_0)); //111 - ������� ������ �������� ����������� ������ ���2
SET_BIT (DAC->CR, DAC_CR_WAVE1_1); //��������� ���� � ���1
SET_BIT (DAC->CR, DAC_CR_WAVE2_1); //��������� ���� � ���2
SET_BIT (DAC->CR, (DAC_CR_MAMP1_3 | DAC_CR_MAMP1_1)); //������������ ��������� ���� �� ���1
SET_BIT (DAC->CR, (DAC_CR_MAMP2_3 | DAC_CR_MAMP2_0)); //������������ ��������� ���� �� ���2
SET_BIT (DAC->CR,  DAC_CR_EN1); // ��������� ���1
SET_BIT (DAC->CR,  DAC_CR_EN2); // ��������� ���2

// �������� ���� ���������
while(1)
	{
	SET_BIT (DAC->SWTRIGR, (DAC_SWTRIGR_SWTRIG1 | DAC_SWTRIGR_SWTRIG2)); // �������� ����������� ������ ���1 � ���2
	while (READ_BIT (DAC->SWTRIGR, (DAC_SWTRIGR_SWTRIG1 | DAC_SWTRIGR_SWTRIG2))) {}; // ������� ��������� �������������� ���1 � ���2
	//while (READ_BIT (DAC->SWTRIGR, DAC_SWTRIGR_SWTRIG2)) {}; // ������� ��������� ��������������
	}
}
