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

/* 20051027 增加缓冲ELNODE的支持，目前对FORMAT/FORM/DEMAP三种结构缓冲 */
#define MAXELNBUF      128            /* 最大缓冲个数 */

/*
 * 定义数据元素类型
 */
#define  SYS_CONST    1                /* 系统常量 */
#define  SYS_VAR      2                /* 系统变量 */
#define  APP_CONST    3                /* 应用常量 */
#define  APP_VAR      4                /* 应用变量 */

/*
 * 定义数据类型
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
 * 表达式类型支持
 */
#define  DVOID        0
#define  DNULL       -1
#define  DCHAR        9

/*
 * data to buf type definition
 */
#define  ALL_EL_AND_DEF    1   /* 转换所有数据元素并包括数据元素的定义 */
#define  ALL_EL_NO_DEF     2   /* 转换所有数据元素不包括数据元素的定义 */
#define  DAT_EL_AND_DEF    3   /* 转换有数据的数据元素并包括数据元素的定义 */
#define  DAT_EL_NO_DEF     4   /* 转换有数据的数据元素不包括数据元素的定义 */

/*
 * 数据元素操作记录宏
 */
#define  MAXOPERST        20   /* 最大操作层次堆栈容量 */
#define  ELGETOPER        1    /* 取数操作 */
#define  ELPUTOPER        2    /* 存数操作 */
#define  ELINSOPER        3    /* 插入数操作 */
#define  ELDELOPER        4    /* 删除数操作 */

/*
 * 外存数据元素操作标志
 */
#define  NOEXTMETA        0   /* 未使用外存数据元素 */
#define  EXTMETA          1   /* 使用外存数据元素 */

extern   int  eperrno;
/*
 * eperrno definition move to sysloc.h
 */

/*
 * element struct definition
 */
#define DEFTLEN     64 /* 20051027 255 -> 64 */
typedef struct {
    /* 通过el_name查找速度慢，改为用el_id查找 */
    unsigned long el_id;                   /* 数据元素标识,
                                              由bcl_hash(el_name)算出 */
    /* WangNan 2002/09/16 */
    char     el_name[ NAMELEN + 1 ];       /* 数据元素名 */
    char     el_desc[ DESCLEN + 1 ];       /* 数据元素描述 */
    int      el_type;                      /* 数据元素类型 */
    int      da_type;                      /* 数据类型 */
    int      da_typesize;		   /* 此数据类型的单位长度 */
    int      da_dec;                       /* 数据小数位,对DFLOAT 
                                              和 DDOUBLE 有效 */
    int      max_len;                      /* 最大存储长度 */
    char     da_deft[ DEFTLEN+1 ];         /* 缺省值 */
} DATAELEM;
typedef DATAELEM tDATAELEM;

/*
 * 数据元的集合属性，不装载，生成共享内存中DTA访问数据元集合用
 */
#define DECOL_PLAT 1
#define DECOL_DTA  2
#define DECOL_BUSI 3
#define DECOL_SUB  4
#define DECOL_PRJ  5    /* 项目公用数据元素 */
typedef struct {
    unsigned long el_id;                   /* 数据元素标识 */
    char     el_name[ NAMELEN + 1 ];       /* 数据元素名 */
    short    col_type;                     /* 集合类型--1平台、
                                                        2DTA、
                                                        3业务、
                                                        4子业务 */
    char     col_name[ NAMELEN + 1];       /* 集合名称 */
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
    unsigned   long el_id;               /* 同DATAELEM中的el_id */
    /* 2002/09/16 */
    char       el_name[ NAMELEN + 1 ];
    int        idx;                      /* 数据元素小标 0 开始 */
    DATAELEM * da_el;
    int        el_free;                  /* 1数据元素定义需要释放 */
    int        val_num;
/* Added by JiangLing */
    int        val_alloc;                /* 记录内存中val_rec被分配空间的次数 */
/* 2004/08/20 */
    VALREC   * val_rec;
/* 20051027 为ep_put_value中调用_ep_rec_usedel函数 */
    int        ep_id;
/* 20060530 为刷新，登记是否使用标志，刷新变量池后，恢复出EPUSEDEL用 */
    int        el_use;                   /* 1该数据元素被使用 */
    short      cpt_in;                 /* 数据元是否在元件输入列表标志 0否 1是*/
    short      cpt_out;                /* 数据元是否在元件输出列表标志 0否 1是*/
    short      cpt_private;            /* 数据元是否在元件局部数据列表 0否 1是*/
} ELNODE;
typedef ELNODE tELNODE;

/* 数据元素操作记录 */
typedef struct {
    int                   ep_id;              /* 数据元素池标识 */
    unsigned long         el_id;              /* 数据元素标识 */
    char                  el_name[NAMELEN+1]; /* 数据元素名称 */
    int                   idx;                /* 数据元素下标 */
    int                   da_type;            /* 数据类型 */
    short                 oper_typ;           /* 数据元素操作类型
                                                1 - Get操作 2 - Put操作 */
    tVALREC               oper_val;           /* 数据元素值 */
} tOPERREC;

/* 数据元素操作记录集合 */
typedef struct {
    int                   num;               /* 数据元素操作个数 */
    int                   recnum;            /* 空间个数 */
    tOPERREC              *operrec;          /* 数据元素操作记录 */
}tELEMOPER;

/* Global variable definition */
typedef struct {
    int        ep_id;
    int        el_num;
    long       version;  /* 20100107 对应部署共享内存版本 */
    long       verid;                   /* 20101020 对应版本号hashid */
    short      ep_type; /* 20061207 变量池类型 0-不刷新ep_allocd
                                               1-正常ep_allocf */
    ELNODE   * enp;
    tHASHCACHEDEF enp_cache;            /* 排序的enp数组创建CACHE块 */
    int        usednum;                 /* 使用的数据元素个数 */
    int       *usedel;                  /* 使用的数据元素位置登记 */
    int        allocnum;                /* 使用的数据元素分配空间 */
} ELPOOL;
 
typedef struct {
    int        ep_num;
    int        max_id;
    ELPOOL   * eps;
} EPMNG;

#endif
