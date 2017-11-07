#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <errno.h>
#include <assert.h>

#include "type.h"
#include "log.h"
#include "util.h"

#include "shm.h"
#include "blk.h"
#include "tree.h"
#include "seg.h"
#include "load.h"
#include "query.h"
#include "hash.h"
#include "list.h"

#include "sysloc.h"
#include "pladb.h"
#include "mydb.h"

#include "nt.h"



/* ================================================================= */

/* ================================================================= 
   ================================================================= */



extern int dc_net_strisdigit( char * );

static int _dc_net_tcp_listener_( int *sockid, u_int _addr, u_short _port, int _queue);

int dc_net_analysis_port( char *_portstr, u_short *_port )
{
    struct servent  *sent; 

    if( dc_net_strisdigit( _portstr ) ) {
        *_port = (u_short)atoi(_portstr);
        return E_OK;
    }

    if ( (sent=getservbyname( _portstr, "tcp" )) != NULL ) {
        *_port = (u_short)(ntohs(sent->s_port));
        return E_OK;
    }

    elog("error: getservbyname()");

    return E_FAIL;
}


int dc_net_analysis_addr( char *_addrstr, u_int *_addr )
{
    struct hostent *hent; 

    if ( (*_addr=inet_addr( _addrstr )) != INADDR_NONE )
        return E_OK;

    if ( (hent=gethostbyname( _addrstr )) != NULL ) {
        *_addr = ((struct in_addr *)(hent->h_addr_list[0]))->s_addr;
        return E_OK;
    }

    elog("error: gethostbyname()");

    return E_FAIL;
}


int dc_net_select( int _nfd, fd_set *_rfd, fd_set *_wfd, fd_set *_efd, struct timeval *_tv )
{
    int  ret;
    ret = select((SELECT_ARG1)_nfd, (SELECT_ARG234)_rfd,
            (SELECT_ARG234)_wfd, (SELECT_ARG234)_efd, (SELECT_ARG5)_tv);
    return  ret;
}


int dc_net_setnbio( int fd, int flg )
{
    int     optval;

    if ( flg == TRUE ) 
        optval = 1;
    else
        optval = 0;

    if ( ioctl( fd, FIONBIO, &optval ) == -1 ) {
        elog("error: ioctl()");
        return E_FAIL;
    }

    return E_OK;
}


int dc_net_getsockerr( int sock )
{
    int     rv;
    int     errval;

#if defined(DC_XOPEN_SOCKET)
    socklen_t errlen = sizeof(errval);
#else
    int     errlen=sizeof(errval);
#endif
    rv = getsockopt(sock,SOL_SOCKET, SO_ERROR,(void *)&errval, &errlen);
    if (rv < 0)
    {
        elog("error: getsockopt()");
        return E_FAIL;
    }

    errno = errval;

    return errval;
}


int dc_net_bytesreadable( int sock )
{
    int     value=0;

    if ( ioctl( sock, FIONREAD, &value ) < 0 ) {
        elog("error: ioctl()");
        return E_FAIL;
    }

    return value;
}


int dc_net_sockisreadable( int sock, int tmval )
{
    fd_set  readmask;
    struct  timeval tv, *ptv = NULL;

    memset( &tv, 0, sizeof(struct timeval) );
    if ( tmval >= 0 ) {
        tv.tv_sec       = tmval;
        ptv             = &tv;
    }

    for ( ; ; ) {
        FD_ZERO( &readmask );
        FD_SET( sock, &readmask );
        switch ( dc_net_select(sock+1, &readmask, 0, 0, ptv) ) {
            case 0: /* socket normal */
                return SS_TIMEDOUT;
            case -1: /* socket abnormal */
                if ( errno == EINTR )
                    continue;
                elog("error: select()" );
                return SS_BROKEN;
            default: /* socket has event occur */
                if ( FD_ISSET( sock, &readmask ) ) {
#ifndef __INTERIX
                    /* NaWei added 20030321 */
                    if( dc_net_bytesreadable( sock ) <= 0 )
                    {
                        return SS_BROKEN;
                    }
                    /* NaWei added end */
#endif

                    return SS_READABLE;
                }
        }
    }
}


int dc_net_closesock( int sock )
{
    return close( sock );
}


int dc_net_tcp_connector( u_int _addr, u_short _port )
{
    struct  sockaddr_in saddr;
    int     sock=-1;

    for ( ; ; ) {   /* Loop if interrupted by signal */
        if ( (sock=socket(AF_INET, SOCK_STREAM, 0)) < 0 ) {
            if ( errno == EINTR )
            {
                continue;
            }
            elog("error: socket()" );
            return E_FAIL;
        }

        memset( &saddr, 0x0, sizeof( saddr ) );
        saddr.sin_family       = AF_INET;
        saddr.sin_addr.s_addr  = _addr;
        saddr.sin_port         = htons( _port );

        if ( connect(sock, (void*)&saddr, sizeof(saddr)) < 0 ) {
            if ( errno == EINTR ) {
                dc_net_closesock( sock );
                continue;
            }
            elog("error: connect()" );
            dc_net_closesock( sock );
            return E_FAIL;
        }
        else
        {
            return sock;
        }
    }

}


int dc_net_tcp_aconnector( u_int _addr, u_short _port )
{
    struct	sockaddr_in	saddr;
    int		sock=-1;

    if ( (sock=socket(AF_INET, SOCK_STREAM, 0)) < 0 )
    {
        elog("error: socket()" );
        return E_FAIL;
    }

    if ( dc_net_setnbio( sock, TRUE ) != E_OK )
    {
        elog("error: dc_net_setnbio()" );
        dc_net_closesock( sock );
        return E_FAIL;
    }

    memset( &saddr, 0x0, sizeof( saddr ) );
    saddr.sin_family       = AF_INET;
    saddr.sin_addr.s_addr  = _addr;
    saddr.sin_port	   = htons( _port );

    if ( connect(sock, (void*)&saddr, sizeof(saddr)) == 0 )
    {
        /* success immediately */
        if (dc_net_setnbio(sock,FALSE) != E_OK )
        {
            elog("error: dc_net_setnbio()" );
            dc_net_closesock( sock );
            return E_FAIL;
        }
        return sock;
    }

    if ( errno == EINPROGRESS )
    {
        return sock;
    }

    elog("error: connect(): %d", errno);

    dc_net_closesock( sock );

    return E_FAIL;
}


int dc_net_tcp_aconn( u_int _addr, u_short _port, int _tmout )
{
    int	sock=-1;
    fd_set read_fd, write_fd;
    struct timeval tmout;

    if ( (sock=dc_net_tcp_aconnector( _addr, _port )) == E_FAIL )
    {
        elog("error: dc_net_tcp_aconnector()" );
        return E_FAIL;
    }

    if ( errno != EINPROGRESS )
    {
        /* success immediately */
        return sock;
    }

    FD_ZERO( &read_fd );
    FD_ZERO( &write_fd );

    FD_SET( sock, &write_fd );
    FD_SET( sock, &read_fd );

    tmout.tv_sec = _tmout;
    tmout.tv_usec = 0;

    if ( dc_net_select( sock+1, &read_fd, &write_fd, 0, &tmout ) <= 0 )
    {
        elog("error: dc_net_select()" );
        dc_net_closesock( sock );
        return E_FAIL;
    }

    if ( FD_ISSET( sock, &read_fd ) || FD_ISSET( sock, &write_fd ) )
    {
        if ( dc_net_getsockerr( sock ) == 0 )
        {
            dc_net_setnbio( sock, FALSE );
            return sock;
        }
    }

    /* failed */
    elog("error: dc_net_tcp_aconn()" );

    dc_net_closesock( sock );

    return E_FAIL;
}


int dc_net_tcp_accept( int sock, u_int *_peeraddr, u_short *_peerport )
{
    int     new_sock;
    struct  sockaddr_in peeraddr_in;

#if defined(DC_XOPEN_SOCKET)
    socklen_t addr_len;
#else
    int     addr_len;
#endif

    addr_len = sizeof( struct sockaddr_in );
    memset( &peeraddr_in, 0x0, addr_len );

    for ( ; ; ) {   /* Loop if interrupted by signal */
        errno = 0;
        new_sock = accept( sock, (void *)&peeraddr_in,  &addr_len );
        if ( new_sock == -1 ) {
            if ( errno == EINTR )
                continue;

            elog("error: accept()" );
            return E_FAIL;
        }
        break;
    }

    *_peeraddr = peeraddr_in.sin_addr.s_addr;
    *_peerport = peeraddr_in.sin_port;

    return( new_sock );
}


void dc_net_closeread( int sock )
{
    shutdown( sock, 0 );
}


void dc_net_closewrite( int sock )
{
    shutdown( sock, 1 );
}

#define DEFAULTBUFLEN   65535
#define MINSOCKBUFLEN   255


int dc_net_getsndbuflen( int sock )
{
    int     val;

#if defined(DC_XOPEN_SOCKET)
    socklen_t vallen;
#else
    int     vallen;
#endif

    vallen = sizeof(val);

    if ( getsockopt( sock, SOL_SOCKET, SO_SNDBUF, &val, &vallen ) < 0 ) {
        if ( errno == ENOTSOCK )
            val = DEFAULTBUFLEN;
        else
            val = MINSOCKBUFLEN;
    }

    return (val<=0)?MINSOCKBUFLEN:val;
}

int dc_net_getrcvbuflen( int sock )
{
    int     val;

#if defined(DC_XOPEN_SOCKET)
    socklen_t vallen;
#else
    int     vallen;
#endif
    vallen = sizeof(val);

    if ( getsockopt( sock, SOL_SOCKET, SO_RCVBUF, &val, &vallen ) < 0 ) {
        if ( errno == ENOTSOCK )
            val = DEFAULTBUFLEN;
        else
            val = MINSOCKBUFLEN;
    }

    return (val<=0)?MINSOCKBUFLEN:val;
}


int dc_net_send_nbytes( int sock, void *buf, int len )
{
    int     left=len, cnt, sended=0;
    int     s_buf_len=0;

    s_buf_len=dc_net_getsndbuflen( sock );

    while ( left > 0 ) {
        /* NaWei 20030618 modified */
        if ( (cnt=send(sock, (char *)buf+sended, \
                        min(left, s_buf_len),0)) <= 0 ) {
            if ( errno == EINTR )
                continue;
            elog("error: send()" );
            return E_FAIL;
        }

        left   -= cnt;
        sended += cnt;
        /* modified end */
    }

    return sended;
}


int dc_net_recv_nbytes( int sock, void *buf, int len, int tmot )
{
    int     left=len, cnt, readed=0;
    int     r_buf_len=0;

    r_buf_len = dc_net_getrcvbuflen( sock );

    while ( left > 0 )
        switch ( dc_net_sockisreadable(sock, tmot) ) {
            case SS_READABLE:
                if ( (cnt=recv(sock, (char *)buf+readed, \
                                min(left,r_buf_len),0)) <= 0 ) {
                    if ( errno == EINTR )
                        continue;
                    elog("error: recv()" );
                    return E_FAIL;
                }

                left   -= cnt;
                readed += cnt;
                break;

            case SS_TIMEDOUT:
                elog("error: sockisreadable() returned SS_TIMEDOUT" );
                return E_FAIL;
            case SS_BROKEN:
                elog("error: sockisreadable() returned SS_BROKEN" );
                return E_FAIL;
        }

    return readed;
}


int dc_net_tcp_listener( int *sockid, u_int _addr, u_short _port )
{
    return _dc_net_tcp_listener_(sockid, _addr, _port, SOMAXCONN);
}


static int _dc_net_tcp_listener_( int *sockid, u_int _addr, u_short _port, int _queue)
{
    struct      sockaddr_in s_addr_in;
    int         _sockid;
    int         optval=1;

    if ( (_sockid=socket( AF_INET, SOCK_STREAM, 0 )) == -1 ) {
        elog("error: socket()" );
        return E_FAIL;
    }

    memset( &s_addr_in, 0x0, sizeof( s_addr_in ) );

    s_addr_in.sin_family      = AF_INET;
    s_addr_in.sin_port        = htons( _port );
    s_addr_in.sin_addr.s_addr = _addr;

    setsockopt( _sockid, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(int) );


    if ( bind( _sockid, (void *)&s_addr_in, sizeof( s_addr_in ) ) == -1 ) {
        elog("error: bind()" );
        dc_net_closesock( _sockid );
        return E_FAIL;
    }

    if ( listen( _sockid, _queue) == -1 ) {
        elog("error: listen()" );
        dc_net_closesock( _sockid );
        return E_FAIL;
    }

    fcntl( _sockid, F_SETFD, 1 );
    *sockid = _sockid;

    return E_OK;
}



#if HAVE_IFACE_IFCONF
/* this works for Linux 2.2, Solaris 2.5, SunOS4, HPUX 10.20, OSF1
   V4.0, Ultrix 4.4, SCO Unix 3.2, IRIX 6.4 and FreeBSD 3.2.
   It probably also works on any BSD style system.  */
static int _get_ifaces( IFINFO *_ifaces, int _max_ifaces )
{
    struct ifconf ifc;
    char buff[8192];
    int fd, i, n;
    struct ifreq *ifr=NULL;
    int total = 0;
    struct in_addr ipaddr;
    struct in_addr nmask;
    char *iname;

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        return -1;

    ifc.ifc_len = sizeof(buff);
    ifc.ifc_buf = buff;

    if (ioctl(fd, SIOCGIFCONF, &ifc) != 0) {
        close(fd);
        return -1;
    } 

    ifr = ifc.ifc_req;

    n = ifc.ifc_len / sizeof(struct ifreq);

    /* Loop through interfaces, looking for given IP address */
    for (i=n-1;i>=0 && total < _max_ifaces;i--) {
        if (ioctl(fd, SIOCGIFADDR, &ifr[i]) != 0)
            continue;

        iname = ifr[i].ifr_name;
        ipaddr = (*(struct sockaddr_in *)&ifr[i].ifr_addr).sin_addr;

        if (ioctl(fd, SIOCGIFFLAGS, &ifr[i]) != 0)
            continue;

        if (!(ifr[i].ifr_flags & IFF_UP))
            continue;

        if (ioctl(fd, SIOCGIFNETMASK, &ifr[i]) != 0)
            continue;

        nmask = ((struct sockaddr_in *)&ifr[i].ifr_addr)->sin_addr;

        strncpy(_ifaces[total].if_name, iname, \
                sizeof(_ifaces[total].if_name)-1);
        _ifaces[total].if_name[sizeof(_ifaces[total].if_name)-1] = 0;
        _ifaces[total].if_iaddr = ipaddr;
        _ifaces[total].if_nmask = nmask;
        total++;
    }

    close(fd);

    return total;
}  

#elif HAVE_IFACE_IFREQ

#ifndef I_STR
#include <sys/stropts.h>
#endif

/****************************************************************************
  this should cover most of the streams based systems
  Thanks to Andrej.Borsenkow@mow.siemens.ru for several ideas in this code
 ****************************************************************************/
static int _get_ifaces( IFINFO * _ifaces, int _max_ifaces )
{
    struct ifreq ifreq;
    struct strioctl strioctl;
    char buff[8192];
    int fd, i, n;
    struct ifreq *ifr=NULL;
    int total = 0;
    struct in_addr ipaddr;
    struct in_addr nmask;
    char *iname;

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        return -1;

    strioctl.ic_cmd = SIOCGIFCONF;
    strioctl.ic_dp  = buff;
    strioctl.ic_len = sizeof(buff);
    if (ioctl(fd, I_STR, &strioctl) < 0) {
        close(fd);
        return -1;
    } 

    /* we can ignore the possible sizeof(int) here as the resulting
       number of interface structures won't change */
    n = strioctl.ic_len / sizeof(struct ifreq);

    /* we will assume that the kernel returns the length as an int
       at the start of the buffer if the offered size is a
       multiple of the structure size plus an int */
    if (n*sizeof(struct ifreq) + sizeof(int) == strioctl.ic_len)
        ifr = (struct ifreq *)(buff + sizeof(int));  
    else 
        ifr = (struct ifreq *)buff;  

    /* Loop through interfaces */

    for (i = 0; i<n && total < _max_ifaces; i++) {
        ifreq = ifr[i];

        strioctl.ic_cmd = SIOCGIFFLAGS;
        strioctl.ic_dp  = (char *)&ifreq;
        strioctl.ic_len = sizeof(struct ifreq);
        if (ioctl(fd, I_STR, &strioctl) != 0)
            continue;

        if (!(ifreq.ifr_flags & IFF_UP))
            continue;

        strioctl.ic_cmd = SIOCGIFADDR;
        strioctl.ic_dp  = (char *)&ifreq;
        strioctl.ic_len = sizeof(struct ifreq);
        if (ioctl(fd, I_STR, &strioctl) != 0)
            continue;

        ipaddr = (*(struct sockaddr_in *) &ifreq.ifr_addr).sin_addr;
        iname = ifreq.ifr_name;

        strioctl.ic_cmd = SIOCGIFNETMASK;
        strioctl.ic_dp  = (char *)&ifreq;
        strioctl.ic_len = sizeof(struct ifreq);
        if (ioctl(fd, I_STR, &strioctl) != 0)
            continue;

        nmask = ((struct sockaddr_in *)&ifreq.ifr_addr)->sin_addr;

        strncpy(_ifaces[total].if_name, iname, \
                sizeof(_ifaces[total].if_name)-1);
        _ifaces[total].if_name[sizeof(_ifaces[total].if_name)-1] = 0;
        _ifaces[total].if_iaddr = ipaddr;
        _ifaces[total].if_nmask = nmask;

        total++;
    }

    close(fd);

    return total;
}

#elif HAVE_IFACE_AIX

/****************************************************************************
  this one is for AIX (tested on 4.2)
 ****************************************************************************/
static int _get_ifaces( IFINFO *_ifaces, int _max_ifaces )
{
    char buff[8192];
    int fd, i;
    struct ifconf ifc;
    struct ifreq *ifr=NULL;
    struct in_addr ipaddr;
    struct in_addr nmask;
    char *iname;
    int total = 0;

    if ((fd = socket(AF_INET, SOCK_DGRAM, 0)) == -1)
        return -1;

    ifc.ifc_len = sizeof(buff);
    ifc.ifc_buf = buff;

    if (ioctl(fd, SIOCGIFCONF, &ifc) != 0) {
        close(fd);
        return -1;
    }

    ifr = ifc.ifc_req;

    /* Loop through interfaces */
    i = ifc.ifc_len;

    while (i > 0 && total < _max_ifaces) {
        unsigned inc;

        inc = ifr->ifr_addr.sa_len;

        if (ioctl(fd, SIOCGIFADDR, ifr) != 0)
            goto next;

        ipaddr = (*(struct sockaddr_in *) &ifr->ifr_addr).sin_addr;
        iname = ifr->ifr_name;

        if (ioctl(fd, SIOCGIFFLAGS, ifr) != 0)
            goto next;

        if (!(ifr->ifr_flags & IFF_UP))
            goto next;

        if (ioctl(fd, SIOCGIFNETMASK, ifr) != 0)
            goto next;

        nmask = ((struct sockaddr_in *)&ifr->ifr_addr)->sin_addr;

        strncpy(_ifaces[total].if_name, iname, \
                sizeof(_ifaces[total].if_name)-1);
        _ifaces[total].if_name[sizeof(_ifaces[total].if_name)-1] = 0;
        _ifaces[total].if_iaddr = ipaddr;
        _ifaces[total].if_nmask = nmask;

        total++;

next:
        /*
         * The addresses in the SIOCGIFCONF interface list have
         * a minimum size. Usually this doesn't matter, but if
         * your machine has tunnel interfaces, etc. that have
         * a zero length "link address", this does matter.  */

        if (inc < sizeof(ifr->ifr_addr))
            inc = sizeof(ifr->ifr_addr);
        inc += IFNAMSIZ;

        ifr = (struct ifreq*) (((char*) ifr) + inc);
        i -= inc;
    }


    close(fd);
    return total;
}

#else /* a dummy version */
static int _get_ifaces( IFINFO *_ifaces, int _max_ifaces )
{
    _ifaces = _ifaces;
    _max_ifaces = _max_ifaces;
    return -1;
}
#endif


static int iface_comp( const void *i1, const void *i2 )
{
    IFINFO  *p1, *p2;
    int     r; 

    p1 = (IFINFO *)i1, p2 = (IFINFO *)i2;

    r = strcmp(p1->if_name, p2->if_name);
    if (r) return r;
    r = ntohl(p1->if_iaddr.s_addr) - ntohl(p2->if_iaddr.s_addr);
    if (r) return r;
    r = ntohl(p1->if_nmask.s_addr) - ntohl(p2->if_nmask.s_addr);
    return r;
}


/* this wrapper is used to remove duplicates from the interface list generated
   above */
int dc_net_get_interfaces( IFINFO *_ifaces, int _max_ifaces )
{
    int total, i, j;

    total = _get_ifaces( _ifaces, _max_ifaces );
    if (total <= 0) return total;

    /* now we need to remove duplicates */
    qsort( _ifaces, total, sizeof(_ifaces[0]), iface_comp );

    for (i=1;i<total;)
        if (iface_comp(&_ifaces[i-1], &_ifaces[i]) == 0) {
            for (j=i-1;j<total-1;j++)
                _ifaces[j] = _ifaces[j+1];
            total--;
        }
        else
            i++;

    return total;
}


static int dc_net_cmpr_ipaddr( const void *d1, const void *d2 )
{
    return ((IFINFO *)d1)->if_iaddr.s_addr  - ((IFINFO *)d2)->if_iaddr.s_addr;
}


int dc_net_is_local_addr( u_int _addr )
{
    IFINFO  ifaces[255];
    IFINFO  key;
    int     ifacenum=0;

    memset( ifaces, 0x0, sizeof(ifaces) );
    if ( (ifacenum=dc_net_get_interfaces( ifaces, 255 )) == -1 ) {
        return E_FAIL;
    }

    key.if_iaddr.s_addr = _addr;

    if ( bsearch( &key, ifaces, ifacenum, sizeof(key), dc_net_cmpr_ipaddr ) \
            != NULL )
        return TRUE;

    return FALSE;
}

/*
 * Function Name: dc_net_tcp_setopt
 * Description  :
 * Input        : int _sfd      --
 *                int _opt_name -- SO_REUSEADDR SO_KEEPALIVE SO_LINGER
 *                int _opt_val  -- 0/1
 * Output       : 
 * Return       : E_OK    --  Success
 *                E_FAIL  --  Failure
 */
int dc_net_tcp_setopt( int _sfd, int _opt_name, int _opt_val )
{
    struct linger lin;
    int ret=E_FAIL;

    switch( _opt_name ) {
        case SO_REUSEADDR:
            ret = setsockopt( _sfd, SOL_SOCKET, SO_REUSEADDR, &_opt_val, sizeof(int) );
            break;
        case SO_KEEPALIVE:
            ret = setsockopt( _sfd, SOL_SOCKET, SO_KEEPALIVE, &_opt_val, sizeof(int) );
            break;
        case SO_LINGER:
            lin.l_onoff = _opt_val;
            if( _opt_val )
                lin.l_linger = 2 * 10;
            else
                lin.l_linger = 0;
            ret = setsockopt( _sfd, SOL_SOCKET, SO_LINGER, &lin, sizeof(lin) );
            break;
    }

    if( ret == E_FAIL )
    {
        elog( "error: setsockopt()" );
    }

    return ret;
}


int dc_net_strisdigit( char *_str )
{
    while ( *_str && *_str != '\n' && *_str != '\r' ) {
        if ( !isdigit( (int)*(_str ++) ) ){
            return FALSE;
        }
    }

    return TRUE;
}


int dc_net_str_ip(unsigned int _ip, char *_buffer, int _size)
{
    struct in_addr  peer_in;

    memset(&peer_in, 0, sizeof(peer_in));

    peer_in.s_addr = _ip;

    snprintf(_buffer, _size, inet_ntoa(peer_in));

    return 0;
}

/*****************************************************************************/
/*****************************************************************************/
/*****************************************************************************/

int dc_net_simple_listen(char *_s_port)
{
    int             sid=0;
    int             queue = 0;
    unsigned int    addr=0;
    unsigned short  port=0;
    char           *any = "0.0.0.0";

    /* get ip */
    if (dc_net_analysis_addr(any, &addr) != 0)
    {
        elog("error: invalid ip[%s]", any);
        goto _end;
    }

    /* get port */
    if (dc_net_analysis_port(_s_port, &port) != 0)
    {
        elog("error: invalid port[%s]", _s_port);
        goto _end;
    }

    /* listen */
    queue = 10; /* keep shallow   */
    if (_dc_net_tcp_listener_(&sid, addr, port, queue))
    {
        elog("error: _dc_net_tcp_listener_ failure");
        goto _end;
    }
    else
    {
#if DC_DEBUG
        olog("nice: sid: %d", sid);
#endif
        return sid;
    }

_end:
    return -1;
}


int dc_net_simple_request(int _sid,
        char *_input,  int _len,
        char *_output, int _size, int _life)
{
    int             rv = 0;
    int             cnt= 0;
    int             life = 0;

    if (_life <= 0)
    {
        life = 30;
    }
    else
    {
        life = _life;
    }
#if DC_DEBUG
    olog("life: %d", life);
#endif

    /* send */
    cnt = dc_net_send_nbytes(_sid, _input, _len);
    if (cnt != _len)
    {
        elog("error: dc_net_send_nbytes: %d", cnt);
        return -1;
    }

    /* receive */
    cnt = dc_net_recv_nbytes(_sid, _output, _size, life);
    if (cnt <= 0)
    {
        elog("error: dc_net_recv_nbytes: %d", cnt);
        return -1;
    }
#if DC_DEBUG
    olog("received bytes: %d", cnt);
#endif

    return rv;
}


int dc_net_simple_connect(char *_ip, char *_port)
{
    int             sid= -1;
    unsigned int    addr=0;
    unsigned short  port=0;

    /* get ip */
    if (dc_net_analysis_addr(_ip, &addr) != 0)
    {
        elog("error: invalid ip[%s]", _ip);
        return -1;
    }

    /* get port */
    if (dc_net_analysis_port(_port, &port) != 0)
    {
        elog("error: invalid port[%s]", _port);
        return -1;
    }

    /* connect */
    sid = dc_net_tcp_connector(addr, port);
    if (sid < 0)
    {
        elog("error: dc_net_tcp_connector");
        return -1;
    }

    return sid;
}

#if RUN_NET

int main(int argc, char *argv[])
{
    int  i   = 0;
    int  rv  = 0;
    int  listen = 0;
    int  client = 0;
    int  sock = 0;
    int  len = 0;
    int  max = 0;
    int  type = 0;
    unsigned int peer_addr = 0;
    unsigned short peer_port = 0;
    char s_port[16] = {0};
    char s_ip[32] = {0};
    char buffer[128] = {0};
    char remote_ip[32] = {0};
    char remote_port[16] = {0};

    (void)argc;
    (void)argv;

    olog("net begin");

    strcpy(s_port, "8899");
    listen = dc_net_simple_listen(s_port);
    if (listen < 0)
    {
        elog("error: dc_net_simple_listen: %s", s_port);
        return -1;
    }
    olog("net: %s ==> %d", s_port, listen);

    /*+++++++++++++++++++++++++++++++++++++++++++++*/

#if 0
    client = dc_net_tcp_accept(listen, &peer_addr, &peer_port);
    if (client < 0)
    {
        elog("error: dc_net_tcp_accept: %d", listen);
        return -1;
    }
    else
    {
        olog("client: %d", client);
    }

    dc_net_str_ip(peer_addr, s_ip, sizeof(s_ip));
    olog("client from %s", s_ip);
#endif

    /*
    if (dc_net_init())
    {
        elog("error: dc_net_init");
        return -1;
    }

    dc_net_destroy();
    */

    pause();

    olog("net end");

    return rv;
}
#endif

/* net.c */
