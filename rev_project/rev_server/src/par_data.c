/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  par_data.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(01/04/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "01/04/24 21:12:39"
 *                 
 ********************************************************************************/

#include <stdio.h>
#include "par_data.h"

int str_sscanf(char* data_buf,char* id,float* temp,char* local_t)
{
	int		n=-1;
	float	temperature;
	n=sscanf(data_buf,"%s %f %s",id,&temperature,local_t);
	*temp=temperature;
	return n;
}
