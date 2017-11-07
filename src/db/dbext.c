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

extern short FETCH_AREA;
extern tSqlStru ** TSQLSTRU;

extern short MAX_VAR_NUM;
extern tSqlValue * TSQLVarPool;

#if DB2
/*------------------------------------------------------------------------*
 *  Function Name : long db_get_rowsize( )                                *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/
long db_get_rowsize()
{
    tSqlValue	* sql_value;
    int		i;
    void	*p;

    p = NULL;

    for( i = 0; i < TSQLSTRU[FETCH_AREA]->column_num ; i++ )
    {
        sql_value = &TSQLSTRU[FETCH_AREA]->tValuepb[i];
        /*
        printf("name(%s) -- len(%d)", sql_value->name, sql_value->cb);
        */
        switch ( sql_value->type ) {
             case DFFAPDATE:
             case DFFAPDTIME:
             case DFFAPINTERVAL:
             case DFFAPCHAR:
             case DFFAPVCHAR:
             case DFFAPLONG:
             case DFFAPVCHAR2:
                  STRALIGN( p , sizeof( char ) );
                  p = (void *)((long)p + sizeof( char ) * ( sql_value->cb + 1 ));
                  break;
             case DFFAPTEXT:
             case DFFAPBYTES:
                  STRALIGN( p , sizeof( char ) );
                  p = (void *)((long)p + sizeof( char ) * ( DEF_CHAR_LEN + 1 ));
                  break;
             case DFFAPSMINT:
                  STRALIGN( p , sizeof( short ) );
                  p = (void *)((long)p + sizeof( short ));
                  break;
             case DFFAPINTEGER:
/* 20051019
                  STRALIGN( p , sizeof( int ) );
                  p = (void *)((long)p + sizeof( int ));
                  break;
*/
             case DFFAPINT:
                  STRALIGN( p , sizeof( long ) );
                  p = (void *)((long)p + sizeof( long ));
                  break;
             case DFFAPDECIMAL:
             case DFFAPFLOAT:
                  STRALIGN( p , sizeof( double ) );
                  p = (void *)((long)p + sizeof( double ));
                  break;
             case DFFAPSMFLOAT:
                  STRALIGN( p , sizeof( float ) );
                  p = (void *)((long)p + sizeof( float ));
                  break;
        }
    }
    return (long)p;
}

/*------------------------------------------------------------------------*
 *  Function Name : void  db_get_column( )                                *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/
long db_get_column(int _idx, char *_name, int _name_max, int *_type, int *_value_max)
{
    int  i;
    tSqlValue	* sql_value;
    void	*p;
    void    *p1;

    p = NULL;
    p1= NULL;

    if (_idx >= TSQLSTRU[FETCH_AREA]->column_num)
    {
        printf("error: column idx exceeds: %d >= %d\n",
                _idx, TSQLSTRU[FETCH_AREA]->column_num);
        return E_FAIL;
    }


    for( i = 0; i <= _idx; i++ )
    {
        sql_value = &TSQLSTRU[FETCH_AREA]->tValuepb[i];
        switch ( sql_value->type ) {

             case DFFAPDATE:
             case DFFAPDTIME:
             case DFFAPINTERVAL:
             case DFFAPCHAR:
             case DFFAPVCHAR:
             case DFFAPLONG:
             case DFFAPVCHAR2:
                  STRALIGN( p , sizeof( char ) );
                  p1 = p;
                  p = (void *)((long)p + sizeof( char ) * ( sql_value->cb + 1 ));
                  break;
             case DFFAPTEXT:
             case DFFAPBYTES:
                  STRALIGN( p , sizeof( char ) );
                  p1 = p;
                  p = (void *)((long)p + sizeof( char ) * ( DEF_CHAR_LEN + 1 ));
                  break;
             case DFFAPSMINT:
                  STRALIGN( p , sizeof( short ) );
                  p1 = p;
                  p = (void *)((long)p + sizeof( short ));
                  break;
             case DFFAPINTEGER:
/* 20051019
                  STRALIGN( p , sizeof( int ) );
                  p1 = p;
                  p = (void *)((long)p + sizeof( int ));
                  break;
*/
             case DFFAPINT:
                  STRALIGN( p , sizeof( long ) );
                  p1 = p;
                  p = (void *)((long)p + sizeof( long ));
                  break;
             case DFFAPDECIMAL:
             case DFFAPFLOAT:
                  STRALIGN( p , sizeof( double ) );
                  p1 = p;
                  p = (void *)((long)p + sizeof( double ));
                  break;
             case DFFAPSMFLOAT:
                  STRALIGN( p , sizeof( float ) );
                  p1 = p;
                  p = (void *)((long)p + sizeof( float ));
                  break;
        }
    }

    sql_value = &TSQLSTRU[FETCH_AREA]->tValuepb[_idx];
    snprintf(_name, _name_max, "%s", sql_value->name);
    *_type = sql_value->type;
    *_value_max = sql_value->cb;

    return (long)p1;
}
#endif

#if ORACLE
/*------------------------------------------------------------------------*
 *  Function Name : long db_get_rowsize( )                                *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/
long db_get_rowsize()
{
    tSqlValue	* sql_value;
    int		i;
    void	*p;

    p = NULL;

    for( i = 0; i < TSQLSTRU[FETCH_AREA]->column_num ; i++ )
    {
        sql_value = &TSQLSTRU[FETCH_AREA]->tValuepb[i];
        /*
        printf("name(%s) -- len(%d)", sql_value->name, sql_value->cb);
        */
        switch ( sql_value->type ) {
             case DFFAPRAW:
             case DFFAPLONG:
             case DFFAPLONGRAW:
             case DFFAPCHAR:
             case DFFAPVCHAR2:
                  STRALIGN( p , sizeof( char ) );
                  p = (void *)((long)p + sizeof( char ) * ( sql_value->cb + 1 ));
                  break;
             case DFFAPDATE:
                  STRALIGN( p , sizeof( char ) );
                  p = (void *)((long)p + sizeof( char ) * ( DEF_DATE_LEN + 1 ));
                  break;
             case DFFAPSMINT:
                  STRALIGN( p , sizeof( short ) );
                  p = (void *)((long)p + sizeof( short ));
                  break;
             case DFFAPINT:
             case DFFAPNUMBER:
                  STRALIGN( p , sizeof( long ) );
                  p = (void *)((long)p + sizeof( long ));
                  break;
             case DFFAPDECIMAL:
             case DFFAPFLOAT:
                  STRALIGN( p , sizeof( double ) );
                  p = (void *)((long)p + sizeof( double ));
                  break;
             case DFFAPSMFLOAT:
                  STRALIGN( p , sizeof( float ) );
                  p = (void *)((long)p + sizeof( float ));
                  break;
        }
    }
    return (long)p;
}

/*------------------------------------------------------------------------*
 *  Function Name : void  db_get_column( )                                *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/
long db_get_column(int _idx, char *_name, int _name_max, int *_type, int *_value_max)
{
    int  i;
    tSqlValue	* sql_value;
    void	*p;
    void    *p1;

    p = NULL;
    p1= NULL;

    if (_idx >= TSQLSTRU[FETCH_AREA]->column_num)
    {
        printf("error: column idx exceeds: %d >= %d\n",
                _idx, TSQLSTRU[FETCH_AREA]->column_num);
        return E_FAIL;
    }


    for( i = 0; i <= _idx; i++ )
    {
        sql_value = &TSQLSTRU[FETCH_AREA]->tValuepb[i];
        switch ( sql_value->type ) {
             case DFFAPRAW:
             case DFFAPLONG:
             case DFFAPLONGRAW:
             case DFFAPCHAR:
             case DFFAPVCHAR2:
                  STRALIGN( p , sizeof( char ) );
                  p1 = p;
                  p = (void *)((long)p + sizeof( char ) * ( sql_value->cb + 1 ));
                  break;
             case DFFAPDATE:
                  STRALIGN( p , sizeof( char ) );
                  p1 = p;
                  p = (void *)((long)p + sizeof( char ) * ( DEF_DATE_LEN + 1 ));
                  break;
             case DFFAPSMINT:
                  STRALIGN( p , sizeof( short ) );
                  p1 = p;
                  p = (void *)((long)p + sizeof( short ));
                  break;
             case DFFAPINT:
             case DFFAPNUMBER:
                  STRALIGN( p , sizeof( long ) );
                  p1 = p;
                  p = (void *)((long)p + sizeof( long ));
                  break;
             case DFFAPDECIMAL:
             case DFFAPFLOAT:
                  STRALIGN( p , sizeof( double ) );
                  p1 = p;
                  p = (void *)((long)p + sizeof( double ));
                  break;
             case DFFAPSMFLOAT:
                  STRALIGN( p , sizeof( float ) );
                  p1 = p;
                  p = (void *)((long)p + sizeof( float ));
                  break;
        }
    }

    sql_value = &TSQLSTRU[FETCH_AREA]->tValuepb[_idx];
    snprintf(_name, _name_max, "%s", sql_value->name);
    *_type = sql_value->type;
    *_value_max = sql_value->cb;

    return (long)p1;
}
#endif



#if ORACLE
/**********************************************************************
  version 2 for empty table 2017-11-2
  It depends the preceding calling of OraGetTabDesc()/DBGetTabDesc();
 *********************************************************************/

/*------------------------------------------------------------------------*
 *  Function Name : long db_get_rowsize_v2(int _column_num )              *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/
long db_get_rowsize_v2(int _column_num)     /* oracle */
{
    int		i = 0;
    int     offset = 0;
    void	*p;
    tSqlValue	* sql_value;

    p = NULL;

    offset =  MAX_VAR_NUM/2;
    for( i = 0; i < _column_num; i++ )
    {
        sql_value = &TSQLVarPool[i+offset];
        /*
        printf("after: name(%s) -- len(%d)\n", sql_value->name, sql_value->cb);
        */
        switch ( sql_value->type ) {
             case DFFAPRAW:
             case DFFAPLONG:
             case DFFAPLONGRAW:
             case DFFAPCHAR:
             case DFFAPVCHAR2:
                  STRALIGN( p , sizeof( char ) );
                  p = (void *)((long)p + sizeof( char ) * ( sql_value->cb + 1 ));
                  break;
             case DFFAPDATE:
                  STRALIGN( p , sizeof( char ) );
                  p = (void *)((long)p + sizeof( char ) * ( DEF_DATE_LEN + 1 ));
                  break;
             case DFFAPSMINT:
                  STRALIGN( p , sizeof( short ) );
                  p = (void *)((long)p + sizeof( short ));
                  break;
             case DFFAPINT:
             case DFFAPNUMBER:
                  STRALIGN( p , sizeof( long ) );
                  p = (void *)((long)p + sizeof( long ));
                  break;
             case DFFAPDECIMAL:
             case DFFAPFLOAT:
                  STRALIGN( p , sizeof( double ) );
                  p = (void *)((long)p + sizeof( double ));
                  break;
             case DFFAPSMFLOAT:
                  STRALIGN( p , sizeof( float ) );
                  p = (void *)((long)p + sizeof( float ));
                  break;
        }
    }
    return (long)p;
}

/*------------------------------------------------------------------------*
 *  Function Name : void  db_get_column_v2( )                             *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/
long db_get_column_v2(int _idx, char *_name, int _name_max, int *_type, int *_value_max)  /*  oracle */
{
    int  i;
    int  offset = 0;
    tSqlValue	* sql_value;
    void	*p;
    void    *p1;

    p = NULL;
    p1= NULL;

    offset =  MAX_VAR_NUM/2;
    for( i = 0; i <= _idx; i++ )
    {
        sql_value = &TSQLVarPool[i+offset];
        switch ( sql_value->type ) {
             case DFFAPRAW:
             case DFFAPLONG:
             case DFFAPLONGRAW:
             case DFFAPCHAR:
             case DFFAPVCHAR2:
                  STRALIGN( p , sizeof( char ) );
                  p1 = p;
                  p = (void *)((long)p + sizeof( char ) * ( sql_value->cb + 1 ));
                  break;
             case DFFAPDATE:
                  STRALIGN( p , sizeof( char ) );
                  p1 = p;
                  p = (void *)((long)p + sizeof( char ) * ( DEF_DATE_LEN + 1 ));
                  break;
             case DFFAPSMINT:
                  STRALIGN( p , sizeof( short ) );
                  p1 = p;
                  p = (void *)((long)p + sizeof( short ));
                  break;
             case DFFAPINT:
             case DFFAPNUMBER:
                  STRALIGN( p , sizeof( long ) );
                  p1 = p;
                  p = (void *)((long)p + sizeof( long ));
                  break;
             case DFFAPDECIMAL:
             case DFFAPFLOAT:
                  STRALIGN( p , sizeof( double ) );
                  p1 = p;
                  p = (void *)((long)p + sizeof( double ));
                  break;
             case DFFAPSMFLOAT:
                  STRALIGN( p , sizeof( float ) );
                  p1 = p;
                  p = (void *)((long)p + sizeof( float ));
                  break;
        }
    }

    sql_value = &TSQLVarPool[_idx+offset];
    snprintf(_name, _name_max, "%s", sql_value->name);
    *_type = sql_value->type;
    *_value_max = sql_value->cb;

    return (long)p1;
}
#endif


#if DB2
/**********************************************************************
  version 2 for empty table 2017-11-2
  It depends the preceding calling of Db2GetTabDesc()/DBGetTabDesc();
 *********************************************************************/

/*------------------------------------------------------------------------*
 *  Function Name : long db_get_rowsize_v2(int _column_num )              *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/
long db_get_rowsize_v2(int _column_num)     /* DB2 */
{
    int		i = 0;
    int     offset = 0;
    void	*p;
    tSqlValue	* sql_value;

    p = NULL;

    offset =  MAX_VAR_NUM/2;
    for( i = 0; i < _column_num ; i++ )
    {
        sql_value = &TSQLVarPool[i+offset];
        /*
        printf("name(%s) -- len(%d)", sql_value->name, sql_value->cb);
        */
        switch ( sql_value->type ) {
             case DFFAPDATE:
             case DFFAPDTIME:
             case DFFAPINTERVAL:
             case DFFAPCHAR:
             case DFFAPVCHAR:
             case DFFAPLONG:
             case DFFAPVCHAR2:
                  STRALIGN( p , sizeof( char ) );
                  p = (void *)((long)p + sizeof( char ) * ( sql_value->cb + 1 ));
                  break;
             case DFFAPTEXT:
             case DFFAPBYTES:
                  STRALIGN( p , sizeof( char ) );
                  p = (void *)((long)p + sizeof( char ) * ( DEF_CHAR_LEN + 1 ));
                  break;
             case DFFAPSMINT:
                  STRALIGN( p , sizeof( short ) );
                  p = (void *)((long)p + sizeof( short ));
                  break;
             case DFFAPINTEGER:
/* 20051019
                  STRALIGN( p , sizeof( int ) );
                  p = (void *)((long)p + sizeof( int ));
                  break;
*/
             case DFFAPINT:
                  STRALIGN( p , sizeof( long ) );
                  p = (void *)((long)p + sizeof( long ));
                  break;
             case DFFAPDECIMAL:
             case DFFAPFLOAT:
                  STRALIGN( p , sizeof( double ) );
                  p = (void *)((long)p + sizeof( double ));
                  break;
             case DFFAPSMFLOAT:
                  STRALIGN( p , sizeof( float ) );
                  p = (void *)((long)p + sizeof( float ));
                  break;
        }
    }
    return (long)p;
}

/*------------------------------------------------------------------------*
 *  Function Name : void  db_get_column_v2( )                             *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/
long db_get_column_v2(int _idx, char *_name, int _name_max, int *_type, int *_value_max)    /* DB2 */
{
    int  i;
    int  offset = 0;
    tSqlValue	* sql_value;
    void	*p;
    void    *p1;

    p = NULL;
    p1= NULL;


    offset = MAX_VAR_NUM/2;
    for( i = 0; i <= _idx; i++ )
    {
        sql_value = &TSQLVarPool[i+offset];
        switch ( sql_value->type ) {

             case DFFAPDATE:
             case DFFAPDTIME:
             case DFFAPINTERVAL:
             case DFFAPCHAR:
             case DFFAPVCHAR:
             case DFFAPLONG:
             case DFFAPVCHAR2:
                  STRALIGN( p , sizeof( char ) );
                  p1 = p;
                  p = (void *)((long)p + sizeof( char ) * ( sql_value->cb + 1 ));
                  break;
             case DFFAPTEXT:
             case DFFAPBYTES:
                  STRALIGN( p , sizeof( char ) );
                  p1 = p;
                  p = (void *)((long)p + sizeof( char ) * ( DEF_CHAR_LEN + 1 ));
                  break;
             case DFFAPSMINT:
                  STRALIGN( p , sizeof( short ) );
                  p1 = p;
                  p = (void *)((long)p + sizeof( short ));
                  break;
             case DFFAPINTEGER:
/* 20051019
                  STRALIGN( p , sizeof( int ) );
                  p1 = p;
                  p = (void *)((long)p + sizeof( int ));
                  break;
*/
             case DFFAPINT:
                  STRALIGN( p , sizeof( long ) );
                  p1 = p;
                  p = (void *)((long)p + sizeof( long ));
                  break;
             case DFFAPDECIMAL:
             case DFFAPFLOAT:
                  STRALIGN( p , sizeof( double ) );
                  p1 = p;
                  p = (void *)((long)p + sizeof( double ));
                  break;
             case DFFAPSMFLOAT:
                  STRALIGN( p , sizeof( float ) );
                  p1 = p;
                  p = (void *)((long)p + sizeof( float ));
                  break;
        }
    }

    sql_value = &TSQLVarPool[_idx+offset];
    snprintf(_name, _name_max, "%s", sql_value->name);
    *_type = sql_value->type;
    *_value_max = sql_value->cb;

    return (long)p1;
}

#endif
