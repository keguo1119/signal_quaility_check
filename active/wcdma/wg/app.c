#include "oss.h"
#include "config.h"
#include "protocol_common.h"
#include "modem.h"
#include "wg_common.h"
#include "wcdma_cfg.h"
#include "wcdma_sm.h"
#include "wcdma.h"
#include "gsm_cfg.h"
#include "gsm_sm.h"
#include "gsm.h"

#ifdef WIN32
char *strConfigFilePath = "c:/lte/cfg/wgcfg.txt";
#else
char *strConfigFilePath = "/etc/lte/wgcfg.txt";
#endif

#define   SOFT_VERSION  "version 0.2.0"

typedef struct Tag_comm_para {
	char port[64];
	int nBaudRate;
	int nParity;
	int nByteSize;
	int nStopBits;

	int handle;
}uart_para_t;

uart_para_t  uart_para[10];
#define BUF_SIZE  1024

int nBaudRate = 115200;
int nParity = 0;
int nByteSize = 8;
int nStopBits = 1;

uint8_t *init_cmd1 = "1AT\r\n";
uint8_t *init_cmd2 = "1ATE0\r\n";
uint8_t *init_cmd3 = "2ATE0\r\n";
uint8_t *init_cmd4 = "3ATE0\r\n";
uint8_t *init_cmd5 = "1AT+CGSN\r\n";
uint8_t *init_cmd6 = "2AT+CGSN\r\n";
uint8_t *init_cmd7 = "3AT+CGSN\r\n";

uint8_t *gsm_cmd_init1="1AT+CLIP=1\r\n";
uint8_t *gsm_cmd_init2="1AT#MONI=7\r\n";
uint8_t *gsm_cmd_init_cmcc="1AT+COPS=1,2,\"46000\"\r\n";
uint8_t *gsm_cmd_scell_get="1AT#SERVINFO\r\n";
uint8_t *gsm_cmd_ncell_get="1AT#MONI\r\n";

uint8_t *wcdma_cmd_init1="2at+eprefmode=14\r\n";
uint8_t *wcdma_cmd_init2="2at+freqlock=1,10713\r\n";
uint8_t *wcdma_cmd_cell_get="2at+crus\r\n";

/***************************************************************************/
void uart_test();

int  app_uart_init(uart_para_t  *uart, uint8_t *port);
void app_gsm_param_init(uart_para_t  *uart);
void app_wcdma_param_init(uart_para_t  *uart);
void app_gsm_param_get(	uart_para_t  *uart);
void app_wcdma_param_get(	uart_para_t  *uart);
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

}
/***************************************************************************/

void main(int argc, char *argv[])
{
	int ret;
	int iRole = 1;  // 0, gsm; 1, wcdma

#ifndef WIN32
	struct sigaction sa;
#endif
	
#ifndef WIN32
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

//	uart_test();

	ret = oss_cfg_init(strConfigFilePath);
	if(ret < 0)
	{
		return ;
	}
	
	oss_log_init(iRole);


	Log(OS_LOG_INFO, GetVersionInfo(), "");

	if(iRole)
	{
		wcdma_init();
		wcdma_run();
	}
	else
	{
		gsm_init();
		gsm_run();
	}
}
/////////////////////////////////////////////////////////////////////
int	app_uart_write(int handle, uint8_t *cmd, int len, uint8_t *rsp, int max_size)
{
	int ret, offset = 0;
	int loop;
	uint8_t *rsp_end = "OK\r\n";
	uint8_t *err_end = "ERROR";

	printf("[send] %s ", cmd);
	ret = oss_uart_write(handle, cmd, len);
	if(ret <= 0)
	{
		return  RET_FAILED;
	}

	oss_delay(200);

	memset(rsp, 0, max_size);

	loop = 5;
	while(loop > 0)
	{
		len = oss_uart_read(handle, rsp + offset, max_size);
		if (len <= 0)
		{
			ret =  RET_FAILED;
		}
		
		if(strstr(rsp, rsp_end))
		{
			ret =  RET_OK;
			break;
		}
		if(strstr(rsp, err_end))
		{
			ret =  RET_FAILED;
			break;
		}

		offset += len;

		oss_delay(200);
		loop--;
	}

	printf("[resp] %s\n", rsp);

	return ret;
}
/////////////////////////////////////////////////////////////////////
void uart_test()
{
	const char* port = "COM53";
	uart_para_t  *uart;
	int ret, loop;

	uart = &uart_para[0];

	app_uart_init(uart, port);

	app_gsm_param_init(uart);
	app_wcdma_param_init(uart);

	loop = 3;
	while(loop > 0)
	{
		app_gsm_param_get(uart);
		app_wcdma_param_get(uart);
		loop--;
	}

	oss_uart_close(uart->handle);
}

int app_uart_init(uart_para_t  *uart, const char* port)
{
	int ret, len;
	
	uint8_t rsp[BUF_SIZE];

	strcpy(uart->port, port);
	uart->nBaudRate = nBaudRate;
	uart->nParity   = nParity;
	uart->nByteSize = nByteSize;
	uart->nStopBits = nStopBits;
	uart->handle    = 0;

	ret = oss_uart_open(uart->port, uart->nBaudRate, uart->nParity, uart->nByteSize, uart->nStopBits);
	if(ret > 0)
	{
		uart->handle = ret;
	}
	else
	{
		return  RET_FAILED;
	}

	//CMD1
	app_uart_write(uart->handle, init_cmd1, strlen(init_cmd1), rsp, BUF_SIZE);

	//CMD2
	app_uart_write(uart->handle, init_cmd2, strlen(init_cmd2), rsp, BUF_SIZE);

	//CMD3
	app_uart_write(uart->handle, init_cmd3, strlen(init_cmd3), rsp, BUF_SIZE);

	//CMD4
	app_uart_write(uart->handle, init_cmd4, strlen(init_cmd4), rsp, BUF_SIZE);

	//CMD6
	app_uart_write(uart->handle, init_cmd6, strlen(init_cmd6), rsp, BUF_SIZE);

	//CMD7
	app_uart_write(uart->handle, init_cmd7, strlen(init_cmd7), rsp, BUF_SIZE);

	//CMD5
	app_uart_write(uart->handle, init_cmd5, strlen(init_cmd5), rsp, BUF_SIZE);

	return  RET_OK;
}

void app_gsm_param_init(uart_para_t  *uart)
{
	uint8_t rsp[BUF_SIZE];

	//
	app_uart_write(uart->handle, gsm_cmd_init1, strlen(gsm_cmd_init1), rsp, BUF_SIZE);

	//
	app_uart_write(uart->handle, gsm_cmd_init2, strlen(gsm_cmd_init2), rsp, BUF_SIZE);

	//
	app_uart_write(uart->handle, gsm_cmd_init_cmcc, strlen(gsm_cmd_init_cmcc), rsp, BUF_SIZE);
}

void app_wcdma_param_init(uart_para_t  *uart)
{
	uint8_t rsp[BUF_SIZE];

	//
	app_uart_write(uart->handle, wcdma_cmd_init1, strlen(wcdma_cmd_init1), rsp, BUF_SIZE);
	//
	app_uart_write(uart->handle, wcdma_cmd_init2, strlen(wcdma_cmd_init2), rsp, BUF_SIZE);
}

void app_gsm_param_get(	uart_para_t  *uart)
{
	uint8_t rsp[BUF_SIZE];
	int loop = 5, ret;

	app_uart_write(uart->handle, gsm_cmd_scell_get, strlen(gsm_cmd_scell_get), rsp, BUF_SIZE);

	while(loop)
	{
		loop--;
		oss_delay(500);
		ret = app_uart_write(uart->handle, gsm_cmd_ncell_get, strlen(gsm_cmd_ncell_get), rsp, BUF_SIZE);
		if(ret == RET_OK)
		{
			break;
		}
	}
}

void app_wcdma_param_get(uart_para_t  *uart)
{
	uint8_t rsp[BUF_SIZE];
	int loop = 5;
	int ret;

	app_uart_write(uart->handle, wcdma_cmd_cell_get, strlen(wcdma_cmd_cell_get), rsp, BUF_SIZE);

	while(loop)
	{
		loop--;
		oss_delay(500);
		ret = app_uart_write(uart->handle, wcdma_cmd_cell_get, strlen(wcdma_cmd_cell_get), rsp, BUF_SIZE);
		if(ret == RET_OK)
		{
			break;
		}
	}	
}