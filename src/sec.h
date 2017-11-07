#ifndef __DC_SEC_H_
#define __DC_SEC_H_

#include "list.h"

/*  */
typedef struct {
    int      id;
    int      len;
    void    *index_node[TABLE_MAX_INDEXES];
    Link     link;
    char     row[1];
} dc_sec_t;

int dc_sec_memset(void *_data_unit, int _len);

int dc_sec_memcpy(void *_data_unit, void *_data, int _len);

int dc_sec_reversed_link(void *_data_unit, int _ind_idx, void *_index_node);

int dc_sec_delete(void *_data_unit);

int dc_sec_insert(List *_list, void *_data_unit);

void *dc_sec_extract(List *_list);

int dc_sec_calc_size(int _row_size);

void *dc_sec_get_row(void *_data_unit);

void dc_sec_set_id(void *_data_unit, int _id);
int  dc_sec_get_id(void *_data_unit);

#endif
