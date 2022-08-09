//�������������� ������ ��������� ��������������� ������� ����� ����������.
#define stm32f4xx
#include "stm32f4xx.h" // �������� ���������
#include "stdio.h" // �������� ����������� �������
// ����������� ������ ���
#define APB1_FREQUENCY 16000000L // ������� ����������
#define UART_BANDWITH 57600 //���������� ����������� UART
typedef unsigned char byte;

#include "DELAY_TIM7.h" // ������� ��������
#include "UART_STM32.h" // ���������� ������� ��� ������ � USART3
char msg[100]; // ������ ��� ������������ ��������� ��� ������ �� UART

/*���������� ����������� �������������� ���������� ������� � ������, �.�. ���������������� ����� �� �� ��������� � ���������� ��������
 � ���������� ���������� (��������� ����� �OVR�). ������� ����� �������� ����� ���������� ���������� � �������� �������� ������� ���*/
float ADC_DR_tmp[16]; // ������ ��� �������� ����������� ��� ���������� �������
__IO byte ADC_DR_tmp_ptr = 0; // ��������� �� ������� � ������� ADC_DR_tmp
__IO byte measure_good = 0; // ����: 1 - ��������� ������, 0 - ���
__IO byte is_OVR = 0; // ����: 1 - ���� ���������� ����������, 0 - ���
			
//���
#define LENGTH 200

// ���
float Vref = 3.3; // ������� ���������� ��� ���
float ADC_float; // ��������� ��� � ���� ����������

// ������ ����������� (������ �� �������� ��� ������� �������� ����������� �� �������)
#define V25 0.76 // VSENSE value for 25� C
#define Avg_Slope 2.5 // mV/�C

int main()
{
// ��������� ������ ���
SET_BIT (RCC->AHB1ENR, RCC_AHB1ENR_GPIOCEN); // ��������� �������� ��������� GPIOC
SET_BIT (GPIOC->MODER, GPIO_MODER_MODER2_0 | GPIO_MODER_MODER2_1); // ����������� ����� PC2 �� ���������� �����
SET_BIT (GPIOC->MODER, GPIO_MODER_MODER1_0 | GPIO_MODER_MODER1_1); // ����������� ����� PC1 �� ���������� �����

// ��������� ���
SET_BIT (RCC->APB2ENR, RCC_APB2ENR_ADC1EN); // �������� �������� ���������;
// ���������� ������:
SET_BIT (ADC1->SQR1, ADC_SQR1_L_0); // �������������� 2-� �������
SET_BIT (ADC1->SQR3, (ADC_SQR3_SQ1_0 | ADC_SQR3_SQ1_1 | ADC_SQR3_SQ1_3)); // ����� 11 (PC1)
SET_BIT (ADC1->SQR3, (ADC_SQR3_SQ2_2 | ADC_SQR3_SQ2_3)); // ����� 12 (PC2)
SET_BIT (ADC1->CR2, ADC_CR2_EOCS); //���� ���������� ��� EOCS: ��� EOC ��������������� � ����� ������� ����������� ��������������. �������� ����������� ������������.
SET_BIT (ADC1->CR1, ADC_CR1_EOCIE); //�������� ���������� �� ��������� ��������������
// ��������������� ������: ��� 1 ������ ����� ������ ���� ������� � JSQ4
// ��� 2 ������� ������ ������� ������������������ ������ ���� �������� � JSQ3 � JSQ4
// ��� 3 ������� ������ ������� ������������������ ������ ���� �������� � JSQ2 � JSQ3 � JSQ4
// ��� 4 ������� ������ ������� ������������������ ������ ���� �������� � JSQ1 � JSQ2 � JSQ3 � JSQ4
CLEAR_BIT (ADC1->CR1, (ADC_CR1_RES_0 | ADC_CR1_RES_1)); //12 ��� ���
CLEAR_BIT (ADC1->CR2, ADC_CR2_ALIGN); //������ ������������ �����
SET_BIT (ADC1->JSQR, ADC_JSQR_JL_1); //3 ��������������
SET_BIT (ADC1->JSQR, (ADC_JSQR_JSQ2_4)); //���������� ������ �����������: ����� 16 � JSQ2
SET_BIT (ADC1->JSQR, (ADC_JSQR_JSQ3_4 | ADC_JSQR_JSQ3_0)); //���������� ������ �������� ����. VREFINT: ����� 17 � JSQ3
SET_BIT (ADC->CCR, ADC_CCR_TSVREFE); // �������� ��������� �������������� ������� � �������� ����������
SET_BIT (ADC1->JSQR, (ADC_JSQR_JSQ4_4 | ADC_JSQR_JSQ4_1)); //�����. ������ ����-� ���������: ����� 18 � JSQ4
SET_BIT (ADC->CCR, ADC_CCR_VBATE); // �������� ��������� Vbat
SET_BIT (ADC1->CR1, ADC_CR1_JEOCIE); // �������� ���������� �� ��������� �������������� ��������������� �������
SET_BIT (ADC1->CR1, ADC_CR1_JAUTO); // �������� ������ �� ��������� �������������� ���������� �������
SET_BIT (ADC1->CR1, ADC_CR1_OVRIE); //// �������� ���������� ��� ����������
SET_BIT (ADC->CCR, ADC_CCR_ADCPRE);  //ADC prescaler. �������� ������� ������� ��� �� 8
//SET_BIT (ADC->CCR, ADC_CCR_ADCPRE_0); //ADC prescaler. �������� ������� ������� ��� �� 4
SET_BIT (ADC1->CR1, ADC_CR1_SCAN); // ������ ��� �� ������������ ��������� �������

// ��������� ������� �� ��������
SET_BIT (ADC1->CR2, ADC_CR2_JEXTEN_1); // ������� ����������� �� ���������� ������
SET_BIT (ADC1->CR2, ADC_CR2_JEXTSEL_1); // � �������� �������� ������ ������ "Timer 2 ��1 event"
//SET_BIT (ADC1->CR2, (ADC_CR2_JEXTSEL_1 | ADC_CR2_JEXTSEL_0)); // � �������� �������� ������ ������ "Timer 2 TRGO event"
SET_BIT (ADC1->CR2, ADC_CR2_ADON); // �������� ���

UART3_init(APB1_FREQUENCY, UART_BANDWITH); // ������������� UART3
UART3_PutRAMString("ADC injected channels with :\r\n");

// ��������� ������� TIM2
SET_BIT (RCC->APB1ENR, RCC_APB1ENR_TIM2EN); // �������� �������� ��������� TIM2
WRITE_REG(TIM2->PSC, 8000-1); // ������������
WRITE_REG(TIM2->ARR, 2000-1); // �������� ������������
SET_BIT (TIM2->CCMR1, (TIM_CCMR1_OC1M_2 | TIM_CCMR1_OC1M_1)); // ������� ����� ��� 1
WRITE_REG(TIM2->CCR1, LENGTH); // ������������ ���
SET_BIT (TIM2->CCER, TIM_CCER_CC1E); // �������� ����� ������� �� ����� ��������� �� ����� ��
SET_BIT (TIM2->CR1, TIM_CR1_CEN); // �������� ������ �������

NVIC_EnableIRQ(ADC_IRQn); // �������� ���������� ADC � NVIC

while(1)
	{
	ADC_DR_tmp_ptr = 0; // ��������� �� ������� � ������� ADC_DR_tmp
	SET_BIT (ADC1->CR2, ADC_CR2_SWSTART); // ����������� ������ ���
	while(!measure_good) {}; // ���� ��������� ��������������
	// ���������� ������
	UART3_PutRAMString ("ADC_regular_channels:\r\n"); // ������� ������ �� UART
	sprintf(msg,("ADC_result 1 = %f V\r\n"), Vref*ADC_DR_tmp[0]/4096); // ������� �������� ��� � ������
	UART3_PutRAMString(msg); // ������� ������ �� UART
	sprintf(msg,("ADC_result 2 = %f V\r\n"), Vref*ADC_DR_tmp[1]/4096); // ������� �������� ��� � ������
	UART3_PutRAMString(msg); // ������� ������ �� UART
	// ��������������� ������
	UART3_PutRAMString("ADC_injected_channels:\r\n"); // ������� ������ �� UART
	sprintf(msg,("ADC_Temp = %f C, ADC_Vref = %f, ADC_Vbat = %f V\r\n"), (((Vref*ADC1->JDR1/4096)-V25)/Avg_Slope+25.),
	(Vref*ADC1->JDR2/4096), (Vref*ADC1->JDR3/4096*2)); // ������� �������� ��� � ������
	UART3_PutRAMString (msg); // ������� ������ �� UART
	measure_good = 0; // ����: 1 - ��������� ������, 0 - ���

	// �������� ����������
	if(is_OVR)
		{
		UART3_PutRAMString ("ADC_SR_OVR\r\n"); // ������� ������ �� UART
		is_OVR = 0; // ����: 1 - ���� ���������� ����������, 0 - ���
		}
	delay_ms(1000);
	}
}

// ��������� ���������� �� ADC
void ADC_IRQHandler(void)
{
if(READ_BIT(ADC1->SR, ADC_SR_EOC)) // ���� ��������� ���������� �� ADC1 EOC
	{
	ADC_DR_tmp[ADC_DR_tmp_ptr++] = ADC1->DR; // ��������
	CLEAR_BIT (ADC1->SR, ADC_SR_EOC); // ������� ���� ���������
	}
if(READ_BIT(ADC1->SR, ADC_SR_JEOC)) // ���� ��������� ���������� �� ADC1 JEOC
	{
	measure_good = 1; // ����: 1 - ��������� ������, 0 - ���
	CLEAR_BIT (ADC1->SR, ADC_SR_JEOC); // ������� ���� ���������
	}
if(READ_BIT (ADC1->SR, ADC_SR_OVR)) // ���� ��������� ���������� �� ADC1 OVR - ���������� ��������
	{
	is_OVR = 1; // ����: 1 - ���� ���������� ����������, 0 - ���
	CLEAR_BIT (ADC1->SR, ADC_SR_OVR); // ������� ���� ���������
	}
}
