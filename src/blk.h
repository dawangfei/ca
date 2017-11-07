#ifndef __DC_BLK_H_
#define __DC_BLK_H_


#define SEGMENT_NAME_MAX 32     /* segment name => table name */


int  dc_blk_init(void *_shm_addr);

int  dc_blk_alloc_segment(void *_block_addr, long _size, int *_errcode);

int  dc_blk_set_segment_name(void *_block_addr, int _idx, char *_name);

void *dc_blk_get_segment_addr(void *_block_addr, int _idx);

void *dc_blk_get_segment_addr_by_name(void *_block_addr, char *_seg_name);

int   dc_blk_switch();

void *dc_blk_get_read_addr();
void *dc_blk_get_write_addr();

int  dc_blk_get_segment_number(void *_blk_addr);

int  dc_blk_calc_size(int _all_segment_size);

int  dc_blk_get_segment_index(void *_block_addr, char *_seg_name);

int  dc_blk_set_timestamp(void *_block_addr, long _t);
long dc_blk_get_timestamp(void *_block_addr);

int  dc_blk_set_semid(void *_block_addr, int _id);
int  dc_blk_get_semid(void *_block_addr);


void dc_blk_iterator_init(void *_block_addr);
char *dc_blk_iterator_next();
void dc_blk_iterator_end();

#endif
