#ifndef __DC_RS_H_
#define __DC_RS_H_


int  dc_rs_init(void);
void dc_rs_destroy(void);

void  dc_rs_set_seg_header(void *_seg_header);
void *dc_rs_get_seg_header();

void dc_rs_reset();
int  dc_rs_add_row(void *_row, int _len, int _id);

void dc_rs_iterator_init();
void *dc_rs_iterator_next();

void *dc_rs_get_head_row();
void *dc_rs_get_curr_row();
int   dc_rs_get_row2(void **_row, int *_len);

int dc_rs_get_num();
int dc_rs_restore_context(void *_row, int _row_len, void *_seg_header);
int dc_rs_uniq();

#endif
