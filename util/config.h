#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "oss.h"

typedef struct TagItem {
	char *key;
	char *value;
}TItem;

//////////////////////////////////////////////////////
int oss_cfg_init(const char *file);

int open_conf_file(const char *file);
int read_conf_value(int fp, const char *key, char *value);
int read_conf_value_ext(FILE *fp, const char *key, char *value, char split, char value_brace);
//////////////////////////////////////////////////////
int CfgGetUartPort(int index, char *str);
int CfgGetSca(int index, char *str);

u32 CfgGetMrMasterIp();
u32 CfgGetMrSlaveIp();
u32 CfgGetNotifyServerIp();
u16 CfgGetMrUdpPort();
u16 CfgGetMrTcpPort();
u16 CfgGetNotifyServerPort();

int CfgGetLogStdOut();
int CfgGetLogLevel();

#endif
