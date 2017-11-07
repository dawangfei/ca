/****************************** MODULE HEADER ********************************
 *                                                                           *
 * FILE   : db2sqlfun.h            VERSION : 1.0                             *
 *                                                                           *
 * MACHINE: PC                     LANGUAGE: C                               *
 *                                                                           *
 * OS & ENVIROMENT:                LINUX     Redhat 9.0                      *
 *                                 DB2       UDB 8.1.0.0                     *
 *                                                                           *
 * DESCRIPTION:The head file of sqlpubfun module for Nantian StarRing System.*
 *                                                                           *
 * HISTORY:                                                                  *
 *                                                                           *
 * MM/DD/YYYY ADDRESS PROGRAMMER | DESCRIPTION OF CHANGE (Most recent first) *
 *                                                                           *
 * 10/26/2003 BEIJING HANDQ       | Modify.                                  *
 *                                                                           *
 *************************** END OF MODULE HEADER ****************************/

#ifndef __DB2SQLFUN_H
#define __DB2SQLFUN_H

#ifndef DATE_FMT
#define DATE_FMT "YYYYMMDD"
#endif

#ifndef DEF_DATE_FMT
#define DEF_DATE_FMT "MM/DD/YYYY"
#endif

#ifndef DEF_DTIME_FMT
#define DEF_DTIME_FMT "%H:%M:%S"
#endif

#ifndef DEF_INTERVAL_FMT
#define DEF_INTERVAL_FMT "%Y-%m-%d %H:%M:%S"
#endif

#ifndef DEF_CHAR_LEN
#define DEF_CHAR_LEN 32700
#endif
#ifndef DEF_VARCHAR_LEN
#define DEF_VARCHAR_LEN 255
#endif

#ifndef SQLCODE
#define SQLCODE sqlca.sqlcode
#endif


#ifndef SQLNOTFOUND
#define SQLNOTFOUND 100        /*SQLCODE has a value of +1403 (+100 when MODE=A                                  NSI) because Oracle could
                                  not find a row */
#endif
#ifndef DEF_SMINT_LEN            /* include smint */
#define DEF_SMINT_LEN 2
#endif
#ifndef DEF_INTEGER_LEN          /* include int */
#define DEF_INTEGER_LEN 4
#endif
#ifndef DEF_BIGINT_LEN           /* include big int */
#define DEF_BIGINT_LEN 8
#endif
#ifndef DEF_DATE_LEN
#define DEF_DATE_LEN 10
#endif
#ifndef DEF_DTIME_LEN
#define DEF_DTIME_LEN 8
#endif
#ifndef DEF_INTERVAL_LEN
#define DEF_INTERVAL_LEN 26
#endif

#ifndef DEF_ALLOC_NUM
#define DEF_ALLOC_NUM 255        /* default allocate column number         */
                                 /* used in SqlAllocate function varable   */
#endif /* ifndef DEF_ALLOC_NUM */

#ifdef __cplusplus
extern "C" {
#endif

extern int Db2MallocArea    _(( void ));
extern int Db2FreeArea      _(( void ));
extern int Db2FreeAreaD      _(( char * ));

extern int  Db2PutData        _(( int , int, void * ));
extern int  Db2GetData        _(( int , void * , int ));
extern int  Db2GetType        _(( int , short * ));
extern int  Db2GetTabDesc     _(( char * , int * ));
extern int  Db2PutStru        _(( char * , int , void * , int ));
extern int  Db2GetStru        _(( int , void * , int ));

/* SQL functions */
extern void Db2SqlSetConnect  _(( char * ));
extern int  Db2SqlOpenDB      _(( int, ...));
extern int  Db2SqlCloseDB     _(( void ));
 
extern int  Db2SqlBeginWork   _(( void ));
extern int  Db2SqlCommit      _(( void ));
extern int  Db2SqlRollBack    _(( void ));

extern int  Db2SqlDBErr       _(( char * , int , tSqlStru * ));

extern int  Db2SqlDBFree      _(( int ));
extern int  Db2SqlSelect      _(( int, char *, char, long ));
extern int  Db2SqlExec        _(( char * ));
extern int  Db2SqlBatExec     _(( int, char * ));

/* Dynamtic SQL functions */
extern int  Db2SqlAllocate  _((tSqlStru *, const char *,int ));
extern int  Db2SqlPrepare   _((tSqlStru *, const char *, const char *));
extern int  Db2SqlDeclare   _(( tSqlStru *, const char * , char));
extern int  Db2SqlDescribe  _(( tSqlStru *, char ));
extern int  Db2SqlExecute   _(( tSqlStru * ));
extern int  Db2SqlOpenCur   _(( tSqlStru * ));
extern int  Db2SqlFetchCur  _(( tSqlStru * , long ));
extern int  Db2SqlCloseCur  _(( tSqlStru * ));
extern int  Db2SqlFreeCur   _(( tSqlStru * ));
extern int  Db2SqlFreePrep  _(( tSqlStru * ));
extern int  Db2SqlDealloc   _(( tSqlStru * ));

extern int  Db2SqlGetValue   _(( tSqlStru *, short ));
extern int  Db2SqlPutValue   _(( tSqlStru *, short ));

#ifdef __cplusplus
}
#endif

#define DB_ERR( _sqlstru ) { \
    if( _sqlstru == NULL ) \
        return E_FAIL; \
    if( _sqlstru->sql_code < 0 || _sqlstru->user_code ) { \
        err_msg[0] = 0; \
        if( _sqlstru->sql_code ) \
           sqlaintp( err_msg, ERR_MSG_LEN, 80, &sqlca ); \
        FAP_SQLCODE = _sqlstru->sql_code; \
        bclerreg(E_DB_ERR , _FL_ , \
            _sqlstru->sql_code, 0, _sqlstru->user_code, \
            err_msg, "", _sqlstru->user_msg ); \
        return E_FAIL; \
    } \
    else if ( _sqlstru->sql_code > 0 ) { \
        err_msg[0] = 0; \
        if( _sqlstru->sql_code ) \
           sqlaintp( err_msg, ERR_MSG_LEN, 80, &sqlca ); \
        FAP_SQLCODE = _sqlstru->sql_code; \
        if ( FETCH_END( _sqlstru ) || _sqlstru->sql_code == SQLNOTFOUND ) { \
            bclerreg(E_DB_ERR , _FL_ , \
                _sqlstru->sql_code, 0, _sqlstru->user_code, \
                err_msg, "", _sqlstru->user_msg ); \
            return E_NOTFOUND; \
        } \
        else { \
            bclerreg(E_DB_ERR , _FL_ , \
                _sqlstru->sql_code, 0, _sqlstru->user_code, \
                err_msg, "", _sqlstru->user_msg ); \
            bcl_log_err( "db2warn.log", __FILE__, __LINE__, "DBWarnMsg=[%s]", err_msg ); \
        } \
    } \
}

#endif /* End of __DB2SQLFUN_H */

