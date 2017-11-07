#ifndef __DC_DB_H_
#define __DC_DB_H_

extern int  db_get_ncols( int _area_id );
extern long db_get_rowsize();
extern long db_get_column(int _idx, char *_name, int _name_max, int *_type, int *_value_max);


extern long db_get_rowsize_v2(int _column_num);
extern long db_get_column_v2(int _idx, char *_name, int _name_max, int *_type, int *_value_max);


#endif
