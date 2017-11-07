/* $Id
 *    Copyright (c) 2001-2002 ADTEC Ltd.
 *    All rights reserved
 *
 *    This is unpublished proprietary
 *    source code of ADTEC Ltd.
 *    The copyright notice above does not
 *    evidence any actual or intended
 *    publication of such source code.
 *
 *    NOTICE: UNAUTHORIZED DISTRIBUTION, ADAPTATION OR USE MAY BE
 *    SUBJECT TO CIVIL AND CRIMINAL PENALTIES.
 *
 *
 *    FILE:         arclog.h
 *    DESCRIPTION:  The head file of  writing log file module 
 *    AUTHOR:
 *                  NaWei 07/15/2003 - Developed for eSWITCH system
 *    MODIFIED:
 *
 */


#ifndef __ARCLOG_H
#define __ARCLOG_H

#include "sysunix.h"
#include "sysloc.h"
#include "__arclog.h"

#ifdef __cplusplus
extern "C" {
#endif

extern int  bclerrinit _(( void ));
extern int  bclerrconf _(( char * ));
extern int  bclerrclear _(( void ));
extern int  bclerrend _(( void ));

extern void bclerrsuspend _(( void ));
extern void bclerrcontinue _(( void ));
extern int  bclerreg _(( int , char * , long, ... )) ;
extern int  bclerrlog _(( int, char *, int, ... ));
extern char *bclerrmsg _(( int ));

extern void bclappsvc _(( char * ));
extern int  bclapperr _(( int, char *, int, ... ));
extern int  bclapplog _(( char *, int, char *, int, ... ));
extern int  bclgetapperr _(( void ));

extern int  bclerrlogname _(( int,  char * , char *, int ,  ... ));
extern void  bcl_log_byprio _(( char * , char *, int , int, char * ));
extern void  bcl_log _(( char * , char *, int ,  char *, ... ));
extern void  bcl_log_err _(( char * , char *, int ,  char *, ... ));
extern void  bcl_log_warn _(( char * , char *, int ,  char *, ... ));
extern void  bcl_log_debug _(( char * , char *, int ,  char *, ... ));
extern void  bcl_log_trace _(( char * , char *, int ,  char *, ... ));
extern void bcl_dump_buffer _(( char *, char *, int , unsigned char *,int));
extern void bcl_dump_buffer_err _(( char *, char *, int , unsigned char *,int));
extern void bcl_dump_buffer_warn _(( char *, char *, int , unsigned char *,int));
extern void bcl_dump_buffer_debug _(( char *, char *, int , unsigned char *,int));
extern void bcl_dump_buffer_trace _(( char *, char *, int , unsigned char *,int));
extern void bcl_dump_buffer_byprio _(( char *, char *, int , unsigned char *, int, int));
extern int get_last_reg _(( int *, int * ));
extern int clr_last_reg _(( void ));

extern int apperreg(char *_file, int _line, char *_format, ...);
extern int apperrlog(char *_file, int _line, char *_format, ...);
extern int apperrclr(void);
extern int get_first_apperr _(( int *, char **, int *, int *, char **));

extern const char *log_dir _((void));
extern const char *log_locale _((void));
extern int log_reg_lvl _((void));
extern long log_max_size _((void));
extern char log_del_log _((void));
extern u_short log_dbg_typ _((void));
extern char log_ext_cmlog _((void));
extern char log_fmt_log _((void));
extern char log_evt_log _((void));
extern char log_rut_log _((void));
extern char log_expr_log _((void));
extern char log_flw_log _((void));
extern char log_rvs_log _((void));
extern char log_db_log _((void));
extern char log_press_log _((void));
extern int log_log_prio _((void));

#ifdef __cplusplus
}
#endif
  
#endif
