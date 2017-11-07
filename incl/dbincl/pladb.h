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
 *    ProgramName : pladb.h
 *    SystemName  : StarRing
 *    Version     : 3.0
 *    Language    : C
 *    OS & Env    : RedHat Linux 9.0
                    Oracle  9i
 *    Description : 数据库封装函数
 *    History     : 
 *    YYYY/MM/DD        Position        Author         Description
 *    -------------------------------------------------------------
 *    2003/08/06        BeiJing         Handq          Creation
 */

#ifndef __PLADB_H
#define __PLADB_H

#include "sysunix.h"

#include "__pladb.h"

/* constants for SQL  FUNCATION */
#ifdef __cplusplus
extern "C" {
#endif

extern short MAX_VAR_NUM;
extern tSqlValue * TSQLVarPool;

extern short MAX_AREA;
extern short CURRENT_AREA;
extern short FETCH_AREA;
extern tSqlStru ** TSQLSTRU;

extern long FAP_SQLCODE;
extern char CONNECT_NAME[];

extern short DbId;
extern DB_API DbApi[];

extern int FAPCHAR;
extern int FAPSMINT;
extern int FAPINTEGER;
extern int FAPINT;
extern int FAPFLOAT;
extern int FAPSMFLOAT;
extern int FAPDECIMAL;
extern int FAPSERIAL;
extern int FAPDATE;
extern int FAPMONEY;
extern int FAPNULL;
extern int FAPDTIME;
extern int FAPBYTES;
extern int FAPTEXT;
extern int FAPVCHAR;
extern int FAPINTERVAL;
extern int FAPNCHAR;
extern int FAPNVCHAR;
extern int FAPVCHAR2;
extern int FAPNUMBER;
extern int FAPLONG;
extern int FAPRAW;
extern int FAPLONGRAW;

extern int IDEF_DTIME_LEN;
extern int IDEF_DATE_LEN;
extern int IDEF_CHAR_LEN;

#ifdef __cplusplus
}
#endif

/* 应用使用的二层封装接口 */
#define MallocArea         (*(DbApi[DbId].MArea))
#define FreeArea           (*(DbApi[DbId].FArea))
#define FreeAreaD          (*(DbApi[DbId].FAreaD))
#define DBPutData          (*(DbApi[DbId].PutData))
#define DBGetData          (*(DbApi[DbId].GetData))
#define DBGetType          (*(DbApi[DbId].GetType))
#define DBGetTabDesc       (*(DbApi[DbId].GetTabDesc))
#define DBPutStru          (*(DbApi[DbId].PutStru))
#define DBGetStru          (*(DbApi[DbId].GetStru))
#define DBSetConnect       (*(DbApi[DbId].SqlSetConnect))
#define DBOpen             (*(DbApi[DbId].SqlOpenDB))
#define DBClose            (*(DbApi[DbId].SqlCloseDB))
#define DBBeginWork        (*(DbApi[DbId].SqlBeginWork))
#define DBCommit           (*(DbApi[DbId].SqlCommit))
#define DBRollBack         (*(DbApi[DbId].SqlRollBack))
#define DBSelect           (*(DbApi[DbId].SqlSelect))
#define DBExec             (*(DbApi[DbId].SqlExec))
/* 20051027 二层封装增加批量操作支持 */
#define DBBatExec          (*(DbApi[DbId].SqlBatExec))

#endif
