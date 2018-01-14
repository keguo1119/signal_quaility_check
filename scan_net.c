#include "oss.h"
#include "scan_net.h"
//#include "scan_file.h"
//#include "scan_modem.h"
#include <stdio.h>
#include <stdlib.h>
#include "unp.h"
#include "main.h"

static int count = 0;
int   sockfd;
struct sockaddr_in  cliaddr;
pthread_t net_id;

static int scan_net_recv(int sockfd, SA *pcliaddr, socklen_t clilen)
{
    socklen_t       len;
    char            mesg[MAXLINE];

    while(1) {
        len = clilen;
        Recvfrom(sockfd, mesg, MAXLINE, 0, pcliaddr, &len);
        count++;

        printf("count=%d, mesg=%d\n", count, mesg);
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

    while(1) {
//        len ;
        Recvfrom(sockfd, mesg, MAXLINE, 0, (SA *)&cliaddr, &len);
        count++;

        printf("count=%d, mesg=%d\n", count, mesg);
    }
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

        sockfd = Socket(AF_INET, SOCK_DGRAM, 0);

        bzero(&servaddr, sizeof(servaddr));
        servaddr.sin_family      = AF_INET;
        servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
        servaddr.sin_port        = htons(SERV_PORT);

        Bind(sockfd, (SA *) &servaddr, sizeof(servaddr));

//        scan_net_recv(sockfd, (SA *) &cliaddr, sizeof(cliaddr));
}

