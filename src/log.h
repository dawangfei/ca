#ifndef __DC_LOG_H
#define __DC_LOG_H

#include <stdio.h>
#include <sys/times.h>
#include <stdarg.h>
#include <unistd.h>
#include <sys/time.h>


#if STARRING_INTEGRATE

#if PE_LOG

int pe_log( char *file_name, int line_no, char * formt, ... );
#define olog(...) (pe_log(__FILE__, __LINE__, __VA_ARGS__))
#define elog(...) (pe_log(__FILE__, __LINE__, __VA_ARGS__))

#else

void bcl_log(char * logfile, char * file, int line, char * format, ...);
#define olog(...) (bcl_log("CACHE.log", __FILE__, __LINE__, __VA_ARGS__))
#define elog(...) (bcl_log("CACHE.log", __FILE__, __LINE__, __VA_ARGS__))

#endif


#else

void _log(FILE *, char *, int, char *, ...);
#define olog(...) (_log(stdout, __FILE__, __LINE__, __VA_ARGS__))
#define elog(...) (_log(stderr, __FILE__, __LINE__, __VA_ARGS__))

#endif



#endif
