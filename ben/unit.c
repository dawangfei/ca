/* Copyright (c)  2017 Cmbc 
   All rights reserved

   THIS IS UNPUBLISHED PROPRIETARY
   SOURCE CODE OF dc Systems, Inc.
   The copyright notice above does not
   evidence any actual or intended
   publication of such source code.
 */

#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <locale.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>

#include "log.h"
#include "aux.h"
#include "caapi.h"


/*******************************************************************
 *******************************************************************/

/* ================================================================= */
extern long get_time(void);
extern int dc_random_int(int _base);

/* ================================================================= */


typedef struct _sql_t
{
    char  sql[1024];

    int   type;

    int   i_value;
    char  s_value[64];
}  sql_t;

#define SQL_MAX     100
sql_t g_sql[SQL_MAX];
int   g_num = 0;

enum 
{
    SQL_STR,
    SQL_INT
};

static int dc_cache_add(char *_sql, int _type, int _i, char *_s)
{

    if (g_num  >= SQL_MAX)
    {
        elog("error:  too much: %d", g_num);
        return -1;
    }

    snprintf(g_sql[g_num].sql, sizeof(g_sql[g_num].sql), "%s", _sql);

    g_sql[g_num].type =  _type;

    if (_type == SQL_INT)
    {
        g_sql[g_num].i_value = _i;
    }
    else if (_type == SQL_STR)
    {
        snprintf(g_sql[g_num].s_value, sizeof(g_sql[g_num].s_value), "%s", _s);
    }
    else
    {
        elog("error: invalid type: %d", _type);
        return -1;
    }

    g_num++;

    return  0;
}


static int dc_cache_dump()
{
    int i = 0;
    int type = 0;

    for (i = 0; i < g_num; i++)
    {
        type  = g_sql[i].type;
        if (type == SQL_INT)
        {
            olog("dump[%03d]:[%s] => [%d](INT)", i, g_sql[i].sql, g_sql[i].i_value);
        }
        else
        {
            olog("dump[%03d]:[%s] => [%s](STR)", i, g_sql[i].sql, g_sql[i].s_value);
        }
    }

    return  0;
}


static int  dc_cache_load_case(char *_path)
{
    int  len = 0;
    char *sep = "|";
    char *sql = NULL;
    char *value = NULL;
    char *type = "";
    char line[1024] = {0};

    FILE *fp = NULL;

    fp = fopen(_path, "r");
    if (fp == NULL)
    {
        elog("error: fopen: %s, %d, %s",  _path, errno,  strerror(errno));
        return -1;
    }

    memset(line, 0, sizeof(line));
    while ((fgets(line, sizeof(line), fp) != NULL))
    {
        len = strlen(line);

        /* clear newline */
        if (line[len-1] == '\n')
        {
            line[len-1] = 0;
            len--;
        }

        /* check line */
        if (len < 10)
        {
            elog("error: invalid input: [%s]",  line);
            return -1;
        }

        /*  field 1 */
        sql = strtok(line, sep);
        if (sql == NULL)
        {
            elog("error: no sql field");
            return  -1;
        }

        /*  field 2 */
        value = strtok(NULL, sep);
        if (value == NULL)
        {
            elog("error: no value field");
            return  -1;
        }

        /*  field 3 */
        type  = strtok(NULL, sep);
        if (type == NULL)
        {
            /* default is string */
            type = "S";
        }

        olog("[%s] =>  [%s][%s]",   sql, value, type);
        if (type[0] == 'I')
        {
            if (dc_cache_add(sql, SQL_INT, atoi(value), NULL))
            {
                elog("error: dc_cache_add: INT");
                return -1;
            }
            olog("add INTEGER case succeeds");
        }
        else
        {
            if (dc_cache_add(sql, SQL_STR, 0,  value))
            {
                elog("error: dc_cache_add: STRING");
                return -1;
            }
            olog("add STRING case succeeds");
        }

        memset(line, 0, sizeof(line));
    }

    fclose(fp);

    return  0;
}


/**
 */
static int dc_cache_unit30(int _max)
{
    int  idx = 0;
    int  rv = 0;
    int  k  = 0;
    int  y   = 0;
    long begin = 0;
    long costs = 0;
    char *sql = NULL;
    char v[64] = {0};
    sql_t *p = NULL;


    olog("num: [%d]", g_num);

    olog("----------------------------------------");

    begin = get_time();

    for (k = 0; k < _max; k++)
    {
        idx  = dc_random_int(g_num);
        olog("idx: %d", idx);

        p = &g_sql[idx];

        olog("sql: %s, %d", p->sql, p->type);

        if ((rv = dc_cache_api_select(p->sql)) < 0)
        {
            elog("error: dc_cache_api_select: %s", p->sql);
            return -1;
        }
        else if (rv == 0)
        {
            olog("no data");
            /* only for this case */
            return -1;
        }
        else
        {
            rv = 0;
        }

        if (p->type == SQL_INT)
        {
            if (dc_cache_api_get_data(0, &y, sizeof(y)) < 0)
            {
                elog("error: dc_cache_api_get_data");
                return -1;
            }

            if (y == p->i_value)
            {
                elog("[INT]nice: equal: %d == %d", y,  p->i_value);
            }
            else
            {
                elog("[INT]error: not equal: %d != %d", y,  p->i_value);
            }
        }
        else
        {
            if (dc_cache_api_get_data(0, v, sizeof(v)) < 0)
            {
                elog("error: dc_cache_api_get_data");
                return -1;
            }

            if (strcmp(v,p->s_value) == 0)
            {
                elog("[STR]nice: equal: %s == %s", v,  p->s_value);
            }
            else
            {
                elog("[STR]error: not equal: %s != %s", v,  p->s_value);
            }
        }

    } /* for */

    costs = get_time() - begin;

    if (k > 0)
    {
        olog("loop(%d) costs(%ld)us, avg: %.2f", k, costs, costs*1.00/k);
    }
    else
    {
        elog("error!");
    }

    return rv;
}




int main(int argc, char *argv[])
{
    int  rv  = 0;
    int  max = 0;
    long key = 7788;
    char path[1024] = {0};

    (void)argc;
    (void)argv;

    olog("mix begin");

    if (dc_cache_api_open(key))
    {
        elog("error: dc_cache_api_open");
        return -1;
    }

    snprintf(path, sizeof(path), "%s", "unit.case");
    dc_cache_load_case(path);

    dc_cache_dump();

    dc_cache_unit30(1);

    olog("mix end");

    return rv;
}

/* mix.c */
