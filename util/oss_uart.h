#ifndef __UART_H__
#define __UART_H__

#ifdef WIN32

#include <windows.h>

#else

#include     <stdio.h>      /*标准输入输出定义*/
#include     <stdlib.h>     /*标准函数库定义*/
#include     <unistd.h>     /*Unix 标准函数定义*/
#include     <sys/types.h>  
#include     <sys/stat.h>   
#include     <fcntl.h>      /*文件控制定义*/
#include     <termios.h>    /*PPSIX 终端控制定义*/
#include     <errno.h>      /*错误号定义*/

#endif

int oss_uart_write(int fd, char * pData, int nLength); 

int oss_uart_read(int fd, char *atCmd, char * pData, int nLength); 

int oss_uart_open(const char * pPort, int nBaudRate, int nParity, int nByteSize, int nStopBits); 

#endif