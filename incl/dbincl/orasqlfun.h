/****************************** MODULE HEADER ********************************
 *                                                                           *
 * FILE   : orasqlfun.h            VERSION : 1.0                             *
 *                                                                           *
 * MACHINE: PC                     LANGUAGE: C                               *
 *                                                                           *
 * OS & ENVIROMENT:                SCO UNIX  3.2v5.0.4                       *
 *                                 ORACLE    7.2.3.1.0                       *
 *                                                                           *
 * DESCRIPTION:The head file of sqlpubfun module for Nantian StarRing System.*
 *                                                                           *
 * HISTORY:                                                                  *
 *                                                                           *
 * MM/DD/YYYY ADDRESS PROGRAMMER | DESCRIPTION OF CHANGE (Most recent first) *
 *                                                                           *
 * 10/08/2000 BEIJING ZOLA        | Modify.                                  *
 *                                                                           *
 *************************** END OF MODULE HEADER ****************************/

#ifndef __ORASQLFUN_H
#define __ORASQLFUN_H

#ifndef DATE_FMT
#define DATE_FMT "YYYYMMDD"
#endif

#ifndef DEF_DATE_FMT
#define DEF_DATE_FMT "MM/DD/YYYY"
#endif

#ifndef DEF_DTIME_FMT
#define DEF_DTIME_FMT "%Y-%m-%d %H:%M:%S"
#endif

#ifndef DEF_CHAR_LEN
#define DEF_CHAR_LEN 1024
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
#ifndef DEF_DATA_LEN            /* include int smint float smfloat serial */
#define DEF_DATA_LEN 10
#endif
#ifndef DEF_DATE_LEN
#define DEF_DATE_LEN 30
#endif
#ifndef DEF_DTIME_LEN
#define DEF_DTIME_LEN 60
#endif

#ifndef DEF_ALLOC_NUM
#define DEF_ALLOC_NUM 255        /* default allocate column number         */
                                 /* used in SqlAllocate function varable   */
#endif /* ifndef DEF_ALLOC_NUM */

#ifdef __cplusplus
extern "C" {
#endif

extern int OraMallocArea    _(( void ));
extern int OraFreeArea      _(( void ));
extern int OraFreeAreaD     _(( char * ));

extern int  OraPutData        _(( int , int, void * ));
extern int  OraGetData        _(( int , void * , int ));
extern int  OraGetType        _(( int , short * ));
extern int  OraGetTabDesc     _(( char * , int * ));
extern int  OraPutStru        _(( char * , int , void * , int ));
extern int  OraGetStru        _(( int , void * , int ));

/* SQL functions */
extern void OraSqlSetConnect  _(( char * ));
extern int  OraSqlOpenDB      _(( int, ...));
extern int  OraSqlCloseDB     _(( void ));
 
extern int  OraSqlBeginWork   _(( void ));
extern int  OraSqlCommit      _(( void ));
extern int  OraSqlRollBack    _(( void ));

extern int  OraSqlDBErr       _(( char * , int , tSqlStru * ));

extern int  OraSqlDBFree      _(( int ));
extern int  OraSqlSelect      _(( int, char *, char, long ));
extern int  OraSqlExec        _(( char * ));
extern int  OraSqlBatExec     _(( int, char * ));

/* Dynamtic SQL functions */
extern int  OraSqlAllocate  _((tSqlStru *, const char *,int ));
extern int  OraSqlPrepare   _((tSqlStru *, const char *, const char *));
extern int  OraSqlDeclare   _(( tSqlStru *, const char * , char));
extern int  OraSqlDescribe  _(( tSqlStru *, char ));
extern int  OraSqlExecute   _(( tSqlStru * ));
extern int  OraSqlOpenCur   _(( tSqlStru * ));
extern int  OraSqlFetchCur  _(( tSqlStru * , long ));
extern int  OraSqlCloseCur  _(( tSqlStru * ));
extern int  OraSqlFreeCur   _(( tSqlStru * ));
extern int  OraSqlFreePrep  _(( tSqlStru * ));
extern int  OraSqlDealloc   _(( tSqlStru * ));

extern int  OraSqlGetValue   _(( tSqlStru *, short ));
extern int  OraSqlPutValue   _(( tSqlStru *, short ));

#ifdef __cplusplus
}
#endif

#define DB_ERR( _sqlstru ) { \
    if ( _sqlstru == NULL ) \
        return E_FAIL; \
    if( _sqlstru->sql_code || _sqlstru->user_code ) { \
        FAP_SQLCODE = _sqlstru->sql_code; \
        bclerreg(E_DB_ERR , _FL_ , \
            _sqlstru->sql_code, 0, _sqlstru->user_code, \
            sqlca.sqlerrm.sqlerrmc, "", _sqlstru->user_msg ); \
        if ( FETCH_END( _sqlstru ) || _sqlstru->sql_code == SQLNOTFOUND ) \
            return E_NOTFOUND; \
        else \
            return E_FAIL; \
    } \
    return E_OK; \
}

extern char  *tmpoint;

#define  DumpTrace { \
                     fprintf( stderr, "%s %d coredump here?...", __FILE__, __LINE__ ); \
                     fflush( stderr ); \
                     tmpoint = (char *)malloc(7168); \
                     free( tmpoint ); \
                     fprintf( stderr, "no\n" ); \
                     fflush( stderr );  \
                   }

#endif /* End of __ORASQLFUN_H */

