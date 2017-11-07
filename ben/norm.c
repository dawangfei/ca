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
 * single row
 */
static int dc_cache_norm_SINGLE(int _max)
{
    int  rv = 0;
    int  k  = 0;
    long begin = 0;
    long costs = 0;
    char sql[256] = {0};
    char v[32] = {0};
    char *e = "20170508190147";

    olog("----------------------------------------");
    olog("sql-SINGLE: TBL_LOG_COUNTER"); 
    snprintf(sql, sizeof(sql), "%s", 
            "SELECT INST_DATE_TIME, SUCC_TOTAL_NUM  FROM TBL_LOG_COUNTER "
            "WHERE PAN='6216920000008264' AND TXN_TYPE ='ZHXXYZ' ");

    begin = get_time();

    for (k = 0; k < _max; k++)
    {
        if ((rv = dc_cache_api_select(sql)) < 0)
        {
            elog("error: dc_cache_api_select: %s", sql);
            return -1;
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


/**
 * case1: use index and hit
 * single row: T_TX
 */
static int dc_cache_case1(int _max)
{
    int  rv = 0;
    int  k  = 0;
    long begin = 0;
    long costs = 0;
    char sql[256] = {0};
    char v[32] = {0};
    char *e = "BAT_HANGUP";

    olog("----------------------------------------");
    olog("sql-SINGLE: T_TX"); 
    snprintf(sql, sizeof(sql), "%s", 
            "SELECT * FROM t_tx WHERE tx_code='PAY_361'");

    begin = get_time();

    for (k = 0; k < _max; k++)
    {
        if ((rv = dc_cache_api_select(sql)) < 0)
        {
            elog("error: dc_cache_api_select: %s", sql);
            return -1;
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

        if (dc_cache_api_get_data(11, v, sizeof(v)) < 0)
        {
            elog("error: dc_cache_api_get_data");
            return -1;
        }

        if (strcmp(e, v) == 0)
        {
            if (_max == 1)
            {
                olog("matched: [%s]", v);
            }
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


/**
 * case2: use index but can't hit
 * single row: T_TX
 */
static int dc_cache_case2(int _max)
{
    int  rv = 0;
    int  k  = 0;
    long begin = 0;
    long costs = 0;
    char sql[256] = {0};

    olog("----------------------------------------");
    olog("sql-SINGLE: T_TX"); 
    snprintf(sql, sizeof(sql), "%s", 
            "SELECT * FROM t_tx WHERE tx_code='PAY_888'");

    begin = get_time();

    for (k = 0; k < _max; k++)
    {
        if ((rv = dc_cache_api_select(sql)) < 0)
        {
            elog("error: dc_cache_api_select: %s", sql);
            return -1;
        }
        else if (rv == 0)
        {
            /* come here */
            rv = 1;
            continue;
        }
        else
        {
            rv = 0;
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


/**
 * case3: full table scan
 * single row: T_TX
 */
static int dc_cache_case3(int _max)
{
    int  rv = 0;
    int  k  = 0;
    long begin = 0;
    long costs = 0;
    char sql[256] = {0};
    char v[32] = {0};
    char *e = "DCRD_001";

    olog("----------------------------------------");
    olog("sql-SINGLE: T_TX"); 
    snprintf(sql, sizeof(sql), "%s", 
            "SELECT tx_code FROM t_tx WHERE item_no='DDCARD'");

    begin = get_time();

    for (k = 0; k < _max; k++)
    {
        if ((rv = dc_cache_api_select(sql)) < 0)
        {
            elog("error: dc_cache_api_select: %s", sql);
            return -1;
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
            if (_max == 1)
            {
                olog("matched: [%s]", v);
            }
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


/**
 * case4: use index and hit
 * single row: T_BIN
 */
static int dc_cache_case4(int _max)
{
    int  rv = 0;
    int  k  = 0;
    long begin = 0;
    long costs = 0;
    char sql[256] = {0};
    char v[32] = {0};
    char *e = "622622";

    olog("----------------------------------------");
    olog("sql-SINGLE: T_BIN"); 
    snprintf(sql, sizeof(sql), "%s", "select * from t_bin where bin ='622622'");

    begin = get_time();

    for (k = 0; k < _max; k++)
    {
        if ((rv = dc_cache_api_select(sql)) < 0)
        {
            elog("error: dc_cache_api_select: %s", sql);
            return -1;
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
            if (_max == 1)
            {
                olog("matched: [%s]", v);
            }
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


/**
 * case5: use index but can't hit
 * single row: T_TX
 */
static int dc_cache_case5(int _max)
{
    int  rv = 0;
    int  k  = 0;
    long begin = 0;
    long costs = 0;
    char sql[256] = {0};

    olog("----------------------------------------");
    olog("sql-SINGLE: T_BIN: no data case"); 
    snprintf(sql, sizeof(sql), "%s", "select * from t_bin where bin ='622'");

    begin = get_time();

    for (k = 0; k < _max; k++)
    {
        if ((rv = dc_cache_api_select(sql)) < 0)
        {
            elog("error: dc_cache_api_select: %s", sql);
            return -1;
        }
        else if (rv == 0)
        {
            /* come here */
            rv = 1;
            if (_max == 1)
            {
                olog("not found");
            }
            continue;
        }
        else
        {
            rv = 0;
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



/**
 * case6: full table scan
 * single row: T_CREDIT_BIN
 */
static int dc_cache_case6(int _max)
{
    int  rv = 0;
    int  k  = 0;
    long begin = 0;
    long costs = 0;
    char sql[256] = {0};
    char v[32] = {0};
    char *e = "622623";

    olog("----------------------------------------");
    olog("sql-SINGLE: T_CREDIT_BIN: full-table-scan"); 
    snprintf(sql, sizeof(sql), "%s", 
		    "select bin, name from t_credit_bin where name = 'CUPnianqing'");

    begin = get_time();

    for (k = 0; k < _max; k++)
    {
        if ((rv = dc_cache_api_select(sql)) < 0)
        {
            elog("error: dc_cache_api_select: %s", sql);
            return -1;
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
            if (_max == 1)
            {
                olog("matched: [%s]", v);
            }
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


/**
 * case7: use index and hit
 * single row: T_CTRL_ACCT
 */
static int dc_cache_case7(int _max)
{
    int  rv = 0;
    int  k  = 0;
    long begin = 0;
    long costs = 0;
    char sql[256] = {0};
    char v[32] = {0};
    char *e = "PLMTDZK0000000000001";

    olog("----------------------------------------");
    olog("sql-SINGLE: T_CTRL_ACCT: use index"); 
    snprintf(sql, sizeof(sql), "%s", "select * from T_CTRL_ACCT "
            "where acct_no='20121003266810046551' "
            "and ctrl_type = '01'");
    olog("%s", sql);

    begin = get_time();

    for (k = 0; k < _max; k++)
    {
        if ((rv = dc_cache_api_select(sql)) < 0)
        {
            elog("error: dc_cache_api_select: %s", sql);
            return -1;
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

        if (dc_cache_api_get_data(2, v, sizeof(v)) < 0)
        {
            elog("error: dc_cache_api_get_data");
            return -1;
        }

        if (strcmp(e, v) == 0)
        {
            if (_max == 1)
            {
                olog("matched: [%s]", v);
            }
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


char *dc_cache_random_select_sql(char *_sql_list[], int _num)
{
    int  idx = 0;

    idx = dc_random_int(_num);
    /* olog("idx: [%d]", idx); */

    return _sql_list[idx];
}


char *dc_cache_indicated_select_sql(char *_sql_list[], int _num, int _seq)
{
    int  idx = 0;

    idx = _seq % _num;
    /* olog("idx2: [%d]", idx); */

    return _sql_list[idx];
}


/**
 * HIS
 */
static int dc_cache_norm_HIS(int _max)
{
    int  rv = 0;
    int  k  = 0;
    int  len = 0;
    long begin = 0;
    long costs = 0;
    char *sql = NULL;
    char v[32] = {0};
    char *sql_list[] = {
            "select * from T_CTRL_ACCT where acct_no='20121003266810046551' and ctrl_type = '01'",
            "select * from T_CTRL_ACCT where acct_no='9708110001000404' and ctrl_type = '02'",
            "select * from T_CTRL_ACCT where acct_no='1111116' and ctrl_type = '12'",
            "select * from T_CTRL_ACCT where acct_no='9594000112810003' and ctrl_type = '02'", 
            "select * from T_CTRL_ACCT where acct_no='693155455' and ctrl_type = '03'", 
            "select * from T_CTRL_ACCT where acct_no='695173845' and ctrl_type = '01'", 
            "select * from T_CTRL_ACCT where acct_no='20121003266810043526' and ctrl_type = '01'",
            "select * from T_CTRL_ACCT where acct_no='20121003266810044822' and ctrl_type = '01'", 
            "select * from T_CTRL_ACCT where acct_no='20121003266810051898' and ctrl_type = '01'"
            };

    len = sizeof(sql_list)/sizeof(sql_list[0]);
    olog("len: [%d]", len);

    olog("----------------------------------------");
    olog("sql-HIS: T_CTRL_ACCT: use index"); 

    begin = get_time();

    for (k = 0; k < _max; k++)
    {
        /* sql = dc_cache_random_select_sql(sql_list, len); */
        sql = dc_cache_indicated_select_sql(sql_list, len, k);
	/* olog("%s", sql); */
        if ((rv = dc_cache_api_select(sql)) < 0)
        {
            elog("error: dc_cache_api_select: %s", sql);
            return -1;
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

        if (dc_cache_api_get_data(2, v, sizeof(v)) < 0)
        {
            elog("error: dc_cache_api_get_data");
            return -1;
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




/**
 * multiple rows
 */
static int dc_cache_norm_MUL()
{
    int  rv = 0;
    int  idx = 0;
    char sql[256] = {0};
    char v[32] = {0};

    olog("----------------------------------------");
    olog("sql1: TBL_LOG_COUNTER"); 
    snprintf(sql, sizeof(sql), "%s", 
            "SELECT INST_DATE_TIME, SUCC_TOTAL_NUM  FROM TBL_LOG_COUNTER "
            "WHERE TXN_TYPE=  'ZHXXYZ' ");

    if (dc_cache_api_cursor(sql) < 0)
    {
        elog("error: dc_cache_api_cursor: %s", sql);
        return -1;
    }

    while (dc_cache_api_fetch() != NULL)
    {
        idx = 0;
        memset(v, 0, sizeof(v));
        if (dc_cache_api_get_data(idx, v, sizeof(v)) < 0)
        {
            elog("error: dc_cache_api_get_data");
            return -1;
        }
        olog("      ===>>: %s", v);
    }

    olog("----------------------------------------");

    /*********************************************************************/

    return rv;
}


static int dc_cache_norm_ERROR()
{
    int  rv = 0;
    int  idx = 0;
    int  rows = 0;
    char sql[256] = {0};
    char v[32] = {0};

    olog("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
    snprintf(sql, sizeof(sql), "%s", 
            "SElECT inst_DATE_TIME, SUCC_tOTAL_NUM  fROM tbl_Log_counter "
            "WHErE pan ='6216920000008264' AnD TXN_TYPE=  'ZHXXYZ' "
            " ANd TXn_DATE = '20170509' ");

    snprintf(sql, sizeof(sql), "%s", 
            "SElECT inst_DATE_TIME, SUCC_tOTAL_NUM  fROM tbl_Log_counter "
            "WHErE pan ='6216920000008264' AnD TXN_TYPE=  'ZHXXYZ' "
            " OR TXn_DATE = '20170509' ");

    rows = dc_cache_api_select(sql);
    if (rows < 0)
    {
        elog("error: dc_cache_api_select: %s", sql);
        return -1;
    }
    else if (rows == 0)
    {
        olog("not found");
    }
    else
    {
        idx = 0;
        memset(v, 0, sizeof(v));
        if (dc_cache_api_get_data(idx, v, sizeof(v)) < 0)
        {
            elog("error: dc_cache_api_get_data");
            return -1;
        }
        olog("      ===>>: %s", v);
    }

    olog("----------------------------------------");

    /*********************************************************************/

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


#if 0
    max = 2;
    max = 1000000;
    max = 1000;
    max = 10000;
    if (dc_cache_norm_SINGLE(max))
    {
        elog("error: dc_cache_norm_SINGLE");
        return -1;
    }


    if (dc_cache_norm_MUL())
    {
        elog("error: dc_cache_norm_MUL");
        return -1;
    }


    if (dc_cache_norm_ERROR())
    {
        elog("error: dc_cache_norm_ERROR");
        return -1;
    }
#endif


#if CASE1
    max = 1;
    max = 1000;
    max = 10000;
    if (dc_cache_case1(max))
    {
        elog("error: dc_cache_case1");
        return -1;
    }
#endif


#if CASE2
    max = 10000;
    max = 1000000;
    if ((rv = dc_cache_case2(max)) < 0)
    {
        elog("error: dc_cache_case2");
        return -1;
    }
    else if (rv == 1)
    {
        olog("no data");
    }
#endif


#if CASE3
    max = 10000;
    if ((rv = dc_cache_case3(max)) < 0)
    {
        elog("error: dc_cache_case3");
        return -1;
    }
    else if (rv == 1)
    {
        olog("no data");
    }
#endif


#if CASE4
    max = 1;
    max = 10000;
    if ((rv = dc_cache_case4(max)) < 0)
    {
        elog("error: dc_cache_case4");
        return -1;
    }
    else if (rv == 1)
    {
        olog("no data");
    }
#endif

#if CASE5
    max = 1;
    max = 10000;
    max = 1000000;
    if ((rv = dc_cache_case5(max)) < 0)
    {
        elog("error: dc_cache_case5");
        return -1;
    }
    else if (rv == 1)
    {
        olog("no data");
    }
#endif


#if CASE6
    max = 1;
    max = 10000;
    if ((rv = dc_cache_case6(max)) < 0)
    {
        elog("error: dc_cache_case6");
        return -1;
    }
    else if (rv == 1)
    {
        olog("no data");
    }
#endif


#if CASE7
    max = 1;
    max = 10000;
    if ((rv = dc_cache_case7(max)) < 0)
    {
        elog("error: dc_cache_case7");
        return -1;
    }
    else if (rv == 1)
    {
        olog("no data");
    }
#endif


    max = 1;
    max = 10000;
    if (dc_cache_norm_HIS(max))
    {
        elog("error: dc_cache_norm_HIS");
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
