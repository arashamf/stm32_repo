
// Includes ------------------------------------------------------------------//
#include "stanok_math.h"
#include "typedef.h"
#include <math.h>

// Functions -----------------------------------------------------------------//

// Variables -----------------------------------------------------------------//

//---------------------------------------------------------------------------------------------------//
uint32_t calc_steps_mode1 (angular_data_t * handle, float step_unit)
{
		uint32_t need_step = 0;
		float buffer = 0;
	
		handle->mode1_error += ((remainderf(handle->StepAngleInSec, step_unit))/step_unit); //������ �������
		buffer = rintf((handle->StepAngleInSec)/step_unit); //���������� �� ������ ����������� �������� ���� float
		need_step = (uint32_t)(REDUCER*buffer);	
		if (handle->mode1_error > 1.0) //���� ���������� ����������� ������ 1
		{	
			handle->mode1_error -= 1.0;	
			need_step += REDUCER;
		}
		else
		{
			if (handle->mode1_error < (-1.0))  //���� ���������� ����������� ������ 1
			{	
				handle->mode1_error += 1.0;	
				need_step -= REDUCER;
			}
		}
		return need_step;
}

//---------------------------------------------------------------------------------------------------//
uint32_t calc_steps_milling (milling_data_t * handle, float step_unit)
{
		uint32_t need_step = 0;
		float buffer = 0;
	
		handle->milling_error += ((remainderf(handle->AngleTeethInSec, step_unit))/step_unit); //������ �������
		buffer = rintf((handle->AngleTeethInSec)/step_unit); //���������� �� ������ ����������� �������� ���� float
		need_step = (uint32_t)(REDUCER*buffer);	
		if (handle->milling_error > 1.0) //���� ���������� ����������� ������ 1
		{	
			handle->milling_error -= 1.0;	
			need_step += REDUCER;
		}
		else
		{
			if (handle->milling_error < (-1.0))  //���� ���������� ����������� ������ 1
			{	
				handle->milling_error += 1.0;	
				need_step -= REDUCER;
			}
		}
		return need_step;
}

//---------------------------------------------------------------------------------------------------//
uint32_t steps_for_back_to_zero (uint32_t Second, float step_unit)
{
		uint32_t need_step = 0;

		need_step = (uint32_t)(REDUCER*(Second/step_unit));	
		return need_step;
}