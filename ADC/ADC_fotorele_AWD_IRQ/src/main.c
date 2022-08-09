// ���. ����������� ������. ��������. ������������� AWD & IRQ

#define stm32f4xx
#include "stm32f4xx.h" // �������� ���������
#include "stdio.h" // �������� ����������� �������

// ����������� ������ ��� ��
#define BANDWITH 57600 //���������� ����������� ����
#define APB1_FREQUENCY 16000000L // ������� ���� APB1

#include "DELAY_TIM7.h" // ������� ��������
#include "UART_STM32.h" // ���������� ������� ��� ������ � USART3
char msg[130]; // ������ ��� ������������ ��������� ��� ������ �� UART

__I float Vref = 3.3; // ������� ���������� ��� ���; __I = volatile const
__IO float ADC_float = 0; // ��������� ��� � ���� ����������; __IO = volatile

// ������ ������������ ��������, �
const float theshold_hi = 2.8; // �������
const float theshold_lo = 1.6; // ������

void WWDG_IRQHandler(void)
{
if(ADC_float > theshold_hi)
	GPIOD->BSRRL |= GPIO_BSRR_BS_12; // ��������
else
	if(ADC_float < theshold_lo)
		GPIOD->BSRRH |= GPIO_BSRR_BS_12; // ���������
ADC1->SR &= ~ADC_SR_AWD; // ������� ����
}


int main()
{
//��������� ������ 12 ��� �������� ������������
RCC->AHB1ENR |= RCC_AHB1ENR_GPIODEN; // ��������� �������� ��������� GPIOD
GPIOD->MODER |= GPIO_MODER_MODER12_0; //��� 12 �� ������� �����

// ��������� ����� � ��� ���
RCC->AHB1ENR |= RCC_AHB1ENR_GPIOCEN; // ��������� �������� ��������� GPIOC
GPIOC->MODER |= GPIO_MODER_MODER2_1 | GPIO_MODER_MODER2_0; // ����������� ����� PC2 �� ���������� �����
GPIOC->MODER |= GPIO_MODER_MODER1_1 | GPIO_MODER_MODER1_0; // ����������� ����� PC1 �� ���������� �����

// ��������� ���
RCC->APB2ENR |= RCC_APB2ENR_ADC1EN; // �������� �������� ���������
/*������� SQx[4:0] ������ ����� ������, ��� � � ��� ����� ��������������.*/
CLEAR_BIT(ADC1->SQR1, ADC_SQR1_L); //������� ���������� ������� ���������� ������: 1
ADC1->SQR3 |= ADC_SQR3_SQ1_0 | ADC_SQR3_SQ1_1| ADC_SQR3_SQ1_3; // ����� 11 (PC1)
ADC1->CR2 |= ADC_CR2_ADON; // �������� ���

//��������� Analog watchdog
ADC1->HTR = theshold_hi*4096/Vref; // ������� ����� ������������
ADC1->LTR = theshold_lo*4096/Vref; // ������ ����� ������������
SET_BIT(ADC1->CR1, ADC_CR1_AWDSGL); //AWD  enabled on a single channel
ADC1->CR1 |= ADC_CR1_AWDCH_0 | ADC_CR1_AWDCH_1 | ADC_CR1_AWDCH_4; //�������� 11 �����
ADC1->CR1 |= ADC_CR1_AWDEN; // AWD enable on regular channels
SET_BIT(ADC1->CR1, ADC_CR1_AWDIE);  //Analog watchdog interrupt enabled
NVIC_EnableIRQ(WWDG_IRQn); //������� ���������� �� AWD �� ����������� ����������

UART3_init(APB1_FREQUENCY, BANDWITH); // ������������� UART3
UART3_PutRAMString("ADC fotorele with AWD_IRQ:\r\n");

__enable_irq(); // �������� ���������� ���������

while(1)
	{
	ADC1->CR2 |= ADC_CR2_SWSTART; // ����������� ������ ���, ����� ������� �������������� ���������� ��������� ��������
	while(!(ADC1->SR & ADC_SR_EOC)) {}; // ������� ADC_SR - ��������� �������, ��������� ����, ��������������� �� ��������� ��������������
	/* ������� ����������*��������� ��������������/������������ ����� ���������*/
	sprintf(msg,("ADC_result 11 = %f V\r\n"), ADC_float = Vref*ADC1->DR/4096); // ������� �������� ��� � ������
	UART3_PutRAMString(msg); // ������� ������ �� UART
		// �������� ������
	delay_ms(1000); // �������� 1 �
	}
}

