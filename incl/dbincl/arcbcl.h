/*
 *    Copyright (c) 2001-2003 ADTEC Ltd.
 *    All rights reserved
 *
 *    This is unpublished proprietary source code of ADTEC Ltd.
 *    The copyright notice above does not evidence any actual
 *    or intended publication of such source code.
 *
 *    NOTICE: UNAUTHORIZED DISTRIBUTION, ADAPTATION OR USE MAY BE
 *    SUBJECT TO CIVIL AND CRIMINAL PENALTIES.
 */

/*
 *    ProgramName : arcbcl.h
 *    SystemName  : Starring
 *    Version     : 3.0
 *    Language    : C
 *    OS & Env    : RedHat Linux 8.x
                    Informix  7.30.U
 *    Description : Base function module
 *    History     : 
 *    YYYY/MM/DD	Position	Author		Description
 *    -------------------------------------------------------------
 *    2003/07/15	BeiJing		Handq		Creation
 */
#ifndef __ARCBCL_H_
#define __ARCBCL_H_

#include "sysunix.h"
#include "__arcbcl.h"

#ifdef __cplusplus
extern "C" {
#endif

/* String manipulations */

#if defined( HAVE_NOT_SNPRINTF )
define snprintf bcl_snprintf
#endif

extern int bcl_snprintf _(( char *, size_t, const char *, ... ));
extern size_t  bcl_align_8bytes _(( size_t ));

extern void bcl_str_bitmove _(( unsigned char*, int, char, int ));
extern int  bcl_strisdigit _(( char * ));
extern int  bcl_strisalpha _(( char * ));
extern int  bcl_strisalnum _(( char * ));
extern int  bcl_ischn_1stbyte _(( u_char ));
extern int  bcl_ischn_word _(( u_char, u_char ));
extern int  bcl_ischn_in_str _(( u_char *, int, int ));
extern char** bcl_strsplit _(( char*, char, int* ));
extern void bcl_freespace _(( char **p, int nitems ));

extern void bcl_str_toupper _(( char * ));
extern void bcl_str_tolower _(( char* ));
extern int  bcl_strhasspace _(( char * ));
extern char *bcl_ltrim _(( char* ));
extern char *bcl_rtrim _(( char* ));
extern char *bcl_trimall _(( char * ));
extern char *bcl_trim_comment _(( char* ));
extern char* bcl_fill_str _(( char*, char*, int, int, char, char ));
extern char* bcl_unfill_str _(( char *, size_t ,  char , char ));
extern int bcl_unfill_bstr _(( char *, size_t ,  char , char ));
extern int bcl_char_num _(( char *str, char ch ));
extern char* bcl_strchr _(( char *str, char ch ));

extern char *bcl_dleadzero _(( char* ));
extern void bcl_conv_terminator _(( unsigned char * , unsigned char * , int * )); 
extern void bcl_mem_conv_term _(( unsigned char * , int, unsigned char * , int * )); 
extern void bcl_conv_str _(( unsigned char * , unsigned char * , int * )); 
extern void bcl_conv_bstr _(( unsigned char * , unsigned char * , int * )); 


extern void bcl_edit_money _(( char *, char * ));

/* File operations */

extern int bcl_executeable _(( char* ));
extern long bcl_get_file_size _(( char* ));
extern int bcl_isdir _(( char* ));
extern char* bcl_basename _(( char* ));
extern char* bcl_dirname _(( char* ));
extern int bcl_mkdir _(( char* ));
extern int bcl_getline _(( FILE *, char *, int ));

/* Process */

extern int bcl_daemon_start _(( char * ));
extern int bcl_exec_proc _(( char *, char **, char ** ));
extern char * bcl_exit_str _(( int, char * ));
extern int bcl_exec_func _(( char *, USERFORKFUNC ));
extern int bcl_exec_func_p _(( char *, USERFORKFUNC_P, void * ));

/* Math */
extern int bcl_myround _(( double ));
extern double bcl_myatof _(( char * ));
extern int bcl_ngcd _(( int a[],int n ));
extern int bcl_gcd2 _(( int a,int b ));
extern int bcl_gcd _(( int a,int b )) ;

/* Memory */
extern void bcl_alloccopy _(( unsigned char **, unsigned char *, int, int * ));
extern void bcl_myfree _(( char ** ));
extern char * bcl_item_get_by_dlmt _(( char *, char *, int * ));
extern char * bcl_item_cut_by_dlmt _(( char *, char *, int * ));

/* time */
extern void bcl_fmt_date_time _(( time_t * ,char * , char * ));
extern int bcl_convfmt_date_time _(( char *, char *, char *, char * ));
extern void bcl_get_date_time _(( char *, char * ));
extern double bcl_get_ctime _(( ));
extern int bcl_get_year _(( char * ));
extern int bcl_get_month _(( char * ));
extern int bcl_get_day _(( char * ));
extern int bcl_get_mon_end _(( int, int ));
extern void bcl_cal_date_by_mon _(( char *, int, char * ));
extern void bcl_cal_date_by_day _(( char *, int, char * ));
extern void bcl_cal_date_by_year _(( char *, int, char * ));
extern void bcl_udelay _(( long ));
extern void bcl_get_time _(( tTimeStru * ));
extern char * bcl_get_dtime_str _(( char * ));
extern char * bcl_get_date_str _(( char * ));
extern char * bcl_get_time_str _(( char * ));
extern int bcl_isleap _(( int ));
extern int bcl_days_span _(( char *, char * ));
extern int bcl_date_verify _(( char * ));

/* IPC */
extern key_t bcl_get_ipc_key _(( char *_path, int _key_id ));

extern u_char* bcl_shm_create _(( size_t, char *, int, int ));
extern u_char* bcl_shm_createbykey _(( size_t, key_t, int, int * ));
extern int bcl_shm_remove _(( char *, int ));
extern int bcl_shm_removebykey _(( key_t ));
extern int bcl_shm_removebyid _(( int ));
extern u_char* bcl_shm_attach _(( char *, int ));
extern u_char* bcl_shm_attach_rdonly _(( char *, int ));
extern u_char* bcl_shm_attachbykey _(( key_t ));
extern u_char* bcl_shm_attachbyid _(( int ));
extern u_char* bcl_shm_attachbyid_rdonly _(( int ));
extern int bcl_shm_detach _(( u_char * ));
extern int bcl_shm_getsize _(( char *, int ));
extern int bcl_shm_getsize_byid _(( int ));
extern int bcl_shm_getid _(( char *, int ));
extern int bcl_shm_setperm _(( char *, int, int ));
extern int bcl_shm_setperm_byid _(( int, int ));

extern int bcl_msq_remove _(( char *, long ));
extern int bcl_msq_removebykey _(( key_t ));
extern int bcl_msq_removebyid _(( int ));
extern int bcl_msq_get _(( char *, int, int ));
extern int bcl_msq_getbykey _(( key_t, int ));
extern int bcl_msq_init _(( char *, int, int ));
extern int bcl_msq_initbykey _(( key_t, int ));
extern int bcl_msq_attach _(( char *, int ));
extern int bcl_msq_attachbykey _(( key_t ));
extern int bcl_msq_send _(( int, void *, size_t, int ));
extern int bcl_msq_recv _(( int, void *, long, size_t, int ));
extern int bcl_msq_getmaxbytes _(( int ));

extern int bcl_sem_create _(( char *, int, int, int, u_short * ));
extern int bcl_sem_createbykey _(( key_t, int, int, u_short * ));
extern int bcl_sem_remove _(( char *, int ));
extern int bcl_sem_removebykey _(( key_t ));
extern int bcl_sem_removebyid _(( int ));
extern int bcl_sem_open _(( char *, int ));
extern int bcl_sem_openbykey _(( key_t ));
extern int bcl_sem_op _(( int, struct sembuf *, int ));
extern int bcl_sem_wait _(( int, int ));
extern int bcl_sem_trywait _(( int, int ));
extern int bcl_sem_signal _(( int, int ));
extern int bcl_sem_setval _(( int, int, u_short ));
extern int bcl_sem_setall _(( int, int, u_short * ));
extern int bcl_sem_getval _(( int, int ));
extern int bcl_sem_getncnt _(( int, int ));
extern int bcl_sem_getzcnt _(( int, int ));

/* signal */
extern void bcl_sig_empty_suspdmask _(( void ));
extern void bcl_sig_add_all2suspdmask _(( void ));
extern void bcl_sig_add_suspdmask _(( int  ));
extern void bcl_sig_del_suspdmask _(( int  ));
extern void bcl_sig_empty_blkmask _(( void ));
extern void bcl_sig_add_all2blkmask _(( void ));
extern void bcl_sig_add_blkmask _(( int  ));
extern void bcl_sig_del_blkmask _(( int  ));
extern int _bcl_sig_delay _(( char *, int ));
extern int _bcl_sig_enable _(( char *, int ));
extern int bcl_sig_enableone _(( int  ));
extern int bcl_sig_enableall _(( void ));
extern int bcl_sig_wait_for _(( int  ));
extern int bcl_sig_suspend _(( void ));
extern int bcl_sig_pending _(( sigset_t * ));
extern int bcl_sig_ismember _(( const sigset_t *, int  ));
extern Sigfunc bcl_sig_sethandler _(( int , Sigfunc  ));
extern void bcl_signal_handle _(( int  ));
extern SIGALRMTRAP bcl_set_sigalrm_trap _(( SIGALRMTRAP ));
extern void bcl_unset_sigalrm_trap _(( void ));
extern SIGCLDTRAP bcl_set_sigcld_trap _(( SIGCLDTRAP ));
extern void bcl_unset_sigcld_trap _(( void ));
extern SIGTERMTRAP bcl_set_sigterm_trap _(( SIGTERMTRAP ));
extern void bcl_unset_sigterm_trap _(( void ));
extern SIGUSR1TRAP bcl_set_sigusr1_trap _(( SIGUSR1TRAP ));
extern void bcl_unset_sigusr1_trap _(( void ));
extern SIGUSR2TRAP bcl_set_sigusr2_trap _(( SIGUSR2TRAP ));
extern void bcl_unset_sigusr2_trap _(( void ));
extern SIGUSR2TRAP bcl_get_sigusr2_trap _(( ));
extern void bcl_set_signal_trap _(( void ));
extern void bcl_reset_signal_trap _(( void ));
extern void bcl_catch_to_exit _(( void ));
extern void bcl_catch_to_def _(( void ));

/* LINK */
extern LINK *bcl_link_init _(( void ));
extern LINK *bcl_link_join _(( LINK *, LINK * ));
extern void bcl_link_drop _(( LINK *, USERLINKFUNC ));
extern int  bcl_link_addelmt _(( LINK *, const void *, size_t, USERLINKFUNC ));
extern void *bcl_link_getelmt _(( const LINK *, u_int, size_t * ));
extern void *bcl_link_search _(( const LINK *, const void *, LINKCMPFUNC ));
extern void bcl_link_delelmt _(( LINK *, u_int, USERLINKFUNC ));
extern int bcl_link_dump _(( const u_char *, size_t, const LINK *, USERLINKFUNC));
extern int bcl_link_push _(( LINK *, const void *, size_t, USERLINKFUNC ));
extern int bcl_link_pop _(( LINK *, void *, size_t *, USERLINKFUNC ));
extern void bcl_link_reset _(( LINK * ));
extern void bcl_link_free _(( LINK * ));
extern int bcl_link_getlast _(( LINK *, void **, size_t *, USERLINKFUNC ));
extern int bcl_link_updlast _(( LINK *, const void *, size_t, USERLINKFUNC ));

/* SOCKET */
extern LINK *bcl_gather_if_info _(( void ));
extern int bcl_is_local_addr _(( u_int ));
extern int bcl_tcp_listener _(( int *, u_int, u_short ));
extern int bcl_select _(( int, fd_set *, fd_set *, fd_set *, struct timeval * ));
extern int bcl_tcp_connector _(( u_int, u_short ));
extern int bcl_tcp_aconnector _(( u_int, u_short ));
extern int bcl_tcp_aconn _(( u_int, u_short, int ));
extern int bcl_tcp_accept _(( int, u_int *, u_short * ));
extern int bcl_sp_listener _(( int *, char * ));
extern int bcl_sp_connector _(( char * ));
extern int bcl_sp_accept _(( int ));
extern int bcl_setnbio _(( int, int ));
extern int bcl_getsockerr _(( int ));
extern int bcl_send_fileds _(( int, int, void*, int ));
extern int bcl_recv_fileds _(( int, int*, void*, int ));
extern int bcl_sockisreadable _(( int, int ));
extern int bcl_sockfd_isset _(( int ));
extern int bcl_bytesreadable _(( int ));
extern int bcl_closesock _(( int ));
extern int bcl_send_nbytes _(( int, void *, int ));
extern int bcl_send_nbytes_tm _(( int, void *, int,int ));
extern int bcl_recv_nbytes _(( int, void *, int, int ));
extern int bcl_peek_nbytes _(( int, void *, int, int ));
extern int bcl_strx_to_xstr _(( char * strx, unsigned char * xstr ));
extern int bcl_analysis_port _(( char *, u_short * ));
extern int bcl_analysis_addr _(( char *, u_int * ));
extern int bcl_tcp_server _(( int, char **, tGWDCONF * ));
extern int bcl_tcp_backfd _(( void ));
extern int bcl_tcp_closefd _(( void ));
extern int bcl_tcp_backfd2 _(( int ));
extern int bcl_tcp_closefd2 _(( int ));
extern int bcl_tcp_setopt _(( int, int, int ));
extern int bcl_tcp_get_spfd _(( void ));

/* NSBUF */
extern void bcl_init_nsbuf _(( struct nsbuf* ));
extern void bcl_free_nsbuf _(( struct nsbuf* ));
extern int bcl_read_nsbuf _(( struct nsbuf*, char*, int ));
extern int bcl_write_nsbuf _(( struct nsbuf*, char*, int ));
extern void bcl_reset_nsbuf _(( struct nsbuf* ));
extern int bcl_size_nsbuf _(( struct nsbuf* ));
extern int bcl_send_nsbuf _(( int, struct nsbuf* ));
extern int bcl_recv_nsbuf _(( int, struct nsbuf*, int, int ));

/* 文件锁 */
extern int bcl_lock_reg _(( int, int, int, off_t, int, off_t ));
extern int bcl_readw_lock _(( int ));
extern int bcl_writew_lock _(( int ));
extern int bcl_un_lock _(( int ));

extern int bcl_file_zip _(( char *, char * ));
extern int bcl_file_unzip _(( char *, char *, int * ));
extern char * bcl_str_clr_ch _(( char * _src, char _ch ));

/* 正则表达式匹配 */
extern int bcl_match _(( char *, char * ));

/* HASH 算法 */
extern unsigned long bcl_hash1 _(( char * ));
extern unsigned long bcl_hash2 _(( char * ));
extern unsigned long bcl_hash3 _(( char * ));
extern unsigned long bcl_hash4 _(( char * ));
extern unsigned long bcl_hash5 _(( char * ));
extern unsigned long bcl_hash6 _(( char * ));

/*
extern unsigned long bcl_hash _(( char * ));
*/
#define bcl_hash( name ) bcl_hash4( name )

extern void prepareCryptTable _(());
extern unsigned long bcl_hashn _(( char *, unsigned int ));

/* 只读内存创建HASH CACHE */
extern unsigned long bcl_hcache_init _(( tHASHCACHEDEF * ));
extern unsigned long bcl_hcache_get_pos _(( tHASHCACHEDEF *, char * ));
extern unsigned long bcl_hcache_get_pos_byid _(( tHASHCACHEDEF *, unsigned long));
extern void bcl_hcache_free _(( tHASHCACHEDEF * ));

/* HASH TABLE存放结构数组 */
extern unsigned long bcl_htable_create _(( tHASHTABLEDEF * ));
extern int bcl_htable_put _(( tHASHTABLEDEF *, const char *, void * ));
extern char *bcl_htable_check_same _(( tHASHTABLEDEF *, const char * ));
extern void *bcl_htable_get _(( tHASHTABLEDEF *, const char * ));
extern void *bcl_htable_remove _(( tHASHTABLEDEF *, const char * ));
extern unsigned long bcl_htable_recycle _(( tHASHTABLEDEF *, unsigned long ));
extern void bcl_htable_clear _(( tHASHTABLEDEF * ));
extern void bcl_htable_free _(( tHASHTABLEDEF * ));

#define bcl_sig_delay() _bcl_sig_delay( __FILE__, __LINE__ )
#define bcl_sig_enable() _bcl_sig_enable( __FILE__, __LINE__ )

extern int bcl_sigusr2_flag _(( void ));
extern void bcl_reset_sigusr2_flag _(( void ));

extern long bcl_str_2_time _(( char *, char * ));
extern char * bcl_time_2_str _(( long, char *, char * ));

#ifdef __cplusplus
}
#endif

#endif /* end of arcbcl.h */
