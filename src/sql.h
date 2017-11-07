#ifndef __DC_SQL_H_
#define __DC_SQL_H_

#define DC_SQL_MAX_COLUMNS   100

typedef struct {
    int             col_num;
    table_def_t     col[DC_SQL_MAX_COLUMNS];
} dc_sql_c_t;


int  dc_sql_init(void);
void dc_sql_reset(void);

int dc_sql_analyze(char *_sql, int _len);

int dc_sql_check_use_this_index(char *_index_columns, int _len);
int dc_sql_combine_index_value (char *_index_columns, int _len, char *_value, int _size);

int dc_sql_where_columns(void);


void  dc_sql_where_iterator_init();
char *dc_sql_where_iterator_next();


int dc_sql_get_where_value(char *_column, void **_value, int *_len);

char *dc_sql_get_select_column(int _idx);

char *dc_sql_get_table_name();
void  dc_sql_set_table_name(char *_name);


void *dc_sql_get_select();
int   dc_sql_set_select(void *_select);

int dc_sql_get_select_num();

int dc_sql_analyze_where_and(char *_sql, int _len);
int dc_sql_get_where(char **_where, int *_len);
void dc_sql_reset_where(void);

int dc_sql_resovle(char *_sql, int _len, char *_buffer, int _size);
int dc_sql_is_aggregation(void);
int dc_sql_clear_semicolon(char *_sql, int _len);

#endif
