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
 *    ProgramName : __pladb.h
 *    SystemName  : StarRing
 *    Version     : 3.0
 *    Language    : C
 *    OS & Env    : RedHat Linux 9.0
                    Oracle  9i
 *    Description : 数据库封装定义
 *    History     : 
 *    YYYY/MM/DD        Position        Author         Description
 *    -------------------------------------------------------------
 *    2003/08/06        BeiJing         Handq          Creation
 */

#ifndef ____PLADB_H
#define ____PLADB_H

#include "sysdef.h"

struct __for_test1 {
    char m1;
    double m2;
};
#define STMAXALIGN        offsetof(struct __for_test1, m2)

#define STRALIGN(p,s) (p)=(void *)((long)p + (((long)(p)%(min((s),(STMAXALIGN)))==0)?0:(min((s),(STMAXALIGN))-(long)(p)%min((s),(STMAXALIGN)))))

/* Max length of sql statement */
#define  SQLSTRLEN	4096

/* SQL functions  */

typedef struct {
       int  (* MArea) ( void );
       int  (* FArea ) ( void );
       int  (* PutData) ( int , int , void * );
       int  (* GetData) ( int , void * , int );
       int  (* GetType) ( int , short * );
       int  (* GetTabDesc) ( char * , int * );
       int  (* PutStru) ( char * , int , void * , int );
       int  (* GetStru) ( int , void * , int );
       void (* SqlSetConnect) ( char * );
       int  (* SqlOpenDB) ( int, ...);
       int  (* SqlCloseDB) ( void );
 
       int  (* SqlBeginWork) ( void );
       int  (* SqlCommit) ( void );
       int  (* SqlRollBack) ( void );

       int  (* SqlSelect) ( int, char *, char, long );
       int  (* SqlExec) ( char * );
       int  (* SqlBatExec) ( int, char * );
       int  (* FAreaD ) ( char * );
} DB_API;

typedef struct { 
  char             * c_value;
  int              i_value;
  short            s_value;
  long             l_value;
  unsigned  int    ui_value;
  unsigned  short  us_value;
  unsigned  long   ul_value;
  float            f_value;
  double           d_value;
} tSqlPubData;

typedef struct  {
   int               type;
   int               cb;
   char              name[32];
   tSqlPubData       pubdata; 
}tSqlValue;

typedef struct {
    unsigned char fetchend;   
                  /* fetch return SQLCODE is 100 */
    unsigned char prep_stat;
		  /* write if execute SqlPrepare or SqlFreePrep */
                  /*   execute SqlPrepare set 1 ,SqlFreePrep set 0*/
    unsigned char cur_allocstat;
		  /* write if execute  SqlDeclare  or SqlFreeCur  */
                  /*   execute SqlDeclare set 1 SqlFreeCur set 0  */
    unsigned char alloc_stat; 
		  /* write if execute  SqlAllocate  SqlDealloc    */
                  /*   execute SqlAllocate set 1 ,SqlDealloc set 0*/
    unsigned char put_stat; 
		  /* write if execute  SqlDescribe for question   */
                  /* 它的值等于已经描述了的条件个数值             */
    unsigned char cur_openstat;  
		  /* write if execute  SqlOpenCur or SqlCloseCur  */
                  /*   execute open set 1 ,close set 0            */
                  /*unsigned char get_stat;                       */
		  /* write if execute  SqlDescribe for  column    */
                  /* 它的值等于已经描述了的表的字段个数           */
    unsigned char cur_fetchstat; 
		  /* write if execute  SqlFetchCur                */
                  /*   execute set 1 else set 0                   */
}tSqlStation;

typedef struct {
    long	user_code;	/* user operate error value                */
    char        user_msg[128];  /* user error msg                          */
    long	sql_code;	/* SQL operate return value                */
				/*  1000 - check the user_code             */
    long	isam_code;	/* ISAM error code                         */
    long	proc_row;	/* number of rows processed                */
    int  	ques_num;       /* number of where clause ? number         */
    char	prep_id[20];	/* id which created by prepare statement   */
    char	curs_id[20]; 	/* id which created by declare statement   */
    char	*desc_id_in; 	/* for oracle/db2 bind  */
    char	*desc_id_out; 	/* for oracle/db2 select  */
    char	*desc_id_tmp; 	/* for db2 tmp, keep sqlind sqldata point  */
    char	desc_id[20]; 	/* id which created by describe statement  */
    int         column_num;     /* number of cols fetch */
    char        op_flag;        /* flag of fetch type */
    short       offset;         /* RELATIVE or ABSOLUTE fetch offset */
    tSqlStation tsqlstat;   
    tSqlValue   *tValuepb;      /* put or get data buffer */
    int         tV_num;         /* buffer size */
/* 20051027 记录已经分配的tValuepb个数 */
    int         max_num;         /* tValuepb size */
   /* char	buffer[1024];	 tempery space use for selected value    */
    char        sql_str[SQLSTRLEN+1]; /* 临时保存的SQL语句 */
} tSqlStru;

/* constants for SQL  FUNCATION */

#ifndef FETCH_END  
#define FETCH_END(a) (a)->tsqlstat.fetchend
#endif  

#ifndef GetQuesNum
#define GetQuesNum(pb)      pb->ques_num; 
#endif

#ifndef GetColNum
#define GetColNum(pb)      pb->column_num; 
#endif

#define APP_ERR( eid ) { \
                  bclerreg( eid, _FL_ ); \
                  return E_FAIL; \
}

#ifdef INFORMIX

#include   <infsqlfun.h>

#define DB_ID          0

#define DFFAPCHAR        0
#define DFFAPSMINT       1
#define DFFAPINT         2
#define DFFAPFLOAT       3
#define DFFAPSMFLOAT     4
#define DFFAPDECIMAL     5
#define DFFAPSERIAL      6
#define DFFAPDATE        7
#define DFFAPMONEY       8
#define DFFAPNULL        9
#define DFFAPDTIME       10
#define DFFAPBYTES       11
#define DFFAPTEXT        12
#define DFFAPVCHAR       13
#define DFFAPINTERVAL    14
#define DFFAPNCHAR       15
#define DFFAPNVCHAR      16

#define DFFAPINTEGER     -2 /* 20050428 支持 INTEGER,内部转换成INT */
#define DFFAPVCHAR2      43 /*         lvarchar 支持               */
#define DFFAPNUMBER      2
#define DFFAPLONG        12
#define DFFAPRAW         11
#define DFFAPLONGRAW     11

#endif   /* End of ifdef INFORMIX */

#ifdef ORACLE

#include   <orasqlfun.h>

#define DB_ID          1

#define DFFAPVCHAR2      1
#define DFFAPNUMBER      2
#define DFFAPCHAR        96
#define DFFAPINT         3
#define DFFAPFLOAT       4
#define DFFAPSMFLOAT     5
#define DFFAPDECIMAL     7
#define DFFAPLONG        8
#define DFFAPSMINT       11
#define DFFAPDATE        12 
#define DFFAPRAW         23
#define DFFAPLONGRAW     24

#define DFFAPINTEGER     -3 /* 20050428 支持 INTEGER,内部转换成INT */
#define DFFAPSERIAL      3
#define DFFAPMONEY       2
#define DFFAPNULL        0
#define DFFAPDTIME       12
#define DFFAPBYTES       23
#define DFFAPTEXT        8
#define DFFAPVCHAR       1
#define DFFAPINTERVAL    12
#define DFFAPNCHAR       1
#define DFFAPNVCHAR      1

#endif   /* End of ifdef ORACLE*/

#ifdef SYBASE

#include   <sybsqlfun.h>

#define DB_ID          2

#define DFFAPCHAR        1
#define DFFAPSMINT       5
#define DFFAPINT         4
#define DFFAPSMFLOAT     7
#define DFFAPFLOAT       8
#define DFFAPNUMBER      2
#define DFFAPDECIMAL     3
#define DFFAPDATE        16  /* smalldatetime */
#define DFFAPMONEY       -10
#define DFFAPDTIME       17   /* datetime */
#define DFFAPBYTES       -5
#define DFFAPTEXT        -3

#define DFFAPVCHAR       -2
#define DFFAPNCHAR       13
#define DFFAPNVCHAR      14
#define DFFAPINTERVAL    9
#define DFFAPSERIAL      99

#define DFFAPINTEGER     -40 /* 20050428 支持 INTEGER,内部转换成INT */
#define DFFAPNULL        0
#define DFFAPVCHAR2      1
#define DFFAPLONG        1
#define DFFAPRAW         1
#define DFFAPLONGRAW     1

#endif   /* End of ifdef SYBASE */

#ifdef DB2

#include   <db2sqlfun.h>

#define DB_ID          3

#define DFFAPNULL        1
#define DFFAPDATE        385  /* DATE */
#define DFFAPDTIME       389  /* TIME */
#define DFFAPINTERVAL    393  /* TIMESTAMP */
#define DFFAPVCHAR       449  /* VARCHAR */
#define DFFAPCHAR        453  /* CHAR */
#define DFFAPLONG        457  /* LONG VARCHAR */
#define DFFAPTEXT        409  /* CLOB */
#define DFFAPBYTES       405  /* BLOB */
#define DFFAPFLOAT       481  /* DOUBLE/REAL */
#define DFFAPDECIMAL     485  /* DECIMAL */
#define DFFAPNUMBER      485  /* NUMERIC or 505 */
#define DFFAPINT         493  /* BIGINT */
#define DFFAPINTEGER     497  /* INTEGER */
#define DFFAPSMINT       501  /* SMALLINT */

#define DFFAPSMFLOAT     3    /* REAL convert to 481 */
#define DFFAPVCHAR2      461  /* CSTR for varying length string for C (\0) */

#define DFFAPRAW         0
#define DFFAPLONGRAW     0

#define DFFAPSERIAL      0
#define DFFAPMONEY       0
#define DFFAPNCHAR       0
#define DFFAPNVCHAR      0

#endif   /* End of ifdef db2 */

#ifdef NODATABASE

#include   <nosqlfun.h>

#define DB_ID          4

#define DFFAPCHAR        0
#define DFFAPSMINT       0
#define DFFAPINT         0
#define DFFAPFLOAT       0
#define DFFAPSMFLOAT     0
#define DFFAPDECIMAL     0
#define DFFAPSERIAL      0
#define DFFAPDATE        0
#define DFFAPMONEY       0
#define DFFAPNULL        0
#define DFFAPDTIME       0
#define DFFAPBYTES       0
#define DFFAPTEXT        0
#define DFFAPVCHAR       0
#define DFFAPINTERVAL    0
#define DFFAPNCHAR       0
#define DFFAPNVCHAR      0

#define DFFAPINTEGER     0
#define DFFAPVCHAR2      0
#define DFFAPNUMBER      0
#define DFFAPLONG        0
#define DFFAPRAW         0
#define DFFAPLONGRAW     0


#endif   /* End of ifdef NODATABASE */

#endif
