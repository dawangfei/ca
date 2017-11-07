/* #ident "@(#)infdyn.ec,v 1.0 2000/05/30 10:17:26 hdq database informix dynamtic application interface source"
 *      SCCS IDENTIFICATION
 * copyright (C) 2000, 2000 by hdq.
 * MUST be distributed in source form only.
 */

/*****************************************************************************
 * File            : infdyn.ec                                               *
 * System Name     : StarRing                                                *
 * VERSION         : 1.0                                                     *
 * LANGUAGE        : C                                                       *
 * OS & ENVIROMENT : SCO UNIX  5                                             * 
 *                   INFORMIX  7.23UC13                                      *
 * DESCRIPTION     : this program refer to all of informix functions about   * 
 *                   dynamtic operation function of database                 *
 * HISTORY:                                                                  *
 * MM/DD/YYYY      ADDRESS          PROGRAMMER            DESCRIPTION        *
 *---------------------------------------------------------------------------*
 * 05/27/2000      Nanjing          Hdq                    Creation          *
 *                                                                           *
 *****************************************************************************/

#include <arcbcl.h>
#include <arclog.h>

EXEC SQL include sqlca.h;
EXEC SQL include sqlda.h;
EXEC SQL include sqltypes.h;

/* 20050426 for SQ_EXECPROC */
EXEC SQL include sqlstype.h;

/*  Application include file define using different type */

#include <__pladb.h>
#include <arctrc.h>

/* Global variable definition */
extern long FAP_SQLCODE;

extern int get_comma_num _(( char * , char ));

/*Function in this program define*/

void
InfDynGetErr( tSqlStru *_sqlstru )
{
    if ( _sqlstru == NULL )
        return;

    _sqlstru->sql_code = SQLCODE;

    _sqlstru->isam_code = sqlca.sqlerrd[1];

    _sqlstru->proc_row=sqlca.sqlerrd[2];
}

/*------------------------------------------------------------------------*
 *  Function Name :int InfPutDescData ()                                  *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/
int 
InfPutDescData( _sqlvalue ,_datapb)
tSqlValue *_sqlvalue ;
EXEC SQL BEGIN DECLARE SECTION;
     PARAMETER char *_datapb;
EXEC SQL END DECLARE SECTION;
{
    EXEC SQL BEGIN DECLARE SECTION;
             date  tmp_date;
    EXEC SQL END DECLARE SECTION;
    int ret;


    switch ( _sqlvalue->type )   {
        case DFFAPCHAR:
        case DFFAPDTIME:
        case DFFAPBYTES:
        case DFFAPTEXT:
        case DFFAPINTERVAL:
        case DFFAPNCHAR:
        case DFFAPNVCHAR:
        case DFFAPVCHAR2:
             strcpy( _datapb , _sqlvalue->pubdata.c_value );
             break;
        case DFFAPVCHAR:
             memcpy( _datapb , _sqlvalue->pubdata.c_value, _sqlvalue->cb );
             break;
        case DFFAPDATE:
             ret = rdefmtdate( &tmp_date, DATE_FMT, _sqlvalue->pubdata.c_value);
             if( !ret )
                 rdatestr( tmp_date, _datapb );
             else
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
             gcvt( _sqlvalue->pubdata.d_value, 16, _datapb );
             bcl_rtrim(_datapb);
             bcl_ltrim(_datapb);
             break;
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
 *  Function Name :int InfGetDescData ()                                  *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/
int 
InfGetDescData( _sqlvalue , _datapb, _datatype )
tSqlValue *_sqlvalue ;
EXEC SQL BEGIN DECLARE SECTION;
     PARAMETER char    *_datapb;
     PARAMETER int     _datatype;
EXEC SQL END DECLARE SECTION;
{
    EXEC SQL BEGIN DECLARE SECTION;
             date tmp_date;
    EXEC SQL END DECLARE SECTION;


    switch ( _sqlvalue->type ) {
        case DFFAPCHAR:
        case DFFAPDTIME:
        case DFFAPBYTES:
        case DFFAPTEXT:
        case DFFAPVCHAR:
        case DFFAPVCHAR2:
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
        case DFFAPDTIME:
        case DFFAPBYTES:
        case DFFAPTEXT:
        case DFFAPVCHAR:
        case DFFAPVCHAR2:
        case DFFAPINTERVAL:
        case DFFAPNCHAR:
        case DFFAPNVCHAR:
             if( _sqlvalue->cb > 0 ) {
                 _sqlvalue->pubdata.c_value = calloc( 1, _sqlvalue->cb+1 );
                 if ( _sqlvalue->pubdata.c_value == NULL ) 
                    return E_FAIL;
                 if( _datatype == DFFAPVCHAR ||
                     _datatype == DFFAPVCHAR2 )
                     memcpy( _sqlvalue->pubdata.c_value, _datapb, \
                             _sqlvalue->cb );
                 else
                     strncpy( _sqlvalue->pubdata.c_value,_datapb, \
                              (_sqlvalue->cb+1) );
             }
/*             _sqlvalue->cb = strlen( _datapb ); */
             break;
        case DFFAPDATE:
             _sqlvalue->pubdata.c_value = calloc( 1, DEF_DATE_LEN );
             if( _sqlvalue->pubdata.c_value == NULL ) 
                 return E_FAIL;
             rstrdate( _datapb, &tmp_date );
             rfmtdate( tmp_date, DATE_FMT, _sqlvalue->pubdata.c_value );
/*             _sqlvalue->cb = strlen( _datapb ); */
             break;
        case DFFAPSMINT:
/* 20051014 handq 统一使用l_value
             _sqlvalue->pubdata.s_value = (short ) atoi( _datapb );
             break;
*/
        case DFFAPINT:
        case DFFAPSERIAL:
             _sqlvalue->pubdata.l_value = (long ) atol(_datapb) ;
             break;
        /* 关于SQLSMFLOAT有待测试*/ 
        case DFFAPSMFLOAT:
/* 20051014 handq 统一使用d_value
             _sqlvalue->pubdata.f_value = ( float ) bcl_myatof(_datapb);
             break;
*/
        case DFFAPMONEY:
/* 20051014 handq 统一使用d_value
             _sqlvalue->pubdata.d_value =  ( double ) bcl_myatof(_datapb+1);
             break;
*/
        case DFFAPDECIMAL:
        case DFFAPFLOAT:
             _sqlvalue->pubdata.d_value =  ( double ) bcl_myatof(_datapb);
             break;
        case DFFAPNULL:
             break;
        default: 
             return E_FAIL;
    }

    return E_OK;
}


/*------------------------------------------------------------------------*
 *  Function Name :int InfSqlAllocate ()                                  *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/

int 
InfSqlAllocate ( tSqlStru *_sqlstru, const char * _descname, int  _alloc_count)
{
    EXEC SQL BEGIN DECLARE SECTION;
         int           h_alloc_count;
         char          h_desc_id[20];
    EXEC SQL END DECLARE SECTION;
    char sql_str[SQLSTRLEN];
    
    h_alloc_count=_alloc_count;
    
    strcpy( _sqlstru->desc_id, _descname );
    strcpy(h_desc_id,_sqlstru->desc_id);
    
    trc_db_rec( "", SQLCODE );
    EXEC SQL allocate descriptor :h_desc_id with max :h_alloc_count;
    sprintf( sql_str, "ALLOCATE DESCRIPTOR %s WITH MAX %d", \
                       h_desc_id, h_alloc_count );
    trc_db_rec( sql_str, SQLCODE );
    _sqlstru->sql_code=SQLCODE;
    if( _sqlstru->sql_code == 0 )
        _sqlstru->tsqlstat.alloc_stat =1;
    
    if(_sqlstru->sql_code || _sqlstru->user_code )  {
        strcpy( _sqlstru->user_msg , "Informix SqlAllocate error" );
        InfDynGetErr( _sqlstru );
        DB_ERR( _sqlstru )
    }
    else  
        return E_OK;
}

/*------------------------------------------------------------------------*
 *  Function Name : int InfSqlPrepare  ()                                 *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/
int 
InfSqlPrepare ( tSqlStru *_sqlstru,const char *_prepare_name,const char *_sql_str )
{
    EXEC SQL BEGIN DECLARE SECTION;
         char          *h_sql_str;
         char          h_prep_id[20];
    EXEC SQL END DECLARE SECTION;
    char sql_str[SQLSTRLEN];
    
    if ( _sqlstru->tsqlstat.prep_stat == 0 )  {
        h_sql_str = _sql_str;
        strcpy(_sqlstru->prep_id,_prepare_name);
        strcpy(h_prep_id,_sqlstru->prep_id);
        strcpy( _sqlstru->sql_str, _sql_str );
        _sqlstru->ques_num = get_comma_num( _sql_str, '?' );

        trc_db_rec( "", SQLCODE );
        EXEC SQL prepare :h_prep_id from :h_sql_str;
        snprintf( sql_str, SQLSTRLEN, "PREPARE %s FROM %s", h_prep_id, h_sql_str );
        trc_db_rec( sql_str, SQLCODE );

        _sqlstru->sql_code=SQLCODE;
        if( _sqlstru->sql_code == 0 )
            _sqlstru->tsqlstat.prep_stat =1;
    }
    else 
        _sqlstru->user_code = E_MULTI_PREP;

    if(_sqlstru->sql_code || _sqlstru->user_code )  {
        strcpy( _sqlstru->user_msg , "Informix SqlPrepare error");
        InfDynGetErr( _sqlstru );
        DB_ERR( _sqlstru )
    }
    else  
        return E_OK;
}

/*------------------------------------------------------------------------*
 *  Function Name : int InfSqlDeclare  ()                                 *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/
int 
InfSqlDeclare  ( tSqlStru *_sqlstru ,const char * _cursor_name , char _op_flag)
{
    EXEC SQL BEGIN DECLARE SECTION;
         char          h_curs_id[20];
         char          h_prep_id[20];
    EXEC SQL END DECLARE SECTION;
    char sql_str[SQLSTRLEN];

    strcpy(_sqlstru->curs_id,_cursor_name);
    strcpy(h_curs_id,_sqlstru->curs_id);
    _sqlstru->op_flag = _op_flag;
    if( _sqlstru->tsqlstat.prep_stat ) {
        strcpy(h_prep_id,_sqlstru->prep_id);

        trc_db_rec( "", SQLCODE );
        if ( _sqlstru->op_flag == 'H' || _sqlstru->op_flag == 'h' ) {
            EXEC SQL declare :h_curs_id cursor with hold for :h_prep_id;
            sprintf( sql_str, "DECLARE %s CURSOR WITH HOLD FOR %s", h_curs_id, h_prep_id );
        }
        else if ( _sqlstru->op_flag ) {
            EXEC SQL declare :h_curs_id scroll cursor for :h_prep_id;
            sprintf( sql_str, "DECLARE %s SCROLL CURSOR FOR %s", h_curs_id, h_prep_id );
        }
        else {
            EXEC SQL declare :h_curs_id cursor for :h_prep_id;
            sprintf( sql_str, "DECLARE %s CURSOR FOR %s", h_curs_id, h_prep_id );
        }
        trc_db_rec( sql_str, SQLCODE );

        _sqlstru->sql_code=SQLCODE;
        if(_sqlstru->sql_code == 0 )
            _sqlstru->tsqlstat.cur_allocstat = 1;
    }
    else 
        _sqlstru->user_code=E_PREP_ID;

    if(_sqlstru->sql_code || _sqlstru->user_code ) {
        strcpy( _sqlstru->user_msg , "Informix SqlDeclare error" );
        InfDynGetErr( _sqlstru );
        DB_ERR( _sqlstru )
    }
    else  
        return E_OK;

}

/*------------------------------------------------------------------------*
 *  Function Name : int InfSqlDescribe ( )                                * 
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/ 
int 
InfSqlDescribe ( tSqlStru *_sqlstru )
{
    EXEC SQL BEGIN DECLARE SECTION;
         char          h_curs_id[20];
         char          h_prep_id[20];
         char          h_desc_id[20];
         int           h_column_num=0;  
    EXEC SQL END DECLARE SECTION;
    char sql_str[SQLSTRLEN];
    
    strcpy(h_curs_id,_sqlstru->curs_id);
    strcpy(h_desc_id,_sqlstru->desc_id);
    
    if( _sqlstru->tsqlstat.cur_openstat ) {
        strcpy(h_prep_id,_sqlstru->prep_id);
        trc_db_rec( "", SQLCODE );
        EXEC SQL describe :h_prep_id using sql descriptor :h_desc_id;
        sprintf( sql_str, "DESCRIBE %s USING SQL DESCRIPTOR %s", \
                          h_prep_id, h_desc_id );
/* 20050426 EXECUTE PROCEDURE SQLCODE=SQ_EXECPROC */
        if( SQLCODE == SQ_EXECPROC ) {
            strcat( sql_str, "(execute procedure)" );
            _sqlstru->sql_code = 0;
        }
        else
            _sqlstru->sql_code = SQLCODE;
        trc_db_rec( sql_str, _sqlstru->sql_code );
        if( !( _sqlstru->sql_code ) ) {
            EXEC SQL get descriptor :h_desc_id :h_column_num = COUNT ;
            sprintf( sql_str, "GET DESCRIPTOR %s %d = COUNT", \
                              h_desc_id, h_column_num );
            trc_db_rec( sql_str, SQLCODE );
            _sqlstru->column_num = h_column_num;
        }
    } /* end of if cur_openstat */
    else 
        _sqlstru->user_code=E_PREP_ID;
    
    if(_sqlstru->sql_code || _sqlstru->user_code ) {
        strcpy( _sqlstru->user_msg , "Informix SqlDescribe error" );
        InfDynGetErr( _sqlstru );
        DB_ERR( _sqlstru )
    }
    else 
        return E_OK;
}


/*------------------------------------------------------------------------*
 *  Function Name :int InfSqlExecute()                                    *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/
int 
InfSqlExecute  ( tSqlStru *_sqlstru )
{
    EXEC SQL BEGIN DECLARE SECTION;
         char      h_desc_id[20];
         char      h_prep_id[20];
    EXEC SQL END DECLARE SECTION;
    char sql_str[SQLSTRLEN];

    strcpy(h_desc_id,_sqlstru->desc_id);
    strcpy(h_prep_id,_sqlstru->prep_id);

    if( _sqlstru->tsqlstat.prep_stat ) {
        if( _sqlstru->ques_num != 0 ) {
            if( _sqlstru->tsqlstat.alloc_stat ) {
                if(_sqlstru->ques_num==( short )_sqlstru->tsqlstat.put_stat ) {
                    trc_db_rec( "", SQLCODE );
                    EXEC SQL execute :h_prep_id using sql descriptor :h_desc_id;
                    sprintf( sql_str, "EXECUTE %s USING SQL DESCRIPTOR %s", \
                                      h_prep_id, h_desc_id );
                    trc_db_rec( sql_str, SQLCODE );
                    _sqlstru->sql_code=SQLCODE;
                    _sqlstru->proc_row=sqlca.sqlerrd[2];
                }
                else 
                    _sqlstru->user_code = E_PUT_NOMATCH;
            }
            else
                _sqlstru->user_code = E_EXE_NOALLOC;
        }
        else {
        /*这段程序是专为无待定参数的update ,delete,insert 的程序设计的*/
            trc_db_rec( "", SQLCODE );
            EXEC SQL execute :h_prep_id ;
            sprintf( sql_str, "EXECUTE %s", h_prep_id );
            trc_db_rec( sql_str, SQLCODE );
            _sqlstru->sql_code=SQLCODE;
            _sqlstru->proc_row=sqlca.sqlerrd[2];
        } /*end of else of ques_num*/
    } /* end of prep_stat */
    else 
        _sqlstru->user_code = E_PREP_ID;

    if(_sqlstru->sql_code || _sqlstru->user_code ) {
        strcpy( _sqlstru->user_msg , "Informix SqlExecute error");
        InfDynGetErr( _sqlstru );
        DB_ERR( _sqlstru )
    }
    else 
        return E_OK;
}

/*------------------------------------------------------------------------*
 *  Function Name : int InfSqlOpenCur  ()                                 *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/
int
InfSqlOpenCur  ( tSqlStru *_sqlstru )
{
    EXEC SQL BEGIN DECLARE SECTION;
         char     h_curs_id[20];
         char     h_desc_id[20];
    EXEC SQL END DECLARE SECTION;
    char sql_str[SQLSTRLEN];

    if( _sqlstru->tsqlstat.cur_allocstat ) {
        if ( _sqlstru->tsqlstat.alloc_stat ) {
            strcpy(h_curs_id,_sqlstru->curs_id);
            strcpy(h_desc_id,_sqlstru->desc_id);
            if( _sqlstru->ques_num != 0) {
                if(_sqlstru->ques_num == (short)_sqlstru->tsqlstat.put_stat) {
                    trc_db_rec( "", SQLCODE );
                    EXEC SQL open :h_curs_id using sql descriptor :h_desc_id;
                    sprintf( sql_str, "OPEN %s USING SQL DESCRIPTOR %s", \
                                       h_curs_id, h_desc_id );
                    trc_db_rec( sql_str, SQLCODE ); 
                    _sqlstru->sql_code = SQLCODE;
                    if ( !(_sqlstru->sql_code) )
                        _sqlstru->tsqlstat.cur_openstat = 1;
                }
                else 
                    _sqlstru->user_code = E_PUT_NOMATCH;
            }
            else {
                trc_db_rec( "", SQLCODE );
                EXEC SQL  open :h_curs_id; 
                sprintf( sql_str, "OPEN %s", h_curs_id );
                trc_db_rec( sql_str, SQLCODE ); 
                _sqlstru->sql_code = SQLCODE;
                if ( !(_sqlstru->sql_code) )
                    _sqlstru->tsqlstat.cur_openstat = 1;
            } /* end of else */
        } /* end of alloc_stat */
        else 
            _sqlstru->user_code = E_CUR_NOALLOC;
    } /* end of cur_allocstat */
    else 
        _sqlstru->user_code=E_CURS_ID;
    
    if(_sqlstru->sql_code || _sqlstru->user_code ) {
        strcpy( _sqlstru->user_msg , "Informix SqlOpenCur error" );
        InfDynGetErr( _sqlstru );
        DB_ERR( _sqlstru )
    }
    else 
        return E_OK;
}

/*------------------------------------------------------------------------*
 *  Function Name : int InfSqlFetchCur ()                                 *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/
int 
InfSqlFetchCur ( tSqlStru *_sqlstru , long _off_set )
{
    EXEC SQL BEGIN DECLARE SECTION;
         char    h_desc_id[20];
         char    h_curs_id[20];
         long   off_set;
    EXEC SQL END DECLARE SECTION;
    char sql_str[SQLSTRLEN];

    strcpy(h_desc_id,_sqlstru->desc_id);
    strcpy(h_curs_id,_sqlstru->curs_id);
    _sqlstru->offset = _off_set;
    off_set = _off_set;
    if ( _sqlstru->tsqlstat.cur_openstat ) {
        trc_db_rec( "", SQLCODE );
        switch ( _sqlstru->op_flag ) {
           case 'P':
           case 'p':
             EXEC SQL fetch PREVIOUS :h_curs_id using sql descriptor :h_desc_id;
             sprintf( sql_str, "FETCH PREVIOUS %s USING SQL DESCRIPTOR %s", \
                               h_curs_id, h_desc_id );
             break;
           case 'C':
           case 'c':
             EXEC SQL fetch CURRENT :h_curs_id using sql descriptor :h_desc_id;
             sprintf( sql_str, "FETCH CURRENT %s USING SQL DESCRIPTOR %s", \
                               h_curs_id, h_desc_id );
             break;
           case 'F':
           case 'f':
             EXEC SQL fetch FIRST :h_curs_id using sql descriptor :h_desc_id;
             sprintf( sql_str, "FETCH FIRST %s USING SQL DESCRIPTOR %s", \
                               h_curs_id, h_desc_id );
             break;
           case 'L':
           case 'l':
             EXEC SQL fetch LAST :h_curs_id using sql descriptor :h_desc_id;
             sprintf( sql_str, "FETCH LAST %s USING SQL DESCRIPTOR %s", \
                               h_curs_id, h_desc_id );
             break;
           case 'R':
           case 'r':
             EXEC SQL fetch RELATIVE :off_set :h_curs_id using sql descriptor :h_desc_id;
             sprintf( sql_str, "FETCH RELATIVE %s USING SQL DESCRIPTOR %s", \
                               h_curs_id, h_desc_id );
             break;
           case 'A':
           case 'a':
             EXEC SQL fetch ABSOLUTE :off_set :h_curs_id using sql descriptor :h_desc_id;
             sprintf( sql_str, "FETCH ABSOLUTE %s USING SQL DESCRIPTOR %s", \
                               h_curs_id, h_desc_id );
             break;
           default:
             EXEC SQL fetch NEXT :h_curs_id using sql descriptor :h_desc_id;
             sprintf( sql_str, "FETCH NEXT %s USING SQL DESCRIPTOR %s", \
                               h_curs_id, h_desc_id );
             break;
        } /* end of switch */
        trc_db_rec( sql_str, SQLCODE ); 
        _sqlstru->sql_code = SQLCODE;
/* 此处的标志有待商榷*/
        if ( (_sqlstru->sql_code == 0) || (_sqlstru->sql_code == 100)) {
            _sqlstru->tsqlstat.cur_fetchstat = 1;
            _sqlstru->proc_row++;
        }
        if ( _sqlstru->sql_code == 100 )
            _sqlstru->tsqlstat.fetchend = 1;
    }
    else
        _sqlstru->user_code=E_CUR_NOOPEN;
    
    if((_sqlstru->sql_code || _sqlstru->user_code ) \
         && ( _sqlstru->sql_code != 100 ) ) {
        strcpy( _sqlstru->user_msg , "Informix SqlFetchCur error" );
        InfDynGetErr( _sqlstru );
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
 *  Function Name : int InfSqlCloseCur ()                                 *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/
int 
InfSqlCloseCur ( tSqlStru *_sqlstru )
{
    EXEC SQL BEGIN DECLARE SECTION;
         char h_curs_id[20];
    EXEC SQL END DECLARE SECTION;
    char sql_str[SQLSTRLEN];

    if( _sqlstru->tsqlstat.cur_openstat == 1 ) {
        strcpy(h_curs_id,_sqlstru->curs_id);
        trc_db_rec( "", SQLCODE );
        EXEC SQL close :h_curs_id;
        sprintf( sql_str, "CLOSE %s", h_curs_id );
        trc_db_rec( sql_str, SQLCODE );
        _sqlstru->sql_code=SQLCODE;
        if ( !( _sqlstru->sql_code ) )  
            _sqlstru->tsqlstat.cur_openstat = 0;
    }
    else 
        _sqlstru->user_code = E_CUR_UNCLOSED;
    
    if(_sqlstru->sql_code || _sqlstru->user_code ) {
        strcpy( _sqlstru->user_msg , "Informix SqlCloseCur error" );
        InfDynGetErr( _sqlstru );
        DB_ERR( _sqlstru )
    }
    else 
        return E_OK;
}

/*------------------------------------------------------------------------*
 *  Function Name : int InfSqlFreeCur  ()                                 *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/
int 
InfSqlFreeCur ( tSqlStru *_sqlstru )
{
    EXEC SQL BEGIN DECLARE SECTION;
         char h_curs_id[20];
    EXEC SQL END DECLARE SECTION;
    char sql_str[SQLSTRLEN];

    if( _sqlstru->tsqlstat.cur_openstat == 0 )  {
        strcpy(h_curs_id,_sqlstru->curs_id);
        trc_db_rec( "", SQLCODE );
        EXEC SQL free :h_curs_id;
        sprintf( sql_str, "FREE %s", h_curs_id );
        trc_db_rec( sql_str, SQLCODE );
        _sqlstru->sql_code=SQLCODE;
    }
    else 
        _sqlstru->user_code=E_CUR_NOCLOSE;
    
    if( (_sqlstru->user_code == 0)  && (_sqlstru->sql_code == 0) )
        memset(_sqlstru->curs_id,0x00,sizeof(_sqlstru->curs_id));
    
    if(_sqlstru->sql_code || _sqlstru->user_code ) {
        strcpy( _sqlstru->user_msg , "Informix SqlFreeCur error" );
        InfDynGetErr( _sqlstru );
        DB_ERR( _sqlstru )
    }
    else 
        return E_OK;

}

/*------------------------------------------------------------------------*
 *  Function Name :int InfSqlDealloc  ()                                  *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/
int 
InfSqlDealloc  ( tSqlStru *_sqlstru )
{
    EXEC SQL BEGIN DECLARE SECTION;
         char h_desc_id[20];
    EXEC SQL END DECLARE SECTION;
    char sql_str[SQLSTRLEN];

    if( _sqlstru->tsqlstat.alloc_stat == 1 )  {
        strcpy(h_desc_id,_sqlstru->desc_id);
        trc_db_rec( "", SQLCODE );
        EXEC SQL deallocate descriptor :h_desc_id;
        sprintf( sql_str, "DEALLOCATE DESCRIPTOR %s", h_desc_id );
        trc_db_rec( sql_str, SQLCODE );
        _sqlstru->sql_code=SQLCODE;
        if ( !( _sqlstru->sql_code ) ) 
            _sqlstru->tsqlstat.alloc_stat = 0;
    }
    else 
        _sqlstru->user_code=E_DESC_UNALLOCATED;
    
    if( (_sqlstru->user_code == 0)  && (_sqlstru->sql_code == 0) )
        memset(_sqlstru->desc_id,0x00,sizeof(_sqlstru->desc_id));
    
    if(_sqlstru->sql_code || _sqlstru->user_code ) {
        strcpy( _sqlstru->user_msg , "Informix SqlDealloc error" );
        InfDynGetErr( _sqlstru );
        DB_ERR( _sqlstru )
    }
    else 
        return E_OK;
}



/*------------------------------------------------------------------------*
 *  Function Name :int InfSqlGetValue ()                                  *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/

int 
InfSqlGetValue ( tSqlStru *_sqlstru, short _getvalue_serial )
{
    EXEC SQL BEGIN DECLARE SECTION;
         char    h_desc_id[20];
         char    h_prep_id[20];
         short   h_get_serial;
    
         char            h_get_name[20];
         short           h_get_type;
         int             h_get_length;
         char          * h_get_data;
         int             ret;
    EXEC SQL END DECLARE SECTION;
     
    h_get_serial = _getvalue_serial;
    if( h_get_serial <= _sqlstru->column_num ) {
        if( _sqlstru->tsqlstat.cur_fetchstat ) {
            strcpy(h_desc_id,_sqlstru->desc_id);
            strcpy(h_prep_id,_sqlstru->prep_id);
            EXEC SQL get descriptor :h_desc_id VALUE :h_get_serial
                                                     :h_get_name   = NAME,
                                                     :h_get_type   = TYPE,
                                                     :h_get_length = LENGTH;
            bcl_trimall(h_get_name);
            /*    lvarchar 长度多1，为什么不知道     */
            if (h_get_type == DFFAPVCHAR2 && h_get_length > 1)
                h_get_length -= 1;

/*  +MAX_DOUBLE_STR_LEN INT LENGTH is 4, but need at least 10 char buffer */
            h_get_data = calloc( 1, h_get_length+MAX_DOUBLE_STR_LEN );
            if( h_get_data == NULL ) {
                bclerreg( E_ALLOC, _FL_, h_get_length );
                return E_FAIL;
            }
            EXEC SQL get descriptor :h_desc_id VALUE :h_get_serial
                                                     :h_get_data   = DATA;
            bcl_rtrim(h_get_data);

/* (: use old type free space :) */
            _sqlstru->tValuepb[h_get_serial-1].cb     = h_get_length;
            ret = InfGetDescData ( &(_sqlstru->tValuepb[h_get_serial-1]),h_get_data,h_get_type);
            _sqlstru->tValuepb[h_get_serial-1].type   = h_get_type;
            strcpy( _sqlstru->tValuepb[h_get_serial-1].name , h_get_name );
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
        sprintf( _sqlstru->user_msg , "Informix SqlGetValue(GET IN_VAL[%d],TYPE[%d],LEN[%d]) error", h_get_serial, h_get_type, h_get_length );
        strcpy( _sqlstru->user_msg , "Informix SqlGetValue error" );
        InfDynGetErr( _sqlstru );
        DB_ERR( _sqlstru )
    }
    else 
        return E_OK;
}


/*------------------------------------------------------------------------*
 *  Function Name : int InfSqlPutValue ()                                 *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/
int 
InfSqlPutValue ( tSqlStru *_sqlstru, short _putvalue_serial)
{
    EXEC SQL BEGIN DECLARE SECTION;
         char          h_curs_id[20];
         char          h_prep_id[20];
         char          h_desc_id[20];
         short         h_ques_num;  
         int           ret;  
         short         i=0;
         short         h_t_type;
         short         h_t_length;
         char        * h_t_data;
    EXEC SQL END DECLARE SECTION;

    strcpy(h_curs_id,_sqlstru->curs_id);
    strcpy(h_desc_id,_sqlstru->desc_id);
    
    if( _sqlstru->tsqlstat.prep_stat )  {
        if( _sqlstru->tsqlstat.alloc_stat )  {
            strcpy(h_prep_id,_sqlstru->prep_id);
            h_ques_num=_sqlstru->ques_num;
            i=_putvalue_serial;
            if( h_ques_num >= i )  {
                EXEC SQL set descriptor :h_desc_id COUNT = :h_ques_num;
                h_t_type   = _sqlstru->tValuepb[i-1].type;
                h_t_length = _sqlstru->tValuepb[i-1].cb;
                h_t_data = calloc( 1, h_t_length+MAX_DOUBLE_STR_LEN);
                if ( h_t_data == NULL ) {
                    bclerreg( E_ALLOC, _FL_,  h_t_length );
                    return E_FAIL;
                }
                ret=InfPutDescData(&(_sqlstru->tValuepb[i-1]),h_t_data);
                if( ret != E_OK )  {
                    free( h_t_data );
                    _sqlstru->user_code = E_ILLLEGAL_TYPE;
                    APP_ERR( E_ILLLEGAL_TYPE )
                }
                /*   原因不明lvarchar只能用DFFAPCHAR 类型才能正确插入数据 */
                if (h_t_type == DFFAPVCHAR2 )
                    h_t_type = DFFAPCHAR ;

                EXEC SQL set descriptor :h_desc_id VALUE    :i 
                                                   TYPE   = :h_t_type,
                                                   LENGTH = :h_t_length,
                                                   DATA   = :h_t_data;

#ifdef DEBUGDB
faptrace( "log/db.trc" , (u_char *)"" , 0 , \
       "PUT IN_VAL[%d],TYPE[%d],LEN[%d],DATA[%.128s]\n" , \
       i,h_t_type , h_t_length , h_t_data );
#endif

                free( h_t_data );
                _sqlstru->sql_code =SQLCODE;
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
        sprintf( _sqlstru->user_msg , "Informix SqlPutValue(PUT IN_VAL[%d],TYPE[%d],LEN[%d] error", i, h_t_type, h_t_length );
        InfDynGetErr( _sqlstru );
        DB_ERR( _sqlstru )
    }
    else 
        return E_OK;
}

/*------------------------------------------------------------------------*
 *  Function Name :int InfSqlFreePrep ()                                  *
 *  Description   :                                                       *
 *  Input         :                                                       *
 *  Output        :                                                       *
 *------------------------------------------------------------------------*/

int 
InfSqlFreePrep( tSqlStru *_sqlstru )
{
    EXEC SQL BEGIN DECLARE SECTION;
         char h_prep_id[20];
    EXEC SQL END DECLARE SECTION;
    char sql_str[SQLSTRLEN];

    if( _sqlstru->tsqlstat.prep_stat )  {
        strcpy(h_prep_id,_sqlstru->prep_id);
        trc_db_rec( "", SQLCODE );
        EXEC SQL free :h_prep_id;
        sprintf( sql_str, "FREE %s", h_prep_id );
        trc_db_rec( sql_str, SQLCODE );
        _sqlstru->sql_code=SQLCODE;
        if ( _sqlstru->sql_code == 0 )
            _sqlstru->tsqlstat.prep_stat = 0;
    }
    else 
        _sqlstru->user_code=E_PREP_ID;
    
    if( (_sqlstru->user_code == 0)  && (_sqlstru->sql_code == 0) )
        memset(_sqlstru->curs_id,0x00,sizeof(_sqlstru->prep_id));
      
    if(_sqlstru->sql_code || _sqlstru->user_code ) {
        strcpy( _sqlstru->user_msg , "Informix SqlFreePrep error" );
        return E_FAIL;
    }
    else 
        return E_OK;
}

