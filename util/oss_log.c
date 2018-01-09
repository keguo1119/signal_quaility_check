#include "oss.h"
#include "config.h"
#include "oss_log.h"

/*********************************************************************/
#define  MAX_LOG_BUF_LEN   (4096)
#define  MAX_LOG_DUMP_LEN  (MAX_LOG_BUF_LEN - 1024)

int  gLogBufLen;
char gabyLogBuf[MAX_LOG_BUF_LEN];

SYSTEMTIME gtLastTime;  // file created time 

int gtLogFp = 0;

int gLogLevel = OS_LOG_INFO;
int gLogStdOut = 1;
/*********************************************************************/
void CreateMultilevelPath(char *p) ;
int LogFileCreate(int isRoute);
int gIsRoute = 0;

/*********************************************************************/
HMUTEX   ghLogMutex;

int LogLockInit();
void LogLock();
void LogUnlock();
/*********************************************************************/
int oss_log_init(int isRoute)
{
	int ret;

#ifdef WIN32
	
	char *p0="c:\\mobiledetect\\log";
	char *p1="c:\\mobiledetect\\log\\controller";    
	char *p2="c:\\mobiledetect\\log\\router";     

	if(!PathFileExists(p0))
	{
		ret = mkdir(p0);
	}

	if(!PathFileExists(p1))
	{
		ret = mkdir(p1);
	}

	if(!PathFileExists(p2))
	{
		ret = mkdir(p2);
	}
	
	gLogBufLen = 0;

	gIsRoute = isRoute;
	gtLogFp = LogFileCreate(isRoute);
	if(gtLogFp == 0)
	{
		return RET_FAILED;
	}
	
#else

    openlog("", LOG_PID, LOG_USER);

#endif

	ret = LogLockInit();
	if(ret != RET_OK)
	{
		gLogLevel  = OS_LOG_ERR;
		gLogStdOut = 0;
		return RET_FAILED;
	}

	ret = CfgGetLogLevel();
	if(ret > 0)
	{
		gLogLevel = ret;
	}
	
	ret = CfgGetLogStdOut();
	if(ret > 0)
	{
		gLogStdOut = ret;
	}

	return ret;
}
/***************************************************************************************************/
int LogLockInit()
{
	MutexInit(&ghLogMutex);

	return RET_OK;
}

/***************************************************************************************************/
void LogLock()
{
	MutexLock(&ghLogMutex);
}

/***************************************************************************************************/
void LogUnlock()
{
	MutexUnLock(&ghLogMutex);
}

/*********************************************************************/
int LogFileCreate(int isRoute)
{
	char buf[128];
	int fp;
#ifdef WIN32
	char *logRouterPath="c:/mobiledetect/log/router/%d-%d-%d.txt";
	char *logCtrlPath="c:/mobiledetect/log/controller/%d-%d-%d.txt";	
#else
	char *logRouterPath="/var/log/detect/router/%d-%d-%d.txt";
	char *logCtrlPath="/var/log/detect/controller/%d-%d-%d.txt";	
#endif
	
	GetSystemTime(&gtLastTime);
	
	memset(buf, 0, 128);
	if(isRoute)
	{
		sprintf(buf, logRouterPath, gtLastTime.wYear, gtLastTime.wMonth, gtLastTime.wDay);
	}
	else
	{
		sprintf(buf, logCtrlPath, gtLastTime.wYear, gtLastTime.wMonth, gtLastTime.wDay);
	}

	fp = (int)fopen(buf, "a+");
	if(fp == 0)
	{
		return 0;
	}

	return fp;
}

/*********************************************************************/
#define  MAX_PRINT_LEN  (512)
void LogWrite(char *content, int len)
{
	SYSTEMTIME time;
	char  tmp, *p;
	int   totalLen = len;

	GetSystemTime(&time);

	if(gLogStdOut)
	{
		// to print in one screen		
		if(len < MAX_PRINT_LEN)
		{
			printf(content);
			return;
		}

		// too long to print in one screen
		p = content;
		while(len > MAX_PRINT_LEN)
		{
			tmp = p[MAX_PRINT_LEN];  // dont care utf16/wchar 
			p[MAX_PRINT_LEN] = 0;
			printf(p);
			p[MAX_PRINT_LEN] = tmp;

			len = len - MAX_PRINT_LEN;
			p = p + MAX_PRINT_LEN;
		}
		printf(p);
	}

	if(gtLogFp == 0)
	{
		return;
	}

	if(time.wDay == gtLastTime.wDay)
	{
		fwrite(content, 1, totalLen, (FILE *)gtLogFp);
		fflush((FILE *)gtLogFp);
		return;
	}

	fclose((FILE *)gtLogFp);

	gtLogFp = LogFileCreate(gIsRoute);
	if(gtLogFp == 0)
	{
		return;
	}

	fwrite(content, 1, totalLen, (FILE *)gtLogFp);
}

/*********************************************************************/
char tmpBuf[MAX_LOG_BUF_LEN];
char *logLevel[] = {
	"ERROR",
	"WARNING",
	"INFO",
	"DEBUG"
};
void LogRecord(int level, char *format, char *content, char *file, int line)
{
	int len;
	struct timeval time;

	if(level > gLogLevel)
	{
		return;
	}

	LogLock();

#if 0
	
	oss_get_cur_time(&time);

	//time + file + line
	memset(tmpBuf, 0, 512);
	sprintf(tmpBuf, "\n[%s][tick=%d, file=%s, line=%d]", logLevel[level], time.tv_sec, file, line);
	len = strlen(tmpBuf);
	memcpy(gabyLogBuf + gLogBufLen, tmpBuf, len);	
	
	gLogBufLen += len; 

	// content
	memset(tmpBuf, 0, 512);
	sprintf(tmpBuf, format, content);
	len = strlen(tmpBuf);

	if((gLogBufLen+len) >= MAX_LOG_DUMP_LEN)
	{
		// write buf to file 
		LogWrite(gabyLogBuf, gLogBufLen);
		// clear buf
		memset(gabyLogBuf, 0, gLogBufLen);
		gLogBufLen = 0;
	}
	else
	{
		memcpy(gabyLogBuf + gLogBufLen, tmpBuf, len);	
		gLogBufLen += len; 	
	}

#else

	//time + file + line
	memset(tmpBuf, 0, 512);
	sprintf(tmpBuf, "\n[%s] %s:line=%d %s %s", logLevel[level], file, line, format, content);
	
	syslog(OS_LOG_DEBUG, tmpBuf);
	
	if(gLogStdOut)
	{
		printf(tmpBuf);	
	}
#endif	
	
	LogUnlock();
}

/*********************************************************************/
void LogFlush()
{
	LogLock();

	LogWrite(gabyLogBuf, gLogBufLen);
	memset(gabyLogBuf, 0, gLogBufLen);
	gLogBufLen = 0;

	LogUnlock();
}
/*********************************************************************/
void CreateMultilevelPath(char *p) 
{
	int i,len;

#ifdef WIN32	
	len=strlen(p);
	for(i=0;i<len;i++)
	{
		if(p[i]=='\\')
		{
			p[i]='\0';
			_mkdir(p);
			p[i]='\\';
		}
	}

	if(len>0)
	{
		_mkdir(p);
	}
#endif	
}  
