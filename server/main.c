/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  main.c
 *    Description:  This file is
 *                 
 *        Version:  1.0.0(18/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "18/03/24 14:50:09"
 *                 
 ********************************************************************************/
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

#define MAX_EVENTS 			512
int socket_server_init(char *listenip,int listenport);
void print_usage(char * progname);
void set_socket_limit(void);
int str_strtok(char* str,char *data_buf);

int main(int argc,char **argv)
{
	int						listenfd,connfd;
	int						serv_port=0;
	int						daemon_run=0;
	char					*progname=NULL;
	int						opt;
	int						rv;
	int						i,j;
	int						found;
	char					buf[1024];
	char					str[128];
	int						epollfd;
	int						events;
	struct epoll_event		event;
	struct epoll_event		event_array[MAX_EVENTS];

	struct option			long_options[]=
	{
		{"daemon",no_argument,NULL,'b'},
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
		printf("%s server listen on %d port failure:%s\n",argv[0],serv_port,strerror(errno));
		return -2;
	}
	printf("%s server start listen on %d port\n",argv[0],serv_port);

	if(daemon_run)
	{
		daemon(0,0);
	}

	if((epollfd=epoll_create(MAX_EVENTS))<0)
	{
		printf("epoll add listen socket failure:%s\n",strerror(errno));
		return -3;
	}

	event.events=EPOLLIN;
	event.data.fd=listenfd;

	if(epoll_ctl(epollfd,EPOLL_CTL_ADD,listenfd,&event)<0)
	{
		printf("epoll add listen socket failure:%s\n",strerror(errno));
		return -4;
	}
	for(;;)
	{
		events=epoll_wait(epollfd,event_array,MAX_EVENTS,-1);
		if(events<0)
		{
			printf("epoll failure:%s\n",strerror(errno));
			break;
		}
		else if(events==0)
		{
			printf("epoll get timeout\n");
			continue;
		}
		for(i=0;i<events;i++)
		{
			if((event_array[i].events&EPOLLERR)||(event_array[i].events&EPOLLHUP))
			{
				printf("epoll_wait get error on fd[%d]:%s\n",event_array[i].data.fd,strerror(errno));
				epoll_ctl(epollfd,EPOLL_CTL_DEL,event_array[i].data.fd,NULL);
				close(event_array[i].data.fd);
			}

			if(event_array[i].data.fd==listenfd)
			{
				if((connfd=accept(listenfd,(struct sockaddr *)NULL,NULL))<0)
				{
					printf("accept new client failure:%s\n",strerror(errno));
					continue;
				}
				event.data.fd=connfd;
				event.events=EPOLLIN;

				if(epoll_ctl(epollfd,EPOLL_CTL_ADD,connfd,&event)<0)
				{
					printf("epoll add client socket failure:%s\n",strerror(errno));
					close(event_array[i].data.fd);
					continue;
				}
				printf("epoll add new client socket[%d] successfully\n",connfd);
			}
			else
			{
				if((rv=read(event_array[i].data.fd,buf,sizeof(buf)))<=0)
				{
					printf("Socket[%d] read failure or get disconnected\n",event_array[i].data.fd);
					epoll_ctl(epollfd,EPOLL_CTL_DEL,event_array[i].data.fd,NULL);
					close(event_array[i].data.fd);
					continue;
				}

				else
				{
					printf("Socket[%d] read %d bytes data \n",event_array[i].data.fd,rv);
					printf("%s\n",buf);
					if(write(event_array[i].data.fd,buf,rv)<0)
					{
						printf("Socket[%d] write failure:%s\n",event_array[i].data.fd,strerror(errno));
						epoll_ctl(epollfd,EPOLL_CTL_DEL,event_array[i].data.fd,NULL);
						close(event_array[i].data.fd);
					}
					if((str_strtok(str,buf))<0)
					{
						printf("Failed to split character:%s\n",strerror(errno));
					}

					db=sqlite3_open_database(DB_NAME);
					if((sqlite3_create_table(db,TABLE_NAME))==0)
					{
						printf("Create table successfully\n");
						if((sqlite3_insert(db,TABLE_NAME,id,temp,local_t))==0)
						{
							printf("Insert data successfully\n");
						}
					}
				}
			}
		}
	}
CleanUp:
	close(listenfd);
return 0;
}

void set_socket_limit(void)
{
	struct rlimit limit={0};
	getrlimit(RLIMIT_NOFILE,&limit);
	limit.rlim_cur=limit.rlim_max;
	setrlimit(RLIMIT_NOFILE,&limit);
	printf("Set socket open fd max count to %d\n",limit.rlim_max);
}

void print_usage(char * progname)
{
	printf("usage:%s[option]\n",progname);
	printf("-b[daemon] set program running on background\n");
	printf("-p[port] socket server port\n");
	printf("-h[help] display this help inforamtion\n");
	return ;
}

int socket_server_init(char *listenip,int listenport)
{
	struct sockaddr_in		servaddr;
	int						rv=0;
	int						on=1;
	int						listenfd;

	if((listenfd=socket(AF_INET,SOCK_STREAM,0))<0)
	{
		printf("Create socket failure:%s\n",strerror(errno));
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
			printf("inet_pton() set listen ip address failure:%s\n",strerror(errno));
			rv=-2;
			goto CleanUp;
		}
	}
	
	if(bind(listenfd,(struct sockaddr *)&servaddr,sizeof(servaddr))<0)
	{
		printf("Bind the socket failure:%s\n",strerror(errno));
		rv=-3;
		goto CleanUp;
	}

	if(listen(listenfd,64)<0)
	{
		printf("Bind the socket failure:%s\n",strerror(errno));
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

int str_strtok(char *data_buf)
{
	const char		s[2]=",";
	char			*token;

	token=strtok(str,s);
	while(token!=NULL)
	{
		printf("%s\n",token);
		token=strtok(NULL,s);
	}
	return 0;
}
