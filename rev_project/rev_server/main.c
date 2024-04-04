/*********************************************************************************
 *      Copyright:  (C) 2024 LiYi<1751425323@qq.com>
 *                  All rights reserved.
 *
 *       Filename:  main.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(02/04/24)
 *         Author:  LiYi <1751425323@qq.com>
 *      ChangeLog:  1, Release initial version on "02/04/24 16:30:59"
 *                 
 ********************************************************************************/
#include "main.h"

int main(int argc,char **argv)
{
	//socket
	int						listenfd,connfd;
	int						serv_port=0;
	int						daemon_run=0;
	char					*progname=NULL;
	int						opt;
	int						rv;
	int						i,j;
	int						found;
	int						epollfd;
	int						events;
	struct epoll_event		event;
	struct epoll_event		event_array[MAX_EVENTS];
	//data
	char					buf[1024];
	char					id[16];
	float					temp;
	char					local_t[128];
	//sqlite
	sqlite3					*db;

	struct option			long_options[]=
	{
		{"deamon",no_argument,NULL,'b'},
		{"port",required_argument,NULL,'p'},
		{"help",no_argument,NULL,'h'},
		{NULL,0,NULL,0}
	};
	
	progname=basename(argv[0]);

	while((opt=getopt_long(argc,argv,"bp:h",long_options,NULL))!=-1)
	{
		switch(opt)
		{
			case 'b':
				daemon_run=1;
				break;
			case 'p':
				serv_port=atoi(optarg);
				break;
			case 'h':
				print_usage(progname);
				return EXIT_SUCCESS;
			default:
				break;
		}
	}

	if(!serv_port)
	{
		print_usage(progname);
		return -1;
	}

	set_socket_limit();
	if((listenfd=socket_server_init(NULL,serv_port))<0)
	{
		dbg_print("%s server listen on %d port failure:%s\n",argv[0],serv_port,strerror(errno));
		return -2;
	}
	dbg_print("%server start listen on %d port\n",argv[0],serv_port);

	if(daemon_run)
	{
		daemon(0,0);
	}

	if((epollfd=epoll_create(MAX_EVENTS))<0)
	{
		dbg_print("epoll and listen socket failure:%s\n",strerror(errno));
		return -3;
	}

	event.events=EPOLLIN;
	event.data.fd=listenfd;

	if(epoll_ctl(epollfd,EPOLL_CTL_ADD,listenfd,&event)<0)
	{
		dbg_print("epoll add listen socket failure:%s\n",strerror(errno));
		return -4;
	}
	for(;;)
	{
		events=epoll_wait(epollfd,event_array,MAX_EVENTS,-1);
		if(events<0)
		{
			dbg_print("epoll failure:%s\n",strerror(errno));
			break;
		}
		else if(events==0)
		{
			dbg_print("epoll get timeout\n");
			continue;
		}
		for(i=0;i<events;i++)
		{
			if((event_array[i].events&EPOLLERR)||(event_array[i].events&EPOLLHUP))
			{
				dbg_print("epoll_wait get error on fd[%d]:%s\n",event_array[i].data.fd,strerror(errno));
				epoll_ctl(epollfd,EPOLL_CTL_DEL,event_array[i].data.fd,NULL);
				close(event_array[i].data.fd);
			}

			if(event_array[i].data.fd==listenfd)
			{
				if((connfd=accept(listenfd,(struct sockaddr *)NULL,NULL))<0)
				{
					dbg_print("accept new client failure:%s\n",strerror(errno));
					continue;
				}
				event.data.fd=connfd;
				event.events=EPOLLIN;

				if(epoll_ctl(epollfd,EPOLL_CTL_ADD,connfd,&event)<0)
				{
					dbg_print("epoll add client socket failure:%s\n",strerror(errno));
					close(event_array[i].data.fd);
					continue;
				}
				dbg_print("epoll add new client socket[%d] successfully\n",connfd);
			}
			else
			{
				if((rv=read(event_array[i].data.fd,buf,sizeof(buf)))<=0)
				{
					dbg_print("Socket[%d] read failure or get disconnnected\n",event_array[i].data.fd);
					epoll_ctl(epollfd,EPOLL_CTL_DEL,event_array[i].data.fd,NULL);
					close(event_array[i].data.fd);
					continue;
				}
				else
				{
					dbg_print("Socket[%d] read %d bytes data\n",event_array[i].data.fd,rv);
					printf("%s\n",buf);
					if(write(event_array[i].data.fd,buf,rv)<0)
					{
						dbg_print("Socket[%d] write failure:%s\n",event_array[i].data.fd,strerror(errno));
						epoll_ctl(epollfd,EPOLL_CTL_DEL,event_array[i].data.fd,NULL);
						close(event_array[i].data.fd);
					}
					if((str_sscanf(buf,id,&temp,local_t))<0)/* 分割字符串 */
					{
						dbg_print("Failed to split character:%s\n",strerror(errno));
					}
					db=sqlite3_open_database(DB_NAME);/* 打开数据库 */
					if(sqlite3_create_table(db,TABLE_NAME)==0)/* 创建表 */
					{
						dbg_print("Create table %s successfully\n",TABLE_NAME);
						if(sqlite3_insert(db,TABLE_NAME,id,&temp,local_t)==0)/*插入数据*/
						{
							dbg_print("Insert data successfully\n");
						}
					}
				}
			}
		}
	}
CleanUp:
	close(connfd);
	return 0;
}
