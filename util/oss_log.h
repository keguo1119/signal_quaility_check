#ifndef __LOG_H__
#define __LOG_H__


typedef enum EnumLogLevel {
	OS_LOG_ERR = 0,
	OS_LOG_WARNING ,
	OS_LOG_INFO ,
	OS_LOG_DEBUG
}ELogLvel;

#ifdef WIN32

#define Log(level, format, content)  LogRecord(level, format, content, __FILE__, __LINE__)

void LogRecord(int level, char *format, char *content, char *file, int line);

void LogFlush();

#else

#include <syslog.h>

#define  Log(level, format, args...)      syslog(level, format, args)

//#define Log(level, format, args)  LogRecord(level, format, args, __FILE__, __LINE__)

#endif


int oss_log_init(int mode);


#endif