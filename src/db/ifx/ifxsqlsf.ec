/* #ident "@(#)infsf.ec,v 1.0 2000/05/30 10:17:26 hdq INFORMIX 7.3 interface source"
 *      SCCS IDENTIFICATION
 * copyright (C) 2000, 2000 by hdq.
 * MUST be distributed in source form only.
 */

/*****************************************************************************
 * File            : informixsf.ec                                           *
 * System Name     : StarRing                                                *
 * VERSION         : 1.0                                                     *
 * LANGUAGE        : ESQLC                                                   *
 * OS & ENVIROMENT : SCO UNIX  5                                             * 
 *                   INFORMIX  7.23UC13                                      *
 * DESCRIPTION     : this program refer to all of functions about            * 
 *                   operation of database( informix sql function)           *
 * HISTORY:                                                                  *
 * MM/DD/YYYY      ADDRESS          PROGRAMMER            DESCRIPTION        *
 *---------------------------------------------------------------------------*
 * 05/27/2000      NanJin           Hdq                   Creation           *
 * 03/06/2003      BeiJing          Yuciguo               Added              *
 *                                                                           *
 *****************************************************************************/

#include <arcbcl.h>
#include <arclog.h>
#include <__arcde.h>

EXEC SQL include sqlca.h;
EXEC SQL include sqlda.h;
EXEC SQL include sqltypes.h;

/*  Application include file define using different type */

#include <__pladb.h>
#include <arctrc.h>

extern int         get_comma_num   _(( char * , char ));

extern short MAX_VAR_NUM;
extern tSqlValue * TSQLVarPool;

extern short MAX_AREA;
extern short CURRENT_AREA;
extern short FETCH_AREA;
extern tSqlStru ** TSQLSTRU;

extern long FAP_SQLCODE;

/* Global variable definition */
EXEC SQL BEGIN DECLARE SECTION;
    char CONNECT_NAME[64]="";
EXEC SQL END DECLARE SECTION;

/*Function in this program define*/

/*------------------------------------------------------------------------*
 *  Function Name : int  InfMallocArea()                                  *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/
int 
InfMallocArea()
{
   int i;
   tSqlValue *tv;
   int  max_num;

   if ( CURRENT_AREA < MAX_AREA-1 )
       CURRENT_AREA++;
   else {
       bclerreg( E_MAXAREA, _FL_, CURRENT_AREA + 1 );
       return E_FAIL;
   }

   i = CURRENT_AREA;

   if( TSQLSTRU == NULL )
      APP_ERR( E_DB_NOOPEN )

   if ( TSQLSTRU[ CURRENT_AREA ] == NULL ) {
       TSQLSTRU[ CURRENT_AREA ] = calloc( 1, sizeof( tSqlStru ) );
       if ( TSQLSTRU[ CURRENT_AREA ] == NULL ) {
          bclerreg( E_ALLOC, _FL_, sizeof( tSqlStru ) );
          return E_FAIL;
       }
   }
   /* 20051027 不再释放tValuepb */
   tv = TSQLSTRU[CURRENT_AREA]->tValuepb;
   max_num = TSQLSTRU[CURRENT_AREA]->max_num;

   memset( TSQLSTRU[ CURRENT_AREA ], 0x00 , sizeof( tSqlStru ) );

   TSQLSTRU[CURRENT_AREA]->tValuepb = tv;
   TSQLSTRU[CURRENT_AREA]->max_num = max_num;

   return i;
}

/*------------------------------------------------------------------------*
 *  Function Name : int  InfFreeArea()                                    *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/
int 
InfFreeArea()
{
   int i;

   i = CURRENT_AREA;
   if ( CURRENT_AREA > 0 ) {

      if ( TSQLSTRU[ CURRENT_AREA ] != NULL )
          InfSqlDBFree( CURRENT_AREA );

      CURRENT_AREA--;
   }

   return i;
}

/*------------------------------------------------------------------------*
 *  Function Name : int  InfFreeAreaD()                                   *
 *  Description   :                                                       *
 *  Input         : dtaname DTA名称                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/
int 
InfFreeAreaD( char *dtaname )
{
   int i;

   i = CURRENT_AREA;
   if ( CURRENT_AREA > 0 ) {

      if ( TSQLSTRU[ CURRENT_AREA ] != NULL ) {
          bcl_log( "DBDBG.log", _FL_, "DTA[%s]执行语句[%s]的工作区[%d]未释放", dtaname, TSQLSTRU[CURRENT_AREA]->sql_str, CURRENT_AREA );
          InfSqlDBFree( CURRENT_AREA );
      }

      CURRENT_AREA--;
   }

   return i;
}
/*------------------------------------------------------------------------*
 *  Function Name : void  FreeValueBuf()                                  *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/
void
FreeValueBuf(tSqlStru *_sqlstru)
{

    if ( _sqlstru->tValuepb != NULL )   {

/* 20051027 不再释放，提高效率
        free( _sqlstru->tValuepb );
        _sqlstru->tValuepb = NULL;
*/
        _sqlstru->tV_num = 0;
    }

}

void
FreeCValue( tSqlValue * _sqlvalue )
{
    switch ( _sqlvalue->type ) {
     case DFFAPCHAR:
     case DFFAPDTIME:
     case DFFAPDATE:
     case DFFAPBYTES:
     case DFFAPTEXT:
     case DFFAPVCHAR:
     case DFFAPINTERVAL:
     case DFFAPNCHAR:
     case DFFAPNVCHAR:
     case DFFAPVCHAR2:
          if( _sqlvalue->pubdata.c_value != NULL ) {
              free( _sqlvalue->pubdata.c_value );
              _sqlvalue->pubdata.c_value = NULL;
          }
          break;
    }
}

/*------------------------------------------------------------------------*
 *  Function Name : int  AllocValueBuf()                                  *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/
int 
AllocValueBuf(tSqlStru *_sqlstru,int _alloc_num)
{
/* 20051027 只calloc一次
     FreeValueBuf( _sqlstru );
     _sqlstru->tValuepb= \
         (tSqlValue *) calloc ((size_t) _alloc_num,(size_t) sizeof(tSqlValue) );
*/

    if( _sqlstru->tValuepb == NULL ) {
        _sqlstru->tValuepb= \
         (tSqlValue *) malloc ((size_t) _alloc_num*(size_t) sizeof(tSqlValue) );
        _sqlstru->max_num = _alloc_num;
    }
    else if( _alloc_num > _sqlstru->max_num ){
        _sqlstru->tValuepb= \
         (tSqlValue *) realloc ( _sqlstru->tValuepb, \
                  (size_t) _alloc_num*(size_t) sizeof(tSqlValue) );
        _sqlstru->max_num = _alloc_num;
    }
    if( _sqlstru->tValuepb == NULL ) {
        bclerreg( E_ALLOC, _FL_, _alloc_num * sizeof( tSqlValue ) );
        return E_FAIL;
    }
    memset(_sqlstru->tValuepb,0x00,sizeof(tSqlValue) * _alloc_num );
    _sqlstru->tV_num = _alloc_num;
    return E_OK;
}

/*------------------------------------------------------------------------*
 *  Function Name : void  InfPutData  ()                                  *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/

int InfPutData ( int _data_id , int _data_type , void * _data )
{
    char str_data[MAX_DOUBLE_STR_LEN];

    int str_len;

    if ( _data == NULL  ) {
        bclerreg( E_INVALARG, _FL_, "data buffer null" );
        return E_FAIL;
    }

    if ( _data_id > ( MAX_VAR_NUM/2 - 1 ) ) {
        bclerreg( E_INVALARG, _FL_, "data id" );
        return E_FAIL;
    }

    FreeCValue( &TSQLVarPool[ _data_id ] );

    switch ( _data_type )   {
     case DFFAPCHAR:
     case DFFAPDTIME:
     case DFFAPDATE:
     case DFFAPBYTES:
     case DFFAPTEXT:
     case DFFAPVCHAR:
     case DFFAPINTERVAL:
     case DFFAPNCHAR:
     case DFFAPNVCHAR:
     case DFFAPVCHAR2:
          if( _data_type == DFFAPVCHAR )
              str_len = DEF_VARCHAR_LEN;
          else
              str_len = strlen( ( char *)_data ) + 1;
          TSQLVarPool[ _data_id ].pubdata.c_value = calloc( 1, str_len + 1 );
          if( TSQLVarPool[ _data_id ].pubdata.c_value == NULL ) {
              bclerreg( E_ALLOC, _FL_, str_len + 1 );
              return E_FAIL;
          }

          TSQLVarPool[ _data_id ].cb = str_len;
          if ( _data_type == DFFAPDTIME )
             TSQLVarPool[ _data_id ].cb = DEF_DTIME_LEN;
          if ( _data_type == DFFAPDATE )
             TSQLVarPool[ _data_id ].cb = DEF_DATE_LEN;

          trc_db_data_rec( 0, _data_id, _data_type, "CHAR", _data, TSQLVarPool[ _data_id ].cb );
          if( _data_type == DFFAPVCHAR )
              memcpy( TSQLVarPool[ _data_id ].pubdata.c_value, _data, str_len );
          else {
              if( str_len > 1 )
                  strcpy( TSQLVarPool[ _data_id ].pubdata.c_value , _data );
/* 20050220字符串类型，如果为空，查询会返回E_NOTFOUND，追加一个空格避免 */
              else {
                  strcpy( TSQLVarPool[ _data_id ].pubdata.c_value, " " );
                  TSQLVarPool[ _data_id ].cb++;
              }
          }

          break;
     case DFFAPSMINT:
          TSQLVarPool[ _data_id ].pubdata.s_value = *( (short *)_data );
          TSQLVarPool[ _data_id ].cb = DEF_DATA_LEN;

          sprintf( str_data, "%d", *( (short *)_data ) );
          trc_db_data_rec( 0, _data_id, _data_type,  "SMALLINT2", \
                              str_data, TSQLVarPool[ _data_id ].cb );
          break;
/* 20050428 支持DFFAPINTEGER */
     case DFFAPINTEGER:
          _data_type = DFFAPINT;
          TSQLVarPool[ _data_id ].pubdata.l_value = *( (int *)_data );
          TSQLVarPool[ _data_id ].cb = DEF_DATA_LEN;
          sprintf( str_data, "%d", *( (int *)_data ) );
          trc_db_data_rec( 0, _data_id, _data_type, "INT4", \
                              str_data, TSQLVarPool[ _data_id ].cb );
          break;
     case DFFAPINT:
     case DFFAPSERIAL:
          TSQLVarPool[ _data_id ].pubdata.l_value = *( (long *)_data );
          TSQLVarPool[ _data_id ].cb = DEF_DATA_LEN;
          sprintf( str_data, "%ld", *( (long *)_data ) );
          trc_db_data_rec( 0, _data_id, _data_type, "INT8", \
                              str_data, TSQLVarPool[ _data_id ].cb );
          break;
     case DFFAPMONEY:
     case DFFAPDECIMAL:
     case DFFAPFLOAT:
          TSQLVarPool[ _data_id ].pubdata.d_value = *( (double *)_data );
          TSQLVarPool[ _data_id ].cb = DEF_DATA_LEN;
          sprintf( str_data, "%lf", *( (double *)_data ) );
          trc_db_data_rec( 0, _data_id, _data_type, "FLOAT8", \
                              str_data, TSQLVarPool[ _data_id ].cb );
          break;
     case DFFAPSMFLOAT:
          TSQLVarPool[ _data_id ].pubdata.f_value = *( (float *)_data );
          TSQLVarPool[ _data_id ].cb = DEF_DATA_LEN;
          sprintf( str_data, "%f", *( (float *)_data ) );
          trc_db_data_rec( 0, _data_id, _data_type, "SMALLFLOAT4", \
                              str_data, TSQLVarPool[ _data_id ].cb );
          break;
     default:
          trc_db_data_rec( 0, _data_id, _data_type, "NODEFTYPE", \
                              "", 0 );
          APP_ERR( E_NODEF_TYPE )
    }


    TSQLVarPool[ _data_id ].type = _data_type;

    return E_OK;
}

/*------------------------------------------------------------------------*
 *  Function Name : void  InfGetData  ()                                  *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/

int InfGetData ( int _data_id , void * _data , int _len )
{
    char str_data[MAX_DOUBLE_STR_LEN];
    int  len;

    if ( _data == NULL ) {
        bclerreg( E_INVALARG, _FL_, "data buffer null" );
        return E_FAIL;
    }

    _data_id = _data_id + ( MAX_VAR_NUM/2 );
    if ( _data_id > (MAX_VAR_NUM-1) ) {
        bclerreg( E_INVALARG, _FL_, "data id" );
        return E_FAIL;
    }

    switch ( TSQLVarPool[ _data_id ].type ) {
     case DFFAPCHAR:
     case DFFAPDTIME:
     case DFFAPDATE:
     case DFFAPBYTES:
     case DFFAPTEXT:
     case DFFAPINTERVAL:
     case DFFAPNCHAR:
     case DFFAPNVCHAR:
          len = TSQLVarPool[_data_id].cb < _len?TSQLVarPool[_data_id].cb:_len;
          strncpy( _data, TSQLVarPool[ _data_id ].pubdata.c_value , len );
          if ( len < _len )
              ((char *)_data)[len] = 0x0;
          trc_db_data_rec( 1, _data_id - ( MAX_VAR_NUM/2 ), \
                              TSQLVarPool[ _data_id ].type, \
                              TSQLVarPool[ _data_id ].name, \
                              TSQLVarPool[ _data_id ].pubdata.c_value, 
                              TSQLVarPool[ _data_id ].cb );
          break;
     case DFFAPVCHAR:
     case DFFAPVCHAR2:
          len = TSQLVarPool[_data_id].cb < _len?TSQLVarPool[_data_id].cb:_len;
          memcpy( _data, TSQLVarPool[ _data_id ].pubdata.c_value , len );
          if ( len < _len )
              ((char *)_data)[len] = 0x0;
          trc_db_data_rec( 1, _data_id - ( MAX_VAR_NUM/2 ), \
                              TSQLVarPool[ _data_id ].type, \
                              TSQLVarPool[ _data_id ].name, \
                              TSQLVarPool[ _data_id ].pubdata.c_value, 
                              TSQLVarPool[ _data_id ].cb );
          break;
/* 20051014 handq 统一使用l_value/d_value */
     case DFFAPSMINT:
     case DFFAPINT:
     case DFFAPSERIAL:
          if( _len == sizeof( short ) )
              *( (short *)_data ) =
                        (short)TSQLVarPool[ _data_id ].pubdata.l_value;
          else if( _len == sizeof( int ) )
              *( (int *)_data ) =
                        ( int )TSQLVarPool[ _data_id ].pubdata.l_value;
          else
              *( (long *)_data ) =
                        ( long )TSQLVarPool[ _data_id ].pubdata.l_value;
          sprintf( str_data, "%ld", TSQLVarPool[ _data_id ].pubdata.l_value );
          trc_db_data_rec( 1, _data_id - ( MAX_VAR_NUM/2 ), \
                              TSQLVarPool[ _data_id ].type, \
                              TSQLVarPool[ _data_id ].name, \
                              str_data, TSQLVarPool[ _data_id ].cb );
          break;
     case DFFAPSMFLOAT:
     case DFFAPMONEY:
     case DFFAPDECIMAL:
     case DFFAPFLOAT:
          if( _len == sizeof( float ) )
              *( (float *)_data ) = TSQLVarPool[ _data_id ].pubdata.d_value;
          else
              *( (double *)_data ) = TSQLVarPool[ _data_id ].pubdata.d_value;
          sprintf( str_data, "%lf", TSQLVarPool[ _data_id ].pubdata.d_value );
          trc_db_data_rec( 1, _data_id - ( MAX_VAR_NUM/2 ), \
                              TSQLVarPool[ _data_id ].type, \
                              TSQLVarPool[ _data_id ].name, \
                              str_data, TSQLVarPool[ _data_id ].cb );
          break;
    }

    return( E_OK );
}

int InfGetType ( int _data_id , short * _type )
{
    _data_id = _data_id + ( MAX_VAR_NUM/2 );
    if ( _data_id > (MAX_VAR_NUM-1) ) {
        bclerreg( E_INVALARG, _FL_, "data id" );
        return E_FAIL;
    }

    switch ( TSQLVarPool[ _data_id ].type ) {
     case DFFAPCHAR:
     case DFFAPDTIME:
     case DFFAPDATE:
     case DFFAPBYTES:
     case DFFAPTEXT:
     case DFFAPINTERVAL:
     case DFFAPNCHAR:
     case DFFAPNVCHAR:
     case DFFAPVCHAR2:
          *_type = DSTRING;
          break;
     case DFFAPVCHAR:
          *_type = DBINARY;
          break;
     case DFFAPSMINT:
          *_type = DSHORT;
          break;
     case DFFAPINT:
     case DFFAPSERIAL:
          *_type = DLONG;
          break;
     case DFFAPMONEY:
     case DFFAPDECIMAL:
     case DFFAPFLOAT:
          *_type = DDOUBLE;
          break;
     case DFFAPSMFLOAT:
          *_type = DFLOAT;
          break;
    }

    return( E_OK );
}

/*------------------------------------------------------------------------*
 *  Function Name : void  InfGetStru  ()                                  *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/

int InfGetStru ( int _data_id , void * _data , int _len )
{
    tSqlValue	* sql_value;
    int		i;
    void	*p;

    if ( _data == NULL ) {
        bclerreg( E_INVALARG, _FL_, "data buffer null" );
        return E_FAIL;
    }

    p = _data;

    for( i = _data_id ; i < TSQLSTRU[FETCH_AREA]->column_num ; i++ ) {
        sql_value = &TSQLSTRU[FETCH_AREA]->tValuepb[i];
        switch ( sql_value->type ) {
             case DFFAPCHAR:
             case DFFAPBYTES:
             case DFFAPTEXT:
             case DFFAPINTERVAL:
             case DFFAPNCHAR:
             case DFFAPVCHAR:
             case DFFAPVCHAR2:
                  STRALIGN( p , sizeof( char ) );
                  InfGetData( i , p , sql_value->cb );
                  p = (void *)((long)p + sizeof( char ) * ( sql_value->cb + 1 ));
                  break;
             case DFFAPDTIME:
                  STRALIGN( p , sizeof( char ) );
                  InfGetData( i , p , DEF_DTIME_LEN );
                  p = (void *)((long)p + sizeof( char ) * ( DEF_DTIME_LEN + 1));
                  break;
             case DFFAPDATE:
                  STRALIGN( p , sizeof( char ) );
                  InfGetData( i , p , DEF_DATE_LEN );
                  p = (void *)((long)p + sizeof( char ) * ( DEF_DATE_LEN + 1 ));
                  break;
             case DFFAPSMINT:
                  STRALIGN( p , sizeof( short ) );
                  InfGetData( i , p , sizeof( short ) );
                  p = (void *)((long)p + sizeof( short ));
                  break;
             case DFFAPINT:
             case DFFAPSERIAL:
                  STRALIGN( p , sizeof( long ) );
                  InfGetData( i , p , sizeof( long ) );
                  p = (void *)((long)p + sizeof( long ));
                  break;
             case DFFAPMONEY:
             case DFFAPDECIMAL:
             case DFFAPFLOAT:
                  STRALIGN( p , sizeof( double ) );
                  InfGetData( i , p , sql_value->cb );
                  p = (void *)((long)p + sizeof( double ));
                  break;
             case DFFAPSMFLOAT:
                  STRALIGN( p , sizeof( float ) );
                  InfGetData( i , p , sql_value->cb );
                  p = (void *)((long)p + sizeof( float ));
                  break;
        }
/* 长度 = sizeof ( STRUCT ) */
        if( (long)p - (long)_data >= _len )
            break;
    }
    return( E_OK );
}

/*------------------------------------------------------------------------*
 *  Function Name : void  InfGetTabDesc()                                 *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/

int InfGetTabDesc( char * _tab_name , int * _len )
{
    EXEC SQL BEGIN DECLARE SECTION;
         short         h_get_serial;
         char          h_get_name[20];
         int           h_get_type;
         int           h_get_length;
         char          prepname[15];
         char          cursname[15];
         char          descname[15];
    EXEC SQL END DECLARE SECTION;
    char	_statement[SQLSTRLEN];
    int         ret;
    int		_area=0;
    int		data_id=0;

    int		i;

    sprintf(prepname,"selprep%d",_area);
    sprintf(cursname,"C%d",_area);
    sprintf(descname,"seldesc%d",_area);
    sprintf( _statement , "select * from %s where 1=0" , _tab_name );

    ret = InfSqlPrepare( TSQLSTRU[_area] , prepname, _statement );
    if( ret != E_OK ) {
        InfSqlDBFree( _area );
        return ret;
    }

    ret = InfSqlDeclare( TSQLSTRU[_area] , cursname,  0 );
    if( ret != E_OK ) {
        InfSqlDBFree( _area );
        return ret;
    }

    ret = InfSqlAllocate( TSQLSTRU[_area] , descname , DEF_ALLOC_NUM );
    if( ret != E_OK ) {
        InfSqlDBFree( _area );
        return ret;
    }

    ret = InfSqlOpenCur( TSQLSTRU[_area] );
    if( ret != E_OK ) {
        InfSqlDBFree( _area );
        return ret;
    }

    ret = InfSqlDescribe( TSQLSTRU[_area] );
    if( ret != E_OK ) {
        InfSqlDBFree( _area );
        return ret;
    }

    data_id =  MAX_VAR_NUM/2 ;
    for( i = data_id; i < TSQLSTRU[_area]->column_num + data_id ; i++ ) {
/* 20050526需要释放字符串类型，否则type改变后无法释放 */
        FreeCValue( &TSQLVarPool[ i ] );
        h_get_serial = i + 1 - data_id ;
        EXEC SQL get descriptor :descname VALUE :h_get_serial
                                                 :h_get_type   = TYPE,
                                                 :h_get_name   = NAME,
                                                 :h_get_length = LENGTH;
        bcl_trimall( h_get_name );
        strcpy( TSQLVarPool[ i ].name , h_get_name );
        TSQLVarPool[ i ].type = h_get_type;
        TSQLVarPool[ i ].cb = h_get_length;
    }

    *_len = TSQLSTRU[_area]->column_num;

    InfSqlDBFree( _area );
    return( E_OK );

}

/*------------------------------------------------------------------------*
 *  Function Name : void  InfPutStru  ()                                  *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/

int InfPutStru ( char * _tab_name , int _data_id , void * _data , int _len )
{
    int         ret;
    int		i, col_num;
    void	*p;
    tSqlValue	* sql_value;

    if( _data == NULL ) {
        bclerreg( E_INVALARG, _FL_, "data buffer null" );
        return E_FAIL;
    }

    p = _data;

    ret = InfGetTabDesc( _tab_name , &col_num );
    if( ret != E_OK ) {
        bclerreg( E_SQL , _FL_ , "InfGetTabDesc err.");
        return ret;
    }

    for( i = _data_id; i < col_num + _data_id ; i++ ) {
        sql_value = &TSQLVarPool[ i + MAX_VAR_NUM/2 - _data_id ];
        switch ( sql_value->type ) {
             case DFFAPCHAR:
             case DFFAPBYTES:
             case DFFAPTEXT:
             case DFFAPINTERVAL:
             case DFFAPNCHAR:
             case DFFAPVCHAR:
             case DFFAPVCHAR2:
                  STRALIGN( p , sizeof( char ) );
                  InfPutData( i , sql_value->type , p );
                  p = (void *)((long)p + sizeof( char ) * ( sql_value->cb + 1 ));
                  break;
             case DFFAPDTIME:
                  STRALIGN( p , sizeof( char ) );
                  InfPutData( i , sql_value->type , p );
                  p = (void *)((long)p + sizeof( char ) * ( DEF_DTIME_LEN + 1));
                  break;
             case DFFAPDATE:
                  STRALIGN( p , sizeof( char ) );
                  InfPutData( i , sql_value->type , p );
                  p = (void *)((long)p + sizeof( char ) * ( DEF_DATE_LEN + 1 ));
                  break;
             case DFFAPSMINT:
                  STRALIGN( p , sizeof( short ) );
                  InfPutData( i , sql_value->type , p );
                  p = (void *)((long)p + sizeof( short ));
                  break;
             case DFFAPINT:
             case DFFAPSERIAL:
                  STRALIGN( p , sizeof( long ) );
                  InfPutData( i , sql_value->type , p );
                  p = (void *)((long)p + sizeof( long ));
                  break;
             case DFFAPMONEY:
             case DFFAPDECIMAL:
             case DFFAPFLOAT:
                  STRALIGN( p , sizeof( double ) );
                  InfPutData( i , sql_value->type , p );
                  p = (void *)((long)p + sizeof( double ));
                  break;
             case DFFAPSMFLOAT:
                  STRALIGN( p , sizeof( float ) );
                  InfPutData( i , sql_value->type , p );
                  p = (void *)((long)p + sizeof( float ));
                  break;
        }
/* 长度 = sizeof ( STRUCT ) */
        if( (long)p - (long)_data >= _len )
            break;
    }

    return( E_OK );
}

/*------------------------------------------------------------------------*
 *  Function Name : void  InfGetDBErr  ()                                 *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/

void InfGetDBErr ( int _area )
{
    
    if ( TSQLSTRU == NULL )
        return;

    if ( TSQLSTRU[_area] == NULL )
        return;

    TSQLSTRU[_area]->sql_code = SQLCODE;

    TSQLSTRU[_area]->isam_code = sqlca.sqlerrd[1];

    TSQLSTRU[_area]->proc_row=sqlca.sqlerrd[2];
} 

/*------------------------------------------------------------------------*
 *  Function Name : int  InfSqlSetConnect  ()                             *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/
void
InfSqlSetConnect( char * _con_name )
{
    char sql_str[SQLSTRLEN];
    FAP_SQLCODE = 0;
    strcpy( CONNECT_NAME, _con_name );
    trc_db_rec( NULL, 0 );
    EXEC SQL SET CONNECTION :CONNECT_NAME;

    sprintf( sql_str, "SET CONNECTION %s", CONNECT_NAME );
    trc_db_rec( sql_str, sqlca.sqlcode );

    FAP_SQLCODE = sqlca.sqlcode;

    return;
}

/*------------------------------------------------------------------------*
 *  Function Name : int  InfSqlOpenDB  ()                                 *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/

int
InfSqlOpenDB( int _control_num, ... )
{
    EXEC SQL BEGIN DECLARE SECTION;
         char          h_db_name[20];
         char          h_server_name[20];
         char          h_passwd[20];
         char          h_connect[60];
    EXEC SQL END DECLARE SECTION;
    va_list ap;
    int i;
    int _area=0;
    char sql_str[SQLSTRLEN];

    FAP_SQLCODE = 0;
    memset(h_db_name,0x00,sizeof(h_db_name));
    memset(h_server_name,0x00,sizeof(h_server_name));
    memset(h_passwd,0x00,sizeof(h_passwd));
    va_start(ap,_control_num);
    for(i=0;i<_control_num;i++)  {
        switch(i)   {
            case 0:
                strcpy(h_db_name,va_arg( ap, char * ) );
                sprintf( h_connect, "%s" , h_db_name );
                break;
            case 1:
                strcpy(h_server_name,va_arg(ap,char *));
                if( strlen( h_server_name ) > 0 )
                    sprintf( h_connect, "%s@%s" , h_db_name, h_server_name );
                break;
            case 2:
                strcpy(h_passwd,va_arg(ap,char *));
                break;
        }
    }
    va_end(ap);

    if( CONNECT_NAME[0] == 0 ) {
        trc_db_rec( NULL, 0 );
        EXEC SQL DATABASE :h_connect;
        sprintf( sql_str, "DATABASE %s", h_connect );
    }
    else {
        trc_db_rec( NULL, 0 );
        EXEC SQL CONNECT TO :h_connect AS :CONNECT_NAME WITH CONCURRENT TRANSACTION;
        sprintf( sql_str, "CONNECT to %s AS %s WITH CONCURRENT TRANSACTION", h_connect, CONNECT_NAME );
    }
    trc_db_rec( sql_str, sqlca.sqlcode );

    if ( TSQLVarPool == NULL ) {
       TSQLVarPool = (tSqlValue * )calloc( 1, sizeof( tSqlValue ) * (MAX_VAR_NUM+1) );
       if ( TSQLVarPool == NULL ) {
          bclerreg( E_OSCALL , _FL_ , "calloc TSQLVarPool error" );
          return E_FAIL;
       }
    }
    if ( TSQLSTRU == NULL ) {
       TSQLSTRU = (tSqlStru **)calloc( 1, sizeof( tSqlStru * ) * (MAX_AREA+1) );
       if ( TSQLSTRU == NULL ) {
          bclerreg( E_OSCALL , _FL_ , "calloc TSQLSTRU error" );
          return E_FAIL;
       }
    }
    if( TSQLSTRU[0] == NULL ) {
        TSQLSTRU[0] = calloc( 1, sizeof(tSqlStru) );
        if( TSQLSTRU[0] == NULL ) {
            bclerreg( E_OSCALL , _FL_, "calloc TSQLSTRU[0] error" );
            return E_FAIL;
        }
    }

    InfGetDBErr( _area );
    DB_ERR( TSQLSTRU[_area] )

}

/*------------------------------------------------------------------------*
 *  Function Name : int InfSqlCloseDB ()                                  *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/
int
InfSqlCloseDB ( void )
{
    int _area=0;
    char sql_str[SQLSTRLEN];

    FAP_SQLCODE = 0;
    if ( TSQLSTRU == NULL )
        APP_ERR( E_DB_NOOPEN )

    while( InfFreeArea() );

/* 20031217 7.23关闭数据库时如果使用了atexit，在exit过程中使用下面处理会堵塞 */
    if( CONNECT_NAME[0] != 0 ) {
        trc_db_rec( NULL, 0 );
        EXEC SQL DISCONNECT :CONNECT_NAME;
        sprintf( sql_str, "DISCONNECT %s", CONNECT_NAME );
        trc_db_rec( sql_str, sqlca.sqlcode );
    }
    else {
        trc_db_rec( NULL, 0 );
        EXEC SQL DISCONNECT CURRENT;
        trc_db_rec( "DISCONNECT CURRENT", sqlca.sqlcode );
    }

    InfGetDBErr( _area );
    DB_ERR( TSQLSTRU[_area] )

}


/*------------------------------------------------------------------------*
 *  Function Name : int InfSqlBeginWork ()                                *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/
int 
InfSqlBeginWork ( void )
{
    int _area=0;

    FAP_SQLCODE = 0;
    if ( TSQLSTRU == NULL )
        APP_ERR( E_DB_NOOPEN )

    trc_db_rec( NULL, 0 );
    EXEC SQL BEGIN WORK;
    trc_db_rec( "BEGIN WORK", sqlca.sqlcode );
    InfGetDBErr( _area );
    DB_ERR( TSQLSTRU[_area] )
    
}

/*------------------------------------------------------------------------*
 *  Function Name : int InfSqlCommit()                                    *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/
int 
InfSqlCommit ( void )
{
    int _area=0;

    FAP_SQLCODE = 0;
    if ( TSQLSTRU == NULL )
        APP_ERR( E_DB_NOOPEN )

    trc_db_rec( NULL, 0 );
    EXEC SQL COMMIT WORK;
    trc_db_rec( "COMMIT WORK", sqlca.sqlcode );
    InfGetDBErr( _area );
    DB_ERR( TSQLSTRU[_area] )
    
}

/*------------------------------------------------------------------------*
 *  Function Name : int InfSqlRollBack  ()                                *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/
int 
InfSqlRollBack  ( void )
{
    int _area=0;

    FAP_SQLCODE = 0;
    if ( TSQLSTRU == NULL )
        APP_ERR( E_DB_NOOPEN )

    trc_db_rec( NULL, 0 );
    EXEC SQL ROLLBACK WORK;
    trc_db_rec( "ROLLBACK WORK", sqlca.sqlcode );
    InfGetDBErr( _area );
    DB_ERR( TSQLSTRU[_area] )
    
}
/*------------------------------------------------------------------------*
 *  Function Name : int InfSqlDBFree  ()                                  *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/
int 
InfSqlDBFree  ( int _area )
{
    int ret;

    if ( TSQLSTRU == NULL )
        APP_ERR( E_DB_NOOPEN )

    FreeValueBuf( TSQLSTRU[_area] );
    if( TSQLSTRU[_area]->tsqlstat.cur_openstat )
        ret = InfSqlCloseCur( TSQLSTRU[_area] );
    if( TSQLSTRU[_area]->tsqlstat.prep_stat )
        ret = InfSqlFreePrep( TSQLSTRU[_area] );

    if( TSQLSTRU[_area]->tsqlstat.cur_allocstat )
        ret = InfSqlFreeCur( TSQLSTRU[_area] );
    if( TSQLSTRU[_area]->tsqlstat.alloc_stat )
        ret = InfSqlDealloc( TSQLSTRU[_area] );

    TSQLSTRU[_area]->sql_code = 0;
    TSQLSTRU[_area]->user_code = 0;

    TSQLSTRU[_area]->ques_num = 0;
    TSQLSTRU[_area]->tsqlstat.put_stat = 0;
    strcpy( TSQLSTRU[_area]->user_msg , "" );
    strcpy( TSQLSTRU[_area]->sql_str , "" );

    return E_OK;
}

/*------------------------------------------------------------------------*
 *  Function Name : int InfSqlSelect  ()                                  *
 *  Description   :                                                       *
 *  Input         : _area 工作区号  1 -- MAX_AREA (1-10)                  *
 *                : _statement 查询语句                                   *
 *                : _op_flag 游标操作方式 0 - NEXT                        *
 *                :                       P - PREVIOUS                    *
 *                :                       A - ABSOLUTE                    *
 *                :                       C - CURRENT                     *
 *                :                       F - FIRST                       *
 *                :                       L - LAST                        *
 *                :                       R - RELATIVE                    *
 *                : _offset 游标的偏移量                                  *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/
int 
InfSqlSelect ( int _area , char * _statement , char _op_flag , long _offset )
{
    char        prepname[15];
    char        cursname[15];
    char        descname[15];
    int         ret;
    int         alloc_num;
    int         i;
    char        str_tmp[128];
    tSqlValue	*sql_value;

    FAP_SQLCODE = 0;
    sprintf(prepname,"selprep%d",_area);
    sprintf(cursname,"C%d",_area);
    sprintf(descname,"seldesc%d",_area);

    if ( TSQLSTRU == NULL )
        APP_ERR( E_DB_NOOPEN )

    if ( _area >= MAX_AREA || TSQLSTRU[_area] == NULL ) {
        sprintf( str_tmp , "work area [%d] not calloc" , _area );
        bclerreg( E_INVALARG, _FL_, str_tmp );
        return E_FAIL;
    }

/* 已fetch的工作区可直接继续fetch */
    if ( !TSQLSTRU[_area]->tsqlstat.cur_fetchstat ) {
        alloc_num = get_comma_num( _statement , ',' )+1;
        alloc_num =0;
        ret = InfSqlPrepare( TSQLSTRU[_area] , prepname, _statement );
        if ( ret != E_OK ) {
            bclerreg( E_SQL , _FL_ ,"InfSqlPrepare err.");
            return ret;
        }

        ret = InfSqlDeclare( TSQLSTRU[_area] , cursname,  _op_flag );
        if ( ret != E_OK ) {
            bclerreg( E_SQL , _FL_ ,"InfSqlDeclare err.");
            return ret;
        }

        if ( alloc_num < DEF_ALLOC_NUM )
            alloc_num = DEF_ALLOC_NUM;
        ret = InfSqlAllocate( TSQLSTRU[_area] , descname , alloc_num );
        if ( ret != E_OK ) {
            bclerreg( E_SQL , _FL_ ,"InfSqlAllocate err.");
            return ret;
        }

/* 为查询条件赋值 */
        if( TSQLSTRU[_area]->ques_num != 0 )  {
            ret = AllocValueBuf( TSQLSTRU[_area] , TSQLSTRU[_area]->ques_num );
            if ( ret != E_OK ) {
                bclerreg( E_SQL , _FL_ ,"AllocValueBuf err.");
                return ret;
            }

            for( i=1; i <= TSQLSTRU[_area]->ques_num; i++ )  {
                memcpy( &TSQLSTRU[_area]->tValuepb[i-1] , &TSQLVarPool[ i-1 ] ,sizeof( tSqlValue ) );

                ret = InfSqlPutValue( TSQLSTRU[_area] , (short)i );
                if ( ret != E_OK ) {
                    bclerreg( E_SQL , _FL_ ,"InfSqlPutValue err.");
                    return ret;
                }
            } /* end of for */

        } /* end of if ques_num */

        ret = InfSqlOpenCur( TSQLSTRU[_area] );
        if ( ret != E_OK ) {
            bclerreg( E_SQL , _FL_ ,"InfSqlOpenCur err.");
            return ret;
        }

        ret = InfSqlDescribe( TSQLSTRU[_area] );
        if ( ret != E_OK ) {
            bclerreg( E_SQL , _FL_ ,"InfSqlDescibe err.");
            return ret;
        }

    } /* end of if fetchstat */
 
    /*** 针对一个area有多个不同方式的DBSelect的情况 ***/
    TSQLSTRU[_area]->op_flag = _op_flag ; 
    ret = InfSqlFetchCur( TSQLSTRU[_area] , _offset );
    if ( ret == E_FAIL ) {
        bclerreg( E_SQL , _FL_ ,"InfSqlFetchCur err.");
        return ret;
    }

    if ( ret == E_NOTFOUND ) 
        return ret;

/* 为查询结果赋值 */
    ret = AllocValueBuf( TSQLSTRU[_area] , TSQLSTRU[_area]->column_num );
    if ( ret != E_OK ) {
        bclerreg( E_SQL , _FL_ ,"AllocValueBuf err.");
        return ret;
    }

    for( i = 1; i <= TSQLSTRU[_area]->column_num ; i++ ) {
        ret = InfSqlGetValue( TSQLSTRU[_area] , (short)i );
        if ( ret != E_OK )  {
            bclerreg( E_SQL , _FL_ , "InfSqlGetValue err.");
            return ret;
        }

        sql_value = &TSQLVarPool[ i+(MAX_VAR_NUM/2)-1 ];
        FreeCValue( sql_value );
       
        memcpy( sql_value, &TSQLSTRU[_area]->tValuepb[i-1], sizeof(tSqlValue) );
    }

/* 保留当前操作的工作区号 */
    FETCH_AREA = _area;
    return E_OK;
}

/*------------------------------------------------------------------------*
 *  Function Name : int InfSqlExec  ()                                    *
 *  Description   : 使用默认工作区0                                       *
 *  Input         :                                                       *
 *                : _statement 查询语句                                   *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/
int 
InfSqlExec ( char * _statement )
{
    int         _area=0;             /* 使用默认工作区0  */ 
    char        prepname[15];
    char        descname[15];
    int         ret;
    int         alloc_num;
    int         i;
    char        str_tmp[128];

    FAP_SQLCODE = 0;
    sprintf(prepname,"insprep%d",_area);
    sprintf(descname,"insdesc%d",_area);

    if ( TSQLSTRU == NULL )
        APP_ERR( E_DB_NOOPEN )

    if ( TSQLSTRU[_area] == NULL ) {
        sprintf( str_tmp , "work area [%d] not calloc" , _area );
        bclerreg( E_INVALARG, _FL_, str_tmp );
        return E_FAIL;
    }

    alloc_num = get_comma_num( _statement , '?' )+1;
    ret = InfSqlPrepare( TSQLSTRU[_area] , prepname, _statement );
    if ( ret != E_OK ){
        bclerreg( E_SQL , _FL_ , "InfSqlPrepare err.");
        InfSqlDBFree( _area );
        return ret;
    }

    if ( alloc_num < DEF_ALLOC_NUM )
        alloc_num = DEF_ALLOC_NUM;
    ret = InfSqlAllocate( TSQLSTRU[_area] , descname , alloc_num );
    if ( ret != E_OK ) {
        InfSqlDBFree( _area );
        return ret;
    }

/* 为查询条件赋值 */
    if( TSQLSTRU[_area]->ques_num != 0 )  {
        ret = AllocValueBuf( TSQLSTRU[_area] , TSQLSTRU[_area]->ques_num );
        if ( ret != E_OK ) {
           InfSqlDBFree( _area );
           return ret;
        }

        for( i=1; i <= TSQLSTRU[_area]->ques_num; i++ )  {
            memcpy( &TSQLSTRU[_area]->tValuepb[i-1] , &TSQLVarPool[ i-1 ] ,sizeof( tSqlValue ) );

            ret = InfSqlPutValue( TSQLSTRU[_area] ,(short)i );
            if ( ret != E_OK ) {
                InfSqlDBFree( _area );
                return ret;
            }
        } /* end of for */

    } /* end of if ques_num */

    ret = InfSqlExecute( TSQLSTRU[_area] );
    InfSqlDBFree( _area );

    if ( ret != E_OK ) {
        bclerreg( E_SQL , _FL_ , "InfSqlExecute err.");
        return ret;
    }
    
/* 20051027 提高效率
    strncpy( str_tmp, _statement, 10 );
    bcl_str_toupper( str_tmp );
    if ( TSQLSTRU[_area]->proc_row == 0 && strstr(str_tmp,"CREATE") == NULL \
                && strstr(str_tmp,"DROP")== NULL )
       return E_NOTFOUND;
*/
    if ( TSQLSTRU[_area]->proc_row == 0 )
       return E_NOTFOUND;

    return E_OK;
}

/* 20051027 支持批量处理insert/update提高效率 */
int
InfSqlBatExec ( int _area , char * _statement )
{
    char        prepname[15];
    char        descname[15];
    int         ret;
    int         alloc_num;
    int         i;
    char        str_tmp[128];

    FAP_SQLCODE = 0;
    sprintf(prepname,"selprep%d",_area);
    sprintf(descname,"seldesc%d",_area);

    if ( TSQLSTRU == NULL )
        APP_ERR( E_DB_NOOPEN )

    if ( _area >= MAX_AREA || TSQLSTRU[_area] == NULL ) {
        sprintf( str_tmp , "work area [%d] not calloc" , _area );
        bclerreg( E_INVALARG, _FL_, str_tmp );
        return E_FAIL;
    }

/* 已prepare的工作区可直接继续Exec */
    if ( !TSQLSTRU[_area]->tsqlstat.prep_stat ) {
        alloc_num = get_comma_num( _statement , ',' )+1;
        alloc_num =0;
        ret = InfSqlPrepare( TSQLSTRU[_area] , prepname, _statement );
        if ( ret != E_OK ) {
            bclerreg( E_SQL , _FL_ ,"InfSqlPrepare err.");
            TSQLSTRU[_area]->sql_code = 0;
            TSQLSTRU[_area]->user_code = 0;
            return ret;
        }

        if ( alloc_num < DEF_ALLOC_NUM )
            alloc_num = DEF_ALLOC_NUM;
        ret = InfSqlAllocate( TSQLSTRU[_area] , descname , alloc_num );
        if ( ret != E_OK ) {
            TSQLSTRU[_area]->sql_code = 0;
            TSQLSTRU[_area]->user_code = 0;
            return ret;
        }

/* 为查询条件赋值 */
        if( TSQLSTRU[_area]->ques_num != 0 )  {
            ret = AllocValueBuf( TSQLSTRU[_area] , TSQLSTRU[_area]->ques_num );
            if ( ret != E_OK ) {
                bclerreg( E_SQL , _FL_ ,"AllocValueBuf err.");
                TSQLSTRU[_area]->sql_code = 0;
                TSQLSTRU[_area]->user_code = 0;
                return ret;
            }

            for( i=1; i <= TSQLSTRU[_area]->ques_num; i++ )  {
                memcpy( &TSQLSTRU[_area]->tValuepb[i-1] , &TSQLVarPool[ i-1 ], \
                        sizeof( tSqlValue ) );
                ret = InfSqlPutValue( TSQLSTRU[_area] , (short)i );
                if ( ret != E_OK ) {
                    bclerreg( E_SQL , _FL_ ,"InfSqlPutValue err.");
                    TSQLSTRU[_area]->sql_code = 0;
                    TSQLSTRU[_area]->user_code = 0;
                    return ret;
                }
            } /* end of for */

        } /* end of if ques_num */
        EXEC SQL SET BUFFERED LOG;

    } /* end of if prep_stat */
    else
        for( i=1; i <= TSQLSTRU[_area]->ques_num; i++ )  {
            memcpy( &TSQLSTRU[_area]->tValuepb[i-1] , &TSQLVarPool[ i-1 ], \
                    sizeof( tSqlValue ) );
            ret = InfSqlPutValue( TSQLSTRU[_area] , (short)i );
            if ( ret != E_OK ) {
                bclerreg( E_SQL , _FL_ ,"InfSqlPutValue err.");
                TSQLSTRU[_area]->sql_code = 0;
                TSQLSTRU[_area]->user_code = 0;
                return ret;
            }
        } /* end of for */
    ret = InfSqlExecute( TSQLSTRU[_area] );
    TSQLSTRU[_area]->tsqlstat.put_stat = 0;
    TSQLSTRU[_area]->sql_code = 0;
    TSQLSTRU[_area]->user_code = 0;
    if ( ret == E_FAIL ) {
        bclerreg( E_SQL , _FL_ ,"InfSqlExecute err.");
        return ret;
    }

    if ( TSQLSTRU[_area]->proc_row == 0 )
       return E_NOTFOUND;
    return E_OK;
}
