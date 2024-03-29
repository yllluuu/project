/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  ser_sqlt.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(28/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "28/03/24 20:53:23"
 *                 
 ********************************************************************************/

#include<stdio.h>
#include<sqlite3.h>

sqlite3* sqlite3_open_database(char* db_name)
{
	int			rc=0;
	sqlite3		*db;
	if(db_name==NULL||strlen(db_name)==0)
	{
		return NULL;
	}
	rc=sqlite3_open(db_name,&db);
	if(rc!=SQLITE_OK)
	{
		printf("Open db error:%s\n",sqlite3_errmsg(db));
		return NULL;
	}
	else
	{
		printf("Open a sqlite3 database successfully\n");
	}
	return db;
}

void sqlite3_close_database(sqlite3* db)
{
	if(db!=NULL)
	{
		sqlite3_close(db);
	}
}

int sqlite3_create_table(sqlite3* db,char * table_name)
{
	int			rc=0;
	int			ret=-1;
	char		sql[128]={0};
	char		*err_msg=NULL;

	sprintf(sql,"CREATE TABLE IF NOT EXISTS %s (ID TEXT,Temperature REAL,time1 TEXT)",table_name);
	rc=sqlite3_exec(db,sql,0,0,&err_msg);
	if(rc!=SQLITE_OK)
	{
		printf("Create table %s error:%s\n",table_name,err_msg);
		sqlite3_free(err_msg);
		ret=-1;
	}
	else
	{
		ret=0;
	}
	return ret;
}

int sqlite3_insert(sqlite3* db,char* table_name,char *id,float * temp,char *localtime)
{
	int			rc=0;
	int			ret=-1;
	char		sql[128];
	char		*err_msg=NULL;
	sprintf(sql,"INSERT INTO %s(ID,Temperature,time1) VALUES('%s',%f,'%s')",table_name,ID,*temp,localtime);
	rc=sqlite3_exe(db,sql,0,0,&err_msg);
	if(rc!=SQLITE_OK)
	{
		printf("Insert error:%s\n",err_msg);
		sqlite3_free(err_msg);
		ret=-1;
	}
	else
	{
		ret=0;
	}
	return ret;
}


