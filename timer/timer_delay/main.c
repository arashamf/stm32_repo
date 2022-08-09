/* ������ 8. ������� �������� � �������������� �������*/

#define stm32f4xx
#include "stm32f4xx.h" // �������� ���������
#include "DELAY_TIM7.h" // ������� ��������

int main()
{
RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; // �������� �������� ��������� ����� D
GPIOD->MODER |= GPIO_MODER_MODER13_0; //��� 13 �� �����
GPIOD->OTYPER = 0x0;  //����� � ��������� (push-pull)
GPIOD->OSPEEDR = 0x0;  //�������� ����� D ����� ������

while(1)
	{
	GPIOD->ODR |= GPIO_ODR_ODR_13; // ��������
	delay_ms(1000); // ��������
	GPIOD->ODR &= ~GPIO_ODR_ODR_13; // ���������
	delay_ms(500); // ��������
	}
}


