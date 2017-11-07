#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <errno.h>
#include <assert.h>

#include "type.h"
#include "log.h"
#include "tm.h"

#include "shm.h"
#include "blk.h"
#include "tree.h"
#include "seg.h"
#include "sec.h"
#include "load.h"
#include "sql.h"
#include "rs.h"
#include "put.h"
#include "where.h"
#include "query.h"

#include "sysloc.h"
#include "pladb.h"
#include "mydb.h"



/* ================================================================= */

static int dc_query_by_key(void *_seg_addr, int _idx,
        char *_key,     int _key_len,
        void **_data,   int *_data_len, int *_id);

int dc_query_get_column_value(void *_seg_addr, void *_row, char *_col_name, void **_value, int *_max_len, int *_type);

int dc_query_check_where_clause_match(void *_seg_addr, void *_row);

static int dc_query_get_data_inner(void *_seg_addr, void *_row, int _data_id , void * _data , int _size);
static int dc_query_full_table_search(void *_seg_addr);
static int dc_query_sql_select_post_sub(void *_seg_addr);
static int dc_query_dump_row_inner(void *_seg_addr, void *_row);
static int dc_query_get_all_rows(void *_seg_addr);

/* ================================================================= 
   The invoke order must be:
   1. dc_query_init()
   2. 
   ================================================================= */


/*
 *  Function:  dc_query_init
 *
 *  Parameters:
 *      _seg_addr - segment head address
 *
 *  Return Value:
 *      SUCCESS - 
 *      FAILURE - failure
 */
int dc_query_init()
{

    /* sql.c -- local */
    if (dc_sql_init())
    {
        elog("error: dc_sql_init");
        return -1;
    }

    /* rs.c -- local */
    if (dc_rs_init())
    {
        elog("error: dc_rs_init");
        return -1;
    }

    return 0;
} /* dc_query_init */


int dc_query_sql_select_pre(void *_blk_addr, char * _sql, int _sql_len)
{
    int  len = 0;
    int  seg_idx = 0;
    char *table_name = NULL;

    /* must be called before dc_sql_analyze() */
    dc_sql_reset();

    len = dc_sql_clear_semicolon(_sql, _sql_len);
    if (len <= 0)
    {
        elog("error: dc_sql_clear_semicolon: too short sql: [%s]", _sql);
        return -1;
    }

    if (dc_sql_analyze(_sql, len))
    {
        elog("error: dc_sql_analyze");
        return -1;
    }

    /* clear previous result set */
    dc_rs_reset();

    table_name = dc_sql_get_table_name();

    seg_idx = dc_blk_get_segment_index(_blk_addr, table_name);
    if (seg_idx == -1)
    {
        elog("error: dc_blk_get_segment_index: %s", table_name);
        return -1;
    }

#if DC_DEBUG
    olog("%s => [%02d]th", table_name, seg_idx);
#endif

    return seg_idx;
}


int dc_query_sql_select_post(void *_blk_addr, int _seg_idx)
{
    int  k = 0;
    int  rv = 0;
    int  len = 0;
    int  sum = 0;
    int  num = 0;
    int  where_len = 0;
    char *p = NULL;
    char *where_region = NULL;
    void *seg_addr = NULL;
    void *where_list = NULL;
    segment_header_t *seg_header = NULL;

    /* locate */
    seg_addr = dc_blk_get_segment_addr(_blk_addr, _seg_idx);
    if (seg_addr == NULL)
    {
        elog("error: dc_blk_get_segment_addr: %s", _seg_idx);
        return -1;
    }
#if DC_DEBUG
    olog("seg_addr: %p", seg_addr);
#endif

    /* save to local */
    dc_rs_set_seg_header(seg_addr);

    seg_header = (segment_header_t *)seg_addr;

    /* convert '*' to columns for SELECT */
    if (dc_sql_get_select_num() == 0)
    {
        dc_sql_set_select(&seg_header->column_num);
#if DC_DEBUG
        olog("convert SELECT '*' ...");
#endif
    }

    /* get WHERE region */
    if (dc_sql_get_where(&where_region, &where_len) < 0)
    {
        elog("error: dc_sql_get_where");
        return -1;
    }
#if DC_DEBUG
    olog("WHERE: [%.*s]", where_len, where_region);
#endif

    /* no where */
    if (where_region == NULL)
    {
#if DC_DEBUG
        olog("no where region");
#endif
        if ((rv = dc_query_get_all_rows(seg_header)) < 0)
        {
            elog("error: dc_query_get_all_rows: %d", rv);
            return -1;
        }
#if DC_DEBUG
        olog("total [%d] rows", rv);
#endif

        if (dc_rs_uniq() < 0)
        {
            elog("error: dc_rs_uniq");
            return -1;
        }

        return rv;
    }


    /* iterate WHERE list */
    where_list = dc_where_analyze(where_region, where_len);
    if (where_list == NULL)
    {
        elog("error: dc_where_analyze");
        return -1;
    }

    num = dc_where_iterator_init(where_list);

    for (k = 0; k < num; k++)
    {
        p = dc_where_iterator_next();
        if (p == NULL)
        {
            elog("error: dc_where_iterator_next");
            rv = -1;
            goto _end;
        }
        len = strlen(p);
#if DC_DEBUG
        olog("sub[%d]: [%s](%d)", k, p, len);
#endif

        /* MUST: clear previous data */
        dc_sql_reset_where();

        rv = dc_sql_analyze_where_and(p, len);
        if (rv)
        {
            elog("error: dc_sql_analyze_where_and: %d", rv);
            rv = -1;
            goto _end;
        }

        rv = dc_query_sql_select_post_sub(seg_addr);
        if (rv < 0)
        {
            elog("error: dc_query_sql_select_post_sub: %d", rv);
            rv = -1;
            goto _end;
        }
        else if (rv > 0)
        {
            sum += rv;
#if DC_DEBUG
            olog("accumulate: [%d] => [%d]", rv, sum);
#endif
        }
        else
        {
#if DC_DEBUG
            olog("[%s]: not found data", p);
#endif
        }
    }

_end:
    if (where_list != NULL)
    {
        if (dc_where_iterator_end(where_list))
        {
            elog("error: dc_where_iterator_end");
            return -1;
        }
    }

    if (rv >= 0)
    {
        rv = sum;
    }

    return rv;
}


static int dc_query_sql_select_post_sub(void *_seg_addr)
{
    int  i = 0;
    int  rv = 0;
    int  len = 0;
    int  row_id = -1;
    int  matched = 0;
    int  key_len = 0;
    int  data_len = 0;
    int  index_num = 0;
    int  index_column_num = 0;
    int  where_column_num = 0;
    char index[INDEX_COLUMNS_MAX+1] = {0};
    char key[512] = {0};
    void *p_row_data = NULL;
    void *seg_addr = NULL;
    segment_header_t *seg_header = NULL;

    seg_addr = _seg_addr;
    if (seg_addr == NULL)
    {
        elog("error: invalid input");
        return -1;
    }
#if DC_DEBUG
    olog("seg_addr: %p", seg_addr);
#endif

    seg_header = (segment_header_t *)seg_addr;

    index_num = seg_header->index_num;
#if DC_DEBUG
    olog("index number: %d", index_num);
#endif

    for (i = 0; i < index_num; i++)
    {
        len = snprintf(index, sizeof(index), "%s", seg_header->index[i].index_name);
#if DC_DEBUG
        olog("index[%d] ==> [%s]", i, index);
#endif
        index_column_num = dc_sql_check_use_this_index(index, len);
        if (index_column_num > 0)
        {
#if DC_DEBUG
            olog("nice: let's use this index");
#endif
            key_len = dc_sql_combine_index_value(index, len, key, sizeof(key));
            if (key_len <= 0)
            {
                elog("error: dc_sql_combine_index_value: %d", key_len);
                return -1;
            }
#if DC_DEBUG
            olog("[%s => %d(%d), %s]", index, key_len, strlen(key), key);
#endif

            /* dc_seg_dump_index_area(seg_addr, i); */

            data_len    = 0;
            p_row_data  = NULL;
            row_id      = -1;
            rv = dc_query_by_key(seg_addr, i, key, key_len, &p_row_data, &data_len, &row_id);
            if (rv < 0)
            {
                elog("error: dc_query_by_key: [%s, %d]", key, key_len);
            }
            else if (rv == 0)
            {
#if DC_DEBUG
                olog("ohho: not found, dc_query_by_key: [%s, %d]", key, key_len);
#endif
                rv = 0;
            }
            else
            {
#if DC_DEBUG
                olog("query succeeds: %s", key);
                olog("-- data len: [%d], [%d]", data_len, seg_header->row_size);
#endif

                /* check where all columns */
                where_column_num = dc_sql_where_columns();
#if DC_DEBUG
                olog("where column number: %d", where_column_num);
                olog("index column number: %d", index_column_num);
#endif

                matched = 0;
                if (where_column_num > index_column_num)
                {
#if DC_DEBUG
                    olog("do full where check under index");
#endif
                    rv = dc_query_check_where_clause_match(seg_addr, p_row_data);
                    if (rv < 0)
                    {
                        elog("error: dc_query_check_where_clause_match");
                        return -1;
                    }
                    else if (rv == 0)
                    {
                        olog("sorry, can't match");
                        rv = 0;
                    }
                    else
                    {
#if DC_DEBUG
                        olog("nice, all column matches!");
                        dc_query_dump_row_inner(seg_addr, p_row_data);
#endif

                        rv = 1;
                        matched = 1;
                    }
                }
                else
                {
                    rv = 1;
                    matched = 1;
#if DC_DEBUG
                    olog("just use index matches");
                    dc_query_dump_row_inner(seg_addr, p_row_data);
#endif
                }

                if (matched)
                {
                    /*
                    olog("let's save row data");
                    */
                    if (dc_rs_add_row(p_row_data, data_len, row_id))
                    {
                        elog("error: dc_rs_add_row");
                        return -1;
                    }
                }

            } /* found */

            /*
             * unique INDEX means must be found,
             * so no need to try next loop.
             */
            break;
        }
        else
        {
#if DC_DEBUG
            olog("sorry: can't use index: [%s]", index);
#endif
        }
    } /* for */

    if (i == index_num)
    {
        /* full table scan */
#if DC_DEBUG
        olog("Can't use index");
        olog("Let's search by full table scan");
#endif
        rv = dc_query_full_table_search(seg_addr);
        if (rv < 0)
        {
            elog("error: dc_query_full_table_search");
            return -1;
        }
        else if (rv == 0)
        {
#if DC_DEBUG
            olog("no data");
#endif
        }
        else
        {
#if DC_DEBUG
            olog("there are %d rows found", rv);
#endif
        }
    }

    if (dc_rs_uniq() < 0)
    {
        elog("error: dc_rs_uniq");
        return -1;
    }

    return rv;
}


/* for no where case */
static int dc_query_get_all_rows(void *_seg_addr)
{
    int i = 0;
    List    *list = NULL;
    dc_sec_t *p = NULL;
    segment_header_t *seg_header = NULL;

    seg_header = (segment_header_t *)_seg_addr;

    list = &seg_header->data_used_list;

    list_for_each_entry(p, list->head, link)
    {
        if (dc_rs_add_row(p->row, p->len, p->id))
        {
            elog("error: dc_rs_add_row");
            return -1;
        }

        i++;
    }

    return i;
}


/*
 *  Function: 
 *      execute select sql
 *      SELECT FAIL_CONTINUE_NUM FROM TBL_LOG_COUNTER WHERE PAN='1' AND TXN_TYPE='2';
 *      SELECT xxx FROM table WHERE (A = x OR A = y) AND B = z;
 *      SELECT xxx FROM table WHERE (A=x1 AND B = y1) OR (A=x2 AND B=y2) OR (...)
 *
 *  Parameters:
 *     _ - 
 *
 *  Return Value:
 *      SUCCESS - 0 not find, n>0 find
 *      FAILURE - -1
 */
int dc_query_sql_select(char * _sql, int _sql_len)
{
    int  rv   = 0;
    int  seg_idx = 0;
    void *blk_addr = NULL;

    blk_addr = dc_blk_get_read_addr();
    if (blk_addr == NULL)
    {
        elog("error: dc_blk_get_read_addr is NULL");
        return -1;
    }
    olog("blk addr: %p", blk_addr);


    seg_idx = dc_query_sql_select_pre(blk_addr, _sql, _sql_len);
    if (seg_idx < 0)
    {
        elog("error: dc_query_sql_select_pre");
        return -1;
    }

    rv = dc_query_sql_select_post(blk_addr, seg_idx);
    if (rv < 0)
    {
        elog("error: dc_query_sql_select_post: %d", rv);
    }
    else
    {
#if DC_DEBUG
        olog("dc_query_sql_select_post: %d", rv);
#endif
    }

    return rv;
}


/*
 *  Function: 
 *      full table search
 *      only be called after sql-analyze
 *
 *  Parameters:
 *     _ - 
 *
 *  Return Value:
 *      SUCCESS - >0 rows, 0:no data.
 *      FAILURE - -1
 */
static int dc_query_full_table_search(void *_seg_addr)
{
    int  i = 0;
    int  rv = 0;
    int  n = 0;
    int  data_len = 0;
    void *p_row_data = NULL;
    segment_header_t *seg_header = NULL;
    List             *list = NULL;
    dc_sec_t         *p = NULL;

    /*
    olog("-->>>>%s", __func__);
    */

    seg_header = (segment_header_t *)_seg_addr;

#if DC_DEBUG
    olog("-- row size: [%d]", seg_header->row_size);
#endif

    list = &seg_header->data_used_list;

    list_for_each_entry(p, list->head, link)
    {
#if DC_DEBUG
        olog("[%d] => [%d, %s]", i, p->len, p->row);
#endif

        data_len   = p->len;
        p_row_data = p->row;

        /*
        dc_query_dump_row_inner(_seg_addr, p_row_data);
        */

#if DC_DEBUG
        olog("do full where check row");
#endif
        rv = dc_query_check_where_clause_match(_seg_addr, p_row_data);
        if (rv < 0)
        {
            elog("error: dc_query_check_where_clause_match");
            return -1;
        }
        else if (rv == 0)
        {
#if DC_DEBUG
            olog("sorry, row[%d] can't match", i);
#endif
        }
        else
        {
#if DC_DEBUG
            dc_query_dump_row_inner(_seg_addr, p_row_data);
#endif

            /*
            olog("save this row to RESULT SET");
            */
            if (dc_rs_add_row(p_row_data, data_len, p->id))
            {
                elog("error: dc_rs_add_row");
                return -1;
            }
            n++;
        }

        i++;
        /*
        olog("----------------------------------------");
        */
    }

    return n;
}



/*
 *  Function: 
 *      check whether all where columns matches
 *
 *  Parameters:
 *     _xxx -
 *
 *  Return Value:
 *      SUCCESS - 1
 *      FAILURE - -1
 */
int dc_query_check_where_clause_match(void *_seg_addr, void *_row)
{
    int  table_value_max = 0;
    int  where_value_max = 0;
    int  column_type     = 0;
    char *p_where_column = NULL;
    void *p_table_value = NULL;
    void *p_where_value = NULL;

    /*
    olog("full where column check");
    */

    dc_sql_where_iterator_init();

    while ((p_where_column = dc_sql_where_iterator_next()) != NULL)
    {
        /*
        olog("where column [%s]", p_where_column);
        */

        /* get table data */
        if (dc_query_get_column_value(_seg_addr, _row, p_where_column,
                    &p_table_value, &table_value_max, &column_type))
        {
            elog("error: dc_query_get_column_value: %s failure", p_where_column);
            return -1;
        }
        /*
        olog("table -- [%s => %s, %d]", p_where_column, p_table_value, table_value_max);
        */

        /* get where data */
        if (dc_sql_get_where_value(p_where_column, &p_where_value, &where_value_max))
        {
            elog("error: dc_sql_get_where_value: %s failure", p_where_column);
            return -1;
        }
        /*
        olog("where -- [%s => %s, %d]", p_where_column, p_where_value, where_value_max);
        */

        if (where_value_max > table_value_max)
        {
            olog("sorry, too long can't match");
            return 0;
        }

        if (strcmp(p_where_value, p_table_value) != 0)
        {
#if DC_DEBUG
            olog("sorry, column '%s' can't match", p_where_column);
            olog("---- [%s] user", p_where_value);
            olog("---- [%s] db", p_table_value);
#endif
            return 0;
        }

#if DC_DEBUG
        olog("nice: column [%s] match", p_where_column);
#endif
    } /* while */

    return 1;
}


/*
 *  Function: 
 *      set column value before execute sql
 *      double roll_amt=0.00;
 *      DBPutData( 0, FAPDECIMAL, &roll_amt);
 *      DBPutData( 1, FAPCHAR, chnl_seq_no);
 *
 *  Parameters:
 *
 *  Return Value:
 *      SUCCESS - 0
 *      FAILURE - -1
 */
int dc_query_put_data(int _data_id , int _data_type , void * _data )
{

    if (dc_put_one(_data_id, _data_type, _data))
    {
        elog("error: dc_put_one: %d", _data_id);
        return -1;
    }

    return 0;
}


/*
 *  Function: 
 *      get column value
 *      DBGetData( 0, &idx, sizeof( idx ) );
 *      DBGetData( 1, val, sizeof( val ) );
 *
 *  Parameters:
 *
 *  Return Value:
 *      SUCCESS - 0
 *      FAILURE - -1
 */
int dc_query_get_data(int _data_id , void * _data , int _size)
{
    int  agg = 0;
    int  counter = 0;
    int  value_len = 0;
    void *seg_addr  = NULL;
    void *row = NULL;

    seg_addr = dc_rs_get_seg_header();

    agg = dc_sql_is_aggregation();
    if (agg < 0)
    {
        elog("error: dc_sql_is_aggregation");
        return -1;
    }
    else if (agg)
    {
        /* count(1) gramma */
        counter   = dc_rs_get_num();
        if (_size == sizeof(int))
        {
            *(int *)_data = counter;
            value_len = _size;
        }
        else if (_size == sizeof(long))
        {
            *(long *)_data = counter;
            value_len = _size;
        }
        else
        {
            elog("erorr: invalid data type: %d", _size);
            return -1;
        }
#if DC_DEBUG
        olog("aggregation: %d", counter);
#endif
    }
    else
    {
        /* normal gramma */
        row = dc_rs_get_curr_row();
        if (row == NULL)
        {
            elog("error: no row");
            return -1;
        }

        if (dc_query_get_data_inner(seg_addr, row, _data_id, _data, _size))
        {
            elog("error: dc_query_get_data_inner");
            return -1;
        }
    }

    return 0;
}


/*
 *  Function: 
 *      get column value from row by column name
 *
 *  Parameters:
 *
 *  Return Value:
 *      SUCCESS - 0
 *      FAILURE - -1
 */
static int dc_query_get_data_inner(void *_seg_addr, void *_row, int _data_id , void * _data , int _size)
{
    char *p_column = NULL;
    void *p_value  = NULL;
    int   value_len = 0;
    int   data_len  = 0;
    int   column_type = 0;

    p_column = dc_sql_get_select_column(_data_id);
    if (p_column == NULL)
    {
        elog("error: dc_sql_get_select_column: %d", _data_id);
        return -1;
    }
    else
    {
#if DC_DEBUG
        olog("column name: %s", p_column);
#endif
        if (dc_query_get_column_value(_seg_addr, _row, p_column,
                    &p_value, &value_len, &column_type))
        {
            elog("error: dc_query_get_column_value: %s failure", p_column);
            return -1;
        }

        data_len = (_size < value_len ? _size : value_len);
        memcpy(_data, p_value, data_len);

        /* set trailing zero explicitly 2017-10-23 */
        switch (column_type)
        {
            /* both ORACLE and DB2 */
            case DFFAPCHAR:
            case DFFAPVCHAR2:
            case DFFAPDATE:
#if DB2
            case DFFAPVCHAR:
            case DFFAPLONG:
            case DFFAPDTIME:
            case DFFAPINTERVAL:
            case DFFAPTEXT:
            case DFFAPBYTES:
#endif
                if (data_len < _size)
                {
#if DC_DEBUG
                    olog("[%d]: set to zero: %d < %d(size)!", column_type, data_len, _size);
#endif
                    ((char *)_data)[data_len] = 0x0; 
                }
                break;
            default:
                break;
        }

        /*
        olog("column(%s) => len(%d)", p_column, value_len);
        */

    }

    return 0;
}




/*
 *  Function: 
 *      search tree by key
 *
 *  Parameters:
 *     _table_name- table name
 *
 *  Return Value:
 *      SUCCESS - 0 not found, 1 found
 *      FAILURE - -1
 */
static int dc_query_by_key(void *_seg_addr, int _idx,
        char *_key,     int _key_len,
        void **_data,   int *_data_len, int *_id)
{
    int  rv = 0;
    int  index_num = 0;
    segment_header_t  *seg_header = NULL;
    rb_tree_t         *tree = NULL;
    rb_tree_node_t    *node = NULL;

    seg_header = (segment_header_t *)_seg_addr;

    index_num = seg_header->index_num;
    if (_idx < 0 || _idx >= index_num)
    {
        elog("error: invalid input: %d", _idx);
        return -1;
    }

    tree = &seg_header->tree[_idx];
#if DC_DEBUG
    olog("tree[%d]: there are %d nodes", _idx, tree->used_num);
#endif

    node = rb_tree_find_by_string(tree, _key, _key_len); 
    if (node != NULL)
    {
        *_data      = dc_sec_get_row(node->data_unit);
        *_data_len  = node->data_len;
        *_id        = dc_sec_get_id(node->data_unit);
#if DC_DEBUG
        olog("sea: [%s => %d, %s]", node->key_val, *_data_len, *_data);
#endif

        rv = 1;
    }
    else
    {
#if DC_DEBUG
        olog("sea: not found");
#endif
        rv = 0;
    }

    return rv;
} /* dc_query_by_key */


int dc_query_dump_curr_row()
{
    void *seg_addr  = NULL;
    void *row = NULL;

    seg_addr = dc_rs_get_seg_header();

    row = dc_rs_get_curr_row();
    if (row == NULL)
    {
        elog("error: no row");
        return -1;
    }

    dc_query_dump_row_inner(seg_addr, row);

    return 0;
}


/*
 *  Function: 
 *      dump a row as table column
 *
 *  Parameters:
 *     _table_name- table name
 *
 *  Return Value:
 *      SUCCESS - index number
 *      FAILURE - -1
 */
int dc_query_dump_row_inner(void *_seg_addr, void *_row)
{
    int  i = 0;
    int  column_num = 0;
    segment_header_t  *seg_header = NULL;
    table_def_t       *p = NULL;

    seg_header = (segment_header_t *)_seg_addr;

    column_num = seg_header->column_num;
    /*
    olog("column number: %d", column_num);
    */

    for (i = 0; i < column_num; i++)
    {
        p = &seg_header->column[i];
        printf("^%20s::%-2d -- ", p->column_name, p->column_type);
        switch(p->column_type)
        {
            case DFFAPVCHAR2:
#if DB2
            case DFFAPVCHAR:
#endif
            case DFFAPCHAR:
                printf("%s", (char *)_row+p->column_offset);
                break;
            case DFFAPINT:
            case DFFAPINTEGER:
                printf("%ld", *(long *)(_row+p->column_offset));
                break;
            case DFFAPDECIMAL:
                printf("omit: DECIMAL/NUMBER");
                break;
            default:
                printf("unknown type: %d", p->column_type);
                break;
        }
        printf("$\n");
    }

    return 0;
}


/* dump a segment rows */
int dc_query_dump_segment(void *_seg_addr)
{
    int i = 0;
    List    *list = NULL;
    dc_sec_t    *p = NULL;
    segment_header_t    *seg_header = NULL;

    seg_header = (segment_header_t *)_seg_addr;

    list = &seg_header->data_used_list;

    list_for_each_entry(p, list->head, link)
    {
        printf("row:   [%04d]=================================\n", i);
        dc_query_dump_row_inner(_seg_addr, p->row);

        i++;
    }
    printf("total: [%04d]=================================\n", i);

    return 0;
}


/* dump a segment info */
int dc_query_dump_segment_info(void *_seg_addr)
{
    int i = 0;
    int n = 0;
    char tm[32] = {0};
    segment_header_t    *seg_header = NULL;

    seg_header = (segment_header_t *)_seg_addr;

    dctime_format_micro(seg_header->t2, tm, sizeof(tm));

    printf("rows-num:  %d\n", seg_header->rows_used);
    printf("rows-ava:  %d\n", seg_header->rows_free);
    printf("timestamp: %s\n", tm);
    printf("seg-size:  %ld(bytes)\n",
            seg_header->size_allocated + seg_header->size_available + sizeof(segment_header_t));

    /* INDEX */
    n = seg_header->index_num;
    if (n == 0)
    {
        printf("no unique index\n");
    }
    else if (n == 1)
    {
        printf("has 1 unique index, as:\n");
    }
    else
    {
        printf("has %d unique indexes, as:\n", n);
    }
    for (i = 0; i < n; i++)
    {
        printf("INDEX[%d]: %s\n", i, seg_header->index[i].index_name);
    }

    /* TABLE STRUCTURE */
#if 0
    n = seg_header->column_num;
    printf("has [%d] columns\n", n);
    for (i = 0; i < n; i++)
    {
        printf("COLUMN[%02d]: %s\n", i, seg_header->column[i].column_name);
    }
#endif

    return 0;
}


/* dump a segment column */
int dc_query_dump_segment_column(void *_seg_addr)
{
    int i = 0;
    int n = 0;
    int len = 0;
    int max = -1;
    table_def_t         *p = NULL;
    segment_header_t    *seg_header = NULL;

    seg_header = (segment_header_t *)_seg_addr;


    /* TABLE STRUCTURE */
    n = seg_header->column_num;
    for (i = 0; i < n; i++)
    {
        p = &seg_header->column[i];
        len = strlen(p->column_name);
        if (len > max)
        {
            max = len;
        }
    }

    /* TABLE STRUCTURE */
    n = seg_header->column_num;
    printf("has [%d] columns\n", n);
    for (i = 0; i < n; i++)
    {
        p = &seg_header->column[i];
        printf("[%02d]%-*s -- type(%d)\t-- max-length(%d)\n",
                i, max, p->column_name, p->column_type, p->column_max_length);
    }

    return 0;
}


/*
 *  Function: 
 *      query column by column name
 *      XXX: FIXME: improve performance
 *
 *  Parameters:
 *     _seg_addr - table name
 *
 *  Return Value:
 *      SUCCESS - 0
 *      FAILURE - -1
 */
int dc_query_get_col_idx(void *_seg_addr, char *_col_name)
{
    int i   = 0;
    segment_header_t    *seg_header = NULL;

    seg_header = (segment_header_t *)_seg_addr;

    for (i = 0; i < seg_header->column_num; i++)
    {
        if (strcasecmp(_col_name, seg_header->column[i].column_name) == 0)
        {
            /*
            olog("column index: [%s => %d]", _col_name, i);
            */
            return i;
        }
        else
        {
            /*
            olog("not match: [%s, %s]", _col_name, seg_header->column[i].column_name);
            */
        }
    }

    elog("error: not found: '%s'", _col_name);

    return -1;
} /* dc_query_get_col_idx */


/*
 *  Function: 
 *      get vlaue by column name
 *
 *  Parameters:
 *     _seg_addr - table name
 *
 *  Return Value:
 *      SUCCESS - 0
 *      FAILURE - -1
 */
int dc_query_get_column_value(void *_seg_addr, void *_row, char *_col_name, void **_value, int *_max_len, int *_type)
{
    int idx = 0;
    segment_header_t    *seg_header = NULL;

    seg_header = (segment_header_t *)_seg_addr;

    idx = dc_query_get_col_idx(_seg_addr, _col_name);
    if (idx < 0)
    {
        elog("error: dc_query_get_col_idx: %s", _col_name);
        return -1;
    }

    *_max_len   = seg_header->column[idx].column_max_length;
    *_value     = _row + seg_header->column[idx].column_offset;
    *_type      = seg_header->column[idx].column_type;

    return 0;
} /* dc_query_get_column_value */


void *dc_query_rs_next()
{
    return dc_rs_iterator_next();
}


int dc_query_get_stru(void * _data , int _size)
{
    int		i = 0;
    int     k = 0;
    int     num = 0;
    int     column_type = 0;
    int     column_max_len = 0;
    void	*p = NULL;
    char    *name = NULL;
    void    *p_value  = NULL;
    void    *row = NULL;
    segment_header_t *seg_header = NULL;

    int   left = 0;

    if ( _data == NULL || _size <= 0)
    {
        elog("error: _data is NULL");
        return -1;
    }

    row = dc_rs_get_curr_row();
    if (row == NULL)
    {
        elog("error: no row");
        return -1;
    }

    seg_header = (segment_header_t *)dc_rs_get_seg_header();

    num = dc_sql_get_select_num();
    /* olog("select column number: %d", num); */

    p = _data;
    left = _size;
    for (i = 0; i < num ; i++)
    {
        /* column name */
        name = dc_sql_get_select_column(i);
        if (name == NULL)
        {
            elog("error: dc_sql_get_select_column: %d", i);
            return -1;
        }
        /* olog("column name:   [%d] => [%s]", i, name); */

        /* column index of table define */
        k = dc_query_get_col_idx(seg_header, name);
        if (k < 0)
        {
            elog("error: dc_query_get_col_idx: %s", name);
            return -1;
        }
        /* olog("column index:  [%s] => [%d]", name, k); */

        /* column: type, max-size, value */
        column_type     = seg_header->column[k].column_type;
        column_max_len  = seg_header->column[k].column_max_length;
        p_value         = row + seg_header->column[k].column_offset;
        /* olog("column define: type[%d], max[%d]", column_type, column_max_len); */

        /* check exceeds */
        if (column_max_len > left)
        {
            elog("no more space: %d > %d", column_max_len, left);
            return -1;
        }

        /* assign value */
        switch ( column_type )
        {
#if ORACLE
            case DFFAPRAW:
            case DFFAPLONG:
            case DFFAPLONGRAW:
            case DFFAPCHAR:
            case DFFAPVCHAR2:
                STRALIGN( p , sizeof( char ) );
                memcpy(p, p_value, column_max_len);
                p = (void *)((long)p + sizeof( char ) * ( column_max_len + 1 ));
                break;
            case DFFAPDATE:
                STRALIGN( p , sizeof( char ) );
                memcpy(p, p_value, DEF_DATE_LEN);
                p = (void *)((long)p + sizeof( char ) * ( DEF_DATE_LEN + 1 ));
                break;
            case DFFAPSMINT:
                STRALIGN( p , sizeof( short ) );
                memcpy(p, p_value, sizeof( short) );
                p = (void *)((long)p + sizeof( short ));
                break;
            case DFFAPINT:
            case DFFAPNUMBER:
                STRALIGN( p , sizeof( long ) );
                memcpy(p, p_value, sizeof( long) );
                p = (void *)((long)p + sizeof( long ));
                break;
            case DFFAPDECIMAL:
            case DFFAPFLOAT:
                STRALIGN( p , sizeof( double ) );
                memcpy(p, p_value, sizeof( double) );
                p = (void *)((long)p + sizeof( double ));
                break;
            case DFFAPSMFLOAT:
                STRALIGN( p , sizeof( float ) );
                memcpy(p, p_value, sizeof( float) );
                p = (void *)((long)p + sizeof( float ));
                break;
#endif

#if DB2
            case DFFAPDATE:
            case DFFAPDTIME:
            case DFFAPINTERVAL:
            case DFFAPCHAR:
            case DFFAPVCHAR:
            case DFFAPLONG:
            case DFFAPVCHAR2:
                STRALIGN( p , sizeof( char ) );
                memcpy(p, p_value, column_max_len);
                p = (void *)((long)p + sizeof( char ) * ( column_max_len + 1 ));
                break;
            case DFFAPTEXT:
            case DFFAPBYTES:
                STRALIGN( p , sizeof( char ) );
                /* Db2GetData( i , p , DEF_CHAR_LEN ); ?? */
                /* p = (void *)((long)p + sizeof( char ) * ( DEF_CHAR_LEN + 1 )); */
                memcpy(p, p_value, column_max_len);
                p = (void *)((long)p + sizeof( char ) * ( column_max_len + 1 ));
                break;
            case DFFAPSMINT:
                STRALIGN( p , sizeof( short ) );
                memcpy(p, p_value, sizeof( short) );
                p = (void *)((long)p + sizeof( short ));
                break;
            case DFFAPINTEGER:
            case DFFAPINT:
                STRALIGN( p , sizeof( long ) );
                memcpy(p, p_value, sizeof( long) );
                p = (void *)((long)p + sizeof( long ));
                break;
            case DFFAPDECIMAL:
            case DFFAPFLOAT:
                STRALIGN( p , sizeof( double ) );
                memcpy(p, p_value, sizeof( double) );
                p = (void *)((long)p + sizeof( double ));
                break;
            case DFFAPSMFLOAT:
                STRALIGN( p , sizeof( float ) );
                memcpy(p, p_value, sizeof( float) );
                p = (void *)((long)p + sizeof( float ));
                break;
#endif
        }

        left -= column_max_len;

        if (left <= 0)
        {
            olog("no more space2");
            break;
        }
    } /* for */

    return( E_OK );
}


int dc_query_get_first_select_column(char * _name, int _size)
{
    int   idx = 0;
    char *name = NULL;

    name = dc_sql_get_select_column(idx);
    if (name == NULL)
    {
        elog("error: dc_sql_get_select_column: %d", idx);
        return -1;
    }

    snprintf(_name, _size, "%s", name);

    return 0;
}


/*
 *  Function: 
 *      parse sql with ? to value
 *      select * from table where id = ?
 *      ==>
 *      select * from table where id = 'ca'
 *
 *  Parameters:
 *     _sql - original sql
 *
 *  Return Value:
 *      SUCCESS - 0
 *      FAILURE - -1
 */
int dc_query_resolve_sql(char *_sql, int _len, char *_buffer, int _size)
{
    int rv = 0;

    if (dc_sql_resovle(_sql, _len, _buffer, _size))
    {
        elog("error: dc_sql_resovle: %s, %d", _sql, _len);
        return -1;
    }

    return rv;
}


#if RUN_QUERY

static int dc_test_query(char *_sql, int _sql_len, void *_blk)
{
    int  rv = 0;
    long v2 = 0;
    char v1[20+1] = {0};

    (void)_blk;

    rv = dc_query_sql_select(_sql, _sql_len);
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

        /* get select column  */

        if (dc_query_get_data(0, v1, sizeof(v1)-1) < 0)
        {
            elog("error: dc_query_get_data");
        }
        else
        {
            olog("v1: [%s]", v1);
        }

        if (dc_query_get_data(1, &v2, sizeof(v2)) < 0)
        {
            elog("error: dc_query_get_data");
        }
        else
        {
            olog("v2: [%ld]", v2);
        }
    }

    olog("----------------------------------------");
    olog("----------------------------------------");
    olog("----------------------------------------");

    return 0;
}


static int dc_test_query_rows(char *_sql, int _sql_len, void *_blk)
{
    int  rv = 0;
    long v2 = 0;
    char v1[20+1] = {0};

    (void)_blk;

    rv = dc_query_sql_select(_sql, _sql_len);
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
        olog("query get [%d] rows", rv);

        /* get select column  */

        dc_rs_iterator_init();

        while (dc_query_rs_next() != NULL)
        {
            if (dc_query_get_data(0, v1, sizeof(v1)-1) < 0)
            {
                elog("error: dc_query_get_data");
            }
            else
            {
                olog("v1: [%s]", v1);
            }

            /*
            if (dc_query_get_data(1, &v2, sizeof(v2)) < 0)
            {
                elog("error: dc_query_get_data");
            }
            else
            {
                olog("v2: [%ld]", v2);
            }
            */
        }
    }

    olog("----------------------------------------");

    return 0;
}



int main(int argc, char *argv[])
{
    int  rv  = 0;
    int  err = 0;
    int  seg_idx = 0;
    int  to_read   = 0;
    int  to_create = 0;
    int  to_write  = 0;
    int  shm_id   = 0;
    int  shm_key  = 7788;
    int  sql_len = 0;
    long shm_size = 400000;
    void *shm_addr = NULL;
    void *blk_a    = NULL;
    long flag = 0;
    char sql[256] = {0};

    olog("query begin");

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
        if (shm_addr != NULL)
        {
            olog("use unified shm-addr: %p", shm_addr);
            shm_id = dc_shm_init(shm_key, shm_size, &shm_addr, flag, &err);
            if (shm_id == FAILURE)
            {
                elog("error: dc_shm_init2: %d", shm_id);
                return -1;
            }
        }
        else
        {
            elog("error: dc_shm_init1: %d", shm_id);
            return -1;
        }
    }
    /* olog("shm addr: %p", shm_addr); */


    if (argc == 3)
    {
        seg_idx = atoi(argv[1]);

        if (memcmp(argv[2], "R", 1) == 0)
        {
            olog("to_read");
            to_read = 1;
        }
        else if (memcmp(argv[2], "C", 1) == 0)
        {
            olog("to_create");
            to_create = 1;
        }
        else if (memcmp(argv[2], "W", 1) == 0)
        {
            olog("to_write");
            to_write = 1;
        }
        else
        {
            elog("error: unknown cmd: %s", argv[2]);
            return -1;
        }
    }
    else
    {
        seg_idx = 0;
        to_read = 1;
    }


    if (to_read)
    {
        olog("READER");

#if 0
        /* query by index and other column */
        sql_len = snprintf(sql, sizeof(sql), "%s",
                "SELECT INST_DATE_TIME, SUCC_TOTAL_NUM  FROM TBL_LOG_COUNTER "
                "WHERE PAN ='6216920000008264' AND TXN_TYPE=  'ZHXXYZ' "
                " AND TXN_DATE = '20170509' ");

        if (dc_test_query(sql, sql_len, blk_a))
        {
            elog("error: dc_test_query");
            return -1;
        }
#endif

#if ORACLE
        /* 2017-6-28 query by index and other column */
        sql_len = snprintf(sql, sizeof(sql), "%s",
                "SELECT INST_DATE_TIME, SUCC_TOTAL_NUM  FROM TBL_LOG_COUNTER "
                "WHERE TxN_TYPE = 'ZHXXYZ' AND (TXN_dATE = '20170622' OR PAn='6226223100055999')");
        olog("%s", sql);

        if (dc_test_query_rows(sql, sql_len, blk_a))
        {
            elog("error: dc_test_query_rows");
            return -1;
        }

        typedef struct _st_t
        {
            char date_time[14+1];
            long num;
        } test_t;

        test_t st;

        memset(&st, 0, sizeof(st));
        if (dc_query_get_stru(&st, sizeof(st)))
        {
            elog("error: dc_query_get_stru");
            return -1;
        }
        olog("[%s]", st.date_time);
        olog("[%d]", st.num);
#endif


#if DB2
        sql_len = snprintf(sql, sizeof(sql), "%s",
                "SELECT * FROM T_CREDIT_BIN WHERE BIN='625912'");
        olog("%s", sql);

        if (dc_test_query_rows(sql, sql_len, blk_a))
        {
            elog("error: dc_test_query_rows");
            return -1;
        }

        typedef struct _st_t
        {
            char bin[6+1];
            char name[50+1];
        } test_t;

        test_t st;

        memset(&st, 0, sizeof(st));
        if (dc_query_get_stru(&st, sizeof(st)))
        {
            elog("error: dc_query_get_stru");
            return -1;
        }
        olog("[%s]", st.bin);
        olog("[%s]", st.name);
#endif

#if 0
        /* TBL_CFG_LINE: query by index and other column */
        sql_len = snprintf(sql, sizeof(sql), "%s",
                "SELECT REMOTE_ADDR, REMOTE_PORT  FROM TBL_CFG_LINE "
                "WHERE SRV_NAME ='CommCredit' AND REMOTE_SVC_NAME =  'YL03' "
                " AND PROTOCOL  = 'XML' ");
        if (dc_test_query(sql, sql_len, blk_a))
        {
            elog("error: dc_test_query");
            return -1;
        }

        /* TBL_CFG_TXN: no index, query by full table */
        sql_len = snprintf(sql, sizeof(sql), "%s",
                "SELECT DSP, LIVE_TIME FROM TBL_CFG_TXN "
                "WHERE TXN_NUM =  '1073' ");
        if (dc_test_query(sql, sql_len, blk_a))
        {
            elog("error: dc_test_query");
            return -1;
        }
#endif

#if 0
        /* query by index only */
        sql_len = snprintf(sql, sizeof(sql), "%s",
                "SELECT INST_DATE_TIME, FAIL_TOTAL_NUM FROM TBL_LOG_COUNTER "
                "WHERE PAN ='6226223100055923' AND TXN_TYPE=  'ZHXXYZ' ");
        if (dc_test_query(sql, sql_len, blk_a))
        {
            elog("error: dc_test_query");
            return -1;
        }

        /* query by index buy No data */
        sql_len = snprintf(sql, sizeof(sql), "%s",
                "SELECT INST_DATE_TIME, FAIL_TOTAL_NUM FROM TBL_LOG_COUNTER "
                "WHERE PAN ='6226223100055923' AND TXN_TYPE=  'XXXYYY' ");
        if (dc_test_query(sql, sql_len, blk_a))
        {
            elog("error: dc_test_query");
            return -1;
        }

        /* query by full table: 1  */
        sql_len = snprintf(sql, sizeof(sql), "%s",
                "SELECT INST_DATE_TIME, FAIL_TOTAL_NUM FROM TBL_LOG_COUNTER "
                "WHERE INST_DATE_TIME =   '20170508190147' ");
        if (dc_test_query(sql, sql_len, blk_a))
        {
            elog("error: dc_test_query");
            return -1;
        }

        /* query by full table: 0  */
        sql_len = snprintf(sql, sizeof(sql), "%s",
                "SELECT INST_DATE_TIME, FAIL_TOTAL_NUM FROM TBL_LOG_COUNTER "
                "WHERE INST_DATE_TIME =   '30170508190147' ");
        if (dc_test_query(sql, sql_len, blk_a))
        {
            elog("error: dc_test_query");
            return -1;
        }

        /* query by full table: 2  */
        sql_len = snprintf(sql, sizeof(sql), "%s",
                "SELECT INST_DATE_TIME, FAIL_TOTAL_NUM FROM TBL_LOG_COUNTER "
                "WHERE TXN_TYPE =  'ZHXXYZ' ");
        if (dc_test_query(sql, sql_len, blk_a))
        {
            elog("error: dc_test_query");
            return -1;
        }


        /* query by full table:  table no index  */
        sql_len = snprintf(sql, sizeof(sql), "%s",
                "SELECT TXN_NUM, LIVE_TIME FROM TBL_CFG_TXN "
                "WHERE SRV_NAME =  'SwtInner' AND TOT_SRV ='SwtInner' ");
        if (dc_test_query(sql, sql_len, blk_a))
        {
            elog("error: dc_test_query");
            return -1;
        }
#endif

    }

    olog("query end");
    return rv;
}
#endif

/* query.c */
