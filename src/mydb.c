#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/time.h>
#include <errno.h>
#include <assert.h>
#include <stdarg.h>

#include "tm.h"

static char g_log_file[64] = {0};

static long my_get_time();

/* ================================================================= */

int pe_log( char *file_name, int line_no, char * formt, ... )
{
    va_list ap;
    va_start(ap, formt);
    fprintf(stdout, "%10.10s,%04d -- ", 
            file_name, line_no);
    vfprintf(stdout, formt, ap);
    va_end(ap);
    fprintf(stdout, "\n");
    fflush(stdout);
    return 0;
}


void bcl_set_log_file(char *_filename)
{
    snprintf(g_log_file, sizeof(g_log_file), "%s",  _filename);
}


void bcl_log(char * logfile, char *file_name, int line_no, char * formt, ...)
{
    char timestamp[30+1] = {0};

    FILE *fp = NULL;

    dctime_format_micro(my_get_time(), timestamp, sizeof(timestamp));

    va_list ap;
    va_start(ap, formt);

#if STARRING_INTEGRATE
    int  to_close = 0;
    char *ptr = NULL;
    char file_path[4096] = {0};

    /* priority: 1. global set; 2. parameter of function. */
    if (g_log_file[0] != 0)
    {
        logfile = g_log_file;
    }

    if (logfile == NULL || logfile[0] == 0)
    {
        fp = stdout;
    }
    else
    {
        ptr = getenv("FAPWORKDIR");
        if (ptr != NULL)
        {
            snprintf(file_path, sizeof(file_path), "%s/log/%s", ptr, logfile);
        }
        else
        {
            snprintf(file_path, sizeof(file_path), "%s", logfile);
        }

        fp = fopen(file_path, "a+");
        if (fp == NULL)
        {
            fp = stdout;
        }
        else
        {
            to_close = 1;
        }
    }
#else
    fp = stdout;
#endif

    fprintf(fp, "%.23s,%04d,%10.10s,%04d -- ", 
           timestamp, getpid(), file_name, line_no);
    vfprintf(fp, formt, ap);
    va_end(ap);
    fprintf(fp, "\n");
    fflush(fp);

#if STARRING_INTEGRATE
    if  (to_close && fp != NULL)
    {
        fclose(fp);
    }
#endif

    return ;
}


static long my_get_time()
{
    struct timeval tv;

    gettimeofday(&tv, NULL);

    return (long)(tv.tv_sec*1000000+tv.tv_usec);
}

double bcl_myatof( char * str )
{
    return atof( str );
}


void trc_db_data_rec( int _oper_type, int _data_id, int _data_type, char *_data_desc, char *_data, int _len )
{
}

int bclerreg( int appcode, char * file, long line, ... )
{
    return 0;
}

void trc_db_rec( char *_sql_str, int _sql_code )
{
}


void  bcl_log_err ( char *_e , char *_f, int n,  char *f, ... )
{
}

char * bcl_ltrim( char *str )
{
    register char *pstart, *ptr;

    if ( str == NULL )
        return NULL;

    pstart = ptr = str;

    while ( *( ( unsigned char * )ptr ) == 0x20 || *ptr == '\t' || \
            *ptr == '\r' || *ptr == '\n' )
        ptr ++;

    while ( *ptr != 0x0 )
        *str ++ = *ptr ++;

    *str = 0x0;

    return pstart;
}

char * bcl_rtrim( char *str )
{
    register char *ptr;

    if ( str == NULL )
        return NULL;

    ptr = str + strlen( str ) - 1;

    while ( ptr >= str && ( *( ( unsigned char * )ptr ) == 0x20 || \
                *ptr == '\t' ||  *ptr == '\r' || *ptr == '\n' ))
        *ptr -- = 0x0;

    return str;
}

/* mydb.c */
