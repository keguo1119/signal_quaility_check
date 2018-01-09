#ifndef __OSS_NET_H__
#define __OSS_NET_H__

int ModemControllerNetConnectInit();
int MobileRouteNetConnectInit(u16 tcpPort, u16 udpPort);
int NetConnectInit(unsigned short port);

int  NetRecvMsg(char* buf, int maxLen, int *len, int *type, int *peerIp, short *peerPort);
int  NetConnectUdpSend(int peerIpv4Addr, short port, char *buf, int bufLen);
int  NetConnectSocketSend(int iSendSock, char *buf, int len);

int  NetConnectCloseSocket(int socket);
int  NetConnectAton(char *str);
void NetConnectCloseClientConn(int sock);

int NetConnectSleep(int ms);
int NetMacAddrGet(unsigned char *if_name, unsigned char *mac_addr);

#ifdef WIN32
#define  MAX_CLIENT_CONN  (60)  // 500
#else
#define  MAX_CLIENT_CONN  (500)  // 500
#endif

#define  MAX_EVENTS       (MAX_CLIENT_CONN+2)

#endif