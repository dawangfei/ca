#ifndef _DC_UTIL_H
#define _DC_UTIL_H

#include <stdio.h>
#include <sys/times.h>
#include <assert.h>
#include <time.h>
#include <stddef.h>


#define DC_SWP(x,y) (x^=y, y^=x, x^=y)
void dc_strrev(char *p);


long get_time(void);


long dc_get_time_of_seconds(void);


void dc_miscrosleep(unsigned int microsecs);


char *strsep2(char *buffer, char deli, int *length);

char *strrstr2(const char *content, const char *key);

int   strsep3_init(char *_buffer, int _len, char _deli);
char *strsep3_next(char *_buffer, int *_len);

char *dc_str_toupper(char *_buffer);

int dc_random();
int dc_random_int(int _base);

#endif
