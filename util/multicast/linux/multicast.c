#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <errno.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdbool.h>
#include <net/if.h>
#include <netinet/in.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <ifaddrs.h>
#include <netpacket/packet.h>
#include "common.h"


int local_ip_get();
unsigned char  board_ip_str[32];
unsigned char *board_name  =  "test_dev";
unsigned char *board_type  =  "gsm-bts-2"; 
unsigned char *soft_ver    =  "V0.1.1";

unsigned char recv_buf[MAX_BUF_SIZE + 1];
/********************************************************************************/
int board_info_get(board_info_t *ptInfo)
{
	FILE *file;
	int ret;
	
	if (!ptInfo)
	{
		printf("board_info_get:ptInfo is NULL!\n");
		return -1;
	}

	memset(ptInfo, 0, sizeof(board_info_t));

	if (NULL == (file = fopen(BOARD_INFO_FILE, "r")))
	{
		printf("open %s failed!\n", BOARD_INFO_FILE);
		return -1;
	}

	if (fread(ptInfo, sizeof(board_info_t), 1, file) != 1)
	{
		printf( "board_info_get: read %s failed!\n", BOARD_INFO_FILE );
		ret = -1;
	}
	else
	{
		ret = 0;
	}

	fclose( file );
	
	return ret;
}

/********************************************************************************/
int main(int argc, char **argv)
{
	struct sockaddr_in peeraddr;
	struct sockaddr_in recvaddr;
	struct in_addr ia;
	int sockfd;
	unsigned int socklen, n;
	struct hostent *group;
	struct ip_mreq mreq;
	FILE *file;
	board_info_t board_info;

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if (sockfd < 0) 
	{
		return -1;
	}

	// add multicast route to 
    system("route add -net 224.0.0.0 netmask 224.0.0.0 eth0 &");

	/* setup multicast addr */
	bzero(&mreq, sizeof(struct ip_mreq));
	if ((group = gethostbyname(MULTICAST_ADDR)) == (struct hostent *) 0) 
	{
		printf("gethostbyname is failed");
		close(sockfd);
		return -2;
	}

	bcopy((void *) group->h_addr, (void *) &ia, group->h_length);
	bcopy(&ia, &mreq.imr_multiaddr.s_addr, sizeof(struct in_addr));

	/* setup source host ip for multicast group */
	mreq.imr_interface.s_addr = htonl(INADDR_ANY);
	if (setsockopt(sockfd, IPPROTO_IP, IP_ADD_MEMBERSHIP, &mreq, sizeof(struct ip_mreq)) == -1) 
	{
		printf("setsockopt is failed");
		close(sockfd);
		return -2;
	}

	socklen = sizeof(struct sockaddr_in);
	memset(&peeraddr, 0, socklen);
	peeraddr.sin_family = AF_INET;
	peeraddr.sin_port = htons(MULTICAST_PORT);
	if (inet_pton(AF_INET, MULTICAST_ADDR, &peeraddr.sin_addr) <= 0) 
	{
		printf(" dest IP address error!\n");
		close(sockfd);
		return -3;
	}

	if (bind(sockfd, (struct sockaddr *) &peeraddr, sizeof(struct sockaddr_in)) == -1) 
	{
		printf("Bind socket error\n");
		close(sockfd);
		return -3;
	}

	/* recv all multicast pakcet */
	while(1)
	{
		bzero(recv_buf, MAX_BUF_SIZE + 1);
		n = recvfrom(sockfd, recv_buf, MAX_BUF_SIZE, 0, (struct sockaddr *) &recvaddr, &socklen);
		if (n < 0)
		{
			printf("receive multicast packet error!\n");
			close(sockfd);
			return -4;
		}

		/* get an multicast packet */
		recv_buf[n] = 0;
		if (0 != strcmp(recv_buf, MAGIC_STR))
		{
			continue;
		}

		printf("recv multicast:%s \n from host: <%s> \n", recv_buf, inet_ntoa(recvaddr.sin_addr));
		
        memset(&board_info, 0, sizeof(board_info));
        
        local_ip_get();
            
		if (-1 == board_info_get(&board_info))
		{    
			board_info.flag 			= MAGIC_FLAG;
	
			strcpy(board_info.name, board_name);
			memcpy(board_info.ip, board_ip_str,  32);

			memset(board_info.mac, 0,  32);
			memset(board_info.id, 0, 32);	
		
			memcpy(board_info.soft_ver, soft_ver, strlen(soft_ver));
			memcpy(board_info.type, board_type, strlen(board_type));	
			memset(board_info.rsv, 0, 32);	
		}
		else
		{
			board_info.flag = htonl(MAGIC_FLAG);
			memcpy(board_info.ip, board_ip_str,  32);			
		}
		
		if (sendto(sockfd, (char *)&board_info, sizeof(board_info_t), 0, (struct sockaddr *) &peeraddr, socklen) < 0) 
		{
			printf("sendto error!\n");
		}

	}
	
	close(sockfd);
	
	return 0;
}

/********************************************************************************/
int local_ip_get()
{
	int ret;
	char *name =  "eth0";
	int family = AF_INET;	
	struct ifaddrs *ifa;
	struct sockaddr_in *sin;
	unsigned int   board_ip_addr;
	
	ret = getifaddrs(&ifa);
	if (ret != 0)
	{
		printf("[local_ip_get]get if addr failed \n");
		return -1;
	}
	
	for (ifa; ifa != NULL; ifa = ifa->ifa_next) 
	{
		if(memcmp(ifa->ifa_name, name, strlen(name) - 1) == 0)
		{
			sin = (struct sockaddr_in *)(ifa->ifa_addr);
			if (inet_ntop(ifa->ifa_addr->sa_family, (void *)&(sin->sin_addr.s_addr), board_ip_str, sizeof(board_ip_str)) == NULL)
			{
				//printf("[local_ip_get]%s: inet_ntop failed! ip=%x family=%d\n", ifa->ifa_name, sin->sin_addr.s_addr, ifa->ifa_addr->sa_family);
				continue;
			}
			else
			{
				printf("[local_ip_get]%s: %s\n", ifa->ifa_name, board_ip_str);
			}
			
			board_ip_addr = sin->sin_addr.s_addr;
			return sin->sin_addr.s_addr;
		}       
	}
	
	return 0;
}