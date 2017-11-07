#ifndef __DC_NET_H_
#define __DC_NET_H_


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


/* ip+port as key */
typedef struct _dc_nt_addr
{
    char ip[16];
    char port[6];
    int  sid;
} dc_nt_addr;


int dc_net_analysis_port( char *_portstr, u_short *_port );
int dc_net_analysis_addr( char *_addrstr, u_int *_addr );

int dc_net_select( int _nfd, fd_set *_rfd, fd_set *_wfd, fd_set *_efd, struct timeval *_tv );

int dc_net_setnbio( int fd, int flg );
int dc_net_getsockerr( int sock );
int dc_net_bytesreadable( int sock );
int dc_net_sockisreadable( int sock, int tmval );
int dc_net_closesock( int sock );
int dc_net_tcp_connector( u_int _addr, u_short _port );
int dc_net_tcp_aconnector( u_int _addr, u_short _port );
int dc_net_tcp_aconn( u_int _addr, u_short _port, int _tmout );
int dc_net_tcp_accept( int sock, u_int *_peeraddr, u_short *_peerport );
void dc_net_closeread( int sock );
void dc_net_closewrite( int sock );
int dc_net_getsndbuflen( int sock );
int dc_net_getrcvbuflen( int sock );
int dc_net_send_nbytes( int sock, void *buf, int len );
int dc_net_recv_nbytes( int sock, void *buf, int len, int tmot );
int dc_net_tcp_listener( int *sockid, u_int _addr, u_short _port );
int dc_net_tcp_setopt( int _sfd, int _opt_name, int _opt_val );
int dc_net_str_ip(unsigned int _ip, char *_buffer, int _size);

int dc_net_simple_listen(char *_s_port);
int dc_net_simple_connect(char *_ip, char *_port);
int dc_net_simple_request(int _sid,
        char *_input,  int _len,
        char *_output, int _size, int _life);

#endif
