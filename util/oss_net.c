#include "oss.h"
#include "oss_time.h"
#include "oss_net.h"
#include "protocol_common.h"

#ifdef  WIN32
#pragma comment(lib, "ws2_32.lib")
#endif

/******************************************  udp  ********************************************************/
int gtUdpServerSockFd = -1;
struct sockaddr_in gtUdpServerAddr;

int gtTcpServerSockFd = -1;
struct sockaddr_in gtTcpServerAddr;

static int gatClientConn[MAX_CLIENT_CONN]; /* 记录接受的客户连接 */

#if 1 ///shangtao
fd_set read_all;
int    epoll_fd;
int    giMaxFp = 0;
#else
int    epoll_fd;
#endif

/******************************************  udp  ********************************************************/
int NetConnectUdpInit(int localIp, short localPort);
int NetConnectTcpInit(short port);

int NetSetSocketQueue(int flag);
int NetSetSocketNonblock(int fd);

void NetConnectNewClientConn(int tcpServerSock);

/******************************************  udp  ********************************************************/
int MobileRouteNetConnectInit(u16 tcpPort, u16 udpPort)
{
	int ret;

#ifdef WIN32
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	
	wVersionRequested = MAKEWORD( 1, 1 );
	
	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) 
	{
		return -1;
	}
	
	if ( LOBYTE( wsaData.wVersion ) != 1 ||
        HIBYTE( wsaData.wVersion ) != 1 ) 
	{
		WSACleanup( );
		return -1;
	}

#else

    //创建epoll描述符, 可接受100个连接
    epoll_fd = epoll_create(MAX_EVENTS);
	if(epoll_fd < 0)
	{
		return -1;
	}

#endif

	if(udpPort > 0)
	{
		gtUdpServerSockFd = NetConnectUdpInit(0, udpPort);
	}
	else
	{
		gtUdpServerSockFd = NetConnectUdpInit(0, MOBILE_ROUTE_UDP_SERVER_PORT);
	}
	if(gtUdpServerSockFd < 0)
	{
		return -1;
	}

	if(tcpPort > 0)
	{
		ret = NetConnectTcpInit(tcpPort);
	}
	else
	{
		ret = NetConnectTcpInit(MOBILE_ROUTE_TCP_SERVER_PORT);
	}
	if(ret < 0)
	{
		return -1;
	}
	
	return 1;
}

/******************************************  udp  ********************************************************/
int NetConnectInit(unsigned short port)
{
#ifdef WIN32
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	
	wVersionRequested = MAKEWORD( 1, 1 );
	
	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 ) 
	{
		return -1;
	}
	
	if ( LOBYTE( wsaData.wVersion ) != 1 ||
        HIBYTE( wsaData.wVersion ) != 1 ) 
	{
		WSACleanup( );
		return -1;
	}
#endif

	gtUdpServerSockFd = NetConnectUdpInit(0, port);
	if(gtUdpServerSockFd < 0)
	{
		return -1;
	}

	return 1;
}

/******************************************  udp  ********************************************************/
int NetConnectUdpRecv(char *buf, int maxLen, struct sockaddr_in *addr,unsigned int *addrlen)
{
	int n;

	if(gtUdpServerSockFd == -1)
	{
		return 0;
	}

	n = recvfrom(gtUdpServerSockFd, buf, maxLen, 0, (struct sockaddr *)addr, addrlen);

	return n;
}

/**************************************************************************************************/
int NetConnectUdpSend(int peerIpv4Addr, short port, char *buf, int bufLen)
{
	struct sockaddr_in addr;
	int ret;
 	
	if(gtUdpServerSockFd < 0)
	{
		return -1;
	}

	if(peerIpv4Addr == 0)
	{
		return -1;
	}

	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family      = AF_INET;
	addr.sin_addr.s_addr = htonl(peerIpv4Addr);
	addr.sin_port        = htons(port);

	ret = sendto(gtUdpServerSockFd, buf, bufLen, 0,(struct sockaddr *)&addr, sizeof(struct sockaddr_in));
	return ret;
}

/**************************************************************************************************/
int NetConnectUdpInit(int localIp, short localPort)
{
	struct sockaddr_in addr;
	int i;
	int udp_sock;

	/* udp send/recv socket */
	udp_sock = socket(AF_INET, SOCK_DGRAM, 0);
	if(udp_sock < 0)
	{
		printf("[NetConnectUdpInit]socket create failed \n");
		return -1;
	}

	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family      = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port        = htons(localPort);
	if(bind(udp_sock, (struct sockaddr *)&addr, sizeof(struct sockaddr_in))<0)
	{
		NetConnectCloseSocket(udp_sock);
		printf("[NetConnectUdpInit]socket bind failed \n");
		return -1;
	}

	for (i = 0; i < MAX_CLIENT_CONN; i++)
	{
		gatClientConn[i] = INVALID_SOCKET;
	}

	return udp_sock;
}

/******************************************  tcp  ********************************************************/
int NetConnectTcpInit(short port)
{
    int sockfd;
	struct sockaddr_in addr; /* 本机地址信息 */
    int lisnum = 32;
	int  opt = 1;
	int i;

    if ((sockfd = socket(PF_INET, SOCK_STREAM, 0)) == -1) 
	{
        perror("socket");
        exit(1);
    }
	
	// addresss reuse
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (const void*)&opt, sizeof(opt));

	// non blocking
	NetSetSocketNonblock(sockfd);		

	memset(&addr, 0, sizeof(struct sockaddr_in));
    addr.sin_family      = PF_INET;
    addr.sin_port        = htons(port);
    addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sockfd, (struct sockaddr *)&addr, sizeof(struct sockaddr)) == -1) 
	{
		NetConnectCloseSocket(sockfd);
        perror("bind");
        exit(1);
    }
  
    if (listen(sockfd, lisnum) == -1) 
	{
		NetConnectCloseSocket(sockfd);
        perror("listen");
        exit(1);
    }

	for (i = 0; i < MAX_CLIENT_CONN; i++)
	{
		gatClientConn[i] = INVALID_SOCKET;
	}

	gtTcpServerSockFd = sockfd;

	return 1;
}

/******************************************  tcp  ********************************************************/
#if 0
#define  MAX_BUF_LEN    (4096)
char buf[MAX_BUF_LEN];

int NetConnectTcpRecv(char *ipv4Addr)
{
    struct sockaddr_in their_addr; /* 客户地址信息 */
	int new_fd, sin_size, iLen;

    sin_size = sizeof(struct sockaddr_in);
    if ((new_fd = accept(gtTcpServerSockFd, (struct sockaddr *)&their_addr, &sin_size)) == -1) 
	{
        perror("accept");
		return -1;
    }

    printf("server: got connection from %s\n",inet_ntoa(their_addr.sin_addr));

	iLen = recv(new_fd, buf, MAX_BUF_LEN, 0);   
    if((iLen < 0) || (iLen == 0))
    {
        printf("recv() failure!\n");
        return -1;
    }
   
//    NetConnectCloseSocket(new_fd);

	return 1;
} 
#endif
/******************************************  tcp  ********************************************************/
int NetConnectSocketSend(int iSendSock, char *buf, int len)
{
    int tmp;
    size_t total  = len;
    const char *p = buf;
	
    while(1)
		
	{
		/* window: 一般是0
		* linux: 最好设置为MSG_NOSIGNAL；表示出错后不向系统发信号，否则程序会退出！
		*/
		tmp = send(iSendSock, p, total, 0);
		if(tmp < 0)
		{
#ifdef WIN32



#else
			//当进程收到信号会中断正在进行的系统调用、区处理信号，处理完系统返回-1且errno==Eintr;
			//所以可continue继续执行
			if(errno == EINTR)
			{
				continue;
			}
			
			// Eagain表示写缓冲队列已满, usleep后继续发送
			if(errno == EAGAIN)
			{
				usleep(1000);
				continue;
			}
#endif			
			return -1;
		}
		
		if((size_t)tmp == total)
		{
			return len;
		}
		else
		{
			total -= tmp;
			p += tmp;
		}
	}
	
	return tmp;
}

/******************************************  common  *****************************************************/
int NetSetSocketNonblock(int fd)
{
#ifdef WIN32     
	int blockMode = 1;
	
	ioctlsocket(fd, FIONBIO, &blockMode);   //0--block,1--no block

#else

	int opts;

	 opts = fcntl(fd, F_GETFL); // 获得socket的属性
     if (opts < 0)
	 {
         return -1;
	 }

     opts = opts | O_NONBLOCK;

     if(fcntl(fd, F_SETFL, opts) < 0) //设置socket的属性
	 {
         return -1;
	 }

#endif

     return 1;
}


/******************************************  common  *****************************************************/
int  NetSetSocketQueue(int flag)
{
#if  1 // shangtao

    int i;

	FD_ZERO(&read_all);
		
	if(gtTcpServerSockFd > 0)
	{
	    if(gtTcpServerSockFd > giMaxFp) 
			giMaxFp = gtTcpServerSockFd; 
		
		FD_SET(gtTcpServerSockFd, &read_all);

		for (i = 0; i < MAX_CLIENT_CONN; i++)
		{
			if (gatClientConn[i] == INVALID_SOCKET)
			{
				continue ;
			}
			
			if(gatClientConn[i] > giMaxFp)
			    giMaxFp = gatClientConn[i];
			FD_SET(gatClientConn[i], &read_all);
		}
	}
	
	if(gtUdpServerSockFd > 0)
	{
	    if(gtUdpServerSockFd > giMaxFp) 
			giMaxFp = gtUdpServerSockFd; 	
		FD_SET(gtUdpServerSockFd, &read_all);
	}

#else

	struct epoll_event ev;
	int ret, i;

	ev.events = EPOLLIN;

	if(flag == 0)
	{
	}
    //将tcp server socket放到epoll上面开始侦听数据到达和connect连接
	if(gtTcpServerSockFd > 0)
	{
		ev.data.fd = gtTcpServerSockFd;
		ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, gtTcpServerSockFd, &ev);
		if(ret < 0)
		{
			return -1;
		}

		//每个新的连接都放到epoll上面开始侦听数据到达
		for (i = 0; i < MAX_CLIENT_CONN; i++)
		{
			if (gatClientConn[i] == INVALID_SOCKET)
			{
				continue ;
			}
			
			ev.data.fd = gatClientConn[i];
			ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, ev.data.fd, &ev);
			if(ret < 0)
			{
				return -1;
			}
		}
	}
	
    //将udp server socket放到epoll上面开始侦听数据到达
	if(gtUdpServerSockFd > 0)
	{
		ev.data.fd = gtUdpServerSockFd;
		ret = epoll_ctl(epoll_fd, EPOLL_CTL_ADD, ev.data.fd, &ev);
		if(ret < 0)
		{
			return -1;
		}
	}

#endif

	return 1;
}

/******************************************  common  *****************************************************/
#if 1 // shangtao
int  NetRecvMsg(char* buf, int maxLen, int *len, int *type, int *peerIp, short *peerPort)
{
	int i, size, ret;
	struct sockaddr caddr;
	struct sockaddr_in *ptAddr;
    fd_set read_set;
    struct timeval timeout;

	*type = -1;

	giMaxFp = 0;
	ret = NetSetSocketQueue(0);
	if(ret < 0)
	{
		return -1;
	}

	while (1)
    {
		read_set = read_all;
		timeout.tv_sec  = 0;
		timeout.tv_usec = 10*1000; // shangtao

        ret = select(giMaxFp+1, &read_set, NULL, NULL, &timeout);
        if (ret == 0)
        {
			break;
		}

        if (ret < 0)
        {
			return -1;
		}

        if ((gtTcpServerSockFd > 0) && FD_ISSET(gtTcpServerSockFd, &read_set))
		{
			// 有客户端connect过来
			NetConnectNewClientConn(gtTcpServerSockFd);
		}

		// udp recv
        if ((gtUdpServerSockFd > 0) && FD_ISSET(gtUdpServerSockFd, &read_set))
		{
			size = sizeof(caddr);
			i = recvfrom(gtUdpServerSockFd, buf, maxLen, 0, (struct sockaddr *)&caddr, &size);
			if(i > 0)
			{
				*len   = i; 
				ptAddr = (struct sockaddr_in *)&caddr; 
#ifdef WIN32				
				*peerIp   = ntohl(ptAddr->sin_addr.S_un.S_addr); 
#else
				*peerIp   = ntohl(ptAddr->sin_addr.s_addr); 
#endif				
				*peerPort = ntohs(ptAddr->sin_port);
				if(*peerPort == MODEM_CONTROLLER_UDP_SERVER_PORT)
				{
					*type  = MSG_FROM_MODEM_CONTROLLER;
				}
				else
				{
					*type  = MSG_FROM_NOTIFY_SERVER;
				}
				return 1;
			}
		}
	
		// 循环处理socket队列中的每个fd
		for (i = 0; i < MAX_CLIENT_CONN; i++)
		{
			if (gatClientConn[i] == INVALID_SOCKET)
			{
				continue;
			}
			
			if (!FD_ISSET(gatClientConn[i], &read_set))
			{
				continue;
			}

			//接收数据
			*len = recv(gatClientConn[i], buf, maxLen, 0);
			if(*len > 0 )
			{
				*peerIp = 0;
				*type   = MSG_FROM_QUERY_SERVER;					
				return gatClientConn[i];
			}
		}

     }

	return 0;
}

#else

int  NetRecvMsg(char* buf, int maxLen, int *len, int *type, int *peerIp, short *peerPort)
{
	int i, size, ret, n, count;
	struct sockaddr caddr;
	struct sockaddr_in *ptAddr;
    fd_set read_set;
    int  timeout;
	struct epoll_event events[MAX_EVENTS];
	*type = -1;

	ret = NetSetSocketQueue();
	if(ret < 0)
	{
		return -1;
	}

	timeout = 1000;
	while (1)
    {
		
		n = epoll_wait (epoll_fd, events, MAX_EVENTS, timeout);
		
		if(n == 0)
		{
			break;
		}
		
		for (i = 0; i < n; i++)
		{
			if ((events[i].events & EPOLLERR) ||
				(events[i].events & EPOLLHUP) ||
				(!(events[i].events & EPOLLIN)))
			{
			/* An error has occured on this fd, or the socket is not
				ready for reading (why were we notified then?) */
				//fprintf (stderr, "epoll error\n");
				//close (events[i].data.fd);
				break;
			}
			else if (gtTcpServerSockFd == events[i].data.fd)
			{
				// 有客户端connect过来
				NetConnectNewClientConn(gtTcpServerSockFd);
			}	
			else if (gtUdpServerSockFd == events[i].data.fd)
			{
				// udp recv
				size = sizeof(caddr);
				ret = recvfrom(gtUdpServerSockFd, buf, maxLen, 0, (struct sockaddr *)&caddr, &size);
				if(ret > 0)
				{
					*len   = ret; 
					*type  = MSG_FROM_MODEM_CONTROLLER;
					ptAddr = (struct sockaddr_in *)&caddr;		
					*peerIp   = ntohl(ptAddr->sin_addr.s_addr); 
					*peerPort = ntohs(ptAddr->sin_port);
					return 1;
				}
			}	
			else 
			{
				
				count = read (events[i].data.fd, buf, maxLen);
				if (count == -1)
				{
				/* If errno == EAGAIN, that means we have read all
					data. So go back to the main loop. */
					if (errno != EAGAIN)
					{
						
					}
					break;
				}
				else if (count == 0)
				{
				/* End of file. The remote has closed the
					connection. */
					break;
				}
				else
				{
					*peerIp = 0;
					*type   = MSG_FROM_QUERY_SERVER;					
					return events[i].data.fd;
				}
			} // if
		} // for	
	}

	return 0;
}

#endif
/******************************************************************************/
int  NetConnectSleep(int ms)
{
#ifdef WIN32
	Sleep(ms);
#else
	/* Linux中并没有提供系统调用sleep()，sleep()是在库函数中实现的，
	   它是通过调用alarm()来设定报警时间，调用sigsuspend()将进程挂起在信号SIGALARM上 */
	//usleep(ms*1000);
	oss_delay(ms);
#endif

	return 1;
}

/******************************************************************************/
int  NetConnectCloseSocket(int socket)
{
#ifdef WIN32
	closesocket(socket);
#else
	close(socket);
#endif

	return 1;
}

/******************************************************************************/
int  NetConnectAton(char *str)
{
#ifdef WIN32
	
	unsigned long ulAddr;

	ulAddr = inet_addr(str);

	if ( ulAddr == INADDR_NONE ) 
	{
        return -1;
    }   
    
    if (ulAddr == INADDR_ANY) 
	{
        return -1;  
    }

	return ulAddr;

#else

	struct sockaddr_in adrr;

	if(inet_aton(str, &adrr.sin_addr) < 0)
	{
		return -1;
	}

	return adrr.sin_addr.s_addr;

#endif

}

/*


*/
/******************************************************************************/
#ifdef  WIN32

#define IOC_VENDOR 0x18000000 
#define SIO_KEEPALIVE_VALS _WSAIOW(IOC_VENDOR, 4)

typedef struct TagTcpKeepAlive{
	u32 onoff;
	u32 keepalivetime;
	u32 keepaliveinterval;
} TCP_KEEPALIVE;

#endif

/******************************************************************************/
int  NetConnectSetKeepAlive(int sockfd)
{

#ifdef  WIN32

#if 0
	//KeepAlive实现
	TCP_KEEPALIVE inKeepAlive = {0}; //输入参数
	unsigned long ulInLen = sizeof(TCP_KEEPALIVE);
	
	TCP_KEEPALIVE outKeepAlive = {0}; //输出参数
	unsigned long ulOutLen = sizeof(TCP_KEEPALIVE);
	
	unsigned long ulBytesReturn = 0;
	
	//设置socket的keep alive为5秒，并且发送次数为3次
	inKeepAlive.onoff = 1;
	inKeepAlive.keepaliveinterval = 5000; //两次KeepAlive探测间的时间间隔
	inKeepAlive.keepalivetime     = 15000; //开始首次KeepAlive探测前的TCP空闭时间
	if (WSAIoctl((unsigned int)sockfd, SIO_KEEPALIVE_VALS,
		(LPVOID)&inKeepAlive, ulInLen,
		(LPVOID)&outKeepAlive, ulOutLen,
		&ulBytesReturn, NULL, NULL) == SOCKET_ERROR)
	{
		return -1;
	}

#endif

	return 1;

#else

	int keepalive = 1;     // 打开探测
	int keepidle  = 15;     // 开始探测前的空闲等待时间
	int keepintvl = 3;     // 发送探测分节的时间间隔
	int keepcnt   = 3;     // 发送探测分节的次数
			 
	if (setsockopt(sockfd, SOL_SOCKET, SO_KEEPALIVE, (void *)&keepalive, sizeof (keepalive)) < 0)
	{
		perror("fail to set SO_KEEPALIVE");
		return (-1);
	}
	
	if (setsockopt(sockfd, SOL_TCP, TCP_KEEPIDLE, (void *) &keepidle, sizeof (keepidle)) < 0)
	{
		perror("fail to set SO_KEEPIDLE");
		return (-1);
	}
	
	if (setsockopt(sockfd, SOL_TCP, TCP_KEEPINTVL, (void *)&keepintvl, sizeof (keepintvl)) < 0)
	{
		perror("fail to set SO_KEEPINTVL");
		return (-1);
	}

	if (setsockopt(sockfd, SOL_TCP, TCP_KEEPCNT, (void *)&keepcnt, sizeof (keepcnt)) < 0)
	{
		perror("fail to set SO_KEEPALIVE");
		return (-1);
	}

	return 1;

#endif
}

/******************************************************************************/
void NetConnectNewClientConn(int tcpServerSock)
{
    int i;
    struct sockaddr_in faddr;
    int addr_len = sizeof(faddr);
    int newConn;
	
	if(tcpServerSock < 0)
	{
		return;
	}
	
	while(1)
	{
		newConn = accept(tcpServerSock, (struct sockaddr *)&faddr, &addr_len);
#ifdef WIN32
		if (newConn == INVALID_SOCKET)
		{
			return;
		}
#else
		if (newConn == -1)
		{
			if ((errno == EAGAIN) ||
				(errno == EWOULDBLOCK))
			{
			/* We have processed all incoming
				connections. */
				break;
			}
			else
			{
				//perror ("accept");
				break;
			}
		}
#endif
		
		for (i = 0; i < MAX_CLIENT_CONN; i++)
		{
			if (gatClientConn[i] == INVALID_SOCKET)
			{
				gatClientConn[i] = newConn;
				break;
			}
		}
		
		if (i == MAX_CLIENT_CONN) /* 已经满了 */
		{
			NetConnectCloseSocket(newConn);
			return;
		}
	}

    return;
}

/******************************************************************************/
void NetConnectCloseClientConn(int sock)
{
    int i;

    for (i = 0; i < MAX_CLIENT_CONN; i++)
    {
        if (gatClientConn[i] == sock)
        {
            gatClientConn[i] = INVALID_SOCKET;
            break ;
        }
    }

	NetConnectCloseSocket(sock);

    return ;
}
/******************************************************************************/
int NetMacAddrGet(unsigned char *if_name, unsigned char *mac_addr)
{
#ifndef WIN32
	
	int sockfd;
    struct ifreq tmp;   

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if( sockfd < 0)
    {
        perror("create socket fail\n");
        return -1;
    }

    memset(&tmp,0,sizeof(struct ifreq));
    strncpy(tmp.ifr_name, if_name, sizeof(tmp.ifr_name)-1 );
    if( (ioctl(sockfd, SIOCGIFHWADDR, &tmp)) < 0 )
    {
        printf("mac ioctl error\n");
        return -1;
    }

    sprintf(mac_addr, "%02x-%02x-%02x-%02x-%02x-%02x",
            (unsigned char)tmp.ifr_hwaddr.sa_data[0],
            (unsigned char)tmp.ifr_hwaddr.sa_data[1],
            (unsigned char)tmp.ifr_hwaddr.sa_data[2],
            (unsigned char)tmp.ifr_hwaddr.sa_data[3],
            (unsigned char)tmp.ifr_hwaddr.sa_data[4],
            (unsigned char)tmp.ifr_hwaddr.sa_data[5]
            );
    close(sockfd);
	
#endif
	
	return RET_OK;
}