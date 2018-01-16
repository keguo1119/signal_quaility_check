#include "scan_cfg.h" 
#include "oss.h"
#include "config.h"
#include "protocol_common.h"
#include "modem.h"
#include <stdio.h>
#include <stdlib.h>
#include "main.h"
#include "scan_modem.h"

#define 	RELAY_TEST 1

static char *scan_sys_cfg_path = "./";
static char *scan_sys_cfg_file = "sys_cfg";
static char *scan_usr_cfg_path = "/etc/config";
static char *scan_usr_cfg_file = "scan";

extern const char *sys_cfg_path;
extern const char *sys_cfg_file;
extern const char *usr_cfg_path;
extern const char *usr_cfg_file;

/////////////////////////////////////////////
static FILE *scan_sys_cfg_fp = NULL;
static FILE *scan_usr_cfg_fp = NULL;

////////////////////////////////////////////////
int scan_cfg_usr_cfg_file_open()
{
	char file_name_buf[128];

	if(NULL == scan_usr_cfg_fp) {
		memset(file_name_buf, 0, 128);
        snprintf(file_name_buf, 128, "%s/%s", scan_usr_cfg_path, scan_usr_cfg_file);
        scan_usr_cfg_fp = (FILE *)open_conf_file(file_name_buf);
		if(NULL == scan_usr_cfg_fp) {
			printf("%s: scan_usr_cfg_fp open failed\n", __func__);
			return;
		}
	}
}
////////////////////////////////////////////////
static int scan_cfg_usr_cfg_file_close()
{
	if(NULL != scan_usr_cfg_fp) {
		fclose((FILE *)scan_usr_cfg_fp);
		scan_usr_cfg_fp = NULL;
	}
}
///////////////////////////////////////////////////////////////////////////////////////////
int scan_cfg_file_open( const FILE *sys_cfg_fp,  const FILE *usr_cfg_fp)
{
	int ret = RET_FAILED;
	char file_name_buf[128];

	//open sys cfg
    if(NULL != sys_cfg_fp)
    {
        scan_sys_cfg_fp = sys_cfg_fp;
    }
    else
    {
        snprintf(file_name_buf, 128, "%s%s", scan_sys_cfg_path, scan_sys_cfg_file);
        scan_sys_cfg_fp = (FILE *)open_conf_file(file_name_buf);
    }
	if(scan_sys_cfg_fp == NULL)
	{
#ifdef SCAN_DEBUG
	printf("scan_sys_cfg_file open failed\n");
#endif 
		Log(OS_LOG_ERR, "%s", "faile to open scan_sys_cfg  file  ");
		fclose((FILE *)scan_sys_cfg_fp);		
		return ret;
	}	

	//open usr cfg
    if (NULL != usr_cfg_fp)
    {
        scan_usr_cfg_fp = usr_cfg_fp;
    }
    else
    {
		memset(file_name_buf, 0, 128);
        snprintf(file_name_buf, 128, "%s/%s", scan_usr_cfg_path, scan_usr_cfg_file);
        scan_usr_cfg_fp = (FILE *)open_conf_file(file_name_buf);
    }

	if(scan_usr_cfg_fp == NULL)
	{
#ifdef SCAN_DEBUG
	printf("hp usr cfg file open failed\n");
#endif 	
		Log(OS_LOG_ERR, "%s", "faile to open usr cfg file  ");				
		fclose((FILE *)scan_usr_cfg_fp);
		return ret;
	}	
	
	return RET_OK;
}
///////////////////////////////////////////////////////////////////////////////////////////
int scan_cfg_modem_oper_mode_get()
{
	int ret;
	char buf[BUFSIZE];
	char rev_buf[64];
	char file_name_buf[128];
	int  mode = 0;
	
	scan_cfg_usr_cfg_file_open();

	snprintf(buf, BUFSIZE, "scan_oper_mode");

	ret = read_conf_value_ext((int ) scan_usr_cfg_fp, buf, rev_buf, CFG_FILE_KEY_SPLIT, CFG_FILE_VALUE_BRACE);
//	printf("0:%s, buf=%s, scan_usr_cfg_fp=%p\n", __func__, buf, scan_usr_cfg_fp);
	if(ret > 0) {
//		printf("%s, rev_buf=%s, mode=%d\n", __func__, rev_buf, mode);
		sscanf(rev_buf, "%d", &mode);
//		printf("%s, rev_buf=%s, mode=%d\n", __func__, rev_buf, mode);

	} else {
		return RET_FAILED;
	}

	scan_cfg_usr_cfg_file_close();

	return mode;
}
///////////////////////////////////////////////////////////////////////////////////////////
int scan_cfg_modem_auto_operate_flag_get()
{
	int ret;
	char buf[BUFSIZE];
	char rev_buf[64];
	int  flag = 0;
	char file_name_buf[128];

	scan_cfg_usr_cfg_file_open();	

	snprintf(buf, BUFSIZE, "scan_oper_auto");
	ret = read_conf_value_ext((int ) scan_usr_cfg_fp, buf, rev_buf, CFG_FILE_KEY_SPLIT, CFG_FILE_VALUE_BRACE);
	if(ret > 0) {
		sscanf(rev_buf, "%d", &flag);
//		printf("%s, rev_buf=%s, flag=%d\n", __func__, rev_buf, flag);

	} else {
		return RET_FAILED;
	}
	
	scan_cfg_usr_cfg_file_close();

	return flag;
}
///////////////////////////////////////////////////////////////////////////////////////////
int scan_cfg_modem_uart_port_get(int index, char *port)
{
	int ret;
	char buf[BUFSIZE];
	char rev_buf[64];

	if(NULL == port) {
		return RET_FAILED;
	}

	if(NULL == scan_sys_cfg_fp) {
		return RET_FAILED;
	}
	printf("%s: 0\n", __func__);
	snprintf(buf, BUFSIZE, "modem%d_uart_port", index);
	ret = read_conf_value_ext((int)scan_sys_cfg_fp,  buf, rev_buf, CFG_FILE_KEY_SPLIT,CFG_FILE_VALUE_BRACE);
	if(ret > 0) {
		printf("%s: buf=%s\n", __func__, rev_buf);
		strncpy(port, rev_buf, strlen(rev_buf)+1);	
	} else {
		return RET_FAILED;
	}

	return RET_OK;
}
///////////////////////////////////////////////////////////////////////////////////////////
int scan_cfg_init( const FILE *sys_cfg_fp, const FILE *usr_cfg_fp)
{
	printf("0-%s\n", __func__);
	scan_cfg_file_open(sys_cfg_fp, usr_cfg_fp);
	
}
