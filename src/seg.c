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


/* ================================================================= */
/* ================================================================= */


/* ================================================================= 
   1. init segment head 
   2. table definition: add column * M
   3. add  index1, 2, 3: 
        node_size = sizeof(rb_tree_node_t) + key_len
   4. init data area
   ================================================================= */


/*
 *  Function:  dc_seg_init
 *      layout a segment
 *
 *  Parameters:
 *      _seg_addr - segment head address
 *      _seg_idx  - the index of segment in block
 *      _max_rows - estimated rows
 *  _segment_size - size of segment, including segment header
 *
 *  Return Value:
 *      SUCCESS - shmid
 *      FAILURE - failure
 */
int dc_seg_init(void *_seg_addr, int _seg_idx, int _max_rows, long _segment_size)
{
    segment_header_t    *seg_header = NULL;

    seg_header = (segment_header_t *)_seg_addr;

#if DC_DEBUG
    olog("rows: %d", _max_rows);
#endif
    seg_header->segment_idx = _seg_idx;
    seg_header->rows_free   = _max_rows;
    seg_header->rows_used   = 0;

    seg_header->size_available = _segment_size - sizeof(segment_header_t);
    seg_header->size_allocated = 0;

    /* XXX: should be discarded. 2017-11-3 */
    seg_header->t2             = get_time();

#if DC_DEBUG
    olog("avai size: [%ld = %ld - %ld]", seg_header->size_available, _segment_size, sizeof(segment_header_t));
#endif

    return 0;
} /* dc_seg_init */


/* 2017-11-3: XXX: should be discarded */
int dc_seg_gen_timestamp(void *_seg_addr)
{
    long old = 0;
    segment_header_t    *seg_header = NULL;

    seg_header = (segment_header_t *)_seg_addr;

    old = seg_header->t2;
    seg_header->t2 = get_time();
#if DC_DEBUG
    olog("t2: [%ld => %ld]", old, seg_header->t2);
#endif

    return 0;
} /* dc_seg_gen_timestamp */

/*
 *  Function: 
 *      Add a column to define the table,
 *      usually is invoked N times.
 *      Only affect segment header.
 *
 *  Parameters:
 *     _seg_addr  - the address of segment
 *     _column    - the definition of column
 *
 *  Return Value:
 *      SUCCESS - 0
 *      FAILURE - -1
 */
int dc_seg_add_column(void *_seg_addr, table_def_t *_column)
{
    int  idx = 0;
    segment_header_t    *seg_header = NULL;

    seg_header = (segment_header_t *)_seg_addr;

    /*
    olog("-->>>>%s", __func__);
    */

    if (seg_header->column_num >= TABLE_MAX_COLUMNS)
    {
        elog("error: column exceeds: [%d]", seg_header->column_num);
        return -1;
    }

    idx = seg_header->column_num;
#if DC_DEBUG
    olog("column idx: %d", idx);
#endif
    memcpy(&seg_header->column[idx], _column, sizeof(table_def_t));

    seg_header->column_num += 1;

    return idx;
} /* dc_seg_add_column */


/*
 *  Function: 
 *      set row size
 *
 *  Parameters:
 *     _seg_addr  - the address of segment
 *
 *  Return Value:
 *      SUCCESS - 0
 *      FAILURE - -1
 */
int dc_seg_set_row_size(void *_seg_addr, int _row_size)
{
    segment_header_t *seg_header = NULL;

    seg_header = (segment_header_t *)_seg_addr;

    /* segment header */
    seg_header->row_size    = DC_CEIL_ALIGN(_row_size, DC_WORD_WIDTH);
    seg_header->unit_size   = dc_sec_calc_size(seg_header->row_size);
#if DC_DEBUG    
    olog("row_size  aligned: %d -> %d", _row_size, seg_header->row_size);
    olog("unit_size aligned: %d -> %d", seg_header->row_size, seg_header->unit_size);
#endif

    return 0;
}


/*
 *  Function: 
 *      Add a index to the segment
 *      usually is invoked N times.
 *
 *  Parameters:
 *     _seg_addr  - the address of segment
 *
 *  Return Value:
 *      SUCCESS - idx
 *      FAILURE - -1
 */
int dc_seg_add_index_meta(void *_seg_addr, index_def_t *_index)
{
    int  idx = 0;
    segment_header_t    *seg_header = NULL;

    seg_header = (segment_header_t *)_seg_addr;

    if (seg_header->index_num >= TABLE_MAX_INDEXES)
    {
        elog("error: indexes exceed: [%d]", seg_header->index_num);
        return -1;
    }

    idx  = seg_header->index_num;

    /* segment header */
    memcpy(&seg_header->index[idx], _index, sizeof(index_def_t));

    seg_header->index_num += 1;

    return idx;
}



/*
 *  Function: 
 *      Add index area to the segment
 *
 *  Parameters:
 *     _seg_addr  - the address of segment
 *
 *  Return Value:
 *      SUCCESS - 0
 *      FAILURE - -1
 */
int dc_seg_add_index_area(void *_seg_addr, int _idx)
{
    int  rows = 0;
    int  node_size = 0;
    int  key_max_len = 0;
    long index_size = 0;
    segment_header_t    *seg_header = NULL;

    seg_header = (segment_header_t *)_seg_addr;

    if (_idx >= seg_header->index_num)
    {
        elog("error: indexes exceed: [%d, %d]", _idx, seg_header->index_num);
        return -1;
    }

    /* total */
    rows = seg_header->rows_free + seg_header->rows_used;
#if DC_DEBUG
    olog("rows: %d", rows);
#endif

    key_max_len = seg_header->index[_idx].index_value_max;

    /* segment header */
    seg_header->tree[_idx].key_max_len = key_max_len;

    node_size = sizeof(rb_tree_node_t) + key_max_len;
    node_size = DC_CEIL_ALIGN(node_size, DC_WORD_WIDTH);
    seg_header->tree[_idx].node_size   = node_size;
#if DC_DEBUG
    olog("node size: %d", node_size);
#endif

    index_size = node_size * rows;
#if DC_DEBUG
    olog("index[%d] size: %d", _idx, index_size);
#endif

    if (seg_header->size_available < index_size)
    {
        elog("error: available[%ld] < required[%ld]", seg_header->size_available, index_size);
        return -1;
    }

    seg_header->index[_idx].offset     = seg_header->size_allocated;
    seg_header->index[_idx].index_addr = _seg_addr + sizeof(segment_header_t) + seg_header->index[_idx].offset;
    seg_header->tree[_idx].head_addr   = seg_header->index[_idx].index_addr;
#if DC_DEBUG
    olog("index[%d] addr: %p", _idx, seg_header->index[_idx].index_addr);
#endif

    seg_header->size_allocated  += index_size;
    seg_header->size_available  -= index_size;

    return 0;
}


/*
 *  Function: 
 *      Add data area to the segment
 *
 *  Parameters:
 *     _seg_addr  - the address of segment
 *
 *  Return Value:
 *      SUCCESS - 0
 *      FAILURE - -1
 */
int dc_seg_add_data_area(void *_seg_addr)
{
    int  rv = 0;
    int  rows = 0;
    long data_size = 0;
    segment_header_t *seg_header = NULL;

    seg_header = (segment_header_t *)_seg_addr;

    /*
    olog("-->>>>%s", __func__);
    */

    /* total */
    rows = seg_header->rows_free + seg_header->rows_used;
#if DC_DEBUG
    olog("rows2: %d", rows);
#endif

    /* data area */
    data_size = seg_header->unit_size * rows;
#if DC_DEBUG
    olog("data size: %d", data_size);
#endif

    if (seg_header->size_available < data_size)
    {
        elog("error: available[%ld] < required[%ld]", seg_header->size_available, data_size);
        return -1;
    }

    seg_header->data_offset  = seg_header->size_allocated;
    seg_header->data_addr    = _seg_addr + sizeof(segment_header_t) + seg_header->data_offset;

    seg_header->size_allocated  += data_size;
    seg_header->size_available  -= data_size;
#if DC_DEBUG
    olog("after data allocate, avai: [%ld]", seg_header->size_available);
#endif

    return rv;
}


/*
 *  Function: 
 *      layout the structure of index and data
 *      usually is invoked N times.
 *
 *  Parameters:
 *     _seg_addr  - the address of segment
 *     _idx       - index of index
 *     _index_type- index type: RB_INSERT_FUNCTION_STRING, RB_INSERT_FUNCTION_LONG
 *
 *  Return Value:
 *      SUCCESS - 0
 *      FAILURE - -1
 */
int dc_seg_layout_index_area(void *_seg_addr, int _idx, int _index_type)
{
    int  i  = 0;
    int  rv = 0;
    int  rows = 0;
    int  node_size = 0;
    int  unit_size  = 0;
    void *index_addr = NULL;
    void *data_addr  = NULL;
    segment_header_t    *seg_header = NULL;
    rb_tree_node_t      *this_node = NULL;
    rb_tree_node_t      *next_node = NULL;
    rb_tree_t           *tree = NULL;

    seg_header = (segment_header_t *)_seg_addr;

    if (_idx >= seg_header->index_num)
    {
        elog("error: index exceed: [%d, %d]", _idx, seg_header->index_num);
        return -1;
    }

    tree      = &(seg_header->tree[_idx]);
    node_size = tree->node_size;
    unit_size = seg_header->unit_size;
#if DC_DEBUG
    olog("[%d]: node size: [%d], unit size: [%d]", _idx, node_size, unit_size);
#endif

    index_addr = _seg_addr + sizeof(segment_header_t) + seg_header->index[_idx].offset;
#if DC_DEBUG
    olog("[%d]: index_addr1: [%p]", _idx, index_addr);
    olog("[%d]: index_addr2: [%p]", _idx, seg_header->index[_idx].index_addr);
#endif

    data_addr  = _seg_addr + sizeof(segment_header_t) + seg_header->data_offset;
#if DC_DEBUG
    olog("[%d]: data_addr1:  [%p]", _idx, data_addr);
    olog("[%d]: data_addr2:  [%p]", _idx, seg_header->data_addr);
#endif

    rows = seg_header->rows_free;
#if DC_DEBUG
    olog("[%d]: rows:  [%d]", _idx, rows);
#endif

    /* change header area */
    tree->free_list = index_addr;
    tree->free_num  = rows;
    tree->used_num  = 0;

#if DC_DEBUG
    olog("free_list: %p", tree->free_list);
#endif

    /* linked list of index area */
    this_node = index_addr;
    for (i = 0; i < rows-1; i++)
    {
#if DC_DEBUG
        olog("--[%d]: this_node: %p", i, this_node);
#endif
        next_node = (void *)this_node + node_size;

        this_node->data_unit = data_addr;
        this_node->link      = next_node;


        data_addr += unit_size;
        this_node = next_node;
    }

    /* the last one */
    {
#if DC_DEBUG
        olog("--[%d]: this_node: %p", i, this_node);
#endif
        this_node->data_unit = data_addr;
        this_node->link      = NULL;
    }

#if DC_DEBUG
    olog("[%d]: index area linked", _idx);
#endif

    /* binary tree of index area */
    rb_tree_init(tree, &tree->sentinel_instance, _index_type);
#if DC_DEBUG
    olog("[%d]: rb_tree inited", _idx);
#endif

    return rv;
} /* dc_seg_layout_index_area */


/*
 *  Function: 
 *      layout the structure of data area
 *      only is invoked 1 time.
 *
 *  Parameters:
 *     _seg_addr  - the address of segment
 *
 *  Return Value:
 *      SUCCESS - 0
 *      FAILURE - -1
 */
int dc_seg_layout_data_area(void *_seg_addr)
{
    int  i  = 0;
    int  rv = 0;
    int  rows = 0;
    int  row_size  = 0;
    int  unit_size  = 0;
    void *data_addr  = NULL;
    List                *list = NULL;
    void                *this_unit = NULL;
    segment_header_t    *seg_header = NULL;

    seg_header = (segment_header_t *)_seg_addr;

    /* init used list head */
    list = &seg_header->data_used_list;
    list->head = &seg_header->data_used_head;
    INIT_LIST_HEAD(list->head);

    /* init free list head */
    list = &seg_header->data_free_list;
    list->head = &seg_header->data_free_head;
    INIT_LIST_HEAD(list->head);


    row_size   = seg_header->row_size;
#if DC_DEBUG
    olog("row  size: [%d]", row_size);
#endif

    unit_size  = seg_header->unit_size;
#if DC_DEBUG
    olog("unit size: [%d]", unit_size);
#endif

    data_addr  = _seg_addr + sizeof(segment_header_t) + seg_header->data_offset;
#if DC_DEBUG
    olog("data_addr1:  [%p]", data_addr);
    olog("data_addr2:  [%p]", seg_header->data_addr);
#endif

    rows = seg_header->rows_free;
#if DC_DEBUG
    olog("rows:  [%d]", rows);
#endif

    /* linked list of index area */
    this_unit = data_addr;
    for (i = 0; i < rows; i++)
    {
#if DC_DEBUG
        olog("--[%d]: this_unit: %p", i, this_unit);
#endif

        /* for uniq RS */
        dc_sec_set_id(this_unit, i);

        dc_sec_insert(list, this_unit);

        data_addr += unit_size;
        this_unit  = data_addr;
    }

#if DC_DEBUG
    olog("data area linked");
#endif

    return rv;
} /* dc_seg_layout_data_area */


/**
 * extract a node from the index free-list
 */
void *dc_seg_index_alloc(void *_seg_addr, int _idx)
{
    segment_header_t    *seg_header = NULL;
    rb_tree_t           *tree = NULL;
    rb_tree_node_t      *p = NULL;

    seg_header = (segment_header_t *)_seg_addr;

    if (_idx >= seg_header->index_num)
    {
        elog("error: index exceed: [%d, %d]", _idx, seg_header->index_num);
        return NULL;
    }

    tree = &(seg_header->tree[_idx]);

    p = tree->free_list;
    if (p != NULL && tree->free_num > 0)
    {
        p->key      = 0;
        p->key_len  = 0;
        p->data_len = 0;
        memset(p->key_val, 0, tree->key_max_len+1);

        if (seg_header->row_size > 0)
        {
            /* memset to 0 */
            dc_sec_memset(p->data_unit, seg_header->row_size);
        }

        tree->free_list = tree->free_list->link;

        tree->free_num--;
    }
    else
    {
        elog("error: free_list is NULL, %d", tree->free_num);
    }

    return p;
} /* dc_seg_index_alloc */


/**
 * insert a node to the tree
 */
void dc_seg_index_insert(void *_seg_addr, int _idx, rb_tree_node_t *_node)
{
    segment_header_t    *seg_header = NULL;
    rb_tree_t           *tree = NULL;

    seg_header = (segment_header_t *)_seg_addr;

    if (_idx >= seg_header->index_num)
    {
        elog("error: index exceed: [%d, %d]", _idx, seg_header->index_num);
        return ;
    }

    tree = &(seg_header->tree[_idx]);

#if DC_DEBUG
    olog("before insert: used-total(%d)", tree->used_num);
#endif

    rb_tree_insert(tree, _node);

#if DC_DEBUG
    olog("after  insert: used-total(%d)", tree->used_num);
#endif

    return ;
} /* dc_seg_index_insert */


/**
 *  take node back to the free-list
 */
int dc_seg_index_free(void *_seg_addr, int _idx, rb_tree_node_t *_node)
{
    segment_header_t    *seg_header = NULL;
    rb_tree_t           *tree = NULL;

    seg_header = (segment_header_t *)_seg_addr;

    if (_node == NULL)
    {
        return -1;
    }

    if (_idx >= seg_header->index_num)
    {
        elog("error: index exceed: [%d, %d]", _idx, seg_header->index_num);
        return -1;
    }

    tree = &(seg_header->tree[_idx]);

    _node->key       = 0;
    _node->left      = NULL;
    _node->right     = NULL;
    _node->parent    = NULL;
    _node->data_len  = 0;

    if (_node->data_len > 0)
    {
        dc_sec_memset(_node->data_unit, _node->data_len);
    }

    if (_node->key_len > 0)
    {
        memset(_node->key_val, 0, _node->key_len);
        _node->key_len = 0;
    }

    _node->link      = tree->free_list;
    tree->free_list = _node;

    tree->free_num--;

    return 0;
} /* dc_seg_index_free */


/**
 * extract a node from the data free-list
 * for no index case
 */
void *dc_seg_data_alloc(void *_seg_addr)
{
    void                *addr = NULL;
    List                *list = NULL;
    segment_header_t    *seg_header = NULL;

    seg_header = (segment_header_t *)_seg_addr;

    list = &seg_header->data_free_list;

    addr = dc_sec_extract(list);
    if (addr == NULL)
    {
        elog("error: dc_sec_extract");
    }

    return addr;
}


/**
 * 1. copy data to unit area
 * 2. insert to used list
 */
int dc_seg_data_insert(void *_seg_addr, void *_data_unit, void *_row, int _len)
{
    List                *list = NULL;
    segment_header_t    *seg_header = NULL;

    seg_header = (segment_header_t *)_seg_addr;

    list = &seg_header->data_used_list;

    /* copy data to it */
    dc_sec_memcpy(_data_unit, _row, _len);

    /* insert to data-used-list */
    dc_sec_insert(list, _data_unit);

    return 0;
}

/**
 * delete the node from the data free-list
 */
int dc_seg_data_delete(void *_data_unit)
{
    return dc_sec_delete(_data_unit);
}


/**
 *  calculate segment size:
 *     sizeof(segment_header) + size(index1) + size(index2) + ... size(indexN) + size(data)
 *
 */
long dc_seg_calc_size(int _rows, int _row_size, int _index_num, int _key_max_len[])
{
    int  i  = 0;
    int  node_size = 0;
    int  row_size = 0;
    int  unit_size = 0;
    int  index_size = 0;
    int  data_size  = 0;
    long seg_size = 0;

    seg_size += sizeof(segment_header_t);
#if DC_DEBUG
    olog("seg_size[H]: %ld", seg_size);
#endif

    for (i = 0; i < _index_num; i++)
    {
        node_size   = sizeof(rb_tree_node_t) + _key_max_len[i];
        node_size   = DC_CEIL_ALIGN(node_size, DC_WORD_WIDTH);
        index_size  = node_size * _rows;
        seg_size   += index_size;
#if DC_DEBUG
        olog("seg_size[%d]: %ld", i, seg_size);
#endif
    } /* for */

    row_size  = DC_CEIL_ALIGN(_row_size, DC_WORD_WIDTH);
    unit_size = dc_sec_calc_size(row_size);
    data_size = unit_size * _rows;
    seg_size += data_size;
#if DC_DEBUG
    olog("seg_size[D]: %ld", seg_size);
#endif

    return seg_size;
} /* dc_seg_calc_size */


/**
 *  get timestamp from segment
 */
long dc_seg_get_timestamp(void *_seg_addr)
{
    segment_header_t *seg_header = NULL;

    seg_header = (segment_header_t *)_seg_addr;
    /*
    olog("t2[%ld]", seg_header->t2);
    */

    return seg_header->t2;
}


/**
 *  set timestamp of segment 2017-11-3
 */
void dc_seg_set_timestamp(void *_seg_addr, long _t)
{
    segment_header_t *seg_header = NULL;

    seg_header = (segment_header_t *)_seg_addr;

    seg_header->t2  = _t;
    return;
}


/**
 *  get timestamp by table name
 *  external API
 */
long dc_seg_get_timestamp_by_name(char *_segment_name)
{
    long  timestamp = 0;
    void *blk_addr = NULL;
    void *seg_addr = NULL;

    blk_addr = dc_blk_get_read_addr();
    if (blk_addr == NULL)
    {
        elog("error: dc_blk_get_read_addr");
        return -1;
    }
    /*
    olog("blk addr: %p", blk_addr);
    */

    seg_addr = dc_blk_get_segment_addr_by_name(blk_addr, _segment_name);
    if (seg_addr == NULL)
    {
        elog("error: dc_blk_get_segment_addr_by_name: %s", _segment_name);
        return -1;
    }

    timestamp = dc_seg_get_timestamp(seg_addr);
    olog("%s => t2[%ld]", _segment_name, timestamp);

    return timestamp;
}


/**
 *  get the whole size of a segment
 */
long dc_seg_get_segment_size(void *_seg_addr)
{
    long size = 0;
    segment_header_t    *seg_header = NULL;

    seg_header = (segment_header_t *)_seg_addr;

    size = seg_header->size_available + seg_header->size_allocated + sizeof(segment_header_t);

    return size;
}


/**
 *  get max rows of a segment
 */
int dc_seg_get_segment_rows(void *_seg_addr)
{
    int rows = 0;
    segment_header_t    *seg_header = NULL;

    seg_header = (segment_header_t *)_seg_addr;

    rows = seg_header->rows_free + seg_header->rows_used;

    return rows;
}


/*
 *  Function: 
 *      dump the structure of index and data
 *
 *  Parameters:
 *     _seg_addr  - the address of segment
 *     _idx       - index of index
 *
 *  Return Value:
 *      SUCCESS - 0
 *      FAILURE - -1
 */
int dc_seg_dump_index_area(void *_seg_addr, int _idx)
{
    rb_tree_t           *tree = NULL;
    segment_header_t    *seg_header = NULL;

    seg_header = (segment_header_t *)_seg_addr;

    if (seg_header->index_num == 0)
    {
        olog("no index");
        return 0;
    }
    else if (_idx >= seg_header->index_num)
    {
        elog("error: index exceed: [%d, %d]", _idx, seg_header->index_num);
        return 0;
    }

    tree = &(seg_header->tree[_idx]);

    olog("let's dump index: [%d]", _idx);
    rb_tree_iterate_middle(tree->root, tree->sentinel);

    return 0;
}


int dc_seg_dump_data_area(void *_seg_addr)
{
    int i = 0;
    List                *list = NULL;
    dc_sec_t            *p = NULL;
    segment_header_t    *seg_header = NULL;

    seg_header = (segment_header_t *)_seg_addr;

    list = &seg_header->data_used_list;

    olog("let's dump data-used-list");

    list_for_each_entry(p, list->head, link)
    {
        olog("[%04d] => [%s, %p, %d]", i, p->row, p->index_node[0], p->len);

        i++;
    }

    return 0;
}



#if RUN_SEG
int main(int argc, char *argv[])
{
    int  rv  = 0;
    int  err = 0;
    int  col_idx = 0;
    int  seg_idx = 0;
    int  ind_idx = 0;
    int  to_read   = 0;
    int  to_create = 0;
    int  to_write  = 0;
    int  to_refresh = 0;
    int  shm_id   = 0;
    int  shm_key  = 1222;
    int  max_rows = 0;
    int  row_size = 0;
    int  index_num = 0;
    int  index_key_len[5];
    long seg_size = 1024;
    long shm_size = 20000;
    void *shm_addr = NULL;
    void *blk_a    = NULL;
    void *seg_addr = NULL;
    long flag = 0;
    table_def_t col;
    index_def_t ind;
    rb_tree_node_t *node = NULL;
    segment_header_t *header = NULL;
    char *p = NULL;

    olog("shm begin");

    /* tree */
    rb_tree_register();

    /* shm */
    flag = SHM_CREATE;
    shm_id = dc_shm_init(shm_key, shm_size, &shm_addr, flag, &err);
    if (shm_id == FAILURE)
    {
        elog("error: dc_shm_init: %d", shm_id);
        return -1;
    }
    olog("shm addr: %p", shm_addr);

    /* blk */
    dc_blk_init(shm_addr);

    blk_a = dc_shm_get_block_A_addr(shm_addr);
    olog("blk addr: %p", blk_a);

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
        else if (memcmp(argv[2], "U", 1) == 0)
        {
            olog("to_refresh");
            to_refresh = 1;
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

    max_rows = 10;
    row_size = 100;
    index_num = 2;
    index_key_len[0] = 20;
    index_key_len[1] = 30;
    seg_size = dc_seg_calc_size(max_rows, row_size, index_num, index_key_len);
    olog("segment size: [%ld]", seg_size);

    /* segment */
    if (to_create)
    {
        olog("let's create a new segment");
        seg_idx = dc_blk_alloc_segment(blk_a, seg_size, &err);
        if (seg_idx == FAILURE)
        {
            elog("error: dc_blk_alloc_segment: %d", err);
            return -2;
        }
        olog("allocated segment index is <%d>", seg_idx);
    }

    /* segment_addr */
    seg_addr = dc_blk_get_segment_addr(blk_a, seg_idx);
    if (seg_addr == NULL)
    {
        elog("error: dc_blk_get_segment_addr[%d] failure", seg_idx);
        return -3;
    }
    olog("segment[%d] address: %p", seg_idx, seg_addr);

    if (to_create)
    {
        dc_seg_init(seg_addr, seg_idx, max_rows, seg_size);
        olog("segment init succeeds");

        memset(&col, 0, sizeof(col));
        snprintf(col.column_name, sizeof(col.column_name), "%s", "bin");
        col.column_type = 1;
        col.column_max_length = 6;
        col.column_offset = 0;
        col_idx = dc_seg_add_column(seg_addr, &col);
        olog("add column1: %d, %s", col_idx, col.column_name);

        memset(&col, 0, sizeof(col));
        snprintf(col.column_name, sizeof(col.column_name), "%s", "name");
        col.column_type = 1;
        col.column_max_length = 50;
        col.column_offset = 7;
        col_idx = dc_seg_add_column(seg_addr, &col);
        olog("add column2: %d, %s", col_idx, col.column_name);

        memset(&ind, 0, sizeof(ind));
        snprintf(ind.index_name, sizeof(ind.index_name), "%s", "bin");
        ind.index_value_max = 6;
        ind_idx = dc_seg_add_index_meta(seg_addr, &ind);
        olog("add index1: %s", ind.index_name);

        if (dc_seg_add_index_area(seg_addr, ind_idx))
        {
            elog("error: dc_seg_add_index_area: %d", ind_idx);
            return -1;
        }

        dc_seg_set_row_size(seg_addr, row_size);

        if (dc_seg_add_data_area(seg_addr))
        {
            elog("error: dc_seg_add_data_area");
            return -1;
        }

        dc_seg_layout_index_area(seg_addr, ind_idx, RB_INSERT_FUNCTION_STRING);
    }

    header = (segment_header_t *)seg_addr;

    if (to_read)
    {
        olog("READER");
        dc_seg_dump_index_area(seg_addr, ind_idx);

        olog("++++++++++++++++++++++++++++++++++");
        dc_seg_dump_data_area(seg_addr);
        olog("++++++++++++++++++++++++++++++++++");
    }

    if (to_refresh)
    {
        olog("REFRESH");
    }

    if (to_write)
    {
        olog("WRITER");
        node = dc_seg_index_alloc(seg_addr, ind_idx);
        if (node == NULL)
        {
            elog("error: dc_seg_index_alloc");
            return -1;
        }
        else
        {
            olog("node data buffer: %d, %d, %p", header->row_size, node->data_len, node->data_unit);
            node->key_len  = snprintf(node->key_val, header->index[ind_idx].index_value_max+1, "%d", getpid());
            p = dc_sec_get_row(node->data_unit);
            node->data_len = snprintf(p, header->row_size, "%s-%d", "abc", getpid());
            olog("<%s> is written", p);
            dc_seg_index_insert(seg_addr, ind_idx, node);
        }
    }

    olog("shm end");
    return rv;
}
#endif

/* seg.c */
