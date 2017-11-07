#ifndef __DC_PUT_H_
#define __DC_PUT_H_


typedef struct
{ 
    char            *c_value;
    int              i_value;
    short            s_value;
    long             l_value;
    unsigned  int    ui_value;
    unsigned  short  us_value;
    unsigned  long   ul_value;
    float            f_value;
    double           d_value;
} dc_put_data_t;

typedef struct
{
    int             used;
    int             fail;
    int             type;
    int             len;
    dc_put_data_t   data; 
} dc_put_t;


int  dc_put_init(void);
void dc_put_destroy(void);

void dc_put_reset();
int  dc_put_one(int _data_id , int _data_type , void * _data);

void dc_put_iterator_init();
void *dc_put_iterator_next();

int dc_put_get_num();

#endif
