#include "log.h"
#include "tm.h"

static long my_get_time();

void _log(FILE *fp, char *source, int line, char *fmt, ...)
{
    char timestamp[30+1] = {0};
    dctime_format_micro(my_get_time(), timestamp, sizeof(timestamp));

    va_list ap;
    va_start(ap, fmt);
    fprintf(fp, "%.23s,%04d,%10.10s,%04d    ", 
            timestamp, getpid(), source, line);
    vfprintf(fp, fmt, ap);
    va_end(ap);
    fprintf(fp, "\n");
    fflush(fp);
}


static long my_get_time()
{
    struct timeval tv;

    gettimeofday(&tv, NULL);

    return (long)(tv.tv_sec*1000000+tv.tv_usec);
}


#if RUN_LOG
int main(int argc, char *argv[])
{
    int  n = 1;

    olog("hello, world: o %d", n++);
    elog("hello, world: e %d", n++);

    tlog("sleeping", 1);
    sleep(1);
    tlog("sleeping", 2);

    return 0;
}
#endif
/* log.c */
