#ifndef __SCAN_CFG__
#define __SCAN_CFG__

#include <stdio.h>

#define	SCAN_DEBUG 1
#define BUFSIZE 256

#define  CFG_FILE_KEY_SPLIT   ' '
#define  CFG_FILE_VALUE_BRACE '\''

int scan_cfg_init( const FILE *sys_cfg_fp, const FILE *usr_cfg_fp);
int scan_cfg_modem_uart_port_get(int index, char *port);
int scan_cfg_modem_oper_mode_get();
int scan_cfg_modem_auto_operate_flag_get();

#endif
