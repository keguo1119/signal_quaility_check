#ifndef  __OSS_H__
#define  __OSS_H__

//////////////////////////////////////////////////////////////////
typedef  unsigned char  u8;
typedef  unsigned short u16;
typedef  unsigned int   u32;

typedef  unsigned char  uint8_t;
typedef  unsigned short uint16_t;
typedef  unsigned int   uint32_t;

#define FALSE  0
#define TRUE   1

typedef unsigned short WCHAR;

/////////////////////////error code ////////////////////////////////////////
#define  RET_NO_CONFIG  (-2)
#define  RET_FAILED     (-1)
#define  RET_OK      (1)

typedef struct tagOssMsgHead
{
    uint16_t msg_type;
    uint16_t msg_len;
}oss_msg_head_t;
////////////////////////////////////////////////////////////

#ifdef WIN32

#include <windows.h>
#include "winsock.h"
//#include <mstcpip.h>  
//#include <winsock2.h>
//#include <ws2tcpip.h>
#include <stdio.h>
#include <errno.h>
#include <stdlib.h>
#include <string.h>
#include <direct.h> 
#include "shlwapi.h"

#include <time.h>
#include <winbase.h>

#include <fcntl.h>
#include <sys/stat.h>
#include <io.h>
#include <signal.h>
#include <sys/utime.h>

typedef HANDLE  HMUTEX;   
typedef  char  int8_t;
typedef  short int16_t;
typedef  int   int32_t;

#else

#include <sys/epoll.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdio.h>
#include <errno.h>
#include <stdbool.h>
#include <stdlib.h>
#include <strings.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <fcntl.h>
#include <syslog.h>
#include <pthread.h> 
#include <time.h>
#include <signal.h>
#include <semaphore.h>
#include <linux/input.h>
#include <linux/i2c.h>  
#include <linux/i2c-dev.h>  

#define  INVALID_SOCKET  (-1)

typedef bool  BOOL;

typedef struct tagSYSTEMTIME{
u16 wYear ; 
u16 wMonth ;
u16 wDayOfWeek ;
u16 wDay ;
u16 wHour ;
u16 wMinute ;
u16 wSecond ;
u16 wMilliseconds;
}SYSTEMTIME;

typedef pthread_mutex_t  HMUTEX;   
void GetSystemTime(	SYSTEMTIME *time);

#endif

int MutexInit(HMUTEX *mutex);
void MutexLock(HMUTEX *mutex);
void MutexUnLock(HMUTEX *mutex);
void oss_start();

#include "oss_time.h"
#include "oss_net.h"
#include "oss_log.h"
#include "oss_uart.h"

#endif