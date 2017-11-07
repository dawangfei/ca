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
#include <errno.h>

#include "log.h"
#include "aux.h"
#include "caapi.h"

#include "../src/type.h"
#include "../src/list.h"
#include "../src/nt.h"


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

static int maininit();

static int work(char *_port);

/**
 *
 *
 */
static int maininit()
{
    int rv = 0;

    if (getlogin_r(g_user, sizeof(g_user))) 
    {
        fprintf(stderr, "error: getlogin_r failure!\n");
        return -1;
    }
#if DC_DEBUG
    fprintf(stdout, "user: [%s]\n", g_user);
#endif


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


void dc_listen_dump(List *_list, char *_label)
{
    int  n = 0;
    peer_t *p = NULL;

    list_for_each_entry(p, _list->head, link)
    {
        olog("[%s][%d] --- [%d, %d, %s, %s]", _label, n++, p->sid, p->status, p->ip, p->port);
    }

    olog("[%s] list-number: %d", _label, n);
}


int dc_listen_delete(List *_list, int _sid)
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
int dc_listen_clear(List *_list)
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
            olog("find broken socket [%d][%s:%s]!", p->sid, p->ip, p->port);

            if (p->sid >= 0)
            {
                dc_net_closesock(p->sid);
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


/* TODO */
int dc_listen_task(int _sid)
{
    int    cnt = 0;
    int    len = 0;
    char   buffer[1024] = {0};

    len = sizeof(buffer) - 1;

#if DC_DEBUG
    olog("to receive bytes: %d", len);
#endif

    if ((cnt=recv(_sid, buffer, len, 0)) <= 0)
    {
        elog("error: recv(): %d", errno);
        return -1;
    }
    olog("[%d] bytes received", cnt);

    olog("[%s]", buffer);

    /* TODO: do some job */

    if ( (cnt=send(_sid, buffer, cnt, 0)) <= 0)
    {
        elog("error: send(): %d", errno);
        return -1;
    }
    olog("[%d] bytes sent out", cnt);
    olog("[%s]", buffer);

    olog("tasker succeeds");

    return 0;
}


int dc_listen_event(int _tmout)
{
    int n  = 0;
    int k  = 0;
    int rv = 0;
    int max_fd = 0;
    int fd_num = 0;
    int status = 0;
    int client_sid = -1;

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
        k = dc_listen_clear(g_passive_list);
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
                        rv = dc_listen_task(p->sid);
                        if (rv)
                        {
                            elog("error: dc_listen_task");
                            p->status = DC_SYNC_STATUS_BROKEN;
                        }
                        else
                        {
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
} /* dc_listen_event */


static int work(char *_port)
{
    int  rv = 0;

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

    while (1)
    {
#if DC_DEBUG
        olog("-----------------------------");
#endif

        /* check event */
        rv = dc_listen_event(5);
        if (rv < 0)
        {
            elog("error: dc_listen_event");
        }
        else if (rv)
        {
            olog("warn: dc_listen_event");
        }
        else
        {
#if DC_DEBUG
            olog("nice: monitor event succeeds");
#endif
        }

#if DC_DEBUG
        dc_listen_dump(g_passive_list, "passive2");
#endif

    } /* while */


    olog("work done");

    return 0;
}


int main(int argc, char *argv[])
{

    if (argc != 2)
    {
        fprintf(stdout, "usage: %s port\n", argv[0]);
        fprintf(stdout, "e.g.   ");
        fprintf(stdout, "%s 9917\n", argv[0]);
        return 1;
    }

    if (maininit())
    {
        elog("error: maininit");
        return -1;
    }

#if DC_DEBUG
    olog("run!");
#endif

    if (work(argv[1]))
    {
        elog("error: work");
        return -1;
    }

#if DC_DEBUG
    olog("done");
#endif

    return 0;
}


/* calisten.c */
