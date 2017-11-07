#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <ctype.h>
#include <unistd.h>
#include "util.h"


void dc_strrev(char *p)
{
    char *q = p;
    while(q && *q) ++q; /* find eos */
    for(--q; p < q; ++p, --q) DC_SWP(*p, *q);
}

/* get time of t */
long get_time()
{
    struct timeval tv;

    gettimeofday(&tv, NULL);

    return (long)(tv.tv_sec*1000000+tv.tv_usec);
}


/* get time of t */
long dc_get_time_of_seconds()
{
    struct timeval tv;

    gettimeofday(&tv, NULL);

    return (long)(tv.tv_sec);
}


void dc_miscrosleep(unsigned int microsecs)
{
    struct timeval tval;

    tval.tv_sec  = microsecs / 1000000;
    tval.tv_usec = microsecs % 1000000;

    select(0, NULL, NULL, NULL, &tval);
}


/**
 *   len = 0;
 *   ptr = buffer;
 *   while ((ptr = strsep2(ptr, deli, &len)) != NULL)
 *   { 
 *       printf("%.*s", len, ptr);
 *       ...
 *
 *       ptr += len;
 *   }
 *
 */
char *strsep2(char *buffer, char deli, int *length)
{
    int  i = 0;
    int  j = 0;
    int  len = 0;
    char *start = NULL;
    char *ptr = buffer;


    start = buffer;

    len = strlen(buffer);

    /* 1. skip heading 'deli' */
    for (i = 0; i < len; i++, ptr++)
    {
        if (*ptr != deli)
            break;
    }

    /* all is delimeter! */
    if (i == len)
    {
        return NULL;
    }
    start = ptr;

    /* 2. find next 'deli'
     *    a) found: substr(start, found-point)
     *    b) NULL:  substr(start, ...)
     */
    for (j = i; j < len; j++, ptr++)
    {
        if (*ptr == deli)
            break;
    }

    if (j == len) /* not found */
    {
        *length = len - i;
    }
    else
    {
        *length = ptr - start;
    }

    return start;
}

/* By liw. */
char *strrstr2(const char *content, const char *key)
{
    if (*key == '\0')
        return (char *) content;

    char *result = NULL;

    for (;;)
    {
        char *p = strstr(content, key);
        if (p == NULL)
            break;
        result = p;
        content = p + 1;
    }

    return result;
}


/**
 *
 *   ptr = buffer;
 *   strsep3_init(ptr, strlen(ptr), deli);
 *
 *   len = 0;
 *   while ((ptr = strsep3_next(ptr, &len)) != NULL)
 *   {
 *       printf("%.*s", len, ptr);
 *       ...
 *
 *       ptr += len;
 *   }
 *
 */
static char *_g3_buffer = NULL;
static int   _g3_len    = 0;
static char  _g3_deli   = ' ';
int strsep3_init(char *_buffer, int _len, char _deli)
{
    _g3_buffer = _buffer;

    _g3_len    = _len;

    _g3_deli   = _deli;

    return 0;
}


/**
 *
 *   ptr = buffer;
 *   strsep3_init(ptr, strlen(ptr), deli);
 *
 *   len = 0;
 *   while ((ptr = strsep3_next(ptr, &len)) != NULL)
 *   {
 *       printf("%.*s", len, ptr);
 *       ...
 *
 *       ptr += len;
 *   }
 *
 */
char *strsep3_next(char *_buffer, int *_len)
{
    int  i = 0;
    int  j = 0;
    int  len = 0;
    char *start = NULL;
    char *ptr = _buffer;

    len = _g3_len;

    /* 1. skip heading 'deli' */
    for (i = 0; i < len; i++, ptr++, _g3_len--)
    {
        if (*ptr != _g3_deli)
            break;
    }

    /* all is delimeter! */
    if (i == len)
    {
        return NULL;
    }
    start = ptr;

    /* 2. find next 'deli'
     *    a) found: substr(start, found-point)
     *    b) NULL:  substr(start, ...)
     */
    for (j = i; j < len; j++, ptr++, _g3_len--)
    {
        if (*ptr == _g3_deli)
            break;
    }

    if (j == len) /* not found */
    {
        *_len = len - i;
    }
    else
    {
        *_len = ptr - start;
    }

    return start;
}


char *dc_str_toupper(char *_buffer)
{
    char *p = _buffer;

    while(*p)
    {
        *p = toupper((int)*p);
        p++;
    }
    return _buffer;
}


/**
 * not thread safe
 **/
int dc_random()
{
    unsigned int seed;
    int value;
    struct timeval tv;
    static int _random_i_ = 0;
    static char buffer[1024] = {0};
    static pid_t pid = 0;
    static uid_t uid = 0;
 
    if (pid == 0)
    {
        pid = getpid();
    }

    if (uid == 0)
    {
        uid = getuid();
    }

    seed = 82;

    gettimeofday(&tv, NULL);
    seed ^= (unsigned int)tv.tv_usec;

    if (buffer[0] == 0)
    {
        getcwd(buffer, sizeof(buffer));
    }
    seed ^= ++_random_i_%256;

    seed ^= (pid<<8) + (uid<<4) + pid;

    srand(seed);
    value = rand();

    return value;
}


int dc_random_int(int _base)
{
    return (int)((float)(_base) * (dc_random()%RAND_MAX)/(RAND_MAX+1.0));
}


/* util.c */
