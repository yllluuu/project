/********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  get_func.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(01/04/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "01/04/24 13:27:47"
 *                 
 ********************************************************************************/

#ifndef	_GET_FUNC_H
#define _GET_FUNC_H

#define CONFIG_DEBUG
#ifdef  CONFIG_DEBUG
#define dbg_print(format,args...)	printf(format,##args)
#else
#define dbg_print(format,args...)	do{} while(0)
#endif

void print_usage(char* programe);
int get_dev(char* ID,int len);
int get_tm(char* localt);

#endif
