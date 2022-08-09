//���. ����

#define stm32f4xx
#include "stm32f4xx.h" // �������� ���������


int main()
{
uint16_t cnt; // ������� ��� ������ ������� ���

// ��������� ����� �����-������ ��� DAC1
SET_BIT (RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN);// ��������� �������� ��������� GPIOA
SET_BIT (GPIOA->MODER, GPIO_MODER_MODER4); //��� PA4 � ���������� ����� ��� DAC_OUT1, ����� �������� ����������� �����������
SET_BIT (GPIOA->MODER, GPIO_MODER_MODER5); //��� PA5 � ���������� ����� ����� ��� DAC_OUT2, ����� �������� ����������� �����������

// ��������� DAC
SET_BIT (RCC->APB1ENR, RCC_APB1ENR_DACEN); // ��������� �������� ��������� DAC
SET_BIT (DAC->CR, (DAC_CR_TSEL1_2 | DAC_CR_TSEL1_1 | DAC_CR_TSEL1_0)); // 111 - ����������� ������ ���1
SET_BIT (DAC->CR, (DAC_CR_TSEL2_2 | DAC_CR_TSEL2_1 | DAC_CR_TSEL2_0)); //111 - ����������� ������ ���2
//SET_BIT (DAC->CR, DAC_CR_WAVE1_1);
SET_BIT (DAC->CR,  DAC_CR_EN1); // ��������� 1 ������ ���
SET_BIT (DAC->CR,  DAC_CR_EN2); // ��������� 2 ������ ���

// �������� ���� ���������
while(1)
	{
	for(cnt = 0; cnt < 4096; cnt++)
		{
		WRITE_REG(DAC->DHR12R1, cnt); // ������� ����� �������� � ������� � ������ ������������� DAC->DHR12R1 (0 ��� � 0, 11 � 11)
		SET_BIT (DAC->SWTRIGR, DAC_SWTRIGR_SWTRIG1); // �������� ����������� ������ ���1
		while(READ_BIT (DAC->SWTRIGR, DAC_SWTRIGR_SWTRIG1)) // ������� ��������� �������������� ���1
			{}; //��� ��������� SWTRIG1 ���������� ������������ (���� ����), ��� ������ �������� �������� DAC_DHR1��� ���� ��������� � ������� DAC_DOR1
		}
	}
}
