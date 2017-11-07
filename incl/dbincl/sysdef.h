/* $Id:
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
 *    FILE:           sysdef.h
 *    DESCRIPTION:    系统宏/结构定义
 *    AUTHOR:
 *              NaWei 07/10/2003 - Developed for eSWITCH system
 *    MODIFIED:
 *
 */

#ifndef __SYSDEF_H
#define __SYSDEF_H


#define   E_SHMREF       -4     /* 读DRQ对列超时       */
#define   E_DRQTIMEDOUT  -3     /* 读DRQ对列超时       */
#define   E_NOTLOC       -2     /* 不是本机,为监控增加 */
#define   E_FAIL         -1
#define   E_OK            0
#define   E_BUSY          1

/* 进程根据标志判断进程类型，取错误处理设置 */
#define   SYS_PROC_PLAT        0    /* 平台进程 */
#define   SYS_PROC_DTA         1    /* DTA进程 */
#define   SYS_PROC_ALA         2    /* ALA进程 */

/*
 * 定义是非标志,用于定制信息,即:当定制项为单选或复选时,选中为是,否则为否
 */
#define   SYS_YES               1      /* 是 */
#define   SYS_NO                2      /* 非 */
#define   SYS_INUSE             3      /* 正在使用 */
#define   SYS_UNUSED            4      /* 未被使用 */
#define   SYS_RCYCL             5      /* 回收 */

#define   SYS_DELAY             3      /* 日志延迟写 */

#define   XML_SYS_YES           "YES"  /* 是 */
#define   XML_SYS_NO            "NO"   /* 非 */
#define   XML_SYS_DELAY         "DELAY"   /* 延迟写 */

#define         SHM_YES         'Y'     /* 确定 */
#define         SHM_NO          'N'     /* 否定 */

#define   NAMELEN    50   /* 名称长度(由128改为51) */
#define   IPADDR_LEN  NAMELEN /* IP地址长度 */
#define   EXPRLEN    512  /* 表达式长度(可以由255改成128) 20051014->512 */
#define   DESCLEN    60   /* 描述字段长度 */
#define   SVCLEN     NAMELEN   /* 最大服务名长度 20060614 20 -> 50 */
#define   MESGLEN    128  /* 信息/参数长度 */
#define   MSESSLEN   20   /* 最大会话号长度 */
#define   PLATINTLEN 11   /* 20050108固定流水号的长度，不同字节的机器多机交换 */
#define   SEQNOLEN   (PLATINTLEN)*2   /* 最大 system seqno 长度 */
#define   MAXLEN     2048 /* 字符串最大长度 */

#define   FLSTLEN    256  /* 文件名称长度 */
#define   STATLEN    512  /* 交易传输过程状态记录 */
#define   MAXSTNUM   10   /* 最大堆栈容量 */

#define   TRANDATELEN 10   /* 时间长度yyyymmhh */
#define   TRANTIMELEN 8   /* 时间长度hhmmss */
#define   VERSION_LEN 20  /* 共享内存版本号长度 */
#define   DATETIMELEN 19  /* YYYY/MM/DD hh:mm:ss */

/*
 * 定义日志文件类型
 */
#define		EXTCMLOG	1	/* 外部通讯跟踪日志 */
#define		FMTLOG		2	/* 报文格式跟踪日志 */
#define		EVTLOG		3	/* 事件跟踪日志 */
#define		RUTLOG		4	/* 路由跟踪日志 */
#define		EXPRLOG		5	/* 表达式跟踪日志 */
#define		FLWLOG		6	/* 流程跟踪日志 */
#define		RVSLOG		7	/* 自动冲正跟踪日志 */
#define		DBLOG		8	/* 数据库封装跟踪日志 */
#define		PRESSLOG	9	/* 压力测试日志 */

/*
 * 日志等级定义
 */
#define     LOG_ERROR    1   /* 错误 */
#define     LOG_WARN     2  /* 警告 */
#define     LOG_INFO     3  /* 信息 */
#define     LOG_DEBUG    4  /* 调试 */
#define     LOG_TRACE    5  /* 跟踪 */

/*
 * 定义同/异步宏定义
 */
#define         SHM_SYNC        1       /* 同步 */
#define         SHM_ASYNC       2       /* 异步 */

/*
 * 定义DRQ头服务类型宏定义
 */
#define         SVC_NORMAL      0       /* 正常交易 */
#define         SVC_REVS        1       /* 冲正交易 */
#define         SVC_ASYNC       2       /* 异步DTA冲正 */
#define         SVC_COMPST      3       /* 补偿交易 */
#define         SVC_COMPNT      4       /* 元件 */

#define MCH_NAMELIST_LEN 128      /* 机器名对照表长度 */
#define DTA_NAMELIST_LEN 128      /* DTA名对照表长度 */

/*
 * 队列头结构状态
 */
#define     REQNOSTAT        1     /* 初始状态 */
#define     REQSENDOK        2     /* 发送成功 */
#define     REQRECVOK        3     /* 响应成功 */

typedef enum {
    DRQACK_DEFAULT = 0,                /* 正常响应 */
    DRQACK_NO,                  /* 不需要响应 */
    DRQACK_ASYN                       /* 两次响应,第一次在收到请求后响应,第二次在处理完成后响应 */
}DrqAckType;

typedef struct {         /* Message struct of drq msg */
    char            svcname[SVCLEN+1];      /* service name */
    char            dsvcname[SVCLEN+1];     /* 通过指定该值,获得目的方服务名 */
    short           dsvcflag;               /* 目的服务类型,0正常交易,1冲正交易 */
    char            filelst[FLSTLEN+1];     /* file list 多个文件以+号分隔 */
    char            sdtaname[NAMELEN+1];    /* source dta name */
    char            smachnname[NAMELEN+1];  /* source machn name */
    int             sinstid;                /* source dta inst id */
    char            ddtaname[NAMELEN+1];    /* destination dta name */
    char            dmachnname[NAMELEN+1];  /* destination machn name */
    long            reqid;                  /* request id   */
    short           qtype;                  /* Queue type*/
    short           fend;                   /* message end flag */
    short           snaact;                 /* SNA command type 
                                    0 - SNACALL 发送和接收一次完成 
                                    1 - SNASEND 一次发送，多次接收 
                                    2 - SNACONF 发送确认(不推荐使用) */
    short           fconf;                  /* 确认标志 0 - 正确认 1 - 负确认 */
    int             drqerrno;               /* Error code */
    char            errmsg[ DESCLEN + 1 ];  /* Error message */
    short           reqstat;                /* 请求状态 */
                                 /* 0:REQNOSTAT:初始状态 */
                                 /* 1:REQSENDOK:发送成功 */
                                 /* 2:REQRECVOK:响应成功 */
    short           stidx;                 /* 当前堆栈指针,初值为0 */
    char            resdtast[MAXSTNUM+1][NAMELEN+1];  /* 响应DTA堆栈 */
    char            resmchst[MAXSTNUM+1][NAMELEN+1];  /* 响应机器堆栈 */
    char            svcnamest[MAXSTNUM+1][SVCLEN+1];  /* 服务名堆栈 */
    short           retqst[MAXSTNUM+1]; /* 响应DTA接收队列 */
                             /* 100:REQ_QUEUE请求队列;200:SRV_QUEUE服务队列 */
    long            reqidst[MAXSTNUM+1];   /* 消息句柄堆栈 */
    char            session[MSESSLEN+1];   /* 会话号 */
    u_long          deviceid;              /* 请求方设备标识 */
    char            ssdtaname[NAMELEN+1];  /* 第一个source dta name */
    char            ssmachnname[NAMELEN+1];/* 第一个source machn name */
    char            sdtaseq[SEQNOLEN+1];   /* 源DTA流水号 */
    char            seqstat[STATLEN+1];    /*交易处理状态 */
                                /* 格式为 :
             本机id.本DTAid：请求机器id.请求DTA id-目标机器id.目标DTAid
                  1.1:1.1-1.2
                                */
    char            sdtadate[TRANDATELEN+1]; /*源DTA处理系统日期, 格式为：YYYYMMDD */
    double          ssvc_begtime;     /* 交易请求起始时间 */
    int             tran_regid;       /* 登记交易生成的登记区位置 */
    char            mchnamelist[MCH_NAMELIST_LEN+1];       /* 机器名和编号对照表*/
    char            dtanamelist[DTA_NAMELIST_LEN+1];       /* DTA和编号对照表*/
    short           svc_prio;         /* 交易优先级 */
    char            cust_pal_idx[NAMELEN+1]; /* 客户化条目 */
    char            svcid[NAMELEN+1]; /* 服务身份ID */
    int             time_out;         /* 源方剩余超时时间 */
    long            dep_version; /* 交易使用的部署共享内存版本 */
    long            par_version; /* 交易使用的参数共享内存版本 */
    short           ack_flag;    /* 响应标志: 0 -- 正常响应,
                                              1 - 不需要响应
                                              2 - 异步响应即两次响应,第一次在收到请求后响应，第二次在完成处理后响应 */
} DRQMHDR;
typedef DRQMHDR tDRQMHDR;

typedef struct{
    char    dtaname[ NAMELEN+1 ];    /* DTA名称 */
    int     instid;                  /* 实例号 */
    int     epid;                    /* 当前变量池编号 */
    char    machnname[NAMELEN+1];    /* 机器名称 */
    int     dtatype;                 /* 定义DTA类型
                                        DTATYPE_SOURCE       源DTA
                                        DTATYPE_DESTINATION  目的DTA */
    short   synctyp;                 /* 同/异步标志
                                        SHM_SYNC     同步
                                        SHM_ASYNC    异步 */
    char    svcname[SVCLEN+1];       /* 服务名 */
    char    nodename[NAMELEN+1];     /* 节点名 */
    unsigned long  deviceid;                /* 请求方设备标识 */
    short   stidx;                   /* 堆栈顺序号，服务接受时初始化，
                                      * 返回时检查是否有未出栈请求 */
    int     mapid;                   /* 数据映射使用的临时变量池，交换用 */
    double  dtabegtime;              /* 交易请求起始时间 */
    tDRQMHDR *drqmhdr;               /* 交易DRQ信息 */
    char    sysno;                   /* 子系统号 */
    char    nodeno[2];               /* 节点号 */
    int     stepno;                  /* 冲正顺序号 */
    int     pid;                     /* 进程号 */
    int     tranid;                  /* 服务嵌套调用使用的临时变量池，交换用 */
} tDTAINFO;

/* 定义定制共享内存头结构 */
typedef struct {
    int         num;               /* 结构体数组数据个数*/
    size_t      off;               /* 结构体相对头指针的偏移 */
} tSHMCONT;

#define SYS_DMCH_NAME       "__DMCH_NAME" /* 目的机器名  */
#define SYS_SVC_NAME        "__SVCNAME"
#define SYS_ESWT_SEQ        "__ESWT_SEQ"
#define SYS_SQL_STR         "__SQL_STR"   /* 存放数据库操作SQL语句 */
#define SYS_ALA_DEBUG_FILE  "__ALA_DEBUG_FILE"
#define SYS_ITEMDATA        "__ITEMDATA"        /* 报文域数据元素 */
#define SYS_ITEMDATA_LEN    "__ITEMDATA_LENGTH" /* 报文域数据元素长度 */
#define SYS_PACKDATA        "__PACKDATA"        /* 报文数据元素 */
#define SYS_PACKDATA_LEN    "__PACKDATA_LENGTH" /* 报文数据元素长度 */
#define SYS_REC_NO          "__REC_NO"  /* 保存子报文循环次数 */
#define SYS_TRAN_DATE    "__TRAN_DATE"  /* 系统流水表中存放交易日期(应用赋值) */
#define SYS_SEQ_NO       "__SEQ_NO"     /* 系统流水表中存放流水号(应用赋值) */
#define SYS_INST_NO      "__INST_NO"    /* 系统流水表中存放机构码(应用赋值) */
#define SYS_OPER_NO      "__OPER_NO"    /* 系统流水表中存放操作员号(应用赋值) */
#define SYS_BUSI_NO      "__TRAN_BUSI"  /* 系统流水表中存放业务号(应用赋值) */
#define SYS_TRAN_CODE    "__TRAN_CODE"  /* 系统流水表中存放交易码(应用赋值) */
#define SYS_CUST_NO      "__TRAN_CUST"  /* 系统流水表中存放客户号(应用赋值) */
#define SYS_DE_ACCT_NO   "__DE_ACCT_NO" /* 系统流水表中存放借方帐号(应用赋值) */
#define SYS_CR_ACCT_NO   "__CR_ACCT_NO" /* 系统流水表中存放贷方帐号(应用赋值) */
#define SYS_TRAN_AMT     "__TRAN_AMT"   /* 系统流水表中存放交易金额(应用赋值) */
#define SYS_RET_CODE     "__ERR_RET"    /* 系统流水表中存放交易返回码(应用赋值) */
#define SYS_SVC_ID       "__SYS_SVCID"  /* 服务身份ID */
#define SYS_CUSTPAL      "__CUST_PAL"   /* 客户化并发数限制 */
#define SYS_CUSTPAL_IDX  "__CUST_PAL_IDX"  /* 客户化并发条目 */
#define SYS_SUMMARY      "__SUMMARY"    /* 系统流水表中存放交易备注(应用赋值) */
#define GDTAITEMDATA        "__GDTA_ITEMDATA"   /* 报文长度数据元素 */
#define GDTAITEMDATALEN     "__GDTA_ITEMDATA_LENGTH" /* 报文内容数据元素 */
#define GDTASVCNAME         "__GDTA_SVCNAME"         /* 交易名 */
#define GDTABITMAP          "__GDTA_BITMAP"          /* 8583的BITMAP内容 */
#define GDTAFILENAME        "__GDTA_FILENAME"        /* 文件名 */
#define GDTATCPADDR         "__GDTA_TCP_ADDR"        /* 接收到的描述符IP */
#define GDTASHORT           "__GDTA_SHORT"        /* 服务DTA是否短路标志 */
#define AREV_FLAG         "__AUTOREVS_FLAG"          /* 冲正标志 */
#define AREV_STAT         "__AUTOREVS_STAT"          /* 冲正状态 */
#define AREV_SEQ_NO       "__T_AUTO_REV_SEQ_NO"      /* 冲正流水的平台流水号 */
#define AREV_SERI_NO      "__T_AUTO_REV_SERI_NO"     /* 冲正流水的顺序号 */
#define AREV_SRC_DTA      "__T_AUTO_REV_SRC_DTA"     /* 冲正流水的源DTA */
#define AREV_SRC_MCH      "__T_AUTO_REV_SRC_MCH"     /* 冲正流水的源MCH */
#define AREV_SVC_NAME     "__T_AUTO_REV_SVC_NAME"    /* 冲正流水的源服务名 */
#define AREV_DST_DTA      "__T_AUTO_REV_DST_DTA"     /* 冲正流水的目的DTA */
#define AREV_THD_SVC      "__T_AUTO_REV_THD_SVC"     /* 冲正流水的目的服务名 */
#define AREV_REV_CODE     "__T_AUTO_REV_REV_CODE"    /* 冲正流水的目的冲正服务名 */
#define AREV_REV_TIMES    "__T_AUTO_REV_REV_TIMES"   /* 冲正流水的已冲正次数 */

#define SYS_PACKREQID        "__PACKREQID"           /* 报文数据reqid元素 */
#define SYS_REVSREQID        "__REVSREQID"           /* 自动冲正元素 */

#define BUSI_SEQNO        "__BUSI_SEQNO"             /* 业务跟踪号 */
#define COMPENSATE        "__COMPENSATE"             /* 补偿状态 */
#define AUTO_COM_COM_MAX_TIMES    "__T_AUTO_COM_COM_MAX_TIMES"    /* 补偿信息的最大补偿次数 */
#define AUTO_COM_COM_MCH          "__T_AUTO_COM_COM_MCH"    /* 补偿信息的补偿机器 */
#define AUTO_COM_COM_PID          "__T_AUTO_COM_COM_PID"    /* 补偿信息的补偿处理进程 */
#define AUTO_COM_COM_STAT         "__T_AUTO_COM_COM_STAT"   /* 补偿信息的补偿状态，0待补偿、1处理中 */
#define AUTO_COM_COM_TIMES        "__T_AUTO_COM_COM_TIMES"  /* 补偿信息的补偿次数 */
#define AUTO_COM_DST_DTA          "__T_AUTO_COM_DST_DTA"    /* 补偿信息的目的DTA名 */
#define AUTO_COM_LAST_TIME        "__T_AUTO_COM_LAST_TIME"  /* 补偿信息的最后补偿时间 */
#define AUTO_COM_REV_CODE         "__T_AUTO_COM_REV_CODE"   /* 补偿信息的目的服务对应冲正服务名 */
#define AUTO_COM_SEQ_NO           "__T_AUTO_COM_SEQ_NO"     /* 补偿信息的业务流水号 */
#define AUTO_COM_SRC_DTA          "__T_AUTO_COM_SRC_DTA"    /* 补偿信息的源DTA名 */
#define AUTO_COM_SRC_MCH          "__T_AUTO_COM_SRC_MCH"    /* 补偿信息的源机器名 */
#define AUTO_COM_STEP_NO          "__T_AUTO_COM_STEP_NO"    /* 补偿信息的步骤号 */
#define AUTO_COM_SVC_NAME         "__T_AUTO_COM_SVC_NAME"   /* 补偿信息的服务名 */
#define AUTO_COM_THD_SEQ_NO       "__T_AUTO_COM_THD_SEQ_NO" /* 补偿信息的平台流水号 */
#define AUTO_COM_THD_SVC          "__T_AUTO_COM_THD_SVC"    /* 补偿信息的目的服务名 */
#define AUTO_COM_TRAN_DATE        "__T_AUTO_COM_TRAN_DATE"  /* 补偿信息的目的交易日期 */
#define AUTO_COM_TRAN_TIME        "__T_AUTO_COM_TRAN_TIME"  /* 补偿信息的目的交易时间 */
#define SYS_TRANS_LOG        "__TRANSLOG"  /* 交易日志信息 */


/* 数据转换定义 */
#define DATANOCHG     0   /* 字序一样，变量池数据不需要转换 */
#define DATACHG       1   /* 字序不一样，变量池数据需要转换 */

/* DTA类型定义 */
#define     SHM_DTATYPE_SOURCE       1     /* 源DTA */
#define     SHM_DTATYPE_DESTINATION  2     /* 目的DTA */

#define _FL_  __FILE__, __LINE__  


/*
 * 定义系统状态
 */
#define SYS_STAT_OFF      1     /* 脱机状态:未启动管理和部署共享内存 */
#define SYS_STAT_INIT     2     /* 初始状态:仅启动管理共享内存 */
#define SYS_STAT_ON       3     /* 在线状态:启动管理和部署共享内存 */
#define SYS_STAT_NEXT     4     /* 不改变系统状态,切换主控节点 */


/*
 * 定义 DTAAppCall/DTAAppReturn 的应用返回代码
 */
#define   SHM_SYSERR_OK                       0   /* 成功 */
#define   SHM_SYSERR_APP                     -11  /* 应用返回错误 */
#define   SHM_SYSERR_REQ                     -12  /* 内部处理请求失败 */
#define   SHM_SYSERR_ACK                     -13  /* 内部处理响应失败 */
#define   SHM_SYSERR_BUS                     -14  /* 并发数超限 */

/*
 * 定义esl模块中bcl_tcp_aconn使用的超时
 */
#define   ESL_DEF_TMOUT     10

/*
 * 各种锁的类型定义
 */
#define DRQLOCK             0
#define DRQBUFLOCK          1
#define LINKLOCK            2
#define BALCTRLLOCK         3
#define BALSEMLOCK          4
#define SHMLOCK             5
#define INSTLOCK            6
#define SYNCLOCK            7
#define CONCLOCK            8

#define SHM_INSTTYPE_REQ      0 /* 实例为请求实例 */
#define SHM_INSTTYPE_RES      1 /* 实例为响应实例 */
#define SHM_INSTTYPE_PARENT   2 /* 实例为调度实例 */

typedef enum {
    MAX_SHMVER=1,
    RCYSHM_TIME,
    MAX_MCHNUM,
    MAX_DTANUM,
    MAX_ALANUM,
    MAX_DTANODE,
    MAX_FSNUM,
    MAX_RUTNUM,
    MAX_DBNUM,
    MAX_DTACONN,
    MAX_LSTNNUM,
    MAX_NETNUM,
    MAX_PRESSNUM,
    MAX_INSTNUM,
    MAX_FSNINST,
    MAX_DRQSEQNUM,
    MAX_DTAPARLNUM,
    MAX_CUSTPAL,
    MAX_SVCIDNUM,
    MAX_CONNNUM,
    MAX_SVCNUM,
    MAX_APPNUM,
    MAX_SUBAPPNUM,
    MAX_PRIOINST,
    MAX_NESTLVL,
    MAX_SUBFMT,
    MAX_FLSSVR,
    MAX_OPERREC,
    MAX_ACCESSOR,
    EXT_CHUNK_SIZE,
    MAX_CHUNKS,
    QHWM,
    MAX_QUE,
    MAX_TRANS,
    MAX_AUTOTASKNUM,
    MAX_BATTASKNUM,
    MAX_TRANNUM,
    MAX_NEST_LVL,
    MAX_SUB_FMT,
    MAX_STOPSVC
} StarringMaxes;

typedef enum {
    SHM_DEP = 1,
    SHM_PAR = 2,
    SHM_ALL = 3
}StarringShmType;


/* 资源类型枚举 */
typedef enum {
    RES_TYPE_DTA = 0, /* DTA/ALA */
    RES_TYPE_DELEM,   /* DATAELEM */
    RES_TYPE_IOF,     /* IOFormat */
    RES_TYPE_FIOF,    /* FileFmt */
    RES_TYPE_SIGN,    /* Sign */
    RES_TYPE_DSVC,    /* DTASvc/SvcLogic */
    RES_TYPE_NODE,    /* DTANode */
    RES_TYPE_FLW,     /* Flow */
    RES_TYPE_ENUM,    /* Enum */
    RES_TYPE_FORM,    /* Form */
    RES_TYPE_CPT,     /* Component */
    RES_TYPE_STDS,    /* Standard Service */
    RES_TYPE_RT       /* DTA Route */
}StarringResType;

/* 为查看资源列表提供的数据结构 */
typedef struct {
    char            name[NAMELEN+1]; /* 名称 */
    char            desc[DESCLEN+1]; /* 描述 */
}tNAMEDESC;

/* 为查看资源列表提供的数据结构 */
typedef struct {
    char            name[NAMELEN+1]; /* (格式项)数据元素名称 */
    char            desc[DESCLEN+1]; /* 描述 */
    short           type;            /* 数据元素类型 */
    int             len;             /* 格式项长度,数据元素长度 */
    char            deft[DESCLEN+1]; /* 数据元素默认值 */
}tVIEWITEM;

#define SHM_INEXIST    SYS_NO
#define SHM_EXIST      SYS_YES

#endif
