#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <errno.h>
#include <assert.h>

#include "tm.h"
#include "type.h"
#include "log.h"
#include "util.h"
#include "list.h"
#include "hash.h"

#include "shm.h"
#include "blk.h"
#include "tree.h"
#include "seg.h"
#include "sec.h"
#include "sql.h"
#include "rs.h"
#include "query.h"
#include "his.h"

/*******************************************************************
  Only support one row result set case.
  affect rs.c && sql.c
 *******************************************************************/

/* ================================================================= */

static void node_free(void *ptr);
static int dc_his_table_full(void *_table);
static int dc_his_remove_table(void *_table);
static int dc_his_remove_old_sql(void *_table);
static int dc_his_put_sql(void *_table, char *_sql, void *_row, int _row_len, void *_select);
static void *dc_his_make_table(char *_table_name, void *_seg_header);
static int dc_his_refresh(void *_node);

/* ================================================================= */


typedef struct _his_table_def_t {
    long t2;                        /* table timestamp */
    int  num;                       /* total number */
    char name[SEGMENT_NAME_MAX+1];  /* table name */
    segment_header_t seg;
    List list;
    Link head;
} his_table_def_t;

typedef struct _his_node_def_t {
    char *sql;                      /* HEAP memory: also as KEY */
    void *row;                      /* HEAP memory: row data */
    int  row_len;
    int  times;                     /* UPDATE every hit */
    long t3;                        /* UPDATE every hit */
    dc_sql_c_t select;
    void *table;                    /* pointer only */
    Link link;
} his_node_def_t;


#define DC_HIS_SQL_HASH_BUCKETS     768
#define DC_HIS_TBL_HASH_BUCKETS     96

#define DC_HIS_TABLE_MAX_NUM        100  /* TODO: FIXME: optimize, bigger, better */
static int          g_his_total_num = 0;
static int          g_his_table_max = 0;

static hashtable_t *g_his_sql_kv = NULL;
static hashtable_t *g_his_tbl_kv = NULL;

/*
 *  Function:  dc_his_init
 *      only invoke one time!
 *
 *  Parameters:
 *                - 
 *
 *  Return Value:
 *      SUCCESS -
 *      FAILURE - failure
 */
int dc_his_init()
{
    int  rv = 0;
    long bucket = DC_HIS_SQL_HASH_BUCKETS;

    if (g_his_sql_kv != NULL)
    {
        hashtable_destroy(g_his_sql_kv);
        g_his_sql_kv = NULL;
    }

    g_his_sql_kv = hashtable_create(bucket);
    if (g_his_sql_kv == NULL)
    {
        elog("error: hashtable_create[%d]!", bucket);
        rv = -1;
        goto _end;
    }

    /*
    olog("HIS-sql container created");
    */

    /* key: node->sql, which is freed in node_free(). */
    hashtable_set_deallocation_function(g_his_sql_kv, NULL, node_free);

    /**************************************************************/

    bucket = DC_HIS_TBL_HASH_BUCKETS;

    if (g_his_tbl_kv != NULL)
    {
        hashtable_destroy(g_his_tbl_kv);
        g_his_tbl_kv = NULL;
    }

    g_his_tbl_kv = hashtable_create(bucket);
    if (g_his_tbl_kv == NULL)
    {
        elog("error: hashtable_create[%d]!", bucket);
        rv = -1;
        goto _end;
    }

    /*
    olog("HIS-tbl container created");
    */

    /* key: table->name, which is not allocated lonely. */
    hashtable_set_deallocation_function(g_his_tbl_kv, NULL, free);

    /**************************************************************/
    g_his_total_num = 0;
    g_his_table_max = DC_HIS_TABLE_MAX_NUM;

_end:
    if (rv)
    {
        if (g_his_sql_kv != NULL)
        {
            hashtable_destroy(g_his_sql_kv);
            g_his_sql_kv = NULL;
        }

        if (g_his_tbl_kv != NULL)
        {
            hashtable_destroy(g_his_tbl_kv);
            g_his_tbl_kv = NULL;
        }
    }

    return rv;
} /* dc_his_init */



/**
 * Try to get row data from local cache.
 *   input: _sql as KEY
 *   output: row data, segment header(column offset), select region of sql, table name of sql.
 *
 *  0 -- success, find data
 * -1 -- failure, exception
 *  1 -- not find
 */
int dc_his_query(char *_sql, void **_row, int *_row_len, void **_seg_header, void **_select, char **_table_name)
{
    int  rv = 0;
    long local_tm = 0;
    long remote_tm = 0;
    long diff_tm = 0;
    char tm[32] = {0};
    void *p = NULL;
    his_node_def_t  *node  = NULL;
    his_table_def_t *table = NULL;

    olog("[%s]", _sql);

    p = hashtable_get(g_his_sql_kv, _sql);
    if (p == NULL)
    {
        /* implies to query from remote(shm) mode */
        olog("Not found from history cache");
        rv = 1;
        goto _end;
    }
    else
    {
        olog("find from history cache");

        node  = (his_node_def_t *)p;
        table = (his_table_def_t *)node->table;
        if (table == NULL)
        {
            elog("error: table is NULL");
            rv = -1;
            goto _end;
        }

        /* 1. get timestamp and check obsolete */
        local_tm  = table->t2;

        /*
        olog("table name: %s", table->name);
        */

        /* remote get, better with lock */
        remote_tm = dc_seg_get_timestamp_by_name(table->name);

        olog("tm-L: %ld", local_tm);
        olog("tm-R: %ld", remote_tm);

        diff_tm = remote_tm - local_tm;

        if (diff_tm > 0)
        {
            olog("%s: already obsolete, %ldus", table->name, diff_tm);

            /* delete table and all related SQL */
            dc_his_remove_table(table);

            rv = 1; 
        }
        else if (diff_tm < 0)
        {
            elog("error: [%s] remote_tm[%ld] < local_tm[%ld]", table->name, remote_tm, local_tm);
            if (remote_tm >  0)
            {
                dctime_format_micro(remote_tm, tm, sizeof(tm));
                elog("error: remote_tm: [%s]", tm);
            }
            if (local_tm >  0)
            {
                dctime_format_micro(local_tm, tm, sizeof(tm));
                elog("error: local_tm:  [%s]", tm);
            }
            rv = -1;
            goto _end;
        }
        else
        {
            olog("nice: find and keep: %d", node->times);

            /* refresh to new */
            node->times++;
            node->t3 = get_time();

            if (dc_his_refresh(node))
            {
                elog("error: dc_his_refresh");
                return -1;
            }

            /* restore context */
            *_row     = node->row;
            *_row_len = node->row_len;
            *_select  = &node->select;

            *_seg_header = &table->seg;
            *_table_name = table->name;
        }
    }

_end:

    return rv;
}


/**
 *  0 -- success, find data
 * -1 -- failure, exception
 *  1 -- not find
 */
int dc_his_query_pre(char *_sql, void **_p, char **_name)
{
    int  rv = 0;
    void *p = NULL;
    his_node_def_t  *node  = NULL;
    his_table_def_t *table = NULL;

    if (g_his_sql_kv == NULL)
    {
        elog("error: not initialized!");
        return -1;
    }

    p = hashtable_get(g_his_sql_kv, _sql);
    if (p == NULL)
    {
        /* mean need to query from remote(shm) mode */
#if DC_DEBUG
        olog("Not found from history cache");
#endif
        rv = 1;
    }
    else
    {
#if DC_DEBUG
        olog("find from history cache");
#endif

        node  = (his_node_def_t *)p;
        table = (his_table_def_t *)node->table;
        if (table == NULL)
        {
            elog("error: table is NULL");
            rv = -1;
            goto _end;
        }

        *_p = p;
        *_name = table->name;
#if DC_DEBUG
        olog("node: [%p]", node);
        olog("name: [%p, %s]", _name, table->name);
#endif

        rv = 0;
    }

_end:

    return rv;
}


/**
 *  0 -- success, find data
 * -1 -- failure, exception
 *  1 -- obsolete
 */
int dc_his_query_post(void *_p, long _remote_tm,
        void **_row, int *_row_len,
        void **_seg_header, void **_select, char **_table_name)
{
    int  rv = 0;
    long local_tm = 0;
    long remote_tm = 0;
    long diff_tm = 0;
    char tm[32] = {0};
    void *p = NULL;
    his_node_def_t  *node  = NULL;
    his_table_def_t *table = NULL;

    p = _p;

    node  = (his_node_def_t *)p;
    table = (his_table_def_t *)node->table;
    if (table == NULL)
    {
        elog("error: table is NULL");
        rv = -1;
        goto _end;
    }

    /* 1. get timestamp and check obsolete */
    local_tm  = table->t2;

    remote_tm = _remote_tm;

#if DC_DEBUG
    olog("tm-L: %ld", local_tm);
    olog("tm-R: %ld", remote_tm);
#endif

    diff_tm = remote_tm - local_tm;

    if (diff_tm > 0)
    {
#if DC_DEBUG
        olog("%s: already obsolete, %ldus", table->name, diff_tm);
#endif

        /* delete table and all related SQL */
        dc_his_remove_table(table);

        rv = 1; 
    }
    else if (diff_tm < 0)
    {
        elog("error: [%s] remote_tm[%ld] < local_tm[%ld]", table->name, remote_tm, local_tm);
        if (remote_tm >  0)
        {
            dctime_format_micro(remote_tm, tm, sizeof(tm));
            elog("error: remote_tm: [%s]", tm);
        }
        if (local_tm >  0)
        {
            dctime_format_micro(local_tm, tm, sizeof(tm));
            elog("error: local_tm:  [%s]", tm);
        }

        /* 2017-11-3: fatal bug: the timestamp of block-A/block-B are different after switch!  */
        /* 2017-11-3: do the clear also after switch! */
        /* delete table and all related SQL */
        dc_his_remove_table(table);

        rv = -1;
    }
    else
    {
#if DC_DEBUG
        olog("nice: tm keeps: %d", node->times);
#endif

        /* refresh to new */
        node->times++;
        node->t3 = get_time();

        if (dc_his_refresh(node))
        {
            elog("error: dc_his_refresh");
            return -1;
        }

        /* restore context */
        *_row     = node->row;
        *_row_len = node->row_len;
        *_select  = &node->select;

        *_seg_header = &table->seg;
        *_table_name = table->name;

        rv = 0;
    }

_end:

    return rv;
}


/**
 * add table define and a SQL node
 * used scene: once remote(shm) query
 */
int dc_his_add(char *_sql, void *_row, int _row_len, void *_seg_header, void *_select, char *_table_name)
{
    int  rv = 0;
    int  to_make_table = 0;
    long local_tm = 0;
    long remote_tm = 0;
    long diff_tm = 0;
    char name[SEGMENT_NAME_MAX+1] = {0};
    char tm[32] = {0};
    void *p     = NULL;
    List *list  = NULL;
    his_table_def_t *table = NULL;

    snprintf(name, sizeof(name), "%s", _table_name);
    dc_str_toupper(name);

    /* get table define */
    p = hashtable_get(g_his_tbl_kv, name);
    if (p == NULL)
    {
#if DC_DEBUG
        olog("table [%s] not find, let's create it.", name);
#endif

        to_make_table = 1;
    }
    else
    {
#if DC_DEBUG
        olog("table define already added: %p", p);
#endif

        table = (his_table_def_t *)p;
        list  = &table->list;

#if DC_DEBUG
        olog("table [%s] counter: %d", table->name, table->num);
#endif

        /* check obsolete  */
        remote_tm = dc_seg_get_timestamp(_seg_header);
        local_tm  = table->t2;

#if DC_DEBUG
        olog("Add-tm-L: %ld", local_tm);
        olog("Add-tm-R: %ld", remote_tm);
#endif

        diff_tm = remote_tm - local_tm;

        if (diff_tm > 0)
        {
#if DC_DEBUG
            olog("%s: already obsolete2, %ldus", table->name, diff_tm);
#endif

            /* delete table and all related SQL */
            dc_his_remove_table(table);

            to_make_table = 1;
        }
        else if (diff_tm < 0)
        {
            elog("error: %s remote_tm[%ld] < local_tm[%ld]", _table_name, remote_tm, local_tm);
            if (remote_tm >  0)
            {
                dctime_format_micro(remote_tm, tm, sizeof(tm));
                elog("error: remote_tm: [%s]", tm);
            }
            if (local_tm >  0)
            {
                dctime_format_micro(local_tm, tm, sizeof(tm));
                elog("error: local_tm:  [%s]", tm);
            }
            rv = -1;
            goto _end;
        }
        else
        {
#if DC_DEBUG
            olog("not obsolete");
#endif
        }
    }


    if (to_make_table)
    {
#if DC_DEBUG
        olog("make new table: %s", _table_name);
#endif
        p = dc_his_make_table(_table_name, _seg_header);
        if (p == NULL)
        {
            elog("error: dc_his_make_table: %s", _table_name);
            rv = -1;
            goto _end;
        }
        else
        {
#if DC_DEBUG
            olog("create table %s succeeds: %p", _table_name, p);
#endif
        }

        table = (his_table_def_t *)p;
        list  = &table->list;
    }

    /* check table already full */
    if (dc_his_table_full(table))
    {
#if DC_DEBUG
        olog("table %s is full", _table_name);
#endif

        /* delete oldest */
        if (dc_his_remove_old_sql(table))
        {
            elog("warn: dc_his_remove_old_sql");
        }
    }

    /* put SQL node: hash && list */
    if (dc_his_put_sql(table, _sql, _row, _row_len, _select))
    {
        elog("error: dc_his_put_sql");
        rv = -1;
        goto _end;
    }

#if DC_DEBUG
    olog("succeed add [%s] to his", _sql);
#endif

_end:

    return rv;
}


/**
 * to free {his_node_def_t}
 */
static void node_free(void *ptr)
{
    his_node_def_t *p = NULL;

    if (ptr != NULL)
    {
        p = (his_node_def_t *)ptr;

        if (p->sql != NULL)
        {
            free(p->sql);
            p->sql = NULL;
        }

        if (p->row != NULL)
        {
            free(p->row);
            p->row = NULL;
        }

        free(ptr);
    }
    else
    {
        elog("warn: %s: already NULL", __func__);
    }
}


/**
 *  check a table reach max number
 */
static int dc_his_table_full(void *_table)
{
    int  rv = 0;
    his_table_def_t *table = NULL;

    table = (his_table_def_t *)_table;

    if (table->num >= g_his_table_max)
    {
#if DC_DEBUG
        olog("full: curr %d >= %d max ", table->num, g_his_table_max);
#endif
        rv = 1;
    }
    else
    {
        rv = 0;
    }

    return rv;
}


/**
 *  delete table and all nodes
 */
static int dc_his_remove_table(void *_table)
{
    int  rv = 0;
    his_node_def_t  *node  = NULL;
    his_table_def_t *table = NULL;
    List            *list  = NULL;

    /* 1. iterate list */
    table = (his_table_def_t *)_table;

    list = &table->list;

    list_for_each_entry(node, list->head, link)
    {
#if DC_DEBUG
        olog("node[%d]: [%s]", table->num, node->sql);
#endif
        hashtable_remove(g_his_sql_kv, node->sql);
        table->num--;
        g_his_total_num--;
    }

    /* 2. remove table */
#if DC_DEBUG
    olog("table: %s removed", table->name);
#endif
    hashtable_remove(g_his_tbl_kv, table->name);

    return rv;
}


/**
 *  delete one node: the oldest
 *  used scene: this table list is full
 */
static int dc_his_remove_old_sql(void *_table)
{
#if DC_DEBUG
    char fmt[32] = {0};
#endif
    his_node_def_t  *node  = NULL;
    his_table_def_t *table = NULL;
    List            *list  = NULL;

    table = (his_table_def_t *)_table;

    list  = &table->list;

    /* get node(the first/oldest) from list */
    node = list_entry(list->head->next, DC_TYPEOF(*node), link);
    if (&node->link == list->head)
    {
        olog("warn: no SQL node");
        return 1;
    }
    else
    {
        /* 1. extract from list */
        list_del(&node->link);

        /* 2. remove from SQL hash */
#if DC_DEBUG
        olog("oldest node: [%s]", node->sql);
        olog("-----------: times[%d]", node->times);
        olog("-----------: born [%s]", dctime_format_micro(node->t3, fmt, sizeof(fmt)));
#endif
        hashtable_remove(g_his_sql_kv, node->sql);
        table->num--;
        g_his_total_num--;
    }

    return 0;
}


/**
 *  put one SQL node to hash and list
 *  used secen: a new SQL
 */
static int dc_his_put_sql(void *_table, char *_sql, void *_row, int _row_len, void *_select)
{
    int rv  = 0;
    int len = 0;
    his_node_def_t  *node  = NULL;
    his_table_def_t *table = NULL;
    List            *list  = NULL;

    table = (his_table_def_t *)_table;

    list  = &table->list;

    len  = sizeof(his_node_def_t);
    node = (his_node_def_t *)calloc(1, len);
    if (node == NULL)
    {
        elog("error: calloc node: %d", len);
        return -1;
    }

    /* 1. sql */
    len = strlen(_sql);
    node->sql = calloc(len+1, 1);
    if (node->sql == NULL)
    {
        elog("error: calloc sql: %d", len);
        rv = -1;
        goto _end;
    }
    else
    {
        memcpy(node->sql, _sql, len);
    }

    /* 2. timestamp */
    node->t3 = get_time();

    /* 3. select region */
    memcpy(&node->select, _select, sizeof(node->select));

    /* 4. times */
    node->times = 1;

    /* 5. table */
    node->table = _table;

    /* 6. rs */
    node->row_len = _row_len;
    node->row = calloc(_row_len, 1);
    if (node->row == NULL)
    {
        elog("error: calloc row: %d", _row_len);
        rv = -1;
        goto _end;
    }
    else
    {
        memcpy(node->row, _row, _row_len);
    }


    /* add to tail */
    list_add(list, &node->link);

    /* add to hashtable */
    if (hashtable_put(g_his_sql_kv, node->sql, node))
    {
        elog("error: hashtable_put failure");
        rv = -1;
        goto _end;
    }
    else
    {
#if DC_DEBUG
        olog("nice: put to SQL hash succeeds[%s]!", node->sql);
#endif
    }

    table->num++;
    g_his_total_num++;

#if DC_DEBUG
    olog("table(%s): %d", table->name, table->num);
    olog("all SQL number: %d", g_his_total_num);
#endif

_end:
    if (rv)
    {
        node_free(node);
    }

    return rv;
}


/**
 * put a table to hash and create its list
 */
static void *dc_his_make_table(char *_table_name, void *_seg_header)
{
    int  rv  = 0;
    int  len = 0;
    void *p     = NULL;
    List *list  = NULL;
    his_table_def_t *table = NULL;

    /*
    olog("==> %s", __func__);
    */

    len = sizeof(his_table_def_t);
    p   = calloc(1, len);
    if (p == NULL)
    {
        elog("error: calloc table failure: %d", len);
        rv = -1;
        goto _end;
    }

    table = (his_table_def_t *)p;

    /* +: table name */
    snprintf(table->name, sizeof(table->name), "%s", _table_name);
    dc_str_toupper(table->name);

    /* +: t2 from shm segment */
    table->t2 = dc_seg_get_timestamp(_seg_header);
#if DC_DEBUG
    olog("t2: %ld", table->t2);
#endif

    /* +: segment header */
    memcpy(&table->seg, _seg_header, sizeof(table->seg));

    /* +: init linked list */
    list = &table->list;
    list->head = &table->head;
    INIT_LIST_HEAD(list->head);

    if (hashtable_put(g_his_tbl_kv, table->name, p))
    {
        elog("error: hashtable_put failure: %s", table->name);
        rv = -1;
        goto _end;
    }
    else
    {
#if DC_DEBUG
        olog("nice: put [%s] to HIS hash succeeds!", table->name);
#endif
    }

_end:

    if (rv)
    {
        if (p != NULL)
        {
            free(p);
            p = NULL;
        }
    }

    return p;
}


/**
 *  refresh the node: move to tail(make newest)
 *  used scene: hit in the hash
 */
static int dc_his_refresh(void *_node)
{
    his_node_def_t  *node  = NULL;
    his_table_def_t *table = NULL;
    List            *list  = NULL;

    node  = (his_node_def_t *)_node;

    table = (his_table_def_t *)node->table;
    if (table == NULL)
    {
        elog("error: invalid table: NULL");
        return -1;
    }

    list  = &table->list;

    /* remove from list */
    list_del(&node->link);

    /* append to tail */
    list_add(list, &node->link);

#if DC_DEBUG
    olog("refresh node to newest");
#endif

    return 0;
}


static void dc_his_dump_table(void *_table)
{
    int  i = 0;
    char fmt[36] = {0}; 
    his_node_def_t  *node  = NULL;
    his_table_def_t *table = NULL;
    List            *list  = NULL;

    table = (his_table_def_t *)_table;

    olog("table: [%s] [%d]", table->name, table->num);

    list  = &table->list;

    list_for_each_entry(node, list->head, link)
    {
        olog("--[%04d]: [%s]", i, node->sql);
        olog("--born  : [%s]", dctime_format_micro(node->t3, fmt, sizeof(fmt)));
        olog("--times : [%d]", node->times);
        olog("--------");
        i++;
    }

    return;
}


void dc_his_dump()
{
    int i = 0;
    int buck = 0;
    hashpair_t *pair = NULL;
    his_table_def_t *table = NULL;

    olog("########################################");
    olog("########################################");

    buck = hashtable_get_num_buckets(g_his_tbl_kv);

    for (i = 0; i < buck; i++)
    {
        pair = g_his_tbl_kv->bucketArray[i];

        while (pair != NULL)
        {
            table =  (his_table_def_t *)pair->value;

            olog("dump %04d: [%s]", i, pair->key);

            dc_his_dump_table(table);

            pair = pair->next;
        }
    }

    return;
}


int dc_his_hit(char *_sql)
{
    int   rv = 0;
    int   len = 0;

    int   row_len = 0;
    void *row = NULL;
    void *seg_header = NULL;
    void *select = NULL;
    char *name = NULL;

    olog("++++++++++++++++++++++++++++++++++++++++");
    olog("++++++++++++++++++++++++++++++++++++++++");
    olog("++++++++++++++++++++++++++++++++++++++++");

    rv = dc_his_query(_sql, &row, &row_len, &seg_header, &select, &name);
    if (rv < 0)
    {
        elog("error: dc_his_query: %s", _sql);
        return -1;
    }
    else if (rv)
    {
        /* not find */

        /* +: do remote query */
        olog("do remote query");
        len = strlen(_sql);
        rv = dc_query_sql_select(_sql, len);
        if (rv < 0)
        {
            elog("error: dc_query_sql_select: %d", rv);
            return -1;
        }
        else if (rv == 0)
        {
            olog("sorry, can't find data");
        }
        else 
        {
            olog("query get %d rows", rv);

            olog("----------------------------------------");

            /* +: add to his */
            if (rv == 1)    /* only for one row case */
            {
                if (dc_rs_get_row2(&row, &row_len))
                {
                    elog("error: dc_rs_get_row2");
                    return -1;
                }

                seg_header = dc_rs_get_seg_header();

                select = dc_sql_get_select();

                name = dc_sql_get_table_name();

                rv = dc_his_add(_sql, row, row_len, seg_header, select, name);
                if (rv)
                {
                    elog("error: dc_his_add");
                    return -1;
                }
                else
                {
                    olog("dc_his_add succeeds");
                }
            }
            else
            {
                olog("%d: more than one row, don't save", rv);
            }
        }
    }
    else
    {
        /* find */
        olog("nice: query remote(shm) succeeds");

        /* restore to sql && rs */
        dc_sql_set_select(select);
        dc_sql_set_table_name(name);

        if (dc_rs_restore_context(row, row_len, seg_header))
        {
            elog("error: dc_rs_restore_context");
            return -1;
        }

        olog("restore all data");
    }


    return 0;
}


#if RUN_HIS

static int dc_his_function_test()
{
    int rv = 0;
    char sql[256] = {0};

    olog("----------------------------------------");
    olog("sql1: TBL_LOG_COUNTER"); 
    snprintf(sql, sizeof(sql), "%s", 
            "SELECT INST_DATE_TIME, SUCC_TOTAL_NUM  FROM TBL_LOG_COUNTER "
            "WHERE PAN ='6216920000008264' AND TXN_TYPE=  'ZHXXYZ' "
            " AND TXN_DATE = '20170509'");
    if (dc_his_hit(sql))
    {
        elog("error: dc_his_hit: %s", sql);
        return -1;
    }

    olog("----------------------------------------");
    olog("sql2: TBL_LOG_COUNTER"); 
    snprintf(sql, sizeof(sql), "%s", 
            "SELECT INST_DATE_TIME, SUCC_TOTAL_NUM  FROM TBL_LOG_COUNTER "
            "WHERE PAN ='6216920000008264' AND TXN_TYPE=  'ZHXXYZ' "
            " AND TXN_DATE = '20170509' ");
    if (dc_his_hit(sql))
    {
        elog("error: dc_his_hit: %s", sql);
        return -1;
    }

    olog("----------------------------------------");
    olog("sql3: TBL_CFG_LINE"); 
    snprintf(sql, sizeof(sql), "%s", 
            "SELECT REMOTE_ADDR, REMOTE_PORT  FROM TBL_CFG_LINE "
            "WHERE SRV_NAME ='CommCredit' AND REMOTE_SVC_NAME =  'YL03' "
            " AND PROTOCOL  = 'XML' ");
    if (dc_his_hit(sql))
    {
        elog("error: dc_his_hit: %s", sql);
        return -1;
    }


    olog("----------------------------------------");
    olog("sql4: TBL_CFG_TXN"); 
    snprintf(sql, sizeof(sql), "%s",
            "SELECT DSP, LIVE_TIME FROM TBL_CFG_TXN "
            "WHERE TXN_NUM =  '1073' ");
    if (dc_his_hit(sql))
    {
        elog("error: dc_his_hit: %s", sql);
        return -1;
    }


    olog("----------------------------------------");
    olog("sql1: TBL_LOG_COUNTER, to hit!"); 
    snprintf(sql, sizeof(sql), "%s", 
            "SELECT INST_DATE_TIME, SUCC_TOTAL_NUM  FROM TBL_LOG_COUNTER "
            "WHERE PAN ='6216920000008264' AND TXN_TYPE=  'ZHXXYZ' "
            " AND TXN_DATE = '20170509'");
    if (dc_his_hit(sql))
    {
        elog("error: dc_his_hit: %s", sql);
        return -1;
    }


    olog("----------------------------------------");
    olog("sql5: TBL_LOG_COUNTER, new!"); 
    snprintf(sql, sizeof(sql), "%s", 
            "SELECT INST_DATE_TIME, SUCC_TOTAL_NUM  FROM TBL_LOG_COUNTER "
            "WHERE PAN ='6216920000008264' AND TXN_TYPE=  'ZHXXYZ' "
            " AND TXN_DATE = '20170509'     ");
    if (dc_his_hit(sql))
    {
        elog("error: dc_his_hit: %s", sql);
        return -1;
    }

    olog("----------------------------------------");
    olog("sql6: TBL_LOG_COUNTER, new and pop!"); 
    snprintf(sql, sizeof(sql), "%s", 
            "SELECT INST_DATE_TIME, SUCC_TOTAL_NUM  FROM TBL_LOG_COUNTER "
            "WHERE PAN ='6216920000008264' AND TXN_TYPE=  'ZHXXYZ' "
            " AND TXN_DATE = '20170509'      ");
    if (dc_his_hit(sql))
    {
        elog("error: dc_his_hit: %s", sql);
        return -1;
    }

    olog("----------------------------------------");
    dc_his_dump();

    /* remote updated */

    olog("----------------------------------------");
    olog("sql5: TBL_LOG_COUNTER, table obsolete!"); 
    snprintf(sql, sizeof(sql), "%s", 
            "SELECT INST_DATE_TIME, SUCC_TOTAL_NUM  FROM TBL_LOG_COUNTER "
            "WHERE PAN ='6216920000008264' AND TXN_TYPE=  'ZHXXYZ' "
            " AND TXN_DATE = '20170509'     ");
    if (dc_his_hit(sql))
    {
        elog("error: dc_his_hit: %s", sql);
        return -1;
    }

    olog("----------------------------------------");
    olog("sql1: TBL_LOG_COUNTER, already deleted!"); 
    snprintf(sql, sizeof(sql), "%s", 
            "SELECT INST_DATE_TIME, SUCC_TOTAL_NUM  FROM TBL_LOG_COUNTER "
            "WHERE PAN ='6216920000008264' AND TXN_TYPE=  'ZHXXYZ' "
            " AND TXN_DATE = '20170509'");
    if (dc_his_hit(sql))
    {
        elog("error: dc_his_hit: %s", sql);
        return -1;
    }

    olog("----------------------------------------");
    dc_his_dump();

    /*********************************************************************/

    return rv;
}


int main(int argc, char *argv[])
{
    int  rv  = 0;
    int  err = 0;
    int  shm_id   = 0;
    int  shm_key  = 1222;
    long shm_size = 400000;
    void *shm_addr = NULL;
    long flag = 0;

    (void)argc;
    (void)argv;

    olog("his begin");

    if (dc_his_init())
    {
        elog("error: dc_his_init");
        return -1;
    }

    if (dc_query_init())
    {
        elog("error: dc_query_init");
        return -1;
    }

    /* tree */
    rb_tree_register();

    /* shm */
    shm_id = dc_shm_init(shm_key, shm_size, &shm_addr, flag, &err);
    if (shm_id == FAILURE)
    {
        elog("error: dc_shm_init: %d", shm_id);
        return -1;
    }
    olog("shm addr: %p", shm_addr);

    if (dc_his_function_test())
    {
        elog("error: dc_his_function_test");
        return -1;
    }


    olog("his end");
    return rv;
}
#endif

/* his.c */
