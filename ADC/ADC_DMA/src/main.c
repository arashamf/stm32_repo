//���������� ������. ������������� DMA. ������ �� �������.
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

// ������ ��� ��������
#define HalfWords_NUM 20 // ����������� ������� ��������
uint16_t HalfWords[HalfWords_NUM]; // ������ ��������


//���
#define LENGTH 200

// ���
float Vref = 3.3; // ������� ���������� ��� ���
float ADC_float; // ��������� ��� � ���� ����������

// DMA2 - �������������
//
void DMA2_init(void)
{
SET_BIT (RCC->AHB1ENR, RCC_AHB1ENR_DMA2EN); // �������� �������� ��������� DMA2
// ��������� DMA ������������ ��� ���������� ���� "EN"
WRITE_REG (DMA2_Stream4->PAR, ((uint32_t)(&ADC1->DR))); // ����� ������������� ���������� (������� ������ DR)
WRITE_REG (DMA2_Stream4->M0AR, ((uint32_t)(&HalfWords))); // ����� ������ ������
WRITE_REG (DMA2_Stream4->NDTR, HalfWords_NUM); // ������� ����� ���� ��� ��������
CLEAR_BIT (DMA2_Stream4->CR, DMA_SxCR_CHSEL); //������� ����� 0
CLEAR_BIT (DMA2_Stream4->CR, DMA_SxCR_PFCTRL); //DMA �������� ����������� ������
SET_BIT (DMA2_Stream4->CR, DMA_SxCR_PL); //��������� ������ - very high
CLEAR_BIT (DMA2_Stream4->CR, DMA_SxCR_MBURST); //������������ �������� ������� ������; ��������� ��������
CLEAR_BIT (DMA2_Stream4->CR, DMA_SxCR_PBURST); //������������ �������� ������������ �������; ��������� ��������
CLEAR_BIT (DMA2_Stream4->CR, DMA_SxCR_DBM); //����� �������� �������; ��� ������������ ������ � ����� ��������
SET_BIT (DMA2_Stream4->CR, DMA_SxCR_PSIZE_0); //������ ������������ ������; ��������� (16 ���)
SET_BIT (DMA2_Stream4->CR, DMA_SxCR_MSIZE_0); //������ ������ ������; ��������� (16 ���)
CLEAR_BIT (DMA2_Stream4->CR, DMA_SxCR_PINC); //0 - ����� ������������� ���������� �� ����������
SET_BIT (DMA2_Stream4->CR, DMA_SxCR_MINC); //1 - ��������� ������ ������ ������������� ����� ������ �������� ������ �� �������� ��������� � MSIZE
CLEAR_BIT (DMA2_Stream4->CR, DMA_SxCR_CIRC);  //�������� ����� - ��������
CLEAR_BIT (DMA2_Stream4->CR, DMA_SxCR_TCIE);  //0 - ���������� �� ���������� �������� ���������
CLEAR_BIT (DMA2_Stream4->CR, DMA_SxCR_HTIE); //0 - ���������� ����� �������� �������� ���������
CLEAR_BIT (DMA2_Stream4->CR, DMA_SxCR_TEIE); //0 - ���������� ����� ������ �� ����� �������� ���������
CLEAR_BIT (DMA2_Stream4->CR, DMA_SxCR_DMEIE); // 0 - ���������� ����� ������ � Direct mode
CLEAR_BIT (DMA2_Stream4->CR, DMA_SxCR_DIR); // �������� �� ������������� ���������� � ������
//DMA2->LIFCR - ������� ������ ������ ��������� ���������� DMA-������� �� 0 �� 3, ��� CTCIF0 ���������� ���� TCIF0
//SET_BIT (DMA2->LIFCR, DMA_LIFCR_CTCIF0); // ������� ���� ���������� ���������� �������� ������ 0
SET_BIT (DMA2->HIFCR, DMA_HIFCR_CTCIF4); // ������� ���� ���������� ���������� �������� ������ 4
//SET_BIT (DMA2_Stream4->CR, DMA_SxCR_EN); // �������� ������ DMA
//UART3_PutRAMString("DMA_completed\r\n");
}

int main()
{
// ��������� ������ ���
SET_BIT (RCC->AHB1ENR, RCC_AHB1ENR_GPIOCEN); // ��������� �������� ��������� GPIOC
SET_BIT (GPIOC->MODER, GPIO_MODER_MODER2_0 | GPIO_MODER_MODER2_1); // ����������� ����� PC2 �� ���������� �����
SET_BIT (GPIOC->MODER, GPIO_MODER_MODER1_0 | GPIO_MODER_MODER1_1); // ����������� ����� PC1 �� ���������� �����

// ��������� ���
SET_BIT (RCC->APB2ENR, RCC_APB2ENR_ADC1EN); // �������� �������� ���������;
// ���������� ������:
CLEAR_BIT (ADC1->CR1, (ADC_CR1_RES_0 | ADC_CR1_RES_1)); //12 ��� ���
CLEAR_BIT (ADC1->CR2, ADC_CR2_ALIGN); //������ ������������ �����
SET_BIT (ADC1->SQR1, ADC_SQR1_L_0); // �������������� 2-� �������
SET_BIT (ADC1->SQR3, (ADC_SQR3_SQ1_0 | ADC_SQR3_SQ1_1 | ADC_SQR3_SQ1_3)); // ����� 11 (PC1)
SET_BIT (ADC1->SQR3, (ADC_SQR3_SQ2_2 | ADC_SQR3_SQ2_3)); // ����� 12 (PC2)
//SET_BIT (ADC1->CR2, ADC_CR2_EOCS); //���� ���������� ��� EOCS: ��� EOC ��������������� � ����� ������� ����������� ��������������. �������� ����������� ������������.
SET_BIT (ADC1->CR1, ADC_CR1_EOCIE); //�������� ���������� �� ��������� ��������������
SET_BIT (ADC1->CR1, ADC_CR1_OVRIE); //// �������� ���������� ��� ����������
SET_BIT (ADC->CCR, ADC_CCR_ADCPRE);  //ADC prescaler. �������� ������� ������� ��� �� 8
SET_BIT (ADC1->CR1, ADC_CR1_SCAN); // ������ ��� �� ������������ ��������� �������
SET_BIT (ADC1->CR2, ADC_CR2_DMA); // �������� ������ DMA
// ��������� ������� �� ��������
SET_BIT (ADC1->CR2, ADC_CR2_EXTEN_1); // ������� ����������� �� ���������� ������
SET_BIT (ADC1->CR2, (ADC_CR2_EXTSEL_2 | ADC_CR2_EXTSEL_1)); // � �������� �������� ������ ������ "Timer 2 TRGO event"


//UART3_PutRAMString("ADC with DMA:\r\n");
SET_BIT (ADC1->CR2, ADC_CR2_ADON); // �������� ���
UART3_init(APB1_FREQUENCY, UART_BANDWITH); // ������������� UART3
UART3_PutRAMString("ADC with DMA:\r\n");

// ��������� ������� TIM2
SET_BIT (RCC->APB1ENR, RCC_APB1ENR_TIM2EN); // �������� �������� ��������� TIM2
WRITE_REG(TIM2->PSC, 8000-1); // ������������
WRITE_REG(TIM2->ARR, 2000-1); // �������� ������������
SET_BIT (TIM2->CCER, TIM_CCER_CC1E); // �������� ����� ������� �� ����� ��������� �� ����� ��*/
SET_BIT (TIM2->CR2, TIM_CR2_MMS_1); // � �������� TRGO ����� �������������� ������� "Update"
SET_BIT (TIM2->CR1, TIM_CR1_CEN); // �������� ������ �������

DMA2_init(); // ������������� DMA2
NVIC_EnableIRQ(ADC_IRQn); // �������� ���������� ADC � NVIC
//delay_ms (1000);

SET_BIT (DMA2_Stream4->CR, DMA_SxCR_EN); // �������� ������ DMA
while (!(READ_BIT(DMA2->HISR, DMA_HISR_TCIF4))) //���� ���� ���������� ���������� �������� Stream4 �� ����������
//while (!(READ_BIT(DMA2->LISR, DMA_LISR_TCIF0))) //���� ���� ���������� ���������� �������� Stream0 �� ����������
	{
	if(is_OVR) // �������� ����������
		{
		UART3_PutRAMString ("ADC_SR_OVR\r\n"); // ������� ������ �� UART
		is_OVR = 0; // ����: 1 - ���� ���������� ����������, 0 - ���
		}
	} // ������� ��������� ����� ��������� ��������
sprintf(msg,"ADC1_DMA2_test_programm_end\r\n");
UART3_PutRAMString (msg); // ����
for(byte i = 0; i < HalfWords_NUM; i++)
	{
	sprintf(msg,"i = %u, ", i);
	UART3_PutRAMString (msg); // <a>
	sprintf(msg,"HalfWords[%u] = %5.2f, ", i, (Vref*HalfWords[i]/4096));
	UART3_PutRAMString (msg); //����
	i++;
	sprintf(msg,"HalfWords[%u] = %5.2f\r\n", i, (Vref*HalfWords[i]/4096));
	UART3_PutRAMString (msg); //����
	}

while(1)
	{

	}
}

// ��������� ���������� �� ADC
void ADC_IRQHandler(void)
{
if(READ_BIT(ADC1->SR, ADC_SR_EOC)) // ���� ��������� ���������� �� ADC1 EOC
	{
//	ADC_DR_tmp[ADC_DR_tmp_ptr++] = ADC1->DR; // ��������
	CLEAR_BIT (ADC1->SR, ADC_SR_EOC); // ������� ���� ����������
	}
if(READ_BIT (ADC1->SR, ADC_SR_OVR)) // ���� ��������� ���������� �� ADC1 OVR - ���������� ��������
	{
	is_OVR = 1; // ����: 1 - ���� ���������� ����������, 0 - ���
	CLEAR_BIT (ADC1->SR, ADC_SR_OVR); // ������� ���� ���������
	}
}

