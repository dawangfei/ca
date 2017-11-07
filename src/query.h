#ifndef __DC_QUERY_H_
#define __DC_QUERY_H_


int dc_query_init();

int dc_query_dump_row(void *_seg_addr, void *_row);

int dc_query_sql_select(char * _sql, int _sql_len);

int dc_query_get_data(int _data_id , void * _data , int _size);

void *dc_query_rs_next();

int dc_query_sql_select_pre(void *_blk_addr, char * _sql, int _sql_len);

int dc_query_sql_select_post(void *_blk_addr, int _seg_idx);

int dc_query_dump_segment(void *_seg_addr);
int dc_query_dump_segment_info(void *_seg_addr);
int dc_query_dump_segment_column(void *_seg_addr);

int dc_query_dump_curr_row();

int dc_query_get_stru(void * _data , int _size);

int dc_query_is_aggregation_gramma(char *_select_column);
int dc_query_get_first_select_column(char * _name, int _size);

int dc_query_resolve_sql(char *_sql, int _len, char *_buffer, int _size);
int dc_query_put_data(int _data_id , int _data_type , void * _data );

#endif
