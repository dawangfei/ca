#ifndef __DC_SEG_H_
#define __DC_SEG_H_

#include "list.h"

#define COLUMN_NAME_MAX         32
#define INDEX_COLUMNS_MAX       100         /* sum of all column name of index */
#define TABLE_MAX_INDEXES       5           /* max indexes of table */
#define TABLE_MAX_COLUMNS       100         /* max columns of table */

typedef struct _table_def_t {
    char column_name[COLUMN_NAME_MAX+1];
    int  column_type;
    int  column_max_length;
    int  column_offset;
} table_def_t;


typedef struct _index_def_t {
    char index_name[INDEX_COLUMNS_MAX+1];   /* +col1+col2+col3...colN */
    int  index_value_max;                   /* sum of all column value length */
    long offset;                            /* not include segment header */
    void *index_addr;
} index_def_t;


typedef struct _segment_header_t {
    int             segment_idx;
    int             unit_size;
    int             row_size;
    int             rows_free;                  /* available */
    int             rows_used;                  /* used */
    long            t2;                         /* timestamp */
    long            size_available;             /* not include segment header */
    long            size_allocated;
    long            data_offset;                /* not include segment header */
    void           *data_addr;

    List            data_free_list;             /* for no index case */
    Link            data_free_head;

    List            data_used_list;             /* full table */
    Link            data_used_head;

    int             column_num;
    table_def_t     column[TABLE_MAX_COLUMNS];  /* column define */

    int             index_num;
    index_def_t     index[TABLE_MAX_INDEXES];
    rb_tree_t       tree[TABLE_MAX_INDEXES];
} segment_header_t;



int dc_seg_set_row_size(void *_seg_addr, int _row_size);

int dc_seg_add_column(void *_seg_addr, table_def_t *_column);


/* must keep order */
int dc_seg_add_index_meta(void *_seg_addr, index_def_t *_index);
int dc_seg_add_index_area(void *_seg_addr, int _idx);
int dc_seg_add_data_area(void *_seg_addr);

void *dc_seg_index_alloc(void *_seg_addr, int _idx);

void dc_seg_index_insert(void *_seg_addr, int _idx, rb_tree_node_t *_node);

long dc_seg_calc_size(int _rows, int _row_size, int _index_num, int _key_max_len[]);

int dc_seg_init(void *_seg_addr, int _seg_idx, int _max_rows, long _segment_size);

int dc_seg_layout_index_area(void *_seg_addr, int _idx, int _index_type);

int dc_seg_layout_data_area (void *_seg_addr);


int dc_seg_gen_timestamp(void *_seg_addr);


void *dc_seg_data_alloc(void *_seg_addr);
int   dc_seg_data_insert(void *_seg_addr, void *_data_unit, void *_row, int _len);
int   dc_seg_data_delete(void *_data_unit);

void  dc_seg_set_timestamp(void *_seg_addr, long _t);
long  dc_seg_get_timestamp(void *_seg_addr);
long  dc_seg_get_timestamp_by_name(char *_segment_name);
long  dc_seg_get_segment_size(void *_seg_addr);
int   dc_seg_get_segment_rows(void *_seg_addr);

int dc_seg_dump_index_area(void *_seg_addr, int _idx);
int dc_seg_dump_data_area(void *_seg_addr);


#endif
