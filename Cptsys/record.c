/* ------------------------------------------------------------
 *Record Task: 
 * Copyright (C) 2018 chenming<chenm.idsse.ac.cn>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * Created on Oct 31,2018
 * -------------------------------------------------------------*/
 
#include "vxworks.h"
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <unistd.h>
#include "ioLib.h"
#include "fcntl.h"
#include "timers.h"
#include "time.h"
#include "record.h"
#include "semLib.h"

#define MAX_LEVEL_STR               (10)
#define MAX_DATE_STR                (10)
#define DATE_STR_FMT                "%04d%02d%02d"
#define MAX_TIME_STR                (20)
#define TIME_STR_FMT                "%04d/%02d/%02d %02d:%02d:%02d"
#define MAX_FILE_PATH               (260)
#define MAX_LOG_LINE                (4096)

#define SLOG_MUTEX SEM_ID

typedef struct _logger_cfg 
{
    FILE *log_file;
    SLOG_MUTEX mtx;
    slog_level filter_levle;
    int inited;
} logger_cfg;

static logger_cfg g_logger_cfg = {
    NULL, {0}, S_INFO, FALSE };

/* -------------------------------------------------------------------------------------------
   @brief:  读文件
   @param: 	None
   @retval: None 
----------------------------------------------------------------------------------------------*/
 STATUS read_file(char *filename)
 {
 		char path[255] ;
 		char buf[100] ;
 		int	 fd, i, n ;
 		
 		sprintf(path, "/ata0a/log/%s.log", filename) ;
 		
  	if ((fd = open(path, 2, 0)) == ERROR )
 		{
 			printf("\n open file error!") ;
 			return (ERROR) ;
 		}			
 		
 		while( (n = read(fd,buf,100)) > 0 )
 		{
	 		for(i = 0 ; i < n ; i++)
	 		{
	 			printf("%c",buf[i]) ;
	 		} 			
 		}
 		 	 		
 		close(fd) ;	 	
 		return(OK) ;
 }
 
/* -------------------------------------------------------------------------------------------
   @brief:  获取当前系统时间
   @param: 	None
   @retval: None 
----------------------------------------------------------------------------------------------*/
void  getsystime(char* datetime)
{ 
	struct tm daytime ;
	time_t		stime ;
	
	stime = time(NULL) ;	
	daytime = *localtime(&stime) ;
	sprintf(datetime, "%04d%02d%02d-%02d%02d%02d",
												daytime.tm_year + 1900, daytime.tm_mon + 1, daytime.tm_mday,
												daytime.tm_hour, daytime.tm_min, daytime.tm_sec);
}

/* -------------------------------------------------------------------------------------------
   @brief:  将BIOS时间设置为当前系统时间
   @param: 	None
   @retval: None 
----------------------------------------------------------------------------------------------*/
void biostime(void)
{
  struct tm   ahora;
  unsigned char temp;
  struct timespec tpp;
  
/* 将BIOS的时间读取到ahora中*/
  sysOutByte(0x70,0x00/*second*/);
  temp = sysInByte(0x71);
  ahora.tm_sec = (temp&0x0F) + 10*((temp&0xF0)>>4);
  sysOutByte(0x70,0x02/*minut*/);
  temp = sysInByte(0x71);
  ahora.tm_min = (temp&0x0F) + 10*((temp&0xF0)>>4);
  sysOutByte(0x70,0x04/*hour*/);
  temp = sysInByte(0x71);
  ahora.tm_hour = (temp&0x0F) + 10*((temp&0xF0)>>4);

  sysOutByte(0x70,0x07/*day*/);
  temp = sysInByte(0x71);
  ahora.tm_mday = (temp&0x0F) + 10*((temp&0xF0)>>4);
  sysOutByte(0x70,0x08/*month*/);
  temp = sysInByte(0x71);
  ahora.tm_mon = (temp&0x0F) + 10*((temp&0xF0)>>4) - 1;
  sysOutByte(0x70,0x09/*year*/);
  temp = sysInByte(0x71);
  ahora.tm_year = 100 + (temp&0x0F) + 10*((temp&0xF0)>>4);
  
/* 将ahora中的时间设为系统的时间*/ 
 	tpp.tv_sec = mktime(&ahora) ;
  tpp.tv_nsec = 0;
  clock_settime(CLOCK_REALTIME, &tpp);
}   


static void _slog_init_mutex(SLOG_MUTEX *mtx)
{
	mtx = semBCreate (SEM_Q_PRIORITY, SEM_FULL) ;
}

static void _slog_lock(SLOG_MUTEX *mtx)
{
	semTake (mtx, WAIT_FOREVER);
}

static void _slog_unlock(SLOG_MUTEX *mtx)
{
	semGive(mtx) ;
}

/* -------------------------------------------------------------------------------------------
   @brief:  
   @param: 	None
   @retval: None 
----------------------------------------------------------------------------------------------*/
static void _get_curr_date(int datestr_size, char datestr[])
{
    time_t tt = { 0 };
    struct tm *curr_time = NULL;

    time(&tt);
    curr_time = localtime(&tt);
    sprintf(datestr, DATE_STR_FMT,
        curr_time->tm_year + 1900, curr_time->tm_mon + 1, curr_time->tm_mday);
}

static void _get_curr_time(int timestr_size, char timestr[])
{
    time_t tt = { 0 };
    struct tm *curr_time = NULL;

    time(&tt);
    curr_time = localtime(&tt);
    sprintf(timestr, TIME_STR_FMT,
        curr_time->tm_year + 1900, curr_time->tm_mon + 1, curr_time->tm_mday,
        curr_time->tm_hour, curr_time->tm_min, curr_time->tm_sec);
}

static char *_get_level_str(slog_level level)
{
    switch (level) {
    case S_TRACE:
        return "[TRACE]";
    case S_DEBUG:
        return "[DEBUG]";
    case S_INFO:
        return "[INFO ]";
    case S_WARN:
        return "[WARN ]";
    case S_ERROR:
        return "[ERROR]";
    default:
        return "[     ]";
    }
}

static int _slog_mkdir(const char *log_dir)
{
    if (mkdir(log_dir) != 0) {
        return FALSE;
    }
    return TRUE;
}


int init_logger(const char *log_dir, slog_level level)
{
    char log_filepath[MAX_FILE_PATH] = { 0 };
    char datestr[MAX_DATE_STR] = { 0 };
    int		fd ;

    if (TRUE == g_logger_cfg.inited) {
        return TRUE;
    }

		if ( (fd = fopen(log_dir, 'r')) == NULL )
			{
				if (_slog_mkdir(log_dir) != TRUE)
				 {
            return FALSE;
         }
			}
		else
				fclose(fd) ;
		
/*    if (access(log_dir, 0) != 0) {
        if (_slog_mkdir(log_dir) != TRUE) {
            return FALSE;
        }
    } */
		
		printf("\n create directory success!") ;
    _slog_init_mutex(&g_logger_cfg.mtx);

    _get_curr_date(sizeof(datestr), datestr);
    sprintf(log_filepath, "%s/%s.log", log_dir, datestr);
    printf("\n path file name: %s", log_filepath) ;
    g_logger_cfg.log_file = fopen(log_filepath, "a+");
    if (NULL == g_logger_cfg.log_file) {
        return FALSE;
    }
    g_logger_cfg.filter_levle = level;
    g_logger_cfg.inited = TRUE;

    return TRUE;
}

void write_log(slog_level level, int print_stacktrace, const char *func_name, int line, const char *fmt, ...)
{
    va_list args;
    char *level_str = NULL;
    char timestr[MAX_TIME_STR] = { 0 };
    char log_content[MAX_LOG_LINE] = { 0 };
    char log_line[MAX_LOG_LINE] = { 0 };
    
    if ( !g_logger_cfg.inited )
    	{
				if ( !init_logger("/ata0a/log", S_INFO))
					{
						printf("\n open init logger failure!") ;	
						return(ERROR) ;							
					}			
	   	}

    if (g_logger_cfg.filter_levle > level) {
        return;
    }
    
    va_start(args, fmt);
    vsprintf(log_content, fmt, args) ;
    va_end(args) ;

    _get_curr_time(sizeof(timestr), timestr);
    level_str = _get_level_str(level);
    sprintf(log_line, "%s,%s, %s:%d||,%s\n",
        level_str, timestr, func_name, line, log_content);
    _slog_lock(&g_logger_cfg.mtx);
    fwrite(log_line, sizeof(char), strlen(log_line), g_logger_cfg.log_file);
    fflush(g_logger_cfg.log_file);
    _slog_unlock(&g_logger_cfg.mtx);
}