
#ifndef __BUTTON_H__
#define __BUTTON_H__

#ifdef __cplusplus
extern "C" {
#endif

// Includes --------------------------------------------------------------------------//
#include "main.h"

// Exported types -------------------------------------------------------------------//
	//��� ������� ������
	typedef enum 
	{
		NO_KEY 						= 	0x00,			//������ �� ������	
		KEY_LEFT 					= 	0x01,			//����� ������
		KEY_CENTER_SHORT 	= 	0x02,			//�������� ������� ����������� ������
		KEY_CENTER_LONG 	= 	0x03,			//������� ������� ����������� ������
		KEY_RIGHT					= 	0x04, 		//������ ������
		KEY_ENC_SHORT			= 	0x05,			//�������� ������� ������ ��������
		KEY_ENC_LONG			=		0x06,			//������� ������� ������ ��������
		KEY_MODE_SHORT		=		0x07,			//�������� ������� ������ ������
		KEY_MODE_LONG			=		0x08,			//�������� ������� ������ ������
	} KEY_CODE_t; 					

	//----------------------------------------------------------------------------------//
	typedef enum 
	{
		KEY_STATE_OFF 			= 0,			//����� - ������ �� ������
		KEY_STATE_ON					,				//����� - ������ ������
		KEY_STATE_BOUNCE			, 			//����� -  ������� ������
		KEY_STATE_AUTOREPEAT		 			//����� - ����� �������� (�����������) ������� ������
	} KEY_STATE_t; 									//������ ������������ ����������

	//----------------------------------------------------------------------------------//
	struct KEY_MACHINE_t
	{
		KEY_CODE_t 		key_code;
		KEY_STATE_t 	key_state;
	} ;

//Private defines ------------------------------------------------------------------//

// Prototypes ---------------------------------------------------------------------//
uint16_t scan_keys (void);

#ifdef __cplusplus
}
#endif

#endif /* __BUTTON_H__ */
