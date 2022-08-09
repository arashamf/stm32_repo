/**
  ******************************************************************************
  * @file   fatfs.c
  * @brief  Code for fatfs applications
  ******************************************************************************
  * @attention
  *
  * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
  * All rights reserved.</center></h2>
  *
  * This software component is licensed by ST under Ultimate Liberty license
  * SLA0044, the "License"; You may not use this file except in compliance with
  * the License. You may obtain a copy of the License at:
  *                             www.st.com/SLA0044
  *
  ******************************************************************************
  */

#include "fatfs.h"

uint8_t retSD;    /* Return value for SD */
char SDPath[4];   /* SD logical drive path */
FATFS SDFatFS;    /* File system object for SD logical drive */
FIL SDFile;       /* File object for SD */

/* USER CODE BEGIN Variables */
extern char logSDPath;  // User logical drive path 
extern FIL logfile;     //файловый объект
extern FRESULT result; //код возврата функций FatFs

extern uint32_t byteswritten, bytesread; //счетчики записи/чтения файла
extern uint8_t rtext[TXT_BUFFER_SIZE]; //буффер считанных данных с SD карты
extern char wtext[TXT_BUFFER_SIZE];  //буффер записи на SD карту

/* USER CODE END Variables */

void MX_FATFS_Init(void)
{
  /*## FatFS: Link the SD driver ###########################*/
  retSD = FATFS_LinkDriver(&SD_Driver, SDPath);

  /* USER CODE BEGIN Init */
  /* additional user code for init */
  /* USER CODE END Init */
}

/**
  * @brief  Gets Time from RTC
  * @param  None
  * @retval Time in DWORD
  */
DWORD get_fattime(void)
{
  /* USER CODE BEGIN get_fattime */
  return 0;
  /* USER CODE END get_fattime */
}

/* USER CODE BEGIN Application */

/**********************************************************************************************************************/
void mount_card (FATFS* fs)
{
	
/*	if(FATFS_LinkDriver(&SD_Driver, &logSDPath) != 0)
	{
		sprintf (UART3_msg_TX,"LinkDriver error\r\n");
		UART3_SendString (UART3_msg_TX);
	}*/
	if(BSP_SD_Init() != MSD_OK)
	{
		sprintf (UART3_msg_TX,"bsp_sd_init error\r\n");
		UART3_SendString (UART3_msg_TX);
		return;
	}  
	if(f_mount (fs, (TCHAR const*)"/", 1) != FR_OK) //монтирование рабочей области для каждого тома (если 0 - отложенное монтирование, 1 - немедленное монтирование)  
//	if(f_mount (fs, (TCHAR const*)SDPath, 1) != FR_OK)
	{
		sprintf (UART3_msg_TX, "mount_error\r\n");
		UART3_SendString (UART3_msg_TX);
		return;
	}
}

/**********************************************************************************************************************/
uint32_t write_txt (FIL* fp, const char* buffer)
{
	uint32_t bytesize = 0; 
	unsigned long filesize = 0; //размер файла в байтах
	if ((filesize = f_size(fp)) > 0)
		f_lseek (fp, f_size(fp)); //Перемещение в конец файла для добавления новых данных
	result = f_write (fp, buffer, strlen(buffer), &bytesize); //bytesize - Количество записанных байт
	if((bytesize	!=	strlen(buffer))||(result	!=	FR_OK))
	{
		sprintf (UART3_msg_TX,"file_write_error, %u bytes write, code=%u\r\n", bytesize, result);
		UART3_SendString (UART3_msg_TX);
		return 0;
	}
	else
	{
		sprintf (UART3_msg_TX, "%u bytes write to file, filesize = %lu bytes\r\n", bytesize, filesize);
		UART3_SendString (UART3_msg_TX);
		return bytesize; //bytesize - количество записанных байт
	}
}

/**********************************************************************************************************************/
uint8_t read_txt (FIL* fp, uint32_t byteswritten)
{
	uint32_t bytesread = 0; char buffer [byteswritten+2];
	result = f_read (fp, buffer, byteswritten, &bytesread); //byteswritten-количество байт, которые необходимо прочитать.
	if((bytesread	!=	byteswritten)||(result	!=	FR_OK))
	{
		sprintf (UART3_msg_TX,"file_read_error, code=%u, byte read=%u\r\n", result, bytesread);
		UART3_SendString (UART3_msg_TX);
	}
	else
	{
		UART3_SendString (buffer);
	}
	return bytesread; //bytesread - количество прочитанных байт
}

/**********************************************************************************************************************/
void write_reg (FIL* fp, const char * txtbuf)
{

	if ((result = f_open (fp, "log.txt", FA_OPEN_ALWAYS|FA_WRITE)) != FR_OK) //Если файл существует, то он будет открыт, если же нет, то будет создан новый файл
	{	
		if ((result = f_open (fp, "log.txt", FA_CREATE_NEW|FA_WRITE)) != FR_OK)
		{
		sprintf (UART3_msg_TX,"file_for_write_is_not_open. errorcode=%u\r\n", result);
		UART3_SendString (UART3_msg_TX);
		return;
		}
	}
	byteswritten = write_txt (fp, txtbuf);	
	if ((result = f_close(fp)) != FR_OK)
	{
		sprintf (UART3_msg_TX,"incorrect_sync_file. errorcode=%u\r\n", result);
		UART3_SendString (UART3_msg_TX);
	}
}

/* USER CODE END Application */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/
