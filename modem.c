#include "oss.h"
#include "oss_time.h"
#include "config.h"
#include "protocol_common.h"
#include "modem.h"

//#define AUTO_START_SMS_ACK
/***************************************************************************************************/
#define   MAX_RESEND_COUNT        (2)
#define   MODEM_IDLE_CHECK_TIME   (10)
#define   MODEM_ALIVE_CHECK_TIME  (5) 
#define   MODEM_RESTART_TIME      (15)

#define  MAX_AT_CMD_RESP_SIZE  (1024)
char abyAtCmdRespBuf[MAX_AT_CMD_RESP_SIZE];        

int timeout_ms = 1000;

int giVirtual = 1;

#define  MAX_SEND_FAILED_NUM    (3)
#define  MAX_SMS_INDEX   (256)
#define  MAX_SMS_BUF_LEN (1024)
char strSmsBuf[MAX_SMS_BUF_LEN];     //     
/***************************************************************************************************/
TModemAtCmdStr atGsmAtCmdStr[MAX_AT_CMD_NUM] = {
/*AT_CMD_AT */	      {"AT\r", "OK", ""},
/*AT_CMD_AT_CHUP */	  {"AT+CHUP\r",        "OK", ""},
/*AT_CMD_AT_CMGS */   {"AT+CMGS=%s\r",     "\r\n>", ""},
/*AT_CMD_AT_CMGD */   {"AT+CMGD",          "OK", ""},
/*AT_CMD_AT_CMGL */   {"AT+CMGL=%d\r",     "OK", ""},	
/*AT_CMD_AT_CMGF */   {"AT+CMGF=%d\r",     "OK", ""},	
/*AT_CMD_AT_CSCA */   {"AT+CSCA",          "OK", ""},	
/*AT_CMD_AT_CREG */   {"AT+CREG",          "OK", ""},	
/*AT_CMD_AT_CIMI */   {"AT+CIMI\r",        "OK", "+CME ERROR"},	
/*AT_CMD_ATE */       {"ATE",               "OK", ""},	
/*AT_CMD_AT_CGMI */   {"AT+CGMI\r",         "OK", ""},	
/*AT_CMD_AT_CGMM */   {"AT+CGMM\r",         "OK", ""},	
/*AT_CMD_AT_CGMR */   {"AT+CGMR\r",         "OK", ""},
/*AT_CMD_AT_CNUM */   {"AT+CNUM\r",         "OK", ""},		
/*AT_CMD_AT_CMEE */   {"AT+CMEE",      "OK", ""}	,	
/*AT_CMD_ATH */       {"ATH\r",        "OK", ""}	,
/*AT_CMD_AT_CPMS */   {"AT+CPMS=\"ME\",\"SM\",\"MT\"\r",   "OK", ""},	
/*AT_CMD_AT_CNMI */   {"AT+CNMI=0,0,0,0,1\r",   "OK", ""}	
};

TModemAtCmdStr atCdmaAtCmdStr[MAX_AT_CMD_NUM];
/***************************************************************************************************/
#ifdef WIN32
DWORD WINAPI modem_thread(LPVOID lpPara);
#else
void  * modem_thread(void *lpPara);
#endif

int modem_hw_init(TModem *ptModem);
int modem_is_active(TModem *ptModem);
//int modem_echo(TModem *ptModem, int on);


int modem_phone_num_get(TModem *ptModem);
int modem_manufacture_get(TModem *ptModem);
int modem_hw_get(TModem *ptModem);
//int modem_imsi_get(TModem *ptModem);
//int modem_version_get(TModem *ptModem);
int modem_info_get(TModem *ptModem);

int  modem_at_cmd_fail_proc(TModem *ptModem);
/***************************************************************************************************/
void	modem_status_set(TModem *ptModem, 	int status)
{
	oss_get_cur_time(&ptModem->tCheckTime);
	ptModem->iStatus = status;
}
/***************************************************************************************************/
int modem_init(TModem *ptModem)
{
	int ret;
	int nBaudRate = 9600; 
	int nParity   = 0;
	int nByteSize = 8; 
	int nStopBits = 1;

	ptModem->nBaudRate = nBaudRate; 
	ptModem->nParity   = nParity;
	ptModem->nByteSize = nByteSize; 
	ptModem->nStopBits = nStopBits;

	memset(&ptModem->tMsgQueue, 0, sizeof(ptModem->tMsgQueue));

	// get modem  uart port
	ret = CfgGetUartPort(ptModem->index, ptModem->abyUartPort);
	if(ret != RET_OK )
	{
		return RET_NO_CONFIG;
	}

	// modem init
	ret = modem_hw_init(ptModem);
	if(ret != RET_OK)
	{
		Log(OS_LOG_DEBUG, "modem hardware init failed: %s", ptModem->abyUartPort);
		return -1;
	}

	return 1;
}
/***************************************************************************************************/
int  modem_hw_init(TModem *ptModem)
{
	int ret = 0;

	ptModem->iStatus = MODEM_IDLE;
	
	// ensure modem is present
	ret = modem_open(ptModem); 
	if(ret != RET_OK)
	{
		Log(OS_LOG_DEBUG, "modem open failed:%s", ptModem->abyUartPort);
		return RET_FAILED;
	}
	ptModem->iStatus = MODEM_OPENED;

	modem_error_mode(ptModem, 2);

	ret = modem_info_get(ptModem);
	if(ret != RET_OK)
	{
		Log(OS_LOG_ERR, "modem info get failed", "");
		return RET_FAILED;
	}

	// ensure sim card is present
	ret = modem_imsi_get(ptModem);
	if(ret != RET_OK)
	{
		oss_get_cur_time(&ptModem->tCheckTime);
		Log(OS_LOG_ERR, "modem imsi get failed", "");
		return RET_FAILED;
	}

	// active in network
	ret = modem_is_active(ptModem);
	if(ret != RET_OK)
	{
		Log(OS_LOG_ERR, "modem active in network failed", "");
		return RET_FAILED;
	}

	ptModem->iStatus = MODEM_REG_IN_NET;

	return RET_OK;
}

/***************************************************************************************************/
int Modem_reset(TModem *ptModem)
{
	int ret;
	int tryCount = 1;
	char atCmd[] = "AT+CRESET\r";  //for SIM7100C
	char atCmdOkResp[] = "OK";

	if(ptModem->fd == 0)
	{
		return RET_FAILED;
	}

	while(tryCount)
	{
		tryCount--;
		/* send AT and resp ok */
		ret = modem_at_cmd_send(ptModem, atCmd);
		if(ret != RET_OK)
		{
			continue;
		}

		oss_delay(1000*10);
		ret = modem_at_cmd_wait_rsp(ptModem, atCmdOkResp, NULL);
		if(ret != RET_OK)
		{
			continue;
		}
		else
		{
			return RET_OK;
		}
	}

	modem_at_cmd_fail_proc(ptModem);

	return RET_FAILED;
}

/***************************************************************************************************/
int ModemCheck(TModem *ptModem)
{
	int ret;
	int tryCount = 5;

	if(ptModem->fd == 0)
	{
		return RET_FAILED;
	}

	while(tryCount)
	{
		tryCount--;
		/* send AT and resp ok */
		ret = modem_at_cmd_send(ptModem, atGsmAtCmdStr[AT_CMD_AT].atCmd);
		if(ret != RET_OK)
		{
			continue;
		}
		
		ret = modem_at_cmd_wait_rsp(ptModem, atGsmAtCmdStr[AT_CMD_AT].atCmdOkResp, NULL);
		if(ret != RET_OK)
		{
			continue;
		}
		else
		{
			return RET_OK;
		}
	}

	//modem_at_cmd_fail_proc(ptModem);

//	Log(OS_LOG_ERR, "modem AT cmd check failed: %s", ptModem->abyUartPort);

	return RET_FAILED;
}
/***************************************************************************************************/
int modem_info_get(TModem *ptModem)
{
	modem_hw_get(ptModem);

	modem_manufacture_get(ptModem);

	modem_version_get(ptModem);

	modem_phone_num_get(ptModem);

	return RET_OK;
}

/***************************************************************************************************/
int modem_open(TModem *ptModem)
{
	int ret;

	/* open uart port */
	ret = oss_uart_open((const char*)ptModem->abyUartPort, ptModem->nBaudRate, ptModem->nParity, ptModem->nByteSize, ptModem->nStopBits);
	if(ret < 0)
	{
		printf("oss_uart_open failed, ret=%d!, %s, %d, %d, %d, %d\n", ret, (const char*)ptModem->abyUartPort, ptModem->nBaudRate, ptModem->nParity, ptModem->nByteSize, ptModem->nStopBits);
		return RET_FAILED;
	}
	ptModem->fd = ret;
	
	ret = ModemCheck(ptModem);
	if(ret != RET_OK)
	{
		printf("ModemCheck failed!\n");
		ModemClose(ptModem);
		return RET_FAILED;
	}

	ret = modem_echo(ptModem, 0);
	if(ret != RET_OK)
	{
		printf("modem_echo failed!\n");
		ModemClose(ptModem);
		return RET_FAILED;
	}

	return RET_OK;
}
/***************************************************************************************************/
int ModemClose(TModem *ptModem)
{
	int tmp;
	TPlmn  tPlmn;
	char  buf[MAX_UART_PORT_LEN];

	tmp = ptModem->index;
	memcpy(buf, ptModem->abyUartPort, MAX_UART_PORT_LEN);
	memcpy(&tPlmn, &ptModem->tPlmn, sizeof(TPlmn));

#ifdef WIN32
	CloseHandle((FILE *)ptModem->fd);
#else
	close(ptModem->fd);
#endif

	memset(ptModem, 0, sizeof(TModem));

	memcpy(&ptModem->tPlmn, &tPlmn, sizeof(TPlmn));
	memcpy(ptModem->abyUartPort, buf, MAX_UART_PORT_LEN);
	ptModem->index = tmp;

	modem_status_set(ptModem, MODEM_IDLE);

	return RET_OK;
}
/***************************************************************************************************/
int modem_echo(TModem *ptModem, int on)
{
	/* get imsi */
	int ret;
	char buf[128];

	memset(buf, 0, 128);
	sprintf(buf, "%s%d\r", atGsmAtCmdStr[AT_CMD_ATE].atCmd, on);

	ret = modem_at_cmd_send(ptModem, buf);
	if(ret != RET_OK)
	{
		Log(OS_LOG_ERR, "modem AT cmd ATE send failed", "");
		return RET_FAILED;
	}

	ret = modem_at_cmd_wait_rsp(ptModem, atGsmAtCmdStr[AT_CMD_ATE].atCmdOkResp, NULL);
	if(ret != RET_OK)
	{
		return RET_FAILED;
	}

	return RET_OK;
}
/***************************************************************************************************/
int modem_imsi_get(TModem *ptModem)
{
	/* get imsi */
	int ret;
	char buf[128];
	int tryCount = 5;
	memset(buf, 0, 128);

	while(tryCount)
	{
		oss_delay(1000);

		tryCount--;
		ret = modem_at_cmd_send(ptModem, atGsmAtCmdStr[AT_CMD_AT_CIMI].atCmd);
		if(ret != RET_OK)
		{
			continue;
		}
		
		ret = modem_at_cmd_wait_rsp(ptModem, atGsmAtCmdStr[AT_CMD_AT_CIMI].atCmdOkResp, buf);
		if(ret != RET_OK)
		{
			continue;	
		}
		
		ptModem->tPlmn.isValid = 1;
		memcpy(ptModem->tPlmn.abyMcc, buf+2, 3);
		memcpy(ptModem->tPlmn.abyMnc, buf+5, 2);
		
		memcpy(ptModem->abyImsi, buf+2, MAX_IMSI_LEN);
		ptModem->abyImsi[MAX_IMSI_LEN-1] = 0;
		
		Log(OS_LOG_ERR, "modem is ok: %s ", ptModem->abyUartPort);
		Log(OS_LOG_ERR, "modem imsi: %s ", buf+2);

		return RET_OK;
	}

	Log(OS_LOG_ERR, "get modem imsi failed: %s ", ptModem->abyUartPort);

	return RET_FAILED;
}

/***************************************************************************************************/
int modem_is_active(TModem *ptModem)
{
	/* reg on the network */

/*
锟斤拷锟斤拷锟斤拷应(Response):

 +CREG :<mode>,<stat> [,<lac>,<ci>]

锟斤拷锟斤拷说锟斤拷:

 <mode>
 0 : 锟斤拷止锟斤拷锟斤拷注锟斤拷锟斤拷锟斤拷锟结供锟斤拷锟斤拷锟斤拷耄拷锟斤拷锟斤拷茫锟�
 1 : 锟斤拷锟斤拷锟斤拷锟斤拷注锟斤拷锟斤拷锟斤拷锟结供锟斤拷锟斤拷锟斤拷耄�+CREG锟斤拷<stat>
 2 : 锟斤拷锟斤拷锟斤拷锟斤拷注锟斤拷锟轿伙拷锟斤拷锟较拷锟斤拷锟斤拷锟斤拷锟斤拷锟�+CREG: <stat>[,<lac>,<ci>]

 0锟斤拷未注锟结，锟秸端碉拷前锟斤拷未锟斤拷锟斤拷寻锟铰碉拷锟斤拷营锟斤拷
 1锟斤拷锟斤拷注锟结本锟斤拷锟斤拷锟斤拷
 2锟斤拷未注锟结，锟秸讹拷锟斤拷锟斤拷锟斤拷寻锟斤拷站
 4锟斤拷未知锟斤拷锟斤拷
 5锟斤拷锟斤拷注锟结，锟斤拷锟斤拷锟斤拷锟斤拷状态	
*/
	char buf[128];
	int ret, tmp;

	memset(buf, 0, 128);

	sprintf(buf, "%s?\r", atGsmAtCmdStr[AT_CMD_AT_CREG].atCmd);
	ret = modem_at_cmd_send(ptModem, buf);
	if(ret != RET_OK)
	{
		return RET_FAILED;
	}

	memset(buf, 0, 128);
	ret = modem_at_cmd_wait_rsp(ptModem, atGsmAtCmdStr[AT_CMD_AT_CREG].atCmdOkResp, buf);
	if(ret != RET_OK)
	{
		return RET_FAILED;
	}

	sscanf(buf, "\r\n+CREG: %d,%d", &tmp, &ret);
	if((ret != 1) && (ret != 5))
	{
		Log(OS_LOG_ERR, "modem reg in net failed: %s", buf);
		return RET_FAILED;
	}

	Log(OS_LOG_INFO, "modem reg in net: %s", buf);

	return RET_OK;
}

/***************************************************************************************************/
int ModemDeactive(TModem *ptModem)
{
	/* unreg on the network */
//	int ret;


	return 0;
}
/***************************************************************************************************/
int	ModemTextSmsSend(TModem *ptModem, char *destPhoneNo, char *smsContent)
{
	// setup txt mode
	int ret;
	char buf[256];
	char abySendBuf[256];

	memset(abySendBuf, 0, 256);
	//AT+CMGS="+15127752607"<CR>
	//	>This is a test. 
	sprintf(abySendBuf, atGsmAtCmdStr[AT_CMD_AT_CMGS].atCmd, destPhoneNo);

	//send cmd
	ret = modem_at_cmd_send(ptModem, abySendBuf);
	if(ret != RET_OK)
	{
		return RET_FAILED;
	}

	//wait >
	memset(buf, 0, 256);
	ret = modem_at_cmd_wait_rsp(ptModem, atGsmAtCmdStr[AT_CMD_AT_CMGS].atCmdOkResp, buf);
	if(ret != RET_OK)
	{
		Log(OS_LOG_ERR, "send sms cmd error", buf);
		return RET_FAILED;
	}
	
	// send sms content
	memset(abySendBuf, 0, 256);
	sprintf(abySendBuf, "%s%s", smsContent, "\x01a");
	ret = modem_at_cmd_send(ptModem, abySendBuf);
	if(ret != RET_OK)
	{
		return RET_FAILED;
	}

	//wait "OK"
	memset(buf, 0, 256);
	ret = modem_at_cmd_wait_rsp(ptModem, atGsmAtCmdStr[AT_CMD_AT].atCmdOkResp, buf);
	if(ret != RET_OK)
	{
		Log(OS_LOG_ERR, "send sms content error", buf);
		return RET_FAILED;
	}

	return RET_OK;
}

/***************************************************************************************************/
int	ModemWaitOk()
{
	return 0;
}

/***************************************************************************************************/
int ModemSmsQueue(TModem *ptModem, char *abyMsIsdn, int len, char *strContent, int silent)
{
	int ret;

	ret = ModemMsgPut(&ptModem->tMsgQueue, abyMsIsdn, len, strContent, silent);

	return ret;
}

/*************************************************************************************************/
int modem_hw_get(TModem *ptModem)
{
	int ret;
	char buf[256];

	/* send AT+CGMI */
	ret = modem_at_cmd_send(ptModem, atGsmAtCmdStr[AT_CMD_AT_CGMI].atCmd);
	if(ret != RET_OK)
	{
		return RET_FAILED;
	}
	
	memset(buf, 0, 256);
	ret = modem_at_cmd_wait_rsp(ptModem, atGsmAtCmdStr[AT_CMD_AT_CGMI].atCmdOkResp, buf);
	if(ret != RET_OK)
	{
		return RET_FAILED;
	}
	Log(OS_LOG_DEBUG, "modem manufacture: %s", buf);

	return RET_OK;
}

/*************************************************************************************************/
int modem_manufacture_get(TModem *ptModem)
{
	int ret;
	char buf[256];

	/* send AT+CGMM */
	ret = modem_at_cmd_send(ptModem, atGsmAtCmdStr[AT_CMD_AT_CGMM].atCmd);
	if(ret != RET_OK)
	{
		return RET_FAILED;
	}
	
	memset(buf, 0, 256);
	ret = modem_at_cmd_wait_rsp(ptModem, atGsmAtCmdStr[AT_CMD_AT_CGMM].atCmdOkResp, buf);
	if(ret != RET_OK)
	{
		return RET_FAILED;
	}
	Log(OS_LOG_DEBUG, "modem module: %s", buf);

	return RET_OK;
}
/*************************************************************************************************/
int modem_version_get(TModem *ptModem)
{
	int ret;
	char buf[256];
	char * pData;

	/* send AT+CGMR */
	ret = modem_at_cmd_send(ptModem, atGsmAtCmdStr[AT_CMD_AT_CGMR].atCmd);
	if(ret != RET_OK)
	{
		return RET_FAILED;
	}
	
	memset(buf, 0, 256);
	ret = modem_at_cmd_wait_rsp(ptModem, atGsmAtCmdStr[AT_CMD_AT_CGMR].atCmdOkResp, buf);
	if(ret != RET_OK)
	{
		return RET_FAILED;
	}
	else 
	{
		//+CGMR: 4534B07SIM7100C_160421
		pData = strstr(buf, "+CGMR: ");
	    if (pData != NULL) 
		{
			pData += strlen("+CGMR: ");
			sscanf(pData, "%s", ptModem->abyVer);
		} 
	}
	Log(OS_LOG_DEBUG, "modem module version: %s", buf);

	return RET_OK;
}
/*************************************************************************************************/
int modem_phone_num_get(TModem *ptModem)
{
	char buf[256];
    int  ret;        

	memset(buf, 0, 256);

	/* send AT+CNUM */
	ret = modem_at_cmd_send(ptModem, atGsmAtCmdStr[AT_CMD_AT_CNUM].atCmd);
	if(ret != RET_OK)
	{
		return RET_FAILED;
	}
	
	memset(buf, 0, 256);
	ret = modem_at_cmd_wait_rsp(ptModem, atGsmAtCmdStr[AT_CMD_AT_CNUM].atCmdOkResp, buf);
	if(ret != RET_OK)
	{
		//return RET_FAILED;
	}
	Log(OS_LOG_DEBUG, "modem module phone no: %s", buf);

	return RET_OK;
}
/*************************************************************************************************/
int modem_error_mode(TModem *ptModem, int index)
{
    char cmd[64];         
	char buf[256];
    int  ret;        

	memset(buf, 0, 256);

	sprintf(cmd, "%s=%d\r", atGsmAtCmdStr[AT_CMD_AT_CMEE].atCmd, index);

	ret = modem_at_cmd_send(ptModem, cmd);
	if(ret != RET_OK)
	{
		return RET_FAILED;
	}

	ret = modem_at_cmd_wait_rsp(ptModem, atGsmAtCmdStr[AT_CMD_AT_CMEE].atCmdOkResp, buf);
	Log(OS_LOG_DEBUG, "modem module phone no: %s", buf);
	if(ret != RET_OK)
	{
		return RET_FAILED;
	}

	return RET_OK;
}
/*************************************************************************************************/
int ModemCallHangUp(TModem *ptModem)
{
	int  ret;

	//AT+CHUP 
	ret = modem_at_cmd_send(ptModem, atGsmAtCmdStr[AT_CMD_AT_CHUP].atCmd);
	if(ret != RET_OK)
	{
		return RET_FAILED;
	}

    // OK
	ret = modem_at_cmd_wait_rsp(ptModem, atGsmAtCmdStr[AT_CMD_AT_CHUP].atCmdOkResp, NULL);
	if(ret != RET_OK)
	{
		return RET_FAILED;
	}

	return RET_OK;
}

/*************************************************************************************************/
int modem_at_cmd_send(TModem *ptModem, char *atCmd)
{
    char cmd[256];       
    int ret;

	if(ptModem->fd <= 0 )
	{
		return  RET_FAILED;
	}

	memset(cmd, 0, 256);
    sprintf(cmd, "%s", atCmd);
	//oss_delay(1000);
	ret = oss_uart_write(ptModem->fd, cmd, strlen(cmd));
	if(ret > 0)
	{
		return  RET_OK;
	}

	return  RET_FAILED;

}

/*************************************************************************************************/
int modem_at_cmd_wait_rsp(TModem *ptModem, char *atCmd, char *pbyOut)
{
	int  len;

	if(ptModem->fd <= 0)
	{
		return RET_FAILED;		
	}

    memset(abyAtCmdRespBuf, 0, MAX_AT_CMD_RESP_SIZE );

    oss_delay(10);

	len = oss_uart_read(ptModem->fd, atCmd, abyAtCmdRespBuf, MAX_AT_CMD_RESP_SIZE); 	
	
	
	
	if (len <= 0)
	{
		return RET_FAILED;
	}

	if(ModemIsRestart(abyAtCmdRespBuf))
	{
		modem_status_set(ptModem, MODEM_RESTART);
		return RET_FAILED;
	}		

	if(pbyOut)
	{
		memcpy(pbyOut, abyAtCmdRespBuf, len);
	}
	
	if(NULL == strstr(abyAtCmdRespBuf, atCmd))
	{
		return RET_FAILED;
	}

	return RET_OK;
}
/***************************************************************************************************/
int  modem_at_cmd_fail_proc(TModem *ptModem)
{
	ptModem->byFailedCount++;
	if(ptModem->byFailedCount > MAX_SEND_FAILED_NUM)
	{
		ModemClose(ptModem);
	}

	return RET_OK;
}

int modem_atCmd_w_r(TModem *ptModem, char *atCmd, char *atCmdResp, char *pbyOut)
{
	int ret;
	int tryCount = 3;

	while(tryCount)
	{
		tryCount--;
		ret = modem_at_cmd_send(ptModem, atCmd);
		if(ret != RET_OK)
		{
			continue;
		}
		
		ret = modem_at_cmd_wait_rsp(ptModem, atCmdResp, pbyOut);
		if(ret != RET_OK)
		{
			continue;	
		}
		else 
		{
		    return RET_OK; 
		}
	}

    return ret;
}

/*************************************************************************************************/
int  ModemStart(TModem *ptModem)
{
	int  ret;
#ifdef WIN32		
	HANDLE hThread;
#else
	pthread_t mythread;
#endif	
	u32 dwThreadID;

	ret = modem_init(ptModem);		
	if(ret == RET_NO_CONFIG)
	{
		return -1;
	}

#ifdef WIN32	
	hThread = CreateThread(NULL, 0, modem_thread, (LPVOID)ptModem, 0, &dwThreadID);
	if(NULL == hThread)
	{
		Log(OS_LOG_ERR, "modem thread create failed: %s", ptModem->abyUartPort);
		return -1;
	}
#else
    if ( pthread_create( &mythread, NULL, modem_thread, (void *)ptModem) )
    {
        printf("error creating thread.%s", ptModem->abyUartPort);
        abort();
    } 
#endif	

	return 1;
}
/*************************************************************************************************/
#ifdef WIN32
DWORD WINAPI modem_thread(LPVOID lpPara)
#else
void * modem_thread(void* lpPara)
#endif
{
	int prevStat;
	TModem *ptModem = (TModem *)lpPara;

	while(1)
	{
		prevStat = ModemStatusGet(ptModem);
		
		ModemRun(ptModem);

		// ready to idle: modem failed to resp 
		// ready to open: fail to reg in net 
		if(prevStat == MODEM_READY)
		{
			if(ModemIsStatus(ptModem, MODEM_IDLE) || ModemIsStatus(ptModem, MODEM_OPENED))
			{
				ptModem->iStatusChange = READY_TO_IDLE; 
			}
		}

		// idle to pre-ready : restart modem init ok
		if(ModemIsStatus(ptModem, MODEM_ACK_ENABLED))
		{
			ptModem->iStatusChange = IDLE_TO_READY;
		}
	}

#ifdef WIN32
	return 1;
#else	
	return NULL;
#endif	
}
/*************************************************************************************************/
int  ModemRun(TModem *ptModem)
{
	int ret;
	struct timeval time;

	switch(ptModem->iStatus)
	{
	case MODEM_IDLE:
		// period check . when modem is idle
		oss_get_cur_time(&time);
		if((time.tv_sec - ptModem->tCheckTime.tv_sec) < MODEM_IDLE_CHECK_TIME)
		{
			oss_delay(500);  // 
			return RET_OK;
		}

		ptModem->tCheckTime.tv_sec = time.tv_sec;
		ptModem->tCheckTime.tv_usec = time.tv_usec;

		// hardware restart
		ret = modem_init(ptModem);
		if(ret < 0)
		{
			return RET_FAILED;
		}
	break;

	case MODEM_READY:
		if(ptModem->iStatus == MODEM_RESTART)
		{
			// stop check and wait for init
			ptModem->byFailedCount = 0;
			break;  
		}

		if(ptModem->iStatus == MODEM_IDLE)
		{
			// sending failed and turning to idle state 
			break;  
		}

		// period check . when modem is idle
		oss_get_cur_time(&time);
		if((time.tv_sec - ptModem->tCheckTime.tv_sec) < MODEM_ALIVE_CHECK_TIME)
		{
			return RET_OK;
		}

		ptModem->tCheckTime.tv_sec = time.tv_sec;
		ptModem->tCheckTime.tv_usec = time.tv_usec;

		ModemCheck(ptModem); // check modem alive?
	break;

	case MODEM_RESTART:
		oss_get_cur_time(&time);
		if((time.tv_sec - ptModem->tCheckTime.tv_sec) < MODEM_RESTART_TIME)
		{
			oss_delay(500);
			return RET_OK;
		}

		// software restart
		Log(OS_LOG_ERR, "modem restart to init: %s", ptModem->abyUartPort);
		// reinitialize modem
		ModemClose(ptModem);
		
		ret = modem_init(ptModem);
	break;

	case MODEM_REG_IN_NET:
	case MODEM_OPENED:
		oss_get_cur_time(&time);
		if((time.tv_sec - ptModem->tCheckTime.tv_sec) < MODEM_RESTART_TIME)
		{
			oss_delay(500);
			return RET_OK;
		}

		// software restart
		Log(OS_LOG_ERR, "modem opened to init: %s", ptModem->abyUartPort);
		// reinitialize modem
		ModemClose(ptModem);
		
		ret = modem_init(ptModem);
	break;

	default:
		oss_delay(500);
		break;
	}

	return RET_OK;
}
/*************************************************************************************************/
int  ModemIsRestart(char *buf)
{
	if(strstr(buf, "SYSSTART"))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
/*************************************************************************************************/
int  ModemIsRing(char *buf)
{
	if(strstr(buf, "RING"))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}
/*************************************************************************************************/
int  ModemIsStatus(TModem *ptModem, int status)
{
	if(ptModem->iStatus == status)
	{
		return 1;
	}
	else
	{
		return 0;
	}	
}

/*************************************************************************************************/
int  ModemStatusGet(TModem *ptModem)
{
	return ptModem->iStatus;
}
/*************************************************************************************************/
int  ModemIsStatusChange(TModem *ptModem, int status)
{
	if(ptModem->iStatusChange == status)
	{
		return 1;
	}
	else
	{
		return 0;
	}	
}
/*************************************************************************************************/
void  ModemStatusChangeClear(TModem *ptModem)
{
	ptModem->iStatusChange = 0;
}

/***************************************************************************************/
int  ModemMsgPut(TMsgQueue *ptMsgQueue, char *abyMsIsdn, int len, char *strContent, int silent)
{
	TMsgItem * ptItem;
	
	if(ptMsgQueue->dwGetIndex == (ptMsgQueue->dwPutIndex + 1) % MAX_MSG_NUM)
	{
		return -1;
	}

	ptItem = &ptMsgQueue->atItem[ptMsgQueue->dwPutIndex];

	// 
	ptItem->byLen = len;
	memcpy((unsigned char *)ptItem->abyMsIsdn, abyMsIsdn, len);
	ptItem->abyMsIsdn[len] = 0;

	if(strContent)
	{
		strcpy(ptItem->strContent, strContent);
	}
	else
	{
		ptItem->strContent[0] = 0;
	}

	ptItem->iSilent = silent;

	//move pointer
	ptMsgQueue->dwPutIndex = (ptMsgQueue->dwPutIndex + 1) % MAX_MSG_NUM;

	return 1;
}
/************************************************************************************/
TMsgItem * ModemMsgGet(TMsgQueue *ptMsgQueue)
{
	TMsgItem * ptItem;

	if(ptMsgQueue->dwGetIndex != ptMsgQueue->dwPutIndex)
	{
		ptItem = &ptMsgQueue->atItem[ptMsgQueue->dwGetIndex];
		ptMsgQueue->dwGetIndex++;
		ptMsgQueue->dwGetIndex = ptMsgQueue->dwGetIndex % MAX_MSG_NUM;
		return ptItem;
	}

	return NULL;
}
/*************************************************************************************************/
char  abyDumpBuf[2048];
void ModemDumpStat(TModem *ptModem)
{

#if 1 ///shangtao

	memset(abyDumpBuf, 0, 2048);
	ModemDumpStatBuf(ptModem, abyDumpBuf, 2048);
	Log(OS_LOG_INFO, abyDumpBuf, "");

#else

	Log(OS_LOG_INFO, "\n", "");
	Log(OS_LOG_INFO, "\n %s=%s ", VAR_NAME(abyUartPort), ptModem->abyUartPort);
	Log(OS_LOG_INFO, "\n %s=%d ", VAR_NAME(iStatus), ptModem->iStatus);

	Log(OS_LOG_INFO, "\n %s=%d ", VAR_NAME(iSendSilentSms), ptModem->iSendSilentSms);
	Log(OS_LOG_INFO, "\n %s=%d ", VAR_NAME(iRecvSilentAck), ptModem->iRecvSilentAck);
	Log(OS_LOG_INFO, "\n %s=%d ", VAR_NAME(iRecvSilentNack), ptModem->iRecvSilentNack);

	Log(OS_LOG_INFO, "\n %s=%d ", VAR_NAME(bySendCount), ptModem->bySendCount);
	Log(OS_LOG_INFO, "\n %s=%d ", VAR_NAME(byFailedCount), ptModem->byFailedCount);
	Log(OS_LOG_INFO, "\n %s=%c %c %c", VAR_NAME(tPlmn.abyMcc), ptModem->tPlmn.abyMcc[0], ptModem->tPlmn.abyMcc[1], ptModem->tPlmn.abyMcc[2]);
	Log(OS_LOG_INFO, "\n %s=%c %c", VAR_NAME(tPlmn.abyMnc), ptModem->tPlmn.abyMnc[0], ptModem->tPlmn.abyMnc[1]);

#endif

}

/*************************************************************************************************/
void ModemDumpStatBuf(TModem *ptModem, char *buf, int len)
{

#if 1 /// shangtao
	char tmpBuf[128];
	
	memset(tmpBuf, 0, 128);
	sprintf(tmpBuf, "\n ---------- modem stat ----------");
	strcat(buf, tmpBuf);

	memset(tmpBuf, 0, 128);
	sprintf(tmpBuf, "\n %s=%s ", VAR_NAME(abyUartPort), ptModem->abyUartPort);
	strcat(buf, tmpBuf);

	memset(tmpBuf, 0, 128);
	sprintf(tmpBuf, "\n %s=%d ", VAR_NAME(iStatus), ptModem->iStatus);
	strcat(buf, tmpBuf);

	memset(tmpBuf, 0, 128);	
	sprintf(tmpBuf, "\n %s=%d ", VAR_NAME(iSendSilentSms), ptModem->iSendSilentSms);
	strcat(buf, tmpBuf);

	memset(tmpBuf, 0, 128);
	sprintf(tmpBuf, "\n %s=%d ", VAR_NAME(iRecvSilentAck), ptModem->iRecvSilentAck);
	strcat(buf, tmpBuf);
	
	memset(tmpBuf, 0, 128);
	sprintf(tmpBuf, "\n %s=%d ", VAR_NAME(iRecvSilentNack), ptModem->iRecvSilentNack);
	strcat(buf, tmpBuf);

	memset(tmpBuf, 0, 128);
	sprintf(tmpBuf, "\n %s=%d ", VAR_NAME(bySendCount), ptModem->bySendCount);
	strcat(buf, tmpBuf);

	memset(tmpBuf, 0, 128);
	sprintf(tmpBuf, "\n %s=%d ", VAR_NAME(byFailedCount), ptModem->byFailedCount);
	strcat(buf, tmpBuf);

	memset(tmpBuf, 0, 128);
	sprintf(tmpBuf, "\n %s=%c %c %c", VAR_NAME(tPlmn.abyMcc), ptModem->tPlmn.abyMcc[0], ptModem->tPlmn.abyMcc[1], ptModem->tPlmn.abyMcc[2]);
	strcat(buf, tmpBuf);

	memset(tmpBuf, 0, 128);
	sprintf(tmpBuf, "\n %s=%c %c", VAR_NAME(tPlmn.abyMnc), ptModem->tPlmn.abyMnc[0], ptModem->tPlmn.abyMnc[1]);
	strcat(buf, tmpBuf);

#else

	Log(OS_LOG_INFO, "\n", 0);
	Log(OS_LOG_INFO, "\n %s=%s ", VAR_NAME(abyUartPort), ptModem->abyUartPort);
	Log(OS_LOG_INFO, "\n %s=%d ", VAR_NAME(iStatus), ptModem->iStatus);

	Log(OS_LOG_INFO, "\n %s=%d ", VAR_NAME(iSendSilentSms), ptModem->iSendSilentSms);
	Log(OS_LOG_INFO, "\n %s=%d ", VAR_NAME(iRecvSilentAck), ptModem->iRecvSilentAck);
	Log(OS_LOG_INFO, "\n %s=%d ", VAR_NAME(iRecvSilentNack), ptModem->iRecvSilentNack);

	Log(OS_LOG_INFO, "\n %s=%d ", VAR_NAME(bySendCount), ptModem->bySendCount);
	Log(OS_LOG_INFO, "\n %s=%d ", VAR_NAME(byFailedCount), ptModem->byFailedCount);
	Log(OS_LOG_INFO, "\n %s=%c %c %c", VAR_NAME(tPlmn.abyMcc), ptModem->tPlmn.abyMcc[0], ptModem->tPlmn.abyMcc[1], ptModem->tPlmn.abyMcc[2]);
	Log(OS_LOG_INFO, "\n %s=%c %c", VAR_NAME(tPlmn.abyMnc), ptModem->tPlmn.abyMnc[0], ptModem->tPlmn.abyMnc[1]);

#endif

}
