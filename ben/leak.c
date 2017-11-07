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


/**
 *  invalid sql
 */
static int dc_cache_leak_SQL(int _max)
{
    int  rv = 0;
    int  k  = 0;
    long begin = 0;
    long costs = 0;
    char sql[256] = {0};
    char v[32] = {0};
    char *e = "20170508190147";

    olog("----------------------------------------");
    olog("TBL_LOG_COUNTER"); 
    snprintf(sql, sizeof(sql), "%s", 
            "SELECT INST_DATE_TIME, SUCC_TOTAL_NUM  FROM TBL_LOG_COUNTER "
            "WHERE PAN");

    begin = get_time();

    for (k = 0; k < _max; k++)
    {
        if ((rv = dc_cache_api_select(sql)) < 0)
        {
            elog("error: dc_cache_api_select: %s", sql);
            /* return -1; XXX */
            continue;
        }
        else if (rv == 0)
        {
            olog("no data");
            return 1;
        }
        else
        {
            rv = 0;
        }

        if (dc_cache_api_get_data(0, v, sizeof(v)) < 0)
        {
            elog("error: dc_cache_api_get_data");
            return -1;
        }

        if (strcmp(e, v) == 0)
        {
        }
        else
        {
            olog("expect: [%s]", e);
            olog("but:    [%s]", v);
            rv = -1;

            break;
        }
    }

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

    (void)argc;
    (void)argv;

    olog("norm begin");

    if (dc_cache_api_open(key))
    {
        elog("error: dc_cache_api_open");
        return -1;
    }

    max = 1000;
    max = 10000;
    max = 1000000;
    max = 3;
    if (dc_cache_leak_SQL(max))
    {
        elog("error: dc_cache_leak_SQL");
        return -1;
    }

    if (dc_cache_api_close())
    {
        elog("error: dc_cache_api_close");
        return -1;
    }

    olog("norm end");

    return rv;
}

/* norm.c */
