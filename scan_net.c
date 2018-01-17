#include "oss.h"
#include "scan_net.h"
//#include "scan_file.h"
//#include "scan_modem.h"
#include <stdio.h>
#include <stdlib.h>
#include "unp.h"
#include "main.h"
#include <sys/types.h>
#include <ifaddrs.h>

static int count = 0;
int   sockfd;
char  *interface_name = "eth0";
struct sockaddr_in  cliaddr;
pthread_t net_id;
unsigned char  board_ip_str[32];
static int net_gps_lat, net_gps_lon;
///////////////////////////////////////////////////
static int local_ip_get()
{
	int ret;
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
		if(memcmp(ifa->ifa_name, interface_name, strlen(interface_name) - 1) == 0)
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
/////////////////////////////////////////////////////////////////////////
static int scan_net_recv(int sockfd, SA *pcliaddr, socklen_t clilen)
{
    socklen_t       len;
    char            mesg[MAXLINE];

    while(1) {
        len = clilen;
        Recvfrom(sockfd, mesg, MAXLINE, 0, pcliaddr, &len);
        count++;
        
//        printf("count=%d, mesg=%d\n", count, mesg);
    }
}
/////////////////////////////////////////////////////////////////////////////
/*
static int scan_net_send(int sockfd, struct sockaddr_in *pcliaddr, socklen_t clien)
{
    int n;
    socklen_t len;
    char mesg[1024];

    while(1) {
        len = clien;
        n = recvfrom(sockfd, mesg, 1024, pcliaddr, &len);
        if(n > 0) {
            printf("mesg=%d\n", mesg);
        }
    }
}
*/
/////////////////////////////////////////////////////////////////////////////
static void *scan_net_run_pthread()
{
    socklen_t       len;
    char            mesg[MAXLINE];
    gps_raw_t       *cur_gps;

    while(1) {

        Recvfrom(sockfd, mesg, MAXLINE, 0, (SA *)&cliaddr, &len);
        cur_gps = (gps_raw_t *)mesg;
        count++;
        net_gps_lat = cur_gps->lat_gps;
        net_gps_lon = cur_gps->lon_gps;
        printf("count=%d, lon=%d, lat=%d\n", count, cur_gps->lat_gps, cur_gps->lon_gps);
    }
}
/////////////////////////////////////////////////////////////////////////
int scan_net_gps_info_get(float *lat, float *lon)
{
    *lat = (float)net_gps_lat /1000000;
    *lon = (float)net_gps_lon /1000000;
}
/////////////////////////////////////////////////////////////////////////////
void scan_net_run()
{
    if(pthread_create(&net_id, NULL, (void *)scan_net_run_pthread, NULL)) {
        printf("%s: created pthread failed!\n", __func__);
    }
    printf("%s start!\n", __func__); 
}
/////////////////////////////////////////////////////////////////////////////
int scan_net_init()
{

    struct sockaddr_in      servaddr;
    int reuse = 1;

    sockfd = Socket(AF_INET, SOCK_DGRAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family      = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port        = htons(SERV_PORT + 3);

    setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    Bind(sockfd, (SA *) &servaddr, sizeof(servaddr));

//        scan_net_recv(sockfd, (SA *) &cliaddr, sizeof(cliaddr));
}

