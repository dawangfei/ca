/* #ident "@(#)infdyn.ec,v 1.0 2000/09/25 10:17:26 hdq database sybase dynamtic application interface source"
 *      SCCS IDENTIFICATION
 * copyright (C) 2000, 2000 by hdq.
 * MUST be distributed in source form only.
 */

/*************************************************************************************************
 * File            : sybdyn.ec                                                                   *
 * System Name     : StarRing                                                                    *
 * VERSION         : 1.0                                                                         *
 * LANGUAGE        : C                                                                           *
 * OS & ENVIROMENT : SCO UNIX  5                                                                 *
 *                   SYBASE    11.x                                                              *
 * DESCRIPTION     : this program refer to all of sybase functions about                         *
 *                   dynamtic operation function of database                                     *
 * HISTORY:                                                                                      *
 * MM/DD/YYYY      ADDRESS          PROGRAMMER            DESCRIPTION                            *
 *-----------------------------------------------------------------------------------------------*
 * 09/25/2000      Nanjing          Hdq                   Creation                               *
 * 04/10/2008      BeiJing          Chenff                Modify SybSqlDeclare() Cursor          *  
 *                                                        delete WITH HOLD                       *
 *************************************************************************************************/
                                                                           
#include <arcbcl.h>
#include <arclog.h>

EXEC SQL include sqlca;

/*  Application include file define using different type */

#include <__pladb.h>
#include <arctrc.h>

/* Global variable definition */
extern long FAP_SQLCODE;

/*Function in this program define*/


/*------------------------------------------------------------------------*
 *  Function Name :int SybPutDescData ()                                  *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/
int 
SybPutDescData( _sqlvalue ,_datapb)
tSqlValue *_sqlvalue ;
char *_datapb;
{

    switch ( _sqlvalue->type )   {
        case DFFAPCHAR:
        case DFFAPDTIME:
        case DFFAPBYTES:
        case DFFAPTEXT:
        case DFFAPINTERVAL:
        case DFFAPNCHAR:
        case DFFAPNVCHAR:
             strcpy( _datapb , _sqlvalue->pubdata.c_value );
             break;
        case DFFAPVCHAR:
             memcpy( _datapb , _sqlvalue->pubdata.c_value, _sqlvalue->cb );
             break;
        case DFFAPDATE:
             strcpy( _datapb , _sqlvalue->pubdata.c_value );
             break;

        case DFFAPSMINT:
             sprintf(_datapb ,"%hd",_sqlvalue->pubdata.s_value);
             break;
        case DFFAPINT:
        case DFFAPSERIAL:
             sprintf(_datapb , "%ld", _sqlvalue->pubdata.l_value);
             bcl_rtrim(_datapb);
             bcl_ltrim(_datapb);
             break;
        case DFFAPMONEY:
        case DFFAPDECIMAL:
        case DFFAPFLOAT:
/* 20091016 change by handq %lf -> %.15lf */
             gcvt( _sqlvalue->pubdata.d_value, 16, _datapb );
             bcl_rtrim(_datapb);
             bcl_ltrim(_datapb);
             break;
        /* 关于SQLSMFLOAT有待测试*/ 
        case DFFAPSMFLOAT:
             sprintf(_datapb , "%f", _sqlvalue->pubdata.f_value );
             bcl_rtrim(_datapb);
             bcl_ltrim(_datapb);
             break;
        case DFFAPNULL:
             break;
        default: 
             _datapb = NULL;
             return E_FAIL;
    }

    return E_OK;

}


/*------------------------------------------------------------------------*
 *  Function Name :int SybGetDescData ()                                  *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/
int 
SybGetDescData( _sqlvalue , _datapb, _datatype )
tSqlValue *_sqlvalue ;
char    *_datapb;
int     _datatype;
{
	char errmsg[100];

    switch ( _sqlvalue->type ) {
        case DFFAPCHAR:
        case DFFAPDTIME:
        case DFFAPBYTES:
        case DFFAPTEXT:
        case DFFAPVCHAR:
        case DFFAPINTERVAL:
        case DFFAPNCHAR:
        case DFFAPNVCHAR:
        case DFFAPDATE:
             if( _sqlvalue->pubdata.c_value != NULL ) {
                 free( _sqlvalue->pubdata.c_value );
                 _sqlvalue->pubdata.c_value = NULL;
             }
             break;
    }
    switch ( _datatype )   {
        case DFFAPCHAR:
        case DFFAPBYTES:
        case DFFAPTEXT:
        case DFFAPVCHAR:
        case DFFAPINTERVAL:
        case DFFAPNCHAR:
        case DFFAPNVCHAR:
             if( _sqlvalue->cb > 0 ) {
                 _sqlvalue->pubdata.c_value = calloc( 1, _sqlvalue->cb+1 );
                 if ( _sqlvalue->pubdata.c_value == NULL ) 
                 {
                     bclerreg( E_MESSAGE , _FL_ ,"calloc err.");
                     return E_FAIL;
                 }
                 strncpy(_sqlvalue->pubdata.c_value,_datapb,(_sqlvalue->cb+1));
             }
/*             _sqlvalue->cb = strlen( _datapb ); */
             break;
        case DFFAPDTIME:
             _sqlvalue->pubdata.c_value = calloc( 1, DEF_DTIME_LEN );
             if ( _sqlvalue->pubdata.c_value == NULL ) 
             {
             	 bclerreg( E_MESSAGE , _FL_ ,"calloc err.");
                 return E_FAIL;
             }
             strcpy( _sqlvalue->pubdata.c_value , _datapb );
             break;
        case DFFAPDATE:
             _sqlvalue->pubdata.c_value = calloc( 1, DEF_DATE_LEN );
             if ( _sqlvalue->pubdata.c_value == NULL ) 
             {
             	 bclerreg( E_MESSAGE , _FL_ ,"calloc err.");
                 return E_FAIL;
             }
             strcpy( _sqlvalue->pubdata.c_value , _datapb );
/*             _sqlvalue->cb = strlen( _datapb ); */
             break;
/* 20051014 handq 统一使用l_value */
        case DFFAPSMINT:
             _sqlvalue->pubdata.l_value = (short ) atoi( _datapb );
             break;
        case DFFAPINT:
        case DFFAPSERIAL:
             _sqlvalue->pubdata.l_value = (long ) atol(_datapb) ;
             break;
/* 20051014 handq 统一使用d_value */
        case DFFAPMONEY:
        case DFFAPDECIMAL:
        case DFFAPFLOAT:
             _sqlvalue->pubdata.d_value =  ( double ) bcl_myatof(_datapb);
             break;
        /* 关于SQLSMFLOAT有待测试*/ 
        case DFFAPSMFLOAT:
             _sqlvalue->pubdata.d_value = ( float ) bcl_myatof(_datapb);
             break;
        case DFFAPNULL:
             break;
        default: 
        	 memset( errmsg, 0x00, sizeof( errmsg ) );
        	 sprintf( errmsg, "datatype=[%d] error", _datatype );
        	 bclerreg( E_MESSAGE , _FL_ ,errmsg );
        	 
        	 memset( errmsg, 0x00, sizeof( errmsg ) );
        	 sprintf( errmsg, "_datapb=[%s] error", _datapb );
        	 bclerreg( E_MESSAGE , _FL_ ,errmsg );
        	 
             return E_FAIL;
    }

    return E_OK;
}


/*------------------------------------------------------------------------*
 *  Function Name :int SybSqlAllocate()                                   *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/

int 
SybSqlAllocate( tSqlStru *_sqlstru,const char * _descname, int  _alloc_count)
{
    EXEC SQL BEGIN DECLARE SECTION;
         CS_INT           h_alloc_count;
         CS_CHAR          h_desc_id[20];
    EXEC SQL END DECLARE SECTION;
    char sql_str[SQLSTRLEN];

    h_alloc_count=_alloc_count;

    strcpy( _sqlstru->desc_id, _descname );
    sprintf(h_desc_id,"%sout",_sqlstru->desc_id);
    trc_db_rec( "", sqlca.sqlcode );
    EXEC SQL allocate descriptor :h_desc_id with max :h_alloc_count;
    sprintf( sql_str, "ALLOCATE DESCRIPTOR :%s WITH MAX :%ld", \
                       h_desc_id, h_alloc_count );
    trc_db_rec( sql_str, sqlca.sqlcode );

    sprintf(h_desc_id,"%sin",_sqlstru->desc_id);
    EXEC SQL allocate descriptor :h_desc_id with max :h_alloc_count;
    sprintf( sql_str, "ALLOCATE DESCRIPTOR :%s WITH MAX :%ld", \
                       h_desc_id, h_alloc_count );
    trc_db_rec( sql_str, sqlca.sqlcode );

    _sqlstru->sql_code=sqlca.sqlcode;
    if( _sqlstru->sql_code == 0 )
        _sqlstru->tsqlstat.alloc_stat =1;
    
    if(_sqlstru->sql_code || _sqlstru->user_code )  {
        strcpy( _sqlstru->user_msg , "Sybase SqlAllocateOut error" );
        DB_ERR( _sqlstru )
    }
    else  
        return E_OK;
}

/*------------------------------------------------------------------------*
 *  Function Name : int SybSqlPrepare  ()                                 *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/
int 
SybSqlPrepare ( tSqlStru *_sqlstru,const char *_prepare_name,const char *_sql_str )
{
    EXEC SQL BEGIN DECLARE SECTION;
         CS_CHAR          *h_sql_str;
         CS_CHAR          h_prep_id[20];
    EXEC SQL END DECLARE SECTION;
    char sql_str[SQLSTRLEN];
    
    if ( _sqlstru->tsqlstat.prep_stat == 0 )  {
        h_sql_str = _sql_str;
        strcpy(_sqlstru->sql_str,_sql_str );
        strcpy(_sqlstru->prep_id,_prepare_name);
        strcpy(h_prep_id,_sqlstru->prep_id);
        _sqlstru->ques_num = get_comma_num( _sql_str, '?' );

        trc_db_rec( "", sqlca.sqlcode );
        EXEC SQL prepare :h_prep_id from :h_sql_str;
        snprintf( sql_str, SQLSTRLEN, "PREPARE :%s FROM :%s", h_prep_id, h_sql_str );
        trc_db_rec( sql_str, sqlca.sqlcode );

        _sqlstru->sql_code=sqlca.sqlcode;
        if( _sqlstru->sql_code == 0 )
            _sqlstru->tsqlstat.prep_stat =1;
    }
    else 
        _sqlstru->user_code = E_MULTI_PREP;

    if(_sqlstru->sql_code || _sqlstru->user_code )  {
        strcpy( _sqlstru->user_msg , "Sybase SqlPrepare error" );
        DB_ERR( _sqlstru )
    }
    else  
        return E_OK;
}

/*------------------------------------------------------------------------*
 *  Function Name : int SybSqlDeclare  ()                                 *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/
int 
SybSqlDeclare  ( tSqlStru *_sqlstru ,const char * _cursor_name , char _op_flag)
{
    EXEC SQL BEGIN DECLARE SECTION;
         CS_CHAR          h_curs_id[20];
         CS_CHAR          h_curs_id_H[20];
         CS_CHAR          h_curs_id_scroll[20];
         CS_CHAR          h_prep_id[20];
    EXEC SQL END DECLARE SECTION;
    char sql_str[SQLSTRLEN];
    
    EXEC SQL WHENEVER SQLERROR continue;
    EXEC SQL WHENEVER SQLWARNING continue;
    EXEC SQL WHENEVER NOT FOUND continue;

    strcpy(_sqlstru->curs_id,_cursor_name);
    strcpy(h_curs_id,_sqlstru->curs_id);
    strcpy(h_curs_id_scroll,_sqlstru->curs_id);
    _sqlstru->op_flag = _op_flag;
    if( _sqlstru->tsqlstat.prep_stat )  {
        strcpy(h_prep_id,_sqlstru->prep_id);
        trc_db_rec( "", sqlca.sqlcode );
        if ( _sqlstru->op_flag == 'H' || _sqlstru->op_flag == 'h' ) {
            /* chenff 090409 delete with hold */
            EXEC SQL DECLARW :h_curs_id_H SCROLL CURSOR WITH HOLD for :h_prep_id;
/*
            EXEC SQL DECLARE :h_curs_id_H CURSOR for :h_prep_id;
*/
            sprintf( sql_str, "DECLARE :%s SCROLL CURSOR WITH HOLD FOR %s", h_curs_id, h_prep_id );
        }
        else if ( _sqlstru->op_flag ) {
            EXEC SQL DECLARE :h_curs_id_scroll SCROLL CURSOR for :h_prep_id;
            sprintf( sql_str, "DECLARE :%s SCROLL CURSOR FOR %s", h_curs_id, h_prep_id );
        }
        else {
            EXEC SQL DECLARE :h_curs_id CURSOR for :h_prep_id;
            sprintf( sql_str, "DECLARE :%s CURSOR FOR %s", h_curs_id, h_prep_id );
        }
        trc_db_rec( sql_str, sqlca.sqlcode );
        _sqlstru->sql_code=sqlca.sqlcode;
        if(_sqlstru->sql_code == 0 )
            _sqlstru->tsqlstat.cur_allocstat = 1;
    }
    else 
        _sqlstru->user_code=E_PREP_ID;

    if(_sqlstru->sql_code || _sqlstru->user_code ) {
        strcpy( _sqlstru->user_msg , "Sybase SqlDeclare error" );
        DB_ERR( _sqlstru )
    }
    else  
        return E_OK;
}

/*------------------------------------------------------------------------*
 *  Function Name : int SybSqlDescribe ( )                                * 
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/ 
int 
SybSqlDescribe ( tSqlStru *_sqlstru )
{
    EXEC SQL BEGIN DECLARE SECTION;
         CS_CHAR          h_curs_id[20];
         CS_CHAR          h_prep_id[20];
         CS_CHAR          h_desc_id[20];
         CS_INT           h_column_num;
    EXEC SQL END DECLARE SECTION;
    char sql_str[SQLSTRLEN];
 
    memset(h_curs_id,0x00,sizeof(h_curs_id));
    strcpy(h_curs_id,_sqlstru->curs_id);
    memset(h_desc_id,0x00,sizeof(h_desc_id));
    strcpy(h_desc_id,_sqlstru->desc_id);
    
    if( _sqlstru->tsqlstat.cur_openstat )  {
        memset(h_prep_id,0x00,sizeof(h_prep_id));
        strcpy(h_prep_id,_sqlstru->prep_id);
        strcat( h_desc_id , "out" );
        trc_db_rec( "", sqlca.sqlcode );
        EXEC SQL describe output :h_prep_id using sql descriptor :h_desc_id;
        sprintf( sql_str, "DESCRIBE OUTPUT :%s USING SQL DESCRIPTOR :%s", \
                                            h_prep_id, h_desc_id );
        trc_db_rec( sql_str, sqlca.sqlcode );
        _sqlstru->sql_code =sqlca.sqlcode;
        if( !( _sqlstru->sql_code ) ) {
            EXEC SQL get descriptor :h_desc_id :h_column_num = COUNT ;
            sprintf( sql_str, "GET DESCRIPTOR :%s :%ld = COUNT", \
                              h_desc_id, h_column_num );
            trc_db_rec( sql_str, sqlca.sqlcode );
            _sqlstru->column_num = h_column_num;
        }
    } /* end of if cur_openstat */
    else 
        if( _sqlstru->tsqlstat.cur_allocstat ) {
            memset(h_prep_id,0x00,sizeof(h_prep_id));
            strcpy(h_prep_id,_sqlstru->prep_id);
            strcat( h_desc_id , "in" );
            trc_db_rec( "", sqlca.sqlcode );
            EXEC SQL describe input :h_prep_id using sql descriptor :h_desc_id;
            sprintf( sql_str, "DESCRIBE INPUT :%s USING SQL DESCRIPTOR :%s", \
                                            h_prep_id, h_desc_id );
            trc_db_rec( sql_str, sqlca.sqlcode );
            _sqlstru->sql_code =sqlca.sqlcode;
        }
        else
            _sqlstru->user_code=E_PREP_ID;
    
    if(_sqlstru->sql_code || _sqlstru->user_code ) {
        strcpy( _sqlstru->user_msg , "Sybase SqlDescribe error" );
        DB_ERR( _sqlstru )
    }
    else 
        return E_OK;
}


/*------------------------------------------------------------------------*
 *  Function Name :int SybSqlExecute()                                    *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/
int 
SybSqlExecute  ( tSqlStru *_sqlstru )
{
    EXEC SQL BEGIN DECLARE SECTION;
         CS_CHAR      h_desc_id[20];
         CS_CHAR      h_prep_id[20];
    EXEC SQL END DECLARE SECTION;
    char sql_str[SQLSTRLEN];

    sprintf(h_desc_id,"%sin",_sqlstru->desc_id);
    strcpy(h_prep_id,_sqlstru->prep_id);

    if( _sqlstru->tsqlstat.prep_stat )   
        if( _sqlstru->ques_num != 0 )
            if( _sqlstru->tsqlstat.alloc_stat )  
                if(_sqlstru->ques_num==( short )_sqlstru->tsqlstat.put_stat ) {
                    trc_db_rec( "", sqlca.sqlcode );
                    EXEC SQL execute :h_prep_id using sql descriptor :h_desc_id;
                    sprintf( sql_str, "EXECUTE :%s USING SQL DESCRIPTOR :%s", \
                                      h_prep_id, h_desc_id );
                    trc_db_rec( sql_str, sqlca.sqlcode );
                    _sqlstru->sql_code=sqlca.sqlcode;
                    _sqlstru->proc_row=sqlca.sqlerrd[2];
                }
                else 
                    _sqlstru->user_code = E_PUT_NOMATCH;
            else
                _sqlstru->user_code = E_EXE_NOALLOC;
        else   {
        /*这段程序是专为无待定参数的update ,delete,insert 的程序设计的*/
            trc_db_rec( "", sqlca.sqlcode );
            EXEC SQL execute :h_prep_id ;
            sprintf( sql_str, "EXECUTE %s", h_prep_id );
            trc_db_rec( sql_str, sqlca.sqlcode );
            _sqlstru->sql_code=sqlca.sqlcode;
            _sqlstru->proc_row=sqlca.sqlerrd[2];
        } /*end of else of ques_num*/
    else 
        _sqlstru->user_code = E_PREP_ID;

    if(_sqlstru->sql_code || _sqlstru->user_code ) {
        strcpy( _sqlstru->user_msg , "Sybase SqlExecute error" );
        DB_ERR( _sqlstru )
    }
    else 
        return E_OK;
}

/*------------------------------------------------------------------------*
 *  Function Name : int SybSqlOpenCur  ()                                 *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/
int
SybSqlOpenCur  ( tSqlStru *_sqlstru )
{
    EXEC SQL BEGIN DECLARE SECTION;
         CS_CHAR     h_curs_id[20];
         CS_CHAR     h_desc_id[20];
    EXEC SQL END DECLARE SECTION;
    char sql_str[SQLSTRLEN];

    if( _sqlstru->tsqlstat.cur_allocstat )   
    if ( _sqlstru->tsqlstat.alloc_stat )   {
            memset(h_curs_id,0x00,sizeof(h_curs_id));
            strcpy(h_curs_id,_sqlstru->curs_id);
            memset(h_desc_id,0x00,sizeof(h_desc_id));
            sprintf(h_desc_id,"%sin",_sqlstru->desc_id);
            if( _sqlstru->ques_num != 0)  
                if(_sqlstru->ques_num == (short)_sqlstru->tsqlstat.put_stat)  {
                    trc_db_rec( "", sqlca.sqlcode );
                    EXEC SQL open :h_curs_id using sql descriptor :h_desc_id;
                    sprintf( sql_str, "OPEN :%s USING SQL DESCRIPTOR %s", \
                                       h_curs_id, h_desc_id );
                    trc_db_rec( sql_str, sqlca.sqlcode );
                    _sqlstru->sql_code = sqlca.sqlcode;
                    if ( !(_sqlstru->sql_code) )
                        _sqlstru->tsqlstat.cur_openstat = 1;
                }
                else 
                    _sqlstru->user_code = E_PUT_NOMATCH;
            else   {
                trc_db_rec( "", sqlca.sqlcode );
                EXEC SQL  open :h_curs_id; 
                sprintf( sql_str, "OPEN %s", h_curs_id );
                trc_db_rec( sql_str, sqlca.sqlcode );
                _sqlstru->sql_code = sqlca.sqlcode;
                if ( !(_sqlstru->sql_code) )
                    _sqlstru->tsqlstat.cur_openstat = 1;
            } /* end of else */
    } /* end of alloc_stat */
    else 
        _sqlstru->user_code = E_CUR_NOALLOC;
    else 
        _sqlstru->user_code=E_CURS_ID;
    
    if(_sqlstru->sql_code || _sqlstru->user_code ) {
        strcpy( _sqlstru->user_msg , "Sybase SqlOpenCur error" );
        DB_ERR( _sqlstru )
    }
    else 
        return E_OK;
}

/*------------------------------------------------------------------------*
 *  Function Name : int SybSqlFetchCur ()                                 *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/
int 
SybSqlFetchCur ( tSqlStru *_sqlstru , long _off_set )
{
    
    EXEC SQL BEGIN DECLARE SECTION;
         CS_CHAR    h_desc_id[20];
         CS_CHAR    h_curs_id[20];
         CS_INT     off_set;
    EXEC SQL END DECLARE SECTION;
    int i;
    char sql_str[SQLSTRLEN];

    memset(h_desc_id,0x00,sizeof(h_desc_id));
    memset(h_curs_id,0x00,sizeof(h_curs_id));
    sprintf(h_desc_id,"%sout",_sqlstru->desc_id);
    strcpy(h_curs_id,_sqlstru->curs_id);
    _sqlstru->offset = _off_set;
    off_set = _off_set;
    if ( _sqlstru->tsqlstat.cur_openstat )  {
        trc_db_rec( "", sqlca.sqlcode );
        switch ( _sqlstru->op_flag ) {
           case 'P':
           case 'p':
/*
             EXEC SQL fetch PREVIOUS :h_curs_id using sql descriptor :h_desc_id;
*/
             if( _sqlstru->proc_row == 1 ) {
                 sqlca.sqlcode = 100;
                 break;
             }
             SybSqlCloseCur( _sqlstru );
             SybSqlOpenCur( _sqlstru );
             for( i=0; i < _sqlstru->proc_row - 1 ; i++ )
                 EXEC SQL fetch :h_curs_id into sql descriptor :h_desc_id;
             if( sqlca.sqlcode == 0 )
                 _sqlstru->proc_row--;
             break;
           case 'C':
           case 'c':
/*
             EXEC SQL fetch CURRENT :h_curs_id using sql descriptor :h_desc_id;
*/
             if( _sqlstru->proc_row == 0 )
                 _sqlstru->proc_row = 1;
             SybSqlCloseCur( _sqlstru );
             SybSqlOpenCur( _sqlstru );
             for( i=0; i < _sqlstru->proc_row ; i++ )
                 EXEC SQL fetch :h_curs_id into sql descriptor :h_desc_id;
             break;
           case 'F':
           case 'f':
/*
             EXEC SQL fetch FIRST :h_curs_id using sql descriptor :h_desc_id;
*/
             _sqlstru->proc_row = 1;
             SybSqlCloseCur( _sqlstru );
             SybSqlOpenCur( _sqlstru );
             for( i=0; i < _sqlstru->proc_row ; i++ )
                 EXEC SQL fetch :h_curs_id into sql descriptor :h_desc_id;
             break;
           case 'L':
           case 'l':
/*
             EXEC SQL fetch LAST :h_curs_id using sql descriptor :h_desc_id;
*/
             while( sqlca.sqlcode == 0 ) {
                 EXEC SQL fetch :h_curs_id into sql descriptor :h_desc_id;
                 _sqlstru->proc_row++;
             }
             _sqlstru->proc_row--;
             SybSqlCloseCur( _sqlstru );
             SybSqlOpenCur( _sqlstru );
             for( i=0; i < _sqlstru->proc_row ; i++ )
                 EXEC SQL fetch :h_curs_id into sql descriptor :h_desc_id;
             break;
/*
           case 'R':
           case 'r':
             EXEC SQL fetch RELATIVE $off_set :h_curs_id using sql descriptor :h_desc_id;
             break;
           case 'A':
           case 'a':
             EXEC SQL fetch ABSOLUTE $off_set :h_curs_id using sql descriptor :h_desc_id;
             break;
*/
           default:
             
             EXEC SQL fetch :h_curs_id into sql descriptor :h_desc_id;
             if( sqlca.sqlcode == 0 )
                 _sqlstru->proc_row++;
             break;
        } /* end of switch */

        sprintf( sql_str, "FETCH :%s INTO SQL DESCRIPTOR :%s", h_curs_id, h_desc_id );
        trc_db_rec( sql_str, sqlca.sqlcode );
        _sqlstru->sql_code = sqlca.sqlcode;

/* 此处的标志有待商榷*/
        if ( (_sqlstru->sql_code == 0) || (_sqlstru->sql_code == 100)) {
            _sqlstru->tsqlstat.cur_fetchstat = 1;
        }
        if ( _sqlstru->sql_code == 100 )
            _sqlstru->tsqlstat.fetchend = 1;
    }
    else
        _sqlstru->user_code=E_CUR_NOOPEN;
    
    if((_sqlstru->sql_code || _sqlstru->user_code ) \
         && ( _sqlstru->sql_code != 100 ) ) {
        strcpy( _sqlstru->user_msg , "Sybase SqlFetchCur error" );
        DB_ERR( _sqlstru )
    }
    else {
       if ( _sqlstru->sql_code == 100 ) 
          return E_NOTFOUND;
       else
          return E_OK;
    }
}


/*------------------------------------------------------------------------*
 *  Function Name : int SybSqlCloseCur ()                                 *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/
int 
SybSqlCloseCur ( tSqlStru *_sqlstru )
{
    
    EXEC SQL BEGIN DECLARE SECTION;
         CS_CHAR   h_curs_id[20];
    EXEC SQL END DECLARE SECTION;
    char sql_str[SQLSTRLEN];

    if( _sqlstru->tsqlstat.cur_openstat == 1 )   {
        strcpy(h_curs_id,_sqlstru->curs_id);
        
        trc_db_rec( "", sqlca.sqlcode );
        EXEC SQL close :h_curs_id;
        sprintf( sql_str, "CLOSE %s", h_curs_id );
        trc_db_rec( sql_str, sqlca.sqlcode );

        _sqlstru->sql_code=sqlca.sqlcode;
        if ( !( _sqlstru->sql_code ) )  
            _sqlstru->tsqlstat.cur_openstat = 0;
    }
    else 
        _sqlstru->user_code = E_CUR_UNCLOSED;
    
    if(_sqlstru->sql_code || _sqlstru->user_code ) {
        strcpy( _sqlstru->user_msg , "Sybase SqlCloseCur error" );
        DB_ERR( _sqlstru )
    }
    else 
        return E_OK;
}

/*------------------------------------------------------------------------*
 *  Function Name : int SybSqlFreeCur  ()                                 *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/
int 
SybSqlFreeCur  ( tSqlStru *_sqlstru )
{
    
    EXEC SQL BEGIN DECLARE SECTION;
         CS_CHAR   h_curs_id[20];
    EXEC SQL END DECLARE SECTION;

    if( _sqlstru->tsqlstat.cur_openstat == 0 )  {
        strcpy(h_curs_id,_sqlstru->curs_id);
/*
        EXEC SQL deallocate cursor :h_curs_id;
*/
        _sqlstru->sql_code=sqlca.sqlcode;
    }
    else 
        _sqlstru->user_code=E_CUR_NOCLOSE;
    
    if( (_sqlstru->user_code == 0)  && (_sqlstru->sql_code == 0) )
        memset(_sqlstru->curs_id,0x00,sizeof(_sqlstru->curs_id));
    
    if(_sqlstru->sql_code || _sqlstru->user_code ) {
        strcpy( _sqlstru->user_msg , "Sybase SqlFreeCur error" );
        DB_ERR( _sqlstru )
    }
    else 
        return E_OK;

}

/*------------------------------------------------------------------------*
 *  Function Name :int SybSqlDealloc  ()                                  *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/
int 
SybSqlDealloc  ( tSqlStru *_sqlstru )
{
    
    EXEC SQL BEGIN DECLARE SECTION;
         CS_CHAR   h_desc_id[20];
    EXEC SQL END DECLARE SECTION;
    char sql_str[SQLSTRLEN];

    if( _sqlstru->tsqlstat.alloc_stat == 1 )  {
        sprintf(h_desc_id,"%sin",_sqlstru->desc_id);
        
        trc_db_rec( "", sqlca.sqlcode );
        EXEC SQL deallocate descriptor :h_desc_id;
        sprintf( sql_str, "DEALLOCATE DESCRIPTOR %s", h_desc_id );
        trc_db_rec( sql_str, sqlca.sqlcode );

        sprintf(h_desc_id,"%sout",_sqlstru->desc_id);
        
        EXEC SQL deallocate descriptor :h_desc_id;
        sprintf( sql_str, "DEALLOCATE DESCRIPTOR %s", h_desc_id );
        trc_db_rec( sql_str, sqlca.sqlcode );

        _sqlstru->sql_code=sqlca.sqlcode;
        if ( !( _sqlstru->sql_code ) ) 
            _sqlstru->tsqlstat.alloc_stat = 0;
    }
    else 
        _sqlstru->user_code=E_DESC_UNALLOCATED;
    
    if( (_sqlstru->user_code == 0)  && (_sqlstru->sql_code == 0) )
        memset(_sqlstru->desc_id,0x00,sizeof(_sqlstru->desc_id));
    
    if(_sqlstru->sql_code || _sqlstru->user_code ) {
        strcpy( _sqlstru->user_msg , "Sybase SqlDealloc error" );
        DB_ERR( _sqlstru )
    }
    else 
        return E_OK;
}



/*------------------------------------------------------------------------*
 *  Function Name :int SybSqlGetValue ()                                  *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/

int 
SybSqlGetValue ( tSqlStru *_sqlstru, short _getvalue_serial )
{
    
    EXEC SQL BEGIN DECLARE SECTION;

         CS_CHAR           h_desc_id[20];
         CS_CHAR           h_prep_id[20];
         CS_SMALLINT       h_get_serial;
    
         CS_CHAR           h_get_name[20];
         CS_SMALLINT       h_get_type=0;
         CS_INT            h_get_length=0;
         CS_CHAR           * h_get_data;
         CS_INT            ret;
    
    EXEC SQL END DECLARE SECTION;
     
    h_get_serial = _getvalue_serial;
    if( h_get_serial <= _sqlstru->column_num )   {
        if( _sqlstru->tsqlstat.cur_fetchstat )  {
            memset(h_desc_id,0x00,sizeof(h_desc_id));
            memset(h_prep_id,0x00,sizeof(h_prep_id));
            sprintf(h_desc_id,"%sout",_sqlstru->desc_id);
            strcpy(h_prep_id,_sqlstru->prep_id);
            
            EXEC SQL get descriptor :h_desc_id VALUE :h_get_serial
                                                     :h_get_name = NAME,
                                                     :h_get_type = TYPE,
                                                     :h_get_length = LENGTH;

            bcl_trimall( h_get_name );
/*  +MAX_DOUBLE_STR_LEN INT LENGTH is 4, but need at least 10 char buffer */
            h_get_data = calloc( 1, h_get_length+MAX_DOUBLE_STR_LEN );
            if( h_get_data == NULL ) {
                bclerreg( E_ALLOC, _FL_, h_get_length+MAX_DOUBLE_STR_LEN );
                return E_FAIL;
            }
            
            EXEC SQL get descriptor :h_desc_id VALUE :h_get_serial
                                                     :h_get_data = DATA;

            bcl_rtrim(h_get_data);
#ifdef DEBUGDB
faptrace( DB_TRC_FILE , (u_char *)"" , 0 , \
       "GET ROW[%d] NAME[%s] OUT_VAL[%d],TYPE[%d],LEN[%d],DATA[%.64s][%d]\n" , \
       _sqlstru->proc_row, h_get_name, h_get_serial, h_get_type , h_get_length,\
       h_get_data ,_sqlstru->column_num);
#endif

/* (: use old type free space :) */
            _sqlstru-> tValuepb[h_get_serial-1].cb     = h_get_length;
            ret = SybGetDescData ( &(_sqlstru->tValuepb[h_get_serial-1]),h_get_data,h_get_type);
            _sqlstru-> tValuepb[h_get_serial-1].type   = h_get_type;
            strcpy( _sqlstru-> tValuepb[h_get_serial-1].name , h_get_name );
            free( h_get_data );
            if( ret != E_OK )   {
                _sqlstru->user_code = E_NODEF_TYPE;
                APP_ERR( E_NODEF_TYPE )
            }

        } /* end of if prep_id */
        else
            _sqlstru->user_code = E_CUR_NOFETCH ;
    }
    else 
        _sqlstru->user_code =E_GETSERIAL_ERR;
    
    if(_sqlstru->sql_code || _sqlstru->user_code ) {
        strcpy( _sqlstru->user_msg , "Sybase SqlGetValue error" );
        DB_ERR( _sqlstru )
    }
    else 
        return E_OK;
}


/*------------------------------------------------------------------------*
 *  Function Name : int SybSqlPutValue ()                                 *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/
int 
SybSqlPutValue ( tSqlStru *_sqlstru, short _putvalue_serial)
{
    
    EXEC SQL BEGIN DECLARE SECTION;

         CS_CHAR          h_curs_id[20];
         CS_CHAR          h_prep_id[20];
         CS_CHAR          h_desc_id[20];
         CS_SMALLINT      h_ques_num;  
         CS_INT           ret;  
         CS_FLOAT         d_ret;
         CS_SMALLINT      i;
         CS_SMALLINT      h_t_type;
         CS_SMALLINT      h_t_length;
         CS_CHAR        * h_t_data;
    
    EXEC SQL END DECLARE SECTION;

    memset(h_curs_id,0x00,sizeof(h_curs_id));
    strcpy(h_curs_id,_sqlstru->curs_id);
    memset(h_desc_id,0x00,sizeof(h_desc_id));
    sprintf(h_desc_id,"%sin",_sqlstru->desc_id);
    
    if( _sqlstru->tsqlstat.prep_stat )  {
        if( _sqlstru->tsqlstat.alloc_stat )  {
            memset(h_prep_id,0x00,sizeof(h_prep_id));
            strcpy(h_prep_id,_sqlstru->prep_id);
            h_ques_num=_sqlstru->ques_num;
            i=_putvalue_serial;
            if( h_ques_num >= i )  {
/* 1234567890 will change to h_ques_num in make process */
                
                EXEC SQL set descriptor :h_desc_id  COUNT = 1234567890;

                h_t_type   = _sqlstru->tValuepb[i-1].type;
                h_t_length = _sqlstru->tValuepb[i-1].cb;
                h_t_data = calloc( 1, h_t_length+MAX_DOUBLE_STR_LEN);
                if ( h_t_data == NULL ) {
                    bclerreg( E_ALLOC, _FL_, h_t_length+MAX_DOUBLE_STR_LEN );
                    return E_FAIL;
                }
                ret=SybPutDescData(&(_sqlstru->tValuepb[i-1]),h_t_data);
                if( ret != E_OK )  {
                    _sqlstru->user_code = E_ILLLEGAL_TYPE;
                    free( h_t_data );
                    APP_ERR( E_ILLLEGAL_TYPE )
                }

                switch ( h_t_type ) {
                    case DFFAPSMINT:
                            ret = atoi( h_t_data );
                            
                            EXEC SQL set descriptor :h_desc_id VALUE    :i 
                                 TYPE   = :h_t_type,
                                 LENGTH = :h_t_length,
                                 DATA   = :ret;

                            break;
                    case DFFAPINT:
                            ret = atol( h_t_data );
                            
                            EXEC SQL set descriptor :h_desc_id VALUE    :i 
                                  TYPE   = :h_t_type,
                                  LENGTH = :h_t_length,
                                  DATA   = :ret;

                            break;
                    case DFFAPFLOAT:
                            d_ret = bcl_myatof( h_t_data );
                            
                            EXEC SQL set descriptor :h_desc_id VALUE    :i 
                                  TYPE   = :h_t_type,
                                  LENGTH = :h_t_length,
                                  DATA   = :d_ret;

                            break;
                    default:
                            
                            EXEC SQL set descriptor :h_desc_id VALUE    :i 
                                  TYPE   = :h_t_type,
                                  LENGTH = :h_t_length,
                                  DATA   = :h_t_data;
                }

#ifdef DEBUGDB
faptrace( DB_TRC_FILE , (u_char *)"" , 0 , \
       "PUT IN_VAL[%d],TYPE[%d],LEN[%d],DATA[%.128s] DATA=[%s]\n" , \
       i,h_t_type , h_t_length , h_t_data, h_t_data );
#endif

                free( h_t_data );
                _sqlstru->sql_code =sqlca.sqlcode;
                if( !(_sqlstru->sql_code) )
                    _sqlstru->tsqlstat.put_stat++;
                    /* 
                       这种机制有一个缺点就是若重复PUT同一个值虽然
                       ques_descstat++但是并不表示已经PUT了所有的条件
                    */
            }
            else 
                _sqlstru->user_code=E_PUTERROR;
        }  /* end of alloc_stat */   
        else 
            _sqlstru->user_code = E_PUT_NOALLOC;
    } /* end of if prep_stat */
    else 
        _sqlstru->user_code=E_PREP_ID;
    
    if(_sqlstru->sql_code || _sqlstru->user_code ) {
        strcpy( _sqlstru->user_msg , "Sybase SqlPutValue error" );
        DB_ERR( _sqlstru )
    }
    else 
        return E_OK;
}

/*------------------------------------------------------------------------*
 *  Function Name :int SybSqlFreePrep ()                                  *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/

int 
SybSqlFreePrep( tSqlStru *_sqlstru )
{
    
    EXEC SQL BEGIN DECLARE SECTION;
         CS_CHAR h_prep_id[20];
    EXEC SQL END DECLARE SECTION;
    char sql_str[SQLSTRLEN];

    if( _sqlstru->tsqlstat.prep_stat )  {
        strcpy(h_prep_id,_sqlstru->prep_id);
        
        trc_db_rec( "", sqlca.sqlcode );
        EXEC SQL deallocate prepare :h_prep_id;
        sprintf( sql_str, "DEALLOCATE PREPARE :%s", h_prep_id );
        trc_db_rec( sql_str, sqlca.sqlcode );

        _sqlstru->sql_code=sqlca.sqlcode;
        if ( _sqlstru->sql_code == 0 )
            _sqlstru->tsqlstat.prep_stat = 0;
    }
    else 
        _sqlstru->user_code=E_PREP_ID;
    
    if( (_sqlstru->user_code == 0)  && (_sqlstru->sql_code == 0) )
        memset(_sqlstru->curs_id,0x00,sizeof(_sqlstru->prep_id));
      
    if(_sqlstru->sql_code || _sqlstru->user_code ) {
        strcpy( _sqlstru->user_msg , "Sybase SqlFreePrep error" );
        return E_FAIL;
    }
    else 
        return E_OK;
}
