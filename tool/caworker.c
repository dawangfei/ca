/* Copyright (c)  2017 Cmbc 
   All rights reserved

   THIS IS UNPUBLISHED PROPRIETARY
   SOURCE CODE OF dc Systems, Inc.
   The copyright notice above does not
   evidence any actual or intended
   publication of such source code.
 */
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <ctype.h>
#include <locale.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "log.h"
#include "aux.h"
#include "caapi.h"

#include "../src/type.h"
#include "../src/list.h"
#include "../src/nt.h"

#if STARRING_INTEGRATE
#include <libxml/parser.h>
#endif
#include "sysloc.h"
#include "pladb.h"

#define DC_SYNC_STATUS_INIT         0
#define DC_SYNC_STATUS_CONNECTING   1
#define DC_SYNC_STATUS_READY        2
#define DC_SYNC_STATUS_BROKEN       3


typedef struct _peer
{
    int  sid;
    int  status;        /* 0: init; 1: connecting; 2: ready; 3: broken */

    char ip[16];
    char port[6];

    Link link;
} peer_t;


typedef struct _addr
{
    char ip[16];
    char port[6];
} addr_t;

#define DC_SYNC_MAX_HOST    32
addr_t g_addr_list[DC_SYNC_MAX_HOST];

static List  *g_passive_list    = NULL;
static int    g_listen_sid = -1;

static char g_user[100] = {0};
static char g_config_file[1024] = {0};
static char g_listen_port[10] = {0};

static int maininit(int argc, char *argv[]);

static int work(char *_port);

static int get_db_connect(char* db_name);

extern int dc_cfg_get_value(const char *section,
        const char *entry,
        const char *filename,
        char       *value,
        int         size);
extern long get_time();
extern long dc_get_time_of_seconds();
extern char *dctime_format_micro(long _us, char *_fmt, int _size);
extern void bcl_set_log_file(char *_filename);

/**
 *
 *
 */
static int maininit(int argc, char *argv[])
{
    int rv = 0;
    char *ptr = NULL;

    if (getlogin_r(g_user, sizeof(g_user))) 
    {
        fprintf(stderr, "error: getlogin_r failure!\n");
        return -1;
    }
#if DC_DEBUG
    fprintf(stdout, "user: [%s]\n", g_user);
#endif

#if STARRING_INTEGRATE
    ptr = getenv("FAPWORKDIR");
    if (ptr == NULL)
    {
        fprintf(stderr, "error: please set environment: export FAPWORKDIR=/path/\n");
        return -1;
    }
    snprintf(g_config_file, sizeof(g_config_file), "%s/etc/cache.cfg", ptr);
#else
    ptr = getenv("DC_CACHE_CONFIG");
    if (ptr == NULL)
    {
        fprintf(stderr, "error: please set environment: export DC_CACHE_CONFIG=/path/a.txt\n");
        return -1;
    }
    snprintf(g_config_file, sizeof(g_config_file), "%s", ptr);
#endif
    fprintf(stdout, "config: [%s]\n", g_config_file);


    if (argc == 2) 
    {
        snprintf(g_listen_port, sizeof(g_listen_port), "%s", argv[1]);
        fprintf(stdout, "listen port from command line: [%s]\n", g_listen_port);
    }
    else
    {
        rv = dc_cfg_get_value("HOST", "PORT", g_config_file, g_listen_port, sizeof(g_listen_port));
        if (rv < 0)
        {
            elog("error: dc_cfg_get_value: HOST:PORT from %s", g_config_file);
            return -1;
        }
        fprintf(stdout, "listen port from config file: [%s]\n", g_listen_port);
    }

    if (g_listen_port[0] == 0 || g_listen_port[0] == ' ')
    {
        fprintf(stderr, "error: invalid port: [%s]", g_listen_port);
        return -1;
    }

    /* as server, other connect to me */
    g_passive_list = create_list();
    if (g_passive_list == NULL)
    {
        elog("error: create_list2 failure");
        return -1;
    }
#if DC_DEBUG
    olog("passive list created");
#endif

    return rv;
}


int boot_udp_server(unsigned short _port)
{
    int rv = 0;
    int sid = 0;

    struct sockaddr_in server;

    sid = socket(AF_INET, SOCK_DGRAM, 0);
    if (sid < 0)
    {
        elog("error: socket failure");
        return -1;
    }
    olog("DGRAM sid: %d", sid);

    memset(&server, 0, sizeof(server));
    server.sin_family = AF_INET;
    server.sin_port   = htons(_port);
    server.sin_addr.s_addr = htonl(INADDR_ANY);

    rv = bind(sid, (struct sockaddr *)&server, sizeof(server));
    if (rv < 0)
    {
        elog("error: bind: %d failure", sid);
        return -1;
    }

    return 0;
}


int send_message(char *_ip, unsigned short _port, char *_message, int _len)
{
    int rv = 0;
    int sid = 0;
    int flag = 0;

    struct sockaddr_in target;
    socklen_t addrlen;

    sid = socket(AF_INET, SOCK_DGRAM, 0);
    if (sid < 0)
    {
        elog("error: socket failure");
        return -1;
    }
    olog("DGRAM sid: %d", sid);

    memset(&target, 0, sizeof(target));
    target.sin_family = AF_INET;
    target.sin_port   = htons(_port);
    inet_pton(AF_INET, _ip, &target.sin_addr);

    addrlen = sizeof(target);
    rv = sendto(sid, _message, _len, flag, (struct sockaddr *)&target, addrlen);

    olog("send: %d", rv);

    return rv;
}


void dc_worker_dump(List *_list, char *_label)
{
    int  n = 0;
    peer_t *p = NULL;

    list_for_each_entry(p, _list->head, link)
    {
        olog("[%s][%d] --- [%d, %d, %s, %s]", _label, n++, p->sid, p->status, p->ip, p->port);
    }

    olog("[%s] list-number: %d", _label, n);
}


int dc_worker_delete(List *_list, int _sid)
{
    int  rv = -1;
    peer_t *p = NULL;
    peer_t *q = NULL;

    p = list_entry(_list->head->next, DC_TYPEOF(*p), link);

    while (&p->link != _list->head)
    {
        q = list_entry(p->link.next, DC_TYPEOF(*p), link);

        olog("sid[%d]: [%d], [%s, %s]", p->sid, p->status, p->ip, p->port);

        if (_sid == p->sid)
        {
            list_del(&p->link);
            olog("find [%d], delete it!", p->sid);
            rv = 0;
            free(p);
        }

        p = q;
    } /* while */

    return rv;
}


/* clear the broken socket from list */
int dc_worker_clear(List *_list)
{
    int  n = 0;
    peer_t *p = NULL;
    peer_t *q = NULL;

    p = list_entry(_list->head->next, DC_TYPEOF(*p), link);

    while (&p->link != _list->head)
    {
        q = list_entry(p->link.next, DC_TYPEOF(*p), link);


        if (p->status == DC_SYNC_STATUS_BROKEN)
        {
            olog("find broken socket [%d][%s:%s]", p->sid, p->ip, p->port);

            if (p->sid >= 0)
            {
                dc_net_closesock(p->sid);
                olog("close socket [%d][%s:%s], bye!", p->sid, p->ip, p->port);
            }

            list_del(&p->link);
            free(p);
            n++;
        }
        else
        {
#if DC_DEBUG
            olog("can't-clear: sid[%d], status[%d], [%s, %s]", p->sid, p->status, p->ip, p->port);
#endif
        }

        p = q;
    } /* while */

    return n;
}


int add_to_container(List *_list, int _sid, int _status, char *_ip, char *_port)
{
    peer_t *p = NULL;

    p = calloc(1, sizeof(peer_t));
    if (p == NULL)
    {
        elog("error: calloc");
        return -1;
    }

    p->sid = _sid;
    p->status = _status;
    snprintf(p->ip, sizeof(p->ip), "%s", _ip);
    snprintf(p->port, sizeof(p->port), "%s", _port);

    list_add(_list, &p->link);

    return 0;
}




#if STARRING_INTEGRATE
/* PE-starring mode */
static int get_db_connect(char* db_name)
{
    char        filename[255];
    xmlDocPtr   xml;
    xmlNodePtr  node_ptr;
    xmlNodePtr  root_ptr;
    int         ret;

    node_ptr = NULL;
    root_ptr = NULL;

    memset (filename, 0x00, sizeof(filename));
    sprintf (filename, "%s/etc/ESAdmin.xml", (char*)getenv("FAPWORKDIR"));

    xml = xmlParseFile (filename);
    if (xml == NULL)
    {
         fprintf (stderr, "打开XML文件【%s】失败\n", filename);

         return -1;
    }

    root_ptr = xmlDocGetRootElement (xml);
    if (root_ptr == NULL)
    {
         fprintf (stderr, "error: xmlDocGetRootElement\n");
         xmlFreeDoc (xml);

         return -1;
    }

    /* printf ("root_ptr -> name:%s\n", root_ptr -> name); */
    /* printf ("root_ptr -> content:%s\n", root_ptr -> content); */

    node_ptr = root_ptr -> children;

    while (1)
    {
         if (node_ptr == NULL)
         {
              fprintf (stderr, "error: node_ptr\n");
              xmlFreeDoc (xml);

              return -1;
         }

         if (strcmp ((char*)node_ptr -> name, "DataBaseTab") == 0 )
         {
              node_ptr = node_ptr -> children;
              while (1)
              {
                   if (node_ptr == NULL)
                   {
                        fprintf (stderr, "error: [%s]\n",db_name);
                        xmlFreeDoc (xml);

                        return -1;
                   }
                   if (strcmp ((char*)node_ptr -> name,"DataBase") == 0)
                   {
                        if (strcmp ((char*)xmlGetProp(node_ptr,(const unsigned char*)"DBName"), db_name) == 0)
                        {
                             ret=DBOpen( 3, db_name, (char*)xmlGetProp(node_ptr,(const unsigned char*)"DBUserName"), (char*)xmlGetProp(node_ptr,(const unsigned char*)"DBPassword") );
                             if (ret == -1)
                             {
                                  fprintf (stderr, "error: DBOpen\n");
                                  xmlFreeDoc (xml);

                                  return -1;
                             }

                             printf ("connect to db [%s]\n",db_name);

                             break;
                        }
                   }
                   node_ptr = node_ptr -> next;
              }

              break;
         }

         node_ptr = node_ptr -> next;
    }

    xmlFreeDoc (xml);

    return 0;
}
#else
/* self-mode */
static int get_db_connect(char* _db_name)
{
    int  rv = 0;
    char section[64] = {0};
    char entry[64] = {0};
    char username[64] = {0};
    char password[64] = {0};

    /* get username */
    strcpy(section, "DB");
    strcpy(entry, "USERNAME");
    rv = dc_cfg_get_value(section, entry, g_config_file, username, sizeof(username));
    if (rv < 0)
    {
        elog("error: dc_cfg_get_value: %s:%s", section, entry);
        return -1;
    }

    /* get password */
    strcpy(section, "DB");
    strcpy(entry, "PASSWORD");
    rv = dc_cfg_get_value(section, entry, g_config_file, password, sizeof(password));
    if (rv < 0)
    {
        elog("error: dc_cfg_get_value: %s:%s", section, entry);
        return -1;
    }

    olog("username[%s], password[%s]", username, password);
    rv = DBOpen( 3, _db_name, username, password);
    if (rv == -1)
    {
        elog("error: DBOpen: %s, %s, %s", _db_name, username, password);
        return -1;
    }

    olog("connect to db [%s]", _db_name);

    return 0;
}
#endif




/* */
int dc_worker_job(char *_input, char *_output, int _size, int *_len)
{
    int rv = 0;
    int has_attached  = 0;
    int with_database = 0;
    long shm_key = 0;
    long curr_time = 0;
    char section[64] = {0};
    char entry[64] = {0};
    char database[64] = {0};
    char buffer[64] = {0};
    dc_cache_api_svc_t *p = NULL;
    dc_cache_api_svc_t *q = NULL;

    p = (dc_cache_api_svc_t *)_input;
    q = (dc_cache_api_svc_t *)_output;

    *_len = sizeof(dc_cache_api_svc_t);

    olog("===code:  [%s]", p->code);
    olog("===table: [%s]", p->table);
    olog("===time:  [%s]", p->timestamp0);


    memcpy(q, p, _size);

    curr_time = get_time();
    dctime_format_micro(curr_time, q->timestamp1, sizeof(q->timestamp1));

    /* get IPC key*/
    strcpy(section, "IPC");
    strcpy(entry, "SHM_KEY");
    rv = dc_cfg_get_value(section, entry, g_config_file, buffer, sizeof(buffer));
    if (rv < 0)
    {
        elog("error: dc_cfg_get_value: %s:%s", section, entry);
        return -1;
    }
    shm_key = atol(buffer);
    olog("shm-key: %ld", shm_key);


    if (strcmp(p->code, DC_CODE_RELOAD) == 0)
    {
        olog("task: reload");

        /* get database name */
        strcpy(section, "DB");
        strcpy(entry, "DATABASE");
        rv = dc_cfg_get_value(section, entry, g_config_file, database, sizeof(database));
        if (rv < 0)
        {
            elog("error: dc_cfg_get_value: %s:%s", section, entry);
            return -1;
        }
        olog("database: %s", database);

        /* connect to cache */
        if (dc_cache_api_open(shm_key))
        {
            elog("error: dc_cache_api_open");
            snprintf(q->resp, sizeof(q->resp), "%s", "96");
            snprintf(q->message, sizeof(q->message), "%s", "error: connect-db");
            rv = 1;
            goto _end;
        }
        has_attached = 1;
        olog("attache to SHM succeeds");

        /* connect to database */
        if (get_db_connect(database) == -1)
        {
            elog("error: get_db_connect: %s", database);
            snprintf(q->resp, sizeof(q->resp), "%s", "96");
            snprintf(q->message, sizeof(q->message), "%s", "error: connect-db");
            rv = 1;
            goto _end;
        }
        with_database = 1;
        olog("connect DB succeeds");

        /* execute reload */
        if (dc_cache_api_reload(p->table))
        {
            elog("error: dc_cache_api_reload: %s", p->table);
            snprintf(q->resp, sizeof(q->resp), "%s", "96");
            snprintf(q->message, sizeof(q->message), "%s", "error: reload failure");
            rv = 1;
            goto _end;
        }
        olog("reload succeeds");

        snprintf(q->resp, sizeof(q->resp), "%s", "00");
        snprintf(q->message, sizeof(q->message), "%s", "reload succeeds");
    }
    else if (strcmp(p->code, DC_CODE_ENABLE) == 0)
    {
        olog("enable--job");

        /* connect to cache */
        if (dc_cache_api_open(shm_key))
        {
            elog("error: dc_cache_api_open");
            snprintf(q->resp, sizeof(q->resp), "%s", "96");
            snprintf(q->message, sizeof(q->message), "%s", "error: connect-db");
            rv = 1;
            goto _end;
        }
        has_attached = 1;
        olog("attache to SHM succeeds");

        /* execute enable */
        rv = dc_cache_api_set_mode(1);
        if (rv)
        {
            elog("error: dc_cache_api_set_mode1: %s", p->table);
            snprintf(q->resp, sizeof(q->resp), "%s", "96");
            snprintf(q->message, sizeof(q->message), "%s", "error: enable failure");
            rv = 1;
            goto _end;
        }
        olog("cache mode enabled");

        snprintf(q->resp, sizeof(q->resp), "%s", "00");
        snprintf(q->message, sizeof(q->message), "%s", "enable succeeds");
    }
    else if (strcmp(p->code, DC_CODE_DISABLE) == 0)
    {
        olog("disable--job");

        /* connect to cache */
        if (dc_cache_api_open(shm_key))
        {
            elog("error: dc_cache_api_open");
            snprintf(q->resp, sizeof(q->resp), "%s", "96");
            snprintf(q->message, sizeof(q->message), "%s", "error: connect-db");
            rv = 1;
            goto _end;
        }
        has_attached = 1;
        olog("attache to SHM succeeds");

        /* execute disable */
        rv = dc_cache_api_set_mode(2);
        if (rv)
        {
            elog("error: dc_cache_api_set_mode2: %s", p->table);
            snprintf(q->resp, sizeof(q->resp), "%s", "96");
            snprintf(q->message, sizeof(q->message), "%s", "error: disable failure");
            rv = 1;
            goto _end;
        }
        olog("cache mode disabled");

        snprintf(q->resp, sizeof(q->resp), "%s", "00");
        snprintf(q->message, sizeof(q->message), "%s", "disable succeeds");
    }
    else
    {
        olog("error: invalid job: %s", p->code);
        snprintf(q->resp, sizeof(q->resp), "%s", "96");
        snprintf(q->message, sizeof(q->message), "%s", "unknown command");
        rv = -1;
    }

_end:


    /* disconnect from database */
    if (with_database)
    {
        if (DBClose() == -1)
        {
            elog("error: DBClose");
        }
    }

    if (has_attached)
    {
        if (dc_cache_api_close())
        {
            elog("error: dc_cache_api_close");
        }
    }

    curr_time = get_time();
    dctime_format_micro(curr_time, q->timestamp2, sizeof(q->timestamp2));

    return rv;
}


/* receive, do, send */
int dc_worker_task(int _sid)
{
    int    rv  = 0;
    int    cnt = 0;
    int    len = 0;
    char   input[1024] = {0};
    char   output[1024] = {0};

    len = sizeof(input) - 1;

#if DC_DEBUG
    olog("to receive bytes: %d", len);
#endif

    cnt = recv(_sid, input, len, 0);
    if (cnt < 0)
    {
        elog("error: recv(): %d", errno);
        return -1;
    }
    else if (cnt == 0)
    {
        olog("the other endpoint has closed");
        return 1;
    }
    olog("[%d] bytes received", cnt);

    /* do some job */
    rv = dc_worker_job(input, output, sizeof(output), &cnt);
    if (rv < 0)
    {
        elog("error: dc_worker_job failure");
        return -1;
    }
    else if (rv)
    {
        elog("warn: error: dc_worker_job not succeeds ");
    }

    olog("output size: [%d]", cnt);

    if ( (cnt=send(_sid, output, cnt, 0)) <= 0)
    {
        elog("error: send(): %d", errno);
        return -1;
    }
    olog("[%d] bytes sent out", cnt);

    olog("tasker succeeds");

    return 0;
}


int dc_worker_event(int _tmout)
{
    int n  = 0;
    int k  = 0;
    int rv = 0;
    int max_fd = 0;
    int fd_num = 0;
    int status = 0;
    int client_sid = -1;
    long begin = 0;

    unsigned int peer_addr = 0; 
    unsigned short peer_port = 0; 

    char ip[16] = {0};
    char port[6]= {0};

    peer_t *p = NULL;
    fd_set read_fd;
    struct timeval tmout;

#if DC_DEBUG
    olog("let's check I/O events");
#endif

    while (1)
    {

        /* clear broken socket */
        k = dc_worker_clear(g_passive_list);
#if DC_DEBUG
        if (k > 0)
        {
            olog("%d passive socket cleared", k);
        }
        olog("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
#endif

        /* set fd_set */
        {
            FD_ZERO( &read_fd );
            fd_num = 0;

            /* listen socket */
            if (g_listen_sid < 0)
            {
                elog("error: invalid listen socket: %d", g_listen_sid);
                return -1;
            }
            else
            {
                FD_SET(g_listen_sid, &read_fd );
                max_fd = g_listen_sid;
                fd_num += 1;
#if DC_DEBUG
                olog("to monitor listen sid: %d", g_listen_sid);
#endif
            }

            /* server */
            list_for_each_entry(p, g_passive_list->head, link)
            {
                if (p->sid >= 0)
                {
#if DC_DEBUG
                    olog("to monitor passive sid: %d", p->sid);
#endif
                    FD_SET( p->sid, &read_fd );
                    fd_num += 1;

                    if (p->sid > max_fd)
                    {
#if DC_DEBUG
                        olog("---refreshes: %d --> %d", max_fd, p->sid);
#endif
                        max_fd = p->sid;
                    }
                }
                else
                {
                    olog("passive socket: %s:%s already broken: %d", p->ip, p->port, p->sid);
                }
            }

#if DC_DEBUG
            olog("max-fd: %d", max_fd);
#endif
        } /* fd_set */

        tmout.tv_sec = _tmout;
        tmout.tv_usec = 0;

        n = dc_net_select( max_fd+1, &read_fd, 0, 0, &tmout );
        switch(n)
        {
            case -1:
                if (errno == EINTR)
                {
                    elog("warn: interrupted, but continue");
                    continue;
                }
                else
                {
                    elog("error: select: %d, %s", errno, strerror(errno));
                    return -1;
                }
            case 0:
                /* time out */
#if DC_DEBUG
                olog("waked, has time out");
#endif
                break;
            default:
#if DC_DEBUG
                olog("nice, %d fd is avaiable", n);
#endif
                break;
        }

        if (n > 0)
        {
            /* case: check is server socket */
            p = NULL;
            list_for_each_entry(p, g_passive_list->head, link)
            {
                if ( FD_ISSET( p->sid , &read_fd ) )
                {
                    olog("passive socket-IN  --- [%d, %d, %s:%s]", p->sid, p->status, p->ip, p->port);
                    if ( (rv = dc_net_getsockerr( p->sid)) == 0 )
                    {
                        /* do-task */
                        olog("passive socket [%d] do task", p->sid);
                        begin = get_time();
                        rv = dc_worker_task(p->sid);
                        if (rv)
                        {
                            if (rv < 0)
                            {
                                olog("job fails, cost %ld us", get_time()-begin);
                                elog("error: dc_worker_task");
                            }
                            p->status = DC_SYNC_STATUS_BROKEN;
                        }
                        else
                        {
                            olog("job succeeds, cost %ld us", get_time()-begin);
                            olog("nice: execute task succeeds");
                        }
                    }
                    else
                    {
                        /* socket broken */
                        olog("warn: passive socket [%d] broken: %d", p->sid, rv);

                        /* mark the socket as discared */
                        p->status = DC_SYNC_STATUS_BROKEN;
                    }
                }
                else
                {
#if DC_DEBUG
                    olog("passive socket-NOT --- [%d, %d, %s, %s]", p->sid, p->status, p->ip, p->port);
#endif
                }
            }

            /* case: check is listen socket */
            if (FD_ISSET(g_listen_sid, &read_fd))
            {
#if DC_DEBUG
                olog("listen socket is readable");
#endif

                client_sid = dc_net_tcp_accept(g_listen_sid, &peer_addr, &peer_port);
                if (client_sid < 0)
                {
                    elog("error: dc_net_tcp_accept");
                    return -1;
                }

                /* as server */
                status = DC_SYNC_STATUS_READY;
                dc_net_str_ip(peer_addr, ip, sizeof(ip));
                snprintf(port, sizeof(port), "%d", peer_port);
                olog("++++++++++++++++++++++++++++++++++++++++++++++++++");
                olog("new client arrives: %d -- %s:%s", client_sid, ip, port);

                rv = add_to_container(g_passive_list, client_sid, status, ip, port);
                if (rv)
                {
                    elog("error: add_to_container: passive");
                    return -1;
                }
                else
                {
#if DC_DEBUG
                    olog("add to passive container: sid:%d, status:%d, %s:%s", client_sid, status, ip, port);
#endif
                }
            }
        }

        break;
    } /* while */

#if DC_DEBUG
    olog("function {%s} ends", __func__);
#endif

    return 0;
} /* dc_worker_event */


static int work(char *_port)
{
    int  rv = 0;
    long curr_time = 0;
    long last_time = 0;
    long diff_time = 0;

    if (_port == NULL || _port[0] == 0)
    {
        elog("error: invalid parameter");
        return -1;
    }

    /* create listen socket */
    g_listen_sid = dc_net_simple_listen(_port);
    if (g_listen_sid < 0)
    {
        elog("error: dc_net_simple_listen: %s", _port);
        return -1;
    }
#if DC_DEBUG
    olog("nice: listen socket is %d", g_listen_sid);
#endif

    last_time = dc_get_time_of_seconds();
    while (1)
    {
        curr_time = dc_get_time_of_seconds();
        diff_time = curr_time - last_time;
        if (diff_time > 600)
        {
            last_time = curr_time;
            olog("##################################################");
        }

#if DC_DEBUG
        olog("-----------------------------");
#endif

        /* check event */
        rv = dc_worker_event(5);
        if (rv < 0)
        {
            elog("error: dc_worker_event");
        }
        else if (rv)
        {
            olog("warn: dc_worker_event");
        }
        else
        {
#if DC_DEBUG
            olog("nice: monitor event succeeds");
#endif
        }

#if DC_DEBUG
        dc_worker_dump(g_passive_list, "passive2");
#endif

    } /* while */


    olog("work done");

    return 0;
}


int main(int argc, char *argv[])
{
    bcl_set_log_file("caworker.log");

    if (maininit(argc, argv))
    {
        elog("error: maininit");
        return -1;
    }

#if DC_DEBUG
    olog("run!");
#endif

    if (work(g_listen_port))
    {
        elog("error: work");
        return -1;
    }

#if DC_DEBUG
    olog("done");
#endif

    return 0;
}


/* caworker.c */
