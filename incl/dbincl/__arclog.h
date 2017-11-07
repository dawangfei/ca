/* $Id
 *    Copyright (c) 2001-2002 ADTEC Ltd.
 *    All rights reserved
 *
 *    This is unpublished proprietary
 *    source code of ADTEC Ltd.
 *    The copyright notice above does not
 *    evidence any actual or intended
 *    publication of such source code.
 *
 *    NOTICE: UNAUTHORIZED DISTRIBUTION, ADAPTATION OR USE MAY BE
 *    SUBJECT TO CIVIL AND CRIMINAL PENALTIES.
 *
 *
 *    FILE:         __arclog.h
 *    DESCRIPTION:  The head file of  writing log file module 
 *    AUTHOR:
 *                  NaWei 07/15/2003 - Developed for eSWITCH system
 *    MODIFIED:
 *
 */


#ifndef ____ARCLOG_H
#define ____ARCLOG_H

#include "sysdef.h"

#define eserrno bclgetapperr()
#define MESSAGE  100

#define CATFILENAME       "errmsg"		/*日志信息文件名称 */

#define ERRFILENAME       "errlogmd"            /* 日志模块日志信息文件名称 */
#define DESCPFILE         "errdef"              /* 日志说明定制文件名称     */

#define DEFAULT_LOCAL     "zh_cn.gb"            /* 缺省语言                 */
#define DSP_MUN           3                     /* 日志信息描述项数         */
#define ERR_MSG_LEN       512                   /* 最大err_msg文件大小      */
#define FMT_LEN           512                   /* 格式长度大小             */

/* add by qiuyp on 09/17/2001 */
/* add begin */
#define MAX_LOG_SIZE      2000000               /* 日志默认的最大Size       */
/* add end   */

typedef struct {
    int         appcode;                        /* 应用定义出错码           */
    int         syscode;                        /* unix (errno)             */
    pid_t       p_id;                           /* 进程号                   */
    char        *file;                          /* 日志文件名称             */
    int         line;                           /* 日志文件所在行           */
    short       level;                          /* 数嵌套的级次             */
    char        appmsg[ERR_MSG_LEN];            /* 应用定义出错信息         */
    char        dep_version[VERSION_LEN+1];     /* 部署共享内存版本号       */
    char        par_version[VERSION_LEN+1];     /* 参数共享内存版本号       */ 
} BCLErrMsg;

#endif
