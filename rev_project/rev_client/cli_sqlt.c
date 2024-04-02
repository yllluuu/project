/*********************************************************************************
 *      Copyright:  (C) 2024 linuxer<linuxer@email.com>
 *                  All rights reserved.
 *
 *       Filename:  sqlt.c
 *    Description:  This file 
 *                 
 *        Version:  1.0.0(20/03/24)
 *         Author:  linuxer <linuxer@email.com>
 *      ChangeLog:  1, Release initial version on "20/03/24 20:17:15"
 *                 
 ********************************************************************************/
//#include<stdio.h>
//#include<sqlite3.h>
//#include<string.h>
#include<errno.h>
#include<stdlib.h>
#include<unistd.h>

//创建打开表
sqlite3* sqlite3_open_database(char * db_name)
{
	int			 	rc=0;
	sqlite3* 		db;

	if(db_name==NULL||strlen(db_name)==0)
	{
		return NULL;
	}

	rc=sqlite3_open(db_name,&db);
	if(rc!=SQLITE_OK)
	{
		dbg_print("Open db error:%s\n",sqlite3_errmsg(db));
		return NULL;
	}

	else
	{
		dbg_print("Open a sqlite3 database successfully!\n");
	}

	return db;
}

void sqlite3_close_database(sqlite3 *db)
{
	if(db!=NULL)
	{
		sqlite3_close(db);
	}
}

//创建表
int sqlite3_create_table(sqlite3* db,char * table_name)
{
	int 	rc=0;
	int 	ret=-1;
	char	sql[128]={0};
	char	*err_msg=NULL;

	sprintf(sql,"CREATE TABLE IF NOT EXISTS %s(ID TEXT,Temperature REAL,time1 TEXT)",table_name);
	rc=sqlite3_exec(db,sql,0,0,&err_msg);
	if(rc!=SQLITE_OK)
	{
		dbg_print("create table %s error:%s\n",table_name,err_msg);
		sqlite3_free(err_msg);
		ret=-1;
	}
	else
	{
		ret=0;
	}
	return ret;
}

//删除表
int sqlite3_delete_table(sqlite3*db,char * table_name)
{
	int		rc=0;
	int 	ret=-1;
	char	sql[128]={0};
	char	*err_msg=NULL;

	sprintf(sql,"DROP TABLE %s",table_name);
	rc=sqlite3_exec(db,sql,0,0,&err_msg);
	if(rc!=SQLITE_OK)
	{
		dbg_print("delete table %s error:%s\n",table_name,err_msg);
		sqlite3_free(sql);
		ret=-1;
	}
	else
	{
		ret=0;
	}
	return ret;
}

//插入数据
int sqlite3_insert(sqlite3* db,char* table_name,char *ID,float  *temp,char * localtime)
{
	int		rc=0;
	int		ret=-1;
	char	sql[128];
	char	*err_msg=NULL;

	sprintf(sql,"INSERT INTO %s(ID,Temperature,time1) VALUES ('%s',%f,'%s')",table_name,ID,*temp,localtime);
	
	rc=sqlite3_exec(db,sql,0,0,&err_msg);
	if(rc!=SQLITE_OK)
	{
		dbg_print("Insert error:%s\n",err_msg);
		sqlite3_free(err_msg);
		ret=-1;
	}
	else
	{
		ret=0;
	}
	return ret;
}

//删除数据
int	sqlite3_delete(sqlite3* db,char* table_name)
{
	int 	rc=0;
	int		ret=-1;
	char	sql[128]={0};
	char	*err_msg=NULL;
	sprintf(sql,"DELETE FROM %s WHERE ROWID = (SELECT MIN(rowid) FROM %s);",table_name,table_name);
	rc=sqlite3_exec(db,sql,0,0,&err_msg);
	if(rc!=SQLITE_OK)
	{
		dbg_print("delete error:%s\n",err_msg);
		sqlite3_free(err_msg);
		ret=-1;
	}
	else{
		ret=0;
	}
	return ret;
}

//查询数据
int sqlite3_select(sqlite3* db,char* table_name,char *data_buf)
{
	int		i;
	char	*err_msg=NULL;
	char	sql[128]={0};
	int		rc=0;
	int		rows,columns;
	char	**results;

	sprintf(sql,"SELECT * FROM %s",table_name);
	rc=sqlite3_get_table(db,sql,&results,&rows,&columns,&err_msg);
	if(rc!=SQLITE_OK)
	{
		dbg_print("Select error:%s\n",err_msg);
		sqlite3_free(err_msg);
		return -1;
	}	
	snprintf(data_buf,128,"%s %.2f %s\n",results[3],atof(results[4]),results[5]);		    
	return rows;
}
