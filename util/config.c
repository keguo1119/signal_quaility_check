#include "oss.h"
#include "config.h"

/***************************************************************************************************/
HMUTEX   ghCfgMutex;
/***************************************************************************************************/
int CfgLockInit()
{
	MutexInit(&ghCfgMutex);
	
	return RET_OK;
}

/***************************************************************************************************/
void CfgLock()
{
	MutexLock(&ghCfgMutex);
}

/***************************************************************************************************/
void CfgUnlock()
{
	MutexUnLock(&ghCfgMutex);
}

////////////////////////////////////////////////////////////////////////////
/*
*去除字符串右端空格
*/
char *strtrimr(char *pstr)
{
	int i;
	i = strlen(pstr) - 1;
	while (isspace(pstr[i]) && (i >= 0))
		pstr[i--] = '\0';
	return pstr;
}
/*
*去除字符串左端空格
*/
char *strtriml(char *pstr)
{
	int i = 0,j;
	j = strlen(pstr) - 1;
	while (isspace(pstr[i]) && (i <= j))
		i++;
	if (0<i)
		strcpy(pstr, &pstr[i]);
	return pstr;
}
/*
*去除字符串两端空格
*/
char *strtrim(char *pstr)
{
	char *p;
	p = strtrimr(pstr);
	return strtriml(p);
}


/*
*从配置文件的一行读出key或value,返回item指针
*line--从配置文件读出的一行
*/
int get_item_from_line(char *line, TItem *item, char split, char value_brace)
{
	char *p = strtrim(line);
	int index, loop, len;
	char *p2;

	len = strlen(p);
	if(len <= 0)
	{
		return 1;//空行
	}
	else if(p[0]=='#')
	{
		return 2;
	}

	p2 = strchr(p, split);
	if(p2 == NULL)
	{
		return 3;
	}

	*p2 = '\0';
	p2++;

	//save entry key
	item->key = (char *)malloc(strlen(p) + 1);
	strcpy(item->key, p);
	
	//save entry value	
	len = strlen(p2);
	item->value = (char *)malloc(len + 1);
	if(0 == value_brace)
	{
		strcpy(item->value, p2);			
		return 0;
	}	
	
	//delete value_brace
	index = 0;
	loop = 0;
	item->value[0] = 0;
	while(index < len)
	{
		if(p2[index] != value_brace)
		{
			item->value[loop] = p2[index];
			loop++;
		}
		index++;
	}
	item->value[loop] = 0;

	return 0;//查询成功
}

/////////////////////////////////////////////////////////////////////////
int file_to_items(const char *file, TItem *items, int *num)
{
	char line[1024];
	FILE *fp;
	int i, len;
	char *p, *p2;

	fp = fopen(file,"r");
	if(fp == NULL)
		return 1;
	
	i = 0;
	while(fgets(line, 1023, fp))
	{
		p = strtrim(line);
		len = strlen(p);
		
		if(len <= 0)
		{
			continue;
		}
		else if(p[0]=='#')
		{
			continue;
		}
		else
		{
			p2 = strchr(p, '=');
			/*这里认为只有key没什么意义*/
			if(p2 == NULL)
				continue;
			
			*p2++ = '\0';
			items[i].key = (char *)malloc(strlen(p) + 1);
			items[i].value = (char *)malloc(strlen(p2) + 1);
			strcpy(items[i].key,p);
			strcpy(items[i].value,p2);
			
			i++;
		}
	}

	(*num) = i;
	fclose(fp);
	
	return 0;
}

/*
*读取value
*/
int open_conf_file(const char *file)
{
	FILE *fp;
	
	fp = fopen(file, "r");
	if(fp == NULL)
	{
		return -1;
	}

	return (int)fp;
}

/*******************************************************************/
int read_conf_value(int fp, const char *key, char *value)
{
	char line[1024];
	TItem item;
	fpos_t pos;
	int ret;

	if(fp < 0 )
	{
		return -1;//文件打开错误
	}

	CfgLock();
	
#ifdef WIN32	
	pos = 0;
	if( fsetpos( (FILE *)fp, &pos ) != 0 )
	{
		CfgUnlock();
		return -1; // 重新开始读取配置
	}
#else
	fseek((FILE *)fp, 0, SEEK_SET); 
#endif	

	while (fgets(line, 1023, (FILE *)fp))
	{
		ret = get_item_from_line(line, &item, '=', 0);
		if(ret != 0)
		{
			continue;
		}

		if(!strcmp(item.key, key))
		{
			strcpy(value,item.value);
			free(item.key);
			free(item.value);		

			CfgUnlock();
			return 1;
		}
		
		free(item.key);
		free(item.value);		
	}

	CfgUnlock();

	return -1;//成功
	
}
/*******************************************************************/
int read_conf_value_ext(FILE *fp, const char *key, char *value, char split, char value_brace)
{
	char line[1024];
	TItem item;
	fpos_t pos;
	int ret;
	char *p;
	
	if(fp < 0 )
	{
		return -1;//文件打开错误
	}

	CfgLock();

	fseek((FILE *)fp, 0, SEEK_SET); 

	while (fgets(line, 1023, (FILE *)fp))
	{
		p = strstr(line, key);		
		if(p == NULL)
		{
			continue;
		}

		ret = get_item_from_line(p, &item, split, value_brace);		
		if(ret != 0)
		{
			continue;
		}
		
		if(!strcmp(item.key, key))
		{
			strcpy(value,item.value);
			free(item.key);
			free(item.value);		

			CfgUnlock();
			return 1;
		}
		
		free(item.key);
		free(item.value);		
	}

	CfgUnlock();

	return -1;//成功
	
}

//////////////////////////////////////////////////////////////////////////////
int write_conf_value(const char *key, char *value, const char *file)
{
	TItem items[20];// 假定配置项最多有20个
	int num;//存储从文件读取的有效数目
	int i=0;
	FILE *fp;

	CfgLock();

	file_to_items(file, items, &num);

	//查找要修改的项
	for(i=0;i<num;i++)
	{
		if(!strcmp(items[i].key, key))
		{
			items[i].value = value;
			break;
		}
	}
	
	// 更新配置文件,应该有备份，下面的操作会将文件内容清除
	fp = fopen(file, "w");
	if(fp == NULL)
	{
		CfgUnlock();
		return 1;
	}

	i=0;
	for(i=0;i<num;i++)
	{
		fprintf(fp,"%s=%s\n",items[i].key, items[i].value);
		//printf("%s=%s\n",items[i].key, items[i].value);
	}

	fclose(fp);
	//清除工作
	/* i=0;
	for(i=0;i<num;i++){
	free(items[i].key);
	free(items[i].value);
}*/
	
	CfgUnlock();
		
	return 0;
	
}

//////////////////////////////////////////////////////////////////////////////
int gFp = -1;
int oss_cfg_init(const char *file)
{
	int ret = RET_FAILED;

	gFp = open_conf_file(file);
	if(gFp < 0)
	{
		return ret;
	}	

	ret = CfgLockInit();
	if(ret != RET_OK)
	{
		printf("config lock init failed\n");
		return ret;
	}

	return RET_OK;
}

//////////////////////////////////////////////////////////////////////////////
int CfgGetUartPort(int index, char * str)
{
	char key[128];
	int ret;

	if(gFp < 0)
	{
		return RET_FAILED;
	}

	memset(key, 0, 128);
	// check max index 
	sprintf(key, "uartPort%d", index);

	ret = read_conf_value(gFp,  key, str);
	if(ret < 0)
	{
		return RET_FAILED;
	}

	return RET_OK;
}
//////////////////////////////////////////////////////////////////////////////
int CfgGetSca(int index, char *str)
{
	char key[128];
	int ret;

	if(gFp < 0)
	{
		return RET_FAILED;
	}


	memset(key, 0, 128);
	// check max index 
	sprintf(key, "sca%d", index);

	ret = read_conf_value(gFp,  key, str);
	if(ret < 0)
	{
		return RET_FAILED;
	}

	return RET_OK;
}
//////////////////////////////////////////////////////////////////////////////
u32 CfgGetMrMasterIp()
{
	int ret;
	int ip;
	char buf[256];

	if(gFp < 0)
	{
		return 0;
	}

	ret = read_conf_value(gFp,  "masterIp", buf);
	if(ret < 0)
	{
		return 0;
	}

	ip = NetConnectAton(buf);
	if( ip == -1)
	{
		return 0;
	}

	return ip;
}

//////////////////////////////////////////////////////////////////////////////
u32 CfgGetMrSlaveIp()
{
	int ret;
	int ip;
	char buf[256];

	if(gFp < 0)
	{
		return 0;
	}

	ret = read_conf_value(gFp,  "slaveIp", buf);
	if(ret < 0)
	{
		return 0;
	}

	ip = NetConnectAton(buf);
	if( ip == -1)
	{
		return 0;
	}

	return ip;
}

//////////////////////////////////////////////////////////////////////////////
u16 CfgGetMrUdpPort()
{
	int ret;
	int port;
	char buf[256];

	if(gFp < 0)
	{
		return 0;
	}

	ret = read_conf_value(gFp,  "udpPort", buf);
	if(ret < 0)
	{
		return 0;
	}

	sscanf(buf, "%d", &port);

	return (u16)port;
}

//////////////////////////////////////////////////////////////////////////////
u16 CfgGetMrTcpPort()
{
	int ret;
	int port;
	char buf[256];

	if(gFp < 0)
	{
		return 0;
	}

	ret = read_conf_value(gFp,  "tcpPort", buf);
	if(ret < 0)
	{
		return 0;
	}

	sscanf(buf, "%d", &port);

	return (u16)port;
}
//////////////////////////////////////////////////////////////////////////////
u16 CfgGetNotifyServerPort()
{
	int ret;
	int port;
	char buf[256];

	if(gFp < 0)
	{
		return 0;
	}

	ret = read_conf_value(gFp,  "notifyServerPort", buf);
	if(ret < 0)
	{
		return 0;
	}

	sscanf(buf, "%d", &port);

	return (u16)port;
}

//////////////////////////////////////////////////////////////////////////////
u32 CfgGetNotifyServerIp()
{
	int ret;
	int ip;
	char buf[256];

	if(gFp < 0)
	{
		return 0;
	}

	ret = read_conf_value(gFp,  "notifyServerIp", buf);
	if(ret < 0)
	{
		return 0;
	}

	ip = NetConnectAton(buf);
	if( ip == -1)
	{
		return 0;
	}

	return ip;
}

//////////////////////////////////////////////////////////////////////////////
int CfgGetLogStdOut()
{
	int ret;
	int flag;
	char buf[256];

	if(gFp < 0)
	{
		return -1;
	}

	ret = read_conf_value(gFp,  "logStdOut", buf);
	if(ret < 0)
	{
		return -1;
	}

	sscanf(buf, "%d", &flag);

	return flag;
}

//////////////////////////////////////////////////////////////////////////////
int CfgGetLogLevel()
{
	int ret;
	int flag;
	char buf[256];

	if(gFp < 0)
	{
		return -1;
	}

	ret = read_conf_value(gFp,  "logLevel", buf);
	if(ret < 0)
	{
		return -1;
	}

	sscanf(buf, "%d", &flag);

	return flag;
}

