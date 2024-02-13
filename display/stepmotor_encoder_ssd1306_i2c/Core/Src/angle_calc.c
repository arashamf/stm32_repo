
// Includes ---------------------------------------------------------------------------------------//
#include "angle_calc.h"

// Functions --------------------------------------------------------------------------------------//

//----------------------------���������� ������ ��������� ������ 1 � ������ EEPROM----------------------------//
void angle_to_EEPROMbuf (angular_data_t * handle, uint8_t * EEPROM_buffer)
{	
	//�������� ������� ������ ��������� �������� ���� �� EEPROM
	*(EEPROM_buffer+0) = (uint8_t)(handle->StepAngleInSec >> 24); 
	*(EEPROM_buffer+1) = (uint8_t)(handle->StepAngleInSec >> 16); 
	*(EEPROM_buffer+2) = (uint8_t)(handle->StepAngleInSec >> 8); 
	*(EEPROM_buffer+3) = (uint8_t)(handle->StepAngleInSec >> 0); 
	//�������� ������� ������ �������� ��������� ���� �� EEPROM
	*(EEPROM_buffer+4) = (uint8_t)(handle->ShaftAngleInSec >> 24); 
	*(EEPROM_buffer+5) = (uint8_t)(handle->ShaftAngleInSec >> 16); 
	*(EEPROM_buffer+6) = (uint8_t)(handle->ShaftAngleInSec >> 8); 
	*(EEPROM_buffer+7) = (uint8_t)(handle->ShaftAngleInSec >> 0);
}

//----------------------------��������� ������ ��������� ������ 1 �� ������� EEPROM----------------------------//
void angle_from_EEPROMbuf (angular_data_t * handle, uint8_t * EEPROM_buffer)
{
	handle->StepAngleInSec = (uint32_t)(((*(EEPROM_buffer+0))<<24) | ((*(EEPROM_buffer+1))<<16) | ((*(EEPROM_buffer+2))<<8) | ((*(EEPROM_buffer+3))<<0));
	handle->ShaftAngleInSec = (uint32_t)(((*(EEPROM_buffer+4))<<24) | ((*(EEPROM_buffer+5))<<16) | ((*(EEPROM_buffer+6))<<8) | ((*(EEPROM_buffer+7))<<0));
}

//------------------------������� ���� ���� ���� ���� �� ������� ��/���/� � �������------------------------//
void SetAngle_in_Seconds (angular_data_t * handle)
{
	handle->StepAngleInSec=0;
	handle->StepAngleInSec += handle->set_second;
	handle->StepAngleInSec += handle->set_minute*SECOND_PER_MINUTE ;
	handle->StepAngleInSec += handle->set_degree*SECOND_PER_DEGREE;
}

//-------------------------������� ���� ���� ���� ���� �� ������ � ������ ��/���/�-------------------------//
void GetSetAngle_from_Seconds (angular_data_t * handle)
{
	uint32_t tmp = 0;
	handle->set_degree = handle->StepAngleInSec/ SECOND_PER_DEGREE; //���������� ��������
	tmp = handle->StepAngleInSec % SECOND_PER_DEGREE; //������� � �������� � ���������
	handle->set_minute = tmp/SECOND_PER_MINUTE;  //���������� �����
	handle->set_second = tmp % SECOND_PER_MINUTE; //���������� ������
}

//-------------------������� ���� �������� ��������� ���� �� ������� ��/���/� � �������-------------------//
void ShaftAngle_in_Seconds (angular_data_t * handle)
{
	handle->ShaftAngleInSec = 0;
	handle->ShaftAngleInSec += handle->shaft_second;
	handle->ShaftAngleInSec += handle->shaft_minute*SECOND_PER_MINUTE;
	handle->ShaftAngleInSec += handle->shaft_degree*SECOND_PER_DEGREE;
}

//-------------------������� ���� �������� ��������� ���� �� ������ � ������ ��/���/�-------------------//
void GetAngleShaft_from_Seconds (angular_data_t * handle)
{
	uint32_t tmp = 0;
	handle->shaft_degree = handle->ShaftAngleInSec/(SECOND_PER_DEGREE); //���������� ��������
	tmp = handle->ShaftAngleInSec % SECOND_PER_DEGREE; //������� � �������� � ���������
	handle->shaft_minute = tmp/SECOND_PER_MINUTE; //���������� �����
	handle->shaft_second = tmp % SECOND_PER_MINUTE; //���������� ������
}

//-------------------------------��������� ���� �������� ��������� ���� -------------------------------//
void AngleShaftReset (angular_data_t * handle)
{
	handle->shaft_second = 0; 
	handle->shaft_minute = 0; 
	handle->shaft_degree = 0;
	handle->ShaftAngleInSec = 0; //������� ��������� ���� - �������
}

//---------------------------����� ���� ���� ���� ���� �� ����������� ��������---------------------------//
void SetAngleReset (angular_data_t * handle)
{
	handle->set_second = 0;
	handle->set_minute = 1; 
	handle->set_degree = 0;
	SetAngle_in_Seconds (handle);
}

//-----------------������� ���� �������� � ������ ���������� �� ������� ��/���/� � �������-----------------//
void MilAngleTeeth_in_Seconds (milling_data_t * handle)
{
	handle->AngleTeethInSec = 0;
	handle->AngleTeethInSec += handle->step_shaft_second;
	handle->AngleTeethInSec += handle->step_shaft_minute*SECOND_PER_MINUTE;
	handle->AngleTeethInSec += handle->step_shaft_degree*SECOND_PER_DEGREE;
}

//------------------������� ���� �������� � ������ ���������� �� ������ � ������ ��/���/�------------------//
void MilAngleTeeth_from_Seconds (milling_data_t * handle)
{
	uint32_t tmp = 0;
	handle->step_shaft_degree = handle->AngleTeethInSec/SECOND_PER_DEGREE; //���������� ��������
	tmp = handle->AngleTeethInSec % SECOND_PER_DEGREE; //������� � �������� � ���������
	handle->step_shaft_minute = tmp/SECOND_PER_MINUTE; //���������� �����
	handle->step_shaft_second = tmp % SECOND_PER_MINUTE; //���������� ������
}

//---------------------------������ ���� �������� ����� ����� ���������� �����---------------------------//
void GetMilAngleTeeth (milling_data_t * handle)
{	
	uint32_t tmp = 0;
	handle->AngleTeethInSec = CIRCLE_IN_SEC/handle->teeth_gear_numbers; //���� ����� �������
	MilAngleTeeth_from_Seconds (handle); //������� ���� �������� �� ������ � ������ ��/���/�
}

//---------------------����� ������� ������ ������ ���������� �� ����������� ��������---------------------//
void MilAngleTeethReset (milling_data_t * handle, STATUS_FLAG_t * status_flag)
{	
	handle->teeth_gear_numbers 	= 2;
	handle->remain_teeth_gear 	= handle->teeth_gear_numbers;
	handle->step_shaft_degree 	= 0; 
	handle->step_shaft_minute 	= 0; 
	handle->step_shaft_second 	= 0;
	handle->AngleTeethInSec 		= 0;
	MilingFlagReset (handle, status_flag);
}

//------------------------------------------------------------------------------------------------//
void RemainTeethReset (milling_data_t * handle)
{
	handle->remain_teeth_gear = handle->teeth_gear_numbers;
}

//------------------------------------------------------------------------------------------------//
void MilingFlagReset (milling_data_t * handle, STATUS_FLAG_t * status_flag)
{
	status_flag->right_flag 		= OFF;
	status_flag->left_flag 			= OFF;
	handle->milling_error = 0;
}

//------------------------------------------------------------------------------------------------//
void teeth_angle_to_EEPROMbuf (milling_data_t * handle, uint8_t * EEPROM_buffer, STATUS_FLAG_t * status_flag)
{
	//�������� ������� ������ ��������� �������� ���� �� EEPROM
	*(EEPROM_buffer+8) = (uint8_t)(handle->remain_teeth_gear);
	*(EEPROM_buffer+9) = (int8_t)(handle->teeth_gear_numbers); 
	*(EEPROM_buffer+10) = (uint8_t)(status_flag->flag); 		
}

//------------------------------------------------------------------------------------------------//
void teeth_angle_from_EEPROMbuf (milling_data_t * handle, uint8_t * EEPROM_buffer, STATUS_FLAG_t * status_flag)
{
	handle->remain_teeth_gear = (uint8_t)(*(EEPROM_buffer+8)); //���������� ���������� ���������� ������
	handle->teeth_gear_numbers = (uint8_t)(*(EEPROM_buffer+9)); //���������� �������������� ���������� ������
	status_flag->flag = (uint8_t)(*(EEPROM_buffer+10));
}

//------------------------------------------------------------------------------------------------//
void remain_teeth_to_EEPROMbuf (milling_data_t * handle, uint8_t * EEPROM_buffer)
{
	*(EEPROM_buffer+8) = (uint8_t)(handle->remain_teeth_gear); //���������� ���������� ���������� ������
}

//------------------------------------------------------------------------------------------------//
