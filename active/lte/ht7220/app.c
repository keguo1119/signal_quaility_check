#include "oss.h"
#include "config.h"
#include "protocol_common.h"
#include "modem.h"
#include "lte_cfg.h"
#include "lte_sm.h"
#include "lte.h"

#ifdef WIN32
char *strConfigFilePath = "c:/lte/cfg/ltecfg.txt";
#else
char *strConfigFilePath = "/usr/local/lte/workspace/lteapp/ltecfg.txt";
#endif

#define   SOFT_VERSION  "version 0.2.0"

/***************************************************************************/
char strSoftVersion[256];
char *GetVersionInfo()
{
	char szTmpDate[40]={0};

	memset(strSoftVersion, 0, 256);

    sprintf(strSoftVersion,"%s: create at ", SOFT_VERSION); 

    //获取编译日期、时间
    sprintf(szTmpDate,"%s  ",__DATE__); 
	strcat(strSoftVersion, szTmpDate);

    sprintf(szTmpDate,"%s",__TIME__); 
	strcat(strSoftVersion, szTmpDate);

	return strSoftVersion;
}
/***************************************************************************/
void PrintUsage()
{
	printf("using this program as following: \n");
	printf("mobiledetect -c   \
            or                \
		    mobiledetect -r   \
			mobiledetect -v   \
			-c to start modem controller  \
			-r to start modem router \
			-v to print soft version \n\n");
}
/***************************************************************************/
void main(int argc, char *argv[])
{
	int ret;
	int iRole = 1;

#ifndef WIN32
	struct sigaction sa;
    int save_fd, fd;
    fflush(stdout);
    setvbuf(stdout,NULL,_IONBF,0);
	save_fd = dup(STDOUT_FILENO); // create a duplicate handle for standard output
	fd = open("applog.txt", (O_RDWR | O_CREAT), 0644);
	dup2(fd,STDOUT_FILENO); //用我们新打开的文件描述符替换掉'标准输出'
	close(fd);
	
    //simcom initial
	FDDtoTDDInit();
    SimCtrlInit();
	oss_delay(2000); //2s
	
	sa.sa_handler = SIG_IGN;//设定接受到指定信号后的动作为忽略
	sa.sa_flags = 0;
	ret = sigemptyset(&sa.sa_mask);
	if ( ret == -1 ) //初始化信号集为空
	{
		perror("failed to clear SIGPIPE; sigaction");
		exit(EXIT_FAILURE);
	}

	//屏蔽SIGPIPE信号
	ret = sigaction(SIGPIPE, &sa, 0);
	if( ret == -1) 
	{ 
		perror("failed to ignore SIGPIPE; sigaction");
		exit(EXIT_FAILURE);
	}	
#endif

	oss_timer_init();

	ret = oss_cfg_init(strConfigFilePath);
	if(ret < 0)
	{
		return ;
	}
	
	//oss_log_init(iRole);
	//Log(OS_LOG_INFO, GetVersionInfo(), "");
    	
	ret = lte_init();
    if(ret < 0)
	{
		return ;
	}
	
	lte_run();
	
#ifndef WIN32
    dup2(save_fd,STDOUT_FILENO); //再恢复回来标准输出
#endif
}
