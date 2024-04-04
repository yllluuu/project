/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  ds18b20_get_temp.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(31/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "31/03/24 16:12:55"
 *                 
 ********************************************************************************/
#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<sys/types.h>
#include<dirent.h>
#include<fcntl.h>
#include<unistd.h>
#include<stdlib.h>
#include"ds18b20_get_temp.h"

int get_temperature(float *temp)
{
	int                             fd=-1;
	char                            buf[128];
	char                            *ptr=NULL;
	DIR                             *dirp=NULL;
	struct dirent           		*direntp=NULL;
	char                            w1_path[64]="/sys/bus/w1/devices/";
	char                            chip_sn[32];
	int                             found=0;

	dirp=opendir(w1_path);
	if(!dirp)
	{
		dbg_print("Open folder %s failure:%s\n",w1_path,strerror(errno));
		return -1;
	}

	while(NULL!=(direntp=readdir(dirp)))
	{
		if(strstr(direntp->d_name,"28-"))
		{
			strncpy(chip_sn,direntp->d_name,sizeof(chip_sn));
			found=1;
		}
	}

	closedir(dirp);

	if(!found)
	{
		dbg_print("Cannot find ds18b20 chipset\n");
		return -2;
	}

	strncat(w1_path,chip_sn,sizeof(w1_path)-strlen(w1_path));
	strncat(w1_path,"/w1_slave",sizeof(w1_path)-strlen(w1_path));

	if((fd=open(w1_path,O_RDONLY))<0)
	{
		dbg_print("Open file failure:%s\n",strerror(errno));
		return -3;
	}

	memset(buf,0,sizeof(buf));
	if(read(fd,buf,sizeof(buf))<0)
	{
		dbg_print("Read data from fd[%d] failure:%s\n",fd,strerror(errno));
		return -4;
	}

	ptr=strstr(buf,"t=");
	if(!ptr)
	{
		dbg_print("Cannot find t= string\n");
		return -5;
	}

	ptr +=2;
	*temp=atof(ptr)/1000;
	close(fd);
	return 0;
}

