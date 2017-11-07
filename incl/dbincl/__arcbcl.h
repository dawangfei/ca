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
/* 20051027 减少反复calloc/free */
    u_int       node_len; /* 已经分配的节点数 */
    LINKNODE   *node_buf; /* 已经分配的节点 */
    u_int       buf_len;  /* 已经分配的BUFFER */
    unsigned char *buf;   /* 缓冲 */
    u_int       use_len;  /* 已经使用的BUFFER */
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

/* DTA监听地址信息 */
typedef struct {
    int                  lsfd;                /* 监听描述符*/
    struct sockaddr_in   saddr;               /* 地址 */
    int                  max_conn;            /* 最大连接数Server DTA使用 */
    int                  cur_conn;            /*  当前连接数*/
} tGWDLSN;

typedef struct {
    short              conn;       /* 0 - short  1 - long */
    int                pnum;       /* 进程数 */
    int                min_pnum;   /* 最小进程数 */
    int                max_pnum;   /* 最大进程数 */
    int                add_num ;   /* 触发动态伸缩的堵塞连接数 */
    int                lsn_num ;   /* 监听地址个数 */
    tGWDLSN            *gwd_lsn; /* DTA监听列表 */
} tGWDCONF;

typedef struct {
    short              conn;       /* 0 - short  1 - long */
    int                pnum;       /* 进程数 */
    int                min_pnum;   /* 最小进程数 */
    int                max_pnum;   /* 最大进程数 */
    int                lsn_num ;   /* 监听地址个数 */
    int                rcv_inst;  /* 接收实例数 */
    tGWDLSN            *gwd_lsn; /* DTA监听列表 */
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
    int action;        /* 动作 0.请求连接 1.归还连接 3.请求处理完毕 
                               4.处理失败需要断开连接*/
    int value;         /*对于归还连接，此理存放的进程数组的下标ID*/
    time_t time_stamp; /* 登记交易向ALA发送时候的的时间戳 */
    char tran_id[64];  /* 标示一个交易唯一有效ID      */
    char svcname[64];  /*     服务名也是要保留的 :(   */
    int time_out;      /* 该交易的超时时间            */
}tGWDDATAUNIT;



/* hash 算法的效率 4 2 3 1 5 6 n */
/* 创建只读BUFFER的缓存CACHE,不再保存记录内容,可以提高查询速度 */
/* 对排序的记录 再创建缓存,保证缓存位置没有冲突,然后直接定址,加快搜索速度 */
/* 没有cache的记录,再走bsearch */
#define MAXHASHCACHENUM 8

typedef struct {
    int c_num;                 /* 1-8 缓存块数, 0设为3 */
    int h_alog;                /* hash算法，0默认为4 */
    unsigned long init_csize;  /* 缓存的初始化桶数, 0设为系统自动 
                    89 97 997 9973 99991 999983 9999991 99999989 千万级别
                                  使用质数 被整除的概率小些 */
    unsigned long max_csize;   /* 每块缓存的最大桶数,0系统自动 */
    int ext_pre;               /* 1-99扩展因子,0为75 */
    unsigned long buf_num;     /* 需缓存的BUFFER记录个数 */
    int key_size;              /* 需缓存的BUFFER的key字段,每个占有空间大小 */
    char * key;                /* 需缓存的BUFFER的key字段数组指针 */
    int offset;                /* key结构中的key字段偏移,0默认值 */

    char * key_stat;                        /* BUFFER中对应位置记录是否缓存了 */
                                            /* 1被缓冲 0未缓冲 */
    unsigned long key_cached;               /* 被缓冲记录数 */
    unsigned long sizen[MAXHASHCACHENUM];   /* 缓存n的大小 */
    unsigned long *cachen[MAXHASHCACHENUM]; /* 缓存n数组, 值为零表示没有缓存
                                  根据hashn的值定位,登记BUF对应位置值,1开始
                                  查找只需要c_num/sizen/cachen这三字段 */
    unsigned long *putn[MAXHASHCACHENUM];   /* n的各位置命中个数 */
    unsigned long numn[MAXHASHCACHENUM];    /* 缓存n的使用数,
                                  超过扩展因子,扩大缓存,重算,清空冲突时-- */
}tHASHCACHEDEF;

typedef struct {
    char key_stat;             /* 桶对应位置是否被使用 */
                               /* 1被使用 0未使用 */
    char *pkey;                /* key内存指针 */
    void *pval;                /* 记录内存指针 */
    unsigned long hid;         /* 存放key的hash id,决定存放位置,判断相同key */
    unsigned long tch;         /* key被get的次数 */
    char lru_flag;             /* 0在lru链表上,1 2 3...在mru链表上 */
    void *prev;                /* 上个节点 */
    void *next;                /* 下个节点 */
}tHASHTABLEREC;

/* put 新key放当前lru层的lru_head */
/* get 加一记录的tch字段 */
/* recycle 从lru_tail,tch>def_lru则减半进mru,即下层lru的链表head */
/*         循环处理,直到<=def_lru的记录数足够,最后一层mru直接清零进入０层 */
/*         完成清理后，检查lru层，如果没有记录，设置下层为lru层 */
/* remove 需要从链表摘除该节点 */
typedef struct {
    unsigned long lru_num;     /* lru链表记录数 */
    void *lru_head;            /* 最近最少使用记录的指针链表，head热tail冷 */
    void *lru_tail;
}tLRULINKDEF;

/* HASH TABLE模式存放key值唯一的记录,读写模式,内部保存了记录内容 */
/* 使用hash定位和顺延+预设防冲突策略 */
/* 如果到达最大桶数限制,自动清理get次数最小的记录 */
/* key和记录都有长度限制, 桶个数不需要超出太大 */
typedef struct {
    int h_alog;                /* hash算法，0默认为4 */
    unsigned long init_tsize;  /* 初始化桶数, 0设为系统自动 997 */
    unsigned long max_tsize;   /* 最大桶数,0系统自动 */
    int ext_pre;               /* 1-99扩展因子,0为75 */
    int key_size;              /* key空间大小 必填 */
    int val_size;              /* 记录空间大小 必填 */
    char same_id;              /* 全局情况是否有可能出现hid相同 */
                               /* 0-可能 */
                               /* 1-不可能,可以避免每次与key字符串比对 */
    char auto_rcycl;           /* 是否自动清理最近使用次数最少的记录 */
                               /* 0-自动 */
                               /* 1-手工调用bcl_htable_recycle */
    char def_lru;              /* 默认LRU认为的最小次数 0设为2 */
    char link_size;            /* 双向链表个数 0设为3,最少两个分别为lru/mru */

    unsigned long size;        /* 桶个数 */
    unsigned long key_num;     /* 记录数 */
    tHASHTABLEREC *key_rec;    /* key记录内容 */
    char *key;                 /* 存放key内存 */
    void *val;                 /* 存放记录内存 */
    tLRULINKDEF *lru;          /* 存放LRU链表数组 */
    char lru_beg;              /* lru的起始层，从0开始，到link_size后回绕 */
}tHASHTABLEDEF;

#endif /* end of __arcbcl.h */
