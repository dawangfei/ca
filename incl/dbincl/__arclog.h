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

#define CATFILENAME       "errmsg"		/*��־��Ϣ�ļ����� */

#define ERRFILENAME       "errlogmd"            /* ��־ģ����־��Ϣ�ļ����� */
#define DESCPFILE         "errdef"              /* ��־˵�������ļ�����     */

#define DEFAULT_LOCAL     "zh_cn.gb"            /* ȱʡ����                 */
#define DSP_MUN           3                     /* ��־��Ϣ��������         */
#define ERR_MSG_LEN       512                   /* ���err_msg�ļ���С      */
#define FMT_LEN           512                   /* ��ʽ���ȴ�С             */

/* add by qiuyp on 09/17/2001 */
/* add begin */
#define MAX_LOG_SIZE      2000000               /* ��־Ĭ�ϵ����Size       */
/* add end   */

typedef struct {
    int         appcode;                        /* Ӧ�ö��������           */
    int         syscode;                        /* unix (errno)             */
    pid_t       p_id;                           /* ���̺�                   */
    char        *file;                          /* ��־�ļ�����             */
    int         line;                           /* ��־�ļ�������           */
    short       level;                          /* ��Ƕ�׵ļ���             */
    char        appmsg[ERR_MSG_LEN];            /* Ӧ�ö��������Ϣ         */
    char        dep_version[VERSION_LEN+1];     /* �������ڴ�汾��       */
    char        par_version[VERSION_LEN+1];     /* ���������ڴ�汾��       */ 
} BCLErrMsg;

#endif
