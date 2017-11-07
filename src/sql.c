#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <errno.h>
#include <assert.h>
#include <ctype.h>

#include "type.h"
#include "log.h"
#include "util.h"

#include "shm.h"
#include "blk.h"
#include "tree.h"
#include "seg.h"
#include "load.h"
#include "query.h"
#include "hash.h"
#include "put.h"
#include "sql.h"



#define DC_SQL_AND_SEP              "AND"
#define DC_SQL_AND_SEP_LEN          3

#define DC_SQL_OR_SEP               "OR"
#define DC_SQL_OR_SEP_LEN           2

#define DC_SQL_KEYWORD_FROM         "FROM"
#define DC_SQL_KEYWORD_FROM_LEN     4

#define DC_SQL_KEYWORD_SELECT       "SELECT"
#define DC_SQL_KEYWORD_SELECT_LEN   6

#define DC_SQL_KEYWORD_WHERE        "WHERE"
#define DC_SQL_KEYWORD_WHERE_LEN    5


#define DC_SQL_EQUAL_SEP        '='
#define DC_SQL_QUESTION_SEP     '?'

#define DC_SQL_HASH_BUCKETS     52


/* where K=V, this is for value */
typedef struct {
    int         len;
    int         attr;
    char        data[1];
} dc_sql_v_t;



static hashtable_t *g_where_kv = NULL;
static dc_sql_c_t   g_where;
static dc_sql_c_t   g_select;
static char         g_table[SEGMENT_NAME_MAX+1] = {0};

static int          g_where_idx = 0;

static char        *g_where_region = NULL;
static int          g_where_region_len = 0;

/* ================================================================= */

static int dc_sql_analyze_kv(char *_kv, int _len);
static int dc_sql_set(char *_key, int _key_len, char *_val, int _val_len);

int dc_sql_analyze_select_region(char *_select_region, int _len);
int dc_sql_analyze_where_region(char *_where_region, int _len);

/* ================================================================= 
   ================================================================= */


/*
 *  Function:  dc_sql_init
 *      create the hash as inner container;
 *      this function should be invoked only once!
 *  Parameters:
 *
 *  Return Value:
 *      SUCCESS - 0
 *      FAILURE - -1
 */
int dc_sql_init(void)
{
    long bucket = DC_SQL_HASH_BUCKETS;

    if (g_where_kv != NULL)
    {
#if DC_DEBUG
        olog("previous container cleared");
#endif
        hashtable_destroy(g_where_kv);
        g_where_kv = NULL;
    }

    g_where_kv = hashtable_create(bucket);
    if (g_where_kv == NULL)
    {
        elog("error: hashtable_create[%d]!", bucket);
        return -1;
    }

#if DC_DEBUG
    olog("SQL container created");
#endif

    hashtable_set_deallocation_function(g_where_kv, free, free);

    /* table name */
    memset(g_table, 0, sizeof(g_table));

    /* select region */
    memset(&g_select, 0, sizeof(g_select));

    /* where list region */
    memset(&g_where,  0, sizeof(g_where));

    return 0;
} /* dc_sql_init */


int dc_sql_destroy(void)
{
    olog("%s", __func__);
    if (g_where_kv != NULL)
    {
        hashtable_destroy(g_where_kv);
    }
    g_where_kv = NULL;

    return 0;
}


/*
 *  Function:
 *      clear the last ';' of SQL if exists
 *
 *  Parameters:
 *      _sql -  sql
 *
 *  Return Value:
 *      SUCCESS - left length without ';'
 *      FAILURE - failure
 */
int dc_sql_clear_semicolon(char *_sql, int _len)
{
    int i = 0;
    int len = 0;

    for (i = _len-1; i >= 0; i--)
    {
        if (_sql[i] == ';')
        {
            len = i;
            break;
        }
        else if (_sql[i] == ' ')
        {
            /* ignore */
        }
        else
        {
            len = i + 1;
            break;
        }
    }

    return len;
}


/*
 *  Function:
 *      SELECT FAIL_CONTINUE_NUM FROM TBL_LOG_COUNTER 
 *          WHERE PAN='6216920000008264' AND TXN_TYPE='ZHXXYZ';
 *
 *  Parameters:
 *      _xxx - segment head address
 *
 *  Return Value:
 *      SUCCESS - 
 *      FAILURE - failure
 */
int dc_sql_analyze(char *_sql, int _len)
{
    int   len = 0;
    int   from_area     = 0;
    int   table_len     = 0;
    int   where_area    = 0;
    int   where_len     = 0;
    int   select_area   = 0;
    int   select_len    = 0;
    char  sep = ' ';
    char *ptr = NULL;
    char *p_select = NULL;
    char *p_where  = NULL;

    strsep3_init(_sql, _len, sep);

    len = 0;
    ptr = _sql;
    while ((ptr = strsep3_next(ptr, &len)) != NULL)
    {
        if (select_area)
        {
            select_area = 0;
            p_select = ptr;
        }
        else if (from_area)
        {
            from_area = 0;
            table_len = len;

            snprintf(g_table, sizeof(g_table), "%.*s", len, ptr);
            if (g_table[len-1] == ';')
            {
                g_table[len-1] = 0;
                len--;
            }
#if DC_DEBUG
            olog("table name: [%s]", g_table);
#endif

            ptr += len;
            continue;
        }
        else if (where_area)
        {
            where_area = 0;
            where_len  = _len - (ptr - _sql);

            g_where_region      = ptr;
            g_where_region_len  = where_len;

#if DC_DEBUG
            olog("where region1: [%.*s], %d", where_len, ptr, where_len);
#endif
            break;
        }

        if (len == DC_SQL_KEYWORD_SELECT_LEN && strncasecmp(DC_SQL_KEYWORD_SELECT, ptr, len) == 0)
        {
#if DC_DEBUG
            olog("keyword: %.*s", len, ptr);
#endif
            select_area = 1;
        }
        else if (len == DC_SQL_KEYWORD_FROM_LEN && strncasecmp(DC_SQL_KEYWORD_FROM, ptr, len) == 0)
        {
#if DC_DEBUG
            olog("keyword: %.*s", len, ptr);
#endif
            from_area   = 1;
        }
        else if (len == DC_SQL_KEYWORD_WHERE_LEN && strncasecmp(DC_SQL_KEYWORD_WHERE, ptr, len) == 0)
        {
#if DC_DEBUG
            olog("keyword: %.*s", len, ptr);
#endif
            where_area = 1;
            p_where = ptr + DC_SQL_KEYWORD_WHERE_LEN;
        }
        else
        {
            /*
            olog("token: %.*s", len, ptr);
            */
        }

        if (from_area)
        {
            if (p_select == NULL)
            {
                elog("error: invalid sql, not found SELECT key word");
                return -1;
            }

            /* SELECT: got select region [start, end) */
            select_len = ptr - p_select;
            /*
            olog("select region: [%.*s]", select_len, p_select);
            */

            if (dc_sql_analyze_select_region(p_select, select_len))
            {
                elog("error: dc_sql_analyze_select_region failure");
                return -1;
            }
        }
 
        ptr += len;
    }

    if (where_area)
    {
        where_area = 0;
        where_len  = _len - (p_where - _sql);

        g_where_region      = p_where;
        g_where_region_len  = where_len;

#if DC_DEBUG
        olog("where region2: [%.*s], %d", where_len, p_where, where_len);
#endif
    }

    return 0;
} /* dc_sql_analyze */



/*
 *  Function:
 *      select region: col1, col2, colN
 *
 *
 *  Parameters:
 *      _select_region - col1, col2, colN
 *
 *  Return Value:
 *      SUCCESS - 
 *      FAILURE - failure
 */
int dc_sql_analyze_select_region(char *_select_region, int _len)
{
    int  i = 0;
    int  idx = 0;
    int  word_len = 0;
    int  word_started = 0;
    char *p = NULL;
    char *start = NULL;

    memset(&g_select, 0, sizeof(g_select));

    p = _select_region;

    if (*p == '*')
    {
        return 0;
    }

#if DC_DEBUG
    olog("select region: [%.*s]", _len, p);
#endif

    for (i = 0; i < _len; i++, p++)
    {
        if (*p == ' ' || *p == ',')
        {
            if (word_started)
            {
                word_started = 0;
#if DC_DEBUG
                olog("word: [%.*s]", word_len, start);
#endif

                idx = g_select.col_num;
                if (idx >= DC_SQL_MAX_COLUMNS)
                {
                    elog("error: too many columns: %d", idx);
                    return -1;
                }
                snprintf(g_select.col[idx].column_name, sizeof(g_select.col[idx].column_name), "%.*s", word_len, start);
                g_select.col_num++;
            }
        }
        else
        {
            if (word_started)
            {
                word_len++;
            }
            else
            {
                word_started = 1;
                start = p;
                word_len = 1;
            }
        }
    }

    if (word_started)
    {
        word_started = 0;
        olog("word: [%.*s]", word_len, start);

        idx = g_select.col_num;
        if (idx >= DC_SQL_MAX_COLUMNS)
        {
            elog("error: too many columns: %d", idx);
            return -1;
        }
        snprintf(g_select.col[idx].column_name, sizeof(g_select.col[idx].column_name), "%.*s", word_len, start);
        g_select.col_num++;
    }


    return 0;
} /* dc_sql_analyze_select_region */


/**
 *  Function:
 *      PAN='1' AND TXN_TYPE='2';
 *      KV: { "PAN" => "1"; "TXN_TYPE" => "2" }
 *      WARN: only suitable for 'and' clause
 *
 *
 *  Parameters:
 *      _sql - PAN='1' AND TXN_TYPE='2'
 *
 *  Return Value:
 *      SUCCESS - 0
 *      FAILURE - failure
 */
int dc_sql_analyze_where_and(char *_sql, int _len)
{
    int  len = 0;
    int  len_v = 0;
    char sep = ' ';
    char *ptr = NULL;
    char *start = NULL;

#if DC_DEBUG
    olog("where input:  [%.*s]", _len, _sql);
#endif

    memset(&g_where, 0, sizeof(g_where));

    strsep3_init(_sql, _len, sep);

    len = 0;
    ptr = _sql;
    start = ptr;
    while ((ptr = strsep3_next(ptr, &len)) != NULL)
    {
        /*
        olog("token++: [%.*s]", len, ptr);
        */

        if (len == DC_SQL_AND_SEP_LEN && strncasecmp(DC_SQL_AND_SEP, ptr, len) == 0)
        {
            len_v = ptr - start;
#if DC_DEBUG
            olog("section: [%.*s]", len_v, start);
#endif
            if (dc_sql_analyze_kv(start, len_v))
            {
                elog("error: dc_sql_analyze_kv: %*s", len_v, start);
                return -1;
            }

            start = ptr + DC_SQL_AND_SEP_LEN;
        }
 
        ptr += len;
    }

    /* last time */
    len_v = _len - (start - _sql);
    /*
    olog("section: [%.*s]", len_v, start);
    */
    if (dc_sql_analyze_kv(start, len_v))
    {
        elog("error: dc_sql_analyze_kv: %*s", len_v, start);
        return -1;
    }


    /* the last one */

    return 0;
} /* dc_sql_analyze_where_and */


/*
 *  Function:
 *      input:  PAN='1' 
 *      output:"PAN" => "1"
 *
 *
 *
 *  Parameters:
 *      _kv - PAN='1'
 *
 *  Return Value:
 *      SUCCESS - 
 *      FAILURE - failure
 */
static int dc_sql_analyze_kv(char *_kv, int _len)
{
    int  len1 = 0;
    int  len2 = 0;
    char *p = NULL;
    char *p1 = NULL;
    char *p2 = NULL;


    p = strchr(_kv, DC_SQL_EQUAL_SEP);
    if (p == NULL)
    {
        elog("error: invalid expr: [%.*s]", _len, _kv);
        return -1;
    }

    /* p1 */
    p1   = _kv;
    len1 = p - p1;
    /* olog("p1:  [%.*s] %d", len1, p1, len1); */

    /* p2 */
    p2   = p + 1;
    len2 = _len - (p2-_kv);
    /* olog("p2:  [%.*s] %d", len2, p2, len2); */
    if (len2 == 0)
    {
        elog("error: no value after '='");
        return -1;
    }

    /* save */
    if (dc_sql_set(p1, len1, p2, len2))
    {
        elog("error: dc_sql_set");
        return -1;
    }

#if DC_DEBUG
    olog("------------------------------------------");
#endif

    return 0;
} /* dc_sql_analyze_kv */


void dc_sql_where_iterator_init()
{
    g_where_idx = 0;
}


char *dc_sql_where_iterator_next()
{
    if (g_where_idx >= g_where.col_num)
    {
        return NULL;
    }
    else
    {
        return g_where.col[g_where_idx++].column_name;
    }
}


/*
 *  Function:  dc_sql_set
 *      input: " PAN ", 5, "'1'", 3
 *      output:PAN => 1
 *
 *
 *
 *  Parameters:
 *      _kv - PAN='1'
 *
 *  Return Value:
 *      SUCCESS - 
 *      FAILURE - failure
 */
static int dc_sql_set(char *_key, int _key_len, char *_val, int _val_len)
{
    int  rv = 0;
    int  idx  = 0;
    int  is_alp = 0;

    char *pk = NULL;
    char *pv = NULL;

    char *ps = NULL;
    char *pd = NULL;

    dc_sql_v_t *pt = NULL;

    /*
    olog("key: [%.*s] %d", _key_len, _key, _key_len);
    */
    pk = calloc(_key_len+1, 1);
    if (pk == NULL)
    {
        elog("error: calloc pk: %d", _key_len);
        return -1;
    }

    /*
    olog("val: [%.*s] %d", _val_len, _val, _val_len);
    */
    pv = calloc(sizeof(dc_sql_v_t)+_val_len, 1);
    if (pv == NULL)
    {
        elog("error: calloc pv: %d", _val_len);
        free(pk);
        return -1;
    }

    /* KEY */
    ps = _key;
    while ( *((unsigned char *)ps) == 0x20 && _key_len > 0 )
    {
        ps ++; 
        _key_len--;
    }
    /* olog("pk1: %s, %d", ps, _key_len); */

    pd = pk;
    while ( *ps != 0x0 && *((unsigned char *)ps) != 0x20 && _key_len > 0)
    {
        *pd ++ = *ps ++; 
        _key_len--;
    }

    *pd = 0x0;
#if DC_DEBUG
    olog("column name:  [%s]", pk);
#endif

    /* VALUE */
    ps = _val;
    while ( *((unsigned char *)ps) == 0x20 && _val_len > 0)
    {
        ps ++; 
        _val_len--;
    }

    if (*ps == '\'')
    {
        ps ++; 
        _val_len--;
        is_alp = 1;
    }

    pt = (dc_sql_v_t*)pv;
    pd = (char *)pt->data;
    while ( *ps != 0x0 && *((unsigned char *)ps) != 0x20 && *ps != '\'' && _val_len > 0)
    {
        *pd ++ = *ps ++; 
        _val_len--;
    }

    *pd = 0x0;
    pt->len = (void*)pd - (void*)pt->data;
#if DC_DEBUG
    olog("column value: [%s]", pt->data);
#endif

    /* recognize CHAR or NUMBER, to be implemented
    if (!is_alp)
    {
        pt->len = atol(pt->data);
        pt->data[0] = 0;
        olog("column value: [%ld]", pt->len);
    }
    */

    /* list */
    idx = g_where.col_num;
    if (idx >= DC_SQL_MAX_COLUMNS)
    {
        elog("error: too many columns: %d", idx);
        free(pk);
        free(pv);
        return -1;
    }
    dc_str_toupper(pk);
    snprintf(g_where.col[idx].column_name, sizeof(g_where.col[idx].column_name), "%s", pk);
    g_where.col_num++;
    /*
    olog("pk: %s", pk);
    */

    rv = hashtable_put(g_where_kv, pk, pv);

    return rv;
}


/*
 *  Function:
 *      clear all the buckets in hash,
 *
 *  Parameters:
 *
 *  Return Value:
 *      SUCCESS - 0
 *      FAILURE - -1
 */
void dc_sql_reset(void)
{
    g_where_region     = NULL;
    g_where_region_len = 0;
}


/*
 *  Function:
 *      clear all the buckets in hash,
 *
 *  Parameters:
 *
 *  Return Value:
 *      SUCCESS - 0
 *      FAILURE - -1
 */
void dc_sql_reset_where(void)
{
    if (g_where_kv != NULL && !hashtable_is_empty(g_where_kv))
    {
        hashtable_remove_all(g_where_kv);
    }
}


int dc_sql_where_columns(void)
{
    int num = 0;

    if (g_where_kv != NULL)
    {
        num = (int)hashtable_size(g_where_kv);
    }

    return num;
}

/*
 *  Function: 
 *      check whether this index should be used.
 *      +PAN+TXN_TYPE => check PAN in hash, TXN_TYPE in hash
 *
 *  Parameters:
 *      _index_columns - +PAN+TXN_TYPE
 *
 *  Return Value:
 *      SUCCESS - >=1, index column number
 *      FAILURE - 0
 */
int dc_sql_check_use_this_index(char *_index_columns, int _len)
{
    int   len = 0;
    int   col_counter = 0;
    char *ptr = NULL;
    char  col[COLUMN_NAME_MAX+1] = {0};

    strsep3_init(_index_columns, _len, DC_INDEX_SEP[0]);

    len = 0;
    ptr = _index_columns;
    while ((ptr = strsep3_next(ptr, &len)) != NULL)
    {
        snprintf(col, sizeof(col), "%.*s", len, ptr);
        dc_str_toupper(col);
#if DC_DEBUG
        olog("index column: %s", col);
#endif
        if (!hashtable_contains_key(g_where_kv, col))
        {
            /* this column of index not appear in WHERE clause */
#if DC_DEBUG
            olog("index column [%s] not used", col);
#endif
            return 0;
        }

        col_counter += 1;
 
        ptr += len;
    }

    return col_counter;
}



/*
 *  Function: 
 *      combine all column values of index
 *      +PAN+TXN_TYPE => 6216920000008264ZHXXYZ
 *
 *  Parameters:
 *      _index_columns - +PAN+TXN_TYPE
 *
 *  Return Value:
 *      SUCCESS - > 0
 *      FAILURE - 0
 */
int dc_sql_combine_index_value(char *_index_columns, int _len, char *_value, int _size)
{
    int   len = 0;
    int   used= 0;
    int   left= 0;
    char *pv  = NULL;
    char *ptr = NULL;
    char  col[COLUMN_NAME_MAX+1] = {0};
    dc_sql_v_t *pt = NULL;

    left = _size;
    pv   = _value;

    strsep3_init(_index_columns, _len, DC_INDEX_SEP[0]);

    len = 0;
    ptr = _index_columns;
    while ((ptr = strsep3_next(ptr, &len)) != NULL)
    {
        /* no need memset */
        snprintf(col, sizeof(col), "%.*s", len, ptr);
        dc_str_toupper(col);

        pt = (dc_sql_v_t *)hashtable_get(g_where_kv, col);
        if (pt == NULL)
        {
            elog("error: hashtable_get: %s", col);
            return -1;
        }

        if (left <= 1)
        {
            elog("error: small space: %d", _size);
            return 0;
        }

        used = snprintf(pv, left, "%s", pt->data);
        pv  += used;
        left-= used;

 
        ptr += len;
    }

    return (int)((long)pv - (long)_value);
}



/*
 *  Function: 
 *      get where value by key
 *      PAN => "6216920000008264"
 *
 *  Parameters:
 *      _column - PAN
 *
 *  Return Value:
 *      SUCCESS - > 0
 *      FAILURE - 0
 */
int dc_sql_get_where_value(char *_column, void **_value, int *_len)
{
    dc_sql_v_t *pt = NULL;

    pt = (dc_sql_v_t *)hashtable_get(g_where_kv, _column);
    if (pt == NULL)
    {
        elog("error: hashtable_get: %s", _column);
        return -1;
    }

    *_value = pt->data;
    *_len   = pt->len;

    return 0;
}


/*
 *  Function: 
 *      get select column
 *      0 => column1
 *
 *  Parameters:
 *      _column - PAN
 *
 *  Return Value:
 *      SUCCESS - > 0
 *      FAILURE - 0
 */
char *dc_sql_get_select_column(int _idx)
{
    if (_idx >= g_select.col_num)
    {
        return NULL;
    }
    else
    {
        return g_select.col[_idx].column_name;
    }
}


int dc_sql_get_select_num()
{
    return g_select.col_num;
}


/*
 *  Function: 
 *      get table name
 *
 *  Parameters:
 * 
 *
 *  Return Value:
 *      SUCCESS - > 0
 *      FAILURE - 0
 */
char *dc_sql_get_table_name()
{
    return g_table;
}


void dc_sql_set_table_name(char *_name)
{
    snprintf(g_table, sizeof(g_table), "%s", _name);
}


/*
 *  Function: 
 *      get select region
 *
 *  Parameters:
 *      
 *
 *  Return Value:
 *      SUCCESS - address
 *      FAILURE - NULL
 */
void *dc_sql_get_select()
{
    if (g_select.col_num > 0)
    {
        return &g_select;
    }
    else
    {
        return NULL;
    }
}


/*
 *  Function: 
 *      set select region
 *
 *  Parameters:
 *      
 *
 *  Return Value:
 *      SUCCESS - 0
 *      FAILURE - 
 */
int dc_sql_set_select(void *_select)
{
    memcpy(&g_select, _select, sizeof(g_select));

    return 0;
}


int dc_sql_get_where(char **_where, int *_len)
{
    if (g_where_region == NULL)
    {
        return 1;
    }

    if (g_where_region_len <= 0)
    {
        elog("error: invalid parsed length");
        return -1;
    }

    *_where = g_where_region;
    *_len  = g_where_region_len;

    return 0;
}


/*
 *  Function:
 *      SELECT FAIL_CONTINUE_NUM FROM TBL_LOG_COUNTER 
 *          WHERE PAN=? AND TXN_TYPE=?;
 *
 *  Parameters:
 *      _sql -
 *
 *  Return Value:
 *      SUCCESS - 
 *      FAILURE - failure
 */
int dc_sql_resovle(char *_sql, int _len, char *_buffer, int _size)
{
    char  c = 0;
    int   i = 0;
    int   k = 0;
    int   pos = 0;
    int   len = 0;
    int   left = 0;
    int   width = 0;
    char *ptr = NULL;
    dc_put_t *pt = NULL;

    left = _size;

    if (strchr(_sql, DC_SQL_QUESTION_SEP) == NULL)
    {
        /* no question mark */
#if DC_DEBUG
        olog("no question mark");
#endif
        snprintf(_buffer, _size, "%s", _sql);
    }
    else
    {
        /* find question mark */
#if DC_DEBUG
        olog("find: %s", _sql);
#endif

        if (_len > _size)
        {
            elog("too short space: %d", _size);
            return -1;
        }

        /* to upper */
        for (i = 0; i < _len; i++)
        {
            _buffer[i] = (char)toupper((int)_sql[i]);
        }
        _buffer[i] = 0;

        if ((ptr = strstr(_buffer, DC_SQL_KEYWORD_WHERE)) == NULL)
        {
            /* no where section */
#if DC_DEBUG
            olog("[%s]: no where region", _sql);
#endif
        }
        else
        {
            memset(_buffer, 0, _size);
            dc_put_iterator_init();

            /* before where region */
            ptr   = _sql + (ptr - _buffer);
            ptr  += DC_SQL_KEYWORD_WHERE_LEN;
            width = ptr - _sql;
            len = snprintf(_buffer+pos, left, "%.*s", width, _sql);
            pos  += len;
            left -= len;

            /* after where region */
            _len = _len - len;
            for (i = 0, k = 0; i < _len && left > 0; i++)
            {
                c = ptr[i];

                if (c == DC_SQL_QUESTION_SEP)
                {
                    pt = (dc_put_t*)dc_put_iterator_next();
                    if (pt == NULL)
                    {
                        elog("error: too many '?'");
                        return -1;
                    }
                    else
                    {
#if DC_DEBUG
                        olog("%s", pt->data.c_value);
#endif
                        len = snprintf(_buffer+pos, left, "'%s'", pt->data.c_value);
                        pos  += len;
                        left -= len;
                    }
                }
                else
                {
                    _buffer[pos] = c;
                    pos  += 1;
                    left -= 1;
                }
            } /* for */
        } /* with WHERE */
    } /* with '?' */

    return 0;
} /* dc_sql_resovle */




void dc_sql_dump(void)
{
    int         i;
    hashpair_t *pair = NULL;
    dc_sql_v_t *pt   = NULL;

    for (i=0; i<g_where_kv->numOfBuckets; i++)
    {
        pair = g_where_kv->bucketArray[i];

        while (pair != NULL)
        {
            pt = (dc_sql_v_t *)pair->value;
            olog("{\"%s\" => \"%s\" (%ld)};", pair->key, pt->data, pt->len);

            pair = pair->next;
        }
    }

    return;
}


void dc_sql_dump_select(void)
{
    int         i;

    for (i=0; i<g_select.col_num; i++)
    {
        olog("{%d => \"%s\" };", i, g_select.col[i].column_name);
    }

    return;
}


/**
 *  judge is count(1)/count(*)
 *  1: yes
 *  0: not
 * -1: error
 */
int dc_sql_is_aggregation(void)
{
    int   rv  = 0;
    int   idx = 0;
    char *name = NULL;

    name = dc_sql_get_select_column(idx);
    if (name == NULL)
    {
        elog("error: dc_sql_get_select_column: %d", idx);
        return -1;
    }

    if (strncasecmp(name, "COUNT(*)", 8) == 0
            || strncasecmp(name, "COUNT(1)", 8) == 0)
    {
        rv = 1;
    }
    else
    {
        rv = 0;
    }

    return rv;
}

#if RUN_SQL

#include "sysloc.h"
#include "pladb.h"
#include "mydb.h"

int main(int argc, char *argv[])
{
    int  rv  = 0;
    int  len = 0;
    int  i = 0;
    int  type = 0;
    char put[64] = {0};
    char where[128] = {0};
    char index[128] = {0};
    char buffer[256]= {0};
    char sql[256] = {0};

    (void)argc;
    (void)argv;

    olog("sql begin");

    if (dc_sql_init())
    {
        elog("error: dc_sql_init");
        return -1;
    }

    olog("=========================================");
    len = snprintf(where, sizeof(where), "%s", "  PAN  ='1x3' ");
    len = snprintf(where, sizeof(where), "%s", "  PAN  ='1x3'  AND   TXN_TYPE=  12 ");
    rv = dc_sql_analyze_where_and(where, len);
    if (rv)
    {
        elog("error: dc_sql_analyze_where_and");
        return -1;
    }

    olog("before reset");
    dc_sql_dump();
    olog("=========================================");

    len = snprintf(index, sizeof(index), "%s", "+PAN+TXN_TYPE");
    if (dc_sql_check_use_this_index(index, len))
    {
        olog("nice: let's use this index");
        dc_sql_combine_index_value(index, len, buffer, sizeof(buffer));
        olog("[%s => %s]", index, buffer);
    }
    else
    {
        olog("sorry: can't use this index");
    }

    olog("=========================================");

    len = snprintf(sql, sizeof(sql), "%s", "col1, col2,  kkk");
    if (dc_sql_analyze_select_region(sql, len))
    {
        elog("error: dc_sql_analyze_select_region");
    }
    else
    {
        olog("dc_sql_analyze_select_region succeeds");
        dc_sql_dump_select();
    }

    dc_sql_reset();

    olog("after  reset");
    dc_sql_dump();

    olog("=========================================");
    olog("=========================================");
    olog("=========================================");
    len = snprintf(sql, sizeof(sql), "%s", "SELECT col1, col2,  kkk FROM TBL_LOG_COUNTER "
            "WHERE PAN ='8216920000008264' AND TXN_TYPE=  'ZHXXYZ'");
    len = snprintf(sql, sizeof(sql), "%s", "SELECT * FROM TBL_LOG_COUNTER WHERE TXN_TYPE = 'x';");
    len = snprintf(sql, sizeof(sql), "%s", "SELECT * FROM TBL_LOG_COUNTER WHERE TXN_TYP");
    if (dc_sql_analyze(sql, len))
    {
        elog("error: dc_sql_analyze");
    }
    else
    {
        olog("nice: dc_sql_analyze succeeds");
    }

    /* ==================================================== */

    dc_put_init();
    snprintf(put, sizeof(put), "%ld", get_time());
    i = 0;
    type = FAPCHAR;
    if (dc_put_one(i, type, put))
    {
        elog("error: dc_put_one: %s", put);
        return -1;
    }

    len = snprintf(sql, sizeof(sql), "%s", "select * from table where key='xx'");
    len = snprintf(sql, sizeof(sql), "%s", "select * from table where k = ? and j=? and  key='xx'");
    len = snprintf(sql, sizeof(sql), "%s", "select * from /*?*/table");
    len = snprintf(sql, sizeof(sql), "%s", "select * from table where key=?");
    len = snprintf(sql, sizeof(sql), "%s", "selectselect * from table where key=?");
    if (dc_sql_resovle(sql, len, buffer, sizeof(buffer)))
    {
        elog("error: dc_sql_resovle: %s", sql);
        return -1;
    }
    olog("%s", buffer);
    /* ==================================================== */

    len = snprintf(sql, sizeof(sql), "%s", "select * from table where key='xy';");      /* case1 */
    len = snprintf(sql, sizeof(sql), "%s", "select * from table where key='xy';   ");   /* case2 */
    len = snprintf(sql, sizeof(sql), "%s", "select * from table where key='xy'    ");   /* case3 */
    len = snprintf(sql, sizeof(sql), "%s", "select * from table where key='xy'  ; ");   /* case4 */
    len = snprintf(sql, sizeof(sql), "%s", "select * from table where key='xy'");       /* case5 */
    olog("input:  %d: [%s]", len, sql);
    len = dc_sql_clear_semicolon(sql, len);
    olog("output: %d: [%.*s]", len, len, sql);


    char *ps = NULL;
    char *pd = NULL;
    int  val_len;

    memset(buffer, 0, sizeof(buffer));

    ps = " '123' " ;
    pd = buffer;

    val_len  = strlen(ps);
    olog("[%s], %d", ps, val_len);

    /* VALUE */
    while ( *((unsigned char *)ps) == 0x20 && val_len > 0)
    {
        ps ++; 
        val_len--;
        olog("111: %s",  ps);
    }

    if (*ps == '\'')
    {
        ps ++; 
        val_len--;
        olog("222: %s", ps);
    }

    while ( *ps != 0x0 && *((unsigned char *)ps) != 0x20 && *ps != '\'' && val_len > 0)
    {
        *pd ++ = *ps ++; 
        val_len--;
    }
    olog("pd:  [%s]",  pd);

    olog("sql end");
    return rv;
}
#endif

/* sql.c */
