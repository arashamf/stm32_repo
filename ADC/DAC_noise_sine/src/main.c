//���. ��������� � ���

#define stm32f4xx
#include "stm32f4xx.h" // �������� ���������
#include "math.h"
typedef unsigned char byte;


int main()
{
//byte cnt = 0; // ������� ��� ������ ������� ���

//----------------������ ��� ������������ ���������------------------//
uint16_t sinus[72]={2225,2402,2577,2747,2912,3070,3221,3363,3494,3615,
					3724,3820,3902,3971,4024,4063,4085,4095,4085,4063,
					4024,3971,3902,3820,3724,3615,3495,3363,3221,3071,
					2912,2747,2577,2403,2226,2047,1869,1692,1517,1347,
					1182,1024, 873, 731, 600, 479, 370, 274, 192, 124,
					70,	 31,	10,	0,	 10,  31,  70,  123, 192, 274,
					370, 479, 599, 731, 873,1023,1182,1347,1517, 1691,
					1868,2047
					};

// ��������� ����� �����-������ ��� DAC1
SET_BIT (RCC->AHB1ENR, RCC_AHB1ENR_GPIOAEN);// ��������� �������� ��������� GPIOA
SET_BIT (GPIOA->MODER, GPIO_MODER_MODER4); //��� PA4 � ���������� ����� ��� DAC_OUT1, ����� �������� ����������� �����������
SET_BIT (GPIOA->MODER, GPIO_MODER_MODER5); //��� PA5 � ���������� ����� ����� ��� DAC_OUT2, ����� �������� ����������� �����������

// ��������� DAC
SET_BIT (RCC->APB1ENR, RCC_APB1ENR_DACEN); // ��������� �������� ��������� DAC
SET_BIT (DAC->CR, DAC_CR_TEN1); //������� ���1 �������, ������ �� �������� DAC->DHR�, ���������� ����� 3 ����� APB1 � ������� DAC_DOR1 (��� ����������� ������� ����� ���� ����)
SET_BIT (DAC->CR, DAC_CR_TEN2); //������� ���2 �������
SET_BIT (DAC->CR, (DAC_CR_TSEL1_2 | DAC_CR_TSEL1_1 | DAC_CR_TSEL1_0)); // 111 - ������� ������ �������� ����������� ������ ���1
//SET_BIT (DAC->CR, (DAC_CR_TSEL1_2) ); // ������� ������� ���1 - "Timer 2 TRGO event"
//SET_BIT (DAC->CR, DAC_CR_WAVE1_1); //��������� ���� � ���1
//SET_BIT (DAC->CR, DAC_CR_WAVE1_0); //��������� ���������������� ���� � ���1
SET_BIT (DAC->CR, (DAC_CR_TSEL2_2 | DAC_CR_TSEL2_1 | DAC_CR_TSEL2_0)); // 111 - ������� ������ �������� ����������� ������ ���2
SET_BIT (DAC->CR, DAC_CR_WAVE2_0); //��������� ���������������� ���� � ���2
SET_BIT (DAC->CR, (DAC_CR_MAMP2_3 | DAC_CR_MAMP2_1)); //������������ ��������� �� ���2
SET_BIT (DAC->CR,  DAC_CR_EN1); // ��������� ���1
SET_BIT (DAC->CR,  DAC_CR_EN2); // ��������� ���2
// ��������� ������� TIM2
/*SET_BIT (RCC->APB1ENR, RCC_APB1ENR_TIM2EN); // �������� �������� ��������� TIM2
WRITE_REG(TIM2->PSC, 800-1); // ������������
WRITE_REG(TIM2->ARR, 200-1); // �������� ������������
SET_BIT (TIM2->CCER, TIM_CCER_CC1E); // �������� ����� ������� �� ����� ��������� �� ����� ��
SET_BIT (TIM2->CR2, TIM_CR2_MMS_1); // � �������� TRGO ����� �������������� ������� "Update"
SET_BIT (TIM2->CR1, TIM_CR1_CEN); // �������� ������ �������*/

// �������� ���� ���������
while(1)
	{
	for (byte i = 0; i < 72; i++)
		{
		WRITE_REG (DAC->DHR12R1, sinus [i]);
		WRITE_REG (DAC->DHR12R2, sinus [i]);
		SET_BIT (DAC->SWTRIGR, (DAC_SWTRIGR_SWTRIG1 | DAC_SWTRIGR_SWTRIG2)); // �������� ����������� ������ ���1 � ���2
		while (READ_BIT (DAC->SWTRIGR, (DAC_SWTRIGR_SWTRIG1 | DAC_SWTRIGR_SWTRIG2))) {}; // ������� ��������� �������������� ���1 � ���2
		}
	}
}

