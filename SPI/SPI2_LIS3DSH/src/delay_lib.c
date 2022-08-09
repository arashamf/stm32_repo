/*
 * delay_lib.c
 *
 *  Created on: 17 ����. 2020 �.
 *      Author: ALEXEY
 */

#include "delay_lib.h"
#include "stm32f4xx.h"
#define stm32f4xx

void ini_TIMER5 ()
{
	RCC_APB1PeriphClockCmd (RCC_APB1Periph_TIM5, ENABLE);  //�������� ������������
	TIM_TimeBaseInitTypeDef TIM5_ini;
	TIM5_ini.TIM_Prescaler = 48000 - 1; //������������
	TIM5_ini.TIM_CounterMode = TIM_CounterMode_Up; //���� �� ����������
	TIM5_ini.TIM_Period = 1000 - 1;  //�������� ������������
	TIM5_ini.TIM_ClockDivision = TIM_CKD_DIV1; //���������� ��������� ������� ����� �������� ������� (CK_INT) � �������� �������� �������������, ������������ ��������� ��������� (ETR, TIx). 00: tDTS = tCK_INT
	TIM5_ini.TIM_RepetitionCounter = 0x000;//���� ������� ����� N>0, �� ���������� ����� �������������� �� ��� ������ ������������ ��������, � �� ������ N ������������. � ������ ��� (N+1) �������������:
	 	 	 	 	 	 	 	 	 	//���������� �������� ��� � ������ ������������ �� ����� ��� ���������� ������������ ��� � ������ �������������;
	TIM_TimeBaseInit(TIM5, &TIM5_ini);
	TIM_ITConfig(TIM5, TIM_IT_Update, ENABLE);  //�������� ��������� �� ���������� ������� 5
	NVIC_EnableIRQ(TIM5_IRQn); //�������� ���������� �� ������� 5 � ����������� ���������1

	TIM_Cmd(TIM5, ENABLE); //������� ������ 5
}

void delay_us(uint32_t us)
{
	if (us <= 1)
			return;
	RCC_ClocksTypeDef RCC_Clocks; //������� ���������
	RCC_GetClocksFreq (&RCC_Clocks); //���������� ������� SYSCLK, HCLK, PCLK1 � PCLK2
	TIM_DeInit(TIM5); //������������� � �������� TIM5 �������� ������ �� ���������
	RCC_APB1PeriphClockCmd (RCC_APB1Periph_TIM5, ENABLE);
	TIM_TimeBaseInitTypeDef TIM5_ini;
	TIM5_ini.TIM_Prescaler =  (2*RCC_Clocks.PCLK1_Frequency/1000000) - 1; //������������
	TIM5_ini.TIM_CounterMode = TIM_CounterMode_Up; //���� �� ����������
	TIM5_ini.TIM_Period =  us-1;  //�������� ������������
	TIM5_ini.TIM_ClockDivision = TIM_CKD_DIV1; //���������� ��������� ������� ����� �������� ������� (CK_INT) � �������� �������� �������������, ������������ ��������� ��������� (ETR, TIx). 00: tDTS = tCK_INT
	TIM5_ini.TIM_RepetitionCounter = 0x000;//���� ������� ����� N>0, �� ���������� ����� �������������� �� ��� ������ ������������ ��������, � �� ������ N ������������.
	TIM_TimeBaseInit(TIM5, &TIM5_ini);
	
	TIM_ClearFlag(TIM5, TIM_FLAG_Update); // ������� ���� ����������
	TIM_Cmd(TIM5, ENABLE); //������� ������ 5
	while (!(TIM_GetFlagStatus(TIM5, TIM_FLAG_Update))) {} //��� ��������� ����� ���������� ������� 5
	TIM_Cmd(TIM5, DISABLE);  //��������� ������ 5
	TIM_ClearFlag(TIM5, TIM_FLAG_Update); // ������� ���� ����������
}

void delay_ms(uint32_t ms)
{
	if (ms <= 1)
		return;
	RCC_ClocksTypeDef RCC_Clocks;  //������� ���������
	RCC_GetClocksFreq (&RCC_Clocks); //���������� ������� SYSCLK, HCLK, PCLK1 � PCLK2
	TIM_DeInit(TIM5); //������������� � �������� TIM5 �������� ������ �� ���������
	RCC_APB1PeriphClockCmd (RCC_APB1Periph_TIM5, ENABLE);
	TIM_TimeBaseInitTypeDef TIM5_ini;
	TIM5_ini.TIM_Prescaler = (2*RCC_Clocks.PCLK1_Frequency/1000) - 1; //������������
	TIM5_ini.TIM_CounterMode = TIM_CounterMode_Up; //���� �� ����������
	TIM5_ini.TIM_Period = ms-1;  //�������� ������������
	TIM5_ini.TIM_ClockDivision = TIM_CKD_DIV1; //���������� ��������� ������� ����� �������� ������� (CK_INT) � �������� �������� �������������, ������������ ��������� ��������� (ETR, TIx). 00: tDTS = tCK_INT
	TIM5_ini.TIM_RepetitionCounter = 0x000;//���� ������� ����� N>0, �� ���������� ����� �������������� �� ��� ������ ������������ ��������, � �� ������ N ������������.
	TIM_TimeBaseInit(TIM5, &TIM5_ini);
	
	TIM_ClearFlag(TIM5, TIM_FLAG_Update); // ������� ���� ����������
	TIM_Cmd(TIM5, ENABLE); //������� ������ 5
	while (!(TIM_GetFlagStatus(TIM5, TIM_FLAG_Update))) {} //��� ��������� ����� ���������� ������� 5
	TIM_Cmd(TIM5, DISABLE);  //��������� ������ 5
	TIM_ClearFlag(TIM5, TIM_FLAG_Update); // ������� ���� ����������
}
