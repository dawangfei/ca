#ifndef __DC_HIS_H_
#define __DC_HIS_H_


int dc_his_init();

int dc_his_query_pre(char *_sql, void **_p, char **_name);

int dc_his_query_post(void *_p, long _remote_tm,
        void **_row, int *_row_len,
        void **_seg_header, void **_select, char **_table_name);

int dc_his_add(char *_sql, void *_row, int _row_len, void *_seg_header, void *_select, char *_table_name);

#endif
