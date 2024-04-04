/********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  main.h
 *    Description:  This file 
 *
 *        Version:  1.0.0(01/04/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "01/04/24 15:35:07"
 *                 
 ********************************************************************************/
#ifndef	_MAIN_H_
#define _MAIN_H_


#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <getopt.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <time.h>
#include <sqlite3.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <dirent.h>
#include <fcntl.h>
#include <netinet/tcp.h>
#include "cli_sqlt.h"
#include "ds18b20_get_temp.h"
#include "get_func.h"
#include "sock_ali_rec.h"

#define CONFIG_DEBUG
#ifdef CONFIG_DEBUG
#define dbg_print(format,args...)	printf(format,##args)
#else
#define dbg_print(format,args...)	do{} while(0)
#endif

#define DB_NAME		"ser_dtbase.db"
#define TABLE_NAME	"ser_user"

#endif

