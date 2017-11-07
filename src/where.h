#ifndef __DC_WHERE_H_
#define __DC_WHERE_H_


int  dc_where_init(void);
int  dc_where_destroy();

void *dc_where_analyze(char *_sql, int _len);

int  dc_where_iterator_init(void *_addr);
void *dc_where_iterator_next();
int  dc_where_iterator_end(void *_list);

#endif
