/********************************************************************************
 *      Copyright:  (C) 2024 yanglu
 *                  All rights reserved.
 *
 *       Filename:  main.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(02/04/24)
 *         Author:  LiYi <1751425323@qq.com>
 *      ChangeLog:  1, Release initial version on "02/04/24 19:22:03"
 *                 
 ********************************************************************************/
#ifndef _MAIN_H_
#define _MAIN_H_

#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<unistd.h>
#include<stdlib.h>
#include<getopt.h>
#include<ctype.h>
#include<libgen.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<sys/epoll.h>
#include<sys/resource.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sqlite3.h>
#include"ser_sqlt.h"
#include"sock_func.h"
#include"par_data.h"

#define MAX_EVENTS		512
#define DB_NAME			"ser_dtbase.db"
#define TABLE_NAME		"ser_user"

#define CONFIG_DEBUG
#ifdef  CONFIG_DEBUG
#define dbg_print(format,args...)	printf(format,##args)
#else
#define dbg_print(format,args...)	do{} while(0)
#endif

#endif

