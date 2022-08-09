/*
 * readhex.c
 * 
 * Copyright 2021 Пользователь <Пользователь@DESKTOP-49E48BL>
 * 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 * 
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston,
 * MA 02110-1301, USA.
 * 
 * 
 */


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef unsigned char uint8_t;

int main()
{
	FILE *fp1; char nameoffile1 [] = "f103_flash.hex";//указатель на файл
	char buffer1 [48];// char buffer2 [48]; //char buffer3 [48];//char * ptr = buffer;
	uint8_t id = 0, length = 0; unsigned long ext_adress = 0; unsigned int adress = 0;
	
	if ((fp1 = fopen (nameoffile1, "r")) == NULL) //функция fopen возвращает указатель на файл или NULL, если не удалось открыть файл. 
	{					//1й аргумент адрес строки, содержащий имя файла, 2й - аргумент, определяющий режим открытия файла
	printf ("Не удаётся открыть %s", nameoffile1); //"r" - открыть тектовый файл только для чтения. 
	exit (1);  //функция закрывает программу, аргумент передаётся ОС, 0 при корректном завершение, 1 - при аварийном.
	}
	
	while (fgets (buffer1, 48, fp1) != NULL) 
	{
	printf ("%s", buffer1);
	length = 0x2 * ((buffer1 [1] - 48)*16 + (buffer1 [2] - 48));
	id = buffer1 [8] - 48;	
	if (id == 4)
		{
		for (uint8_t count = 0; count < length; count++)	
			{
			unsigned long mnogitel = 1;
			for (uint8_t i = (length-count-1); i > 0; i--)
				mnogitel *= 16;			
			adress += (buffer1 [count+9] - 0x30) * mnogitel;
			printf ("%d buffer2 = %x\n",adress, (buffer1 [count+9] - 0x30));
			}
		printf ("id = %x,  length = %x, adress = %x\n\n", id, length, adress);
		}
	if (!id)
		{
			ext_adress = (unsigned long)(adress << 16);
		for (uint8_t count = 0; count < 4; count++)	
			{
			unsigned long mnogitel = 1;
			for (uint8_t i = 3 - count; i > 0; i--)
				mnogitel *= 16;	
			ext_adress += (buffer1 [count+3] - 0x30) * mnogitel;
			}
		printf ("id = %x,  length = %x, ext_adress = %lx\n\n", id, length, ext_adress);
		}
	}
	
	fclose (fp1); //ф-я fclose закрывает файл, очищая при этом буферы. Ф-я возвращает 0 в случае успешного закрытия или EOF в противном случае
	
	return 0;
}

