#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <errno.h>
#include <assert.h>

#include "type.h"
#include "log.h"
#include "util.h"

#include "shm.h"
#include "blk.h"
#include "tree.h"
#include "seg.h"
#include "sec.h"
#include "aux.h"
#include "load.h"

#include "sysloc.h"
#include "pladb.h"
#include "mydb.h"


static segment_header_t  G_seg_header;
static int g_all_segment_size = 0;

/* ================================================================= */

static int dc_load_table_column(void *_seg_addr, char *_table_name);

static int dc_load_index_column(void *_seg_addr, char *_table_name);

static int dc_load_index_column_one(void *_seg_addr, char *_index_name);

static int dc_load_table_row(void *_seg_addr, void *_row, int _len);


static int dc_load_calc_one_segment_size(char *_table_name, int _max_rows);

static int dc_load_table_structure(void *_seg_addr);

static int dc_load_table_data(void *_seg_addr, char *_table_name);

static int dc_load_table_inner(void *_seg_addr, char *_table_name);

static int dc_load_table_new(void *_blk_addr, char *_table_name, int _max_rows, int _seg_size, long _t2);

/* ================================================================= 
   ================================================================= */


/*
 *  Function:
 *
 *  Parameters:
 *      _seg_addr - segment head address
 *
 *  Return Value:
 *      SUCCESS - 
 *      FAILURE - failure
 */
int dc_load_init()
{

    memset(&G_seg_header, 0, sizeof(G_seg_header));

    g_all_segment_size = 0;

    return 0;
}


/*
 *  Function: 
 *      load all tables from aux
 *
 *  Parameters:
 *     _table_name- table name
 *
 *  Return Value:
 *      SUCCESS - 0
 *      FAILURE - -1
 */
int dc_load_all_tables(void *_blk_addr, long _t2)
{
    int  idx = 0;
    int  cnt = 0;
    int  max_rows = 0;
    int  seg_size = 0;
    char *name = NULL;

    /*
    olog("-->>>>%s", __func__);
    */

    if (_blk_addr == NULL)
    {
        elog("error: input _blk_addr is NULL");
        return -1;
    }

    dc_aux_seg_iterator_init();

    while ((idx = dc_aux_seg_iterator_next(&name, &max_rows)) >= 0)
    {
#if DC_DEBUG
        olog("[%02d] => [%s, %d]", idx, name, max_rows);
#endif

        /* AUX: get segment size */
        seg_size = dc_aux_seg_get_size(name);
        if (seg_size <= 0)
        {
            elog("error: dc_aux_seg_get_size %s failure", name);
            return -1;
        }
#if DC_DEBUG
        olog("segment [%s] size: %d", name, seg_size);
#endif

        if (dc_load_table_new(_blk_addr, name, max_rows, seg_size, _t2) < 0)
        {
            elog("error: dc_load_table_new: %s", name);
            return -1;
        }
#if DC_DEBUG
        olog("load table %s succeeds", name);
#endif

        cnt++;

        name = NULL;
        max_rows = 0;
    }

#if DC_DEBUG
    olog("load [%d] tables succeed", cnt);
#endif

    return 0;
} /* dc_load_all_tables */


/*
 *  Function: 
 *      re-load a table
 *      implement: clear segment info and reload
 *
 *  Parameters:
 *     _table_name- table name
 *
 *  Return Value:
 *      SUCCESS - 0
 *      FAILURE - -1
 */
int dc_load_table_refresh(void *_blk_addr, char *_table_name, long _tm)
{
    int  size1 = 0;
    int  size2 = 0;
    int  seg_idx = 0;
    int  max_rows = 0;
    void *seg_addr = NULL;

    /*
    olog("-->>>>%s", __func__);
    */

    /* block: get index of segment*/
    seg_idx = dc_blk_get_segment_index(_blk_addr, _table_name);
    if (seg_idx < 0)
    {
        elog("error: dc_blk_get_segment_index");
        return -1;
    }
#if DC_DEBUG
    olog("segment index is <%d>", seg_idx);
#endif

    /* block: get segment addr */
    seg_addr = dc_blk_get_segment_addr(_blk_addr, seg_idx);
    if (seg_addr == NULL)
    {
        elog("error: dc_blk_get_segment_addr[%d] failure", seg_idx);
        return -1;
    }
#if DC_DEBUG
    olog("segment[%d] address: %p", seg_idx, seg_addr);
#endif

    /* get previous segment size from remote */
    size1 = dc_seg_get_segment_size(seg_addr);
    if (size1 <= 0)
    {
        elog("error: dc_seg_get_segment_size failure");
        return -1;
    }
#if DC_DEBUG
    olog("previous segment size: %d", size1);
#endif

    /* AUX: get user input max rows */
    max_rows = dc_seg_get_segment_rows(seg_addr);
    if (max_rows <= 0)
    {
        elog("error: dc_seg_get_segment_rows %s failure", _table_name);
        return -1;
    }
#if DC_DEBUG
    olog("previous max rows: %d", max_rows);
#endif

    /* XXX: calculate segment size, MUST */
    size2 = (long)dc_load_calc_one_segment_size(_table_name, max_rows);
    if (size2 <= 0)
    {
        elog("error: dc_load_calc_one_segment_size failure");
        return -1;
    }
#if DC_DEBUG
    olog("calculated segment size: [%ld]", size2);
#endif

    /* check size: aux vs calc */
    if (size1 != size2)
    {
        elog("error: size changed: %d => %ld", size1, size2);
        return -1;
    }
    else
    {
#if DC_DEBUG
        olog("nice: size keeps: %d", size1);
#endif
    }

    /* clear all info of this segment */
    memset(seg_addr, 0, size1);

    /* segment: init header */
    memcpy(seg_addr, &G_seg_header, sizeof(segment_header_t));
    dc_seg_init(seg_addr, seg_idx, max_rows, size1);
#if DC_DEBUG
    olog("segment init succeeds: %p", seg_addr);
#endif

    if (dc_load_table_inner(seg_addr, _table_name))
    {
        elog("error: dc_load_table_inner: %s", _table_name);
        return -6;
    }

    /* make the timestamp identified between the block A and B 2017-11-3 */
    dc_seg_set_timestamp(seg_addr, _tm);

#if DC_DEBUG
    olog("load table %s succeeds", _table_name);
#endif

    return 0;
}


/*
 *  Function: 
 *      load a table content: column define, index, data
 *
 *  Parameters:
 *     _table_name- table name
 *
 *  Return Value:
 *      SUCCESS - 0
 *      FAILURE - -1
 */
static int dc_load_table_inner(void *_seg_addr, char *_table_name)
{
    /*
    olog("-->>>>%s", __func__);
    */

    /* set timestamp */
    /* 2017-11-3: should be dicarded */
    dc_seg_gen_timestamp(_seg_addr);

    /* definition of table and index */
    if (dc_load_table_structure(_seg_addr))
    {
        elog("error: dc_load_table_structure");
        return -1;
    }
#if DC_DEBUG
    olog("load structure succeeds");
#endif

    /* data */
    if (dc_load_table_data(_seg_addr, _table_name))
    {
        elog("error: dc_load_table_data");
        return -1;
    }
#if DC_DEBUG
    olog("load data succeeds");
#endif

    return 0;
}

/*
 *  Function: 
 *      load a table, with allocate a segment
 *      WARN: this function must be with inner preceding dc_load_calc_one_segment_size()
 *
 *  Parameters:
 *     _table_name- table name
 *
 *  Return Value:
 *      SUCCESS - 0
 *      FAILURE - -1
 */
static int dc_load_table_new(void *_blk_addr, char *_table_name, int _max_rows, int _seg_size, long _t2)
{
    int  err = 0;
    int  seg_idx = 0;
    long seg_size = 0;
    void *seg_addr = NULL;

    /*
    olog("-->>>>%s", __func__);
    */

    if (_blk_addr == NULL)
    {
        elog("error: input _blk_addr is NULL");
        return -1;
    }

    /* XXX: calculate segment size, MUST */
    seg_size = (long)dc_load_calc_one_segment_size(_table_name, _max_rows);
    if (seg_size <= 0)
    {
        elog("error: dc_load_calc_one_segment_size failure");
        return -1;
    }
#if DC_DEBUG
    olog("segment size: [%ld]", seg_size);
#endif

    /* check size with previous calculated */
    if (seg_size != (long)_seg_size)
    {
        elog("error: size changed: %d => %ld", _seg_size, seg_size);
        return -2;
    }
    else
    {
#if DC_DEBUG
        olog("nice: size keeps: %d", seg_size);
#endif
    }

    /* block: allocate */
    seg_idx = dc_blk_alloc_segment(_blk_addr, seg_size, &err);
    if (seg_idx < 0)
    {
        elog("error: dc_blk_alloc_segment: %d", err);
        return -3;
    }
#if DC_DEBUG
    olog("allocated segment index is <%d>", seg_idx);
#endif

    /* block: set segment/table name */
    if (dc_blk_set_segment_name(_blk_addr, seg_idx, _table_name))
    {
        elog("error: dc_blk_set_segment_name: %d, %s", seg_idx, _table_name);
        return -4;
    }

    /* block: segment_addr */
    seg_addr = dc_blk_get_segment_addr(_blk_addr, seg_idx);
    if (seg_addr == NULL)
    {
        elog("error: dc_blk_get_segment_addr[%d] failure", seg_idx);
        return -5;
    }
#if DC_DEBUG
    olog("segment[%d] address: %p", seg_idx, seg_addr);
#endif

    /* segment: init header */
    memcpy(seg_addr, &G_seg_header, sizeof(segment_header_t));
    dc_seg_init(seg_addr, seg_idx, _max_rows, seg_size);
#if DC_DEBUG
    olog("segment init succeeds: %p", seg_addr);
#endif

    if (dc_load_table_inner(seg_addr, _table_name))
    {
        elog("error: dc_load_table_inner: %s", _table_name);
        return -6;
    }

    /* make the timestamp identified between the block A and B 2017-11-3 */
    dc_seg_set_timestamp(seg_addr, _t2);

#if DC_DEBUG
    olog("load table inner %s succeeds", _table_name);
#endif

    return seg_idx;
} /* dc_load_table_new */


/*
 *  Function: 
 *      get all columns definition of some table
 *      don't support empty table
 *
 *  Parameters:
 *     _table_name- table name
 *
 *  Return Value:
 *      SUCCESS - row size
 *      FAILURE - -1
 */
int dc_load_table_column_v0(void *_seg_addr, char *_table_name)
{
    int  i = 0;
    int  rv = 0;
    int  area = 0;
    int  col_num = 0;
    long row_size = 0;
    char sql_str[64+1] = {0};
    table_def_t col;
    segment_header_t  *seg_header = NULL;

    seg_header = (segment_header_t *)_seg_addr;

#if DC_DEBUG
    olog("-->>>>%s", __func__);
#endif

    snprintf(sql_str, sizeof(sql_str), "SELECT * FROM %s", _table_name);
#if DC_DEBUG
    olog("sql_str[%s]", sql_str);
#endif

    area = MallocArea();
    if ( -1 == area )
    {
        elog("error: MallocArea()");
        return -1;
    }

    do
    {
        rv = DBSelect(area, sql_str, 0, 0);
        if (rv == E_NOTFOUND)
        {
            olog("No more data found [%s]", sql_str );
            rv = 0;
            break;
        }

        if (rv == -1)
        {
            elog("error: sql execute [%d][%s]", FAP_SQLCODE, sql_str );
            FreeArea();
            return -1;
        }
#if DC_DEBUG
        olog("-------------------------------------------");
#endif

        row_size = db_get_rowsize();
#if DC_DEBUG
        olog("row size: %ld", row_size);
#endif

        col_num = db_get_ncols(area);
#if DC_DEBUG
        olog("column number: %d", col_num);
#endif

        for (i = 0; i < col_num; i++)
        {
            memset(&col, 0, sizeof(col));
            col.column_offset = db_get_column(i, col.column_name, sizeof(col.column_name),
                    &col.column_type, &col.column_max_length);
#if DC_DEBUG
            olog("column: [%s, %d, %d, %ld]",
                    col.column_name, col.column_type, col.column_max_length, col.column_offset);
#endif
            assert(dc_seg_add_column(_seg_addr, &col) != -1);
        }

    } while (0);


    FreeArea();

    return (int)row_size;
} /* dc_load_table_column_v0 */



/*
 *  Function: 
 *      get all columns definition of some table
 *      support empty table
 *
 *  Parameters:
 *     _table_name- table name
 *
 *  Return Value:
 *      SUCCESS - row size
 *      FAILURE - -1
 */
static int dc_load_table_column(void *_seg_addr, char *_table_name)
{
    int  i = 0;
    int  col_num = 0;
    long row_size = 0;
    table_def_t col;

#if DC_DEBUG
    olog("-->>>>%s", __func__);
#endif

    if (DBGetTabDesc(_table_name, &col_num))
    {
        elog("error: DBGetTabDesc: %d, %s", FAP_SQLCODE, _table_name);
        return -1;
    }
#if DC_DEBUG
        olog("column number: %d", col_num);
#endif

    row_size = db_get_rowsize_v2(col_num);
#if DC_DEBUG
        olog("row size: %ld", row_size);
#endif

    for (i = 0; i < col_num; i++)
    {
        memset(&col, 0, sizeof(col));
        col.column_offset = db_get_column_v2(i, col.column_name, sizeof(col.column_name),
                &col.column_type, &col.column_max_length);
#if DC_DEBUG
            olog("column: [%s, %d, %d, %ld]",
                    col.column_name, col.column_type, col.column_max_length, col.column_offset);
#endif
            assert(dc_seg_add_column(_seg_addr, &col) != -1);
    }

    return (int)row_size;
} /* dc_load_table_column*/


/*
 *  Function: 
 *      load all indexes definition of some table
 *
 *  Parameters:
 *     _table_name- table name
 *
 *  Return Value:
 *      SUCCESS - index number
 *      FAILURE - -1
 */
static int dc_load_index_column(void *_seg_addr, char *_table_name)
{
    int  rv  = 0;
    int  area = 0;
    int  index_num = 0;
    char sql_str[256] = {0};
    char index_name[64+1] = {0};
    segment_header_t  *seg_header = NULL;

    seg_header = (segment_header_t *)_seg_addr;

    /*
    olog("-->>>>%s", __func__);
    */

    area = MallocArea();
    if ( -1 == area )
    {
        elog("error: MallocArea()");
        return -1;
    }

#if ORACLE
    snprintf(sql_str, sizeof(sql_str),
            "SELECT INDEX_NAME FROM USER_INDEXES "
            "WHERE TABLE_NAME='%s' AND UNIQUENESS='UNIQUE'", _table_name);
#elif DB2
    snprintf(sql_str, sizeof(sql_str),
            "SELECT INDNAME FROM SYSCAT.INDEXES "
            "WHERE TABNAME='%s' and UNIQUERULE='U'", _table_name);
#endif
    dc_str_toupper(sql_str);

#if DC_DEBUG
    olog("sql_str[%s]", sql_str);
#endif

    while (1)
    {
#if DC_DEBUG
        olog("-------------------------------------------");
#endif

        rv = DBSelect( area, sql_str, 0, 0 );
        if( rv == E_NOTFOUND )
        {
#if DC_DEBUG
            olog("no more INDEX found");
#endif
            rv = 0;
            break;
        }

        if( rv == -1 )
        {
            elog("error: sql excute[%d][%s]", FAP_SQLCODE, sql_str );
            FreeArea();
            return -1;
        }

        DBGetData(0, index_name, sizeof(index_name));
#if DC_DEBUG
        olog("index name [%s]", index_name);
#endif
        rv = dc_load_index_column_one(_seg_addr, index_name);
        if (rv < 0)
        {
            elog("error: dc_load_index_column_one: %s", index_name);
            FreeArea();
            return -1;
        }
#if DC_DEBUG
        olog("nice, index [%s] added", index_name);
#endif
        index_num++;
    }

    FreeArea();

    return index_num;
} /* dc_load_index_column */


/*
 *  Function: 
 *      load one index definition
 *
 *  Parameters:
 *     _table_name- table name
 *
 *  Return Value:
 *      SUCCESS - 0
 *      FAILURE - -1
 */
static int dc_load_index_column_one(void *_seg_addr, char *_index_name)
{
    int  rv  = 0;
    int  idx = 0;
    int  len = 0;
    int  left = 0;
    int  area = 0;
    int  col_idx = 0;
    char col_name[64+1] = {0};
    char sql_str[256] = {0};
#if DB2
    char buffer[256] = {0};
    char *p2 = NULL;
#endif
    char *p1 = NULL;
    table_def_t *p_col = NULL;
    segment_header_t    *seg_header = NULL;
    index_def_t ind;

    seg_header = (segment_header_t *)_seg_addr;

    /*
    olog("-->>>>%s", __func__);
    */

    area = MallocArea();
    if ( -1 == area )
    {
        elog("error: MallocArea()");
        return -1;
    }

    memset(&ind, 0, sizeof(ind));
    p1   = ind.index_name;
    left = sizeof(ind.index_name);


#if ORACLE
    /* oracle */
    snprintf(sql_str, sizeof(sql_str),
            "SELECT COLUMN_NAME FROM USER_IND_COLUMNS "
            "WHERE INDEX_NAME='%s' ORDER BY COLUMN_POSITION", _index_name);
#elif DB2
    /* DB2 */
    snprintf(sql_str, sizeof(sql_str),
            "SELECT COLNAMES FROM SYSIBM.SYSINDEXES "
            "WHERE NAME='%s'", _index_name);
#endif
    dc_str_toupper(sql_str);

#if DC_DEBUG
    olog("sql_str: [%s]", sql_str);
#endif

#if DB2
    rv = DBSelect( area, sql_str, 0, 0 );
    if ( rv == E_NOTFOUND )
    {
        olog("error: no index column");
        FreeArea();
        return -1;
    }

    if ( rv == -1 )
    {
        elog("error: sql excute[%d][%s]", FAP_SQLCODE, sql_str );
        FreeArea();
        return -1;
    }

    DBGetData(0, ind.index_name, sizeof(ind.index_name));
    snprintf(buffer, sizeof(buffer), "%s", ind.index_name);
#if DC_DEBUG
    olog("DB2: index-col-names: [%s]", buffer);
#endif

    p2 = strtok(buffer, DC_INDEX_SEP);
#endif



    while (left > 1)
    {
#if DC_DEBUG
        olog("-------------------------------------------");
#endif

#if ORACLE
        rv = DBSelect( area, sql_str, 0, 0 );
        if ( rv == E_NOTFOUND )
        {
#if DC_DEBUG
            olog("ORA: No more index column");
#endif
            rv = 0;
            break;
        }

        if ( rv == -1 )
        {
            elog("error: sql excute[%s]", sql_str );
            FreeArea();
            return -1;
        }

        DBGetData(0, col_name, sizeof(col_name));
        len = snprintf(p1, left, "%s%s", DC_INDEX_SEP, col_name);
        p1   += len;
        left -= len;

#elif DB2

        if (p2 == NULL)
        {
#if DC_DEBUG
            olog("DB2: No more index column");
#endif
            rv = 0;
            break;
        }

        snprintf(col_name, sizeof(col_name), "%s", p2);

        p2 = strtok(NULL, DC_INDEX_SEP);
#endif

#if DC_DEBUG
        olog("index column [%s => %s]", col_name, ind.index_name);
#endif

        col_idx = dc_load_get_col_idx(_seg_addr, col_name);
        if (col_idx < 0)
        {
            elog("error: dc_load_get_col_idx: %s", col_name);
            return -1;
        }

        p_col = &seg_header->column[col_idx];
        if (!dc_load_is_valid_column_type(p_col->column_type))
        {
            elog("error: column type NOT support[%s, %d]", col_name, p_col->column_type);
            FreeArea();
            return -1;
        }
        ind.index_value_max += p_col->column_max_length;
#if DC_DEBUG
        olog("column max: [%s => %d, %d]",
                p_col->column_name, p_col->column_max_length, ind.index_value_max);
#endif
    } /* while */

#if ORACLE
    if ((unsigned long)(p1-ind.index_name) >= sizeof(ind.index_name))
    {
        elog("error: key exceeds: %s", ind.index_name);
        FreeArea();
        return -1;
    }
#endif

    FreeArea();

#if DC_DEBUG
    olog("index key: [%s]", ind.index_name);
    olog("value max: [%d]", ind.index_value_max);
#endif

    idx = dc_seg_add_index_meta(_seg_addr, &ind);

#if DC_DEBUG
    olog("index subscript: [%d]", idx);
#endif

    return idx;
} /* dc_load_index_column_one */



static int dc_load_calc_one_segment_size(char *_table_name, int _max_rows)
{
    int  idx = 0;
    int  row_size = 0;
    int  index_num = 0;
    int  seg_size = 0;
    int  index_key_len[TABLE_MAX_INDEXES] = {0};
    void *addr = NULL;

    /*
    olog("-->>>>%s", __func__);
    */

    memset(&G_seg_header, 0, sizeof(G_seg_header));

    addr = (void *)&G_seg_header;

    /* step1 */
    row_size = dc_load_table_column(addr, _table_name);
    if (row_size <= 0)
    {
        elog("error: dc_load_table_column: %s", _table_name);
        return -1;
    }
#if DC_DEBUG
    olog("row size: %d", row_size);
#endif

    /* step2 */
    dc_seg_set_row_size(addr, row_size);

    /* loop: load all indexes => segment_header.index */
    index_num = dc_load_index_column(addr, _table_name);
    if (index_num < 0)
    {
        elog("error: dc_load_index_column: %s", _table_name);
        return -1;
    }
#if DC_DEBUG
    olog("%d indexes loaded", index_num);
#endif

    if (index_num > TABLE_MAX_INDEXES)
    {
        elog("error: index number too much");
        return -1;
    }

    for (idx = 0; idx < index_num; idx++)
    {
        index_key_len[idx] = G_seg_header.index[idx].index_value_max;
#if DC_DEBUG
        olog("index key len [%d -- %d]", idx, index_key_len[idx]);
#endif
    }

    seg_size = dc_seg_calc_size(_max_rows, row_size, index_num, index_key_len);
#if DC_DEBUG
    olog("segment size: [%ld]", seg_size);
#endif

    return seg_size;
} /* dc_load_calc_one_segment_size */



/*
 *  Function: 
 *      include table column and index column
 *      WARN: this function must be with preceding dc_load_calc_one_segment_size()
 *
 *  Parameters:
 *     _seg_addr - table name
 *
 *  Return Value:
 *      SUCCESS - 0
 *      FAILURE - -1
 */
static int dc_load_table_structure(void *_seg_addr)
{
    int  rv = 0;
    int  idx = 0;
    int  index_num = 0;
    segment_header_t *seg_header = NULL;

    /*
    olog("-->>>>%s", __func__);
    */

    seg_header = (segment_header_t *)_seg_addr;
    index_num = seg_header->index_num;
#if DC_DEBUG
    olog("total index number: %d", index_num);
#endif

    /* iterate allocate index area */
    for (idx = 0; idx < index_num; idx++)
    {
        rv = dc_seg_add_index_area(_seg_addr, idx);
        if (rv)
        {
            elog("error: dc_seg_add_index_area: %d", idx);
            return -1;
        }
    }
#if DC_DEBUG
    olog("index area loaded");
#endif

    /* allocate data area  */
    if (dc_seg_add_data_area(_seg_addr))
    {
        elog("error: dc_seg_add_data_area");
        rv = -1;
    }

#if DC_DEBUG
    olog("data area loaded");
#endif

    /* iterate layout index */
    for (idx = 0; idx < index_num; idx++)
    {
#if DC_DEBUG
        olog("-------------------------------------------");
        olog("layout index[%d]", idx);
#endif
        rv = dc_seg_layout_index_area(_seg_addr, idx, RB_INSERT_FUNCTION_STRING);
        if (rv)
        {
            elog("error: dc_seg_layout_index_area: %d", idx);
            return -1;
        }
#if DC_DEBUG
        olog("-------------------------------------------");
#endif
    }
#if DC_DEBUG
    olog("index area layouted");
#endif

    rv = dc_seg_layout_data_area(_seg_addr);
    if (rv)
    {
        elog("error: dc_seg_layout_data_area: %d", idx);
        return -1;
    }
#if DC_DEBUG
    olog("data area layouted");
#endif

    return rv;
} /* dc_load_table_structure */


/*
 *  Function: 
 *      load all rows of table to shared memory
 *
 *  Parameters:
 *     _table_name- table name
 *
 *  Return Value:
 *      SUCCESS - 0
 *      FAILURE - -1
 */
static int dc_load_table_data(void *_seg_addr, char *_table_name)
{
    int  rv  = 0;
    int  area = 0;
    int  row_size = 0;
    char sql_str[256] = {0};
    void *row = NULL;
    segment_header_t    *seg_header = NULL;

    seg_header = (segment_header_t *)_seg_addr;

    /*
    olog("-->>>>%s", __func__);
    */

    snprintf(sql_str, sizeof(sql_str), "SELECT * FROM %s", _table_name);
#if DC_DEBUG
    olog("sql_str[%s]", sql_str);
#endif

    area = MallocArea();
    if ( -1 == area )
    {
        elog("error: MallocArea()");
        return -1;
    }

    row_size  = seg_header->row_size;

    /* allocate row size */
    row = calloc(row_size, 1);
    if (row == NULL)
    {
        elog("error: calloc failure");
        FreeArea();
        return -1;
    }

    while (1)
    {
#if DC_DEBUG
        olog("-------------------------------------------");
#endif

        rv = DBSelect(area, sql_str, 0, 0 );
        if( rv == E_NOTFOUND )
        {
#if DC_DEBUG
            olog("No more data found");
#endif
            rv = 0;
            break;
        }

        if( rv == -1 )
        {
            elog("error: sql execute [%d][%s]", FAP_SQLCODE, sql_str);
            free(row);
            row = NULL;
            FreeArea();
            return -1;
        }

        memset(row, 0, row_size);
        DBGetStru(0, row, row_size);

        rv = dc_load_table_row(_seg_addr, row, row_size);
        if (rv)
        {
            elog("error: dc_load_table_row");
            free(row);
            row = NULL;
            FreeArea();
            return -1;
        }

    }

    FreeArea();

    if (row != NULL)
    {
        free(row);
        row = NULL;
    }

    return rv;
} /* dc_load_table_data */


/*
 *  Function: 
 *      load a row of table to shared memory
 *   -- get index key from row
 *
 *  Parameters:
 *     _seg_addr - table name
 *
 *  Return Value:
 *      SUCCESS - 0
 *      FAILURE - -1
 */
static int dc_load_table_row(void *_seg_addr, void *_row, int _len)
{
    int  len = 0;
    int  left = 0;
    int  ind_idx = 0;
    int  col_idx = 0;
    int  index_num = 0;
    int  key_max = 0;
    long row_size = 0;
    char *p = NULL;
    char *p_key = NULL;
    void *data_unit = NULL;

    index_def_t ind;
    table_def_t         *p_col = NULL;
    rb_tree_node_t      *node = NULL;
    segment_header_t    *seg_header = NULL;

    seg_header = (segment_header_t *)_seg_addr;

    /*
    olog("-->>>>%s", __func__);
    */

    index_num = seg_header->index_num;

    row_size = seg_header->row_size;


    if (index_num == 0)
    {
        /* No index case */

        /* extract a node from data-free-list */
        data_unit = dc_seg_data_alloc(_seg_addr);
        if (data_unit == NULL)
        {
            elog("error: dc_seg_data_alloc: %d");
            return -1;
        }
#if DC_DEBUG
        olog("pure data allocated: %p", data_unit);
#endif

        /* save data */
        dc_seg_data_insert(_seg_addr, data_unit, _row, _len);

    }
    else
    {
#if DC_DEBUG
        olog("with index: %d", index_num);
#endif

        /* fill every index */
        for (ind_idx = 0; ind_idx < index_num; ind_idx++)
        {
#if DC_DEBUG
            olog("-------------------------------------------");
#endif
            /* copy for strtok() */
            memcpy(&ind, &seg_header->index[ind_idx], sizeof(ind));
            key_max = ind.index_value_max;
#if DC_DEBUG
            olog("--- index[%d]: [%s, max(%d)] ---", ind_idx, ind.index_name, key_max);
#endif

            /* allocate a node */
            node = (rb_tree_node_t*)dc_seg_index_alloc(_seg_addr, ind_idx);
            if (node == NULL)
            {
                elog("error: dc_seg_index_alloc: %d", ind_idx);
                return -1;
            }
#if DC_DEBUG
            olog("index node allocated: %p", node);
#endif

            /* data unit */
            node->data_len  = _len;

            /* reverse found */
            dc_sec_reversed_link(node->data_unit, ind_idx, node);

            /* XXX: the first index special treat */
            if (ind_idx == 0)
            {
                /* delete from data-free-list */
                dc_seg_data_delete(node->data_unit);

                /* save data and to data-used-list */
                dc_seg_data_insert(_seg_addr, node->data_unit, _row, _len);
            }

            /* to assign value */
            p_key = node->key_val;
            left  = key_max + 1;

            p = strtok(ind.index_name, DC_INDEX_SEP);
            while (p != NULL)
            {
#if DC_DEBUG
                olog("index.column(%s)", p);
#endif

                if (left <= 1)
                {
                    elog("error: no more space to save key");
                    return -1;
                }

                /* get column def */
                col_idx = dc_load_get_col_idx(_seg_addr, p);
                if (col_idx < 0)
                {
                    elog("error: dc_load_get_col_idx: %s", p);
                    return -1;
                }
                p_col = &seg_header->column[col_idx];

                /* XXX: more data type */
                len = snprintf(p_key, left, "%s", (char*)_row+p_col->column_offset);
                p_key += len;
                left  -= len;
                node->key_len += len;
#if DC_DEBUG
                olog("key: [%s]", node->key_val);
#endif

                p = strtok(NULL, DC_INDEX_SEP);
            } /* while */

            dc_seg_index_insert(_seg_addr, ind_idx, node);   /* no return value */

        } /* for */
    }

    seg_header->rows_free--;
    seg_header->rows_used++;

    return 0;
} /* dc_load_table_row */


/*
 *  Function: 
 *      query column by column name
 *      TODO: improve performance
 *
 *  Parameters:
 *     _seg_addr - table name
 *
 *  Return Value:
 *      SUCCESS - 0
 *      FAILURE - -1
 */
int dc_load_get_col_idx(void *_seg_addr, char *_col_name)
{
    int i   = 0;
    segment_header_t    *seg_header = NULL;

    seg_header = (segment_header_t *)_seg_addr;

    /*
    olog("-->>>>%s", __func__);
    */

    for (i = 0; i < seg_header->column_num; i++)
    {
        if (strcasecmp(_col_name, seg_header->column[i].column_name) == 0)
        {
#if DC_DEBUG
            olog("column index: [%s => %d]", _col_name, i);
#endif
            return i;
        }
        else
        {
            /*
            olog("not match: [%s, %s]", _col_name, seg_header->column[i].column_name);
            */
        }
    }

#if DC_DEBUG
    elog("error: not found: %s", _col_name);
#endif

    return -1;
} /* dc_load_get_col_idx */


/* XXX */
int dc_load_is_valid_column_type(int _type)
{
#if DC_DEBUG
    olog("-->>>>%s", __func__);

    olog("column type: %d, is good!", _type);
#endif

    return 1;
} /* dc_load_is_valid_column_type */



static int dc_load_calc_all_segment_size()
{
    int  idx = 0;
    int  max_rows = 0;
    int  seg_size = 0;
    int  sum = 0;
    char *name = NULL;

    /*
    olog("-->>>>%s", __func__);
    */

    dc_aux_seg_iterator_init();

    while ((idx = dc_aux_seg_iterator_next(&name, &max_rows)) >= 0)
    {
        /*
        olog("[%02d] => [%s, %d]", idx, name, max_rows);
        */
        seg_size = dc_load_calc_one_segment_size(name, max_rows);
        if (seg_size <= 0)
        {
            elog("error: dc_load_calc_one_segment_size failure");
            return -1;
        }
#if DC_DEBUG
        olog("segment[%s] size: [%d]", name, seg_size);
#endif

        if (dc_aux_seg_set(idx, seg_size))
        {
            elog("error: dc_aux_seg_set: %d, %d", idx, seg_size);
            return -1;
        }

        name = NULL;
        max_rows = 0;
    }

    sum = dc_aux_seg_sum_size();
#if DC_DEBUG
    olog("sum size: %d", sum);
#endif

    if (sum > 0)
    {
        g_all_segment_size = sum;
    }

    return sum;
}


int dc_load_calc_shm_size()
{
    int  all_seg_size = 0;
    int  blk_size = 0;
    int  shm_size = 0;

    /*
    olog("-->>>>%s", __func__);
    */

    all_seg_size = dc_load_calc_all_segment_size();
    if (all_seg_size <= 0)
    {
        elog("error: dc_load_calc_all_segment_size");
        return -1;
    }
    /*
    olog("all segment size: %d", all_seg_size);
    */

    blk_size = dc_blk_calc_size(all_seg_size);
    /*
    olog("one block size: %d", blk_size);
    */

    shm_size = dc_shm_calc_size(blk_size);

    return shm_size;
}


#if RUN_LOAD
static int dc_load_input()
{
    int  max_rows = 0;
    char table_name[32] = {0};

    dc_aux_init();

    /* external (user) invoke */
    snprintf(table_name, sizeof(table_name), "%s", "TBL_LOG_COUNTER");
    max_rows = 10;
    if (dc_aux_seg_add(table_name, max_rows))
    {
        elog("error: dc_aux_seg_add: %s", table_name);
        return -1;
    }

    snprintf(table_name, sizeof(table_name), "%s", "TBL_CFG_TXN");
    max_rows = 100;
    if (dc_aux_seg_add(table_name, max_rows))
    {
        elog("error: dc_aux_seg_add: %s", table_name);
        return -1;
    }

    snprintf(table_name, sizeof(table_name), "%s", "TBL_CFG_LINE");
    max_rows = 100;
    if (dc_aux_seg_add(table_name, max_rows))
    {
        elog("error: dc_aux_seg_add: %s", table_name);
        return -1;
    }

    return 0;
}

static int dc_load_function_test(int argc, char *argv[])
{
    int  rv  = 0;
    int  seg_idx = 0;
    int  ind_idx = 0;
    int  to_read   = 0;
    int  to_create = 0;
    int  to_write  = 0;
    long tm = 0;
    void *blk_a    = NULL;
    void *seg_addr = NULL;
    char table_name[32] = {0};
    segment_header_t *header = NULL;

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

    /* segment */
    if (to_create)
    {
        olog("let's create a new segment");

        if (dc_blk_switch())
        {
            elog("error: dc_blk_switch");
            return -5;
        }

        blk_a = dc_blk_get_read_addr();
        olog("blk addr: %p", blk_a);

        if (dc_load_all_tables(blk_a))
        {
            elog("error: dc_load_all_tables");
            return -1;
        }
    }
    else
    {
        blk_a = dc_blk_get_read_addr();
        olog("blk addr: %p", blk_a);
    }

    /* segment_addr */
    seg_addr = dc_blk_get_segment_addr(blk_a, seg_idx);
    if (seg_addr == NULL)
    {
        elog("error: dc_blk_get_segment_addr[%d] failure", seg_idx);
        return -3;
    }
    olog("segment[%d] address: %p", seg_idx, seg_addr);

    header = (segment_header_t *)seg_addr;

    if (to_read)
    {
        olog("READER");
        dc_seg_dump_index_area(seg_addr, ind_idx);
        dc_seg_dump_data_area(seg_addr);
    }

    if (to_write)
    {
        olog("let's refresh a segment");

        if (DBOpen(3, "", "toplink", "toplink") == -1)
        {
            elog("error: DBOpen[%ld]", FAP_SQLCODE );
            return -1;
        }

        if (dc_load_init())
        {
            elog("error: dc_load_init");
            return -4;
        }
        snprintf(table_name, sizeof(table_name), "%s", "TBL_LOG_COUNTER");

        tm = get_time();
        if (dc_load_table_refresh(blk_a, table_name, tm))
        {
            elog("error: dc_load_table_refresh: %s", table_name);
            return -4;
        }

        olog("refresh table %s succeeds", table_name);

        olog("++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++");
    }

    return rv;
}

int main(int argc, char *argv[])
{
    int  rv  = 0;
    int  err = 0;
    int  to_create = 0;
    int  shm_id   = 0;
    int  shm_key  = 7788;
    int  all_segment_size = 0;
    long shm_size = 0;
    void *shm_addr = NULL;
    long flag = 0;

    olog("load begin");

#if 0
    if (argc == 3)
    {
        if (memcmp(argv[2], "C", 1) == 0)
        {
            olog("to_create");
            to_create = 1;
        }
    }

    if (to_create)
    {
        flag = SHM_CREATE;

        if (dc_load_input())
        {
            elog("error: dc_load_input");
            return -5;
        }

        if (DBOpen(3, "", "toplink", "toplink") == -1)
        {
            elog("error: DBOpen[%ld]", FAP_SQLCODE );
            return -1;
        }

        if (dc_load_init())
        {
            elog("error: dc_load_init");
            return -4;
        }

        all_segment_size = dc_load_calc_all_segment_size();
        if (all_segment_size <= 0)
        {
            elog("error: dc_load_calc_all_segment_size");
            return -1;
        }
        olog("all segment size: %d", all_segment_size);

        shm_size = dc_load_calc_shm_size();
        if (shm_size <= 0)
        {
            elog("error: dc_load_calc_shm_size");
            return -1;
        }
        olog("total shm size: %d", shm_size);
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

    /* blk */
    dc_blk_init(shm_addr);

    /* TEST */
    rv = dc_load_function_test(argc, argv);
    if (rv)
    {
        elog("error: dc_load_function_test");
        return -1;
    }
#endif

    int  i = 0;
    int  col_num = 0;
    char table[32+1] = {0};
    long row_size = 0;
    table_def_t col;

    if (dc_ext_connect_db())
    {
        elog("error: dc_ext_connect_db");
        return -1;
    }

    /* XXX: dc_load_table_column_v0:  don't support empty table */


    /* version2:  support empty table */
    strcpy(table, "tbl_log_empty1");
    if (DBGetTabDesc(table, &col_num))
    {
        elog("error: DBGetTabDesc: %d, %s", FAP_SQLCODE, table);
        return -1;
    }
    olog("column number: %d", col_num);

    row_size = db_get_rowsize_v2(col_num);
    olog("row size: %ld", row_size);

    for (i = 0; i < col_num; i++)
    {
        memset(&col, 0, sizeof(col));
        col.column_offset = db_get_column_v2(i, col.column_name, sizeof(col.column_name),
                &col.column_type, &col.column_max_length);
        olog("column: [%s, %d, %d, %ld]",
                col.column_name, col.column_type, col.column_max_length, col.column_offset);
    }

    olog("++++++++++++++++++++++++++++++++++++++++++++++++++++++");
    /* XXX: dc_load_table_column:  support empty table */
    strcpy(table, "tbl_log_counter");
#if 0
    if (dc_load_table_column(NULL, table) <= 0)
    {
        elog("error: dc_load_table_column: %s", table);
        return -1;
    }
#endif


    olog("load end");

    return rv;
}
#endif

/* load.c */
