
#ifndef __MNP_MSG_H__
#define __MNP_MSG_H__

#ifdef __cplusplus
extern "C" {
#endif

// Includes ------------------------------------------------------------------------//
#include "typedef.h"

//Private defines ------------------------------------------------------------------//
#define SPECIAL_CMD_ID 1
#define SPECIAL_CMD_CODE_RESET 12 //����� �������� ������ ��������
#define CMD_CODE_CONFIG 2
#define CMD_CODE_SATTELITES_MASK 6

//Exported types -----------------------------------------------------------------//

//------------------------------------------------------------------
typedef enum
{
	MSG_3000      = 0x0BB8, //messages 3000
	MSG_3001      = 0x0BB9, //messages 3001
	MSG_3011      = 0x0BC3, //messages 3011
	MSG_3002      = 0x0BBA, //messages 3002
	MSG_3003      = 0x0BBB, //messages 3003
	MSG_3006      = 0x0BBE, //messages 3006
	MSG_2000      = 0x07D0, //messages 2000
	MSG_2200      = 0x0898  //messages 2200
} HEAD_MNP_MSG_ID_t;

//----------------------------������� � ����. �������----------------------------//
typedef struct 
{
	struct
	{
		union //��� ������� 3006
		{
	    struct 
			{
				uint16_t RAM		: 1;	//������� ������� � RAM ���������
				uint16_t dummy1	: 1;	//������
				uint16_t flash	: 1;	//������� � FLASH ������
				uint16_t dummy2	: 4;	//������
				uint16_t write	: 1; // 0 - ������ 1 - ������
				uint16_t code		: 8; // ��� ������� (0x2 - ����� 32-� ��������� ����������������)
			} cmd_3006;
			
		  struct 
			{
				uint16_t special_code	: 8; // ��� ����. ������� (0x0C - �����)
				uint16_t id						: 8; // ��� ����. ������ ������ ���� ����� (SPECIAL_CMD_ID = 0x1)
			} specialcmd_3006;
			
			uint8_t code_cmd [2]; //������ ��� ���������� ������ ���� �������
		};
		
		uint16_t	dummy; //2 ������ ����� (������)
		
	}command_code;
	
	union
	{
		struct // ������� CMD_CODE_CONFIG (��������� ������������)
		{		
			//������� 32 ��������� �����
			uint32_t      					: 32; //1 ����� - ������ (0x0)
			//������ 32 ��������� �����
			uint32_t protocol0			: 8; 	// �������� ������ ������0. 0x1-MNP binary, 0�8-NMEA-0183 (0x8)
			uint32_t dummy0     		: 14; //������ (0x0)
			uint32_t baud_divider0	: 10; //������������ UART0 = 460800/baudrate (0x60)
			//������ 32 ��������� �����
			uint32_t protocol1			: 8; // �������� ������ ������1 - MNP binary, 8 - NMEA-0183 (0x1)
			uint32_t dummy1       	: 14; //������ (0x0)
			uint32_t baud_divider1	: 10; //������������ UART1 = 460800/baudrate (0x4)
			//������ 32 ��������� �����
			uint32_t dummy2     		: 32;  //������ (0x0)
			//�������� 32 ��������� �����
			uint32_t dummy3       	: 2;  //������ (0x0)
			uint32_t tropo_corr			: 1; // ���������� ������������� ������ ���������� (0x1)
			uint32_t use_difc				: 1; // ���������� ������������� ���������������� �������� (0x1)
			uint32_t dummy4      					: 2; //������ (0x0)
			uint32_t dif_only				: 1; // �������������� ������������� ����������������� ������ (0x0) 
			uint32_t sol_smooth			: 1; // �������� ��������� ��� �������� ����� 1 �/c (0x0) 
			uint32_t sol_filter			: 1; // ����������� ������� (0x1)
			uint32_t meas_filter		: 1; // ����������� ���������� �� ���� � ���� ������� (0x1)
			uint32_t iono_corr			: 1; // ���������� ����������� ��������� (0x1)
			uint32_t disable_2D			: 1; // ������ ��������� ��������� (0x0)
			uint32_t use_RAIM				: 1; // ��������� ��������� RAIM (0x1)
			uint32_t enable_warm_startup	: 1; // ���������� �������� ��������� ������ (0x1)
			uint32_t true_PPS     	: 1; 			//
			uint32_t inst_velocity  : 1;
			// ����  sys_time � glo_time ���������� ��������  ��������� � ������ ��������� ����� �������:
			// glo_time | sys_time | ��������
			// --------------------------------
			//  0       |	0      | UTC(USNO)
			//  0       |	1      | GPS
			//  1       |	0      | UTC(SU)
			//  1       |	1      | �������
			uint32_t sys_time						: 1; // �������� ��������� ����� � ������� ������������� ������� (0x0)
			uint32_t glo_time						: 1; // ��������� ����� ������� / UTC(SU) (0x0)   
			uint32_t shift_meas					: 1; // �������� ��������� � ��������� ����� (0x1)
			uint32_t enable_SBAS				: 1; //���������� SBAS (0x0)
			uint32_t enable_iono_SBAS		: 1; //���������� ������ ��������� SBAS (0x0)
			uint32_t GPS_compatibility	: 1; //����� ������������� � ����������� GPS. � ���������� NMEA-0183 ������ ������������� �������� ������� $GP (0x0)
			uint32_t fake_PPS      			: 1; //
			uint32_t fake_M3						: 1; //
			uint32_t wr_alms 						: 1; // ���������� ���������� � flash ���������� (0x1)
			uint32_t wr_ephs   					: 1; // ���������� ���������� � flash �������� (0x1)
			uint32_t wr_ionoutc					: 1; // ���������� ���������� � flash ������ UTC GPS (0x1)
			uint32_t wr_coords					: 1; // ���������� ���������� � flash ��������� (0x1)
			uint32_t dummy6     							: 4;  //������ (0x1)
			//����� �����
			uint32_t enable_3000_1000_GGA_0	: 1; //���������� ����� 3000/1000/�GGA� �� ������ 0 (0x1)
			uint32_t enable_3011_1002_GSA_0	: 1; //���������� ����� 3011/1002/�GSA� �� ������ 0 (0x1)
			uint32_t enable_3002_1003_GSV_0	: 1; //���������� ����� 3002/1003/�GSV� �� ������ 0 (0x1)
			uint32_t enable_3003_1012_RMC_0	: 1; //���������� ����� 3003/1012/�RMC� �� ������ 0 (0x1)
			uint32_t dummy7     									: 4;  //������ (0x0)
			uint32_t enable_3000_1000_GGA_1	: 1; //���������� ����� 3000/1000/�GGA� �� ������ 1 (0x1)
			uint32_t enable_3011_1002_GSA_1	: 1; //���������� ����� 3011/1002/�GSA� �� ������ 1 (0x1)
			uint32_t enable_3002_1003_GSV_1	: 1; //���������� ����� 3002/1003/�GSV� �� ������ 1 (0x1)
			uint32_t enable_3003_1012_RMC_1	: 1; //���������� ����� 3003/1012/�RMC� �� ������ 1 (0x0)
			uint32_t dummy8     									: 20;  //������ (0x0)

		} config;

		struct // ������� CMD_CODE_SATTELITES_MASK (����� �� ���������), ��� 0�6
		{				
			uint32_t	GPS;
			uint32_t	GLONASS;
			/*uint32_t	SBAS; 
			uint32_t	Galileo;
			uint32_t	P_GPS;
			uint32_t	P_GLONASS;
			uint32_t	QZSS_GPS;
			uint32_t	QZSS_SBAS;
			uint32_t	BDS;*/
		} sat_mask;
		
		struct // ������� CMD_CODE_SATTELITES_MASK (����� �� ���������), ��� 0�6
		{				
			uint32_t	interval; //����� ��������� ��������� (2000=1c), ��� ������� 0�07
		} int_mask;
	
		
		struct //����� ������ (������������ � ����� ������� specialcmd_3006)
		{
			uint32_t rtc    				: 1; //����� RTC
			uint32_t coord    			: 1; // �������� ����������� ���������
			uint32_t ephs    				: 1; //�������� ��������
			uint32_t alm    				: 1; // �������� ���������
			uint32_t iono_model			: 1; //��������� ������ ���������/UTC GPS
			uint32_t dummy					: 27; //������
		} reset_mask;
	};

}MNP_MSG_3006_t;


//--------------------�������� ����� 3000 - ������������� �������--------------------//
typedef struct 
{
	struct 
	{
		double 				longitude;       // ������ D ���
		double 				latitude;        // ������� D ���
		double 				hor_height;	     // ������ D �
		double 				hor_vel;		 			// ������ �������� �������� D �/�
		double 				azimuth;		 		// ������ D ���
		double 				vert_vel;		 		// �������� ������� D �/�
		uint32_t      channels;		 		// ������ � �������
		uint32_t      diff_corr;       // ������� ���������������� ��������
		int32_t 			year;			 			// ��� DI ��� -
		int32_t 			month;					 // ����� DI ����� 1-12
		int32_t 			day;             // ���� DI ���� 1-31
		int32_t 			hour;			 			// ��� DI � 0-23
		int32_t 			minute;			 			// ������ DI ��� 0-59
		int32_t 			second;			 			// ������� DI � 0-60
		uint32_t 			msec;			 			// ���������� ����� ��������� UDI �� - ���� �������� ������� 0,5 ��
		float					Hz;              // ��������� ���������� F �� -
		float					gDOP;						 // GDOP F - -
		float					pDOP;			 			// PDOP F - -
		double	    	fil_lon;		 		// ������������� ������ D ��� -
		double		    fil_lat;		 		// ������������� ������� D ��� -
		double      	fil_height;      // ������������� ������ D � -
		double        fil_vel_module;	 // ������������� �������� (������) D �/� -
		double        fil_vel_az;		 // ������������� �������� (������) D ��� -
		double        fil_vel_vert;    // ������������� �������� (������) D �/� -
			
		struct  // ����� 
		{                     
			uint32_t two_dim_sol	: 1; // ���������� �������
			uint32_t fix_diff			: 1; // ������������� ������� ������� ����� GPS � �������
			uint32_t ellipsoid		: 2; // ��������� 0-WGS-84, 1-��-90, 2- �����������, 3 � ������������
			uint32_t solution_OK	: 1; // �������� �������
			uint32_t time_OK			: 1; // �������� ������� 
			uint32_t							: 4; // ������
			uint32_t system_coord	: 3; // ������� ��������� 0 - WGS-84, 1- ��-90, 2 - ��-42, 3 - ��-95, 4 - ������������ �������������, 5-7 ������
			uint32_t diff_mode		: 1; // ���������������� �����
			uint32_t 							: 2; // ������
			uint32_t							: 2; // ������
			uint32_t flags_ver		: 4; // ������ ������
		}flags;	

		uint32_t		ephemerides;       // ������� �������� 
		float				temperature;       // ����������� �C - ��� ������� ������������
		uint32_t		failed_meas;       // ������������� ���������. 1 � ��������� ����������� ���������� RAIM
	};	
}MNP_MSG_3000_t;

//--------------------������ ��������� ��������� ��������� ��������� MNP-binary--------------------//
typedef struct 
{
	uint16_t sync;
	union 
	{	
		struct 
		{
			uint8_t ID_B; //������� ����� ID ���� ���������
			uint8_t ID_A; //������� ����� ID ���� ���������
		};
		uint16_t msg_id;
	};	
	
	union 
	{	
		struct 
		{
			uint8_t length_B; //������� ����� ����� ���������
			uint8_t length_A; //������� ����� ����� ���������
		};
		uint16_t data_size; //����� �������� ����� ���������
	};	
	
		union 
	{	
		struct 
		{
			uint8_t  dummy_B; 
			uint8_t  dummy_A; 
		};
		uint16_t dummy; 
	};	
	
	
	union 
	{	
		struct 
		{
			uint8_t	chksum_B; //������� ����� ����������� �����
			uint8_t chksum_A; //������� ����� ����������� �����
		};
		uint16_t chksum;
	};
	
} HEAD_MNP_MSG_t;

//-------------------------������ ��������� ��������� ��������� MNP-binary-------------------------//
typedef struct 
{
	union
	{
		HEAD_MNP_MSG_t MNP_header;
		uint16_t header_words[5];
		uint8_t header_bytes[10];
	} msg_header;
	
	union
	{
		MNP_MSG_3000_t	msg3000;
		MNP_MSG_3006_t	msg3006;
		uint16_t raw_words[BUFFER_SIZE/2]; //������ ��� ���������� ������ ���� MNP-���������
		uint8_t raw_bytes[BUFFER_SIZE]; //������ ��� ���������� (2 �����) ������ ���� MNP-���������
	}	payload;
		
	enum 
	{
		__SYNC_BYTE1 = 1,
		__SYNC_BYTE2,
		__TYPE_ID_BYTE1,
		__TYPE_ID_BYTE2,
		__LENGTH_BYTE1,
		__LENGTH_BYTE2,
		__RESERVE_BYTE1,
		__RESERVE_BYTE2,
		__CK_HEAD_BYTE1,
		__CK_HEAD_BYTE2,
		__PAYLOAD,
		__PARSER
	}rx_state;	

	enum 
	{
		TYPE3000 = 1,
		TYPE3001,
		TYPE3011,
		TYPE3002,
		TYPE3003,
		TYPE3006,
		TYPE2200
	}mnp_msg_type;	
	
}MNP_MSG_t;

extern MKS2_t MKS2;
extern MNP_M7_CFG_t MNP_M7_CFG;

//Constants ----------------------------------------------------------------------//

//Private prototypes--------------------------------------------------------------//
void read_config_MNP ( MNP_MSG_t *);
void read_flash_MNP ( MNP_MSG_t *);
void put_msg2000 (MNP_MSG_t *);
void Set_GNSS_interval (MNP_MSG_t *, uint32_t );
void Get_GNSS_interval (MNP_MSG_t *, uint32_t );
void Read_SN (MNP_MSG_t *);
void MKS_context_ini (void);
void GPS_Init(void);
int8_t GPS_wait_data_Callback (void);
void GPS_Hard_Reset(void);
void GPS_Soft_Reset(void);
#ifdef __cplusplus
}
#endif

#endif /* __MNP_MSG_H__ */

