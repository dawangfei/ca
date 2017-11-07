#ifndef _DC_TM_H
#define _DC_TM_H

#include <time.h>
#include <sys/time.h>

/**
 *  function:
 *      convert time_t to struct tm
 *
 *  parameter:
 *     timep - input
 *      _tz  - input [-24, 24]
 *          e.g.: TZ=EAT-8, then set _tz = 8
 *      p_tm - output 
 *
 *  return:
 *      p_tm
 */
struct tm * localtime_tz(const time_t * const  timep, const int _tz, struct tm *p_tm);


/**
 *  function:
 *      convert time_t to struct tm using EAT8
 *
 *  parameter:
 *      p_tm - output 
 *
 *  return:
 *      p_tm
 */
struct tm * localtime_cn(const time_t * const  timep, struct tm *p_tm);


/**
 *  function:
 *      get current time first, then call localtime.
 *
 *  parameter:
 *      _tz  - input [-24, 24]
 *          e.g.: TZ=EAT-8, then set _tz = 8
 *      p_tm - output 
 *
 *  return:
 *      p_tm
 */
struct tm * dctime_tz(const int _tz, struct tm *p_tm);


/**
 *  function:
 *      get current time first, then call localtime using EAT8.
 *
 *  parameter:
 *      p_tm - output 
 *          use EAT-8 time zone, which means _tz is 8
 *
 *  return:
 *      p_tm
 */
struct tm * dctime_cn(struct tm *p_tm);


/**
 *  parameter:
 *      _tv_sec: input: time in second from Epoch
 *      _fmt   : output 
 */
char *dctime_format(time_t _tv_sec, char *_fmt, int _size);


/**
 *  parameter:
 *      _us     : input: time in micro second from Epoch
 *      _fmt    : output 
 */
char *dctime_format_micro(long _us, char *_fmt, int _size);


#endif
