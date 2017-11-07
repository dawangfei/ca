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
 *    ProgramName : __arcbcl.h
 *    SystemName  : Starring
 *    Version     : 3.0
 *    Language    : C
 *    OS & Env    : RedHat Linux 8.x
                    Informix  7.30.U
 *    Description : Base function module define
 *    History     : 
 *    YYYY/MM/DD	Position	Author		Description
 *    -------------------------------------------------------------
 *    2003/07/15	BeiJing		Handq		Creation
 */
#ifndef ____ARCBCL_H_
#define ____ARCBCL_H_

#include "sysdef.h"

/* String */
#define ADJ_LEFT    'L'
#ifndef MANI_LA
#define MANI_LA         ADJ_LEFT
#endif

#define ADJ_RIGHT   'R'
#ifndef MANI_RA
#define MANI_RA         ADJ_RIGHT
#endif

#define ADJ_CENTER  'C'

/* fork */
typedef int (*USERFORKFUNC) _(( void ));
typedef int (*USERFORKFUNC_P) _(( void * ));

/* signal */
typedef int (*SIGALRMTRAP) _(( void ));
typedef int (*SIGCLDTRAP) _(( pid_t, int ));
typedef int (*SIGTERMTRAP) _(( void ));
typedef int (*SIGUSR1TRAP) _(( void ));
typedef int (*SIGUSR2TRAP) _(( void ));
typedef void (*Sigfunc) _((int));

/* LINK */
typedef int (*USERLINKFUNC) _((void *));
typedef int (*LINKCMPFUNC) _((const void *, const void *));

typedef struct _linknode LINKNODE;
struct _linknode {
    size_t      dsize;
    void        *data;
    LINKNODE    *next;
};

typedef struct  _linkhdr {
    u_int       nodenum;
    LINKNODE    *first;
/* 20051027 ���ٷ���calloc/free */
    u_int       node_len; /* �Ѿ�����Ľڵ��� */
    LINKNODE   *node_buf; /* �Ѿ�����Ľڵ� */
    u_int       buf_len;  /* �Ѿ������BUFFER */
    unsigned char *buf;   /* ���� */
    u_int       use_len;  /* �Ѿ�ʹ�õ�BUFFER */
} LINK;

/* SOCKET */
#define SS_BROKEN       0X01
#define SS_TIMEDOUT     0X02
#define SS_READABLE     0X03
#define SS_WRITEABLE    0X04

#ifndef IFNAMSIZ
#define IFNAMSIZ        16
#endif

typedef struct __ifinfo {
    char                if_name[IFNAMSIZ];
    struct in_addr      if_iaddr;
    struct in_addr      if_nmask;
} IFINFO;

#ifdef __cplusplus
extern "C" {
#endif

/* DTA������ַ��Ϣ */
typedef struct {
    int                  lsfd;                /* ����������*/
    struct sockaddr_in   saddr;               /* ��ַ */
    int                  max_conn;            /* ���������Server DTAʹ�� */
    int                  cur_conn;            /*  ��ǰ������*/
} tGWDLSN;

typedef struct {
    short              conn;       /* 0 - short  1 - long */
    int                pnum;       /* ������ */
    int                min_pnum;   /* ��С������ */
    int                max_pnum;   /* �������� */
    int                add_num ;   /* ������̬�����Ķ��������� */
    int                lsn_num ;   /* ������ַ���� */
    tGWDLSN            *gwd_lsn; /* DTA�����б� */
} tGWDCONF;

typedef struct {
    short              conn;       /* 0 - short  1 - long */
    int                pnum;       /* ������ */
    int                min_pnum;   /* ��С������ */
    int                max_pnum;   /* �������� */
    int                lsn_num ;   /* ������ַ���� */
    int                rcv_inst;  /* ����ʵ���� */
    tGWDLSN            *gwd_lsn; /* DTA�����б� */
} tGWDASYNCONF;


#ifdef __cplusplus
}
#endif

/* Time */
typedef struct {
    char    year[5];
    char    month[3];
    char    day[3];
    char    hour[3];
    char    minute[3];
    char    second[3];
}tTimeStru;

/* NSBUF */
#define READPTR(ns)     (ns)->rd_ptr.pp
#define READPOOL(ns)    (ns)->rd_ptr.pp->pool
#define READOFFSET(ns)  (ns)->rd_ptr.off
#define WRITEPTR(ns)    (ns)->wr_ptr.pp
#define WRITEPOOL(ns)   (ns)->wr_ptr.pp->pool
#define WRITEOFFSET(ns) (ns)->wr_ptr.off


/* IO buffer pool size */
#define POOLSIZE        1024

/* IO buffer */
struct nsbuf_pool {
    char                pool[POOLSIZE]; /* buffer pool */
    struct nsbuf_pool   *last;          /* pointer to last pool */
    struct nsbuf_pool   *next;          /* pointer to next pool */
};


/* IO buffer read/write pointer */
struct rdwr_ptr {
    struct nsbuf_pool   *pp;    /* buffer pool */
    int                 off;    /* offset in pool */
};


/* IO buffer */
struct nsbuf {
    int                 pool_num;       /* total pool number in buffer */
    struct nsbuf_pool   *f_pool;        /* first pool */
    struct rdwr_ptr     rd_ptr;         /* read pointer */
    struct rdwr_ptr     wr_ptr;         /* write pointer */
};

typedef struct nsbuf NSBUF;


#define TCPSRCASYN_REQCONN                  0 
#define TCPSRCASYN_RETCONN                  1 
#define TCPSRCASYN_RETINFO                  3
#define TCPSRCASYN_DISCONN                  4


typedef struct {
    int action;        /* ���� 0.�������� 1.�黹���� 3.��������� 
                               4.����ʧ����Ҫ�Ͽ�����*/
    int value;         /*���ڹ黹���ӣ������ŵĽ���������±�ID*/
    time_t time_stamp; /* �Ǽǽ�����ALA����ʱ��ĵ�ʱ��� */
    char tran_id[64];  /* ��ʾһ������Ψһ��ЧID      */
    char svcname[64];  /*     ������Ҳ��Ҫ������ :(   */
    int time_out;      /* �ý��׵ĳ�ʱʱ��            */
}tGWDDATAUNIT;



/* hash �㷨��Ч�� 4 2 3 1 5 6 n */
/* ����ֻ��BUFFER�Ļ���CACHE,���ٱ����¼����,������߲�ѯ�ٶ� */
/* ������ļ�¼ �ٴ�������,��֤����λ��û�г�ͻ,Ȼ��ֱ�Ӷ�ַ,�ӿ������ٶ� */
/* û��cache�ļ�¼,����bsearch */
#define MAXHASHCACHENUM 8

typedef struct {
    int c_num;                 /* 1-8 �������, 0��Ϊ3 */
    int h_alog;                /* hash�㷨��0Ĭ��Ϊ4 */
    unsigned long init_csize;  /* ����ĳ�ʼ��Ͱ��, 0��Ϊϵͳ�Զ� 
                    89 97 997 9973 99991 999983 9999991 99999989 ǧ�򼶱�
                                  ʹ������ �������ĸ���СЩ */
    unsigned long max_csize;   /* ÿ�黺������Ͱ��,0ϵͳ�Զ� */
    int ext_pre;               /* 1-99��չ����,0Ϊ75 */
    unsigned long buf_num;     /* �軺���BUFFER��¼���� */
    int key_size;              /* �軺���BUFFER��key�ֶ�,ÿ��ռ�пռ��С */
    char * key;                /* �軺���BUFFER��key�ֶ�����ָ�� */
    int offset;                /* key�ṹ�е�key�ֶ�ƫ��,0Ĭ��ֵ */

    char * key_stat;                        /* BUFFER�ж�Ӧλ�ü�¼�Ƿ񻺴��� */
                                            /* 1������ 0δ���� */
    unsigned long key_cached;               /* �������¼�� */
    unsigned long sizen[MAXHASHCACHENUM];   /* ����n�Ĵ�С */
    unsigned long *cachen[MAXHASHCACHENUM]; /* ����n����, ֵΪ���ʾû�л���
                                  ����hashn��ֵ��λ,�Ǽ�BUF��Ӧλ��ֵ,1��ʼ
                                  ����ֻ��Ҫc_num/sizen/cachen�����ֶ� */
    unsigned long *putn[MAXHASHCACHENUM];   /* n�ĸ�λ�����и��� */
    unsigned long numn[MAXHASHCACHENUM];    /* ����n��ʹ����,
                                  ������չ����,���󻺴�,����,��ճ�ͻʱ-- */
}tHASHCACHEDEF;

typedef struct {
    char key_stat;             /* Ͱ��Ӧλ���Ƿ�ʹ�� */
                               /* 1��ʹ�� 0δʹ�� */
    char *pkey;                /* key�ڴ�ָ�� */
    void *pval;                /* ��¼�ڴ�ָ�� */
    unsigned long hid;         /* ���key��hash id,�������λ��,�ж���ͬkey */
    unsigned long tch;         /* key��get�Ĵ��� */
    char lru_flag;             /* 0��lru������,1 2 3...��mru������ */
    void *prev;                /* �ϸ��ڵ� */
    void *next;                /* �¸��ڵ� */
}tHASHTABLEREC;

/* put ��key�ŵ�ǰlru���lru_head */
/* get ��һ��¼��tch�ֶ� */
/* recycle ��lru_tail,tch>def_lru������mru,���²�lru������head */
/*         ѭ������,ֱ��<=def_lru�ļ�¼���㹻,���һ��mruֱ��������룰�� */
/*         �������󣬼��lru�㣬���û�м�¼�������²�Ϊlru�� */
/* remove ��Ҫ������ժ���ýڵ� */
typedef struct {
    unsigned long lru_num;     /* lru�����¼�� */
    void *lru_head;            /* �������ʹ�ü�¼��ָ������head��tail�� */
    void *lru_tail;
}tLRULINKDEF;

/* HASH TABLEģʽ���keyֵΨһ�ļ�¼,��дģʽ,�ڲ������˼�¼���� */
/* ʹ��hash��λ��˳��+Ԥ�����ͻ���� */
/* ����������Ͱ������,�Զ�����get������С�ļ�¼ */
/* key�ͼ�¼���г�������, Ͱ��������Ҫ����̫�� */
typedef struct {
    int h_alog;                /* hash�㷨��0Ĭ��Ϊ4 */
    unsigned long init_tsize;  /* ��ʼ��Ͱ��, 0��Ϊϵͳ�Զ� 997 */
    unsigned long max_tsize;   /* ���Ͱ��,0ϵͳ�Զ� */
    int ext_pre;               /* 1-99��չ����,0Ϊ75 */
    int key_size;              /* key�ռ��С ���� */
    int val_size;              /* ��¼�ռ��С ���� */
    char same_id;              /* ȫ������Ƿ��п��ܳ���hid��ͬ */
                               /* 0-���� */
                               /* 1-������,���Ա���ÿ����key�ַ����ȶ� */
    char auto_rcycl;           /* �Ƿ��Զ��������ʹ�ô������ٵļ�¼ */
                               /* 0-�Զ� */
                               /* 1-�ֹ�����bcl_htable_recycle */
    char def_lru;              /* Ĭ��LRU��Ϊ����С���� 0��Ϊ2 */
    char link_size;            /* ˫��������� 0��Ϊ3,���������ֱ�Ϊlru/mru */

    unsigned long size;        /* Ͱ���� */
    unsigned long key_num;     /* ��¼�� */
    tHASHTABLEREC *key_rec;    /* key��¼���� */
    char *key;                 /* ���key�ڴ� */
    void *val;                 /* ��ż�¼�ڴ� */
    tLRULINKDEF *lru;          /* ���LRU�������� */
    char lru_beg;              /* lru����ʼ�㣬��0��ʼ����link_size����� */
}tHASHTABLEDEF;

#endif /* end of __arcbcl.h */
