/* #ident "@(#)dbapi.c,v 1.0 2000/05/30 10:17:26 hdq database application interface source"
 *      SCCS IDENTIFICATION
 * copyright (C) 2000, 2000 by hdq.
 * MUST be distributed in source form only.
 */

/*****************************************************************************
 * File            : dbapi.c                                                 *
 * System Name     : StarRing                                                *
 * VERSION         : 1.0                                                     *
 * LANGUAGE        : C                                                       *
 * OS & ENVIROMENT : SCO UNIX  5                                             * 
 *                   INFORMIX  7.23UC13                                      *
 *                   SYBASE    11.x                                          *
 * DESCRIPTION     : this program refer to all of API about                  * 
 *                   operation of database                                   *
 * HISTORY:                                                                  *
 * MM/DD/YYYY      ADDRESS          PROGRAMMER            DESCRIPTION        *
 *---------------------------------------------------------------------------*
 * 05/27/2000      NanJing          Hdq                   Creation           *
 * 03/06/2003      BeiJing          Yuciguo               Modified           *
 *                                                                           *
 *****************************************************************************/

#include <sysloc.h>
#include <arcbcl.h>


/*  Application include file define using different type */

#include <__pladb.h>

extern void        FreeValueBuf    _(( tSqlStru * ));

/* Global variable definition */
int FAPCHAR=DFFAPCHAR;
int FAPSMINT=DFFAPSMINT;
int FAPINT=DFFAPINT;
int FAPFLOAT=DFFAPFLOAT;
int FAPSMFLOAT=DFFAPSMFLOAT;
int FAPDECIMAL=DFFAPDECIMAL;
int FAPSERIAL=DFFAPSERIAL;
int FAPDATE=DFFAPDATE;
int FAPMONEY=DFFAPMONEY;
int FAPNULL=DFFAPNULL;
int FAPDTIME=DFFAPDTIME;
int FAPBYTES=DFFAPBYTES;
int FAPTEXT=DFFAPTEXT;
int FAPVCHAR=DFFAPVCHAR;
int FAPINTERVAL=DFFAPINTERVAL;
int FAPNCHAR=DFFAPNCHAR;
int FAPNVCHAR=DFFAPNVCHAR;
/* for oracle */
int FAPVCHAR2=DFFAPVCHAR2;
int FAPNUMBER=DFFAPNUMBER;
int FAPLONG=DFFAPLONG;
int FAPRAW=DFFAPRAW;
int FAPLONGRAW=DFFAPLONGRAW;
/* for db2 */
int FAPINTEGER=DFFAPINTEGER;

int IDEF_DTIME_LEN=DEF_DTIME_LEN;
int IDEF_DATE_LEN=DEF_DATE_LEN;
int IDEF_CHAR_LEN=DEF_CHAR_LEN;

short DbId=DB_ID;
DB_API DbApi[]={
#ifdef INFORMIX
       { InfMallocArea,
         InfFreeArea,
         InfPutData,
         InfGetData,
         InfGetType,
         InfGetTabDesc,
         InfPutStru,
         InfGetStru,
         InfSqlSetConnect,
         InfSqlOpenDB,
         InfSqlCloseDB,
         InfSqlBeginWork,
         InfSqlCommit,
         InfSqlRollBack,
         InfSqlSelect,
         InfSqlExec,
         InfSqlBatExec,
         InfFreeAreaD
        },
#else
        {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL},
#endif
#ifdef ORACLE
       { OraMallocArea,
         OraFreeArea,
         OraPutData,
         OraGetData,
         OraGetType,
         OraGetTabDesc,
         OraPutStru,
         OraGetStru,
         OraSqlSetConnect,
         OraSqlOpenDB,
         OraSqlCloseDB,
         OraSqlBeginWork,
         OraSqlCommit,
         OraSqlRollBack,
         OraSqlSelect,
         OraSqlExec,
         OraSqlBatExec,
         OraFreeAreaD
        },
#else
        {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL},
#endif
#ifdef SYBASE
       { SybMallocArea,
         SybFreeArea,
         SybPutData,
         SybGetData,
         SybGetType,
         SybGetTabDesc,
         SybPutStru,
         SybGetStru,
         SybSqlSetConnect,
         SybSqlOpenDB,
         SybSqlCloseDB,
         SybSqlBeginWork,
         SybSqlCommit,
         SybSqlRollBack,
         SybSqlSelect,
         SybSqlExec,
         SybSqlBatExec,
         SybFreeAreaD
        },
#else
        {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL},
#endif
#ifdef DB2
       { Db2MallocArea,
         Db2FreeArea,
         Db2PutData,
         Db2GetData,
         Db2GetType,
         Db2GetTabDesc,
         Db2PutStru,
         Db2GetStru,
         Db2SqlSetConnect,
         Db2SqlOpenDB,
         Db2SqlCloseDB,
         Db2SqlBeginWork,
         Db2SqlCommit,
         Db2SqlRollBack,
         Db2SqlSelect,
         Db2SqlExec,
         Db2SqlBatExec,
         Db2FreeAreaD
        },
#else
        {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL},
#endif
#ifdef NODATABASE
       { NoMallocArea,
         NoFreeArea,
         NoPutData,
         NoGetData,
         NoGetType,
         NoGetTabDesc,
         NoPutStru,
         NoGetStru,
         NoSqlSetConnect,
         NoSqlOpenDB,
         NoSqlCloseDB,
         NoSqlBeginWork,
         NoSqlCommit,
         NoSqlRollBack,
         NoSqlSelect,
         NoSqlExec,
         NoSqlBatExec,
         NoFreeAreaD
        },
#else
        {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL},
#endif
        {NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL,NULL}
};

short MAX_VAR_NUM=1024;
tSqlValue * TSQLVarPool=NULL;

short MAX_AREA=10;
short CURRENT_AREA=0;
short FETCH_AREA=0;
tSqlStru ** TSQLSTRU=NULL;

long FAP_SQLCODE=0;

int
db_put_type( int _id )
{
    if( TSQLVarPool == NULL )
        return E_FAIL;
    return TSQLVarPool[ _id ].type;
}

int
db_put_cb( int _id )
{
    if( TSQLVarPool == NULL )
        return E_FAIL;
    return TSQLVarPool[ _id ].cb;
}

char *
db_put_name( int _id )
{
    if( TSQLVarPool == NULL )
        return NULL;
    return TSQLVarPool[ _id ].name;
}

int
db_get_type( int _id )
{
    if( TSQLVarPool == NULL )
        return E_FAIL;
    return TSQLVarPool[ _id + ( MAX_VAR_NUM/2 ) ].type;
}

int
db_get_cb( int _id )
{
    if( TSQLVarPool == NULL )
        return E_FAIL;
    return TSQLVarPool[ _id + ( MAX_VAR_NUM/2 ) ].cb;
}

char *
db_get_name( int _id )
{
    if( TSQLVarPool == NULL )
        return NULL;
    return TSQLVarPool[ _id + ( MAX_VAR_NUM/2 ) ].name;
}

int
db_get_ncols( int _area_id )
{
    if ( TSQLSTRU == NULL || CURRENT_AREA < _area_id )
        return E_FAIL;

    if ( TSQLSTRU[_area_id] == NULL )
        return E_FAIL;

    return ( TSQLSTRU[_area_id]->column_num );
}

/* 20050710云南建行删除表的操作会锁全表，需要增加AVOID_FULL关键字避免 */
char *db_avoid_full( char *_tbl_name )
{
    static char           str[1024];

    str[0] = 0;
    if ( _tbl_name == NULL )
        return str;

#ifdef INFORMIX
    sprintf( str, "{+AVOID_FULL( %s )}", _tbl_name );
#endif

    return str;
}

/* 20051112顺德农信DB2表的操作会锁全表，需要增加WITH UR */
char *db_with_ur( void )
{
    static char           str[128];

    str[0] = 0;
#ifdef DB2
    strcpy( str, " WITH UR" );
#endif

    return str;
}


#ifndef NODATABASE

/*Function in this program define*/
/*------------------------------------------------------------------------*
 *  Function Name : int  get_comma_num  ()                                *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/

int get_comma_num( char * str_com , char comma )
{
    register int i = 0;
    register int j = 0;
    register int flag = 0;

    while( str_com[i++] != 0x00 ) {
        if ( str_com[i] == '\'' ) {
            flag = !flag;
            continue;
        }
        if ( flag != 0 )
            continue;

        if ( str_com[i] == comma ) j++;
    }
    return j;
}

/*------------------------------------------------------------------------*
 *  Function Name : int   AllocSqlStru( tSqlStru * _sqlstru )             *
 *  Description   : alloc a tSqlstru struct                               *
 *  Input         : void                                                  *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/
tSqlStru * 
AllocSqlStru( void )
{
    tSqlStru * ret_stru;
    ret_stru = calloc( 1, sizeof( tSqlStru ) );
    if ( ret_stru == NULL )
        return NULL;
    else
        return ret_stru;
}

/*------------------------------------------------------------------------*
 *  Function Name : void      *FreeSqlStru()                              *
 *  Description   : free a tSqlstru struct                                *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/

void  
FreeSqlStru(tSqlStru * _sqlstru)
{

    if ( _sqlstru != NULL )   {
        FreeValueBuf( _sqlstru );
        free( _sqlstru );
        _sqlstru = NULL;
    }


}

#else /* NODATABASE */

int NoMallocArea()
{
    return E_OK;
}

int NoFreeArea()
{
    return E_OK;
}

int NoFreeAreaD( char *dtaname )
{
    return E_OK;
}

int NoPutData ( int _data_id , int _data_type , void * _data )
{
    return E_OK;
}

int NoGetData ( int _data_id , void * _data , int _len )
{
    return E_OK;
}

int NoGetType ( int _data_id , short *_type )
{
    return E_OK;
}

int NoGetStru ( int _data_id , void * _data , int _len )
{
    return E_OK;
}

int NoGetTabDesc( char * _tab_name , int * _len )
{
    return E_OK;
}

int NoPutStru ( char * _tab_name , int _data_id , void * _data , int _len )
{
    return E_OK;
}

char CONNECT_NAME[64]="conn00";
void NoSqlSetConnect( char *_con_name )
{
    return;
}

int NoSqlOpenDB( int _control_num, ... )
{
    return E_OK;
}

int NoSqlCloseDB ( void )
{
    return E_OK;
}

int NoSqlBeginWork ( void )
{
    return E_OK;
}

int NoSqlCommit ( void )
{
    return E_OK;
}

int NoSqlRollBack  ( void )
{
    return E_OK;
}

int NoSqlSelect ( int _area , char * _statement , char _op_flag , long _offset )
{
    return E_NOTFOUND;
}

int NoSqlExec ( char * _statement )
{
    return E_NOTFOUND;
}

int NoSqlBatExec ( int _area, char * _statement )
{
    return E_NOTFOUND;
}

#endif  /* NODATABAE */
