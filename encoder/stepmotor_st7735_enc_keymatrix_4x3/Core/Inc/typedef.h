#ifndef __TYPEDEF_H
#define __TYPEDEF_H

// Includes --------------------------------------------------------------------------//

// Exported types -------------------------------------------------------------------//
#pragma anon_unions(1)
#pragma pack(1)

//----------------------------------------------------------------------------------//
	typedef enum 
	{
		NO_KEY = 0,					// ��� ������, ������� �� ���� ������������ � ��������� ������: // 0 - �� ������
		KEY_s0_d0 = 0x0001,
		KEY_s0_d1 = 0x0002, 
		KEY_s0_d2 = 0x0004, 
		KEY_s1_d0 = 0x0008,
		KEY_s1_d1 = 0x0010,
		KEY_s1_d2 = 0x0020,
		KEY_s2_d0 = 0x0040,
		KEY_s2_d1 = 0x0080,
		KEY_s2_d2 = 0x0100,
		KEY_s3_d0 = 0x0200, 
		KEY_s3_d1 = 0x0400,
		KEY_s3_d2 = 0x0800 
	} KEY_CODE_t; 					//��� ������� ������

	typedef enum 
	{
		KEY_STATE_OFF = 0,				//������ - ������ �� ������
		KEY_STATE_BOUNCE, 				//������ -  ������� ������
		KEY_STATE_ON,							//������ - ������ ������
		KEY_STATE_AUTOREPEAT 			//������ - ����� �����������
	} KEY_STATE_t; 								//������ ������������ ����������


//Private defines ------------------------------------------------------------------//
#define KEY_BOUNCE_TIME 50 				// ����� �������� � ��
#define KEY_AUTOREPEAT_TIME 100 	// ����� ����������� � ��
//Constants ----------------------------------------------------------------------//

#endif