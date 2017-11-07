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


/* sid as key */
typedef struct _peer
{
    int  sid;
    int  status;        /* 0: init; 1: connecting; 2: ready; 3: broken */

    char ip[16];
    char port[6];

    Link link;
} peer_t;


/* ip+port as key */
typedef struct _addr
{
    char ip[16];
    char port[6];

    int  status;
} addr_t;

#define DC_SYNC_MAX_HOST    32
addr_t g_addr_list[DC_SYNC_MAX_HOST];

static List  *g_initiative_list = NULL;
static List  *g_passive_list    = NULL;
static int    g_listen_sid = -1;

static char g_user[100] = {0};

static int maininit();

static int work();

extern void dc_strrev(char *p);
extern long get_time();
extern long dc_get_time_of_seconds();

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
    fprintf(stdout, "user: [%s]\n", g_user);

    /* as client, connect to other servers */
    g_initiative_list = create_list();
    if (g_initiative_list == NULL)
    {
        elog("error: create_list1 failure");
        return -1;
    }
    olog("initiative list created");

    /* as server, other connect to me */
    g_passive_list = create_list();
    if (g_passive_list == NULL)
    {
        elog("error: create_list2 failure");
        return -1;
    }
    olog("passive list created");

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


void dc_sync_dump(List *_list, char *_label)
{
    int  n = 0;
    peer_t *p = NULL;

    list_for_each_entry(p, _list->head, link)
    {
        olog("dump:[%s][%d] --- [%d, %d, %s, %s]", _label, n++, p->sid, p->status, p->ip, p->port);
    }

    olog("dump:[%s] list-number: %d", _label, n);
}


int dc_sync_delete(List *_list, int _sid)
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


/* only for initiative client */
int dc_sync_set_config_status(char *_ip, char *_port, int _status)
{
    int i = 0;
    int rv = -1;
    addr_t *addr = NULL;

    /*  connect other servers */
    for (i = 0; i < DC_SYNC_MAX_HOST; i++)
    {
        addr = &g_addr_list[i];

        if (strlen(addr->ip) == 0)
        {
#if DC_DEBUG
            olog("config: [%d]th no data, means the last.", i);
#endif
            break;
        }

        if (strcmp(_ip, addr->ip) == 0 && strcmp(_port, addr->port) == 0)
        {
            addr->status = _status;
            rv = 0;
            olog("config: set [%s:%s] to status[%d]", _ip, _port, _status);
        }
    }

    return rv;
}


/* clear the broken socket from list */
int dc_sync_clear(List *_list)
{
    int  n = 0;
    peer_t *p = NULL;
    peer_t *q = NULL;

    p = list_entry(_list->head->next, DC_TYPEOF(*p), link);

    while (&p->link != _list->head)
    {
        q = list_entry(p->link.next, DC_TYPEOF(*p), link);

        if (p->sid < 0 || p->status == DC_SYNC_STATUS_BROKEN)
        {
            olog("find broken socket[%d] [%s:%s], delete it!", p->sid, p->ip, p->port);

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
            olog("keep: sid[%d], status[%d], [%s, %s]", p->sid, p->status, p->ip, p->port);
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


int dc_sync_aconnect(addr_t *_list, int _n)
{
    int i = 0;
    int	sid = -1;
    int status = 0;
    unsigned int peer_addr = 0;
    unsigned short peer_port = 0;
    char   ip[16] = {0};
    char   port[6] = {0};

    addr_t *addr = NULL;

    /*  connect other servers */
    for (i = 0; i < _n; i++)
    {
        addr = &_list[i];
        if (strlen(addr->ip) == 0)
        {
#if DC_DEBUG
            olog("[%d]th no data, means the last.", i);
#endif
            break;
        }
        else
        {
            snprintf(ip,    sizeof(ip), "%s", addr->ip);
            snprintf(port,  sizeof(port), "%s", addr->port);
            olog("[%d]th -- [%s, %s]", i, ip, port);
        }

        /* check status */
        if (addr->status == DC_SYNC_STATUS_INIT || addr->status == DC_SYNC_STATUS_BROKEN)
        {
            olog("config--init/broken, let's connect: %d", addr->status);
        }
        else
        {
            olog("already connecting/connected: %d", addr->status);
            continue;
        }

        /* get ip */
        if (dc_net_analysis_addr(ip, &peer_addr) != 0)
        {
            elog("error: invalid ip[%s]", ip);
            return -1;
        }

        /* get port */
        if (dc_net_analysis_port(port, &peer_port) != 0)
        {
            elog("error: invalid port[%s]", port);
            return -1;
        }

        /* asynchronous connect */
        if ( (sid = dc_net_tcp_aconnector( peer_addr, peer_port )) == -1)
        {
            elog("error: dc_net_tcp_aconnector()" );
            return -1;
        }

        if ( errno != EINPROGRESS )
        {
            /* success immediately */
            status = DC_SYNC_STATUS_READY;
            olog("immediately succeeds: %d", sid);
        }
        else
        {
            status = DC_SYNC_STATUS_CONNECTING;
            olog("in-progress: %d", sid);
        }

        /* as client */
        if (add_to_container(g_initiative_list, sid, status, ip, port))
        {
            elog("error: add_to_container: initiative");
            dc_net_closesock(sid);
            return -1;
        }
        else
        {
            olog("add to container: sid:%d, status:%d, %s:%s", sid, status, ip, port);
        }

        addr->status = status;

    } /* for */

    return 0;
} /* dc_sync_aconnect */



int dc_sync_task(int _sid)
{
    int    cnt = 0;
    int    len = 0;
    char   buffer[1024] = {0};

    len = dc_net_getrcvbuflen( _sid );

    if (len >= sizeof(buffer))
    {
        len = sizeof(buffer) - 1;
    }
    olog("to receive bytes: %d", len);

    if ((cnt=recv(_sid, buffer, len, 0)) <= 0)
    {
        elog("error: recv(): %d", errno);
        return -1;
    }
    olog("[%d] bytes received", cnt);

    olog("[%s]", buffer);

    /* TODO: do some work */
    if (buffer[cnt-1] == '\n')
    {
        buffer[cnt-1] = 0;
        dc_strrev(buffer);
        /* buffer[cnt-1] = '\n'; */
    }
    else
    {
        dc_strrev(buffer);
    }

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


int dc_sync_event(int _tmout)
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
    fd_set write_fd;    /* for asynchronous connect */
    struct timeval tmout;

    olog("let's check I/O events");

    while (1)
    {

        /* clear broken socket */
        {
            k = dc_sync_clear(g_initiative_list);
            if (k > 0)
            {
                olog("%d initiative socket cleared", k);
            }

            k = dc_sync_clear(g_passive_list);
            if (k > 0)
            {
                olog("%d passive socket cleared", k);
            }
            olog("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
        }

        /* set fd_set */
        {
            FD_ZERO( &read_fd );
            FD_ZERO( &write_fd );
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
                olog("to monitor listen sid: %d", g_listen_sid);
            }

            /* client */
            list_for_each_entry(p, g_initiative_list->head, link)
            {
                if (p->sid >= 0)
                {
                    olog("to monitor initiative sid: %d", p->sid);
                    FD_SET( p->sid, &read_fd );     /* for broken */

                    if (p->status == DC_SYNC_STATUS_CONNECTING)
                    {
                        FD_SET( p->sid, &write_fd );    /* for connect */
                    }

                    fd_num += 1;

                    if (p->sid > max_fd)
                    {
                        olog("---refreshes: %d --> %d", max_fd, p->sid);
                        max_fd = p->sid;
                    }
                }
                else
                {
                    olog("initiative socket: %s:%s already broken: %d", p->ip, p->port, p->sid);
                }
            }

            /* server */
            list_for_each_entry(p, g_passive_list->head, link)
            {
                if (p->sid >= 0)
                {
                    olog("to monitor passive sid: %d", p->sid);
                    FD_SET( p->sid, &read_fd );
                    fd_num += 1;

                    if (p->sid > max_fd)
                    {
                        olog("---refreshes: %d --> %d", max_fd, p->sid);
                        max_fd = p->sid;
                    }
                }
                else
                {
                    olog("passive socket: %s:%s already broken: %d", p->ip, p->port, p->sid);
                }
            }

            olog("max-fd: %d", max_fd);
        } /* fd_set */

        tmout.tv_sec = _tmout;
        tmout.tv_usec = 0;

        n = dc_net_select( max_fd+1, &read_fd, &write_fd, 0, &tmout );
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
                olog("waked, has time out: %d seconds", _tmout);
                break;
            default:
                olog("nice, %d fd is avaiable", n);
                break;
        }

        if (n > 0)
        {
            /* case: check is initiative/client socket */
            p = NULL;
            list_for_each_entry(p, g_initiative_list->head, link)
            {
                if ( FD_ISSET( p->sid, &read_fd))
                {
                    olog("initiative socket-IS  --- [%d, %d, %s:%s]", p->sid, p->status, p->ip, p->port);
                    if ( (rv = dc_net_getsockerr( p->sid)) == 0 )
                    {
                        /* disconnect */
                        olog("initiative socket [%d] read is ready", p->sid);
                        elog("which means the other endpoint had closed");
                        p->status = DC_SYNC_STATUS_BROKEN;

                        /* set initiative config status */
                        if (dc_sync_set_config_status(p->ip, p->port, p->status))
                        {
                            elog("error: dc_sync_set_config_status: %s, %s", p->ip, p->port);
                            return -1;
                        }
                    }
                    else
                    {
                        /* connect failure */
                        /* HPUX: ECONNREFUSED-239 */
                        olog("initiative socket [%d] NOT ready: %d", p->sid, rv);

                        /* mark the socket as discared */
                        olog("initiative socket [%d] broken!", p->sid);
                        p->status = DC_SYNC_STATUS_BROKEN;

                        /* set initiative config status */
                        if (dc_sync_set_config_status(p->ip, p->port, p->status))
                        {
                            elog("error: dc_sync_set_config_status: %s, %s", p->ip, p->port);
                            return -1;
                        }
                    }
                }
                else if (FD_ISSET( p->sid, &write_fd) )
                {
                    olog("initiative socket-IS2 --- [%d, %d, %s:%s]", p->sid, p->status, p->ip, p->port);
                    if ( (rv = dc_net_getsockerr( p->sid)) == 0 )
                    {
                        olog("initiative socket [%d] write is ready", p->sid);
                        dc_net_setnbio( p->sid, FALSE );
                        p->status = DC_SYNC_STATUS_READY;

                        /* set initiative config status */
                        if (dc_sync_set_config_status(p->ip, p->port, p->status))
                        {
                            elog("error: dc_sync_set_config_status: %s, %s", p->ip, p->port);
                            return -1;
                        }
                    }
                    else
                    {
                        /* mark the socket as discared */
                        olog("initiative socket [%d] broken!", p->sid);
                        p->status = DC_SYNC_STATUS_BROKEN;

                        /* set initiative config status */
                        if (dc_sync_set_config_status(p->ip, p->port, p->status))
                        {
                            elog("error: dc_sync_set_config_status: %s, %s", p->ip, p->port);
                            return -1;
                        }
                    }
                }
                else
                {
                    olog("initiative socket-NOT --- [%d, %d, %s, %s]", p->sid, p->status, p->ip, p->port);
                }
            }

            /* case: check is passive/server socket */
            p = NULL;
            list_for_each_entry(p, g_passive_list->head, link)
            {
                if ( FD_ISSET( p->sid , &read_fd ) )
                {
                    olog("passive socket-IN --- [%d, %d, %s:%s]", p->sid, p->status, p->ip, p->port);
                    if ( (rv = dc_net_getsockerr( p->sid)) == 0 )
                    {
                        /* TODO: do-task */
                        olog("passive socket [%d] do task", p->sid);
                        rv = dc_sync_task(p->sid);
                        if (rv)
                        {
                            elog("error: dc_sync_task");
                        }
                        else
                        {
                            olog("nice: execute task succeeds");
                        }
                    }
                    else
                    {
                        /* TODO: socket broken */
                        olog("passive socket [%d] broken: %d", p->sid, rv);

                        /* mark the socket as discared */
                        p->status = DC_SYNC_STATUS_BROKEN;

                        /* TODO: set config status */
                    }
                }
                else
                {
                    olog("passive socket-NOT --- [%d, %d, %s, %s]", p->sid, p->status, p->ip, p->port);
                }
            }

            /* case: check is listen socket */
            if (FD_ISSET(g_listen_sid, &read_fd))
            {
                olog("listen socket is readable");

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
                olog("new client arrives: %d, %s, %s", client_sid, ip, port);

                rv = add_to_container(g_passive_list, client_sid, status, ip, port);
                if (rv)
                {
                    elog("error: add_to_container: passive");
                    return -1;
                }
                else
                {
                    olog("add to passive container: sid:%d, status:%d, %s:%s", client_sid, status, ip, port);
                }
            }


        }

        break;
    } /* while */

#if DC_DEBUG
    olog("function {%s} ends", __func__);
#endif

    return 0;
} /* dc_sync_event */


static int work()
{
    int  n  = 0;
    int  rv = 0;
    int  to_connect = 0;
    int  connect_interval = 5;
    long curr_time = 0;
    long last_time = 0;
    long diff_time = 0;
    char s_port[6] = {0};

    /* create listen socket */
    strcpy(s_port, "9917");
    g_listen_sid = dc_net_simple_listen(s_port);
    if (g_listen_sid < 0)
    {
        elog("error: dc_net_simple_listen: %s", s_port);
        return -1;
    }
    olog("nice: listen socket is %d", g_listen_sid);

    /* TODO: load config file */
    memset(g_addr_list, 0, sizeof(g_addr_list));
    strcpy(g_addr_list[0].ip, "197.1.5.10");
    strcpy(g_addr_list[0].port, "9910");
    strcpy(g_addr_list[1].ip, "197.1.5.18");
    strcpy(g_addr_list[1].port, "9918");

    connect_interval = 10;

    while (1)
    {
        olog("-----------------------------");

        to_connect = 0;
        curr_time = dc_get_time_of_seconds();
        diff_time = curr_time - last_time;
        if (diff_time > connect_interval)
        {
            to_connect = 1;
            last_time = curr_time;
            olog("let's connect again");
        }

        /* connect for not ready client */
        /* as client  */
        if (to_connect)
        {
            to_connect = 0;
            rv = dc_sync_aconnect(g_addr_list, DC_SYNC_MAX_HOST);
            if (rv)
            {
                elog("error: dc_sync_aconnect");
                return -1;
            }

            dc_sync_dump(g_initiative_list, "connect");
        }


        /* check event */
        rv = dc_sync_event(5);
        if (rv < 0)
        {
            elog("error: dc_sync_event");
        }
        else if (rv)
        {
            olog("warn: dc_sync_event");
        }
        else
        {
            olog("nice: monitor event succeeds");
        }

        /* TODO: do local request */


        dc_sync_dump(g_initiative_list, "initiative2");
        dc_sync_dump(g_passive_list, "passive2");

        n++;
        if (n > 5)
        {
            break;
        }
    } /* while */


    olog("work done");

    return 0;
}


int main(int argc, char *argv[])
{

    if (maininit())
    {
        elog("error: maininit");
        return -1;
    }

    olog("run!");

    if (work())
    {
        elog("error: work");
        return -1;
    }

    olog("done");

    return 0;
}


/* casync.c */
