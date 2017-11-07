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
 *    DESCRIPTION:    ϵͳ��/�ṹ����
 *    AUTHOR:
 *              NaWei 07/10/2003 - Developed for eSWITCH system
 *    MODIFIED:
 *
 */

#ifndef __SYSDEF_H
#define __SYSDEF_H


#define   E_SHMREF       -4     /* ��DRQ���г�ʱ       */
#define   E_DRQTIMEDOUT  -3     /* ��DRQ���г�ʱ       */
#define   E_NOTLOC       -2     /* ���Ǳ���,Ϊ������� */
#define   E_FAIL         -1
#define   E_OK            0
#define   E_BUSY          1

/* ���̸��ݱ�־�жϽ������ͣ�ȡ���������� */
#define   SYS_PROC_PLAT        0    /* ƽ̨���� */
#define   SYS_PROC_DTA         1    /* DTA���� */
#define   SYS_PROC_ALA         2    /* ALA���� */

/*
 * �����ǷǱ�־,���ڶ�����Ϣ,��:��������Ϊ��ѡ��ѡʱ,ѡ��Ϊ��,����Ϊ��
 */
#define   SYS_YES               1      /* �� */
#define   SYS_NO                2      /* �� */
#define   SYS_INUSE             3      /* ����ʹ�� */
#define   SYS_UNUSED            4      /* δ��ʹ�� */
#define   SYS_RCYCL             5      /* ���� */

#define   SYS_DELAY             3      /* ��־�ӳ�д */

#define   XML_SYS_YES           "YES"  /* �� */
#define   XML_SYS_NO            "NO"   /* �� */
#define   XML_SYS_DELAY         "DELAY"   /* �ӳ�д */

#define         SHM_YES         'Y'     /* ȷ�� */
#define         SHM_NO          'N'     /* �� */

#define   NAMELEN    50   /* ���Ƴ���(��128��Ϊ51) */
#define   IPADDR_LEN  NAMELEN /* IP��ַ���� */
#define   EXPRLEN    512  /* ���ʽ����(������255�ĳ�128) 20051014->512 */
#define   DESCLEN    60   /* �����ֶγ��� */
#define   SVCLEN     NAMELEN   /* ������������ 20060614 20 -> 50 */
#define   MESGLEN    128  /* ��Ϣ/�������� */
#define   MSESSLEN   20   /* ���Ự�ų��� */
#define   PLATINTLEN 11   /* 20050108�̶���ˮ�ŵĳ��ȣ���ͬ�ֽڵĻ���������� */
#define   SEQNOLEN   (PLATINTLEN)*2   /* ��� system seqno ���� */
#define   MAXLEN     2048 /* �ַ�����󳤶� */

#define   FLSTLEN    256  /* �ļ����Ƴ��� */
#define   STATLEN    512  /* ���״������״̬��¼ */
#define   MAXSTNUM   10   /* ����ջ���� */

#define   TRANDATELEN 10   /* ʱ�䳤��yyyymmhh */
#define   TRANTIMELEN 8   /* ʱ�䳤��hhmmss */
#define   VERSION_LEN 20  /* �����ڴ�汾�ų��� */
#define   DATETIMELEN 19  /* YYYY/MM/DD hh:mm:ss */

/*
 * ������־�ļ�����
 */
#define		EXTCMLOG	1	/* �ⲿͨѶ������־ */
#define		FMTLOG		2	/* ���ĸ�ʽ������־ */
#define		EVTLOG		3	/* �¼�������־ */
#define		RUTLOG		4	/* ·�ɸ�����־ */
#define		EXPRLOG		5	/* ���ʽ������־ */
#define		FLWLOG		6	/* ���̸�����־ */
#define		RVSLOG		7	/* �Զ�����������־ */
#define		DBLOG		8	/* ���ݿ��װ������־ */
#define		PRESSLOG	9	/* ѹ��������־ */

/*
 * ��־�ȼ�����
 */
#define     LOG_ERROR    1   /* ���� */
#define     LOG_WARN     2  /* ���� */
#define     LOG_INFO     3  /* ��Ϣ */
#define     LOG_DEBUG    4  /* ���� */
#define     LOG_TRACE    5  /* ���� */

/*
 * ����ͬ/�첽�궨��
 */
#define         SHM_SYNC        1       /* ͬ�� */
#define         SHM_ASYNC       2       /* �첽 */

/*
 * ����DRQͷ�������ͺ궨��
 */
#define         SVC_NORMAL      0       /* �������� */
#define         SVC_REVS        1       /* �������� */
#define         SVC_ASYNC       2       /* �첽DTA���� */
#define         SVC_COMPST      3       /* �������� */
#define         SVC_COMPNT      4       /* Ԫ�� */

#define MCH_NAMELIST_LEN 128      /* ���������ձ��� */
#define DTA_NAMELIST_LEN 128      /* DTA�����ձ��� */

/*
 * ����ͷ�ṹ״̬
 */
#define     REQNOSTAT        1     /* ��ʼ״̬ */
#define     REQSENDOK        2     /* ���ͳɹ� */
#define     REQRECVOK        3     /* ��Ӧ�ɹ� */

typedef enum {
    DRQACK_DEFAULT = 0,                /* ������Ӧ */
    DRQACK_NO,                  /* ����Ҫ��Ӧ */
    DRQACK_ASYN                       /* ������Ӧ,��һ�����յ��������Ӧ,�ڶ����ڴ�����ɺ���Ӧ */
}DrqAckType;

typedef struct {         /* Message struct of drq msg */
    char            svcname[SVCLEN+1];      /* service name */
    char            dsvcname[SVCLEN+1];     /* ͨ��ָ����ֵ,���Ŀ�ķ������� */
    short           dsvcflag;               /* Ŀ�ķ�������,0��������,1�������� */
    char            filelst[FLSTLEN+1];     /* file list ����ļ���+�ŷָ� */
    char            sdtaname[NAMELEN+1];    /* source dta name */
    char            smachnname[NAMELEN+1];  /* source machn name */
    int             sinstid;                /* source dta inst id */
    char            ddtaname[NAMELEN+1];    /* destination dta name */
    char            dmachnname[NAMELEN+1];  /* destination machn name */
    long            reqid;                  /* request id   */
    short           qtype;                  /* Queue type*/
    short           fend;                   /* message end flag */
    short           snaact;                 /* SNA command type 
                                    0 - SNACALL ���ͺͽ���һ����� 
                                    1 - SNASEND һ�η��ͣ���ν��� 
                                    2 - SNACONF ����ȷ��(���Ƽ�ʹ��) */
    short           fconf;                  /* ȷ�ϱ�־ 0 - ��ȷ�� 1 - ��ȷ�� */
    int             drqerrno;               /* Error code */
    char            errmsg[ DESCLEN + 1 ];  /* Error message */
    short           reqstat;                /* ����״̬ */
                                 /* 0:REQNOSTAT:��ʼ״̬ */
                                 /* 1:REQSENDOK:���ͳɹ� */
                                 /* 2:REQRECVOK:��Ӧ�ɹ� */
    short           stidx;                 /* ��ǰ��ջָ��,��ֵΪ0 */
    char            resdtast[MAXSTNUM+1][NAMELEN+1];  /* ��ӦDTA��ջ */
    char            resmchst[MAXSTNUM+1][NAMELEN+1];  /* ��Ӧ������ջ */
    char            svcnamest[MAXSTNUM+1][SVCLEN+1];  /* ��������ջ */
    short           retqst[MAXSTNUM+1]; /* ��ӦDTA���ն��� */
                             /* 100:REQ_QUEUE�������;200:SRV_QUEUE������� */
    long            reqidst[MAXSTNUM+1];   /* ��Ϣ�����ջ */
    char            session[MSESSLEN+1];   /* �Ự�� */
    u_long          deviceid;              /* �����豸��ʶ */
    char            ssdtaname[NAMELEN+1];  /* ��һ��source dta name */
    char            ssmachnname[NAMELEN+1];/* ��һ��source machn name */
    char            sdtaseq[SEQNOLEN+1];   /* ԴDTA��ˮ�� */
    char            seqstat[STATLEN+1];    /*���״���״̬ */
                                /* ��ʽΪ :
             ����id.��DTAid���������id.����DTA id-Ŀ�����id.Ŀ��DTAid
                  1.1:1.1-1.2
                                */
    char            sdtadate[TRANDATELEN+1]; /*ԴDTA����ϵͳ����, ��ʽΪ��YYYYMMDD */
    double          ssvc_begtime;     /* ����������ʼʱ�� */
    int             tran_regid;       /* �Ǽǽ������ɵĵǼ���λ�� */
    char            mchnamelist[MCH_NAMELIST_LEN+1];       /* �������ͱ�Ŷ��ձ�*/
    char            dtanamelist[DTA_NAMELIST_LEN+1];       /* DTA�ͱ�Ŷ��ձ�*/
    short           svc_prio;         /* �������ȼ� */
    char            cust_pal_idx[NAMELEN+1]; /* �ͻ�����Ŀ */
    char            svcid[NAMELEN+1]; /* �������ID */
    int             time_out;         /* Դ��ʣ�೬ʱʱ�� */
    long            dep_version; /* ����ʹ�õĲ������ڴ�汾 */
    long            par_version; /* ����ʹ�õĲ��������ڴ�汾 */
    short           ack_flag;    /* ��Ӧ��־: 0 -- ������Ӧ,
                                              1 - ����Ҫ��Ӧ
                                              2 - �첽��Ӧ��������Ӧ,��һ�����յ��������Ӧ���ڶ�������ɴ������Ӧ */
} DRQMHDR;
typedef DRQMHDR tDRQMHDR;

typedef struct{
    char    dtaname[ NAMELEN+1 ];    /* DTA���� */
    int     instid;                  /* ʵ���� */
    int     epid;                    /* ��ǰ�����ر�� */
    char    machnname[NAMELEN+1];    /* �������� */
    int     dtatype;                 /* ����DTA����
                                        DTATYPE_SOURCE       ԴDTA
                                        DTATYPE_DESTINATION  Ŀ��DTA */
    short   synctyp;                 /* ͬ/�첽��־
                                        SHM_SYNC     ͬ��
                                        SHM_ASYNC    �첽 */
    char    svcname[SVCLEN+1];       /* ������ */
    char    nodename[NAMELEN+1];     /* �ڵ��� */
    unsigned long  deviceid;                /* �����豸��ʶ */
    short   stidx;                   /* ��ջ˳��ţ��������ʱ��ʼ����
                                      * ����ʱ����Ƿ���δ��ջ���� */
    int     mapid;                   /* ����ӳ��ʹ�õ���ʱ�����أ������� */
    double  dtabegtime;              /* ����������ʼʱ�� */
    tDRQMHDR *drqmhdr;               /* ����DRQ��Ϣ */
    char    sysno;                   /* ��ϵͳ�� */
    char    nodeno[2];               /* �ڵ�� */
    int     stepno;                  /* ����˳��� */
    int     pid;                     /* ���̺� */
    int     tranid;                  /* ����Ƕ�׵���ʹ�õ���ʱ�����أ������� */
} tDTAINFO;

/* ���嶨�ƹ����ڴ�ͷ�ṹ */
typedef struct {
    int         num;               /* �ṹ���������ݸ���*/
    size_t      off;               /* �ṹ�����ͷָ���ƫ�� */
} tSHMCONT;

#define SYS_DMCH_NAME       "__DMCH_NAME" /* Ŀ�Ļ�����  */
#define SYS_SVC_NAME        "__SVCNAME"
#define SYS_ESWT_SEQ        "__ESWT_SEQ"
#define SYS_SQL_STR         "__SQL_STR"   /* ������ݿ����SQL��� */
#define SYS_ALA_DEBUG_FILE  "__ALA_DEBUG_FILE"
#define SYS_ITEMDATA        "__ITEMDATA"        /* ����������Ԫ�� */
#define SYS_ITEMDATA_LEN    "__ITEMDATA_LENGTH" /* ����������Ԫ�س��� */
#define SYS_PACKDATA        "__PACKDATA"        /* ��������Ԫ�� */
#define SYS_PACKDATA_LEN    "__PACKDATA_LENGTH" /* ��������Ԫ�س��� */
#define SYS_REC_NO          "__REC_NO"  /* �����ӱ���ѭ������ */
#define SYS_TRAN_DATE    "__TRAN_DATE"  /* ϵͳ��ˮ���д�Ž�������(Ӧ�ø�ֵ) */
#define SYS_SEQ_NO       "__SEQ_NO"     /* ϵͳ��ˮ���д����ˮ��(Ӧ�ø�ֵ) */
#define SYS_INST_NO      "__INST_NO"    /* ϵͳ��ˮ���д�Ż�����(Ӧ�ø�ֵ) */
#define SYS_OPER_NO      "__OPER_NO"    /* ϵͳ��ˮ���д�Ų���Ա��(Ӧ�ø�ֵ) */
#define SYS_BUSI_NO      "__TRAN_BUSI"  /* ϵͳ��ˮ���д��ҵ���(Ӧ�ø�ֵ) */
#define SYS_TRAN_CODE    "__TRAN_CODE"  /* ϵͳ��ˮ���д�Ž�����(Ӧ�ø�ֵ) */
#define SYS_CUST_NO      "__TRAN_CUST"  /* ϵͳ��ˮ���д�ſͻ���(Ӧ�ø�ֵ) */
#define SYS_DE_ACCT_NO   "__DE_ACCT_NO" /* ϵͳ��ˮ���д�Ž跽�ʺ�(Ӧ�ø�ֵ) */
#define SYS_CR_ACCT_NO   "__CR_ACCT_NO" /* ϵͳ��ˮ���д�Ŵ����ʺ�(Ӧ�ø�ֵ) */
#define SYS_TRAN_AMT     "__TRAN_AMT"   /* ϵͳ��ˮ���д�Ž��׽��(Ӧ�ø�ֵ) */
#define SYS_RET_CODE     "__ERR_RET"    /* ϵͳ��ˮ���д�Ž��׷�����(Ӧ�ø�ֵ) */
#define SYS_SVC_ID       "__SYS_SVCID"  /* �������ID */
#define SYS_CUSTPAL      "__CUST_PAL"   /* �ͻ������������� */
#define SYS_CUSTPAL_IDX  "__CUST_PAL_IDX"  /* �ͻ���������Ŀ */
#define SYS_SUMMARY      "__SUMMARY"    /* ϵͳ��ˮ���д�Ž��ױ�ע(Ӧ�ø�ֵ) */
#define GDTAITEMDATA        "__GDTA_ITEMDATA"   /* ���ĳ�������Ԫ�� */
#define GDTAITEMDATALEN     "__GDTA_ITEMDATA_LENGTH" /* ������������Ԫ�� */
#define GDTASVCNAME         "__GDTA_SVCNAME"         /* ������ */
#define GDTABITMAP          "__GDTA_BITMAP"          /* 8583��BITMAP���� */
#define GDTAFILENAME        "__GDTA_FILENAME"        /* �ļ��� */
#define GDTATCPADDR         "__GDTA_TCP_ADDR"        /* ���յ���������IP */
#define GDTASHORT           "__GDTA_SHORT"        /* ����DTA�Ƿ��·��־ */
#define AREV_FLAG         "__AUTOREVS_FLAG"          /* ������־ */
#define AREV_STAT         "__AUTOREVS_STAT"          /* ����״̬ */
#define AREV_SEQ_NO       "__T_AUTO_REV_SEQ_NO"      /* ������ˮ��ƽ̨��ˮ�� */
#define AREV_SERI_NO      "__T_AUTO_REV_SERI_NO"     /* ������ˮ��˳��� */
#define AREV_SRC_DTA      "__T_AUTO_REV_SRC_DTA"     /* ������ˮ��ԴDTA */
#define AREV_SRC_MCH      "__T_AUTO_REV_SRC_MCH"     /* ������ˮ��ԴMCH */
#define AREV_SVC_NAME     "__T_AUTO_REV_SVC_NAME"    /* ������ˮ��Դ������ */
#define AREV_DST_DTA      "__T_AUTO_REV_DST_DTA"     /* ������ˮ��Ŀ��DTA */
#define AREV_THD_SVC      "__T_AUTO_REV_THD_SVC"     /* ������ˮ��Ŀ�ķ����� */
#define AREV_REV_CODE     "__T_AUTO_REV_REV_CODE"    /* ������ˮ��Ŀ�ĳ��������� */
#define AREV_REV_TIMES    "__T_AUTO_REV_REV_TIMES"   /* ������ˮ���ѳ������� */

#define SYS_PACKREQID        "__PACKREQID"           /* ��������reqidԪ�� */
#define SYS_REVSREQID        "__REVSREQID"           /* �Զ�����Ԫ�� */

#define BUSI_SEQNO        "__BUSI_SEQNO"             /* ҵ����ٺ� */
#define COMPENSATE        "__COMPENSATE"             /* ����״̬ */
#define AUTO_COM_COM_MAX_TIMES    "__T_AUTO_COM_COM_MAX_TIMES"    /* ������Ϣ����󲹳����� */
#define AUTO_COM_COM_MCH          "__T_AUTO_COM_COM_MCH"    /* ������Ϣ�Ĳ������� */
#define AUTO_COM_COM_PID          "__T_AUTO_COM_COM_PID"    /* ������Ϣ�Ĳ���������� */
#define AUTO_COM_COM_STAT         "__T_AUTO_COM_COM_STAT"   /* ������Ϣ�Ĳ���״̬��0��������1������ */
#define AUTO_COM_COM_TIMES        "__T_AUTO_COM_COM_TIMES"  /* ������Ϣ�Ĳ������� */
#define AUTO_COM_DST_DTA          "__T_AUTO_COM_DST_DTA"    /* ������Ϣ��Ŀ��DTA�� */
#define AUTO_COM_LAST_TIME        "__T_AUTO_COM_LAST_TIME"  /* ������Ϣ����󲹳�ʱ�� */
#define AUTO_COM_REV_CODE         "__T_AUTO_COM_REV_CODE"   /* ������Ϣ��Ŀ�ķ����Ӧ���������� */
#define AUTO_COM_SEQ_NO           "__T_AUTO_COM_SEQ_NO"     /* ������Ϣ��ҵ����ˮ�� */
#define AUTO_COM_SRC_DTA          "__T_AUTO_COM_SRC_DTA"    /* ������Ϣ��ԴDTA�� */
#define AUTO_COM_SRC_MCH          "__T_AUTO_COM_SRC_MCH"    /* ������Ϣ��Դ������ */
#define AUTO_COM_STEP_NO          "__T_AUTO_COM_STEP_NO"    /* ������Ϣ�Ĳ���� */
#define AUTO_COM_SVC_NAME         "__T_AUTO_COM_SVC_NAME"   /* ������Ϣ�ķ����� */
#define AUTO_COM_THD_SEQ_NO       "__T_AUTO_COM_THD_SEQ_NO" /* ������Ϣ��ƽ̨��ˮ�� */
#define AUTO_COM_THD_SVC          "__T_AUTO_COM_THD_SVC"    /* ������Ϣ��Ŀ�ķ����� */
#define AUTO_COM_TRAN_DATE        "__T_AUTO_COM_TRAN_DATE"  /* ������Ϣ��Ŀ�Ľ������� */
#define AUTO_COM_TRAN_TIME        "__T_AUTO_COM_TRAN_TIME"  /* ������Ϣ��Ŀ�Ľ���ʱ�� */
#define SYS_TRANS_LOG        "__TRANSLOG"  /* ������־��Ϣ */


/* ����ת������ */
#define DATANOCHG     0   /* ����һ�������������ݲ���Ҫת�� */
#define DATACHG       1   /* ����һ����������������Ҫת�� */

/* DTA���Ͷ��� */
#define     SHM_DTATYPE_SOURCE       1     /* ԴDTA */
#define     SHM_DTATYPE_DESTINATION  2     /* Ŀ��DTA */

#define _FL_  __FILE__, __LINE__  


/*
 * ����ϵͳ״̬
 */
#define SYS_STAT_OFF      1     /* �ѻ�״̬:δ��������Ͳ������ڴ� */
#define SYS_STAT_INIT     2     /* ��ʼ״̬:�������������ڴ� */
#define SYS_STAT_ON       3     /* ����״̬:��������Ͳ������ڴ� */
#define SYS_STAT_NEXT     4     /* ���ı�ϵͳ״̬,�л����ؽڵ� */


/*
 * ���� DTAAppCall/DTAAppReturn ��Ӧ�÷��ش���
 */
#define   SHM_SYSERR_OK                       0   /* �ɹ� */
#define   SHM_SYSERR_APP                     -11  /* Ӧ�÷��ش��� */
#define   SHM_SYSERR_REQ                     -12  /* �ڲ���������ʧ�� */
#define   SHM_SYSERR_ACK                     -13  /* �ڲ�������Ӧʧ�� */
#define   SHM_SYSERR_BUS                     -14  /* ���������� */

/*
 * ����eslģ����bcl_tcp_aconnʹ�õĳ�ʱ
 */
#define   ESL_DEF_TMOUT     10

/*
 * �����������Ͷ���
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

#define SHM_INSTTYPE_REQ      0 /* ʵ��Ϊ����ʵ�� */
#define SHM_INSTTYPE_RES      1 /* ʵ��Ϊ��Ӧʵ�� */
#define SHM_INSTTYPE_PARENT   2 /* ʵ��Ϊ����ʵ�� */

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


/* ��Դ����ö�� */
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

/* Ϊ�鿴��Դ�б��ṩ�����ݽṹ */
typedef struct {
    char            name[NAMELEN+1]; /* ���� */
    char            desc[DESCLEN+1]; /* ���� */
}tNAMEDESC;

/* Ϊ�鿴��Դ�б��ṩ�����ݽṹ */
typedef struct {
    char            name[NAMELEN+1]; /* (��ʽ��)����Ԫ������ */
    char            desc[DESCLEN+1]; /* ���� */
    short           type;            /* ����Ԫ������ */
    int             len;             /* ��ʽ���,����Ԫ�س��� */
    char            deft[DESCLEN+1]; /* ����Ԫ��Ĭ��ֵ */
}tVIEWITEM;

#define SHM_INEXIST    SYS_NO
#define SHM_EXIST      SYS_YES

#endif
