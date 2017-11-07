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

#include "pladb.h"
#include "sysloc.h"
#include "caapi.h"


/*******************************************************************
 *******************************************************************/

/* ================================================================= */

/* ================================================================= */

typedef struct _bin_t
{
    char bin[6+1];
    char name[50+1];
} bin_t;


int query_data()
{
    int  rv = 0;
    int  idx = 0;
    int  rows = 0;
    int  area = 0;
    char sql[256] = {0};
    char v[32] = {0};
    char v2[32] = {0};

    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    snprintf(sql, sizeof(sql), "%s", 
            "SELECT BIN, NAME  FROM T_CREDIT_BIN WHERE BIN='622600'");

    snprintf(sql, sizeof(sql), "%s", 
            "SELECT *  FROM Tbl_LOG_counter WHERE txn_date='20170511' order by pan");

    area = CAMallocArea();
    if (area == E_FAIL)
    {
        printf("error: CAMallocArea\n");
        return -1;
    }

    while (1)
    {
        rv = CADBSelect(area, sql, 0, 0);

        if (E_FAIL == rv)
        {
            CAFreeArea();
            printf("error: CADBSelect\n");
            return E_FAIL;
        }

        if ( rv == E_NOTFOUND )
        {
            printf("no more data\n");
            rv = 0;
            break;
        }

        rows++;

        idx = 0;
        memset(v, 0, sizeof(v));
        if (CADBGetData(idx, v, sizeof(v)) < 0)
        {
            printf("error: CADBGetData\n");
            return -1;
        }

        idx = 1;
        memset(v2, 0, sizeof(v2));
        if (CADBGetData(idx, v2, sizeof(v2)) < 0)
        {
            printf("error: CADBGetData\n");
            return -1;
        }
        printf("      ===>> [%s, %s]\n", v, v2);
    }

    CAFreeArea();

    printf("rows: %d\n", rows);
    printf("----------------------------------------\n");

    return 0;
}


int count_data()
{
    int  rv = 0;
    int  idx = 0;
    int  area = 0;
    char sql[256] = {0};
    int  n = 0;

    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    snprintf(sql, sizeof(sql), "%s", 
            "SELECT COUNT(*) FROM TBL_LOG_COUNTER WHERE TXN_DATE='20170711'");
    snprintf(sql, sizeof(sql), "%s", 
            "SELECT COUNT(*) FROM T_bin where bin='1' ");

    area = CAMallocArea();
    if (area == E_FAIL)
    {
        printf("error: CAMallocArea\n");
        return -1;
    }

    rv = CADBSelect(area, sql, 0, 0);

    if (E_FAIL == rv)
    {
        CAFreeArea();
        printf("error: CADBSelect\n");
        return E_FAIL;
    }

    if ( rv == E_NOTFOUND )
    {
        printf("no data\n");
        CAFreeArea();
        return 0;
    }

    idx = 0;
    if (CADBGetData(idx, &n, sizeof(n)) < 0)
    {
        printf("error: CADBGetData\n");
        CAFreeArea();
        return -1;
    }

    printf("count is: %d\n", n);

    CAFreeArea();

    printf("----------------------------------------\n");

    return 0;
}


int put_data3()
{
    int  rv = 0;
    int  idx = 0;
    int  area = 0;
    char sql[256] = {0};
    char buf[64] = {0};

    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    snprintf(sql, sizeof(sql), "%s", 
            "SELECT count(1) FROM t_Tx WHERE TX_code=?");

    snprintf(sql, sizeof(sql), "%s", 
            "SELECT tx_name FROM t_Tx WHERE TX_code=? and TX_TYPE=? and tx_flag=?");

    area = CAMallocArea();
    if (area == E_FAIL)
    {
        printf("error: CAMallocArea\n");
        return -1;
    }

    CADBPutData(0, FAPCHAR, "PAY_053");
    CADBPutData(1, FAPCHAR, "5");
    CADBPutData(2, FAPCHAR, "2");

    rv = CADBSelect(area, sql, 0, 0);

    if (E_FAIL == rv)
    {
        CAFreeArea();
        printf("error: CADBSelect\n");
        return E_FAIL;
    }

    if ( rv == E_NOTFOUND )
    {
        printf("no more data\n");
        CAFreeArea();
        return 0;
    }

    idx = 0;
#if 0
    if (CADBGetData(idx, &n, sizeof(n)) < 0)
    {
        printf("error: CADBGetData\n");
        CAFreeArea();
        return -1;
    }
#endif
    if (CADBGetData(idx, buf, sizeof(buf)) < 0)
    {
        printf("error: CADBGetData\n");
        CAFreeArea();
        return -1;
    }

    printf("[%s] ==> %s\n", sql, buf);

    CAFreeArea();

    printf("----------------------------------------\n");

    return 0;
}


int put_data4()
{
    int  rv = 0;
    int  idx = 0;
    int  area = 0;
    char sql[256] = {0};
    char buf[64] = {0};

    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    snprintf(sql, sizeof(sql), "%s", 
            "SELECT name FROM t_credit_bin WHERE bin=?");

    area = CAMallocArea();
    if (area == E_FAIL)
    {
        printf("error: CAMallocArea\n");
        return -1;
    }

    CADBPutData(0, FAPCHAR, "379986");

    rv = CADBSelect(area, sql, 0, 0);

    if (E_FAIL == rv)
    {
        CAFreeArea();
        printf("error: CADBSelect\n");
        return E_FAIL;
    }

    if ( rv == E_NOTFOUND )
    {
        printf("no more data\n");
        CAFreeArea();
        return 0;
    }

    idx = 0;
    if (CADBGetData(idx, buf, sizeof(buf)) < 0)
    {
        printf("error: CADBGetData\n");
        CAFreeArea();
        return -1;
    }

    printf("[%s] ==> %s\n", sql, buf);

    CAFreeArea();

    printf("----------------------------------------\n");

    return 0;
}


int put_data()
{
    int  rv = 0;
    int  idx = 0;
    int  area = 0;
    char sql[256] = {0};
    int  n = 0;

    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    snprintf(sql, sizeof(sql), "%s", 
            "SELECT COUNT(*) FROM TBL_LOG_COUNTER WHERE TXN_DATE=?");

    area = CAMallocArea();
    if (area == E_FAIL)
    {
        printf("error: CAMallocArea\n");
        return -1;
    }

    CADBPutData(0, FAPCHAR, "20170911");

    rv = CADBSelect(area, sql, 0, 0);

    if (E_FAIL == rv)
    {
        CAFreeArea();
        printf("error: CADBSelect\n");
        return E_FAIL;
    }

    if ( rv == E_NOTFOUND )
    {
        printf("no more data\n");
        CAFreeArea();
        return 0;
    }

    idx = 0;
    if (CADBGetData(idx, &n, sizeof(n)) < 0)
    {
        printf("error: CADBGetData\n");
        CAFreeArea();
        return -1;
    }

    printf("count-put is: %d\n", n);

    CAFreeArea();

    printf("----------------------------------------\n");

    return 0;
}


int put_data2()
{
    int  rv = 0;
    int  idx = 0;
    int  area = 0;
    char sql[256] = {0};
    char buf[64] = {0};

    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    snprintf(sql, sizeof(sql), "%s", 
            "SELECT DEPTNO_NAME FROM dept WHERE DEPTNO=? or DEPTNO=?");

    area = CAMallocArea();
    if (area == E_FAIL)
    {
        printf("error: CAMallocArea\n");
        return -1;
    }

    CADBPutData(0, FAPCHAR, "20");
    CADBPutData(1, FAPCHAR, "22");

    rv = CADBSelect(area, sql, 0, 0);

    if (E_FAIL == rv)
    {
        CAFreeArea();
        printf("error: CADBSelect\n");
        return E_FAIL;
    }

    if ( rv == E_NOTFOUND )
    {
        printf("no more data\n");
        CAFreeArea();
        return 0;
    }

    idx = 0;
    if (CADBGetData(idx, &buf, sizeof(buf)) < 0)
    {
        printf("error: CADBGetData\n");
        CAFreeArea();
        return -1;
    }

    printf("data: %s\n", buf);

    CAFreeArea();

    printf("----------------------------------------\n");

    return 0;
}


/* 2017-10-10 for empty  */
int put_data5()
{
    int  rv = 0;
    int  idx = 0;
    int  area = 0;
    char sql[256] = {0};
    char buf[256] = {0};
    int  n = 0;

    printf("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n");
    snprintf(sql, sizeof(sql), "%s", 
            "SELECT COUNT(*) FROM TBL_LOG_COUNTER WHERE TXN_DATE=? and LAST_FAIL_TIME=?");

    area = CAMallocArea();
    if (area == E_FAIL)
    {
        printf("error: CAMallocArea\n");
        return -1;
    }

    CADBPutData(0, FAPCHAR, "20170509");
    CADBPutData(1, FAPCHAR, "");

    rv = CADBSelect(area, sql, 0, 0);

    if (E_FAIL == rv)
    {
        CAFreeArea();
        printf("error: CADBSelect\n");
        return E_FAIL;
    }

    if ( rv == E_NOTFOUND )
    {
        printf("no more data\n");
        CAFreeArea();
        return 0;
    }

    idx = 0;
    if (CADBGetData(idx, &n, sizeof(n)) < 0)
    {
        printf("error: CADBGetData\n");
        CAFreeArea();
        return -1;
    }

    memset(buf, 0, sizeof(buf));

    printf("count-put is: %d\n", n);

    CAFreeArea();

    printf("----------------------------------------\n");

    return 0;
}


int query_data0()
{
    int  rv = 0;
    char sql[256] = {0};
    char v[32] = {0};

    snprintf(sql, sizeof(sql), "%s", 
            "SELECT NAME, BIN  FROM T_CREDIT_BIN "
            "WHERE BIN='622600'");

    if ((rv = CASelect(sql)) < 0)
    {
        printf("error: CASelect: %d\n", rv);
        return -1;
    }
    else if (rv == 0)
    {
        printf("no data\n");
        return 1;
    }
    printf("query succeeds\n");

    if (CAGetData(0, v, sizeof(v)) < 0)
    {
        printf("error: CAGetData\n");
        return -1;
    }
    printf("get data: [%s]\n", v);

    return 0;
}


int iterator_data()
{
    int  rv = 0;
    int  idx = 0;
    char sql[256] = {0};
    char v[32] = {0};

    snprintf(sql, sizeof(sql), "%s", 
            "SELECT INST_DATE_TIME, SUCC_TOTAL_NUM  FROM TBL_LOG_COUNTER "
            "WHERE TXN_TYPE=  'ZHXXYZ' ");

    if ((rv = CACursor(sql)) < 0)
    {
        printf("error: CACursor: %d\n", rv);
        return -1;
    }
    printf("open cursor succeeds\n");

    idx = 0;
    while (CAFetch() != NULL)
    {
        memset(v, 0, sizeof(v));
        if (CAGetData(idx, v, sizeof(v)) < 0)
        {
            printf("error: CAGetData\n");
            return -1;
        }
        printf("===>>: %s\n", v);
    }

    printf("iterate cursor ends\n");

    return 0;
}


int dc_ext_connect_db();
int reload_table()
{
    char table_name[32] = {0};

    /* XXX: connect to db as necessary */
    if (dc_ext_connect_db())
    {
        printf("error: dc_ext_connect_db\n");
        return -1;
    }

    snprintf(table_name, sizeof(table_name), "%s", "tbl_log_counter");
    if (CAReload(table_name))
    {
        printf("error: CAReload: %s\n", table_name);
        return -1;
    }
    printf("reload table [%s] succeeds\n", table_name);

    return 0;
}


int txn_remit()
{
    char *sep = "==========================================";

    printf("%s\n", sep);

    if (reload_table())
    {
        printf("error: reload_table\n");
        return -1;
    }
    printf("%s\n", sep);

    if (query_data())
    {
        printf("error: query_data\n");
        return -1;
    }
    printf("%s\n", sep);

    if (count_data())
    {
        printf("error: count_data\n");
        return -1;
    }
    printf("%s\n", sep);


    if (put_data())
    {
        printf("error: put_data\n");
        return -1;
    }
    printf("%s\n", sep);

    if (put_data2())
    {
        printf("error: put_data2\n");
        return -1;
    }
    printf("%s\n", sep);

    if (put_data3())
    {
        printf("error: put_data3\n");
        return -1;
    }
    printf("%s\n", sep);


    if (put_data4())
    {
        printf("error: put_data4\n");
        return -1;
    }
    printf("%s\n", sep);

    if (put_data5())
    {
        printf("error: put_data5\n");
        return -1;
    }
    printf("%s\n", sep);

    /*
    if (iterator_data())
    {
        printf("error: iterator_data\n");
        return -1;
    }
    printf("%s\n", sep);
    */


    return 0;
}


int main(int argc, char *argv[])
{
    int  rv  = 0;
    long key = 7788;

    (void)argc;
    (void)argv;

    printf("ala begin\n");

    /* only one time */
    if (CAOpen(key))
    {
        printf("error: CAOpen\n");
        return -1;
    }

    /* connect to DB, only for mixed mode: both DB and cache */
    if (dc_ext_connect_db() == -1)
    {
        printf("error: dc_ext_connect_db\n");
        return -1;
    }

    /* business */
    if (txn_remit())
    {
        printf("error: txn_remit\n");
        return -1;
    }

    /* only one time */
    if (CAClose())
    {
        printf("error: CAClose\n");
        return -1;
    }

    printf("ala end\n");

    return rv;
}

/* ala.c */
