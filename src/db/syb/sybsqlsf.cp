/* #ident "@(#)infsf.ec,v 1.0 2000/09/25 10:17:26 hdq SYBASE 11.x interface source"
 *      SCCS IDENTIFICATION
 * copyright (C) 2000, 2000 by hdq.
 * MUST be distributed in source form only.
 */

/*************************************************************************************************
 * File            : sybsf.cp                                                                    *
 * System Name     : StarRing                                                                    *
 * VERSION         : 1.0                                                                         *
 * LANGUAGE        : ESQLC                                                                       *
 * OS & ENVIROMENT : SCO UNIX  5                                                                 *
 *                   SYBASE    11.x                                                              *
 * DESCRIPTION     : this program refer to all of functions about                                *
 *                   operation of database( Sybase sql function)                                 *
 * HISTORY:                                                                                      *
 * MM/DD/YYYY      ADDRESS          PROGRAMMER            DESCRIPTION                            *
 *-----------------------------------------------------------------------------------------------*
 * 09/25/2000      NingBo           Hdq                   Creation                               *
 * 04/10/2008      BeiJing          Chenff                Modify SybMallocArea() Add variables   *  
 *                                                        tSqlValue *tv=NULL;  int max_num = 0;  *
 *                                                                                               *
 *************************************************************************************************/

#include <arcbcl.h>
#include <arclog.h>
#include <__arcde.h>

EXEC SQL include sqlca;

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
    CS_CHAR CONNECT_NAME[64]="conn00";
EXEC SQL END DECLARE SECTION;

/*Function in this program define*/

/*------------------------------------------------------------------------*
 *  Function Name : int  SybMallocArea()                                  *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/
int 
SybMallocArea()
{
   int i;
   
   /* chenff 090409  add these variables define */
   tSqlValue *tv=NULL;  
   int max_num = 0; 

   if ( CURRENT_AREA < MAX_AREA-1 )
       CURRENT_AREA++;
   else {
       bclerreg( E_MAXAREA, _FL_, CURRENT_AREA + 1 );
       return E_FAIL;
   }

   i = CURRENT_AREA;

   if ( TSQLSTRU == NULL )
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
 *  Function Name : int  SybFreeArea()                                    *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/
int 
SybFreeArea()
{
   int i;

   i = CURRENT_AREA;
   if ( CURRENT_AREA > 0 ) {

      if ( TSQLSTRU[ CURRENT_AREA ] != NULL )
          SybSqlDBFree( CURRENT_AREA );

      CURRENT_AREA--;
   }

   return i;
}

/*------------------------------------------------------------------------*
 *  Function Name : int  SybFreeAreaD()                                   *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/
int 
SybFreeAreaD( char *dtaname )
{
   int i;

   i = CURRENT_AREA;
   if ( CURRENT_AREA > 0 ) {

      if ( TSQLSTRU[ CURRENT_AREA ] != NULL ) {
          bcl_log( "DBDBG.log", _FL_, "DTA[%s]执行语句[%s]的工作区[%d]未释放", dtaname, TSQLSTRU[CURRENT_AREA]->sql_str, CURRENT_AREA );
          SybSqlDBFree( CURRENT_AREA );
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
     case DFFAPBYTES:
     case DFFAPTEXT:
     case DFFAPVCHAR:
     case DFFAPNCHAR:
     case DFFAPNVCHAR:
     case DFFAPINTERVAL:
     case DFFAPDTIME:
     case DFFAPDATE:
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
 *  Function Name : void  SybPutData  ()                                  *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/

int SybPutData ( int _data_id , int _data_type , void * _data )
{
    char str_data[MAX_DOUBLE_STR_LEN];
    int str_len;

    if ( _data == NULL ) {
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
     case DFFAPBYTES:
     case DFFAPTEXT:
     case DFFAPVCHAR:
     case DFFAPNCHAR:
     case DFFAPNVCHAR:
     case DFFAPINTERVAL:
     case DFFAPDTIME:
     case DFFAPDATE:
          if( _data_type == DFFAPVCHAR )
              str_len = DEF_VARCHAR_LEN;
          else
              str_len = strlen( ( char *)_data );
          TSQLVarPool[ _data_id ].pubdata.c_value = calloc( 1, str_len + 1 );
          if ( TSQLVarPool[ _data_id ].pubdata.c_value == NULL ) {
              bclerreg( E_ALLOC, _FL_, str_len + 1 );
              return E_FAIL;
          }

          TSQLVarPool[ _data_id ].cb = str_len;
          if ( _data_type == DFFAPDTIME )
             TSQLVarPool[ _data_id ].cb = DEF_DTIME_LEN;
          if ( _data_type == DFFAPDATE )
             TSQLVarPool[ _data_id ].cb = DEF_DATE_LEN;

          if( _data_type == DFFAPVCHAR )
              memcpy( TSQLVarPool[ _data_id ].pubdata.c_value, _data, str_len );
          else
              strcpy( TSQLVarPool[ _data_id ].pubdata.c_value , _data );

          trc_db_data_rec( 0, _data_id, _data_type, "CHAR", _data, TSQLVarPool[ _data_id ].cb );
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
 *  Function Name : void  SybGetData  ()                                  *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/

int SybGetData ( int _data_id , void * _data , int _len )
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
     case DFFAPBYTES:
     case DFFAPTEXT:
     case DFFAPNCHAR:
     case DFFAPNVCHAR:
     case DFFAPINTERVAL:
     case DFFAPDTIME:
     case DFFAPDATE:
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

int SybGetType ( int _data_id , short * _type )
{
    _data_id = _data_id + ( MAX_VAR_NUM/2 );
    if ( _data_id > (MAX_VAR_NUM-1) ) {
        bclerreg( E_INVALARG, _FL_, "data id" );
        return E_FAIL;
    }

    switch ( TSQLVarPool[ _data_id ].type ) {
     case DFFAPCHAR:
     case DFFAPBYTES:
     case DFFAPTEXT:
     case DFFAPNCHAR:
     case DFFAPNVCHAR:
     case DFFAPINTERVAL:
     case DFFAPDTIME:
     case DFFAPDATE:
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
 *  Function Name : void  SybGetStru  ()                                  *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/

int SybGetStru ( int _data_id , void * _data , int _len )
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
             case DFFAPVCHAR:
             case DFFAPNCHAR:
             case DFFAPNVCHAR:
             case DFFAPINTERVAL:
                  STRALIGN( p , sizeof( char ) );
                  SybGetData( i , p , sql_value->cb );
                  p = (void *)((long)p + sizeof( char ) * ( sql_value->cb + 1 ));
                  break;
             case DFFAPDTIME:
                  STRALIGN( p , sizeof( char ) );
                  SybGetData( i , p , DEF_DTIME_LEN );
                  p = (void *)((long)p + sizeof( char ) * ( DEF_DTIME_LEN + 1));
                  break;
             case DFFAPDATE:
                  STRALIGN( p , sizeof( char ) );
                  SybGetData( i , p , DEF_DATE_LEN );
                  p = (void *)((long)p + sizeof( char ) * ( DEF_DATE_LEN + 1 ));
                  break;
             case DFFAPSMINT:
                  STRALIGN( p , sizeof( short ) );
                  SybGetData( i , p , sizeof( short ) );
                  p = (void *)((long)p + sizeof( short ));
                  break;
             case DFFAPINT:
             case DFFAPSERIAL:
                  STRALIGN( p , sizeof( long ) );
                  SybGetData( i , p , sizeof( long ) );
                  p = (void *)((long)p + sizeof( long ));
                  break;
             case DFFAPMONEY:
             case DFFAPDECIMAL:
             case DFFAPFLOAT:
                  STRALIGN( p , sizeof( double ) );
                  SybGetData( i , p , sql_value->cb );
                  p = (void *)((long)p + sizeof( double ));
                  break;
             case DFFAPSMFLOAT:
                  STRALIGN( p , sizeof( float ) );
                  SybGetData( i , p , sql_value->cb );
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
 *  Function Name : void  SybGetTabDesc()                                 *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/

int SybGetTabDesc( char * _tab_name , int * _len )
{
    EXEC SQL BEGIN DECLARE SECTION;
         CS_SMALLINT      h_get_serial;
         CS_CHAR          h_get_name[20];
         CS_INT           h_get_type=0;
         CS_INT           h_get_length=0;
         CS_CHAR          prepname[15];
         CS_CHAR          cursname[15];
         CS_CHAR          descname[15];
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

    ret = SybSqlPrepare( TSQLSTRU[_area] , prepname, _statement );
    if( ret != E_OK ) {
        SybSqlDBFree( _area );
        return ret;
    }

    ret = SybSqlDeclare( TSQLSTRU[_area] , cursname,  0 );
    if( ret != E_OK ) {
        SybSqlDBFree( _area );
        return ret;
    }

    ret = SybSqlAllocate( TSQLSTRU[_area] , descname , DEF_ALLOC_NUM );
    if( ret != E_OK ) {
        SybSqlDBFree( _area );
        return ret;
    }

    ret = SybSqlOpenCur( TSQLSTRU[_area] );
    if( ret != E_OK ) {
        SybSqlDBFree( _area );
        return ret;
    }

    ret = SybSqlDescribe( TSQLSTRU[_area] );
    if( ret != E_OK ) {
        SybSqlDBFree( _area );
        return ret;
    }

    data_id =  MAX_VAR_NUM/2 ;
/* sybase use output descriptor get data */
    strcat( descname , "out" );
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

    SybSqlDBFree( _area );
    return( E_OK );

}

/*------------------------------------------------------------------------*
 *  Function Name : void  SybPutStru  ()                                  *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/

int SybPutStru ( char * _tab_name , int _data_id , void * _data , int _len )
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

    ret = SybGetTabDesc( _tab_name , &col_num );
    if( ret != E_OK )
        return ret;

    for( i = _data_id; i < col_num + _data_id ; i++ ) {
        sql_value = &TSQLVarPool[ i + MAX_VAR_NUM/2 - _data_id ];
        switch ( sql_value->type ) {
             case DFFAPCHAR:
             case DFFAPBYTES:
             case DFFAPTEXT:
             case DFFAPVCHAR:
             case DFFAPNCHAR:
             case DFFAPNVCHAR:
             case DFFAPINTERVAL:
                  STRALIGN( p , sizeof( char ) );
                  SybPutData( i , sql_value->type , p );
                  p = (void *)((long)p + sizeof( char ) * ( sql_value->cb + 1 ));
                  break;
             case DFFAPDTIME:
                  STRALIGN( p , sizeof( char ) );
                  SybPutData( i , sql_value->type , p );
                  p = (void *)((long)p + sizeof( char ) * ( DEF_DTIME_LEN + 1));
                  break;
             case DFFAPDATE:
                  STRALIGN( p , sizeof( char ) );
                  SybPutData( i , sql_value->type , p );
                  p = (void *)((long)p + sizeof( char ) * ( DEF_DATE_LEN + 1 ));
                  break;
             case DFFAPSMINT:
                  STRALIGN( p , sizeof( short ) );
                  SybPutData( i , sql_value->type , p );
                  p = (void *)((long)p + sizeof( short ));
                  break;
             case DFFAPINT:
             case DFFAPSERIAL:
                  STRALIGN( p , sizeof( long ) );
                  SybPutData( i , sql_value->type , p );
                  p = (void *)((long)p + sizeof( long ));
                  break;
             case DFFAPMONEY:
             case DFFAPDECIMAL:
             case DFFAPFLOAT:
                  STRALIGN( p , sizeof( double ) );
                  SybPutData( i , sql_value->type , p );
                  p = (void *)((long)p + sizeof( double ));
                  break;
             case DFFAPSMFLOAT:
                  STRALIGN( p , sizeof( float ) );
                  SybPutData( i , sql_value->type , p );
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
 *  Function Name : void  SybGetDBErr  ()                                 *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/

void SybGetDBErr ( int _area )
{
    
    if ( TSQLSTRU == NULL )
        return;

    if ( TSQLSTRU[_area] == NULL )
        return;

    TSQLSTRU[_area]->sql_code = sqlca.sqlcode;

    TSQLSTRU[_area]->isam_code = sqlca.sqlerrd[1];

    TSQLSTRU[_area]->proc_row=sqlca.sqlerrd[2];
} 

/*------------------------------------------------------------------------*
 *  Function Name : int  SybSqlOpenDB  ()                                 *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/
void error_handler()
{
        fprintf(stderr, "\n** SQLCODE=(%ld)", sqlca.sqlcode);

        if (sqlca.sqlerrm.sqlerrml)
        {
                fprintf(stderr, "\n** SQL Server Error ");
                fprintf(stderr, "\n** %s", sqlca.sqlerrm.sqlerrmc);
        }

        fprintf(stderr, "\n\n");

        exit(-1);
}

void
SybSqlSetConnect( char *_con_name )
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

int
SybSqlOpenDB  ( int _control_num,...)
{
    EXEC SQL BEGIN DECLARE SECTION;
         CS_CHAR          h_db_name[20];
         CS_CHAR          h_server_name[20];
         CS_CHAR          h_passwd[20];
    EXEC SQL END DECLARE SECTION;
    va_list ap;
    int i;
    int _area=0;
    char sql_str[SQLSTRLEN];

    FAP_SQLCODE = 0;
    va_start(ap,_control_num);
    memset(h_db_name,0x00,sizeof(h_db_name));
    memset(h_server_name,0x00,sizeof(h_server_name));
    memset(h_passwd,0x00,sizeof(h_passwd));
    for(i=0;i<_control_num;i++)  {
        switch(i)   {
            case 0:
                strcpy(h_db_name,va_arg( ap, char * ) );
                break;
            case 1:
                strcpy(h_server_name,va_arg(ap,char *));
                break;
            case 2:
                strcpy(h_passwd,va_arg(ap,char *));
                break;
        }
    }
    va_end(ap);

/*
    EXEC SQL connect to :h_db_name user :h_server_name ;
	EXEC SQL WHENEVER SQLERROR CALL error_handler();
	EXEC SQL WHENEVER SQLWARNING CONTINUE;
	EXEC SQL WHENEVER NOT FOUND CONTINUE;
*/

    trc_db_rec( NULL, 0 );
    EXEC SQL connect :h_server_name identified by :h_passwd AT :CONNECT_NAME;
    
    sprintf( sql_str, "CONNECT :%s IDENTIFIED BY :%s AT :%s", \
                       h_server_name, h_passwd, CONNECT_NAME );
    trc_db_rec( sql_str, sqlca.sqlcode );
    if( sqlca.sqlcode >= 0 ) {
        EXEC SQL USE :h_db_name;
        sprintf( sql_str, "USE :%s", h_db_name );
        trc_db_rec( sql_str, sqlca.sqlcode );
    }

    if ( TSQLVarPool == NULL ) {
       TSQLVarPool = (tSqlValue * )calloc( 1, sizeof( tSqlValue ) * (MAX_VAR_NUM+1) );
       if ( TSQLVarPool == NULL ) {
          bclerreg( E_ALLOC , _FL_, sizeof( tSqlValue ) * (MAX_VAR_NUM+1) );
          return E_FAIL;
       }
    }
    if ( TSQLSTRU == NULL ) {
       TSQLSTRU = (tSqlStru **)calloc( 1, sizeof( tSqlStru * ) * (MAX_AREA+1) );
       if ( TSQLSTRU == NULL ) {
          bclerreg( E_ALLOC , _FL_, sizeof( tSqlStru * ) * (MAX_AREA+1) );
          return E_FAIL;
       }
    }
    if ( TSQLSTRU[0] == NULL ) {
       TSQLSTRU[0] = calloc( 1, sizeof(tSqlStru) );
       if ( TSQLSTRU[0] == NULL ) {
          bclerreg( E_OSCALL , _FL_, "calloc TSQLSTRU[0] error" );
          return E_FAIL;
       }
    }

    SybGetDBErr( _area );
    DB_ERR( TSQLSTRU[_area] )

}

/*------------------------------------------------------------------------*
 *  Function Name : int SybSqlCloseDB ()                                  *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/
int
SybSqlCloseDB ( void )
{
    int _area=0;

    FAP_SQLCODE = 0;
    if ( TSQLSTRU == NULL )
        APP_ERR( E_DB_NOOPEN )

    while( SybFreeArea() );

    trc_db_rec( NULL, 0 );
    EXEC SQL disconnect current;
    trc_db_rec( "DISCONNECT CURRENT", sqlca.sqlcode );

    SybGetDBErr( _area );
    DB_ERR( TSQLSTRU[_area] )
    
}


/*------------------------------------------------------------------------*
 *  Function Name : int SybSqlBeginWork ()                                *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/
int 
SybSqlBeginWork ( void )
{
    int _area=0;

    FAP_SQLCODE = 0;
    if ( TSQLSTRU == NULL )
        APP_ERR( E_DB_NOOPEN )

    trc_db_rec( NULL, 0 );
    EXEC SQL BEGIN TRANSACTION;
    trc_db_rec( "BEGIN TRANSACTION", sqlca.sqlcode );
    SybGetDBErr( _area );
    DB_ERR( TSQLSTRU[_area] )
    
}

/*------------------------------------------------------------------------*
 *  Function Name : int SybSqlCommit()                                    *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/
int 
SybSqlCommit ( void )
{
    int _area=0;

    FAP_SQLCODE = 0;
    if ( TSQLSTRU == NULL )
        APP_ERR( E_DB_NOOPEN )

    trc_db_rec( NULL, 0 );
    EXEC SQL COMMIT WORK;
    trc_db_rec( "COMMIT WORK", sqlca.sqlcode );
    SybGetDBErr( _area );
    DB_ERR( TSQLSTRU[_area] )
    
}

/*------------------------------------------------------------------------*
 *  Function Name : int SybSqlRollBack  ()                                *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/
int 
SybSqlRollBack  ( void )
{
    int _area=0;

    FAP_SQLCODE = 0;
    if ( TSQLSTRU == NULL )
        APP_ERR( E_DB_NOOPEN )

    trc_db_rec( NULL, 0 );
    EXEC SQL ROLLBACK WORK;
    trc_db_rec( "ROLLBACK WORK", sqlca.sqlcode );
    SybGetDBErr( _area );
    DB_ERR( TSQLSTRU[_area] )
    
}

/*------------------------------------------------------------------------*
 *  Function Name : int SybSqlDBFree  ()                                  *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/
int 
SybSqlDBFree  ( int _area )
{
    int ret;

    if ( TSQLSTRU == NULL )
        APP_ERR( E_DB_NOOPEN )

    FreeValueBuf( TSQLSTRU[_area] );
/*    
    if( TSQLSTRU[_area]->tsqlstat.cur_openstat ) {
        ret = SybSqlCloseCur( TSQLSTRU[_area] );
        if( ret != E_OK ) {
            SybGetDBErr( _area );
            DB_ERR( TSQLSTRU[_area] )
        }
    }
    if( TSQLSTRU[_area]->tsqlstat.cur_allocstat ) {
        ret = SybSqlFreeCur( TSQLSTRU[_area] );
        if( ret != E_OK ) {
            SybGetDBErr( _area );
            DB_ERR( TSQLSTRU[_area] )
        }
    }

    if( TSQLSTRU[_area]->tsqlstat.prep_stat ) {
        ret = SybSqlFreePrep( TSQLSTRU[_area] );
        if( ret != E_OK ) {
            SybGetDBErr( _area );
            DB_ERR( TSQLSTRU[_area] )
        }
    }

    if( TSQLSTRU[_area]->tsqlstat.alloc_stat ) {
        ret = SybSqlDealloc( TSQLSTRU[_area] );
        if( ret != E_OK ) {
            SybGetDBErr( _area );
            DB_ERR( TSQLSTRU[_area] )
        }
    }
*/
    if( TSQLSTRU[_area]->tsqlstat.cur_openstat )
        ret = SybSqlCloseCur( TSQLSTRU[_area] );
    if( TSQLSTRU[_area]->tsqlstat.cur_allocstat )
        ret = SybSqlFreeCur( TSQLSTRU[_area] );
    if( TSQLSTRU[_area]->tsqlstat.prep_stat )
        ret = SybSqlFreePrep( TSQLSTRU[_area] );
    if( TSQLSTRU[_area]->tsqlstat.alloc_stat )
        ret = SybSqlDealloc( TSQLSTRU[_area] );

    TSQLSTRU[_area]->sql_code = 0;
    TSQLSTRU[_area]->user_code = 0;

    TSQLSTRU[_area]->ques_num = 0;
    TSQLSTRU[_area]->tsqlstat.put_stat = 0;
    strcpy( TSQLSTRU[_area]->user_msg , "" );
    strcpy( TSQLSTRU[_area]->sql_str , "" );

    return E_OK;
}

/*------------------------------------------------------------------------*
 *  Function Name : int SybSqlSelect  ()                                  *
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
SybSqlSelect ( int _area , char * _statement , char _op_flag , long _offset )
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
        ret = SybSqlPrepare( TSQLSTRU[_area] , prepname, _statement );
        if ( ret != E_OK )
            return ret;

        ret = SybSqlDeclare( TSQLSTRU[_area] , cursname,  _op_flag );
        if ( ret != E_OK )
            return ret;

        alloc_num = get_comma_num( _statement , '*' );
        if ( alloc_num > 0 )
            alloc_num = DEF_ALLOC_NUM;
        else
            alloc_num = get_comma_num( _statement , ',' )+1;
        alloc_num = max( get_comma_num( _statement , '?' )+1 , alloc_num );
        ret = SybSqlAllocate( TSQLSTRU[_area] , descname , alloc_num );
        if ( ret != E_OK )
            return ret;

        ret = SybSqlDescribe( TSQLSTRU[_area] );
        if ( ret != E_OK )
            return ret;

/* 为查询条件赋值 */
        if( TSQLSTRU[_area]->ques_num != 0 )  {
            ret = AllocValueBuf( TSQLSTRU[_area] , TSQLSTRU[_area]->ques_num );
            if ( ret != E_OK )
                return ret;

            for( i=1; i <= TSQLSTRU[_area]->ques_num; i++ )  {
                memcpy( &TSQLSTRU[_area]->tValuepb[i-1] , &TSQLVarPool[ i-1 ] ,sizeof( tSqlValue ) );

                ret = SybSqlPutValue( TSQLSTRU[_area] , (short)i );
                if ( ret != E_OK )
                    return ret;
            } /* end of for */

        } /* end of if ques_num */

        ret = SybSqlOpenCur( TSQLSTRU[_area] );
        if ( ret != E_OK )
            return ret;

        ret = SybSqlDescribe( TSQLSTRU[_area] );
        if ( ret != E_OK )
            return ret;

    } /* end of if fetchstat */

    ret = SybSqlFetchCur( TSQLSTRU[_area] , _offset );
    if ( ret != E_OK )
        return ret;

/* 为查询结果赋值 */
    ret = AllocValueBuf( TSQLSTRU[_area] , TSQLSTRU[_area]->column_num );
    if ( ret != E_OK )
        return ret;

    for( i = 1; i <= TSQLSTRU[_area]->column_num ; i++ ) {
        ret = SybSqlGetValue( TSQLSTRU[_area] , (short)i );
        if ( ret != E_OK )
            return ret;

        sql_value = &TSQLVarPool[ i+(MAX_VAR_NUM/2)-1 ];
        FreeCValue( sql_value );
       
        memcpy( sql_value, &TSQLSTRU[_area]->tValuepb[i-1], sizeof(tSqlValue) );
    }

    FETCH_AREA = _area;
    return E_OK;
}

/*------------------------------------------------------------------------*
 *  Function Name : int SybSqlExec  ()                                    *
 *  Description   : 使用默认工作区0                                       *
 *  Input         :                                                       *
 *                : _statement 查询语句                                   *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/
int 
SybSqlExec ( char * _statement )
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

    ret = SybSqlPrepare( TSQLSTRU[_area] , prepname, _statement );
    if ( ret != E_OK ) {
        SybSqlDBFree( _area );
        return ret;
    }

    alloc_num = get_comma_num( _statement , '?' )+1;
    ret = SybSqlAllocate( TSQLSTRU[_area] , descname , alloc_num );
    if ( ret != E_OK ) {
        SybSqlDBFree( _area );
        return ret;
    }

/* 为查询条件赋值 */
    if( TSQLSTRU[_area]->ques_num != 0 )  {
        ret = AllocValueBuf( TSQLSTRU[_area] , TSQLSTRU[_area]->ques_num );
        if ( ret != E_OK ) {
           SybSqlDBFree( _area );
           return ret;
        }

        for( i=1; i <= TSQLSTRU[_area]->ques_num; i++ )  {
            memcpy( &TSQLSTRU[_area]->tValuepb[i-1] , &TSQLVarPool[ i-1 ] ,sizeof( tSqlValue ) );

            ret = SybSqlPutValue( TSQLSTRU[_area] ,(short)i );
            if ( ret != E_OK ) {
                SybSqlDBFree( _area );
                return ret;
            }
        } /* end of for */

    } /* end of if ques_num */

    ret = SybSqlExecute( TSQLSTRU[_area] );
    SybSqlDBFree( _area );

    if ( ret != E_OK )
        return ret;

/* 20051027 提高效率
    strncpy( str_tmp, _statement, 10 );
    bcl_str_toupper( str_tmp );
    if ( TSQLSTRU[_area]->proc_row == 0 && strstr(str_tmp,"CREATE")== NULL\
                && strstr(str_tmp,"DROP")== NULL)
       return E_NOTFOUND;
*/
    if ( TSQLSTRU[_area]->proc_row == 0 )
       return E_NOTFOUND;

    return E_OK;
}

/* 20051027 支持批量处理insert/update提高效率 */
int
SybSqlBatExec ( int _area , char * _statement )
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
        ret = SybSqlPrepare( TSQLSTRU[_area] , prepname, _statement );
        if ( ret != E_OK ) {
            bclerreg( E_SQL , _FL_ ,"SybSqlPrepare err.");
            TSQLSTRU[_area]->sql_code = 0;
            TSQLSTRU[_area]->user_code = 0;
            return ret;
        }

        ret = SybSqlAllocate( TSQLSTRU[_area] , descname , alloc_num );
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
                ret = SybSqlPutValue( TSQLSTRU[_area] , (short)i );
                if ( ret != E_OK ) {
                    bclerreg( E_SQL , _FL_ ,"SybSqlPutValue err.");
                    TSQLSTRU[_area]->sql_code = 0;
                    TSQLSTRU[_area]->user_code = 0;
                    return ret;
                }
            } /* end of for */

        } /* end of if ques_num */

    } /* end of if prep_stat */
    else
        for( i=1; i <= TSQLSTRU[_area]->ques_num; i++ )  {
            memcpy( &TSQLSTRU[_area]->tValuepb[i-1] , &TSQLVarPool[ i-1 ], \
                    sizeof( tSqlValue ) );
            ret = SybSqlPutValue( TSQLSTRU[_area] , (short)i );
            if ( ret != E_OK ) {
                bclerreg( E_SQL , _FL_ ,"SybSqlPutValue err.");
                TSQLSTRU[_area]->sql_code = 0;
                TSQLSTRU[_area]->user_code = 0;
                return ret;
            }
        } /* end of for */
    ret = SybSqlExecute( TSQLSTRU[_area] );
    TSQLSTRU[_area]->tsqlstat.put_stat = 0;
    TSQLSTRU[_area]->sql_code = 0;
    TSQLSTRU[_area]->user_code = 0;
    if ( ret == E_FAIL ) {
        bclerreg( E_SQL , _FL_ ,"SybSqlExecute err.");
        return ret;
    }

    if ( TSQLSTRU[_area]->proc_row == 0 )
       return E_NOTFOUND;
    return E_OK;
}

/*
**
**      Displays warning messages.
*/
void warning_handler()
{

        if (sqlca.sqlwarn[1] == 'W')
        {
                fprintf(stderr,
                        "\n** Data truncated.\n");
        }

        if (sqlca.sqlwarn[3] == 'W')
        {
                fprintf(stderr,
                        "\n** Insufficient host variables to store results.\n");
        }
        return;
}
