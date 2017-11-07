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
 * Catalog文件检查代码    999
 * 系统错误               10000-10500
 * 平台公共错误           10501-10999
 * 基础模块错误           11000-11999
 * 报文格式模块           12000-12999
 * 平台流水模块           13000-13999
 * 数据元素模块           14000-14999
 * 负载均衡模块           15000-15999
 * 路由计算模块           16000-16999
 * 表达式模块             17000-17999
 * 共享内存模块           18000-18999
 * 多机通讯模块           19000-19999
 * 客户端管理模块         20000-20999
 * 队列总线模块           21000-21999
 * DTA接口模块            22000-22999
 * 通用DTA模块            23000-23999
 * 错误处理模块           24000-24999
 * Viewstat模块           25000-25999
 * 流程处理模块           26000-26999
 * 数据库封装模块         27000-27999
 * Form处理模块           28000-28999
 * 报表模块               29000-29999
 * 文件传输服务模块       30000-30999
 * 调试日志模块           31000-31999
 * 32000以后 留给应用了!!!
 */

/*
 * 系统错误               10000-10500
 */
#define E_MESSAGE        10000     /* %s */
#define E_PLAT           10001     /* 系统内部错误 */
#define E_EXIT_OK        10002     /* 子进程退出,状态(%s),父进程重启子进程[%d]成功 */
#define E_EXIT_FAIL      10003     /* 子进程退出,状态(%s),父进程重启失败 */
#define E_OSCALL         10201     /* 系统函数(%s)调用出错! */
#define E_FUNCALL        10202     /* 平台函数(%s)调用出错! */
#define E_INVALARG       10203     /* 函数参数(%s)不正确! */
#define E_NOEXEC         10204     /* 文件(%s)不能执行! */
#define E_NOREC          10205     /* 不存在要查找的数据(%s)! */
#define E_SQL            10206     /* SQL执行错(%s)! */
#define E_ALLOC          10300     /* 分配空间[%d]字节失败 */
#define E_NOENV          10301     /* 未设置环境变量[%s] */
#define E_PTR_NULL       10302     /* 指针变量[%s]为空 */
#define E_IP             10303     /* 非法IP[%s] */
#define E_FORK           10304     /* 父进程[%d]启动子进程失败 */
#define E_LISTEN         10305     /* 监听地址[%s:%hu]失败 */
#define E_SOCKET         10306     /* SOCKET操作执行失败[errno=%d] */
#define E_DTA_DEP        10307     /* 取部署共享内存的DTA[%s]信息失败 */
#define E_BALA_PARM      10308     /* 取负载均衡参数失败 */
#define E_HDR_PARM       10309     /* 取参数管理共享内存头指针失败 */
#define E_FILE           10310     /* 文件[%s]操作失败 */
#define E_DTA_PARM       10311     /* 取参数管理共享内存的DTA[%s]信息失败 */
#define E_HDR_RUN        10312     /* 取运行参数共享内存头指针失败 */
#define E_ALA_PARM       10313     /* 取参数管理共享内存的ALA[%s]信息失败 */
#define E_EPGET          10314     /* 取数据元素[%s]的第[%d]个下标的值失败 */
#define E_EPPUT          10315     /* 存数据元素[%s]的第[%d]个下标的值失败 */
#define E_HDR_DEP        10316     /* 取部署共享内存头指针失败 */
#define E_VALUE_INT      10317     /* 错误的整型值[%d] */
#define E_ELEM_DEP       10318     /* 取部署共享内存的数据元素[%s]信息失败 */
#define E_STRING         10319     /* 字符串[%s]操作失败 */
#define E_SOCK_SEND      10320     /* 发送字节数为[%d]的数据失败 */
#define E_SOCK_RECV      10321     /* 接收字节数为[%d]的数据失败 */
#define E_MACHN_PARM     10322     /* 取参数管理共享内存的机器[%s]信息失败 */
#define E_DTAINST_ID     10323     /* 取DTA[%s]的第[%s]号实例的运行共享内存信息失败 */
#define E_DTAMCH_PARM    10324     /* 取DTA[%s]在机器[%s]上的参数管理共享内存失败 */
#define E_DTAINST_RUN    10325     /* 取DTA[%s]实例的运行共享内存信息失败 */
#define E_DTASVC_DEP     10326     /* 取DTA[%s]上服务[%s]的部署共享内存信息错 */
#define E_DEAMON_START   10327     /* 启动常驻进程[%s]失败 */
#define E_SHM_CHECK      10328     /* 检查共享内存失败 */
#define E_VALUE_STRING   10329     /* 错误的字符串[%s] */
#define E_ACCEPT         10330     /* 接收[socket]请求失败 */
#define E_SYS_PARM       10331     /* 取管理共享内存系统信息失败 */
#define E_LOCAL_MACHN_PARM     10332     /* 取本地机器信息失败 */
#define E_IOFNAME_DEP    10333     /* 取报文格式[%s]的定制共享内存信息失败 */
#define E_SIGNNAME_DEP   10334     /* 取标记集[%s]的定制共享内存信息失败 */
#define E_FMTGRP_DEP     10335     /* 取DTA[%s]服务[%s]的报文组定制共享内存信息失败 */
#define E_VALUE_SHORT    10336     /* 错误的短整数[%hd] */
#define E_TRANBASE_DEP   10337     /* 取交易登记区头结构失败 */
#define E_KILL           10338     /* 向进程[%d]发送信号[%s]失败 */
#define E_CVALUE         10339     /* 取队列[%s]中消息数失败 */
#define E_SHELL_CMD      10340     /* 执行shell命令[%s]失败 */
#define E_LOAD_PARM      10341     /* 装载参数管理共享内存失败 */
#define E_LOAD_DEP       10342     /* 装载部署共享内存失败 */
#define E_ATTACH_PARM    10343     /* 连接参数管理共享内存失败 */
#define E_ATTACH_DEP     10344     /* 连接部署共享内存失败 */
#define E_CONNECT        10345     /* 连接地址[%s:%hu]失败 */
#define E_SIGNITEM_DEP   10346     /* 取标记集[%s]的标记[%s]定制共享内存信息失败 */
#define E_EXPR           10347     /* 表达式[%s]计算失败 */
#define E_IOFITEM_BYSIGN_DEP    10348     /* 根据标记名[%s]取格式[%s]的格式项失败 */
#define E_DTANODE_PAR    10349     /* 取DTA[%s]节点[%s]的参数管理信息失败 */
#define E_DTASEQ_DEP     10350     /* 取得并更新DTA[%s]的流水号失败 */
#define E_DTA_BYID_DEP   10351     /* 根据HASH值[%ld]取部署共享内存的DTA信息失败 */
#define E_SIZE_OVERFLOW  10352     /* 空间[%d]超过限制[%d] */
/* ADD BY PENGBIN 2005/07/03 */
#define E_EPUSEDEL_ID    10353     /* 变量池[%d]在使用变量池[%d]登记中未找到 */
#define E_EPUSEDEL_EP    10354     /* 变量池ID[%d]与使用变量池ID[%d]不相同 */
#define E_EPUSEDEL_IDX   10355     /* 使用变量序号[%d]不在使用变量池已使用变量个数[%d]范围内 */
/* ADD END */
#define E_EP_MAXPOOL     10356     /* 进程[%d]使用的变量池个数已经超过限制[%d] */
#define E_HDR_MON        10357     /* 取同步监控共享内存头指针失败 */
#define E_ATTACH_MON     10358     /* 连接监控共享内存失败 */
#define E_SOCKFD_SEND    10359     /* 向描述符[%d]发送字节数为[%d]的数据失败 */
#define E_SOCKFD_RECV    10360     /* 从描述符[%d]接收字节数为[%d]的数据失败 */
#define E_FILE_OPEN      10361     /* 打开文件[%s]失败[%s]*/
#define E_OSCALL_DESC    10362     /* 系统函数[%s]调用失败[%s] */
#define E_COMM_NOTFOUND  10363     /* 在[%s]数组中不能找到指定[%s]数据 */
#define E_SOCK_SND_TIMEOUT   10364     /* 向套结字[%d]在[%d]时间内发送字节数为[%d]的数据超时 */
#define E_SOCK_RCV_TIMEOUT   10365     /* 向套结字[%d]在[%d]时间内接收字节数为[%d]的数据超时 */
#define E_OUTOFBAND      10366     /* 数组[%s]使用第[%d]元素,超出[%d]最大界限 */
#define E_SEMCREATE      10367     /* 创建信号量[%x][%o][%d]失败 */
#define E_SEMREMOVEBYID  10368     /* 删除信号量[%d]失败 */
#define E_SEMSIGNAL      10369     /* 唤醒信号量id[%d],num[%d]失败 */
#define E_SEMWAIT        10370     /* 等待信号量id[%d],num[%d]失败 */
#define E_ATTACH_RUN     10371     /* 连接运行共享内存失败 */
#define E_ATTACH_PARMV   10372     /* 连接参数管理共享内存版本[%s]失败 */
#define E_ATTACH_DEPV    10373     /* 连接部署共享内存版本[%s]失败 */
#define E_HDR_SVCSTAT    10374     /* 取服务状态共享内存头指针失败 */
#define E_HDR_NODESTAT   10375     /* 取节点状态共享内存头指针失败 */
#define E_HDR_ATASKSTAT  10376     /* 取自动任务状态共享内存头指针失败 */
#define E_HDR_BTASKSTAT  10377     /* 取批量任务状态共享内存头指针失败 */
#define E_HDR_TRANSTAT   10378     /* 取逻辑和步骤状态共享内存头指针失败 */
#define E_ATTACH_SVCSTAT 10379     /* 连接服务状态共享内存失败 */
#define E_DTASVC_RUN     10380     /* 取DTA[%s]上服务[%s]的状态共享内存信息错 */
#define E_ATTACH_ATASK   10381     /* 连接自动任务运行状态共享内存失败 */
#define E_GET_ATASK_STAT 10382     /* 取自动任务[%s]运行状态失败 */
#define E_DTAMCH_LISTEN  10383     /* 取DTA[%s]在机器[%s]上的监听地址列表失败 */
#define E_DES3_DEC       10384     /* 用3DES算法解密[%s]长度[%d]密钥[%s]失败 */
#define E_BASE64_DEC     10385     /* 用base64算法解密[%s]失败 */



/*
 * 基础模块错误           11000-11999
 */
#define E_LINK_ADD                  11010   /* [%s]链表增加[%d]节点失败 */
#define E_LINK_DUMP                 11011   /* [%s]链表导出到数组内存失败 */
#define E_FILE_FORMAT               11020   /* 文件[%s]格式错误 */
#define E_LICENSE                   11021   /* 序列号[%s]错误 */
#define E_MATURITY                  11022   /* 已经过了使用期限[%s] */
#define E_LICFLOW                   11023   /* 产品[%s]不具有流程处理权限 */
#define E_LICINST                   11024   /* 产品用户数[%d]超限[%s] */
#define E_SOCKET_ADDR               11100   /* 根据名称[%s]取地址值失败 */
#define E_SOCKET_PORT               11101   /* 根据名称[%s]取端口值失败 */
#define E_SOCKET_CONN               11102   /* 连接地址[%s]端口[%hu]失败 */
#define E_SOCKET_SEND               11103   /* 发送[%ld]字节数据失败 */
#define E_SOCKET_RECV               11104   /* 接受[%ld]字节数据失败 */
#define E_SOCKET_FORK               11105   /* 生成[%d]个连接处理进程失败 */
#define E_SOCKET_FORKONE            11106   /* 生成第[%d]个连接处理进程失败 */
#define E_SOCKET_SERV               11107   /* 监听地址[%s]端口[%hu]失败 */
#define E_SOCKET_REQU               11108   /* 请求地址[%s]端口[%hu]服务失败 */
#define E_SOCKET_INIT               11109   /* 调用初始化函数失败 */
#define E_SOCKET_TRAN               11110   /* 调用连接处理函数失败 */
#define E_SOCKET_MAX                11111   /* 连接请求超过配置的最大数[%d] */
#define E_SOCKET_TERM               11112   /* 服务监听管理进程[%s]异常退出 */
#define E_SOCKET_BIND               11113   /* 绑定指定套结字[%d],[%s]地址[%d]端口失败 */
#define E_SOCKET_LISTEN             11114   /* 监听套结字[%d],队列数[%d]失败 */
#define E_SOCKET_SENDFDS            11115   /* 向域套接字[%d] 发送套接字[%d] 携带[%d]大小数据失败 */
#define E_SOCK_SNDCHLD              11116   /* 向第[%d]个子进程[%d]发送待处理套接字失败 */
#define E_SOCK_NOCHLD               11117   /* 没有可用的子进程 */
#define E_SOCK_ADDCONN              11118   /* 监听进程缓存链路或登记状态失败 */
/* ADD BY HEJN 20130402*/
#define E_LICPLUG                   11119   /* 产品插件数[%d]超过限制[%s] */
#define E_LICDTAINST                11120   /* 产品实例数[%d]超过限制[%s] */
#define E_LICNODE                   11121   /* 产品群集节点数[%d]超过限制[%s] */
#define E_LICERR                    11122   /* 序列号连续错误[%d]天,超过30天平台将停止工作 */
#define E_LICRUN                    11123   /* License运行参数超过限制,平台将停止工作 */
/* ADD END */

/*
 * 报文解析模块         12000-12999
 */
#define E_IOF_TYPE            12000     /* 报文/标记[%s]的类型值[%d]错误 */
#define E_IOF_NEST            12001     /* 报文[%s]的子格式嵌套非法 */
#define E_IOF_GET_TYPE        12002     /* 取报文[%s]的类型失败 */
#define E_IOF_NEST_ITEM       12003     /* 报文[%s]的第[%d]个格式项嵌套的子格式[%s]类型非法 */
#define E_IOF_IOTYPE          12004     /* 报文[%s]的输出类型[%d]错误 */
#define E_IOF_SIGN_NAME       12005     /* 标记类报文[%s]未定义标记集名称 */
#define E_IOF_BUILD           12006     /* 组织报文[%s]失败 */
#define E_IOF_PARSE           12007     /* 解析报文[%s]失败 */
#define E_IOF_ITEM_DEF        12008     /* 报文[%s]的第[%d]个格式项定义非法 */
#define E_IOF_ITEM_BUILD      12009     /* 组织报文[%s]的第[%d]个格式项失败 */
#define E_IOF_CALC_SUB        12010     /* 取报文[%s]的第[%d]个格式项的子格式名称失败 */
#define E_IOF_CALC_SUBREC     12011     /* 取报文[%s]的第[%d]个格式项的子格式记录数失败 */
#define E_IOF_BLD_DATA        12012     /* 组织报文[%s]的第[%d]个格式项的数据值失败 */
#define E_IOF_CALC_ALIGN      12013     /* 计算长度为[%d]的报文对类型[%d]的偏移量失败 */
#define E_IOF_XFML_PUT        12014     /* 存储类FML标记集[%s]的标记[%s]失败 */
#define E_IOF_XFML_FILE       12015     /* 无法将文件与类FML格式[%s]关联 */
#define E_IOF_RECV_FILE       12016     /* 无法对文件类型格式[%s]进行边收边解 */
#define E_IOF_RECV_BUF        12017     /* 通讯描述符或内存地址或长度非法-检查协议报文的定制是否正确 */
#define E_IOF_PARSE_LEN       12018     /* 从报文的第[%d]个字节读取长度为[%d]的数据失败 */
#define E_IOF_SOCK_GETDATA    12019     /* 从SOCKET描述符取数据失败 */
#define E_IOF_PARSE_BITMAP    12020     /* 静态位图格式[%s]与接收位图不相符 */
#define E_IOF_ITEM_PARSE      12021     /* 解析报文[%s]的第[%d]个格式项失败 */
#define E_IOF_PARSE_DATA      12022     /* 解析报文[%s]的第[%d]个格式项的数据值失败 */
#define E_IOF_PARSE_SIGN      12023     /* 从报文的第[%d]个字节读取指定标记[%s]失败 */
#define E_IOF_EXPRTYPE        12024     /* 表达式[%s]返回类型与数据元素[%s]类型不相同 */
#define E_IOF_XFML_GET        12025     /* 获取类FML标记集[%s]的标记[%s]失败 */
#define E_IOF_SIGN_DEF        12026     /* 标记集[%s]定义错误 */
#define E_IOF_GET_SIGN        12027     /* 取XHTTP报文的标记名失败 */
#define E_IOF_COMM            12028     /* 不能对报文[%s]执行边收边解处理 */
#define E_IOF_XFMLNULL        12029     /* 类FML类型报文的接口[%s]未客户化 */
#define E_IOF_FILENULL        12030     /* 格式[%s]未定义文件名取值表达式 */
#define E_IOF_XSUB_DATA       12031     /* 非二进制或字符串数据元素[%s]不能存放扩展子格式数据 */
#define E_IOF_XSUB_LEN        12032 /* 扩展子格式数据长度[%d]超过数据元素[%s]定义的最大长度[%d] */
#define E_IOF_EXPRTYPE_DATATYPE  12033     /* 表达式[%s]返回类型与格式项定义类型不相符 */
#define E_IOF_SUBTYPE_DATATYPE  12034     /* 使用拓展子格式时格式项定义类型必须为BINARY */
#define E_IOF_DETYPE_DATATYPE  12035     /* 数据元素[%s]类型与格式项定义类型不相符 */
#define E_IOF_CALCTYPE_DATATYPE  12036     /* 计算类型与格式项定义类型不相符 */
#define E_IOF_PFMT_XFML          12037     /* 当交易报文为类FML报文时,协议报文不能为非类FML报文 */
#define E_IOF_PARSE_OVER         12038     /* 解析报文[%s]到位置[%d]超过长度[%d] */
#define E_IOF_GETXML_ATTR        12039     /* 取XML报文节点属性[%s]失败 */
#define E_IOF_NEWXML_NODE        12040     /* 生成XML报文节点[%s]失败 */
#define E_IOF_SETXML_ATTR        12041     /* 设XML报文节点属性[%s]失败 */
#define E_IOF_ARRAY_DATA         12042     /* 非二进制数据元素[%s]不能存放ARRAY扩展子格式数据 */
#define E_LOG_PAR_VER            12043     /* 参数共享内存版本号:[%s]  */
#define E_LOG_DEP_VER            12044     /* 部署共享内存版本号:[%s]  */


/*
 * 数据元素模块           14000-14999
 */
#define  E_DELEM_NOELEM     14000       /* 没有数据元素定义 */
#define  E_DELEM_MEM        14001       /* 内存分配失败 */
#define  E_DELEM_ELTYP      14002       /* 数据元素[%s]类型定义el[%d]/da[%d]错误 */
#define  E_DELEM_NOEP       14003       /* 数据元素池[%d]不存在 */
#define  E_DELEM_NOELN      14004       /* 数据元素[%s]不存在 */
#define  E_DELEM_IDX        14005       /* 数据元素[%s]下标[%d]错误,当前个数[%d] */
#define  E_DELEM_NOVAL      14006       /* 数据元素[%s]没有值 */
#define  E_DELEM_USERMEM    14007       /* 用户提供空间为空 */
#define  E_DELEM_COMPRESS   14008       /* 压缩出错 */
#define  E_DELEM_UNCOMPRESS 14009       /* 解压缩出错 */
#define  E_DELEM_TOOBIG     14010       /* 数据元素[%s]值长度[%d]超过最大长度[%d] */
#define  E_DELEM_ELEXIST    14011       /* 数据元素[%s]已存在 */
#define  E_DELEM_OPELTYP    14012       /* 不能变更常量[%s]的内容 */
#define  E_DELEM_TWOELTYP   14013       /* 数据元素[%s]和[%s]的类型不一致 */
#define  E_DELEM_BLDBUF     14014       /* 组织数据元素池数据到缓冲区错误 */
#define  E_DELEM_BUFDATA    14015       /* 分解缓冲区数据到数据元素池错误 */
#define  E_DELEM_DATALEN    14016       /* 数据元素[%s]存值长度[%d]错误 */
#define  E_DELEM_NOEXIST    14100       /* [%s]定制的数据元[%s]不存在 */
#define  E_DELEM_ALLOC      14101       /* 分配变量池失败 */
#define  E_DELEM_PUT        14102       /* 数据元[%s]下标[%d]存值失败 */
#define  E_DELEM_GET        14103       /* 数据元[%s]下标[%d]取值失败 */
#define  E_DELEM_NUMBER     14104       /* 数据元[%s]的类型[%d]不是数值类型 */
#define  E_DELEM_TYPE       14105       /* 取数据元[%s]的类型失败 */
#define  E_DELEM_LEN        14106       /* 取数据元[%s]下标[%d]的值长度失败 */
#define  E_DELEM_TYPELEN    14107       /* 取数据元[%s]类型[%d][%d]折换长度失败 */
#define  E_DELEM_DEIDX      14108       /* 下标数据元素[%s]值[%d]不能小与零 */

#define  E_DELEM_OPINIT     14109       /* 数据元素操作记录未初始化,堆栈号为[%d]*/
#define  E_DELEM_OPMAX      14110       /* 数据元素操作记录堆栈已达上限[%d] */
#define  E_DELEM_XCOPY      14111       /* 变量池[%d][%s]转换到[%d][%s]失败 */
#define  E_DELEM_SYSVAR_COPY      14112 /* 变量池[%d]的系统数据元转换到[%d]失败 */
#define  E_DELEM_EXT        14113       /* 外存变量池无法复制 */
#define  E_DELEM_COPY       14114       /* 变量池[%d][%s]复制到[%d][%s]失败 */
#define  E_DELEM_NODAEL     14115       /* 数据元素[%s]的定义不存在 */
#define  E_DELEM_NOTINCPT   14116       /* 数据元赋值操作[%s]在元件输出列表中不存在 */

/*
 * routing module
 */
#define E_ROUTE_DEFINE              16000   /* DTA[%s]的路由定义错 */
#define E_ROUTE_RULE                16001   /* 未定义DTA[%s]服务[%s]的路由规则 */
#define E_ROUTE_EXPR                16002   /* DTA[%s]路由规则[%s]的表达式[%s]计算错误 */
#define E_ROUTE_ENTR                16003   /* 未定义与DTA[%s]路由规则[%s]的表达式值[%s]对应的路由入口 */
#define E_ROUTE_MACHN               16004   /* DTA[%s]的机器定义错误 */
#define E_ROUTE_BAL                 16005   /* 计算机器[%s]上的DTA[%s]的负载均衡失败 */
#define E_ROUTE_CALC                16006   /* DTA[%s]上服务[%s]的路由计算错误 */
#define E_ROUTE_PARA                16007   /* 没有在参数管理中定义DTA[%s]上路由[%s]的入口[%s]的机器参数 */
#define E_ROUTE_DTA_NORUN           16008   /* 没有运行该DTA[%s]的机器 */
#define E_ROUTE_MCHDTA_NORUN        16009   /* 机器[%s]没有运行该DTA[%s] */
#define E_ROUTE_SVCSTOP             16010   /* 因目的方服务停止,路由失败 */
#define E_ROUTE_SUBSYS              16011   /* 因子系统不符,路由失败 */

/*
 * 表达式模块             17000-17999
 */
#define E_SLANGINIT                 17000   /* S-Lang初始化失败 */
#define E_SLANGLOAD_PUB             17001   /* S-Lang装载系统共用文件[%s]失败 */
#define E_SLANGLOAD_COM             17002   /* S-Lang装载通信接口文件[%s]失败 */
#define E_SLANGLOAD_APP             17003   /* S-Lang装载业务文件[%s]失败 */
#define E_SLANGLOAD_SUB             17004   /* S-Lang装载子业务文件[%s]失败 */
#define E_SLANGLOAD_DTA             17005   /* S-Lang装载DTA文件[%s]失败 */

#define E_PUT_VAR                   17101   /* 数据元素[%s]赋值失败! */
#define E_APP_FUNC                  17102   /* 应用函数(%s)调用出错! */
#define E_GET_VAR                   17103   /* 数据元素[%s]取值失败! */
#define E_DATA_TYPE                 17104   /* 平台函数(%s)调用出错,此数据类型不正确! */
#define E_CALC                      17105   /* 平台函数(%s)调用出错,计算出错! */
#define E_STR_CALC                  17106   /* 字符型数据参与了减、乘、除运算! */
#define E_DIV_IS_ZERO               17107   /* 除数为０! */
#define E_SYNTAX                    17108   /* 语法错，错误位置[%d!][%s...] */
#define E_GET_EXPR_VAL              17109   /* 表达式第[%d]条语句求值出错! */

/*
 * 共享内存模块           18000-18999
 */
#define    E_SHMDB_LOAD_XML         18000   /* 装载节点[%s]的XML文件[%s]信息失败 */
#define    E_SHMDB_LOAD_PAR         18001   /* 装载参数管理[%s]配置信息失败 */
#define    E_SHMDB_LOAD_DEP         18002   /* 装载业务部署[%s]配置信息失败 */
#define    E_SHMDB_LOAD_PROJECT     18003   /* 装载部署资源树[%s]配置信息失败 */
#define    E_SHMDB_LOAD_DTA         18004   /* 装载部署DTA[%s]配置信息失败 */
#define    E_SHMDB_LOAD_DTA_INST    18005   /* 装载部署DTA实例[%s]配置信息失败*/
#define    E_SHMDB_LOAD_DTA_NODE    18006   /* 装载部署DTA节点[%s]配置信息失败*/
#define    E_SHMDB_LOAD_DTA_SVC     18007   /* 装载部署DTA服务[%s]配置信息失败*/
#define    E_SHMDB_LOAD_REV_SVC     18008   /* 装载部署冲正服务[%s]配置信息失败 */
#define    E_SHMDB_LOAD_FMT_GRP     18009   /* 装载部署格式组[%s]配置信息失败 */
#define    E_SHMDB_LOAD_ELEM        18010   /* 装载部署数据元[%s]配置信息失败 */
#define    E_SHMDB_LOAD_MAP         18011   /* 装载部署数据映射[%s]配置信息失败 */
#define    E_SHMDB_LOAD_SIGN        18012   /* 装载部署标记[%s]配置信息失败 */
#define    E_SHMDB_LOAD_SIGN_ITEM   18013   /* 装载部署标记项[%s]配置信息失败 */
#define    E_SHMDB_LOAD_FORMAT      18014   /* 装载部署格式[%s]配置信息失败 */
#define    E_SHMDB_LOAD_FMT_ITEM    18015   /* 装载部署格式项[%s]配置信息失败 */
#define    E_SHMDB_LOAD_RULE        18016   /* 装载部署路由规则[%s]配置信息失败 */
#define    E_SHMDB_LOAD_ENTRANCE    18017   /* 装载部署路由入口[%s]配置信息失败 */
#define    E_SHMDB_LOAD_ALA         18018   /* 装载部署子业务[%s]配置信息失败 */
#define    E_SHMDB_LOAD_FLOW        18019   /* 装载部署流程[%s]配置信息失败 */
#define    E_SHMDB_LOAD_LOGIC       18020   /* 装载部署逻辑[%s]配置信息失败 */
#define    E_SHMDB_LOAD_FORM        18021   /* 装载部署FORM[%s]配置信息失败 */
#define    E_SHMDB_LOAD_TASK        18022   /* 装载部署任务[%s]配置信息失败 */
#define    E_SHMDB_INIT_DTA_DE_COL  18023   /* 装载部署DTA数据元集合[%s]配置信息失败 */
#define    E_SHMDB_MERGE_ALL        18024   /* 合并生成共享内存头结构失败 */
#define    E_SHMDB_DTA_NUM          18025   /* 取DTA数目失败 */
#define    E_SHMDB_IPC              18026   /* 取IPC PERM标识失败 */
#define    E_SHMDB_CREATE_SHM       18027   /* 创建[%d]字节共享内存失败 */
#define    E_SHMDB_SHM_REM_KEY      18028   /* 通过键值[%s]删除共享内存失败 */
#define    E_SHMDB_SHM_ATT_KEY      18029   /* 通过键值[%s]连接共享内存失败 */
#define    E_SHMDB_SHM_DETACH       18030   /* 断开共享内存失败 */
#define    E_SHMDB_XML_PARSE        18031   /* 解析XML文件[%s]失败 */
#define    E_SHMDB_XML_ROOT         18032   /* 取XML文件[%s]根节点信息失败 */
#define    E_SHMDB_NODE_NAME        18033   /* 根据名称[%s]取子节点信息失败 */
#define    E_SHMDB_ATTR_NAME        18034   /* 取节点[%s]的属性信息失败 */
#define    E_SHMDB_ATTR_LIM         18035   /* 节点[%s]的属性[%s]与定义不符 */
#define    E_SHMDB_XML_REC          18036   /* [%s]记录数[%d]与实际项[%d]不符 */
#define    E_SHMDB_XML_NAME         18037   /* 取节点[%s]的子项失败 */
#define    E_SHMDB_APPXML_LOAD      18038   /* 业务[%s]装载[%s]XML函数调用失败 */
#define    E_SHMDB_SUBAPPXML_LOAD   18039   /* 子业务[%s]装载[%s]XML函数调用失败 */
#define    E_SHMDB_PTR_NULL         18049   /* 指针变量[%s]为空 */
#define    E_SHMDB_ADDBUF           18050   /* [%s]内存增加第[%d]节点失败 */
#define    E_SHMDB_PAR_MAXALA       18060   /* 子业务ALA个数超过限制[%d] */
#define    E_SHMDB_PAR_MAXALAMCH    18061   /* 子业务ALA运行机器个数超过限制[%d] */
#define    E_SHMDB_PAR_MAXDTA       18062   /* 通信DTA个数超过限制[%d] */
#define    E_SHMDB_PAR_MAXDTAMCH    18063   /* 通信DTA运行机器个数超过限制[%d] */
#define    E_SHMDB_PAR_MAXDTANODE   18064   /* 通信DTA运行节点个数超过限制[%d] */
#define    E_SHMDB_PAR_MAXFS        18065   /* 文件服务器个数超过限制[%d] */
#define    E_SHMDB_PAR_MAXMCH       18066   /* 部署机器个数超过限制[%d] */
#define    E_SHMDB_PAR_MAXRUT       18067   /* 路由表的记录数超过限制[%d] */
#define    E_SHMDB_PROJECT          18100   /* 取业务部署共享内存的资源树信息失败 */
#define    E_SHMDB_DRQ_NAME         18101   /* 不存在DTA队列名[%s]，检查DTA是否启动 */
#define    E_SHMDB_DTA_NAME         18102   /* 不存在DTA名[%s] */
#define    E_SHMDB_DTAINST_NAME     18103   /* 根据DTA名[%s]取[%d]号实例信息失败*/
#define    E_SHMDB_DTANODE_NAME     18104   /* 根据DTA名[%s]取节点[%s]信息失败*/
#define    E_SHMDB_DTASVC_NAME      18105   /* 根据DTA名[%s]取服务[%s]信息失败*/
#define    E_SHMDB_REVSVC_NAME      18106   /* 根据DTA名[%s]取冲正客户化[%s]信息失败*/
#define    E_SHMDB_FMTGRP_NAME      18107   /* 根据DTA名[%s]取服务[%s]报文组信息失败*/
#define    E_SHMDB_ELEM_NAME        18108   /* 不存在数据元名[%s] */
#define    E_SHMDB_DTAMAP_NAME      18109   /* 不存在DTA[%s]与DTA[%s]的映射 */
#define    E_SHMDB_SIGN_NAME        18110   /* 不存在标记名[%s] */
#define    E_SHMDB_FORMAT_NAME      18111   /* 不存在格式名[%s] */
#define    E_SHMDB_RULE_NAME        18112   /* 不存在路由规则名[%s] */
#define    E_SHMDB_ENTRANCE_NAME    18113   /* 不存在路由入口名[%s] */
#define    E_SHMDB_ALA_NAME         18114   /* 不存在ALA名[%s] */
#define    E_SHMDB_TRAN_NAME        18115   /* 不存在交易名[%s] */
#define    E_SHMDB_FLOW_NAME        18116   /* 不存在交易[%s]对应的流程[%s]定制 */
#define    E_SHMDB_FLOW_NOSEQ       18117   /* 不存在交易[%s]顺序号属性 */
#define    E_SHMDB_FLOW_TRANSEQ     18118   /* 不存在交易[%s]流程[%s]的顺序号[%d]定制 */
#define    E_SHMDB_FORM_NAME        18119   /* 不存在数据元集合名[%s] */
#define    E_SHMDB_BTASK_NAME       18120   /* 不存在日终任务名[%s] */
#define    E_SHMDB_ATASK_NAME       18121   /* 不存在自动任务名[%s] */
#define    E_SHMDB_MAX_MAP_ITEM     18122   /* 映射目的DTA[%s]的字段个数[%d]超过限制[%d] */
#define    E_SHMDB_APP_NAME         18130   /* 不存在业务[%s]的资源树节点 */
#define    E_SHMDB_SUBAPP_NAME      18131   /* 不存在子业务[%s]的资源树节点 */
#define    E_SHMDB_MCH_NAME         18150   /* 不存在机器名参数[%s] */
#define    E_SHMDB_MCH_NOFSRV       18151   /* 不存在机器名[%s]的文件服务参数 */
#define    E_SHMDB_MCH_DTA          18152   /* 不存在机器[%s]的DTA[%s] */
#define    E_SHMDB_MCH_ALA          18153   /* 不存在机器[%s]的ALA[%s] */
#define    E_SHMDB_DATATYPE         18154   /* 不存在[%s]类型 */
#define    E_SHMDB_DRQ_TBL          18155   /* 不存在DTA[%s]的[%d]的DRQ队列 */
#define    E_SHMDB_PAR_MAXSVC       18156   /* 服务个数超过限制[%d] */

#define    E_SHMDB_PAR_HDR          18200   /* 取参数管理共享内存头失败 */
#define    E_SHMDB_RUN_HDR          18201   /* 取运行参数共享内存头失败 */
#define    E_SHMDB_DEP_HDR          18202   /* 取业务部署共享内存头失败 */
#define    E_SHMDB_PAR_ATT          18203   /* 连接参数管理共享内存失败 */
#define    E_SHMDB_DEP_ATT          18204   /* 连接业务部署共享内存失败 */
#define    E_SHMDB_PAR_CHK          18205   /* 检查参数管理共享内存失败 */
#define    E_SHMDB_DEP_CHK          18206   /* 检查业务部署共享内存失败 */
#define    E_SHMDB_PAR_LOAD         18207   /* 装载参数管理共享内存失败 */
#define    E_SHMDB_DEP_LOAD         18208   /* 装载业务部署共享内存失败 */
#define    E_SHMDB_PAR_REMOVE       18209   /* 删除参数管理共享内存失败 */
#define    E_SHMDB_DEP_REMOVE       18210   /* 删除业务部署共享内存失败 */
/* 20091016 handq for 3.2 */
#define    E_SHMDB_RUN_BASE         18211   /* 创建基础运行共享内存失败 */
#define    E_SHMDB_RUN_AUTOTASK     18212   /* 创建自动任务状态扩展运行共享内存失败 */
#define    E_SHMDB_RUN_BATTASK      18213   /* 创建批量任务状态扩展运行共享内存失败 */
#define    E_SHMDB_RUN_DTANODE      18214   /* 创建DTA节点状态运行共享内存失败 */
#define    E_SHMDB_RUN_TRAN         18215   /* 创建交易及步骤状态运行共享内存失败 */
#define    E_SHMDB_RUN_CHK          18216   /* 检查运行状态共享内存失败 */
#define    E_SHMDB_RUN_REMOVE       18217   /* 删除运行状态共享内存失败 */


#define    E_SHMDB_MAXINSTNUM       18301   /* 运行的DTA实例数超过平台限制[%d] */
#define    E_SHMDB_RUNINIT_LOCAL    18302   /* 初始化本地运行参数失败 */
#define    E_SHMDB_RUNINIT_SYNC     18303   /* 初始化同步运行参数失败 */
#define    E_SHMDB_RUNINIT_DTAINST  18304   /* 初始化DTA实例运行参数失败 */
#define    E_SHMDB_MCH_LISTEN_ADDR  18305   /* 不存在监听在地址[%s:%hu]上的机器,检查ESAdmin.xml的机器参数 */
#define    E_SHMDB_RUNINIT_FSINST   18306   /* 初始化文件服务实例运行参数失败 */
#define    E_SHMDB_RUN_NOFSRV       18307   /* 取文件服务实例运行参数[%s]失败 */
#define    E_SHMDB_RUN_NOMCH        18308   /* 取机器[%s]的状态信息失败 */
#define    E_SHMDB_RUN_NODTA        18309   /* 取机器[%s]的DTA[%s]状态信息失败 */
#define    E_SHMDB_RUN_DTASTAT      18310   /* 机器[%s]的DTA[%s]状态为停止 */
#define    E_SHMDB_LOAD_FUNCCHK     18311   /* 装载检查函数返回值列表失败 */
#define    E_SHMDB_GET_FUNCCHK      18312   /* 取节点[%s]检查函数返回值列表[%s]失败 */
#define    E_SHMDB_LOAD_FMTRPT      18313   /* 装载格式报表失败 */
#define    E_SHMDB_GET_FMTRPT       18314   /* 取节点[%s]格式报表[%s]失败 */
#define    E_SHMDB_PAR_MAXDB        18315   /* 超过最大数据库个数 */
#define    E_SHMDB_PAR_MAXPRIOINST  18316   /* DTA/ALA运行优先级实例数超过限制[%d] */
/* ADD BY PENGBIN 2007/3/13 */
#define    E_SHMDB_DTA_PARL         18317   /* 不存在DTA[%s]机器[%s]的并发数信息 */
#define    E_DTA_PARL               18318   /* DTA[%s]机器[%s]并发数[%hd]超限[%hd] */
#define    E_DTA_PARL_LVL           18319   /* DTA[%s]机器[%s]优先级[%hd]并发数[%hd]超限[%hd] */
#define    E_SHMDB_PAR_MAXDTAPARL   18320   /* DTA并发数配置个数超过限制[%d] */
#define    E_SHMDB_RUNINIT_DTAPAL   18321   /* 初始化DTA并发数运行参数失败 */
#define    E_SHMDB_RUNINIT_CUSTPAL  18322   /* 初始化客户化条目并发数运行参数失败 */
#define    E_SHMDB_CUST_PAL         18323   /* 增加源DTA[%s]客户化条目[%s]并发数参数失败 */
#define    E_CUST_PAL               18324   /* 源DTA[%s]客户化条目[%s]并发数[%hd]超限[%hd] */
#define    E_SHMDB_CUSTPAL_MAX      18325   /* 客户化条目个数已达最大值[%d] */
#define    E_SVCID_RUN_INST         18326   /* DTA[%s]已经有实例[%d]在执行黑名单服务身份ID[%s]的交易 */
#define    E_SHMDB_SVCID            18327   /* 查询或增加服务身份ID[%s]的信息失败 */
#define    E_SHMDB_RUNINIT_SVCID    18328   /* 初始化服务身份ID并发数运行参数失败 */
#define    E_SHMDB_SVCID_MAX        18329   /* 服务身份ID个数超过最大个数[%d] */
#define    E_SVCID_RUN              18330   /* 服务身份ID[%s]标记为黑名单,并且有[%d]笔交易正在处理 */
/* ADD END */
#define    E_SHMDB_MONINIT          18331   /* 初始化同步监控共享内存失败 */
#define    E_SHMDB_MON_MCH          18332   /* 不存在机器[%s]的同步信息 */
#define    E_SHMDB_MON              18333   /* 监控共享内存错误 */
#define    E_SHMDB_RUNINIT_BAL      18334   /* 初始化负载均衡控制结构失败 */
#define    E_SHMDB_PAR_MAXDTACONN   18335   /* 通信DTA运行客户端IP个数超过限制 */
#define    E_SHMDB_RUNINIT_LNS      18336   /* 初始化长连接登记区失败 */
#define    E_SHMDB_PAR_MAXLISTEN    18337   /* DTA监听地址列表超过最大个数[%d]限制 */
#define    E_SHMDB_MAXTZNUM         18338   /* 任务[%s]的时间段个数[%d]超过限制[%d] */
#define    E_SHMDB_MAXES_TYPE       18339   /* 未知的系统最大值类型[%d] */
#define    E_SHMDB_MAXDRQSEQNUM     18340   /* DRQ流水号参数个数超过最大个数[%d]限制 */
#define    E_SHMDB_SHMVER           18341   /* 类型[%d]版本[%s]的共享内存状态不存在 */
#define    E_SHMDB_NOIDLE           18342   /* 类型[%d]的共享内存不为空闲状态 */
#define    E_MAX_SHMVER             18343   /* 最大并存共享内存数[%d]小于更新的顺序号[%d] */
#define    E_SHMDB_RUNINIT_MAXES    18344   /* 初始化运行管理最大值限制失败 */

#define    E_SHMDB_MAX_TRAN         18345   /* 交易状态个数[%d]超过限制[%d] */
#define    E_SHMDB_NOEXIT           18346   /* 共享内存[%s]不存在 */
#define    E_SHMDB_SHM_ATT_ID       18347   /* 根据共享内存ID号[%d]连接共享内存失败 */
#define    E_SHMDB_SHM_REM_ID       18348   /* 通过标识号shmid[%d]删除共享内存失败 */
#define    E_SHMDB_RUNREF_SVCSTAT   18349   /* 刷新服务状态共享内存失败 */
#define    E_SHMDB_RUNREF_NODESTAT  18350   /* 刷新节点状态共享内存失败 */
#define    E_SHMDB_RUNREF_ATASK     18351   /* 刷新自动任务状态共享内存失败 */
#define    E_SHMDB_RUNREF_BTASK     18352   /* 刷新批量任务状态共享内存失败 */
#define    E_SHMDB_RUNREF_TRANSTAT  18353   /* 刷新逻辑和步骤状态共享内存失败 */
#define    E_SHMDB_RUN_DTASVC       18354   /* 创建DTA服务状态运行共享内存失败 */
#define    E_SHMDB_VER_EXIST        18355   /* 共享内存类型[%d]版本[%s]已经存在 */
#define    E_SHMDB_DTASVCSTAT       18356   /* 取DTA[%s]上服务[%s]状态失败 */
#define    E_SHMDB_SET_DTASTAT      18357   /* 设置DTA[%s]的状态[%d]失败 */
#define    E_SHMDB_INVALMAXES       18358   /* 最大值参数[%s]非法 */
#define    E_SHMDB_REFSTATBYDEP     18359   /* 根据部署共享内存[%s]刷新状态共享内存失败 */
#define    E_SHMDB_REFSTATBYPAR     18360   /* 根据参数共享内存[%s]部署共享内存[%s]刷新状态共享内存失败 */
#define    E_SHMDB_REFSTAT          18361   /* 根据参数共享内存[%s]和部署共享内存[%s]刷新状态共享内存失败 */
#define    E_SHMDB_MAXSTOPSVCNUM    18362   /* 停止服务状态个数超过最大值[%d]限制 */
#define    E_SHMDB_GET_STOPSVC      18363   /* 取机器[%s]上DTA[%s]服务[%s]的停止状态失败 */
#define    E_SHMDB_RESTOR           18364   /* 从映像文件恢复版本为[%s]的共享内存失败 */
#define    E_SHMDB_CURVER           18365   /* 类型[%d]版本[%s]的是当前共享内存版本,不能删除 */
#define    E_SHMDB_DEP_CREATE       18366   /* 创建第[%d]个部署共享内存失败 */
#define    E_SHMDB_PAR_CREATE       18367   /* 创建第[%d]个参数共享内存失败 */
#define    E_SHMDB_DDTA_BYREF       18368   /* 因为DTA[%s]被删除,平台将停止该DTA再生效刷新 */
#define    E_SHMDB_DNODE_BYREF      18369   /* 因为机器节点[%s]被删除,平台将停止该DTA再生效刷新 */
#define    E_SHMDB_DMAST_BYREF      18370   /* 新参数删除了当前主控节点[%s],不能生效 */
#define    E_SHMDB_PAR_ACTFAIL      18371   /* 参数共享内存预约刷新生效失败 */
#define    E_SHMDB_DEP_ACTFAIL      18372   /* 部署共享内存预约刷新生效失败 */
#define    E_SHMDB_PAR_ACTSUCC      18373   /* 参数共享内存预约刷新生效成功 */
#define    E_SHMDB_DEP_ACTSUCC      18374   /* 部署共享内存预约刷新生效成功 */
#define    E_SHMDB_RCYCL_DEP        18375   /* 部署共享内存[%s]长时间未使用被回收 */
#define    E_SHMDB_RCYCL_PAR        18376   /* 参数共享内存[%s]长时间未使用被回收 */
#define    E_SHMDB_RCYCL_NOREC      18377   /* 参数共享内存[%d:%s]本地未找到版本未回收 */
#define    E_SHMVER_RSTMAX          18388   /* 最大并存共享内存数[%d]小于更新的顺序号[%d],版本[%s]未恢复 */
#define    E_SHMDB_SHMVER_INACTIVE  18389   /* 类型[%d]版本[%s]的共享内存状态[%d]不是可用状态,不允许连接 */
#define    E_SHMDB_RUNSET_DTAINST  18390   /* 设置DTA实例状态失败 */
#define    E_SHMDB_PAR_ATASKMMCH    18391   /* 集群模式下,自动任务未定义主控节点[%s]机器参数 */
#define    E_SHMDB_MAXSVRDYNCON       18392   /* 超过最大运行的动态服务系统并发数信息 */
#define    E_SHMDB_SVCDEF       18393 /*获取服务系统[%s]服务队则[%s]的控制信息失败 */
#define    E_SHMDB_SVRSYS_NOTFOUND       18394 /* 未定义服务系统并发数控制信息，忽略错误 */


/*
 * eslisten         19000 - 19999
 */
#define    E_ESL_START_DAEMON       19001   /* 启动常驻进程[%s]失败 */
#define    E_ESL_ESLISTEN_BOOT      19002   /* ESLISTEN启动失败 */
#define    E_ESL_COMM               19003   /* ESLISTEN与命令发起端通信失败 */
#define    E_ESL_REQUEST            19004   /* ESLISTEN报文非法 */
#define    E_ESL_REQ_HEAD           19005   /* ESLISTEN报文头非法 */
#define    E_ESL_SHMPAR_LOAD        19006   /* 装载参数管理共享内存失败 */
#define    E_ESL_DTAM_EXIT          19007   /* DTA管理进程[%s]退出 */
#define    E_ESL_LISTEN_EXIT        19008   /* ESLISTEN监听进程退出 */
#define    E_ESL_REQ_PROC           19009   /* ESLISTEN处理命令[%s]的子命令[%s]失败 */
#define    E_ESL_LOCK               19010   /* ESLISTEN锁[%s]操作错 */
#define    E_ESL_ISSUE_EXIT         19011   /* 发布进程[%s]退出 */
#define    E_ESL_PARM_ATTACH        19012   /* 连接管理共享内存失败 */
#define    E_ESL_ISSUE              19013   /* 发布运行参数失败 */
#define    E_ESL_ISSUE_GATHER       19014   /* 采集节点[%s]同步信息失败 */
#define    E_ESL_SHM_SYNC           19015   /* 节点同步信息存储错 */
#define    E_ESL_ISSUE_UPDATE       19016   /* 更新同步信息失败 */
#define    E_ESL_SHM_PARM           19017   /* 节点管理信息存储错 */
#define    E_ESL_ISSUE_OFFLINE      19018   /* 发生错误[%s],导致主节点无法更新从节点[%s]状态,设置从节点为OFFLINE */
#define    E_ESL_ISSUE_BUILD        19019   /* 组织发布信息失败 */
#define    E_ESL_ISSUE_PARSE        19020   /* 解析同步信息报文[%s]失败 */
#define    E_ESL_SYNC_EXIT          19021   /* 同步进程[%s]退出 */
#define    E_ESL_SYNC_CHLD_MAX      19022   /* 同步进程连接数超过最大节点数 */
#define    E_ESL_RCYCL_EXIT         19023   /* 垃圾回收进程[%s]退出 */
#define    E_ESL_RCYCL_TRAN         19024   /* 回收垃圾交易失败 */
#define    E_ESL_RCYCL_BUFATT       19025   /* 根据键值[%d]连接数据缓冲区失败 */
#define    E_ESL_DEP_ATTACH         19026   /* 连接部署共享内存失败 */
#define    E_ESL_RCYCL_DRQFAIL      19027   /* 回收DTA[%s]队列[%s]中垃圾交易失败 */
#define    E_ESL_DTAM_RCYCL         19028   /* DTA管理进程垃圾回收失败 */
#define    E_ESL_DTAM_BDTA          19029   /* 启动DTA[%s]失败 */
#define    E_ESL_DTAM_DDTA          19030   /* 停止DTA[%s]失败 */
#define    E_ESL_DTAM_RBDTA         19031   /* 进程[%d]终止后,平台处理失败 */
#define    E_ESL_DTAM_BDTA_SUCC     19032   /* 启动DTA[%s]共[%d]个实例 */
#define    E_ESL_DTAM_DDTA_SUCC     19033   /* 停止DTA[%s]成功 */
#define    E_ESL_DTAM_PID           19034   /* 进程[%d]未在系统中登记 */
#define    E_ESL_DTAM_RBDTA_SUCC    19035   /* 重启动DTA[%s]进程[%d]成功 */
#define    E_ESL_DTAM_FLEX          19036   /* DTA管理进程动态伸缩失败 */
#define    E_ESL_DTAM_BDTA_FLEX     19037   /* 动态伸缩模块启动DTA[%s]一个实例[%d] */
#define    E_ESL_DTAM_DDTA_FLEX     19038   /* 动态伸缩模块停止DTA[%s]一个实例[%d] */
#define    E_ESL_DTAM_BDTA_FLEX_F   19039   /* 动态伸缩模块启动DTA[%s]一个实例[%d]失败 */
#define    E_ESL_DTAM_RBDTA_FAIL    19040   /* 重启动DTA[%s]一个实例失败 */
#define    E_ESL_DTAM_DDTA_FLEX_F   19041   /* 动态伸缩模块停止DTA[%s]一个实例[%d]失败 */
#define    E_ESL_DTAM_DDTA_INST     19042   /* 停止DTA[%s]第[%d]实例失败 */
#define    E_ESL_DTAM_BDTA_INST     19043   /* 启动DTA[%s]第[%d]实例失败 */
#define    E_ESL_PKG_NULL           19044   /* 待解析报文为空 */
#define    E_ESL_PKG_FIELD          19045   /* 待解析报文只有[%d]个域 */
#define    E_ESL_PKG_XMLPARSE       19046   /* 解析XML配置数据失败 */
#define    E_ESL_PKG_XMLBUILD       19047   /* 组织XML配置数据失败 */
#define    E_ESL_FIELD_PARSE        19048   /* 取第[%d]个域的值失败 */
#define    E_ESL_SLAVE_CMD          19049   /* 从节点[%s]上不能执行命令[%s],因为此命令需要转发至其他节点[%s] */
#define    E_ESL_REQ_PROC_REMOTE    19050   /* 远程节点[%s]处理命令[%s]的子命令[%s]失败 */
#define    E_ESL_DRQ_INIT           19051   /* DRQ初始化失败 */
#define    E_ESL_DTA_PROC           19052   /* [%s]DTA[%s]失败 */
#define    E_ESL_MODE_SWITCH        19053   /* 系统成功切换到状态[%s] */
#define    E_ESL_MCH_INVALID        19054   /* 不存在指定的机器名[%s] */
#define    E_ESL_DTAM_REQUEST       19055   /* DTA管理进程处理[%s]类请求失败 */
#define    E_ESL_DTAM_FAILURE       19056   /* DTA管理进程处理失败 */
#define    E_ESL_PKG_LENFIELD       19057   /* 待解析报文第[%d]个长度域非法 */
#define    E_ESL_DTAM_CLRDRQ        19058   /* 清除DTA[%s]的队列资源失败 */
#define    E_ESL_ESLISTEN_BOOT_SUCC 19059   /* ESLISTEN监听进程启动成功 */
#define    E_ESL_DTAM_BOOT_SUCC     19060   /* DTA管理进程[%s]启动成功 */
#define    E_ESL_ISSUE_BOOT_SUCC    19061   /* 发布进程[%s]启动成功 */
#define    E_ESL_RCYCL_BOOT_SUCC    19062   /* 垃圾回收进程[%s]启动成功 */
#define    E_ESL_SYNC_BOOT_SUCC     19063   /* 同步进程[%s]启动成功 */
#define    E_ESL_CUST_START_SUCC    19064   /* 客户化启动脚本[%s]执行成功 */
#define    E_ESL_CUST_SHUT_SUCC     19065   /* 客户化终止脚本[%s]执行成功 */
#define    E_ESL_CUST_START_FAIL    19066   /* 客户化启动脚本[%s]执行失败 */
#define    E_ESL_CUST_SHUT_FAIL     19067   /* 客户化终止脚本[%s]执行失败 */
#define    E_ESL_EXEC_DAEMON_FAIL   19068   /* ESLISTEN执行进程[%s]失败 */
#define    E_ESL_DTAM_EXIST         19069   /* DTA管理进程[%d]已经存在 */
#define    E_ESL_SYNC_EXIST         19070   /* 同步进程[%d]已经存在 */
#define    E_ESL_ISSUE_EXIST        19071   /* 发步进程[%d]已经存在 */
#define    E_ESL_RCYCL_EXIST        19072   /* 垃圾回收进程[%d]已经存在 */
#define    E_ESL_DTAM_ISRUN         19073   /* DTA[%s]已经是启动状态 */
#define    E_ESL_DTAM_RUNPLAT       19074   /* 正在启动平台启动DTA[%s]的[%d]个实例... */
#define    E_ESL_DTAM_RUNTRIG       19075   /* 正在启动触发启动DTA[%s]的第一个实例... */
#define    E_ESL_DTAM_RUNUSER       19076   /* 手工启动DTA[%s]暂不启动实例... */
#define    E_ESL_MODE_CHK_VER       19077   /* 机器节点[%s]的版本不符，开始同步XML配置文件... */
#define    E_ESL_DTAM_DOWN_SVC      19078   /* 进程[%d]的服务处理[%s]被中断 */
#define    E_ESL_RECV_CMD           19079   /* ESLISTEN收到客户端[%s]命令[%s:%s:%s]，进行处理... */
#define    E_ESL_SEND_CMD           19080   /* ESLISTEN完成客户端[%s]命令[%s:%s:%s]处理 */
#define    E_ESL_SYS_BUSY           19081   /* 系统有正在处理交易[%s]，无法刷新 */
#define    E_ESL_RECV_ESCMD         19082   /* ESCMD收到客户端[%s]命令[%s]，进行处理... */
#define    E_ESL_SEND_ESCMD         19083   /* ESCMD完成客户端[%s]命令[%s]处理 */
#define    E_ESL_ISSUE_GOSINFO      19084   /* 采集节点[%s]操作系统信息失败 */
#define    E_ESL_MONC               19085   /* 采集其他节点监控信息失败 */
#define    E_ESM_FMT_CONTENT        19086   /* 请求资源类型[%s]格式错误 */
#define    E_ESM_TYPE_CONTENT       19087   /* 请求资源类型[%s]类型未知错误 */
#define    E_ESL_SET_MASTER         19088   /* 设置新主节点[%s]失败 */
#define    E_ESL_NEW_MASTER         19089   /* 主节点将变为机器[%s] */
#define    E_ESL_NODE_STATUS        19090   /* 检查节点[%s]机器状态失败 */
#define    E_ESL_MNODE              19091   /* 备选主节点处理失败 */
#define    E_ESL_NNODE              19092   /* 从节点处理失败 */
#define    E_ESL_RESET              19093   /* 涉及机器[%s]的负载复位失败*/
#define    E_ESL_RESET_LOCAL        19094   /* 复位本机负载信息失败 */
#define    E_ESL_RESET_MCH          19095   /* 复位机器[%s]负载命令处理失败 */
#define    E_ESL_TSF_XCMD           19096   /* 命令[%s %s]转发到机器[%s]执行 */
#define    E_ESL_TSF_XCMD_RET       19097   /* 机器[%s]执行命令[%s:%s]结果[%s]返回信息[%s] */
#define    E_ESL_TSF_RPAR           19098   /* 参数共享内存刷新操作转发到机器[%s]执行 */
#define    E_ESL_TSF_RPAR_RET       19099   /* 机器[%s]执行参数共享内存刷新结果[%s]返回信息[%s] */
#define    E_ESL_TSF_RDEP           19100   /* 部署共享内存刷新操作转发到机器[%s]执行 */
#define    E_ESL_TSF_RDEP_RET       19101   /* 机器[%s]执行部署共享内存刷新结果[%s]返回信息[%s] */
#define    E_ESL_PARSE_PKG          19102   /* ESLISTEN 解析报文[%s]字段[%s]非法[%s] */
#define    E_ESL_STATUS             19103   /* ESLISTEN状态不正确,不能执行指定命令 */
#define    E_ESL_DTAM_BSVC_SUCC     19104   /* 启动DTA[%s]服务[%s]成功 */
#define    E_ESL_DTAM_DSVC_SUCC     19105   /* 停止DTA[%s]服务[%s]成功 */
#define    E_ESL_DTAM_BSVC_FAIL     19106   /* 启动DTA[%s]服务[%s]失败 */
#define    E_ESL_DTAM_DSVC_FAIL     19107   /* 停止DTA[%s]服务[%s]失败 */
#define    E_ESL_NODE_ALL           19108   /* 该命令只能向所有节点发起 */
#define    E_ESL_PUSHAGT_EXIST      19109   /* 主动PUSH进程[%d]已经存在 */
#define    E_ESL_BALCTRL_INIT       19110   /* 初始化负载均衡控制区失败 */
#define    E_ESL_DTAM_SLEEP         19111   /* ESLDTAM根据参数等待[%d]秒 */
#define    E_ESL_LNSLOCK_INIT       19112   /* 链路管理信号灯初始化失败 */
#define    E_ESL_DTAM_BDTA_ATB      19113   /* DTA[%s]实例[%d]自动重启[%d] */
#define    E_ESL_DTAM_DDTA_ATB      19114   /* DTA[%s]实例[%d][%d]满足自动重启条件,需要退出 */
#define    E_ESL_DTAM_NORBT         19115   /* DTA[%s]因重启间隔未重启,常驻实例[%d],当前实例[%d] */
#define    E_ESL_DTA_BUSY           19116   /* 系统有繁忙DTA[%s]，无法刷新 */
#define    E_ESL_DENY_ADD           19117   /* 只能修改现有配置项[%s]，不能新增 */
#define    E_ESL_ISSUE_RCYSHM       19118   /* 回收共享内存版本处理失败 */
#define    E_ESL_REFRESH_CHK        19119   /* 刷新共享内存[%d]版本检查部署版本[%s]参数版本[%s]失败 */
#define    E_ESL_REF_PVER           19120   /* 参数目标共享内存版本[%s]和XML文件版本[%s]不一致 */
#define    E_ESL_REF_DVER           19121   /* 部署目标共享内存版本[%s]和XML文件版本[%s]不一致 */
#define    E_ESL_REF_POVER          19122   /* XML文件版本[%s]比当前参数目标共享内存版本[%s]旧 */
#define    E_ESL_REF_DOVER          19123   /* XML文件版本[%s]与当前部署目标共享内存版本[%s]旧 */
#define    E_ESL_REF_PCVER          19124   /* XML文件版本[%s]与当前参数目标共享内存版本[%s]一致,不需要刷新 */
#define    E_ESL_REF_DCVER          19125   /* XML文件版本[%s]与当前部署目标共享内存版本[%s]一致,不需要刷新 */
#define    E_ESL_RM_DCUR            19126   /* 同步命令要求删除当前部署共享内存[%s],拒绝执行 */
#define    E_ESL_RM_PCUR            19127   /* 同步命令要求删除当前参数共享内存[%s],拒绝执行 */
#define    E_ESL_REF_WORK           19128   /* 因为刷新或同步,进程[%s]暂停相关操作 */
#define    E_ESL_SYNC_NEWMCH        19129   /* 同步时发现新机器[%s],请检查平台状态,建议重启平台 */
#define    E_ESL_ATASK_MCHSTAT      19130   /* 任务类型[%c]任务逻辑[%s]找不到机器节点[%s]的执行状态 */
#define    E_ESL_DTAM_DDTA_ATBN     19131   /* DTA[%s]实例[%d][%d]满足交易数自动重启条件,需要退出 */
#define    E_ESL_DTAM_DDTA_ATBM     19132   /* DTA[%s]实例[%d][%d]满足内存值自动重启条件,需要退出 */
#define    E_ESL_DTAM_DDTA_ATBT     19133   /* DTA[%s]实例[%d][%d]满足交易时间自动重启条件,需要退出 */
#define    E_ESL_DTAM_CLEAN_CC      19134   /* 清理和DTA[%s]关联的服务系统[%s]的动态并发数信息失败 */
#define    E_ESL_MODEX_NEXT         19135   /* 接到机器[%s]切换主控节点命令,主控节点由[%s]切换为[%s] */
#define    E_ESL_MASTER_DEL         19136   /* 原主控节点[%s]被删除,设置新主控节点为[%s] */
#define    E_ESL_MASTER_NOID        19137   /* 原主控节点[%s]不是预定的可做主控节点的节点,设置新主控节点为[%s] */
#define    E_ESL_SET_MASTER_OK      19138   /* 节点[%s]设置新主控节点[%s]成功 */
#define    E_ESL_SET_MASTER_FAIL    19139   /* 节点[%s]设置新主控节点[%s]失败 */
#define    E_ESL_SET_MASTER_CHG     19140   /* 收到节点[%s]命令将主控节点设置为[%s],原主控节点为[%s] */

/*
 * DRQ 模块         21000-21999
 */
#define E_DRQ_BUFINIT               21000  /* 初始化DRQ缓冲区失败 */
#define E_DRQ_GETPERM               21001  /* 获取DRQ的IPC-PERM信息失败 */
#define E_DRQ_SEMCREATE             21002  /* 创建信号量集ID[%d]失败 */
#define E_DRQ_SEMREMOVE             21003  /* 通过信号量集ID[%d]删除信号量集失败 */
#define E_DRQ_SEMLOCK               21004  /* 加锁信号量集ID[%d]队列[%s]信号ID[%d]失败 */
#define E_DRQ_SEMLOCK_TMOUT         21005  /* 加锁信号量集ID[%d]队列[%s]信号ID[%d]超时 */
#define E_DRQ_SEMUNLOCK             21006  /* 解锁信号量集ID[%d]队列[%s]信号ID[%d]失败 */
#define E_DRQ_SEMSETVAL             21007  /* 设置信号量集ID[%d]队列[%s]信号ID[%d]值[%d]失败 */
#define E_DRQ_OVER                  21008  /* DRQ创建队列数超过最大队列数[%d]定制 */
#define E_DRQ_CLEANINST             21009  /* DRQ清理DTA[%s]实例[%d]状态失败 */
#define E_DRQ_NOQNAME               21010  /* DRQ队列名称[%s]不存在 */
#define E_DRQ_GETDTA                21011  /* 按名称[%s]取DTA定制不存在 */
#define E_DRQ_BUFATTACH             21012  /* 按DTA名称[%s]连接DRQ缓冲区[%d]失败 */
#define E_DRQ_CREATQMSG             21013  /* 创建DTA[%s]的DRQ队列[%s]失败 */
#define E_DRQ_DELQMSG               21014  /* 删除DTA[%s]的DRQ队列[%s]失败 */
#define E_DRQ_UNKNOWN_QTYPE         21015  /* DTA[%s]的队列类型[%d]非法 */
#define E_DRQ_GETQID                21020  /* 按队列名称[%s]查队列描述符失败 */
#define E_DRQ_BUFALLOC              21030  /* 分配队列[%s]的[%d]大小缓冲区失败 */
#define E_DRQ_BIDERR                21031  /* 取出的队列[%s]的缓冲区句柄[%d]非法 */
#define E_DRQ_BUFREAD               21032  /* 取队列[%s]的缓冲区[%d]内容[%d]失败 */
#define E_DRQ_BUFWRITE              21033  /* 存队列[%s]的缓冲区[%d]内容[%d]失败 */
#define E_DRQ_MSQRECV_TMOUT         21034  /* 取队列[%s]的消息[%ld]超时 */
#define E_DRQ_MSQRECV_ERR           21035  /* 取队列[%s]的消息[%ld]失败 */
#define E_DRQ_MSQSEND_TMOUT         21036  /* 发送队列[%s]的消息[%ld]超时 */
#define E_DRQ_MSQSEND_ERR           21037  /* 发送队列[%s]的消息[%ld]失败 */
#define E_DRQ_SUBCVALUE             21038  /* 队列[%s]消息数减一失败 */
#define E_DRQ_ADDCVALUE             21039  /* 队列[%s]消息数加一失败 */
#define E_DRQ_SNDFILE               21040  /* 消息[%s]转发文件到机器[%s]失败 */
#define E_DRQ_SNDDATA               21041  /* 消息[%s]转发数据[%d]到机器[%s]失败 */
#define E_DRQ_DRQGET                21042  /* DRQ取消息[%s]失败 */
#define E_DRQ_DRQPUT                21043  /* DRQ存消息[%s]失败 */
#define	E_DRQ_STACK_PUSH            21100  /* 将服务[%s]的信息入栈失败 */
#define	E_DRQ_STACK_POP             21101  /* 将服务[%s]的信息出栈失败 */
#define E_DRQ_TRAN_GETBASE          21200  /* 服务[%s]登记交易信息时,取登记共享内存失败 */
#define E_DRQ_TRAN_LOCK             21201  /* 服务[%s]登记交易信息时,加锁失败 */
#define E_DRQ_TRAN_UNLOCK           21202  /* 服务[%s]登记交易信息时,超过交易并发[%d]参数 */
#define E_DRQ_TRANREG               21203  /* 队列[%s]登记服务[%s]信息失败 */
#define E_DRQ_DTARES_GET            21204  /* DRQ的[%s]DTA资源上锁失败 */
#define E_DRQ_DTARES_FREE           21205  /* DRQ的[%s]DTA资源解锁失败 */
#define E_DRQ_REG_MACHSVR           21210  /* 机器[%s]DRQ服务登记进程失败 */
#define E_DRQ_MACHSVR_RUN           21211  /* 机器[%s]DRQ服务启动 */
#define E_DRQ_MACHSVR_DOWN          21212  /* 机器[%s]DRQ服务退出 */
#define E_DRQ_FAILCLEAN             21213  /* 垃圾清理服务发现超时[%d]秒消息:-读出内容-->服务[%s]源机器[%s]源DTA[%s]目的机器[%s]目的DTA[%s]流水号[%ld]会话号[%s] */
#define E_DRQ_CLEANDTA_OK           21214  /* 清理DTA[%s]成功 */
#define E_DRQ_CLEANDTA_FAIL         21215  /* 清理DTA[%s]失败 */
#define E_DRQ_CLEANINST_OK          21216  /* DRQ清理DTA[%s]实例[%d]状态成功 */
#define E_DRQ_CLEANTRAN_OK          21217  /* DRQ清理DTA[%s]交易[%s]状态成功 */
#define E_DRQ_DRQ_LOCK              21218  /* DTA[%s]申请DRQ锁失败 */
#define E_DRQ_CLEAN_LOCK            21219  /* 垃圾清理服务发现[%d]秒死锁，解锁[%s]结果[%d]，进程[%d] */
#define E_DRQ_CLEAN_BUFF            21220  /* 垃圾清理服务发现BUFFER使用超过吃水线[%d%%]，强制释放[%d]个缓冲 */
#define E_DRQ_SVCPARL               21221  /* DTA[%s]服务优先级别[%hd]登记并发数失败 */
#define E_DRQ_RCYCL_SVCID           21222  /* DTA[%s]服务身份ID[%s]并发数[%d]持续[%d]秒>0被清零 */
#define E_DRQ_RCYCL_PAL             21223  /* 节点[%s]DTA[%s]并发数[%d]{%d %d %d %d %d}持续[%d]秒>0被清零 */
#define E_DRQ_RCYCL_CPAL            21224  /* DTA[%s]客户化并发[%s]并发数[%d]持续[%d]秒>0被清零 */
#define E_DRQ_SHM_LOCK              21225  /* 申请共享内存状态锁加锁失败 */
#define E_DRQ_INST_LOCK             21226  /* 申请实例状态锁加锁失败 */
#define E_DRQ_SYNC_LOCK             21227  /* 申请同步状态锁加锁失败 */
#define E_LOCK_RELOCK               21228  /* 同步锁[%s]重复加锁 */
#define E_SHM_RCYCL_INST            21229  /* 强制DTA[%s]实例[%d]进程号[%d]退出,因为其连接的共享内存版本已经被回收 */
#define E_DRQ_FULL                  21230  /* 创建DTA[%s]的队列失败,无空闲的DRQTBL */

/*
 * DTA module       22000-22999
 */
#define  E_DTAAPI_SHM_ATTACH     22000  /* DTA[%s]连接共享内存失败 */
#define  E_DTAAPI_EXIT_SIGNAL    22001  /* DTA[%s]因收到信号[%d]退出 */
#define  E_DTAAPI_DRQ_ATTACH     22002  /* DTA[%s]连接DRQ失败 */
#define  E_DTAAPI_EP_ALLOC       22003  /* DTA[%s]分配数据元素池 */
#define  E_DTAAPI_SLANG_INIT     22004  /* DTA[%s]的SLang初始化失败 */
#define  E_DTAAPI_DB_OPEN        22005  /* DTA[%s]使用密码[%s]/数据库名[%s]/服务名[%s]的参数打开数据库失败 */
#define  E_DTAAPI_STDOUT_CONV    22006  /* 切换标准输出至文件[%s]失败 */
#define  E_DTAAPI_STDERR_CONV    22007  /* 切换错误输出至文件[%s]失败 */
#define  E_DTAAPI_INST_INIT      22008  /* DTA[%s]的实例初始化失败 */
#define  E_DTAAPI_SEM_OPER       22009  /* 操作DTA[%s]信号量错 */
#define  E_DTAAPI_INST_FULL      22010  /* DTA[%s]启动实例数超过配置值[%d] */
#define  E_DTAAPI_OUT_DIRECT     22011  /* DTA[%s]标准/错误输出重定向失败 */
#define  E_DTAAPI_ROUTE_CALC     22012  /* 计算DTA[%s]的服务[%s]路由失败 */
#define  E_DTAAPI_EPTOBUF        22013  /* DTA[%s]上服务[%s]变量池转换失败 */
#define  E_DTAAPI_DRQPUT         22014  /* DTA[%s]上服务[%s]存DRQ失败 */
#define  E_DTAAPI_DRQGET         22015  /* DTA[%s]上服务[%s]取DRQ失败 */
#define  E_DTAAPI_MCHSESSION     22016  /* DTA[%s]上服务[%s]按表达式[%s]取会话号错 */
#define  E_DTAAPI_QSESSION       22017  /* DTA[%s]上服务[%s]按表达式[%s]取队列信息错 */
#define  E_DTAAPI_STACKPOP       22018  /* DTA[%s]上服务[%s]取队列栈错 */
#define  E_DTAAPI_ERRCONF        22019  /* DTA[%s]初始化日志配置失败 */
#define  E_DTAAPI_MAP            22020  /* 数据映射转换[%s]到[%s]失败 */
#define  E_DTAAPI_TIMEOUT        22021  /* DTA[%s]上服务[%s]处理超时 */
#define  E_DTAAPI_ACK_SESS       22022  /* 目的响应DTA[%s]根据表达式[%s]取会话号失败 */
#define  E_DTAAPI_AREV_INS       22030  /* 登记目的DTA[%s]服务[%s]流水号[%s]的冲正流水失败 */
#define  E_DTAAPI_AREV_UPT       22031  /* 更新目的DTA[%s]服务[%s]的冲正流水失败 */
#define  E_DTAAPI_AREV_DEL       22032  /* 删除目的DTA[%s]服务[%s]的冲正流水失败 */
#define  E_DTAAPI_AREV_ADDBACK   22033  /* 追加目的DTA[%s]服务[%s]的冲正流水失败 */
#define  E_DTAAPI_AREV_NOREC     22034  /* 目的DTA[%s]服务[%s]的冲正服务[%s]没有对应的服务定制 */
#define  E_DTAAPI_AREV_CTRL      22035  /* 切换自动冲正状态为[%s] */
#define  E_DTAAPI_AREV_PRO       22036  /* DTA[%s]的冲正服务[%s]冲正前处理失败 */
#define  E_DTAAPI_AREV_END       22037  /* DTA[%s]的冲正服务[%s]冲正结束判断失败 */
#define  E_DTAAPI_AREV_SETBACK   22038  /* DTA[%s]的服务[%s]记录冲正保存数据元失败 */
#define  E_DTAAPI_AREV_DBSERV    22039  /* DTA[%s]请求异步数据库服务DBSERV失败 */
#define  E_DTAAPI_DB_NAME        22040  /* DTA[%s]打开数据库的参数配置不存在 */
#define  E_DTAAPI_AREV_RECNUM    22041  /* DTA[%s]流水号[%s]登记冲正记录[%d]少了 */
#define  E_DTAAPI_SYS_ID         22042   /* 目的DTA[%s]节点[%s]子系统号[%c]与源DTA[%s]子系统号[%c]不符 */
#define  E_DTAAPI_NO_SYS         22043   /* 目的DTA[%s]没有与源DTA[%s]子系统号[%c]相符的部署方案 */
#define  E_DTASVC_TIMEOUT        22044   /* DTA[%s]服务[%s]剩余超时[%d]小于历史加权最小处理时间[%.3lf] */
#define  E_DTASVC_TMREQ          22045   /* DTA[%s]服务[%s]因时间不足,不做处理 */
#define  E_DTASVC_STOP           22046   /* 机器[%s]DTA[%s]服务[%s]已停止服务 */
#define  E_DTADEPSHM_REATTACH    22047   /* DTA[%s] 实例[%d] 当前共享内存版本[%s] 交易部署共享内存版本[%s] 重连接失败 */
#define  E_DTAPARSHM_REATTACH    22048   /* DTA[%s] 实例[%d] 当前共享内存版本[%s] 交易参数共享内存版本[%s] 重连接失败 */
#define  E_DTAINST_STARTED       22049   /* DTA[%s] 实例[%d] 已经启动，进程号为[%d] */
#define  E_DTAAPI_MCH_DB         22050   /* 打开机器[%s] DTA[%s]的数据库配置失败*/
#define  E_DTAAPI_MCH_SVCST      22051   /* 取机器[%s] DTA[%s]的服务[%s]状态失败*/
#define  E_DTAAPI_SHMREATACH    22052    /* DTA[%s] 实例[%d] 强制重连最新共享内存*/
#define  E_DTAAPI_AREV_SHM      22053  /* 自动冲正对应共享内存版本[DEP:%s PAR:%s]不存在,切换自动冲正[%s]状态为[%c] */

/*
 * 通用DTA                   23000-23999
 */
#define E_GDTA_EXPR        23000  /* DTA[%s]事件[%s]表达式计算错误 */
#define E_GDTA_SVC         23001  /* DTA[%s]服务[%s]处理失败 */
#define E_GDTA_EXPR_CALC   23002  /* 表达式计算返回FALSE,ep_id[%d],idx[%d],expr[%s] */
#define E_GDTA_EXPR_VTYPE  23003  /* 表达式[%s]的返回值类型[%d]错误 */

#define E_GDTA_FMTPAR      23004  /* 解析报文格式[%s]失败 */
#define E_GDTA_RECV        23005  /* DTA[%s]服务[%s]报文接收失败 */
#define E_GDTA_RECVPFMT    23006  /* DTA[%s]接收协议报文格式[%s]失败 */
#define E_GDTA_RECVFMT     23007  /* DTA[%s]服务[%s]接收报文格式[%s]失败 */
#define E_GDTA_RECVFMTG    23008  /* DTA[%s]服务[%s]接收报文组第[%d]项失败 */
#define E_GDTA_RECVFILE    23009  /* DTA[%s]服务[%s]接收文件失败 */
#define E_GDTA_PARSEFILE   23010  /* DTA[%s]服务[%s]解析文件失败 */

#define E_GDTA_FMTBLD      23011  /* 组织报文格式[%s]失败 */
#define E_GDTA_SEND        23012  /* DTA[%s]服务[%s]报文发送失败 */
#define E_GDTA_SENDPFMT    23013  /* DTA[%s]发送协议报文格式[%s]失败 */
#define E_GDTA_SENDFMT     23014  /* DTA[%s]服务[%s]发送报文格式[%s]失败 */
#define E_GDTA_SENDFMTG    23015  /* DTA[%s]服务[%s]发送报文组第[%d]项失败 */
#define E_GDTA_SENDFILE    23016  /* DTA[%s]服务[%s]发送文件失败 */
#define E_GDTA_BUILDFILE   23017  /* DTA[%s]服务[%s]组织文件失败 */

#define E_GDTA_SENDFG      23018  /* DTA[%s]服务[%s]发送报文组主报文[%s]失败 */
#define E_GDTA_SENDFGE     23019  /* DTA[%s]服务[%s]发送报文组辅报文[%s]失败 */
#define E_GDTA_CONV_SVC    23020  /* DTA[%s]服务[%s]进行服务转换失败 */
#define E_GDTA_CONV_ERR    23021  /* DTA[%s]服务[%s]进行错误码[%d]转换失败 */
#define E_GDTA_NODE_ADDR   23022  /* 取DTA[%s]服务[%s]节点[%s]的地址失败 */
#define E_GDTA_NODECONN    23023  /* DTA[%s]连接节点[%s]失败 */
#define E_GDTA_NOCONN      23024  /* 没有连接服务器的情况下试图进行报文发送或接收 */
#define E_GDTA_ADDR        23025  /* 非法地址[%s] */
#define E_GDTA_MCHQSVC     23026  /* 从DTA[%s]的匹配队列取服务名失败 */
#define E_GDTA_GETSVC      23027  /* 根据接收报文取DTA[%s]的服务名失败 */

#define E_GDTA_DTAINIT     23028  /* 根据名称[%s]执行DTA初始化失败 */
#define E_GDTA_DTAACALL    23029  /* DTA[%s]服务[%s]发送内部请求失败 */
#define E_GDTA_DTAGETR     23030  /* DTA[%s]服务[%s]接收内部响应失败 */
#define E_GDTA_DTASERV     23031  /* DTA[%s]接收内部请求失败 */
#define E_GDTA_DTARETURN   23032  /* DTA[%s]服务[%s]发送内部响应失败 */

#define E_GDTA_COMMINIT    23033  /* DTA[%s]通讯初始化失败[DTACommInit] */
#define E_GDTA_COMMTERM    23034  /* DTA[%s]通讯终止失败[DTACommTerm] */
#define E_GDTA_COMMRECV    23035  /* DTA[%s]接收数据失败[DTACommRecv] */
#define E_GDTA_COMMSEND    23036  /* DTA[%s]发送数据失败[DTACommSend] */
#define E_GDTA_COMMRFILE   23037  /* DTA[%s]接收文件失败[DTACommRecvFile] */
#define E_GDTA_COMMSFILE   23038  /* DTA[%s]发送文件失败[DTACommSendFile] */
#define E_GDTA_COMMCONNECT 23039  /* DTA[%s]连接地址[%s:%s]失败[DTACommConnect] */

#define E_GDTA_FGRECGET    23040  /* 根据表达式[%s]取报文组记录数失败 */
#define E_GDTA_FGRECPUT    23041  /* 根据数据元素[%s]存报文组记录数失败 */
#define E_GDTA_FGEFCOND    23042  /* 根据辅报文表达式[%s]计算条件失败 */
#define E_GDTA_FMTXFML     23043  /* 交易报文[%s]不能为类FML报文 */

#define E_GDTA_FILEPAR     23044  /* 解析文件格式[%s]失败 */
#define E_GDTA_FILEBLD     23045  /* 组织文件格式[%s]失败 */
#define E_GDTA_FILESEND    23046  /* 发送文件表达式[%s]处理失败 */
#define E_GDTA_FILERECV    23047  /* 接收文件表达式[%s]处理失败 */

#define E_GDTA_TUX_ARG     23048  /* 未指定TUXEDO程序[%s]的命令行参数 */
#define E_GDTA_TUX_CALL    23049  /* 同步请求TUXEDO服务[%s]失败[%s] */
#define E_GDTA_TUX_ACALL   23050  /* 发送TUXEDO请求[%s]失败[%s] */
#define E_GDTA_TUX_GETRPL  23051  /* 接受TUXEDO响应[%s]失败[%s] */
#define E_GDTA_TUX_ALLOC   23052  /* 分配长度为[%d]的TUXEDO报文失败 */
#define E_GDTA_TUX_BUFTYP  23053  /* TUXEDO服务[%s]接收到的报文类型不为[%s] */
#define E_GDTA_SVR_INIT    23054  /* DTA[%s]服务初始化[DTASvrInit]失败 */

#define E_GDTA_TCP_ARG     23055  /* 未指定TCP程序[%s]的参数 */
#define E_GDTA_TCP_SVR     23056  /* 服务型TCP-DTA[%s]启动失败 */
#define E_GDTA_TCP_INSTNUM 23057  /* 服务型TCP-DTA[%s]的实例数小于1 */
#define E_GDTA_TCP_SOCKET  23058  /* SOCKET描述符非法(小于0) */

#define E_GDTA_SERVICE     23059  /* DTA[%s]服务[DTAService]失败 */
#define E_GDTA_FUNNULL     23060  /* 通信接口[%s]未进行客户化 */

#define E_GDTA_APPERRNO    23061  /* 目的DTA返回源DTA[%s]上服务[%s]的应用返回码[%d]非法 */
#define E_GDTA_APP         23062  /* DTA[%s]服务[%s]目的方应用处理失败 */
#define E_GDTA_RECVSHORT   23063  /* 由于__GDTA_SHORT设成'0'短路，服务[%s]的通信接受直接返回 */
#define E_GDTA_SENDSHORT   23064  /* 由于__GDTA_SHORT设成'0'短路，服务[%s]的通信发送直接返回 */
#define E_GDTA_RFILESHORT  23065  /* 由于__GDTA_SHORT设成'0'短路，服务[%s]的文件接受直接返回 */
#define E_GDTA_SFILESHORT  23066  /* 由于__GDTA_SHORT设成'0'短路，服务[%s]的文件发送直接返回 */
#define E_GDTA_ACALLSHORT  23067  /* 由于__GDTA_SHORT设成'0'短路，服务[%s]的内部请求直接返回 */
#define E_GDTA_GREPLSHORT  23068  /* 由于__GDTA_SHORT设成'0'短路，服务[%s]的内部响应直接返回 */
#define E_GDTA_INITSHORT   23069  /* 由于__GDTA_SHORT设成'0'短路，DTA[%s]的通信初始化直接返回 */
#define E_GDTA_TERMSHORT   23070  /* 由于__GDTA_SHORT设成'0'短路，DTA[%s]的通信结束直接返回 */
#define E_GDTA_CONNECTSHORT 23071  /* 由于__GDTA_SHORT设成'0'短路，连接节点[%s:%s]直接返回 */
#define E_GDTA_CLOSESHORT  23072  /* 由于__GDTA_SHORT设成'0'短路，关闭节点[%s]连接直接返回 */
#define E_GDTA_PARSESHORT  23073  /* 由于__GDTA_SHORT设成'0'短路，服务[%s]的报文[%s]解析直接返回 */

#define  E_GDTA_TYPE       23074  /* error DTA type [%s] */   
#define  E_GDTA_SVCNAME    23075  /* the service name of DTA[%s]error [%s] */
#define  E_GDTA_TUXAGTNAME 23076  /* the svcname of TUXEDO dispatch pattern must include  'AGT_',[%s]*/
#define  E_GDTA_NAME       23077  /* error DTA name [%s] */
#define  E_GDTA_DISPSVCLEN 23078  /* the svcname of TUXEDO dispatch pattern can't exceed 45 characters  */
#define  E_TUXDTA_PARRELL  23079  /* TUXEDO 分发模式下DTA[%s] 代理当前并发数[%d] 超过最大并发数[%d],DTA并发数控制 */
#define  E_TUXCALL_ERROR   23080  /* TUXEDO函数调用[%s]失败:[%d][%s] */
#define  E_GDTA_CLRTIMEOUT 23081  /* DTA[%s]清理超时服务[%s],流水:[%s] [%s] */
#define  E_GDTA_TRANCLR    23082  /* DTA[%s]清理超时服务[%s],流水:%s,发送时间[%d],清理时间戳[%d],超时时间[%d] */
#define  E_TUXDTA_SVCERR   23083  /* DTA[%s]上的[%s]服务,源流水为[%s]处理失败,交易上下文信息将被清除 */
#define E_TUXDTA_RESMATCH  23084  /* DTA[%s]上接收数据长度[%d],TUXEDO应答标示[%d],找不到匹配的记录,可能交易已超时 */
#define E_TUXDTA_CONNERR   23085  /* TUXEDO异步DTA连接服务ip[%s]端口[%s]失败[%s] */
#define E_GDTA_CALLFAIL    23086  /* TCP源DTA[%s]子进程处理服务[%s],流水号[%s]失败 */
#define E_GDTA_ERRINST     23087  /* 不能找到TCP子进程[%d]对应的实例号 */
#define E_GDTA_REQ         23088  /* DTA[%s]服务[%s]目的方应用请求失败 */
#define E_GDTA_ACK         23089  /* DTA[%s]服务[%s]目的方应用接收失败 */
#define E_GDTA_BUS         23090  /* DTA[%s]服务[%s]目的方并发数超限 */

#define E_GDTA_DBBEGIN     23091  /* DTA[%s]服务[%s]开始数据库事务 */
#define E_GDTA_DBCOMMIT    23092  /* DTA[%s]服务[%s]提交数据库事务 */
#define E_GDTA_ASYNCLT     23093  /* 异步源DTA处理源流水为[%s]的交易失败 */
#define E_GDTA_MCHQSSID    23094  /* 错误MCHQ[%s]会话ID[%d] */
#define E_GDTA_HOSTCONN    23095  /* DTA[%s]连接节点[%s]失败  */
#define E_GDTA_SVCERR      23096  /* DTA[%s]服务[%s]源流水[%s]处理失败 */
#define E_GDTA_EXCEDCONN   23097  /* DTA[%s]连接节点[%s]超限[%d] */
#define E_GDTA_NODEIPCONN  23098  /* DTA[%s]连接节点[%s][%s:d] 失败 */
#define E_GDTA_CONNIDX     23099  /* DTA[%s]连接节点[%s]下标[%d]超限 */
#define E_GDTA_DELETRANREG 23100  /* 删除交易信息表错误,信息表标志[%d]交易会话[%d]与请求会话号不符[%d] */
#define E_GDTA_BALATTACH   23101  /* 取DTA[%s]流量控制参数错误 */
#define E_GDTA_PADDR       23102  /* 客户端地址[%s]不合法,拒绝连接 */
#define E_GDTA_PADDR_NUM   23103  /* 客户端地址[%s]连接数限制,拒绝连接 */
#define E_GDTA_PADDR_ONUM  23104  /* 客户端地址[%s]连接数达到配置数[%d],拒绝连接 */
#define E_GDTA_CONN_TMOUT  23105  /* 短连接[%d]在[%d]接入到[%d]未发送请求,连接被关闭 */
#define E_GDTA_CLTLINK_CHK 23106  /* 链路[%s:%hu]被强制清理 */
#define E_GDTA_DYNSVRCON_CURR  23107  /* 服务系统[%s],当前并发数为[%d],超过规则[%d] 定义的并发数[%d],动态并发数控制生效 */
#define E_GDTA_DYNSVCCON_CURR  23108  /* 服务系统[%s]服务[%s],当前并发数为[%d],超过规则[%d] 定义的并发[%d]数,动态并数发控制数生效 */


/* Max = 23104 */

/*
 * FLOW module               26000-26999
 */
#define	E_FLW_CTS_INIT     26000  /* 流程测试进程[%s]初始化失败 */
#define	E_FLW_CTS_LOG      26001  /* 流程测试进程设置ALA[%s]的日志标志失败 */
#define	E_FLW_CTS_CALL     26002  /* 流程测试进程请求ALA[%s]的交易[%s]失败 */
#define	E_FLW_CTS_SENDFILE 26003  /* 流程测试进程发送文件[%s]失败 */
#define E_FLW_TRAN_DEF     26100  /* 交易[%s]定制信息不存在 */
#define E_FLW_TRANDATA     26101  /* 交易[%s]流程无步骤定制信息 */
#define E_FLW_EXEC_FLO     26109  /* 步骤[%d]执行子流程[%s]失败 */
#define E_FLW_EXEC_EXP     26110  /* 步骤[%d]执行计算处理[%s]失败 */
#define E_FLW_EXEC_SQL     26111  /* 步骤[%d]执行数据库操作[%s]失败 */
#define E_FLW_EXEC_FIL     26112  /* 步骤[%d]执行文件操作[%s]失败 */
#define E_FLW_EXEC_RPT     26113  /* 步骤[%d]执行报表操作[%s]失败 */
#define E_FLW_EXEC_DTA     26114  /* 步骤[%d]执行外部通信[%s]失败[%d] */
#define E_FLW_EXEC_CMD     26115  /* 步骤[%d]执行命令行[%s]失败 */
#define E_FLW_EXEC_SWI     26116  /* 步骤[%d]执行开关处理[%s]失败 */
#define E_FLW_EXEC_PRE     26117  /* 步骤[%d]执行前处理失败 */
#define E_FLW_EXEC_SUF     26118  /* 步骤[%d]执行后处理失败 */
#define E_FLW_EXEC_ERR     26119  /* 步骤[%d]执行错误处理失败 */
#define E_FLW_DB_ELBEG     26120  /* 取数据库定制的数据元下标起始位置错 */
#define E_FLW_DB_DBBEG     26121  /* 取数据库定制的数据库记录起始位置错 */
#define E_FLW_DB_DBREC     26122  /* 取数据库定制的最大返回记录错 */
#define E_FLW_DB_DBCODE    26123  /* 存数据库定制的返回数据库错误码错 */
#define E_FLW_DB_DBRETURN  26124  /* 存数据库定制的返回记录数错 */
#define E_FLW_DB_INDTRAN   26125  /* 独立数据库操作的第[%d]条SQL语句[%s]拼装失败 */
#define E_FLW_DB_NOSELECT  26126  /* 独立数据库操作不能有SELECT操作 */
#define E_FLW_DB_TRAN      26127  /* 数据库操作的第[%d]条SQL语句[%s]拼装失败 */
#define E_FLW_DB_EXEC      26128  /* 数据库操作的第[%d]条SQL语句[%s]执行失败 */
#define E_FLW_DB_2EP       26129  /* 数据库操作的第[%d]条SQL语句[%s]输出变量池失败 */
#define E_FLW_DB_SELECT    26130  /* 数据库操作的第[%d]条SQL语句[%s]查询失败 */
#define E_FLW_FILE_TYPE    26140  /* 文件操作类型[%c]错误 */
#define E_FLW_DTA_TYPE     26150  /* 外部通信步骤类型[%c]错误 */
#define E_FLW_DTA_ASYN     26151  /* 外部通信异步匹配模式类型[%c]错误 */
#define E_FLW_CMD_DE       26160  /* 命令行参数定义数据元名称[%s]超长[%d] */
#define E_FLW_CMD_DEIDX    26161  /* 命令行参数定义数据元下标的格式[]不匹配 */
#define E_FLW_CMD_STRLEN   26162  /* 处理后命令行字符串超长,空间[%d]不够 */
#define E_FLW_SWI_TYPE     26170  /* 开关步骤[%d]的处理表达式[%s]返回类型不是整数 */
#define E_FLW_APP_ERROR    26200  /* 流程设置应用信息[%s][%s] */
#define E_FLW_DTA_APP      26201  /* DTA[%s]处理失败-应用错误 */
#define E_FLW_DTA_REQ      26202  /* DTA[%s]处理失败-发送请求失败 */
#define E_FLW_DTA_ACK      26203  /* DTA[%s]处理返回-接受响应失败 */
#define E_FLW_DTA_BUS      26204  /* DTA[%s]处理失败-并发数超限 */

#define E_ALA_EXPR         26800  /* 逻辑部件[%s]执行表达式[%s]失败 */
#define E_ALA_INIT         26801  /* 逻辑部件[%s]初始化失败 */
#define E_ALA_SERV         26802  /* 逻辑部件[%s]接受服务请求失败 */
#define E_ALA_RETURN       26803  /* 逻辑部件[%s]返回服务[%s]响应失败 */
#define E_ALA_TIMEOUT      26804  /* 逻辑部件[%s]服务[%s]处理超时 */
#define E_ALA_DBBEGIN      26805  /* 逻辑部件[%s]服务[%s]开始数据库事务 */
#define E_ALA_DBCOMMIT     26806  /* 逻辑部件[%s]服务[%s]提交数据库事务 */
#define E_ALA_CF           26807  /* 逻辑部件[%s]服务[%s]流程调度失败 */
#define E_ALA_INITEVENT    26810  /* 逻辑部件[%s]初始化事件执行失败 */
#define E_ALA_TERMEVENT    26811  /* 逻辑部件[%s]结束事件执行失败 */
#define E_ALA_SERVBEGIN    26812  /* 逻辑部件[%s]服务[%s]开始事件执行失败 */
#define E_ALA_SERVSUCC     26813  /* 逻辑部件[%s]服务[%s]成功结束事件执行失败 */
#define E_ALA_SERVFAIL     26814  /* 逻辑部件[%s]服务[%s]失败结束事件执行失败 */
#define E_ALA_AREV_UPT     26820  /* 逻辑部件[%s]服务[%s]成功修改冲正状态失败 */
#define E_ALA_AREV_DEL     26821  /* 逻辑部件[%s]服务[%s]失败冲正记录删除失败 */

#define E_TASK_NOINST      26900  /* 任务[%s]没有可用[%s]实例处理，等待下一次调度 */
#define E_TASK_EXEC        26901  /* 任务[%s]请求MCH[%s]ALA[%s]服务失败 */
#define E_TASK_NOSVC       26902  /* 任务[%s]对应服务逻辑没有定义，被停止 */
#define E_TASK_MNOINST     26903  /* 任务[%s]没有可用机器[%s]ALA[%s]实例处理，等待下一次调度 */
/*
 * 数据库封装模块         27000-27999
 */
#define E_DB_ERR	27000	/* 数据库操作错SQLCODE[%ld]ISAM[%ld]USE[%ld]DBMSG[%s:%s]USEMSG[%s] */
#define E_PREP_ID	27001	/* 没有PREPARE分析SQL语句 */
#define E_CURS_ID	27002	/* 打开游标前没有定义游标 */
#define E_DESC_ID       27003	/* 游标描述符错误 */
#define E_GETNULL	27004	/* 取SQL数据区返回空 */
#define E_PUTNULL	27005	/* 存SQL数据区返回空 */
#define E_PUTERROR	27006	/* 输入的SQL数据区序号大于查询条件的个数 */
#define E_CUR_NOOPEN	27007	/* 游标没有打开 */
#define E_CUR_NOALLOC	27008	/* 没有分配游标输入的SQL数据区 */
#define E_PUT_NOALLOC	27009	/* 没有分配条件输入的SQL数据区 */
#define E_EXE_NOALLOC	27010	/* 执行SQL语句前没有分配SQL数据区 */
#define E_PUT_NOMATCH	27011	/* 在打开游标或EXECUTE之前条件描述不完全 */
#define E_CUR_UNCLOSED	27012	/* 在未打开游标之前不能关闭游标 */
#define E_CUR_NOCLOSE	27013	/* 在未关闭游标之前不能释放游标 */
#define E_DESC_UNALLOCATED	27014	/* 在未分派描述空间之前不能释放描述空间 */
#define E_GETSERIAL_ERR	27015	/* 所要得到的列值超过了SQL语句已选出的范围 */
#define E_CUR_NOFETCH	27016	/* 获取数据之前没有FETCH游标 */
#define E_MULTI_PREP	27017	/* 重复PREPARE分析SQL语句 */
#define E_ILLLEGAL_TYPE	27018	/* 在调用程序中引用了非法的SQL数据类型 */
#define E_NODEF_TYPE	27019	/* 在程序中未对合法的SQL数据类型进行说明 */
#define E_DESC_NOOPEN	27020	/* 在程序中执行SqlDescribe之前没有打开游标 */
#define E_EXE_INTERIOR	27021	/* 在执行SqlExecute过程中出现了的内部错误 */
#define E_NOTFOUND	27022	/* 在执行SQL语句过程中未发现记录 */
#define E_NOAREA	27023	/* 执行sel,upd,del,insSQL语句前未分配工作区 */
#define E_DB_NOOPEN     27100   /* 没有打开数据库 */
#define E_MAXAREA       27101   /* 分配的工作区达到最大个数[%d] */

/* 
 * 报表日志       29000-29999
 */
#define E_RPT_EFMT      29000   /* 未知值格式[%s] */
#define E_RPT_FMT       29001   /* 值[%s]格式不符合格式[%s]要求 */
#define E_RPT_FILES     29002   /* 报表[%s]的[%s]节点文件取源定义错误 */
#define E_RPT_DBS       29003   /* 报表[%s]的[%s]节点数据库取源定义错误 */
#define E_RPT_TYPE      29004   /* 未知的报表类型[%d]  */
#define E_RPT_BOXTYPE   29005   /* 未知的边框类型[%s] */
#define E_RPT_CNTTYPE   29006   /* 单元格的类型[%d]与XML文件子节点不匹配 */
#define E_RPT_CURCODE   29007   /* 未知的币种类型[%s] */
#define E_RPT_MONEY     29008   /* 金额数字[%s]格式错 */
#define E_RPT_NUMBER    29009   /* 数字字符串[%s]有非法字符 */
#define E_RPT_CALCMETA  29010   /* 统计数据元个数[%d]与合计数据元个数[%d]不匹配 */
#define E_RPT_PRTTYPE   29011   /* 打印机类型为[%d],打印机控制参数不能为空 */
#define E_RPT_PAGEAREA  29012   /* 未知的页码范围[%s] */
#define E_RPT_DETYPE    29013   /* 未知的数据元素数据类型[%d] */
#define E_RPT_D_QUOTE   29014   /* 双引号不匹配[%s] */
#define E_RPT_S_QUOTE   29015   /* 单引号不匹配[%s] */
#define E_RPT_BRAKETS   29016   /* 括号不匹配[%s] */
#define E_RPT_EXPR_VTYPE    29017   /* 未知的表达式返回值类型[%d] */
#define E_RPT_HIGH      29018   /* 报表高度错误,报表高度[%d]小于固定行高[%d] */
#define E_RPT_GRP       29019   /* 分组数[%d]为零,不能打印 */
#define E_RPT_PAGENUM   29020   /* 报表页码[%d]为零,没有内容打印 */
#define E_RPT_PAGEREC   29021   /* 报表定义错,每页打印记录数[%d]太小 */
#define E_RPT_CUSTFUNC  29022   /* 调用客户化处理流程[%s]错误 */
#define E_RPT_NOCONV    29023   /* 数据[%s]不在转换表[%s]范围内 */

/*
 * 文件传输服务模块       30000-30999
 */
#define E_FLS_RUN       30001   /* 文件服务[%s]启动 */
#define E_FLS_DOWN      30002   /* 文件服务[%s]退出 */
#define E_FLS_FAIL      30003   /* 文件服务[%s]在IP[%s]端口[%s]上监听失败 */

/*
 * 调试日志       31000-31999
 */
#define E_TRC_PRSALA_DUMP    31001   /* 在目录[%s]下记录ALA[%s]的压力测试日志失败 */
#define E_TRC_PRSALA_IDX     31002   /* ALA[%s]压力测试日志空间的索引错 */
#define E_TRC_GET_OPERREC    31003   /* 取第[%d]号数据元素操作信息失败 */
#define E_TRC_GET_ELEMID     31004   /* 通过数据元素编号[%ld]取数据元素信息失败 */

/*
 * 新流程调试日志 31100-31199
 */
#define E_FLW_APPSERV        31100 /* [%d] DTAAppServ( %s, %s )结束 */
#define E_FLW_DTASVC         31101 /* [%d] 取DTA[%s]服务[%s]的部署信息 */
#define E_FLW_ALAPRE         31102 /* [%d] 执行ALA前处理 */
#define E_FLW_ALAFAIL        31103 /* [%d] 执行ALA失败处理 */
#define E_FLW_ALASUCC        31104 /* [%d] 执行ALA成功处理 */
#define E_FLW_ALADONE        31105 /* [%d] 执行ALA结束处理 */
#define E_FLW_BWORK          31106 /* [%d] 开始事务 */
#define E_FLW_CFBEGIN        31107 /* [%d] 服务[%s] CoreFlow开始 */
#define E_FLW_CFEND          31108 /* [%d] 服务[%s] CoreFlow结束 */
#define E_FLW_RWORK          31109 /* [%d] 回滚事务 */
#define E_FLW_CWORK          31110 /* [%d] 提交事务 */
#define E_FLW_REVUA          31111 /* [%d] 提交全部自动冲正信息 */
#define E_FLW_REVDA          31112 /* [%d] 删除全部自动冲正信息 */
#define E_FLW_RETURN         31113 /* [%d] DTAAppReturn( %s, %s, %d ) */
#define E_FLW_ALLEND         31114 /* [%d] 服务处理结束 */
#define E_FLW_CF_CFBEGIN     31115 /* [%d] 服务[%s]流程处理开始 */
#define E_FLW_CF_CFEND       31116 /* [%d] 服务[%s]流程处理结束 */
#define E_FLW_CF_CFPRE       31117 /* [%d] 步骤[%d]执行前处理结束 */
#define E_FLW_CF_CFDONE      31118 /* [%d] 步骤[%d]执行后处理结束 */
#define E_FLW_CF_CFFAIL      31119 /* [%d] 步骤[%d]执行失败处理结束 */
#define E_FLW_CFSUB_BEG      31120 /* [%d] 步骤[%d]子流程[%s]处理开始 */
#define E_FLW_CFSUB_END      31121 /* [%d] 步骤[%d]子流程[%s]处理结束 */
#define E_FLW_CFEXP_BEG      31122 /* [%d] 步骤[%d]计算部件处理开始 */
#define E_FLW_CFEXP_END      31123 /* [%d] 步骤[%d]计算部件处理结束 */
#define E_FLW_CFSQL_BEG      31124 /* [%d] 步骤[%d]数据库部件处理开始 */
#define E_FLW_CFSQL_END      31125 /* [%d] 步骤[%d]数据库部件处理结束 */
#define E_FLW_CFFIL_BEG      31126 /* [%d] 步骤[%d]文件部件处理开始 */
#define E_FLW_CFFIL_END      31127 /* [%d] 步骤[%d]文件部件处理结束 */
#define E_FLW_CFRPT_BEG      31128 /* [%d] 步骤[%d]报表部件处理开始 */
#define E_FLW_CFRPT_END      31129 /* [%d] 步骤[%d]报表部件处理结束 */
#define E_FLW_CFCMD_BEG      31130 /* [%d] 步骤[%d]命令部件处理开始 */
#define E_FLW_CFCMD_END      31131 /* [%d] 步骤[%d]命令部件处理结束 */
#define E_FLW_CFSWI_BEG      31132 /* [%d] 步骤[%d]分支部件处理开始 */
#define E_FLW_CFSWI_END      31133 /* [%d] 步骤[%d]分支部件处理结束 */
#define E_FLW_CFEXP_EXP      31134 /* [%d] 计算部件执行处理[%s] */
#define E_FLW_CFSQL_EXP      31135 /* [%d] 数据库部件执行处理[%s] */
#define E_FLW_CFFIL_BLD      31136 /* [%d] 文件部件执行格式[%s]组织处理 */
#define E_FLW_CFFIL_PAR      31137 /* [%d] 文件部件执行格式[%s]解析处理 */
#define E_FLW_CFRPT_EXP      31138 /* [%d] 报表部件执行格式[%s]处理 */
#define E_FLW_CFCMD_EXP      31139 /* [%d] 命令部件执行命令[%s][%s]处理 */
#define E_FLW_CFSWI_EXP      31140 /* [%d] 分支部件执行处理[%s],下一步骤[%d] */
#define E_FLW_CFEND_EXP      31141 /* [%d] 步骤[%d]结束部件设置错误码[%s]错误信息[%s] */ 
#define E_FLW_DTA_ACALL      31142 /* [%d] 异步调用DTA[%s]服务[%s] */
#define E_FLW_DTA_GETRPL     31143 /* [%d] 异步获得DTA[%s]会话号[%ld]返回信息 */
#define E_FLW_CFDTA_BEG      31144 /* [%d] 步骤[%d]通信部件处理开始 */
#define E_FLW_CFDTA_END      31145 /* [%d] 步骤[%d]通信部件处理结束 */
#define E_FLW_APPSVC         31146 /* [%d] DTAAppServ() 开始接收交易请求 */
#define E_FLW_DTA_CALL       31147 /* [%d] 调用DTA[%s]服务[%s]结束 */
#define E_FLW_CUST_PROC      31148 /* [%d] 步骤[%d]执行客户化处理[%s] */
#define E_FLW_ALAEVT         31149 /* [%d] 执行ALA客户化处理[%s] */
#define E_FLW_CFCPT_BEG      31150 /* [%d] 步骤[%d]元件部件处理开始 */
#define E_FLW_CFCPT_END      31151 /* [%d] 步骤[%d]元件部件处理结束 */
#define E_FLW_CFCPT_EXP      31152 /* [%d] 元件部件[%s]执行处理[%s] */

/* 注意32000以后已经被应用占用,不要再往后扩了 */
#endif
