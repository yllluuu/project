/********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  ds18b20_get_temp.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(31/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "31/03/24 16:18:15"
 *                 
 ********************************************************************************/

#ifndef _DS18B20_GET_TEMP_
#define _DS18B20_GET_TEMP_

#define CONFIG_DEBUG
#ifdef  CONFIG_DEBUG
#define dbg_print(format,args...)	printf(format,##args)
#else
#define dbg_print(format,args...)	do{} while(0)
#endif

int get_temperature(float *temp);

#endif
