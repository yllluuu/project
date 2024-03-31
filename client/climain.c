/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  main.c
 *    Description:  This file is socket client sample source code
 *                 
 *        Version:  1.0.0(17/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "17/03/24 15:24:18"
 *                 
 ********************************************************************************/
#include<stdio.h>
#include<string.h>
#include<errno.h>
#include<unistd.h>
#include<sys/types.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<dirent.h>
#include<stdlib.h>
#include<getopt.h>
#include<fcntl.h>
#include <time.h>
#include<netinet/tcp.h>
#include<sqlite3.h>
#include"sqlt.h"

#define DB_NAME 		"dtbase.db"
#define TABLE_NAME    	"user"
int get_temperature(float *temp);
void print_usage(char *progname);
int get_dev(char *ID,int len);
int get_tm(char *localt);
int socket_alive(int fd);
int sock_reconnect(char *servip,int port);

int main(int argc,char ** argv)
{
	int 						connfd=-1;
	int 						k,l;
	int							rv=-1;
	float						temp;
	int							time;
	char						buf[1024];
	int							len=20;
	char 						Id[16];
	char						data_buf[1024];
	sqlite3						*db;
	char						localt[128];
	struct sockaddr_in			servadrr;
	char						*servip;
	int							port;
	int							ch;
	int 						rows;

	struct option				opts[]=
	{
		{"ipaddr",required_argument,NULL,'i'},
		{"port",required_argument,NULL,'p'},
		{"time",required_argument,NULL,'t'},
		{"help",no_argument,NULL,'h'},
		{NULL,0,NULL,0}
	};

	while((ch=getopt_long(argc,argv,"i:p:t:h",opts,NULL))!=-1)
	{
		switch(ch)
		{
			case 'i':
				servip=optarg;
				break;
			case 'p':
				port=atoi(optarg);
				break;
			case 't':
				time=atoi(optarg);
				break;
			case 'h':
				print_usage(argv[0]);
				return 0;
		}
	}

	if(!servip||!port||!time)
	{
		print_usage(argv[0]);
		return 0;
	}

	connfd=socket(AF_INET,SOCK_STREAM,0);
	if(connfd<0)
	{
		printf("Create socket failure:%s",strerror(errno));
		return -1;
	}

	memset(&servadrr,0,sizeof(servadrr));
	servadrr.sin_family=AF_INET;
	servadrr.sin_port=htons(port);
	inet_aton(servip,&servadrr.sin_addr);

	if(connect(connfd,(struct sockaddr *)&servadrr,sizeof(servadrr))<0)
	{
		printf("connect to server[%s] [%d] failure:%s\n",servip,port,strerror(errno));
		return -1;
	}
	printf("connect to server[%s] [%d] successfully!\n",servip,port);

	while(1)
	{
		memset(buf,0,sizeof(buf));
		get_temperature(&temp);
		get_dev(Id,len);
		get_tm(localt);
		snprintf(buf,sizeof(buf),"%s %f %s",Id,temp,localt);
		k=socket_alive(connfd);
		if(k<0)//服务端断开
		{
			close(connfd);
			//打开数据库
			db=sqlite3_open_database(DB_NAME);
			//创建表
			if((sqlite3_create_table(db,TABLE_NAME))==0)
			{
				//写入表
				if((sqlite3_insert(db,TABLE_NAME,Id,&temp,localt))==0)
				{
					printf("Insert data successfully\n");
				}
			}
			sqlite3_close_database(db);
			//重新连接
			if(sock_reconnect(servip,port)<0)/* 未连上 */	
			{
				printf("Reconnect failure:%s\n",strerror(errno));
			}
			//continue;
			else/* 已连上 */
			{
				db=sqlite3_open_database(DB_NAME);/* 打开数据库 */
				memset(data_buf,0,sizeof(data_buf));
				do
				{
					rows=sqlite3_select(db,TABLE_NAME,data_buf);
					if((write(connfd,data_buf,strlen(data_buf)))<0)
					{
						printf("Write data to server failure:%s\n",strerror(errno));
						goto CleanUp;
					} 
					sqlite3_delete(db,TABLE_NAME);
				}while(rows>1);/* 查找数据发送数据并删除数据库中数据 */

				sqlite3_close_database(db);/* 关闭数据库 */
				//continue;
			}
		}
		else//服务端未断开
		{
			if(write(connfd,buf,strlen(buf))<0)
			{
				printf("Write data to server failure:%s\n",strerror(errno));
				goto CleanUp;
			}	


			memset(buf,0,sizeof(buf));
			rv=read(connfd,buf,sizeof(buf));
			if(rv<0)
			{
				printf("Read data from server failure:%s\n",strerror(errno));
				goto CleanUp;
			}	

			else if(rv==0)
			{
				printf("Client connect to server get disconnected\n");
				goto CleanUp;
			}

			//	printf("%s\n",buf);
		}
		sleep(time);
	}

CleanUp:
	close(connfd);

	return 0;
}

void print_usage(char *progname)
{
	printf("%s usage:\n",progname);
	printf("-i(--ipaddr):specify server IP address.\n");
	printf("-p(--port):specify server port.\n");
	printf("-t(--time):Sampling interval.\n");
	printf("-h(--help):print this help information.\n");
	return;
}

int get_temperature(float *temp)
{
	int					fd=-1;
	char				buf[128];
	char				*ptr=NULL;
	DIR					*dirp=NULL;
	struct dirent		*direntp=NULL;
	char				w1_path[64]="/sys/bus/w1/devices/";
	char				chip_sn[32];
	int					found=0;

	dirp=opendir(w1_path);
	if(!dirp)
	{
		printf("Open folder %s failure:%s\n",w1_path,strerror(errno));
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
		printf("Cannot find ds18b20 chipset\n");
		return -2;
	}

	strncat(w1_path,chip_sn,sizeof(w1_path)-strlen(w1_path));
	strncat(w1_path,"/w1_slave",sizeof(w1_path)-strlen(w1_path));

	if((fd=open(w1_path,O_RDONLY))<0)
	{
		printf("Open file failure:%s\n",strerror(errno));
		return -3;
	}

	memset(buf,0,sizeof(buf));
	if(read(fd,buf,sizeof(buf))<0)
	{
		printf("Read data from fd[%d] failure:%s\n",fd,strerror(errno));
		return -4;
	}

	ptr=strstr(buf,"t=");
	if(!ptr)
	{
		printf("Cannot find t= string\n");
		return -5;
	}

	ptr +=2;
	*temp=atof(ptr)/1000;
	close(fd);
	return 0;
}

//获取产品序列号
int get_dev(char *ID,int len)
{
	int sn=1;
	snprintf(ID,len,"%05d",sn);

}

//获取当地时间
int get_tm(char *localt)
{
	time_t	seconds;
	struct tm *local;

	time(&seconds);

	local = localtime(&seconds);

	snprintf(localt,64,"%d/%d/%d-%d:%d:%d\n",local->tm_year+1900,local->tm_mon+1,local->tm_mday,local->tm_hour,local->tm_min,local->tm_sec);

	return 0;
}

//检查服务端是否断线
int socket_alive(int fd)
{
	struct tcp_info		info;
	int					len=sizeof(info);
	getsockopt(fd,IPPROTO_TCP,TCP_INFO,&info,(socklen_t *)&len);
	if((info.tcpi_state==TCP_ESTABLISHED))
	{
		//printf("The server is not disconnected\n");
		return 0;
	}
	else
	{
		printf("The server is disconnected\n");
		return -1;
	}
}
//重新连接
int sock_reconnect(char *servip,int port)
{
	struct sockaddr_in 		servaddr;
	int						connfd;

	printf("Trying to reconnect server...\n");

	connfd=socket(AF_INET,SOCK_STREAM,0);
	if(connfd<0)
	{	
		printf("Create socket failure:%s\n",strerror(errno));

	}

	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(port);
	inet_aton(servip,&servaddr.sin_addr);
	if(connect(connfd,(struct sockaddr *)&servaddr,sizeof(servaddr))<0)
	{	
		printf("Reconnect to server failure:%s\n",strerror(errno));		
		close(connfd);
		return -1;
	}
	else
	{
		printf("Reconnect server successfully\n");
		return connfd;
	}
}


