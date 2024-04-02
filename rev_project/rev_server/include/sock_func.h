/********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  sock_func.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(01/04/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "01/04/24 20:56:44"
 *                 
 ********************************************************************************/
#ifndef _SOCK_FUNC_H
#define _SOCK_FUNC_H

#define CONFIG_DEBUG
#ifdef  CONFIG_DEBUG
#define dbg_print(format,args...)	printf(format,##args)
#else
#define dbg_print(format,args...)	do{} while(0)
#endif

void set_socket_limit(void);
void print_usage(char* programe);
int socket_server_init(char* listenip,int listenport);

#endif
