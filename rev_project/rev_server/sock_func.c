/*********************************************************************************
 *      Copyright:  (C) 2024 LiYi<1751425323@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  sock_func.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(02/04/24)
 *         Author:  LiYi <1751425323@qq.com>
 *      ChangeLog:  1, Release initial version on "02/04/24 15:57:38"
 *                 
 ********************************************************************************/
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

void set_socket_limit(void)
{
	struct rlimit	limit={0};
	getrlimit(RLIMIT_NOFILE,&limit);
	limit.rlim_cur=limit.rlim_max;
	setrlimit(RLIMIT_NOFILE,&limit);
	dbg_print("Set socket open fd max count to %d\n",limit.rlim_max);
}

void print_usage(char * progname)
{
	dbg_print("usage:%s[option]\n",progname);
	dbg_print("-b[deamon] set program running on background\n");
	dbg_print("-p[port] socket server port\n");
	dbg_print("-h[help] display this help information\n");
}

int socket_server_init(char* listenip,int listenport)
{
	struct sockaddr_in		servaddr;
	int						rv=0;
	int						on=1;
	int						listenfd;

	if((listenfd=socket(AF_INET,SOCK_STREAM,0))<0)
	{
		dbg_print("Create socket failure:%s\n",strerror(errno));
		return -1;
	}
	setsockopt(listenfd,SOL_SOCKET,SO_REUSEADDR,&on,sizeof(on));

	memset(&servaddr,0,sizeof(servaddr));
	servaddr.sin_family=AF_INET;
	servaddr.sin_port=htons(listenport);
	if(!listenip)
	{
		servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
	}
	else
	{
		if(inet_pton(AF_INET,listenip,&servaddr.sin_addr)<=0)
		{
			dbg_print("inet_pton() set listenip address failure:%s\n",strerror(errno));
			rv=-2;
			goto CleanUp;
		}
	}
	if(bind(listenfd,(struct sockaddr*)&servaddr,sizeof(servaddr))<0)
	{
		dbg_print("Bind the socket failure:%s\n",strerror(errno));
		rv=-3;
		goto CleanUp;
	}
	if(listen(listenfd,64)<0)
	{
		dbg_print("Bind the socket failure:%s\n",strerror(errno));
		rv=-4;
		goto CleanUp;
	}
CleanUp:
	if(rv<0)
		close(listenfd);
	else
		rv=listenfd;
	return rv;
}
