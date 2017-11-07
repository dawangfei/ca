#ifndef __DC_LOAD_H_
#define __DC_LOAD_H_

#define DC_INDEX_SEP "+"


int dc_load_init();

int dc_load_get_col_idx(void *_seg_addr, char *_col_name);

int dc_load_is_valid_column_type(int _type);

int dc_load_calc_shm_size();

int dc_load_all_tables(void *_blk_addr, long _t2);

int dc_load_table_refresh(void *_blk_addr, char *_table_name, long _tm);

#endif
