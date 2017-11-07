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
 *    FILE:           __arcde.h
 *    DESCRIPTION:    data element managing header file 
 *    AUTHOR:
 *              Dongjm 03/27/2001 - Developed for eSWITCH system
 *    MODIFIED:
 *
 */

#ifndef ____ARCDE_H
#define ____ARCDE_H

#include "sysdef.h"
#include "__arcbcl.h"

/* 20051027 ���ӻ���ELNODE��֧�֣�Ŀǰ��FORMAT/FORM/DEMAP���ֽṹ���� */
#define MAXELNBUF      128            /* ��󻺳���� */

/*
 * ��������Ԫ������
 */
#define  SYS_CONST    1                /* ϵͳ���� */
#define  SYS_VAR      2                /* ϵͳ���� */
#define  APP_CONST    3                /* Ӧ�ó��� */
#define  APP_VAR      4                /* Ӧ�ñ��� */

/*
 * ������������
 */
#define  DNULL       -1
#define  DVOID        0
#define  DSTRING      1
#define  DSHORT       2
#define  DINT         3
#define  DLONG        4
#define  DFLOAT       5
#define  DDOUBLE      6
#define  DBINARY      7
#define  DANYVALUE    8
#define  DCHAR        9 
#define  DPOINTER     10
#define  DARRAY       11

/*
 * ���ʽ����֧��
 */
#define  DVOID        0
#define  DNULL       -1
#define  DCHAR        9

/*
 * data to buf type definition
 */
#define  ALL_EL_AND_DEF    1   /* ת����������Ԫ�ز���������Ԫ�صĶ��� */
#define  ALL_EL_NO_DEF     2   /* ת����������Ԫ�ز���������Ԫ�صĶ��� */
#define  DAT_EL_AND_DEF    3   /* ת�������ݵ�����Ԫ�ز���������Ԫ�صĶ��� */
#define  DAT_EL_NO_DEF     4   /* ת�������ݵ�����Ԫ�ز���������Ԫ�صĶ��� */

/*
 * ����Ԫ�ز�����¼��
 */
#define  MAXOPERST        20   /* ��������ζ�ջ���� */
#define  ELGETOPER        1    /* ȡ������ */
#define  ELPUTOPER        2    /* �������� */
#define  ELINSOPER        3    /* ���������� */
#define  ELDELOPER        4    /* ɾ�������� */

/*
 * �������Ԫ�ز�����־
 */
#define  NOEXTMETA        0   /* δʹ���������Ԫ�� */
#define  EXTMETA          1   /* ʹ���������Ԫ�� */

extern   int  eperrno;
/*
 * eperrno definition move to sysloc.h
 */

/*
 * element struct definition
 */
#define DEFTLEN     64 /* 20051027 255 -> 64 */
typedef struct {
    /* ͨ��el_name�����ٶ�������Ϊ��el_id���� */
    unsigned long el_id;                   /* ����Ԫ�ر�ʶ,
                                              ��bcl_hash(el_name)��� */
    /* WangNan 2002/09/16 */
    char     el_name[ NAMELEN + 1 ];       /* ����Ԫ���� */
    char     el_desc[ DESCLEN + 1 ];       /* ����Ԫ������ */
    int      el_type;                      /* ����Ԫ������ */
    int      da_type;                      /* �������� */
    int      da_typesize;		   /* ���������͵ĵ�λ���� */
    int      da_dec;                       /* ����С��λ,��DFLOAT 
                                              �� DDOUBLE ��Ч */
    int      max_len;                      /* ���洢���� */
    char     da_deft[ DEFTLEN+1 ];         /* ȱʡֵ */
} DATAELEM;
typedef DATAELEM tDATAELEM;

/*
 * ����Ԫ�ļ������ԣ���װ�أ����ɹ����ڴ���DTA��������Ԫ������
 */
#define DECOL_PLAT 1
#define DECOL_DTA  2
#define DECOL_BUSI 3
#define DECOL_SUB  4
#define DECOL_PRJ  5    /* ��Ŀ��������Ԫ�� */
typedef struct {
    unsigned long el_id;                   /* ����Ԫ�ر�ʶ */
    char     el_name[ NAMELEN + 1 ];       /* ����Ԫ���� */
    short    col_type;                     /* ��������--1ƽ̨��
                                                        2DTA��
                                                        3ҵ��
                                                        4��ҵ�� */
    char     col_name[ NAMELEN + 1];       /* �������� */
} tDECOL;

typedef struct {
/* deleting unuseless member : val_id  ,and adding member :alloc_size 
 * by zhaokun for mem optimaztion 20060704 
 */
    int        alloc_size;
    int        val_len;
    void     * el_val;
} VALREC;
typedef VALREC tVALREC;

typedef struct {
    /* Added by WangNan */
    unsigned   long el_id;               /* ͬDATAELEM�е�el_id */
    /* 2002/09/16 */
    char       el_name[ NAMELEN + 1 ];
    int        idx;                      /* ����Ԫ��С�� 0 ��ʼ */
    DATAELEM * da_el;
    int        el_free;                  /* 1����Ԫ�ض�����Ҫ�ͷ� */
    int        val_num;
/* Added by JiangLing */
    int        val_alloc;                /* ��¼�ڴ���val_rec������ռ�Ĵ��� */
/* 2004/08/20 */
    VALREC   * val_rec;
/* 20051027 Ϊep_put_value�е���_ep_rec_usedel���� */
    int        ep_id;
/* 20060530 Ϊˢ�£��Ǽ��Ƿ�ʹ�ñ�־��ˢ�±����غ󣬻ָ���EPUSEDEL�� */
    int        el_use;                   /* 1������Ԫ�ر�ʹ�� */
    short      cpt_in;                 /* ����Ԫ�Ƿ���Ԫ�������б��־ 0�� 1��*/
    short      cpt_out;                /* ����Ԫ�Ƿ���Ԫ������б��־ 0�� 1��*/
    short      cpt_private;            /* ����Ԫ�Ƿ���Ԫ���ֲ������б� 0�� 1��*/
} ELNODE;
typedef ELNODE tELNODE;

/* ����Ԫ�ز�����¼ */
typedef struct {
    int                   ep_id;              /* ����Ԫ�سر�ʶ */
    unsigned long         el_id;              /* ����Ԫ�ر�ʶ */
    char                  el_name[NAMELEN+1]; /* ����Ԫ������ */
    int                   idx;                /* ����Ԫ���±� */
    int                   da_type;            /* �������� */
    short                 oper_typ;           /* ����Ԫ�ز�������
                                                1 - Get���� 2 - Put���� */
    tVALREC               oper_val;           /* ����Ԫ��ֵ */
} tOPERREC;

/* ����Ԫ�ز�����¼���� */
typedef struct {
    int                   num;               /* ����Ԫ�ز������� */
    int                   recnum;            /* �ռ���� */
    tOPERREC              *operrec;          /* ����Ԫ�ز�����¼ */
}tELEMOPER;

/* Global variable definition */
typedef struct {
    int        ep_id;
    int        el_num;
    long       version;  /* 20100107 ��Ӧ�������ڴ�汾 */
    long       verid;                   /* 20101020 ��Ӧ�汾��hashid */
    short      ep_type; /* 20061207 ���������� 0-��ˢ��ep_allocd
                                               1-����ep_allocf */
    ELNODE   * enp;
    tHASHCACHEDEF enp_cache;            /* �����enp���鴴��CACHE�� */
    int        usednum;                 /* ʹ�õ�����Ԫ�ظ��� */
    int       *usedel;                  /* ʹ�õ�����Ԫ��λ�õǼ� */
    int        allocnum;                /* ʹ�õ�����Ԫ�ط���ռ� */
} ELPOOL;
 
typedef struct {
    int        ep_num;
    int        max_id;
    ELPOOL   * eps;
} EPMNG;

#endif
