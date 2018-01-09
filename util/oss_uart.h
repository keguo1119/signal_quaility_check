#ifndef __UART_H__
#define __UART_H__

#ifdef WIN32

#include <windows.h>

#else

#include     <stdio.h>      /*��׼�����������*/
#include     <stdlib.h>     /*��׼�����ⶨ��*/
#include     <unistd.h>     /*Unix ��׼��������*/
#include     <sys/types.h>  
#include     <sys/stat.h>   
#include     <fcntl.h>      /*�ļ����ƶ���*/
#include     <termios.h>    /*PPSIX �ն˿��ƶ���*/
#include     <errno.h>      /*����Ŷ���*/

#endif

int oss_uart_write(int fd, char * pData, int nLength); 

int oss_uart_read(int fd, char *atCmd, char * pData, int nLength); 

int oss_uart_open(const char * pPort, int nBaudRate, int nParity, int nByteSize, int nStopBits); 

#endif