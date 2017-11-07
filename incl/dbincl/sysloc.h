/* $Id
 *    Copyright (c) 2001-2002 ADTEC Ltd.
 *    All rights reserved
 *
 *    This is unpublished proprietary
 *    source code of ADTEC Ltd.
 *    The copyright notice above does not
 *    evidence any actual or intended
 *     publication of such source code.
 *
 *     NOTICE: UNAUTHORIZED DISTRIBUTION, ADAPTATION OR USE MAY BE
 *    SUBJECT TO CIVIL AND CRIMINAL PENALTIES.
 *
 *
 *    FILE:           sysloc.h
 *    DESCRIPTION:    Setup common environment
 *    AUTHOR:
 *              WangNan 03/20/2001 - Developed for eSWITCH system
 *    MODIFIED:
 *              QiuYP   09/19/2001 - Add BITMAPLEN for 8583
 *              Yuhy    08/02/2002 - Add Macro for tcpip
 *              
 */

#ifndef __SYSLOC_H
#define __SYSLOC_H

/*
 * Catalog�ļ�������    999
 * ϵͳ����               10000-10500
 * ƽ̨��������           10501-10999
 * ����ģ�����           11000-11999
 * ���ĸ�ʽģ��           12000-12999
 * ƽ̨��ˮģ��           13000-13999
 * ����Ԫ��ģ��           14000-14999
 * ���ؾ���ģ��           15000-15999
 * ·�ɼ���ģ��           16000-16999
 * ���ʽģ��             17000-17999
 * �����ڴ�ģ��           18000-18999
 * ���ͨѶģ��           19000-19999
 * �ͻ��˹���ģ��         20000-20999
 * ��������ģ��           21000-21999
 * DTA�ӿ�ģ��            22000-22999
 * ͨ��DTAģ��            23000-23999
 * ������ģ��           24000-24999
 * Viewstatģ��           25000-25999
 * ���̴���ģ��           26000-26999
 * ���ݿ��װģ��         27000-27999
 * Form����ģ��           28000-28999
 * ����ģ��               29000-29999
 * �ļ��������ģ��       30000-30999
 * ������־ģ��           31000-31999
 * 32000�Ժ� ����Ӧ����!!!
 */

/*
 * ϵͳ����               10000-10500
 */
#define E_MESSAGE        10000     /* %s */
#define E_PLAT           10001     /* ϵͳ�ڲ����� */
#define E_EXIT_OK        10002     /* �ӽ����˳�,״̬(%s),�����������ӽ���[%d]�ɹ� */
#define E_EXIT_FAIL      10003     /* �ӽ����˳�,״̬(%s),����������ʧ�� */
#define E_OSCALL         10201     /* ϵͳ����(%s)���ó���! */
#define E_FUNCALL        10202     /* ƽ̨����(%s)���ó���! */
#define E_INVALARG       10203     /* ��������(%s)����ȷ! */
#define E_NOEXEC         10204     /* �ļ�(%s)����ִ��! */
#define E_NOREC          10205     /* ������Ҫ���ҵ�����(%s)! */
#define E_SQL            10206     /* SQLִ�д�(%s)! */
#define E_ALLOC          10300     /* ����ռ�[%d]�ֽ�ʧ�� */
#define E_NOENV          10301     /* δ���û�������[%s] */
#define E_PTR_NULL       10302     /* ָ�����[%s]Ϊ�� */
#define E_IP             10303     /* �Ƿ�IP[%s] */
#define E_FORK           10304     /* ������[%d]�����ӽ���ʧ�� */
#define E_LISTEN         10305     /* ������ַ[%s:%hu]ʧ�� */
#define E_SOCKET         10306     /* SOCKET����ִ��ʧ��[errno=%d] */
#define E_DTA_DEP        10307     /* ȡ�������ڴ��DTA[%s]��Ϣʧ�� */
#define E_BALA_PARM      10308     /* ȡ���ؾ������ʧ�� */
#define E_HDR_PARM       10309     /* ȡ�����������ڴ�ͷָ��ʧ�� */
#define E_FILE           10310     /* �ļ�[%s]����ʧ�� */
#define E_DTA_PARM       10311     /* ȡ�����������ڴ��DTA[%s]��Ϣʧ�� */
#define E_HDR_RUN        10312     /* ȡ���в��������ڴ�ͷָ��ʧ�� */
#define E_ALA_PARM       10313     /* ȡ�����������ڴ��ALA[%s]��Ϣʧ�� */
#define E_EPGET          10314     /* ȡ����Ԫ��[%s]�ĵ�[%d]���±��ֵʧ�� */
#define E_EPPUT          10315     /* ������Ԫ��[%s]�ĵ�[%d]���±��ֵʧ�� */
#define E_HDR_DEP        10316     /* ȡ�������ڴ�ͷָ��ʧ�� */
#define E_VALUE_INT      10317     /* ���������ֵ[%d] */
#define E_ELEM_DEP       10318     /* ȡ�������ڴ������Ԫ��[%s]��Ϣʧ�� */
#define E_STRING         10319     /* �ַ���[%s]����ʧ�� */
#define E_SOCK_SEND      10320     /* �����ֽ���Ϊ[%d]������ʧ�� */
#define E_SOCK_RECV      10321     /* �����ֽ���Ϊ[%d]������ʧ�� */
#define E_MACHN_PARM     10322     /* ȡ�����������ڴ�Ļ���[%s]��Ϣʧ�� */
#define E_DTAINST_ID     10323     /* ȡDTA[%s]�ĵ�[%s]��ʵ�������й����ڴ���Ϣʧ�� */
#define E_DTAMCH_PARM    10324     /* ȡDTA[%s]�ڻ���[%s]�ϵĲ����������ڴ�ʧ�� */
#define E_DTAINST_RUN    10325     /* ȡDTA[%s]ʵ�������й����ڴ���Ϣʧ�� */
#define E_DTASVC_DEP     10326     /* ȡDTA[%s]�Ϸ���[%s]�Ĳ������ڴ���Ϣ�� */
#define E_DEAMON_START   10327     /* ������פ����[%s]ʧ�� */
#define E_SHM_CHECK      10328     /* ��鹲���ڴ�ʧ�� */
#define E_VALUE_STRING   10329     /* ������ַ���[%s] */
#define E_ACCEPT         10330     /* ����[socket]����ʧ�� */
#define E_SYS_PARM       10331     /* ȡ�������ڴ�ϵͳ��Ϣʧ�� */
#define E_LOCAL_MACHN_PARM     10332     /* ȡ���ػ�����Ϣʧ�� */
#define E_IOFNAME_DEP    10333     /* ȡ���ĸ�ʽ[%s]�Ķ��ƹ����ڴ���Ϣʧ�� */
#define E_SIGNNAME_DEP   10334     /* ȡ��Ǽ�[%s]�Ķ��ƹ����ڴ���Ϣʧ�� */
#define E_FMTGRP_DEP     10335     /* ȡDTA[%s]����[%s]�ı����鶨�ƹ����ڴ���Ϣʧ�� */
#define E_VALUE_SHORT    10336     /* ����Ķ�����[%hd] */
#define E_TRANBASE_DEP   10337     /* ȡ���׵Ǽ���ͷ�ṹʧ�� */
#define E_KILL           10338     /* �����[%d]�����ź�[%s]ʧ�� */
#define E_CVALUE         10339     /* ȡ����[%s]����Ϣ��ʧ�� */
#define E_SHELL_CMD      10340     /* ִ��shell����[%s]ʧ�� */
#define E_LOAD_PARM      10341     /* װ�ز����������ڴ�ʧ�� */
#define E_LOAD_DEP       10342     /* װ�ز������ڴ�ʧ�� */
#define E_ATTACH_PARM    10343     /* ���Ӳ����������ڴ�ʧ�� */
#define E_ATTACH_DEP     10344     /* ���Ӳ������ڴ�ʧ�� */
#define E_CONNECT        10345     /* ���ӵ�ַ[%s:%hu]ʧ�� */
#define E_SIGNITEM_DEP   10346     /* ȡ��Ǽ�[%s]�ı��[%s]���ƹ����ڴ���Ϣʧ�� */
#define E_EXPR           10347     /* ���ʽ[%s]����ʧ�� */
#define E_IOFITEM_BYSIGN_DEP    10348     /* ���ݱ����[%s]ȡ��ʽ[%s]�ĸ�ʽ��ʧ�� */
#define E_DTANODE_PAR    10349     /* ȡDTA[%s]�ڵ�[%s]�Ĳ���������Ϣʧ�� */
#define E_DTASEQ_DEP     10350     /* ȡ�ò�����DTA[%s]����ˮ��ʧ�� */
#define E_DTA_BYID_DEP   10351     /* ����HASHֵ[%ld]ȡ�������ڴ��DTA��Ϣʧ�� */
#define E_SIZE_OVERFLOW  10352     /* �ռ�[%d]��������[%d] */
/* ADD BY PENGBIN 2005/07/03 */
#define E_EPUSEDEL_ID    10353     /* ������[%d]��ʹ�ñ�����[%d]�Ǽ���δ�ҵ� */
#define E_EPUSEDEL_EP    10354     /* ������ID[%d]��ʹ�ñ�����ID[%d]����ͬ */
#define E_EPUSEDEL_IDX   10355     /* ʹ�ñ������[%d]����ʹ�ñ�������ʹ�ñ�������[%d]��Χ�� */
/* ADD END */
#define E_EP_MAXPOOL     10356     /* ����[%d]ʹ�õı����ظ����Ѿ���������[%d] */
#define E_HDR_MON        10357     /* ȡͬ����ع����ڴ�ͷָ��ʧ�� */
#define E_ATTACH_MON     10358     /* ���Ӽ�ع����ڴ�ʧ�� */
#define E_SOCKFD_SEND    10359     /* ��������[%d]�����ֽ���Ϊ[%d]������ʧ�� */
#define E_SOCKFD_RECV    10360     /* ��������[%d]�����ֽ���Ϊ[%d]������ʧ�� */
#define E_FILE_OPEN      10361     /* ���ļ�[%s]ʧ��[%s]*/
#define E_OSCALL_DESC    10362     /* ϵͳ����[%s]����ʧ��[%s] */
#define E_COMM_NOTFOUND  10363     /* ��[%s]�����в����ҵ�ָ��[%s]���� */
#define E_SOCK_SND_TIMEOUT   10364     /* ���׽���[%d]��[%d]ʱ���ڷ����ֽ���Ϊ[%d]�����ݳ�ʱ */
#define E_SOCK_RCV_TIMEOUT   10365     /* ���׽���[%d]��[%d]ʱ���ڽ����ֽ���Ϊ[%d]�����ݳ�ʱ */
#define E_OUTOFBAND      10366     /* ����[%s]ʹ�õ�[%d]Ԫ��,����[%d]������ */
#define E_SEMCREATE      10367     /* �����ź���[%x][%o][%d]ʧ�� */
#define E_SEMREMOVEBYID  10368     /* ɾ���ź���[%d]ʧ�� */
#define E_SEMSIGNAL      10369     /* �����ź���id[%d],num[%d]ʧ�� */
#define E_SEMWAIT        10370     /* �ȴ��ź���id[%d],num[%d]ʧ�� */
#define E_ATTACH_RUN     10371     /* �������й����ڴ�ʧ�� */
#define E_ATTACH_PARMV   10372     /* ���Ӳ����������ڴ�汾[%s]ʧ�� */
#define E_ATTACH_DEPV    10373     /* ���Ӳ������ڴ�汾[%s]ʧ�� */
#define E_HDR_SVCSTAT    10374     /* ȡ����״̬�����ڴ�ͷָ��ʧ�� */
#define E_HDR_NODESTAT   10375     /* ȡ�ڵ�״̬�����ڴ�ͷָ��ʧ�� */
#define E_HDR_ATASKSTAT  10376     /* ȡ�Զ�����״̬�����ڴ�ͷָ��ʧ�� */
#define E_HDR_BTASKSTAT  10377     /* ȡ��������״̬�����ڴ�ͷָ��ʧ�� */
#define E_HDR_TRANSTAT   10378     /* ȡ�߼��Ͳ���״̬�����ڴ�ͷָ��ʧ�� */
#define E_ATTACH_SVCSTAT 10379     /* ���ӷ���״̬�����ڴ�ʧ�� */
#define E_DTASVC_RUN     10380     /* ȡDTA[%s]�Ϸ���[%s]��״̬�����ڴ���Ϣ�� */
#define E_ATTACH_ATASK   10381     /* �����Զ���������״̬�����ڴ�ʧ�� */
#define E_GET_ATASK_STAT 10382     /* ȡ�Զ�����[%s]����״̬ʧ�� */
#define E_DTAMCH_LISTEN  10383     /* ȡDTA[%s]�ڻ���[%s]�ϵļ�����ַ�б�ʧ�� */
#define E_DES3_DEC       10384     /* ��3DES�㷨����[%s]����[%d]��Կ[%s]ʧ�� */
#define E_BASE64_DEC     10385     /* ��base64�㷨����[%s]ʧ�� */



/*
 * ����ģ�����           11000-11999
 */
#define E_LINK_ADD                  11010   /* [%s]��������[%d]�ڵ�ʧ�� */
#define E_LINK_DUMP                 11011   /* [%s]�������������ڴ�ʧ�� */
#define E_FILE_FORMAT               11020   /* �ļ�[%s]��ʽ���� */
#define E_LICENSE                   11021   /* ���к�[%s]���� */
#define E_MATURITY                  11022   /* �Ѿ�����ʹ������[%s] */
#define E_LICFLOW                   11023   /* ��Ʒ[%s]���������̴���Ȩ�� */
#define E_LICINST                   11024   /* ��Ʒ�û���[%d]����[%s] */
#define E_SOCKET_ADDR               11100   /* ��������[%s]ȡ��ֵַʧ�� */
#define E_SOCKET_PORT               11101   /* ��������[%s]ȡ�˿�ֵʧ�� */
#define E_SOCKET_CONN               11102   /* ���ӵ�ַ[%s]�˿�[%hu]ʧ�� */
#define E_SOCKET_SEND               11103   /* ����[%ld]�ֽ�����ʧ�� */
#define E_SOCKET_RECV               11104   /* ����[%ld]�ֽ�����ʧ�� */
#define E_SOCKET_FORK               11105   /* ����[%d]�����Ӵ������ʧ�� */
#define E_SOCKET_FORKONE            11106   /* ���ɵ�[%d]�����Ӵ������ʧ�� */
#define E_SOCKET_SERV               11107   /* ������ַ[%s]�˿�[%hu]ʧ�� */
#define E_SOCKET_REQU               11108   /* �����ַ[%s]�˿�[%hu]����ʧ�� */
#define E_SOCKET_INIT               11109   /* ���ó�ʼ������ʧ�� */
#define E_SOCKET_TRAN               11110   /* �������Ӵ�����ʧ�� */
#define E_SOCKET_MAX                11111   /* �������󳬹����õ������[%d] */
#define E_SOCKET_TERM               11112   /* ��������������[%s]�쳣�˳� */
#define E_SOCKET_BIND               11113   /* ��ָ���׽���[%d],[%s]��ַ[%d]�˿�ʧ�� */
#define E_SOCKET_LISTEN             11114   /* �����׽���[%d],������[%d]ʧ�� */
#define E_SOCKET_SENDFDS            11115   /* �����׽���[%d] �����׽���[%d] Я��[%d]��С����ʧ�� */
#define E_SOCK_SNDCHLD              11116   /* ���[%d]���ӽ���[%d]���ʹ������׽���ʧ�� */
#define E_SOCK_NOCHLD               11117   /* û�п��õ��ӽ��� */
#define E_SOCK_ADDCONN              11118   /* �������̻�����·��Ǽ�״̬ʧ�� */
/* ADD BY HEJN 20130402*/
#define E_LICPLUG                   11119   /* ��Ʒ�����[%d]��������[%s] */
#define E_LICDTAINST                11120   /* ��Ʒʵ����[%d]��������[%s] */
#define E_LICNODE                   11121   /* ��ƷȺ���ڵ���[%d]��������[%s] */
#define E_LICERR                    11122   /* ���к���������[%d]��,����30��ƽ̨��ֹͣ���� */
#define E_LICRUN                    11123   /* License���в�����������,ƽ̨��ֹͣ���� */
/* ADD END */

/*
 * ���Ľ���ģ��         12000-12999
 */
#define E_IOF_TYPE            12000     /* ����/���[%s]������ֵ[%d]���� */
#define E_IOF_NEST            12001     /* ����[%s]���Ӹ�ʽǶ�׷Ƿ� */
#define E_IOF_GET_TYPE        12002     /* ȡ����[%s]������ʧ�� */
#define E_IOF_NEST_ITEM       12003     /* ����[%s]�ĵ�[%d]����ʽ��Ƕ�׵��Ӹ�ʽ[%s]���ͷǷ� */
#define E_IOF_IOTYPE          12004     /* ����[%s]���������[%d]���� */
#define E_IOF_SIGN_NAME       12005     /* ����౨��[%s]δ�����Ǽ����� */
#define E_IOF_BUILD           12006     /* ��֯����[%s]ʧ�� */
#define E_IOF_PARSE           12007     /* ��������[%s]ʧ�� */
#define E_IOF_ITEM_DEF        12008     /* ����[%s]�ĵ�[%d]����ʽ���Ƿ� */
#define E_IOF_ITEM_BUILD      12009     /* ��֯����[%s]�ĵ�[%d]����ʽ��ʧ�� */
#define E_IOF_CALC_SUB        12010     /* ȡ����[%s]�ĵ�[%d]����ʽ����Ӹ�ʽ����ʧ�� */
#define E_IOF_CALC_SUBREC     12011     /* ȡ����[%s]�ĵ�[%d]����ʽ����Ӹ�ʽ��¼��ʧ�� */
#define E_IOF_BLD_DATA        12012     /* ��֯����[%s]�ĵ�[%d]����ʽ�������ֵʧ�� */
#define E_IOF_CALC_ALIGN      12013     /* ���㳤��Ϊ[%d]�ı��Ķ�����[%d]��ƫ����ʧ�� */
#define E_IOF_XFML_PUT        12014     /* �洢��FML��Ǽ�[%s]�ı��[%s]ʧ�� */
#define E_IOF_XFML_FILE       12015     /* �޷����ļ�����FML��ʽ[%s]���� */
#define E_IOF_RECV_FILE       12016     /* �޷����ļ����͸�ʽ[%s]���б��ձ߽� */
#define E_IOF_RECV_BUF        12017     /* ͨѶ���������ڴ��ַ�򳤶ȷǷ�-���Э�鱨�ĵĶ����Ƿ���ȷ */
#define E_IOF_PARSE_LEN       12018     /* �ӱ��ĵĵ�[%d]���ֽڶ�ȡ����Ϊ[%d]������ʧ�� */
#define E_IOF_SOCK_GETDATA    12019     /* ��SOCKET������ȡ����ʧ�� */
#define E_IOF_PARSE_BITMAP    12020     /* ��̬λͼ��ʽ[%s]�����λͼ����� */
#define E_IOF_ITEM_PARSE      12021     /* ��������[%s]�ĵ�[%d]����ʽ��ʧ�� */
#define E_IOF_PARSE_DATA      12022     /* ��������[%s]�ĵ�[%d]����ʽ�������ֵʧ�� */
#define E_IOF_PARSE_SIGN      12023     /* �ӱ��ĵĵ�[%d]���ֽڶ�ȡָ�����[%s]ʧ�� */
#define E_IOF_EXPRTYPE        12024     /* ���ʽ[%s]��������������Ԫ��[%s]���Ͳ���ͬ */
#define E_IOF_XFML_GET        12025     /* ��ȡ��FML��Ǽ�[%s]�ı��[%s]ʧ�� */
#define E_IOF_SIGN_DEF        12026     /* ��Ǽ�[%s]������� */
#define E_IOF_GET_SIGN        12027     /* ȡXHTTP���ĵı����ʧ�� */
#define E_IOF_COMM            12028     /* ���ܶԱ���[%s]ִ�б��ձ߽⴦�� */
#define E_IOF_XFMLNULL        12029     /* ��FML���ͱ��ĵĽӿ�[%s]δ�ͻ��� */
#define E_IOF_FILENULL        12030     /* ��ʽ[%s]δ�����ļ���ȡֵ���ʽ */
#define E_IOF_XSUB_DATA       12031     /* �Ƕ����ƻ��ַ�������Ԫ��[%s]���ܴ����չ�Ӹ�ʽ���� */
#define E_IOF_XSUB_LEN        12032 /* ��չ�Ӹ�ʽ���ݳ���[%d]��������Ԫ��[%s]�������󳤶�[%d] */
#define E_IOF_EXPRTYPE_DATATYPE  12033     /* ���ʽ[%s]�����������ʽ������Ͳ���� */
#define E_IOF_SUBTYPE_DATATYPE  12034     /* ʹ����չ�Ӹ�ʽʱ��ʽ������ͱ���ΪBINARY */
#define E_IOF_DETYPE_DATATYPE  12035     /* ����Ԫ��[%s]�������ʽ������Ͳ���� */
#define E_IOF_CALCTYPE_DATATYPE  12036     /* �����������ʽ������Ͳ���� */
#define E_IOF_PFMT_XFML          12037     /* �����ױ���Ϊ��FML����ʱ,Э�鱨�Ĳ���Ϊ����FML���� */
#define E_IOF_PARSE_OVER         12038     /* ��������[%s]��λ��[%d]��������[%d] */
#define E_IOF_GETXML_ATTR        12039     /* ȡXML���Ľڵ�����[%s]ʧ�� */
#define E_IOF_NEWXML_NODE        12040     /* ����XML���Ľڵ�[%s]ʧ�� */
#define E_IOF_SETXML_ATTR        12041     /* ��XML���Ľڵ�����[%s]ʧ�� */
#define E_IOF_ARRAY_DATA         12042     /* �Ƕ���������Ԫ��[%s]���ܴ��ARRAY��չ�Ӹ�ʽ���� */
#define E_LOG_PAR_VER            12043     /* ���������ڴ�汾��:[%s]  */
#define E_LOG_DEP_VER            12044     /* �������ڴ�汾��:[%s]  */


/*
 * ����Ԫ��ģ��           14000-14999
 */
#define  E_DELEM_NOELEM     14000       /* û������Ԫ�ض��� */
#define  E_DELEM_MEM        14001       /* �ڴ����ʧ�� */
#define  E_DELEM_ELTYP      14002       /* ����Ԫ��[%s]���Ͷ���el[%d]/da[%d]���� */
#define  E_DELEM_NOEP       14003       /* ����Ԫ�س�[%d]������ */
#define  E_DELEM_NOELN      14004       /* ����Ԫ��[%s]������ */
#define  E_DELEM_IDX        14005       /* ����Ԫ��[%s]�±�[%d]����,��ǰ����[%d] */
#define  E_DELEM_NOVAL      14006       /* ����Ԫ��[%s]û��ֵ */
#define  E_DELEM_USERMEM    14007       /* �û��ṩ�ռ�Ϊ�� */
#define  E_DELEM_COMPRESS   14008       /* ѹ������ */
#define  E_DELEM_UNCOMPRESS 14009       /* ��ѹ������ */
#define  E_DELEM_TOOBIG     14010       /* ����Ԫ��[%s]ֵ����[%d]������󳤶�[%d] */
#define  E_DELEM_ELEXIST    14011       /* ����Ԫ��[%s]�Ѵ��� */
#define  E_DELEM_OPELTYP    14012       /* ���ܱ������[%s]������ */
#define  E_DELEM_TWOELTYP   14013       /* ����Ԫ��[%s]��[%s]�����Ͳ�һ�� */
#define  E_DELEM_BLDBUF     14014       /* ��֯����Ԫ�س����ݵ����������� */
#define  E_DELEM_BUFDATA    14015       /* �ֽ⻺�������ݵ�����Ԫ�سش��� */
#define  E_DELEM_DATALEN    14016       /* ����Ԫ��[%s]��ֵ����[%d]���� */
#define  E_DELEM_NOEXIST    14100       /* [%s]���Ƶ�����Ԫ[%s]������ */
#define  E_DELEM_ALLOC      14101       /* ���������ʧ�� */
#define  E_DELEM_PUT        14102       /* ����Ԫ[%s]�±�[%d]��ֵʧ�� */
#define  E_DELEM_GET        14103       /* ����Ԫ[%s]�±�[%d]ȡֵʧ�� */
#define  E_DELEM_NUMBER     14104       /* ����Ԫ[%s]������[%d]������ֵ���� */
#define  E_DELEM_TYPE       14105       /* ȡ����Ԫ[%s]������ʧ�� */
#define  E_DELEM_LEN        14106       /* ȡ����Ԫ[%s]�±�[%d]��ֵ����ʧ�� */
#define  E_DELEM_TYPELEN    14107       /* ȡ����Ԫ[%s]����[%d][%d]�ۻ�����ʧ�� */
#define  E_DELEM_DEIDX      14108       /* �±�����Ԫ��[%s]ֵ[%d]����С���� */

#define  E_DELEM_OPINIT     14109       /* ����Ԫ�ز�����¼δ��ʼ��,��ջ��Ϊ[%d]*/
#define  E_DELEM_OPMAX      14110       /* ����Ԫ�ز�����¼��ջ�Ѵ�����[%d] */
#define  E_DELEM_XCOPY      14111       /* ������[%d][%s]ת����[%d][%s]ʧ�� */
#define  E_DELEM_SYSVAR_COPY      14112 /* ������[%d]��ϵͳ����Ԫת����[%d]ʧ�� */
#define  E_DELEM_EXT        14113       /* ���������޷����� */
#define  E_DELEM_COPY       14114       /* ������[%d][%s]���Ƶ�[%d][%s]ʧ�� */
#define  E_DELEM_NODAEL     14115       /* ����Ԫ��[%s]�Ķ��岻���� */
#define  E_DELEM_NOTINCPT   14116       /* ����Ԫ��ֵ����[%s]��Ԫ������б��в����� */

/*
 * routing module
 */
#define E_ROUTE_DEFINE              16000   /* DTA[%s]��·�ɶ���� */
#define E_ROUTE_RULE                16001   /* δ����DTA[%s]����[%s]��·�ɹ��� */
#define E_ROUTE_EXPR                16002   /* DTA[%s]·�ɹ���[%s]�ı��ʽ[%s]������� */
#define E_ROUTE_ENTR                16003   /* δ������DTA[%s]·�ɹ���[%s]�ı��ʽֵ[%s]��Ӧ��·����� */
#define E_ROUTE_MACHN               16004   /* DTA[%s]�Ļ���������� */
#define E_ROUTE_BAL                 16005   /* �������[%s]�ϵ�DTA[%s]�ĸ��ؾ���ʧ�� */
#define E_ROUTE_CALC                16006   /* DTA[%s]�Ϸ���[%s]��·�ɼ������ */
#define E_ROUTE_PARA                16007   /* û���ڲ��������ж���DTA[%s]��·��[%s]�����[%s]�Ļ������� */
#define E_ROUTE_DTA_NORUN           16008   /* û�����и�DTA[%s]�Ļ��� */
#define E_ROUTE_MCHDTA_NORUN        16009   /* ����[%s]û�����и�DTA[%s] */
#define E_ROUTE_SVCSTOP             16010   /* ��Ŀ�ķ�����ֹͣ,·��ʧ�� */
#define E_ROUTE_SUBSYS              16011   /* ����ϵͳ����,·��ʧ�� */

/*
 * ���ʽģ��             17000-17999
 */
#define E_SLANGINIT                 17000   /* S-Lang��ʼ��ʧ�� */
#define E_SLANGLOAD_PUB             17001   /* S-Langװ��ϵͳ�����ļ�[%s]ʧ�� */
#define E_SLANGLOAD_COM             17002   /* S-Langװ��ͨ�Žӿ��ļ�[%s]ʧ�� */
#define E_SLANGLOAD_APP             17003   /* S-Langװ��ҵ���ļ�[%s]ʧ�� */
#define E_SLANGLOAD_SUB             17004   /* S-Langװ����ҵ���ļ�[%s]ʧ�� */
#define E_SLANGLOAD_DTA             17005   /* S-Langװ��DTA�ļ�[%s]ʧ�� */

#define E_PUT_VAR                   17101   /* ����Ԫ��[%s]��ֵʧ��! */
#define E_APP_FUNC                  17102   /* Ӧ�ú���(%s)���ó���! */
#define E_GET_VAR                   17103   /* ����Ԫ��[%s]ȡֵʧ��! */
#define E_DATA_TYPE                 17104   /* ƽ̨����(%s)���ó���,���������Ͳ���ȷ! */
#define E_CALC                      17105   /* ƽ̨����(%s)���ó���,�������! */
#define E_STR_CALC                  17106   /* �ַ������ݲ����˼����ˡ�������! */
#define E_DIV_IS_ZERO               17107   /* ����Ϊ��! */
#define E_SYNTAX                    17108   /* �﷨������λ��[%d!][%s...] */
#define E_GET_EXPR_VAL              17109   /* ���ʽ��[%d]�������ֵ����! */

/*
 * �����ڴ�ģ��           18000-18999
 */
#define    E_SHMDB_LOAD_XML         18000   /* װ�ؽڵ�[%s]��XML�ļ�[%s]��Ϣʧ�� */
#define    E_SHMDB_LOAD_PAR         18001   /* װ�ز�������[%s]������Ϣʧ�� */
#define    E_SHMDB_LOAD_DEP         18002   /* װ��ҵ����[%s]������Ϣʧ�� */
#define    E_SHMDB_LOAD_PROJECT     18003   /* װ�ز�����Դ��[%s]������Ϣʧ�� */
#define    E_SHMDB_LOAD_DTA         18004   /* װ�ز���DTA[%s]������Ϣʧ�� */
#define    E_SHMDB_LOAD_DTA_INST    18005   /* װ�ز���DTAʵ��[%s]������Ϣʧ��*/
#define    E_SHMDB_LOAD_DTA_NODE    18006   /* װ�ز���DTA�ڵ�[%s]������Ϣʧ��*/
#define    E_SHMDB_LOAD_DTA_SVC     18007   /* װ�ز���DTA����[%s]������Ϣʧ��*/
#define    E_SHMDB_LOAD_REV_SVC     18008   /* װ�ز����������[%s]������Ϣʧ�� */
#define    E_SHMDB_LOAD_FMT_GRP     18009   /* װ�ز����ʽ��[%s]������Ϣʧ�� */
#define    E_SHMDB_LOAD_ELEM        18010   /* װ�ز�������Ԫ[%s]������Ϣʧ�� */
#define    E_SHMDB_LOAD_MAP         18011   /* װ�ز�������ӳ��[%s]������Ϣʧ�� */
#define    E_SHMDB_LOAD_SIGN        18012   /* װ�ز�����[%s]������Ϣʧ�� */
#define    E_SHMDB_LOAD_SIGN_ITEM   18013   /* װ�ز�������[%s]������Ϣʧ�� */
#define    E_SHMDB_LOAD_FORMAT      18014   /* װ�ز����ʽ[%s]������Ϣʧ�� */
#define    E_SHMDB_LOAD_FMT_ITEM    18015   /* װ�ز����ʽ��[%s]������Ϣʧ�� */
#define    E_SHMDB_LOAD_RULE        18016   /* װ�ز���·�ɹ���[%s]������Ϣʧ�� */
#define    E_SHMDB_LOAD_ENTRANCE    18017   /* װ�ز���·�����[%s]������Ϣʧ�� */
#define    E_SHMDB_LOAD_ALA         18018   /* װ�ز�����ҵ��[%s]������Ϣʧ�� */
#define    E_SHMDB_LOAD_FLOW        18019   /* װ�ز�������[%s]������Ϣʧ�� */
#define    E_SHMDB_LOAD_LOGIC       18020   /* װ�ز����߼�[%s]������Ϣʧ�� */
#define    E_SHMDB_LOAD_FORM        18021   /* װ�ز���FORM[%s]������Ϣʧ�� */
#define    E_SHMDB_LOAD_TASK        18022   /* װ�ز�������[%s]������Ϣʧ�� */
#define    E_SHMDB_INIT_DTA_DE_COL  18023   /* װ�ز���DTA����Ԫ����[%s]������Ϣʧ�� */
#define    E_SHMDB_MERGE_ALL        18024   /* �ϲ����ɹ����ڴ�ͷ�ṹʧ�� */
#define    E_SHMDB_DTA_NUM          18025   /* ȡDTA��Ŀʧ�� */
#define    E_SHMDB_IPC              18026   /* ȡIPC PERM��ʶʧ�� */
#define    E_SHMDB_CREATE_SHM       18027   /* ����[%d]�ֽڹ����ڴ�ʧ�� */
#define    E_SHMDB_SHM_REM_KEY      18028   /* ͨ����ֵ[%s]ɾ�������ڴ�ʧ�� */
#define    E_SHMDB_SHM_ATT_KEY      18029   /* ͨ����ֵ[%s]���ӹ����ڴ�ʧ�� */
#define    E_SHMDB_SHM_DETACH       18030   /* �Ͽ������ڴ�ʧ�� */
#define    E_SHMDB_XML_PARSE        18031   /* ����XML�ļ�[%s]ʧ�� */
#define    E_SHMDB_XML_ROOT         18032   /* ȡXML�ļ�[%s]���ڵ���Ϣʧ�� */
#define    E_SHMDB_NODE_NAME        18033   /* ��������[%s]ȡ�ӽڵ���Ϣʧ�� */
#define    E_SHMDB_ATTR_NAME        18034   /* ȡ�ڵ�[%s]��������Ϣʧ�� */
#define    E_SHMDB_ATTR_LIM         18035   /* �ڵ�[%s]������[%s]�붨�岻�� */
#define    E_SHMDB_XML_REC          18036   /* [%s]��¼��[%d]��ʵ����[%d]���� */
#define    E_SHMDB_XML_NAME         18037   /* ȡ�ڵ�[%s]������ʧ�� */
#define    E_SHMDB_APPXML_LOAD      18038   /* ҵ��[%s]װ��[%s]XML��������ʧ�� */
#define    E_SHMDB_SUBAPPXML_LOAD   18039   /* ��ҵ��[%s]װ��[%s]XML��������ʧ�� */
#define    E_SHMDB_PTR_NULL         18049   /* ָ�����[%s]Ϊ�� */
#define    E_SHMDB_ADDBUF           18050   /* [%s]�ڴ����ӵ�[%d]�ڵ�ʧ�� */
#define    E_SHMDB_PAR_MAXALA       18060   /* ��ҵ��ALA������������[%d] */
#define    E_SHMDB_PAR_MAXALAMCH    18061   /* ��ҵ��ALA���л���������������[%d] */
#define    E_SHMDB_PAR_MAXDTA       18062   /* ͨ��DTA������������[%d] */
#define    E_SHMDB_PAR_MAXDTAMCH    18063   /* ͨ��DTA���л���������������[%d] */
#define    E_SHMDB_PAR_MAXDTANODE   18064   /* ͨ��DTA���нڵ������������[%d] */
#define    E_SHMDB_PAR_MAXFS        18065   /* �ļ�������������������[%d] */
#define    E_SHMDB_PAR_MAXMCH       18066   /* �������������������[%d] */
#define    E_SHMDB_PAR_MAXRUT       18067   /* ·�ɱ�ļ�¼����������[%d] */
#define    E_SHMDB_PROJECT          18100   /* ȡҵ�������ڴ����Դ����Ϣʧ�� */
#define    E_SHMDB_DRQ_NAME         18101   /* ������DTA������[%s]�����DTA�Ƿ����� */
#define    E_SHMDB_DTA_NAME         18102   /* ������DTA��[%s] */
#define    E_SHMDB_DTAINST_NAME     18103   /* ����DTA��[%s]ȡ[%d]��ʵ����Ϣʧ��*/
#define    E_SHMDB_DTANODE_NAME     18104   /* ����DTA��[%s]ȡ�ڵ�[%s]��Ϣʧ��*/
#define    E_SHMDB_DTASVC_NAME      18105   /* ����DTA��[%s]ȡ����[%s]��Ϣʧ��*/
#define    E_SHMDB_REVSVC_NAME      18106   /* ����DTA��[%s]ȡ�����ͻ���[%s]��Ϣʧ��*/
#define    E_SHMDB_FMTGRP_NAME      18107   /* ����DTA��[%s]ȡ����[%s]��������Ϣʧ��*/
#define    E_SHMDB_ELEM_NAME        18108   /* ����������Ԫ��[%s] */
#define    E_SHMDB_DTAMAP_NAME      18109   /* ������DTA[%s]��DTA[%s]��ӳ�� */
#define    E_SHMDB_SIGN_NAME        18110   /* �����ڱ����[%s] */
#define    E_SHMDB_FORMAT_NAME      18111   /* �����ڸ�ʽ��[%s] */
#define    E_SHMDB_RULE_NAME        18112   /* ������·�ɹ�����[%s] */
#define    E_SHMDB_ENTRANCE_NAME    18113   /* ������·�������[%s] */
#define    E_SHMDB_ALA_NAME         18114   /* ������ALA��[%s] */
#define    E_SHMDB_TRAN_NAME        18115   /* �����ڽ�����[%s] */
#define    E_SHMDB_FLOW_NAME        18116   /* �����ڽ���[%s]��Ӧ������[%s]���� */
#define    E_SHMDB_FLOW_NOSEQ       18117   /* �����ڽ���[%s]˳������� */
#define    E_SHMDB_FLOW_TRANSEQ     18118   /* �����ڽ���[%s]����[%s]��˳���[%d]���� */
#define    E_SHMDB_FORM_NAME        18119   /* ����������Ԫ������[%s] */
#define    E_SHMDB_BTASK_NAME       18120   /* ����������������[%s] */
#define    E_SHMDB_ATASK_NAME       18121   /* �������Զ�������[%s] */
#define    E_SHMDB_MAX_MAP_ITEM     18122   /* ӳ��Ŀ��DTA[%s]���ֶθ���[%d]��������[%d] */
#define    E_SHMDB_APP_NAME         18130   /* ������ҵ��[%s]����Դ���ڵ� */
#define    E_SHMDB_SUBAPP_NAME      18131   /* ��������ҵ��[%s]����Դ���ڵ� */
#define    E_SHMDB_MCH_NAME         18150   /* �����ڻ���������[%s] */
#define    E_SHMDB_MCH_NOFSRV       18151   /* �����ڻ�����[%s]���ļ�������� */
#define    E_SHMDB_MCH_DTA          18152   /* �����ڻ���[%s]��DTA[%s] */
#define    E_SHMDB_MCH_ALA          18153   /* �����ڻ���[%s]��ALA[%s] */
#define    E_SHMDB_DATATYPE         18154   /* ������[%s]���� */
#define    E_SHMDB_DRQ_TBL          18155   /* ������DTA[%s]��[%d]��DRQ���� */
#define    E_SHMDB_PAR_MAXSVC       18156   /* ���������������[%d] */

#define    E_SHMDB_PAR_HDR          18200   /* ȡ�����������ڴ�ͷʧ�� */
#define    E_SHMDB_RUN_HDR          18201   /* ȡ���в��������ڴ�ͷʧ�� */
#define    E_SHMDB_DEP_HDR          18202   /* ȡҵ�������ڴ�ͷʧ�� */
#define    E_SHMDB_PAR_ATT          18203   /* ���Ӳ����������ڴ�ʧ�� */
#define    E_SHMDB_DEP_ATT          18204   /* ����ҵ�������ڴ�ʧ�� */
#define    E_SHMDB_PAR_CHK          18205   /* �������������ڴ�ʧ�� */
#define    E_SHMDB_DEP_CHK          18206   /* ���ҵ�������ڴ�ʧ�� */
#define    E_SHMDB_PAR_LOAD         18207   /* װ�ز����������ڴ�ʧ�� */
#define    E_SHMDB_DEP_LOAD         18208   /* װ��ҵ�������ڴ�ʧ�� */
#define    E_SHMDB_PAR_REMOVE       18209   /* ɾ�������������ڴ�ʧ�� */
#define    E_SHMDB_DEP_REMOVE       18210   /* ɾ��ҵ�������ڴ�ʧ�� */
/* 20091016 handq for 3.2 */
#define    E_SHMDB_RUN_BASE         18211   /* �����������й����ڴ�ʧ�� */
#define    E_SHMDB_RUN_AUTOTASK     18212   /* �����Զ�����״̬��չ���й����ڴ�ʧ�� */
#define    E_SHMDB_RUN_BATTASK      18213   /* ������������״̬��չ���й����ڴ�ʧ�� */
#define    E_SHMDB_RUN_DTANODE      18214   /* ����DTA�ڵ�״̬���й����ڴ�ʧ�� */
#define    E_SHMDB_RUN_TRAN         18215   /* �������׼�����״̬���й����ڴ�ʧ�� */
#define    E_SHMDB_RUN_CHK          18216   /* �������״̬�����ڴ�ʧ�� */
#define    E_SHMDB_RUN_REMOVE       18217   /* ɾ������״̬�����ڴ�ʧ�� */


#define    E_SHMDB_MAXINSTNUM       18301   /* ���е�DTAʵ��������ƽ̨����[%d] */
#define    E_SHMDB_RUNINIT_LOCAL    18302   /* ��ʼ���������в���ʧ�� */
#define    E_SHMDB_RUNINIT_SYNC     18303   /* ��ʼ��ͬ�����в���ʧ�� */
#define    E_SHMDB_RUNINIT_DTAINST  18304   /* ��ʼ��DTAʵ�����в���ʧ�� */
#define    E_SHMDB_MCH_LISTEN_ADDR  18305   /* �����ڼ����ڵ�ַ[%s:%hu]�ϵĻ���,���ESAdmin.xml�Ļ������� */
#define    E_SHMDB_RUNINIT_FSINST   18306   /* ��ʼ���ļ�����ʵ�����в���ʧ�� */
#define    E_SHMDB_RUN_NOFSRV       18307   /* ȡ�ļ�����ʵ�����в���[%s]ʧ�� */
#define    E_SHMDB_RUN_NOMCH        18308   /* ȡ����[%s]��״̬��Ϣʧ�� */
#define    E_SHMDB_RUN_NODTA        18309   /* ȡ����[%s]��DTA[%s]״̬��Ϣʧ�� */
#define    E_SHMDB_RUN_DTASTAT      18310   /* ����[%s]��DTA[%s]״̬Ϊֹͣ */
#define    E_SHMDB_LOAD_FUNCCHK     18311   /* װ�ؼ�麯������ֵ�б�ʧ�� */
#define    E_SHMDB_GET_FUNCCHK      18312   /* ȡ�ڵ�[%s]��麯������ֵ�б�[%s]ʧ�� */
#define    E_SHMDB_LOAD_FMTRPT      18313   /* װ�ظ�ʽ����ʧ�� */
#define    E_SHMDB_GET_FMTRPT       18314   /* ȡ�ڵ�[%s]��ʽ����[%s]ʧ�� */
#define    E_SHMDB_PAR_MAXDB        18315   /* ����������ݿ���� */
#define    E_SHMDB_PAR_MAXPRIOINST  18316   /* DTA/ALA�������ȼ�ʵ������������[%d] */
/* ADD BY PENGBIN 2007/3/13 */
#define    E_SHMDB_DTA_PARL         18317   /* ������DTA[%s]����[%s]�Ĳ�������Ϣ */
#define    E_DTA_PARL               18318   /* DTA[%s]����[%s]������[%hd]����[%hd] */
#define    E_DTA_PARL_LVL           18319   /* DTA[%s]����[%s]���ȼ�[%hd]������[%hd]����[%hd] */
#define    E_SHMDB_PAR_MAXDTAPARL   18320   /* DTA���������ø�����������[%d] */
#define    E_SHMDB_RUNINIT_DTAPAL   18321   /* ��ʼ��DTA���������в���ʧ�� */
#define    E_SHMDB_RUNINIT_CUSTPAL  18322   /* ��ʼ���ͻ�����Ŀ���������в���ʧ�� */
#define    E_SHMDB_CUST_PAL         18323   /* ����ԴDTA[%s]�ͻ�����Ŀ[%s]����������ʧ�� */
#define    E_CUST_PAL               18324   /* ԴDTA[%s]�ͻ�����Ŀ[%s]������[%hd]����[%hd] */
#define    E_SHMDB_CUSTPAL_MAX      18325   /* �ͻ�����Ŀ�����Ѵ����ֵ[%d] */
#define    E_SVCID_RUN_INST         18326   /* DTA[%s]�Ѿ���ʵ��[%d]��ִ�к������������ID[%s]�Ľ��� */
#define    E_SHMDB_SVCID            18327   /* ��ѯ�����ӷ������ID[%s]����Ϣʧ�� */
#define    E_SHMDB_RUNINIT_SVCID    18328   /* ��ʼ���������ID���������в���ʧ�� */
#define    E_SHMDB_SVCID_MAX        18329   /* �������ID��������������[%d] */
#define    E_SVCID_RUN              18330   /* �������ID[%s]���Ϊ������,������[%d]�ʽ������ڴ��� */
/* ADD END */
#define    E_SHMDB_MONINIT          18331   /* ��ʼ��ͬ����ع����ڴ�ʧ�� */
#define    E_SHMDB_MON_MCH          18332   /* �����ڻ���[%s]��ͬ����Ϣ */
#define    E_SHMDB_MON              18333   /* ��ع����ڴ���� */
#define    E_SHMDB_RUNINIT_BAL      18334   /* ��ʼ�����ؾ�����ƽṹʧ�� */
#define    E_SHMDB_PAR_MAXDTACONN   18335   /* ͨ��DTA���пͻ���IP������������ */
#define    E_SHMDB_RUNINIT_LNS      18336   /* ��ʼ�������ӵǼ���ʧ�� */
#define    E_SHMDB_PAR_MAXLISTEN    18337   /* DTA������ַ�б���������[%d]���� */
#define    E_SHMDB_MAXTZNUM         18338   /* ����[%s]��ʱ��θ���[%d]��������[%d] */
#define    E_SHMDB_MAXES_TYPE       18339   /* δ֪��ϵͳ���ֵ����[%d] */
#define    E_SHMDB_MAXDRQSEQNUM     18340   /* DRQ��ˮ�Ų�����������������[%d]���� */
#define    E_SHMDB_SHMVER           18341   /* ����[%d]�汾[%s]�Ĺ����ڴ�״̬������ */
#define    E_SHMDB_NOIDLE           18342   /* ����[%d]�Ĺ����ڴ治Ϊ����״̬ */
#define    E_MAX_SHMVER             18343   /* ��󲢴湲���ڴ���[%d]С�ڸ��µ�˳���[%d] */
#define    E_SHMDB_RUNINIT_MAXES    18344   /* ��ʼ�����й������ֵ����ʧ�� */

#define    E_SHMDB_MAX_TRAN         18345   /* ����״̬����[%d]��������[%d] */
#define    E_SHMDB_NOEXIT           18346   /* �����ڴ�[%s]������ */
#define    E_SHMDB_SHM_ATT_ID       18347   /* ���ݹ����ڴ�ID��[%d]���ӹ����ڴ�ʧ�� */
#define    E_SHMDB_SHM_REM_ID       18348   /* ͨ����ʶ��shmid[%d]ɾ�������ڴ�ʧ�� */
#define    E_SHMDB_RUNREF_SVCSTAT   18349   /* ˢ�·���״̬�����ڴ�ʧ�� */
#define    E_SHMDB_RUNREF_NODESTAT  18350   /* ˢ�½ڵ�״̬�����ڴ�ʧ�� */
#define    E_SHMDB_RUNREF_ATASK     18351   /* ˢ���Զ�����״̬�����ڴ�ʧ�� */
#define    E_SHMDB_RUNREF_BTASK     18352   /* ˢ����������״̬�����ڴ�ʧ�� */
#define    E_SHMDB_RUNREF_TRANSTAT  18353   /* ˢ���߼��Ͳ���״̬�����ڴ�ʧ�� */
#define    E_SHMDB_RUN_DTASVC       18354   /* ����DTA����״̬���й����ڴ�ʧ�� */
#define    E_SHMDB_VER_EXIST        18355   /* �����ڴ�����[%d]�汾[%s]�Ѿ����� */
#define    E_SHMDB_DTASVCSTAT       18356   /* ȡDTA[%s]�Ϸ���[%s]״̬ʧ�� */
#define    E_SHMDB_SET_DTASTAT      18357   /* ����DTA[%s]��״̬[%d]ʧ�� */
#define    E_SHMDB_INVALMAXES       18358   /* ���ֵ����[%s]�Ƿ� */
#define    E_SHMDB_REFSTATBYDEP     18359   /* ���ݲ������ڴ�[%s]ˢ��״̬�����ڴ�ʧ�� */
#define    E_SHMDB_REFSTATBYPAR     18360   /* ���ݲ��������ڴ�[%s]�������ڴ�[%s]ˢ��״̬�����ڴ�ʧ�� */
#define    E_SHMDB_REFSTAT          18361   /* ���ݲ��������ڴ�[%s]�Ͳ������ڴ�[%s]ˢ��״̬�����ڴ�ʧ�� */
#define    E_SHMDB_MAXSTOPSVCNUM    18362   /* ֹͣ����״̬�����������ֵ[%d]���� */
#define    E_SHMDB_GET_STOPSVC      18363   /* ȡ����[%s]��DTA[%s]����[%s]��ֹͣ״̬ʧ�� */
#define    E_SHMDB_RESTOR           18364   /* ��ӳ���ļ��ָ��汾Ϊ[%s]�Ĺ����ڴ�ʧ�� */
#define    E_SHMDB_CURVER           18365   /* ����[%d]�汾[%s]���ǵ�ǰ�����ڴ�汾,����ɾ�� */
#define    E_SHMDB_DEP_CREATE       18366   /* ������[%d]���������ڴ�ʧ�� */
#define    E_SHMDB_PAR_CREATE       18367   /* ������[%d]�����������ڴ�ʧ�� */
#define    E_SHMDB_DDTA_BYREF       18368   /* ��ΪDTA[%s]��ɾ��,ƽ̨��ֹͣ��DTA����Чˢ�� */
#define    E_SHMDB_DNODE_BYREF      18369   /* ��Ϊ�����ڵ�[%s]��ɾ��,ƽ̨��ֹͣ��DTA����Чˢ�� */
#define    E_SHMDB_DMAST_BYREF      18370   /* �²���ɾ���˵�ǰ���ؽڵ�[%s],������Ч */
#define    E_SHMDB_PAR_ACTFAIL      18371   /* ���������ڴ�ԤԼˢ����Чʧ�� */
#define    E_SHMDB_DEP_ACTFAIL      18372   /* �������ڴ�ԤԼˢ����Чʧ�� */
#define    E_SHMDB_PAR_ACTSUCC      18373   /* ���������ڴ�ԤԼˢ����Ч�ɹ� */
#define    E_SHMDB_DEP_ACTSUCC      18374   /* �������ڴ�ԤԼˢ����Ч�ɹ� */
#define    E_SHMDB_RCYCL_DEP        18375   /* �������ڴ�[%s]��ʱ��δʹ�ñ����� */
#define    E_SHMDB_RCYCL_PAR        18376   /* ���������ڴ�[%s]��ʱ��δʹ�ñ����� */
#define    E_SHMDB_RCYCL_NOREC      18377   /* ���������ڴ�[%d:%s]����δ�ҵ��汾δ���� */
#define    E_SHMVER_RSTMAX          18388   /* ��󲢴湲���ڴ���[%d]С�ڸ��µ�˳���[%d],�汾[%s]δ�ָ� */
#define    E_SHMDB_SHMVER_INACTIVE  18389   /* ����[%d]�汾[%s]�Ĺ����ڴ�״̬[%d]���ǿ���״̬,���������� */
#define    E_SHMDB_RUNSET_DTAINST  18390   /* ����DTAʵ��״̬ʧ�� */
#define    E_SHMDB_PAR_ATASKMMCH    18391   /* ��Ⱥģʽ��,�Զ�����δ�������ؽڵ�[%s]�������� */
#define    E_SHMDB_MAXSVRDYNCON       18392   /* ����������еĶ�̬����ϵͳ��������Ϣ */
#define    E_SHMDB_SVCDEF       18393 /*��ȡ����ϵͳ[%s]�������[%s]�Ŀ�����Ϣʧ�� */
#define    E_SHMDB_SVRSYS_NOTFOUND       18394 /* δ�������ϵͳ������������Ϣ�����Դ��� */


/*
 * eslisten         19000 - 19999
 */
#define    E_ESL_START_DAEMON       19001   /* ������פ����[%s]ʧ�� */
#define    E_ESL_ESLISTEN_BOOT      19002   /* ESLISTEN����ʧ�� */
#define    E_ESL_COMM               19003   /* ESLISTEN��������ͨ��ʧ�� */
#define    E_ESL_REQUEST            19004   /* ESLISTEN���ķǷ� */
#define    E_ESL_REQ_HEAD           19005   /* ESLISTEN����ͷ�Ƿ� */
#define    E_ESL_SHMPAR_LOAD        19006   /* װ�ز����������ڴ�ʧ�� */
#define    E_ESL_DTAM_EXIT          19007   /* DTA�������[%s]�˳� */
#define    E_ESL_LISTEN_EXIT        19008   /* ESLISTEN���������˳� */
#define    E_ESL_REQ_PROC           19009   /* ESLISTEN��������[%s]��������[%s]ʧ�� */
#define    E_ESL_LOCK               19010   /* ESLISTEN��[%s]������ */
#define    E_ESL_ISSUE_EXIT         19011   /* ��������[%s]�˳� */
#define    E_ESL_PARM_ATTACH        19012   /* ���ӹ������ڴ�ʧ�� */
#define    E_ESL_ISSUE              19013   /* �������в���ʧ�� */
#define    E_ESL_ISSUE_GATHER       19014   /* �ɼ��ڵ�[%s]ͬ����Ϣʧ�� */
#define    E_ESL_SHM_SYNC           19015   /* �ڵ�ͬ����Ϣ�洢�� */
#define    E_ESL_ISSUE_UPDATE       19016   /* ����ͬ����Ϣʧ�� */
#define    E_ESL_SHM_PARM           19017   /* �ڵ������Ϣ�洢�� */
#define    E_ESL_ISSUE_OFFLINE      19018   /* ��������[%s],�������ڵ��޷����´ӽڵ�[%s]״̬,���ôӽڵ�ΪOFFLINE */
#define    E_ESL_ISSUE_BUILD        19019   /* ��֯������Ϣʧ�� */
#define    E_ESL_ISSUE_PARSE        19020   /* ����ͬ����Ϣ����[%s]ʧ�� */
#define    E_ESL_SYNC_EXIT          19021   /* ͬ������[%s]�˳� */
#define    E_ESL_SYNC_CHLD_MAX      19022   /* ͬ�������������������ڵ��� */
#define    E_ESL_RCYCL_EXIT         19023   /* �������ս���[%s]�˳� */
#define    E_ESL_RCYCL_TRAN         19024   /* ������������ʧ�� */
#define    E_ESL_RCYCL_BUFATT       19025   /* ���ݼ�ֵ[%d]�������ݻ�����ʧ�� */
#define    E_ESL_DEP_ATTACH         19026   /* ���Ӳ������ڴ�ʧ�� */
#define    E_ESL_RCYCL_DRQFAIL      19027   /* ����DTA[%s]����[%s]����������ʧ�� */
#define    E_ESL_DTAM_RCYCL         19028   /* DTA���������������ʧ�� */
#define    E_ESL_DTAM_BDTA          19029   /* ����DTA[%s]ʧ�� */
#define    E_ESL_DTAM_DDTA          19030   /* ֹͣDTA[%s]ʧ�� */
#define    E_ESL_DTAM_RBDTA         19031   /* ����[%d]��ֹ��,ƽ̨����ʧ�� */
#define    E_ESL_DTAM_BDTA_SUCC     19032   /* ����DTA[%s]��[%d]��ʵ�� */
#define    E_ESL_DTAM_DDTA_SUCC     19033   /* ֹͣDTA[%s]�ɹ� */
#define    E_ESL_DTAM_PID           19034   /* ����[%d]δ��ϵͳ�еǼ� */
#define    E_ESL_DTAM_RBDTA_SUCC    19035   /* ������DTA[%s]����[%d]�ɹ� */
#define    E_ESL_DTAM_FLEX          19036   /* DTA������̶�̬����ʧ�� */
#define    E_ESL_DTAM_BDTA_FLEX     19037   /* ��̬����ģ������DTA[%s]һ��ʵ��[%d] */
#define    E_ESL_DTAM_DDTA_FLEX     19038   /* ��̬����ģ��ֹͣDTA[%s]һ��ʵ��[%d] */
#define    E_ESL_DTAM_BDTA_FLEX_F   19039   /* ��̬����ģ������DTA[%s]һ��ʵ��[%d]ʧ�� */
#define    E_ESL_DTAM_RBDTA_FAIL    19040   /* ������DTA[%s]һ��ʵ��ʧ�� */
#define    E_ESL_DTAM_DDTA_FLEX_F   19041   /* ��̬����ģ��ֹͣDTA[%s]һ��ʵ��[%d]ʧ�� */
#define    E_ESL_DTAM_DDTA_INST     19042   /* ֹͣDTA[%s]��[%d]ʵ��ʧ�� */
#define    E_ESL_DTAM_BDTA_INST     19043   /* ����DTA[%s]��[%d]ʵ��ʧ�� */
#define    E_ESL_PKG_NULL           19044   /* ����������Ϊ�� */
#define    E_ESL_PKG_FIELD          19045   /* ����������ֻ��[%d]���� */
#define    E_ESL_PKG_XMLPARSE       19046   /* ����XML��������ʧ�� */
#define    E_ESL_PKG_XMLBUILD       19047   /* ��֯XML��������ʧ�� */
#define    E_ESL_FIELD_PARSE        19048   /* ȡ��[%d]�����ֵʧ�� */
#define    E_ESL_SLAVE_CMD          19049   /* �ӽڵ�[%s]�ϲ���ִ������[%s],��Ϊ��������Ҫת���������ڵ�[%s] */
#define    E_ESL_REQ_PROC_REMOTE    19050   /* Զ�̽ڵ�[%s]��������[%s]��������[%s]ʧ�� */
#define    E_ESL_DRQ_INIT           19051   /* DRQ��ʼ��ʧ�� */
#define    E_ESL_DTA_PROC           19052   /* [%s]DTA[%s]ʧ�� */
#define    E_ESL_MODE_SWITCH        19053   /* ϵͳ�ɹ��л���״̬[%s] */
#define    E_ESL_MCH_INVALID        19054   /* ������ָ���Ļ�����[%s] */
#define    E_ESL_DTAM_REQUEST       19055   /* DTA������̴���[%s]������ʧ�� */
#define    E_ESL_DTAM_FAILURE       19056   /* DTA������̴���ʧ�� */
#define    E_ESL_PKG_LENFIELD       19057   /* ���������ĵ�[%d]��������Ƿ� */
#define    E_ESL_DTAM_CLRDRQ        19058   /* ���DTA[%s]�Ķ�����Դʧ�� */
#define    E_ESL_ESLISTEN_BOOT_SUCC 19059   /* ESLISTEN�������������ɹ� */
#define    E_ESL_DTAM_BOOT_SUCC     19060   /* DTA�������[%s]�����ɹ� */
#define    E_ESL_ISSUE_BOOT_SUCC    19061   /* ��������[%s]�����ɹ� */
#define    E_ESL_RCYCL_BOOT_SUCC    19062   /* �������ս���[%s]�����ɹ� */
#define    E_ESL_SYNC_BOOT_SUCC     19063   /* ͬ������[%s]�����ɹ� */
#define    E_ESL_CUST_START_SUCC    19064   /* �ͻ��������ű�[%s]ִ�гɹ� */
#define    E_ESL_CUST_SHUT_SUCC     19065   /* �ͻ�����ֹ�ű�[%s]ִ�гɹ� */
#define    E_ESL_CUST_START_FAIL    19066   /* �ͻ��������ű�[%s]ִ��ʧ�� */
#define    E_ESL_CUST_SHUT_FAIL     19067   /* �ͻ�����ֹ�ű�[%s]ִ��ʧ�� */
#define    E_ESL_EXEC_DAEMON_FAIL   19068   /* ESLISTENִ�н���[%s]ʧ�� */
#define    E_ESL_DTAM_EXIST         19069   /* DTA�������[%d]�Ѿ����� */
#define    E_ESL_SYNC_EXIST         19070   /* ͬ������[%d]�Ѿ����� */
#define    E_ESL_ISSUE_EXIST        19071   /* ��������[%d]�Ѿ����� */
#define    E_ESL_RCYCL_EXIST        19072   /* �������ս���[%d]�Ѿ����� */
#define    E_ESL_DTAM_ISRUN         19073   /* DTA[%s]�Ѿ�������״̬ */
#define    E_ESL_DTAM_RUNPLAT       19074   /* ��������ƽ̨����DTA[%s]��[%d]��ʵ��... */
#define    E_ESL_DTAM_RUNTRIG       19075   /* ����������������DTA[%s]�ĵ�һ��ʵ��... */
#define    E_ESL_DTAM_RUNUSER       19076   /* �ֹ�����DTA[%s]�ݲ�����ʵ��... */
#define    E_ESL_MODE_CHK_VER       19077   /* �����ڵ�[%s]�İ汾��������ʼͬ��XML�����ļ�... */
#define    E_ESL_DTAM_DOWN_SVC      19078   /* ����[%d]�ķ�����[%s]���ж� */
#define    E_ESL_RECV_CMD           19079   /* ESLISTEN�յ��ͻ���[%s]����[%s:%s:%s]�����д���... */
#define    E_ESL_SEND_CMD           19080   /* ESLISTEN��ɿͻ���[%s]����[%s:%s:%s]���� */
#define    E_ESL_SYS_BUSY           19081   /* ϵͳ�����ڴ�����[%s]���޷�ˢ�� */
#define    E_ESL_RECV_ESCMD         19082   /* ESCMD�յ��ͻ���[%s]����[%s]�����д���... */
#define    E_ESL_SEND_ESCMD         19083   /* ESCMD��ɿͻ���[%s]����[%s]���� */
#define    E_ESL_ISSUE_GOSINFO      19084   /* �ɼ��ڵ�[%s]����ϵͳ��Ϣʧ�� */
#define    E_ESL_MONC               19085   /* �ɼ������ڵ�����Ϣʧ�� */
#define    E_ESM_FMT_CONTENT        19086   /* ������Դ����[%s]��ʽ���� */
#define    E_ESM_TYPE_CONTENT       19087   /* ������Դ����[%s]����δ֪���� */
#define    E_ESL_SET_MASTER         19088   /* ���������ڵ�[%s]ʧ�� */
#define    E_ESL_NEW_MASTER         19089   /* ���ڵ㽫��Ϊ����[%s] */
#define    E_ESL_NODE_STATUS        19090   /* ���ڵ�[%s]����״̬ʧ�� */
#define    E_ESL_MNODE              19091   /* ��ѡ���ڵ㴦��ʧ�� */
#define    E_ESL_NNODE              19092   /* �ӽڵ㴦��ʧ�� */
#define    E_ESL_RESET              19093   /* �漰����[%s]�ĸ��ظ�λʧ��*/
#define    E_ESL_RESET_LOCAL        19094   /* ��λ����������Ϣʧ�� */
#define    E_ESL_RESET_MCH          19095   /* ��λ����[%s]���������ʧ�� */
#define    E_ESL_TSF_XCMD           19096   /* ����[%s %s]ת��������[%s]ִ�� */
#define    E_ESL_TSF_XCMD_RET       19097   /* ����[%s]ִ������[%s:%s]���[%s]������Ϣ[%s] */
#define    E_ESL_TSF_RPAR           19098   /* ���������ڴ�ˢ�²���ת��������[%s]ִ�� */
#define    E_ESL_TSF_RPAR_RET       19099   /* ����[%s]ִ�в��������ڴ�ˢ�½��[%s]������Ϣ[%s] */
#define    E_ESL_TSF_RDEP           19100   /* �������ڴ�ˢ�²���ת��������[%s]ִ�� */
#define    E_ESL_TSF_RDEP_RET       19101   /* ����[%s]ִ�в������ڴ�ˢ�½��[%s]������Ϣ[%s] */
#define    E_ESL_PARSE_PKG          19102   /* ESLISTEN ��������[%s]�ֶ�[%s]�Ƿ�[%s] */
#define    E_ESL_STATUS             19103   /* ESLISTEN״̬����ȷ,����ִ��ָ������ */
#define    E_ESL_DTAM_BSVC_SUCC     19104   /* ����DTA[%s]����[%s]�ɹ� */
#define    E_ESL_DTAM_DSVC_SUCC     19105   /* ֹͣDTA[%s]����[%s]�ɹ� */
#define    E_ESL_DTAM_BSVC_FAIL     19106   /* ����DTA[%s]����[%s]ʧ�� */
#define    E_ESL_DTAM_DSVC_FAIL     19107   /* ֹͣDTA[%s]����[%s]ʧ�� */
#define    E_ESL_NODE_ALL           19108   /* ������ֻ�������нڵ㷢�� */
#define    E_ESL_PUSHAGT_EXIST      19109   /* ����PUSH����[%d]�Ѿ����� */
#define    E_ESL_BALCTRL_INIT       19110   /* ��ʼ�����ؾ��������ʧ�� */
#define    E_ESL_DTAM_SLEEP         19111   /* ESLDTAM���ݲ����ȴ�[%d]�� */
#define    E_ESL_LNSLOCK_INIT       19112   /* ��·�����źŵƳ�ʼ��ʧ�� */
#define    E_ESL_DTAM_BDTA_ATB      19113   /* DTA[%s]ʵ��[%d]�Զ�����[%d] */
#define    E_ESL_DTAM_DDTA_ATB      19114   /* DTA[%s]ʵ��[%d][%d]�����Զ���������,��Ҫ�˳� */
#define    E_ESL_DTAM_NORBT         19115   /* DTA[%s]���������δ����,��פʵ��[%d],��ǰʵ��[%d] */
#define    E_ESL_DTA_BUSY           19116   /* ϵͳ�з�æDTA[%s]���޷�ˢ�� */
#define    E_ESL_DENY_ADD           19117   /* ֻ���޸�����������[%s]���������� */
#define    E_ESL_ISSUE_RCYSHM       19118   /* ���չ����ڴ�汾����ʧ�� */
#define    E_ESL_REFRESH_CHK        19119   /* ˢ�¹����ڴ�[%d]�汾��鲿��汾[%s]�����汾[%s]ʧ�� */
#define    E_ESL_REF_PVER           19120   /* ����Ŀ�깲���ڴ�汾[%s]��XML�ļ��汾[%s]��һ�� */
#define    E_ESL_REF_DVER           19121   /* ����Ŀ�깲���ڴ�汾[%s]��XML�ļ��汾[%s]��һ�� */
#define    E_ESL_REF_POVER          19122   /* XML�ļ��汾[%s]�ȵ�ǰ����Ŀ�깲���ڴ�汾[%s]�� */
#define    E_ESL_REF_DOVER          19123   /* XML�ļ��汾[%s]�뵱ǰ����Ŀ�깲���ڴ�汾[%s]�� */
#define    E_ESL_REF_PCVER          19124   /* XML�ļ��汾[%s]�뵱ǰ����Ŀ�깲���ڴ�汾[%s]һ��,����Ҫˢ�� */
#define    E_ESL_REF_DCVER          19125   /* XML�ļ��汾[%s]�뵱ǰ����Ŀ�깲���ڴ�汾[%s]һ��,����Ҫˢ�� */
#define    E_ESL_RM_DCUR            19126   /* ͬ������Ҫ��ɾ����ǰ�������ڴ�[%s],�ܾ�ִ�� */
#define    E_ESL_RM_PCUR            19127   /* ͬ������Ҫ��ɾ����ǰ���������ڴ�[%s],�ܾ�ִ�� */
#define    E_ESL_REF_WORK           19128   /* ��Ϊˢ�»�ͬ��,����[%s]��ͣ��ز��� */
#define    E_ESL_SYNC_NEWMCH        19129   /* ͬ��ʱ�����»���[%s],����ƽ̨״̬,��������ƽ̨ */
#define    E_ESL_ATASK_MCHSTAT      19130   /* ��������[%c]�����߼�[%s]�Ҳ��������ڵ�[%s]��ִ��״̬ */
#define    E_ESL_DTAM_DDTA_ATBN     19131   /* DTA[%s]ʵ��[%d][%d]���㽻�����Զ���������,��Ҫ�˳� */
#define    E_ESL_DTAM_DDTA_ATBM     19132   /* DTA[%s]ʵ��[%d][%d]�����ڴ�ֵ�Զ���������,��Ҫ�˳� */
#define    E_ESL_DTAM_DDTA_ATBT     19133   /* DTA[%s]ʵ��[%d][%d]���㽻��ʱ���Զ���������,��Ҫ�˳� */
#define    E_ESL_DTAM_CLEAN_CC      19134   /* �����DTA[%s]�����ķ���ϵͳ[%s]�Ķ�̬��������Ϣʧ�� */
#define    E_ESL_MODEX_NEXT         19135   /* �ӵ�����[%s]�л����ؽڵ�����,���ؽڵ���[%s]�л�Ϊ[%s] */
#define    E_ESL_MASTER_DEL         19136   /* ԭ���ؽڵ�[%s]��ɾ��,���������ؽڵ�Ϊ[%s] */
#define    E_ESL_MASTER_NOID        19137   /* ԭ���ؽڵ�[%s]����Ԥ���Ŀ������ؽڵ�Ľڵ�,���������ؽڵ�Ϊ[%s] */
#define    E_ESL_SET_MASTER_OK      19138   /* �ڵ�[%s]���������ؽڵ�[%s]�ɹ� */
#define    E_ESL_SET_MASTER_FAIL    19139   /* �ڵ�[%s]���������ؽڵ�[%s]ʧ�� */
#define    E_ESL_SET_MASTER_CHG     19140   /* �յ��ڵ�[%s]������ؽڵ�����Ϊ[%s],ԭ���ؽڵ�Ϊ[%s] */

/*
 * DRQ ģ��         21000-21999
 */
#define E_DRQ_BUFINIT               21000  /* ��ʼ��DRQ������ʧ�� */
#define E_DRQ_GETPERM               21001  /* ��ȡDRQ��IPC-PERM��Ϣʧ�� */
#define E_DRQ_SEMCREATE             21002  /* �����ź�����ID[%d]ʧ�� */
#define E_DRQ_SEMREMOVE             21003  /* ͨ���ź�����ID[%d]ɾ���ź�����ʧ�� */
#define E_DRQ_SEMLOCK               21004  /* �����ź�����ID[%d]����[%s]�ź�ID[%d]ʧ�� */
#define E_DRQ_SEMLOCK_TMOUT         21005  /* �����ź�����ID[%d]����[%s]�ź�ID[%d]��ʱ */
#define E_DRQ_SEMUNLOCK             21006  /* �����ź�����ID[%d]����[%s]�ź�ID[%d]ʧ�� */
#define E_DRQ_SEMSETVAL             21007  /* �����ź�����ID[%d]����[%s]�ź�ID[%d]ֵ[%d]ʧ�� */
#define E_DRQ_OVER                  21008  /* DRQ����������������������[%d]���� */
#define E_DRQ_CLEANINST             21009  /* DRQ����DTA[%s]ʵ��[%d]״̬ʧ�� */
#define E_DRQ_NOQNAME               21010  /* DRQ��������[%s]������ */
#define E_DRQ_GETDTA                21011  /* ������[%s]ȡDTA���Ʋ����� */
#define E_DRQ_BUFATTACH             21012  /* ��DTA����[%s]����DRQ������[%d]ʧ�� */
#define E_DRQ_CREATQMSG             21013  /* ����DTA[%s]��DRQ����[%s]ʧ�� */
#define E_DRQ_DELQMSG               21014  /* ɾ��DTA[%s]��DRQ����[%s]ʧ�� */
#define E_DRQ_UNKNOWN_QTYPE         21015  /* DTA[%s]�Ķ�������[%d]�Ƿ� */
#define E_DRQ_GETQID                21020  /* ����������[%s]�����������ʧ�� */
#define E_DRQ_BUFALLOC              21030  /* �������[%s]��[%d]��С������ʧ�� */
#define E_DRQ_BIDERR                21031  /* ȡ���Ķ���[%s]�Ļ��������[%d]�Ƿ� */
#define E_DRQ_BUFREAD               21032  /* ȡ����[%s]�Ļ�����[%d]����[%d]ʧ�� */
#define E_DRQ_BUFWRITE              21033  /* �����[%s]�Ļ�����[%d]����[%d]ʧ�� */
#define E_DRQ_MSQRECV_TMOUT         21034  /* ȡ����[%s]����Ϣ[%ld]��ʱ */
#define E_DRQ_MSQRECV_ERR           21035  /* ȡ����[%s]����Ϣ[%ld]ʧ�� */
#define E_DRQ_MSQSEND_TMOUT         21036  /* ���Ͷ���[%s]����Ϣ[%ld]��ʱ */
#define E_DRQ_MSQSEND_ERR           21037  /* ���Ͷ���[%s]����Ϣ[%ld]ʧ�� */
#define E_DRQ_SUBCVALUE             21038  /* ����[%s]��Ϣ����һʧ�� */
#define E_DRQ_ADDCVALUE             21039  /* ����[%s]��Ϣ����һʧ�� */
#define E_DRQ_SNDFILE               21040  /* ��Ϣ[%s]ת���ļ�������[%s]ʧ�� */
#define E_DRQ_SNDDATA               21041  /* ��Ϣ[%s]ת������[%d]������[%s]ʧ�� */
#define E_DRQ_DRQGET                21042  /* DRQȡ��Ϣ[%s]ʧ�� */
#define E_DRQ_DRQPUT                21043  /* DRQ����Ϣ[%s]ʧ�� */
#define	E_DRQ_STACK_PUSH            21100  /* ������[%s]����Ϣ��ջʧ�� */
#define	E_DRQ_STACK_POP             21101  /* ������[%s]����Ϣ��ջʧ�� */
#define E_DRQ_TRAN_GETBASE          21200  /* ����[%s]�Ǽǽ�����Ϣʱ,ȡ�Ǽǹ����ڴ�ʧ�� */
#define E_DRQ_TRAN_LOCK             21201  /* ����[%s]�Ǽǽ�����Ϣʱ,����ʧ�� */
#define E_DRQ_TRAN_UNLOCK           21202  /* ����[%s]�Ǽǽ�����Ϣʱ,�������ײ���[%d]���� */
#define E_DRQ_TRANREG               21203  /* ����[%s]�ǼǷ���[%s]��Ϣʧ�� */
#define E_DRQ_DTARES_GET            21204  /* DRQ��[%s]DTA��Դ����ʧ�� */
#define E_DRQ_DTARES_FREE           21205  /* DRQ��[%s]DTA��Դ����ʧ�� */
#define E_DRQ_REG_MACHSVR           21210  /* ����[%s]DRQ����Ǽǽ���ʧ�� */
#define E_DRQ_MACHSVR_RUN           21211  /* ����[%s]DRQ�������� */
#define E_DRQ_MACHSVR_DOWN          21212  /* ����[%s]DRQ�����˳� */
#define E_DRQ_FAILCLEAN             21213  /* ������������ֳ�ʱ[%d]����Ϣ:-��������-->����[%s]Դ����[%s]ԴDTA[%s]Ŀ�Ļ���[%s]Ŀ��DTA[%s]��ˮ��[%ld]�Ự��[%s] */
#define E_DRQ_CLEANDTA_OK           21214  /* ����DTA[%s]�ɹ� */
#define E_DRQ_CLEANDTA_FAIL         21215  /* ����DTA[%s]ʧ�� */
#define E_DRQ_CLEANINST_OK          21216  /* DRQ����DTA[%s]ʵ��[%d]״̬�ɹ� */
#define E_DRQ_CLEANTRAN_OK          21217  /* DRQ����DTA[%s]����[%s]״̬�ɹ� */
#define E_DRQ_DRQ_LOCK              21218  /* DTA[%s]����DRQ��ʧ�� */
#define E_DRQ_CLEAN_LOCK            21219  /* �������������[%d]������������[%s]���[%d]������[%d] */
#define E_DRQ_CLEAN_BUFF            21220  /* �������������BUFFERʹ�ó�����ˮ��[%d%%]��ǿ���ͷ�[%d]������ */
#define E_DRQ_SVCPARL               21221  /* DTA[%s]�������ȼ���[%hd]�Ǽǲ�����ʧ�� */
#define E_DRQ_RCYCL_SVCID           21222  /* DTA[%s]�������ID[%s]������[%d]����[%d]��>0������ */
#define E_DRQ_RCYCL_PAL             21223  /* �ڵ�[%s]DTA[%s]������[%d]{%d %d %d %d %d}����[%d]��>0������ */
#define E_DRQ_RCYCL_CPAL            21224  /* DTA[%s]�ͻ�������[%s]������[%d]����[%d]��>0������ */
#define E_DRQ_SHM_LOCK              21225  /* ���빲���ڴ�״̬������ʧ�� */
#define E_DRQ_INST_LOCK             21226  /* ����ʵ��״̬������ʧ�� */
#define E_DRQ_SYNC_LOCK             21227  /* ����ͬ��״̬������ʧ�� */
#define E_LOCK_RELOCK               21228  /* ͬ����[%s]�ظ����� */
#define E_SHM_RCYCL_INST            21229  /* ǿ��DTA[%s]ʵ��[%d]���̺�[%d]�˳�,��Ϊ�����ӵĹ����ڴ�汾�Ѿ������� */
#define E_DRQ_FULL                  21230  /* ����DTA[%s]�Ķ���ʧ��,�޿��е�DRQTBL */

/*
 * DTA module       22000-22999
 */
#define  E_DTAAPI_SHM_ATTACH     22000  /* DTA[%s]���ӹ����ڴ�ʧ�� */
#define  E_DTAAPI_EXIT_SIGNAL    22001  /* DTA[%s]���յ��ź�[%d]�˳� */
#define  E_DTAAPI_DRQ_ATTACH     22002  /* DTA[%s]����DRQʧ�� */
#define  E_DTAAPI_EP_ALLOC       22003  /* DTA[%s]��������Ԫ�س� */
#define  E_DTAAPI_SLANG_INIT     22004  /* DTA[%s]��SLang��ʼ��ʧ�� */
#define  E_DTAAPI_DB_OPEN        22005  /* DTA[%s]ʹ������[%s]/���ݿ���[%s]/������[%s]�Ĳ��������ݿ�ʧ�� */
#define  E_DTAAPI_STDOUT_CONV    22006  /* �л���׼������ļ�[%s]ʧ�� */
#define  E_DTAAPI_STDERR_CONV    22007  /* �л�����������ļ�[%s]ʧ�� */
#define  E_DTAAPI_INST_INIT      22008  /* DTA[%s]��ʵ����ʼ��ʧ�� */
#define  E_DTAAPI_SEM_OPER       22009  /* ����DTA[%s]�ź����� */
#define  E_DTAAPI_INST_FULL      22010  /* DTA[%s]����ʵ������������ֵ[%d] */
#define  E_DTAAPI_OUT_DIRECT     22011  /* DTA[%s]��׼/��������ض���ʧ�� */
#define  E_DTAAPI_ROUTE_CALC     22012  /* ����DTA[%s]�ķ���[%s]·��ʧ�� */
#define  E_DTAAPI_EPTOBUF        22013  /* DTA[%s]�Ϸ���[%s]������ת��ʧ�� */
#define  E_DTAAPI_DRQPUT         22014  /* DTA[%s]�Ϸ���[%s]��DRQʧ�� */
#define  E_DTAAPI_DRQGET         22015  /* DTA[%s]�Ϸ���[%s]ȡDRQʧ�� */
#define  E_DTAAPI_MCHSESSION     22016  /* DTA[%s]�Ϸ���[%s]�����ʽ[%s]ȡ�Ự�Ŵ� */
#define  E_DTAAPI_QSESSION       22017  /* DTA[%s]�Ϸ���[%s]�����ʽ[%s]ȡ������Ϣ�� */
#define  E_DTAAPI_STACKPOP       22018  /* DTA[%s]�Ϸ���[%s]ȡ����ջ�� */
#define  E_DTAAPI_ERRCONF        22019  /* DTA[%s]��ʼ����־����ʧ�� */
#define  E_DTAAPI_MAP            22020  /* ����ӳ��ת��[%s]��[%s]ʧ�� */
#define  E_DTAAPI_TIMEOUT        22021  /* DTA[%s]�Ϸ���[%s]����ʱ */
#define  E_DTAAPI_ACK_SESS       22022  /* Ŀ����ӦDTA[%s]���ݱ��ʽ[%s]ȡ�Ự��ʧ�� */
#define  E_DTAAPI_AREV_INS       22030  /* �Ǽ�Ŀ��DTA[%s]����[%s]��ˮ��[%s]�ĳ�����ˮʧ�� */
#define  E_DTAAPI_AREV_UPT       22031  /* ����Ŀ��DTA[%s]����[%s]�ĳ�����ˮʧ�� */
#define  E_DTAAPI_AREV_DEL       22032  /* ɾ��Ŀ��DTA[%s]����[%s]�ĳ�����ˮʧ�� */
#define  E_DTAAPI_AREV_ADDBACK   22033  /* ׷��Ŀ��DTA[%s]����[%s]�ĳ�����ˮʧ�� */
#define  E_DTAAPI_AREV_NOREC     22034  /* Ŀ��DTA[%s]����[%s]�ĳ�������[%s]û�ж�Ӧ�ķ����� */
#define  E_DTAAPI_AREV_CTRL      22035  /* �л��Զ�����״̬Ϊ[%s] */
#define  E_DTAAPI_AREV_PRO       22036  /* DTA[%s]�ĳ�������[%s]����ǰ����ʧ�� */
#define  E_DTAAPI_AREV_END       22037  /* DTA[%s]�ĳ�������[%s]���������ж�ʧ�� */
#define  E_DTAAPI_AREV_SETBACK   22038  /* DTA[%s]�ķ���[%s]��¼������������Ԫʧ�� */
#define  E_DTAAPI_AREV_DBSERV    22039  /* DTA[%s]�����첽���ݿ����DBSERVʧ�� */
#define  E_DTAAPI_DB_NAME        22040  /* DTA[%s]�����ݿ�Ĳ������ò����� */
#define  E_DTAAPI_AREV_RECNUM    22041  /* DTA[%s]��ˮ��[%s]�Ǽǳ�����¼[%d]���� */
#define  E_DTAAPI_SYS_ID         22042   /* Ŀ��DTA[%s]�ڵ�[%s]��ϵͳ��[%c]��ԴDTA[%s]��ϵͳ��[%c]���� */
#define  E_DTAAPI_NO_SYS         22043   /* Ŀ��DTA[%s]û����ԴDTA[%s]��ϵͳ��[%c]����Ĳ��𷽰� */
#define  E_DTASVC_TIMEOUT        22044   /* DTA[%s]����[%s]ʣ�೬ʱ[%d]С����ʷ��Ȩ��С����ʱ��[%.3lf] */
#define  E_DTASVC_TMREQ          22045   /* DTA[%s]����[%s]��ʱ�䲻��,�������� */
#define  E_DTASVC_STOP           22046   /* ����[%s]DTA[%s]����[%s]��ֹͣ���� */
#define  E_DTADEPSHM_REATTACH    22047   /* DTA[%s] ʵ��[%d] ��ǰ�����ڴ�汾[%s] ���ײ������ڴ�汾[%s] ������ʧ�� */
#define  E_DTAPARSHM_REATTACH    22048   /* DTA[%s] ʵ��[%d] ��ǰ�����ڴ�汾[%s] ���ײ��������ڴ�汾[%s] ������ʧ�� */
#define  E_DTAINST_STARTED       22049   /* DTA[%s] ʵ��[%d] �Ѿ����������̺�Ϊ[%d] */
#define  E_DTAAPI_MCH_DB         22050   /* �򿪻���[%s] DTA[%s]�����ݿ�����ʧ��*/
#define  E_DTAAPI_MCH_SVCST      22051   /* ȡ����[%s] DTA[%s]�ķ���[%s]״̬ʧ��*/
#define  E_DTAAPI_SHMREATACH    22052    /* DTA[%s] ʵ��[%d] ǿ���������¹����ڴ�*/
#define  E_DTAAPI_AREV_SHM      22053  /* �Զ�������Ӧ�����ڴ�汾[DEP:%s PAR:%s]������,�л��Զ�����[%s]״̬Ϊ[%c] */

/*
 * ͨ��DTA                   23000-23999
 */
#define E_GDTA_EXPR        23000  /* DTA[%s]�¼�[%s]���ʽ������� */
#define E_GDTA_SVC         23001  /* DTA[%s]����[%s]����ʧ�� */
#define E_GDTA_EXPR_CALC   23002  /* ���ʽ���㷵��FALSE,ep_id[%d],idx[%d],expr[%s] */
#define E_GDTA_EXPR_VTYPE  23003  /* ���ʽ[%s]�ķ���ֵ����[%d]���� */

#define E_GDTA_FMTPAR      23004  /* �������ĸ�ʽ[%s]ʧ�� */
#define E_GDTA_RECV        23005  /* DTA[%s]����[%s]���Ľ���ʧ�� */
#define E_GDTA_RECVPFMT    23006  /* DTA[%s]����Э�鱨�ĸ�ʽ[%s]ʧ�� */
#define E_GDTA_RECVFMT     23007  /* DTA[%s]����[%s]���ձ��ĸ�ʽ[%s]ʧ�� */
#define E_GDTA_RECVFMTG    23008  /* DTA[%s]����[%s]���ձ������[%d]��ʧ�� */
#define E_GDTA_RECVFILE    23009  /* DTA[%s]����[%s]�����ļ�ʧ�� */
#define E_GDTA_PARSEFILE   23010  /* DTA[%s]����[%s]�����ļ�ʧ�� */

#define E_GDTA_FMTBLD      23011  /* ��֯���ĸ�ʽ[%s]ʧ�� */
#define E_GDTA_SEND        23012  /* DTA[%s]����[%s]���ķ���ʧ�� */
#define E_GDTA_SENDPFMT    23013  /* DTA[%s]����Э�鱨�ĸ�ʽ[%s]ʧ�� */
#define E_GDTA_SENDFMT     23014  /* DTA[%s]����[%s]���ͱ��ĸ�ʽ[%s]ʧ�� */
#define E_GDTA_SENDFMTG    23015  /* DTA[%s]����[%s]���ͱ������[%d]��ʧ�� */
#define E_GDTA_SENDFILE    23016  /* DTA[%s]����[%s]�����ļ�ʧ�� */
#define E_GDTA_BUILDFILE   23017  /* DTA[%s]����[%s]��֯�ļ�ʧ�� */

#define E_GDTA_SENDFG      23018  /* DTA[%s]����[%s]���ͱ�����������[%s]ʧ�� */
#define E_GDTA_SENDFGE     23019  /* DTA[%s]����[%s]���ͱ����鸨����[%s]ʧ�� */
#define E_GDTA_CONV_SVC    23020  /* DTA[%s]����[%s]���з���ת��ʧ�� */
#define E_GDTA_CONV_ERR    23021  /* DTA[%s]����[%s]���д�����[%d]ת��ʧ�� */
#define E_GDTA_NODE_ADDR   23022  /* ȡDTA[%s]����[%s]�ڵ�[%s]�ĵ�ַʧ�� */
#define E_GDTA_NODECONN    23023  /* DTA[%s]���ӽڵ�[%s]ʧ�� */
#define E_GDTA_NOCONN      23024  /* û�����ӷ��������������ͼ���б��ķ��ͻ���� */
#define E_GDTA_ADDR        23025  /* �Ƿ���ַ[%s] */
#define E_GDTA_MCHQSVC     23026  /* ��DTA[%s]��ƥ�����ȡ������ʧ�� */
#define E_GDTA_GETSVC      23027  /* ���ݽ��ձ���ȡDTA[%s]�ķ�����ʧ�� */

#define E_GDTA_DTAINIT     23028  /* ��������[%s]ִ��DTA��ʼ��ʧ�� */
#define E_GDTA_DTAACALL    23029  /* DTA[%s]����[%s]�����ڲ�����ʧ�� */
#define E_GDTA_DTAGETR     23030  /* DTA[%s]����[%s]�����ڲ���Ӧʧ�� */
#define E_GDTA_DTASERV     23031  /* DTA[%s]�����ڲ�����ʧ�� */
#define E_GDTA_DTARETURN   23032  /* DTA[%s]����[%s]�����ڲ���Ӧʧ�� */

#define E_GDTA_COMMINIT    23033  /* DTA[%s]ͨѶ��ʼ��ʧ��[DTACommInit] */
#define E_GDTA_COMMTERM    23034  /* DTA[%s]ͨѶ��ֹʧ��[DTACommTerm] */
#define E_GDTA_COMMRECV    23035  /* DTA[%s]��������ʧ��[DTACommRecv] */
#define E_GDTA_COMMSEND    23036  /* DTA[%s]��������ʧ��[DTACommSend] */
#define E_GDTA_COMMRFILE   23037  /* DTA[%s]�����ļ�ʧ��[DTACommRecvFile] */
#define E_GDTA_COMMSFILE   23038  /* DTA[%s]�����ļ�ʧ��[DTACommSendFile] */
#define E_GDTA_COMMCONNECT 23039  /* DTA[%s]���ӵ�ַ[%s:%s]ʧ��[DTACommConnect] */

#define E_GDTA_FGRECGET    23040  /* ���ݱ��ʽ[%s]ȡ�������¼��ʧ�� */
#define E_GDTA_FGRECPUT    23041  /* ��������Ԫ��[%s]�汨�����¼��ʧ�� */
#define E_GDTA_FGEFCOND    23042  /* ���ݸ����ı��ʽ[%s]��������ʧ�� */
#define E_GDTA_FMTXFML     23043  /* ���ױ���[%s]����Ϊ��FML���� */

#define E_GDTA_FILEPAR     23044  /* �����ļ���ʽ[%s]ʧ�� */
#define E_GDTA_FILEBLD     23045  /* ��֯�ļ���ʽ[%s]ʧ�� */
#define E_GDTA_FILESEND    23046  /* �����ļ����ʽ[%s]����ʧ�� */
#define E_GDTA_FILERECV    23047  /* �����ļ����ʽ[%s]����ʧ�� */

#define E_GDTA_TUX_ARG     23048  /* δָ��TUXEDO����[%s]�������в��� */
#define E_GDTA_TUX_CALL    23049  /* ͬ������TUXEDO����[%s]ʧ��[%s] */
#define E_GDTA_TUX_ACALL   23050  /* ����TUXEDO����[%s]ʧ��[%s] */
#define E_GDTA_TUX_GETRPL  23051  /* ����TUXEDO��Ӧ[%s]ʧ��[%s] */
#define E_GDTA_TUX_ALLOC   23052  /* ���䳤��Ϊ[%d]��TUXEDO����ʧ�� */
#define E_GDTA_TUX_BUFTYP  23053  /* TUXEDO����[%s]���յ��ı������Ͳ�Ϊ[%s] */
#define E_GDTA_SVR_INIT    23054  /* DTA[%s]�����ʼ��[DTASvrInit]ʧ�� */

#define E_GDTA_TCP_ARG     23055  /* δָ��TCP����[%s]�Ĳ��� */
#define E_GDTA_TCP_SVR     23056  /* ������TCP-DTA[%s]����ʧ�� */
#define E_GDTA_TCP_INSTNUM 23057  /* ������TCP-DTA[%s]��ʵ����С��1 */
#define E_GDTA_TCP_SOCKET  23058  /* SOCKET�������Ƿ�(С��0) */

#define E_GDTA_SERVICE     23059  /* DTA[%s]����[DTAService]ʧ�� */
#define E_GDTA_FUNNULL     23060  /* ͨ�Žӿ�[%s]δ���пͻ��� */

#define E_GDTA_APPERRNO    23061  /* Ŀ��DTA����ԴDTA[%s]�Ϸ���[%s]��Ӧ�÷�����[%d]�Ƿ� */
#define E_GDTA_APP         23062  /* DTA[%s]����[%s]Ŀ�ķ�Ӧ�ô���ʧ�� */
#define E_GDTA_RECVSHORT   23063  /* ����__GDTA_SHORT���'0'��·������[%s]��ͨ�Ž���ֱ�ӷ��� */
#define E_GDTA_SENDSHORT   23064  /* ����__GDTA_SHORT���'0'��·������[%s]��ͨ�ŷ���ֱ�ӷ��� */
#define E_GDTA_RFILESHORT  23065  /* ����__GDTA_SHORT���'0'��·������[%s]���ļ�����ֱ�ӷ��� */
#define E_GDTA_SFILESHORT  23066  /* ����__GDTA_SHORT���'0'��·������[%s]���ļ�����ֱ�ӷ��� */
#define E_GDTA_ACALLSHORT  23067  /* ����__GDTA_SHORT���'0'��·������[%s]���ڲ�����ֱ�ӷ��� */
#define E_GDTA_GREPLSHORT  23068  /* ����__GDTA_SHORT���'0'��·������[%s]���ڲ���Ӧֱ�ӷ��� */
#define E_GDTA_INITSHORT   23069  /* ����__GDTA_SHORT���'0'��·��DTA[%s]��ͨ�ų�ʼ��ֱ�ӷ��� */
#define E_GDTA_TERMSHORT   23070  /* ����__GDTA_SHORT���'0'��·��DTA[%s]��ͨ�Ž���ֱ�ӷ��� */
#define E_GDTA_CONNECTSHORT 23071  /* ����__GDTA_SHORT���'0'��·�����ӽڵ�[%s:%s]ֱ�ӷ��� */
#define E_GDTA_CLOSESHORT  23072  /* ����__GDTA_SHORT���'0'��·���رսڵ�[%s]����ֱ�ӷ��� */
#define E_GDTA_PARSESHORT  23073  /* ����__GDTA_SHORT���'0'��·������[%s]�ı���[%s]����ֱ�ӷ��� */

#define  E_GDTA_TYPE       23074  /* error DTA type [%s] */   
#define  E_GDTA_SVCNAME    23075  /* the service name of DTA[%s]error [%s] */
#define  E_GDTA_TUXAGTNAME 23076  /* the svcname of TUXEDO dispatch pattern must include  'AGT_',[%s]*/
#define  E_GDTA_NAME       23077  /* error DTA name [%s] */
#define  E_GDTA_DISPSVCLEN 23078  /* the svcname of TUXEDO dispatch pattern can't exceed 45 characters  */
#define  E_TUXDTA_PARRELL  23079  /* TUXEDO �ַ�ģʽ��DTA[%s] ����ǰ������[%d] ������󲢷���[%d],DTA���������� */
#define  E_TUXCALL_ERROR   23080  /* TUXEDO��������[%s]ʧ��:[%d][%s] */
#define  E_GDTA_CLRTIMEOUT 23081  /* DTA[%s]����ʱ����[%s],��ˮ:[%s] [%s] */
#define  E_GDTA_TRANCLR    23082  /* DTA[%s]����ʱ����[%s],��ˮ:%s,����ʱ��[%d],����ʱ���[%d],��ʱʱ��[%d] */
#define  E_TUXDTA_SVCERR   23083  /* DTA[%s]�ϵ�[%s]����,Դ��ˮΪ[%s]����ʧ��,������������Ϣ������� */
#define E_TUXDTA_RESMATCH  23084  /* DTA[%s]�Ͻ������ݳ���[%d],TUXEDOӦ���ʾ[%d],�Ҳ���ƥ��ļ�¼,���ܽ����ѳ�ʱ */
#define E_TUXDTA_CONNERR   23085  /* TUXEDO�첽DTA���ӷ���ip[%s]�˿�[%s]ʧ��[%s] */
#define E_GDTA_CALLFAIL    23086  /* TCPԴDTA[%s]�ӽ��̴������[%s],��ˮ��[%s]ʧ�� */
#define E_GDTA_ERRINST     23087  /* �����ҵ�TCP�ӽ���[%d]��Ӧ��ʵ���� */
#define E_GDTA_REQ         23088  /* DTA[%s]����[%s]Ŀ�ķ�Ӧ������ʧ�� */
#define E_GDTA_ACK         23089  /* DTA[%s]����[%s]Ŀ�ķ�Ӧ�ý���ʧ�� */
#define E_GDTA_BUS         23090  /* DTA[%s]����[%s]Ŀ�ķ����������� */

#define E_GDTA_DBBEGIN     23091  /* DTA[%s]����[%s]��ʼ���ݿ����� */
#define E_GDTA_DBCOMMIT    23092  /* DTA[%s]����[%s]�ύ���ݿ����� */
#define E_GDTA_ASYNCLT     23093  /* �첽ԴDTA����Դ��ˮΪ[%s]�Ľ���ʧ�� */
#define E_GDTA_MCHQSSID    23094  /* ����MCHQ[%s]�ỰID[%d] */
#define E_GDTA_HOSTCONN    23095  /* DTA[%s]���ӽڵ�[%s]ʧ��  */
#define E_GDTA_SVCERR      23096  /* DTA[%s]����[%s]Դ��ˮ[%s]����ʧ�� */
#define E_GDTA_EXCEDCONN   23097  /* DTA[%s]���ӽڵ�[%s]����[%d] */
#define E_GDTA_NODEIPCONN  23098  /* DTA[%s]���ӽڵ�[%s][%s:d] ʧ�� */
#define E_GDTA_CONNIDX     23099  /* DTA[%s]���ӽڵ�[%s]�±�[%d]���� */
#define E_GDTA_DELETRANREG 23100  /* ɾ��������Ϣ�����,��Ϣ���־[%d]���׻Ự[%d]������Ự�Ų���[%d] */
#define E_GDTA_BALATTACH   23101  /* ȡDTA[%s]�������Ʋ������� */
#define E_GDTA_PADDR       23102  /* �ͻ��˵�ַ[%s]���Ϸ�,�ܾ����� */
#define E_GDTA_PADDR_NUM   23103  /* �ͻ��˵�ַ[%s]����������,�ܾ����� */
#define E_GDTA_PADDR_ONUM  23104  /* �ͻ��˵�ַ[%s]�������ﵽ������[%d],�ܾ����� */
#define E_GDTA_CONN_TMOUT  23105  /* ������[%d]��[%d]���뵽[%d]δ��������,���ӱ��ر� */
#define E_GDTA_CLTLINK_CHK 23106  /* ��·[%s:%hu]��ǿ������ */
#define E_GDTA_DYNSVRCON_CURR  23107  /* ����ϵͳ[%s],��ǰ������Ϊ[%d],��������[%d] ����Ĳ�����[%d],��̬������������Ч */
#define E_GDTA_DYNSVCCON_CURR  23108  /* ����ϵͳ[%s]����[%s],��ǰ������Ϊ[%d],��������[%d] ����Ĳ���[%d]��,��̬��������������Ч */


/* Max = 23104 */

/*
 * FLOW module               26000-26999
 */
#define	E_FLW_CTS_INIT     26000  /* ���̲��Խ���[%s]��ʼ��ʧ�� */
#define	E_FLW_CTS_LOG      26001  /* ���̲��Խ�������ALA[%s]����־��־ʧ�� */
#define	E_FLW_CTS_CALL     26002  /* ���̲��Խ�������ALA[%s]�Ľ���[%s]ʧ�� */
#define	E_FLW_CTS_SENDFILE 26003  /* ���̲��Խ��̷����ļ�[%s]ʧ�� */
#define E_FLW_TRAN_DEF     26100  /* ����[%s]������Ϣ������ */
#define E_FLW_TRANDATA     26101  /* ����[%s]�����޲��趨����Ϣ */
#define E_FLW_EXEC_FLO     26109  /* ����[%d]ִ��������[%s]ʧ�� */
#define E_FLW_EXEC_EXP     26110  /* ����[%d]ִ�м��㴦��[%s]ʧ�� */
#define E_FLW_EXEC_SQL     26111  /* ����[%d]ִ�����ݿ����[%s]ʧ�� */
#define E_FLW_EXEC_FIL     26112  /* ����[%d]ִ���ļ�����[%s]ʧ�� */
#define E_FLW_EXEC_RPT     26113  /* ����[%d]ִ�б������[%s]ʧ�� */
#define E_FLW_EXEC_DTA     26114  /* ����[%d]ִ���ⲿͨ��[%s]ʧ��[%d] */
#define E_FLW_EXEC_CMD     26115  /* ����[%d]ִ��������[%s]ʧ�� */
#define E_FLW_EXEC_SWI     26116  /* ����[%d]ִ�п��ش���[%s]ʧ�� */
#define E_FLW_EXEC_PRE     26117  /* ����[%d]ִ��ǰ����ʧ�� */
#define E_FLW_EXEC_SUF     26118  /* ����[%d]ִ�к���ʧ�� */
#define E_FLW_EXEC_ERR     26119  /* ����[%d]ִ�д�����ʧ�� */
#define E_FLW_DB_ELBEG     26120  /* ȡ���ݿⶨ�Ƶ�����Ԫ�±���ʼλ�ô� */
#define E_FLW_DB_DBBEG     26121  /* ȡ���ݿⶨ�Ƶ����ݿ��¼��ʼλ�ô� */
#define E_FLW_DB_DBREC     26122  /* ȡ���ݿⶨ�Ƶ���󷵻ؼ�¼�� */
#define E_FLW_DB_DBCODE    26123  /* �����ݿⶨ�Ƶķ������ݿ������� */
#define E_FLW_DB_DBRETURN  26124  /* �����ݿⶨ�Ƶķ��ؼ�¼���� */
#define E_FLW_DB_INDTRAN   26125  /* �������ݿ�����ĵ�[%d]��SQL���[%s]ƴװʧ�� */
#define E_FLW_DB_NOSELECT  26126  /* �������ݿ����������SELECT���� */
#define E_FLW_DB_TRAN      26127  /* ���ݿ�����ĵ�[%d]��SQL���[%s]ƴװʧ�� */
#define E_FLW_DB_EXEC      26128  /* ���ݿ�����ĵ�[%d]��SQL���[%s]ִ��ʧ�� */
#define E_FLW_DB_2EP       26129  /* ���ݿ�����ĵ�[%d]��SQL���[%s]���������ʧ�� */
#define E_FLW_DB_SELECT    26130  /* ���ݿ�����ĵ�[%d]��SQL���[%s]��ѯʧ�� */
#define E_FLW_FILE_TYPE    26140  /* �ļ���������[%c]���� */
#define E_FLW_DTA_TYPE     26150  /* �ⲿͨ�Ų�������[%c]���� */
#define E_FLW_DTA_ASYN     26151  /* �ⲿͨ���첽ƥ��ģʽ����[%c]���� */
#define E_FLW_CMD_DE       26160  /* �����в�����������Ԫ����[%s]����[%d] */
#define E_FLW_CMD_DEIDX    26161  /* �����в�����������Ԫ�±�ĸ�ʽ[]��ƥ�� */
#define E_FLW_CMD_STRLEN   26162  /* ������������ַ�������,�ռ�[%d]���� */
#define E_FLW_SWI_TYPE     26170  /* ���ز���[%d]�Ĵ�����ʽ[%s]�������Ͳ������� */
#define E_FLW_APP_ERROR    26200  /* ��������Ӧ����Ϣ[%s][%s] */
#define E_FLW_DTA_APP      26201  /* DTA[%s]����ʧ��-Ӧ�ô��� */
#define E_FLW_DTA_REQ      26202  /* DTA[%s]����ʧ��-��������ʧ�� */
#define E_FLW_DTA_ACK      26203  /* DTA[%s]������-������Ӧʧ�� */
#define E_FLW_DTA_BUS      26204  /* DTA[%s]����ʧ��-���������� */

#define E_ALA_EXPR         26800  /* �߼�����[%s]ִ�б��ʽ[%s]ʧ�� */
#define E_ALA_INIT         26801  /* �߼�����[%s]��ʼ��ʧ�� */
#define E_ALA_SERV         26802  /* �߼�����[%s]���ܷ�������ʧ�� */
#define E_ALA_RETURN       26803  /* �߼�����[%s]���ط���[%s]��Ӧʧ�� */
#define E_ALA_TIMEOUT      26804  /* �߼�����[%s]����[%s]����ʱ */
#define E_ALA_DBBEGIN      26805  /* �߼�����[%s]����[%s]��ʼ���ݿ����� */
#define E_ALA_DBCOMMIT     26806  /* �߼�����[%s]����[%s]�ύ���ݿ����� */
#define E_ALA_CF           26807  /* �߼�����[%s]����[%s]���̵���ʧ�� */
#define E_ALA_INITEVENT    26810  /* �߼�����[%s]��ʼ���¼�ִ��ʧ�� */
#define E_ALA_TERMEVENT    26811  /* �߼�����[%s]�����¼�ִ��ʧ�� */
#define E_ALA_SERVBEGIN    26812  /* �߼�����[%s]����[%s]��ʼ�¼�ִ��ʧ�� */
#define E_ALA_SERVSUCC     26813  /* �߼�����[%s]����[%s]�ɹ������¼�ִ��ʧ�� */
#define E_ALA_SERVFAIL     26814  /* �߼�����[%s]����[%s]ʧ�ܽ����¼�ִ��ʧ�� */
#define E_ALA_AREV_UPT     26820  /* �߼�����[%s]����[%s]�ɹ��޸ĳ���״̬ʧ�� */
#define E_ALA_AREV_DEL     26821  /* �߼�����[%s]����[%s]ʧ�ܳ�����¼ɾ��ʧ�� */

#define E_TASK_NOINST      26900  /* ����[%s]û�п���[%s]ʵ�������ȴ���һ�ε��� */
#define E_TASK_EXEC        26901  /* ����[%s]����MCH[%s]ALA[%s]����ʧ�� */
#define E_TASK_NOSVC       26902  /* ����[%s]��Ӧ�����߼�û�ж��壬��ֹͣ */
#define E_TASK_MNOINST     26903  /* ����[%s]û�п��û���[%s]ALA[%s]ʵ�������ȴ���һ�ε��� */
/*
 * ���ݿ��װģ��         27000-27999
 */
#define E_DB_ERR	27000	/* ���ݿ������SQLCODE[%ld]ISAM[%ld]USE[%ld]DBMSG[%s:%s]USEMSG[%s] */
#define E_PREP_ID	27001	/* û��PREPARE����SQL��� */
#define E_CURS_ID	27002	/* ���α�ǰû�ж����α� */
#define E_DESC_ID       27003	/* �α����������� */
#define E_GETNULL	27004	/* ȡSQL���������ؿ� */
#define E_PUTNULL	27005	/* ��SQL���������ؿ� */
#define E_PUTERROR	27006	/* �����SQL��������Ŵ��ڲ�ѯ�����ĸ��� */
#define E_CUR_NOOPEN	27007	/* �α�û�д� */
#define E_CUR_NOALLOC	27008	/* û�з����α������SQL������ */
#define E_PUT_NOALLOC	27009	/* û�з������������SQL������ */
#define E_EXE_NOALLOC	27010	/* ִ��SQL���ǰû�з���SQL������ */
#define E_PUT_NOMATCH	27011	/* �ڴ��α��EXECUTE֮ǰ������������ȫ */
#define E_CUR_UNCLOSED	27012	/* ��δ���α�֮ǰ���ܹر��α� */
#define E_CUR_NOCLOSE	27013	/* ��δ�ر��α�֮ǰ�����ͷ��α� */
#define E_DESC_UNALLOCATED	27014	/* ��δ���������ռ�֮ǰ�����ͷ������ռ� */
#define E_GETSERIAL_ERR	27015	/* ��Ҫ�õ�����ֵ������SQL�����ѡ���ķ�Χ */
#define E_CUR_NOFETCH	27016	/* ��ȡ����֮ǰû��FETCH�α� */
#define E_MULTI_PREP	27017	/* �ظ�PREPARE����SQL��� */
#define E_ILLLEGAL_TYPE	27018	/* �ڵ��ó����������˷Ƿ���SQL�������� */
#define E_NODEF_TYPE	27019	/* �ڳ�����δ�ԺϷ���SQL�������ͽ���˵�� */
#define E_DESC_NOOPEN	27020	/* �ڳ�����ִ��SqlDescribe֮ǰû�д��α� */
#define E_EXE_INTERIOR	27021	/* ��ִ��SqlExecute�����г����˵��ڲ����� */
#define E_NOTFOUND	27022	/* ��ִ��SQL��������δ���ּ�¼ */
#define E_NOAREA	27023	/* ִ��sel,upd,del,insSQL���ǰδ���乤���� */
#define E_DB_NOOPEN     27100   /* û�д����ݿ� */
#define E_MAXAREA       27101   /* ����Ĺ������ﵽ������[%d] */

/* 
 * ������־       29000-29999
 */
#define E_RPT_EFMT      29000   /* δֵ֪��ʽ[%s] */
#define E_RPT_FMT       29001   /* ֵ[%s]��ʽ�����ϸ�ʽ[%s]Ҫ�� */
#define E_RPT_FILES     29002   /* ����[%s]��[%s]�ڵ��ļ�ȡԴ������� */
#define E_RPT_DBS       29003   /* ����[%s]��[%s]�ڵ����ݿ�ȡԴ������� */
#define E_RPT_TYPE      29004   /* δ֪�ı�������[%d]  */
#define E_RPT_BOXTYPE   29005   /* δ֪�ı߿�����[%s] */
#define E_RPT_CNTTYPE   29006   /* ��Ԫ�������[%d]��XML�ļ��ӽڵ㲻ƥ�� */
#define E_RPT_CURCODE   29007   /* δ֪�ı�������[%s] */
#define E_RPT_MONEY     29008   /* �������[%s]��ʽ�� */
#define E_RPT_NUMBER    29009   /* �����ַ���[%s]�зǷ��ַ� */
#define E_RPT_CALCMETA  29010   /* ͳ������Ԫ����[%d]��ϼ�����Ԫ����[%d]��ƥ�� */
#define E_RPT_PRTTYPE   29011   /* ��ӡ������Ϊ[%d],��ӡ�����Ʋ�������Ϊ�� */
#define E_RPT_PAGEAREA  29012   /* δ֪��ҳ�뷶Χ[%s] */
#define E_RPT_DETYPE    29013   /* δ֪������Ԫ����������[%d] */
#define E_RPT_D_QUOTE   29014   /* ˫���Ų�ƥ��[%s] */
#define E_RPT_S_QUOTE   29015   /* �����Ų�ƥ��[%s] */
#define E_RPT_BRAKETS   29016   /* ���Ų�ƥ��[%s] */
#define E_RPT_EXPR_VTYPE    29017   /* δ֪�ı��ʽ����ֵ����[%d] */
#define E_RPT_HIGH      29018   /* ����߶ȴ���,����߶�[%d]С�ڹ̶��и�[%d] */
#define E_RPT_GRP       29019   /* ������[%d]Ϊ��,���ܴ�ӡ */
#define E_RPT_PAGENUM   29020   /* ����ҳ��[%d]Ϊ��,û�����ݴ�ӡ */
#define E_RPT_PAGEREC   29021   /* �������,ÿҳ��ӡ��¼��[%d]̫С */
#define E_RPT_CUSTFUNC  29022   /* ���ÿͻ�����������[%s]���� */
#define E_RPT_NOCONV    29023   /* ����[%s]����ת����[%s]��Χ�� */

/*
 * �ļ��������ģ��       30000-30999
 */
#define E_FLS_RUN       30001   /* �ļ�����[%s]���� */
#define E_FLS_DOWN      30002   /* �ļ�����[%s]�˳� */
#define E_FLS_FAIL      30003   /* �ļ�����[%s]��IP[%s]�˿�[%s]�ϼ���ʧ�� */

/*
 * ������־       31000-31999
 */
#define E_TRC_PRSALA_DUMP    31001   /* ��Ŀ¼[%s]�¼�¼ALA[%s]��ѹ��������־ʧ�� */
#define E_TRC_PRSALA_IDX     31002   /* ALA[%s]ѹ��������־�ռ�������� */
#define E_TRC_GET_OPERREC    31003   /* ȡ��[%d]������Ԫ�ز�����Ϣʧ�� */
#define E_TRC_GET_ELEMID     31004   /* ͨ������Ԫ�ر��[%ld]ȡ����Ԫ����Ϣʧ�� */

/*
 * �����̵�����־ 31100-31199
 */
#define E_FLW_APPSERV        31100 /* [%d] DTAAppServ( %s, %s )���� */
#define E_FLW_DTASVC         31101 /* [%d] ȡDTA[%s]����[%s]�Ĳ�����Ϣ */
#define E_FLW_ALAPRE         31102 /* [%d] ִ��ALAǰ���� */
#define E_FLW_ALAFAIL        31103 /* [%d] ִ��ALAʧ�ܴ��� */
#define E_FLW_ALASUCC        31104 /* [%d] ִ��ALA�ɹ����� */
#define E_FLW_ALADONE        31105 /* [%d] ִ��ALA�������� */
#define E_FLW_BWORK          31106 /* [%d] ��ʼ���� */
#define E_FLW_CFBEGIN        31107 /* [%d] ����[%s] CoreFlow��ʼ */
#define E_FLW_CFEND          31108 /* [%d] ����[%s] CoreFlow���� */
#define E_FLW_RWORK          31109 /* [%d] �ع����� */
#define E_FLW_CWORK          31110 /* [%d] �ύ���� */
#define E_FLW_REVUA          31111 /* [%d] �ύȫ���Զ�������Ϣ */
#define E_FLW_REVDA          31112 /* [%d] ɾ��ȫ���Զ�������Ϣ */
#define E_FLW_RETURN         31113 /* [%d] DTAAppReturn( %s, %s, %d ) */
#define E_FLW_ALLEND         31114 /* [%d] ��������� */
#define E_FLW_CF_CFBEGIN     31115 /* [%d] ����[%s]���̴���ʼ */
#define E_FLW_CF_CFEND       31116 /* [%d] ����[%s]���̴������ */
#define E_FLW_CF_CFPRE       31117 /* [%d] ����[%d]ִ��ǰ������� */
#define E_FLW_CF_CFDONE      31118 /* [%d] ����[%d]ִ�к������ */
#define E_FLW_CF_CFFAIL      31119 /* [%d] ����[%d]ִ��ʧ�ܴ������ */
#define E_FLW_CFSUB_BEG      31120 /* [%d] ����[%d]������[%s]����ʼ */
#define E_FLW_CFSUB_END      31121 /* [%d] ����[%d]������[%s]������� */
#define E_FLW_CFEXP_BEG      31122 /* [%d] ����[%d]���㲿������ʼ */
#define E_FLW_CFEXP_END      31123 /* [%d] ����[%d]���㲿��������� */
#define E_FLW_CFSQL_BEG      31124 /* [%d] ����[%d]���ݿⲿ������ʼ */
#define E_FLW_CFSQL_END      31125 /* [%d] ����[%d]���ݿⲿ��������� */
#define E_FLW_CFFIL_BEG      31126 /* [%d] ����[%d]�ļ���������ʼ */
#define E_FLW_CFFIL_END      31127 /* [%d] ����[%d]�ļ������������ */
#define E_FLW_CFRPT_BEG      31128 /* [%d] ����[%d]����������ʼ */
#define E_FLW_CFRPT_END      31129 /* [%d] ����[%d]������������� */
#define E_FLW_CFCMD_BEG      31130 /* [%d] ����[%d]���������ʼ */
#define E_FLW_CFCMD_END      31131 /* [%d] ����[%d]������������ */
#define E_FLW_CFSWI_BEG      31132 /* [%d] ����[%d]��֧��������ʼ */
#define E_FLW_CFSWI_END      31133 /* [%d] ����[%d]��֧����������� */
#define E_FLW_CFEXP_EXP      31134 /* [%d] ���㲿��ִ�д���[%s] */
#define E_FLW_CFSQL_EXP      31135 /* [%d] ���ݿⲿ��ִ�д���[%s] */
#define E_FLW_CFFIL_BLD      31136 /* [%d] �ļ�����ִ�и�ʽ[%s]��֯���� */
#define E_FLW_CFFIL_PAR      31137 /* [%d] �ļ�����ִ�и�ʽ[%s]�������� */
#define E_FLW_CFRPT_EXP      31138 /* [%d] ������ִ�и�ʽ[%s]���� */
#define E_FLW_CFCMD_EXP      31139 /* [%d] �����ִ������[%s][%s]���� */
#define E_FLW_CFSWI_EXP      31140 /* [%d] ��֧����ִ�д���[%s],��һ����[%d] */
#define E_FLW_CFEND_EXP      31141 /* [%d] ����[%d]�����������ô�����[%s]������Ϣ[%s] */ 
#define E_FLW_DTA_ACALL      31142 /* [%d] �첽����DTA[%s]����[%s] */
#define E_FLW_DTA_GETRPL     31143 /* [%d] �첽���DTA[%s]�Ự��[%ld]������Ϣ */
#define E_FLW_CFDTA_BEG      31144 /* [%d] ����[%d]ͨ�Ų�������ʼ */
#define E_FLW_CFDTA_END      31145 /* [%d] ����[%d]ͨ�Ų���������� */
#define E_FLW_APPSVC         31146 /* [%d] DTAAppServ() ��ʼ���ս������� */
#define E_FLW_DTA_CALL       31147 /* [%d] ����DTA[%s]����[%s]���� */
#define E_FLW_CUST_PROC      31148 /* [%d] ����[%d]ִ�пͻ�������[%s] */
#define E_FLW_ALAEVT         31149 /* [%d] ִ��ALA�ͻ�������[%s] */
#define E_FLW_CFCPT_BEG      31150 /* [%d] ����[%d]Ԫ����������ʼ */
#define E_FLW_CFCPT_END      31151 /* [%d] ����[%d]Ԫ������������� */
#define E_FLW_CFCPT_EXP      31152 /* [%d] Ԫ������[%s]ִ�д���[%s] */

/* ע��32000�Ժ��Ѿ���Ӧ��ռ��,��Ҫ���������� */
#endif
