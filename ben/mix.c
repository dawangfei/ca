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


char *dc_cache_indicated_select_sql_4mix(char *_sql_list[], int _num, int _seq)
{
    int  idx = 0;

    idx = _seq % _num;
    /* olog("idx2: [%d]", idx); */

    return _sql_list[idx];
}


/**
 * MIX
 */
static int dc_cache_case21(int _max)
{
    int  rv = 0;
    int  k  = 0;
    int  len = 0;
    long begin = 0;
    long costs = 0;
    char *sql = NULL;
    char v[32] = {0};

    char *sql_list[] = {

            "SELECT flag FROM t_ctrl_bw_bfr WHERE (product_id ='993130010000000019' OR product_id ='AAAAAA') AND (operate_type ='31399990040313999900400' OR operate_type ='AAAAAA') AND flag ='3'",

            "SELECT ctrl_no FROM t_ctrl_acct where  acct_no='600679928' and ctrl_type='01'",

            "SELECT plat_date FROM t_plat_para",

            "SELECT * FROM TBL_LOG_COUNTER WHERE (TXN_DATE='20170511' or PAN='6216920000008264') and TXN_TYPE='ZHXXYZ';",


            "select * from T_CTRL_ACCT where acct_no='20121003266810046551' and ctrl_type = '01'",

            "select bin, name from t_credit_bin where bin = '622623'",

            "SELECT * FROM t_tx WHERE tx_code='PAY_002'",

            "select * from T_bin where bin ='622622'"
            };

    len = sizeof(sql_list)/sizeof(sql_list[0]);
    olog("len: [%d]", len);

    olog("----------------------------------------");
    olog("sql-HIS: mixed scene"); 

    begin = get_time();

    for (k = 0; k < _max; k++)
    {
        sql = dc_cache_indicated_select_sql_4mix(sql_list, len, k);
        /*
        olog("%s", sql);
        */
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
        /*
        olog("value: [%s]", v);
        */
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
 * MIX: 
 */
static int dc_cache_case22(int _max)
{
    int  rv = 0;
    int  k  = 0;
    int  len = 0;
    long begin = 0;
    long costs = 0;
    char *sql = NULL;
    char v[32] = {0};

    char *sql_list[] = {

            "SELECT flag FROM t_ctrl_bw_bfr WHERE (product_id ='993130010000000019' OR product_id ='AAAAAA') AND (operate_type ='31399990040313999900400' OR operate_type ='AAAAAA') AND flag ='3'",
            "SELECT flag FROM t_ctrl_bw_bfr  WHERE (product_id ='010101I10100000000' OR product_id ='AAAAAA') AND (operate_type ='70200410000623004100001' OR operate_type ='AAAAAA') AND flag ='3'",
            "SELECT flag FROM t_ctrl_bw_bfr  WHERE (product_id ='993130010000000025' OR product_id ='AAAAAA') AND (operate_type ='31399992507313999925070' OR operate_type ='AAAAAA') AND flag ='3'",

            "SELECT ctrl_no FROM t_ctrl_acct where  acct_no='600679928' and ctrl_type='01'",
            "SELECT ctrl_no FROM t_ctrl_acct where  acct_no='9594000112810001' and ctrl_type='02'",
            "SELECT ctrl_no FROM t_ctrl_acct where  acct_no='DOMAINID633' and ctrl_type='02'",
            "SELECT ctrl_no FROM t_ctrl_acct where  acct_no='6216920000279642' and ctrl_type='01'",

            "SELECT plat_date FROM t_plat_para",

            "SELECT * FROM TBL_LOG_COUNTER WHERE PAN='6216920000008264';",
            "SELECT * FROM TBL_LOG_COUNTER WHERE TXN_TYPE = 'ZHXXYZ'",
            "SELECT * FROM TBL_LOG_COUNTER WHERE TXN_DATE='20170509' or PAN='6226223100055923' and TXN_TYPE='ZHXXYZ';",
            "SELECT * FROM TBL_LOG_COUNTER WHERE (TXN_DATE='20170511' or PAN='6216920000008264') and TXN_TYPE='ZHXXYZ';",


            "select * from T_CTRL_ACCT where acct_no='20121003266810046551' and ctrl_type = '01'",
            "select * from T_CTRL_ACCT where acct_no='9708110001000404' and ctrl_type = '02'",
            "select * from T_CTRL_ACCT where acct_no='1111116' and ctrl_type = '12'",
            "select * from T_CTRL_ACCT where acct_no='9594000112810003' and ctrl_type = '02'", 
            "select * from T_CTRL_ACCT where acct_no='693155455' and ctrl_type = '03'", 
            "select * from T_CTRL_ACCT where acct_no='695173845' and ctrl_type = '01'", 
            "select * from T_CTRL_ACCT where acct_no='20121003266810043526' and ctrl_type = '01'",
            "select * from T_CTRL_ACCT where acct_no='20121003266810044822' and ctrl_type = '01'", 
            "select * from T_CTRL_ACCT where acct_no='20121003266810051898' and ctrl_type = '01'",

            "select bin, name from t_credit_bin where bin = '622623'",
            "select bin, name from t_credit_bin where name = 'CUPnianqing'",
            "SELECT * FROM t_plat_para",
            "SELECT * FROM t_tx WHERE tx_code='PAY_002'",
            "SELECT flag FROM t_ctrl_bw_bfr WHERE (product_id ='993130010000000019' OR product_id ='AAAAAA') AND (operate_type ='31399990040313999900400' OR operate_type ='AAAAAA') AND flag ='3'",
            "select * from t_bin where bin ='622622'",
            "SELECT ctrl_no FROM t_ctrl_acct where  acct_no='600679928' and ctrl_type='01'",
            "SELECT ctrl_no FROM t_ctrl_acct where  acct_no='9594000112810001' and ctrl_type='02'",
            "select * from T_bin where bin ='622622'",
            "SELECT PLAT_DATE FROM t_plat_para"
            };

    len = sizeof(sql_list)/sizeof(sql_list[0]);
    olog("len: [%d]", len);
    for (k = 0; k < len; k++)
    {
        olog("%s", sql_list[k]);
    }

    olog("----------------------------------------");
    olog("sql-HIS: T_CTRL_ACCT: use index"); 

    begin = get_time();

    for (k = 0; k < _max; k++)
    {
        sql = dc_cache_indicated_select_sql_4mix(sql_list, len, k);
        /*
        olog("%s", sql);
        */
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
        /*
        olog("value: [%s]", v);
        */
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

    olog("mix begin");

    if (dc_cache_api_open(key))
    {
        elog("error: dc_cache_api_open");
        return -1;
    }




#if CASE21
    max = 1;
    max = 10000;
    if ((rv = dc_cache_case21(max)) < 0)
    {
        elog("error: dc_cache_case21");
        return -1;
    }
    else if (rv == 1)
    {
        olog("no data");
    }

    if (dc_cache_api_close())
    {
        elog("error: dc_cache_api_close");
        return -1;
    }
#endif


    max = 1;
    max = 10000;
    if ((rv = dc_cache_case22(max)) < 0)
    {
        elog("error: dc_cache_case22");
        return -1;
    }
    else if (rv == 1)
    {
        olog("no data");
    }

    if (dc_cache_api_close())
    {
        elog("error: dc_cache_api_close");
        return -1;
    }
#if CASE22
#endif


    olog("mix end");

    return rv;
}

/* mix.c */
