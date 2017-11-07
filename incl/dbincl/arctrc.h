/* $Id:
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
 *    FILE:           dta.h
 *    DESCRIPTION:    DTA managing header file 
 *    AUTHOR:
 *              NaWei 07/10/2003 - Developed for eSWITCH system
 *    MODIFIED:
 *
 */

#ifndef __ARCTRC_H
#define __ARCTRC_H


#ifdef __cplusplus
extern "C" {
#endif

extern void trc_set_conf _(( char* ));

extern void trc_log_rec _(( unsigned char*, int, short ));

#ifdef ____ARCIOF_H
extern void trc_epx_rec _(( ELNODE *, ELPOOL *, int ));
extern void trc_fmt_rec _(( tIOFITEM*, int, void*, int, int, int ));
#endif

#ifdef ____ARCCPT_H
extern void trc_cpt_info _(( int, tCPTINFO *, tCPTSTATUS *, int ));
extern FILE * trc_cpt_open _(( char *, short ));
extern void trc_cpt_close _(( FILE *, char *, short ));
extern void trc_cpt_mapitem _(( int, int, char *, char *, char *, char, char *, FILE * ));
extern void trc_cpt_stackitem( ELNODE *, char *, FILE *, short );
extern void trc_cpt_noexistel( char *, int, char * );
#endif

#ifdef ____DTART_H
extern void trc_rt_rec _(( tSNNODE*, char*, tENNODE* ));
extern void trc_rt_result _(( char*, char *, char *, char *, char * ));
#endif

#ifdef ____ARCCST_H
extern void trc_exp_rec _(( char*, tANYVALUE* ));
extern void trc_exp_string _(( char* ));
extern void trc_exp_result _(( tANYVALUE* ));
#endif

#ifdef ____PLADB_H
extern void trc_db_rec _(( char *, int _sql_code ));
extern void trc_db_data_rec _(( int, int, int, char *, char *, int ));
#endif

#ifdef ____ALACF_H
extern void trc_flw_rec _(( char *, int, char *, short, ANYVALUE *, clock_t ));
#endif

#ifdef ____PLARVS_H
extern void trc_rec_rvs _(( short, tAUTOREVDEF *, char *, char * ));
extern void trc_rvs_rec _(( char *, char *, char *, int, int ));
#endif

extern void trc_tran_rec _(( char *, int ));
extern void vararg_tran_rec _(( int , char * ,... ));

extern void trc_ala_flw( int, int, int, ... );
#ifdef ____ARCDE_H
extern void trc_ala_epx _(( int, char *, char *, int ));
#endif

#ifdef __cplusplus
}
#endif

#endif
