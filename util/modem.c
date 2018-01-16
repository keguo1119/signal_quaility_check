/*************************************************************************************************/
#include "oss.h"
#include "oss_time.h"
#include "oss_uart.h"


/////////////////////////////////////////////////////////////////////////////
int oss_uart_set_parity(int fd, int databits, int stopbits, char parity);
/////////////////////////////////////////////////////////////////////////////
#ifndef  WIN32
sem_t uart_read_res_sem;
#endif

/////////////////////////////////////////////////////////////////////////////
int oss_uart_get_stop_bit(int nStopBits)
{
	switch(nStopBits)
	{
	case 1:
		return 0;

	case 15:
		return 1;

	case 2:
		return 2;

	default:
		return -1;
	}
}
/////////////////////////////////////////////////////////////////////////////
int oss_uart_open(const char* pPort, int nBaudRate, int nParity, int nByteSize, int nStopBits)
{
#ifdef  WIN32

    DCB dcb;        
	HANDLE fd;
	int error;
	char buf[128];
	BOOL  ret;

    COMMTIMEOUTS timeouts = {    
        100,        //  100 ms      
        1,          //  1 ms        9600/8=1200, 1ms per char
        500,        //  500 ms
        1,          //  1 ms        9600/8=1200, 1ms per char
        100};       //  100 ms
   
	memset(buf, 0, 128);
	sprintf(buf, "\\\\.\\%s", pPort);
    fd = CreateFile(buf,   
            GENERIC_READ | GENERIC_WRITE,    
            0, 
			NULL,
			OPEN_EXISTING,   
            0,               
            NULL);           
   
    if(fd == INVALID_HANDLE_VALUE) 
	{
		error = GetLastError(); 
		return -1;        
	}

    ret = GetCommState(fd, &dcb);        
	if(FALSE == ret)
	{
		return -1;
	}
	
	dcb.DCBlength= sizeof(DCB);
    dcb.BaudRate = nBaudRate;
    dcb.ByteSize = nByteSize;
    dcb.Parity   = nParity;
    dcb.StopBits = oss_uart_get_stop_bit(nStopBits);
	dcb.fOutX    = 1;
	dcb.fInX     = 1;

    ret = SetCommState(fd, &dcb);        
	if(FALSE == ret)
	{
		return -1;
	}
   
    ret = SetupComm(fd, 4096, 1024);     // input buffer and output buffer
	if(FALSE == ret)
	{
		return -1;
	}

    ret = SetCommTimeouts(fd, &timeouts);    
	if(FALSE == ret)
	{
		return -1;
	}
	
    return (int)fd;

#else

	int fd, ret; /* File descriptor for the port */
	struct termios options;
	char cParity;
    char err_buf[128];
	
    /* ttyS0 is reserved for system use */
	fd = open(pPort, O_RDWR | O_NOCTTY | O_NDELAY);
	if (fd == -1)
	{
        snprintf(err_buf, 128, "open_port: Unable to open %s ", pPort);
		perror(err_buf);
		return -1;
	}

	// setup speed
	tcgetattr(fd, &options);      
    //cfsetispeed(&options, B9600);
    //cfsetospeed(&options, B9600);
    tcflush(fd, TCIOFLUSH);
    switch( nBaudRate )
    {
    case 2400:
        cfsetispeed(&options, B2400);
        cfsetospeed(&options, B2400);
        break;
    case 4800:
        cfsetispeed(&options, B4800);
        cfsetospeed(&options, B4800);
        break;
    case 9600:
        cfsetispeed(&options, B9600);
        cfsetospeed(&options, B9600);
        break;
    case 19200:
        cfsetispeed(&options, B19200);
        cfsetospeed(&options, B19200);
        break;		
    case 38400:
        cfsetispeed(&options, B38400);
        cfsetospeed(&options, B38400);
        break;				
    case 115200:
        cfsetispeed(&options, B115200);
        cfsetospeed(&options, B115200);
        break;
    default:
		return -1;
    }
	
    ret = tcsetattr(fd, TCSANOW, &options);
    if(ret != 0 )
    {     
       perror("tcsetattr error");
	   close(fd);
       return -1;
    }
	
    tcflush(fd,TCIOFLUSH);
	//fcntl(fd, F_SETFL, FNDELAY);  // immediately return
	fcntl(fd, F_SETFL, 0);
	
	/* get the current options */
	tcgetattr(fd, &options);
	
	/* set raw input, 1 second timeout */
	options.c_cflag     |= (CLOCAL | CREAD);
	options.c_lflag     &= ~(ICANON | ECHO | ECHOE | ISIG); /* Choosing raw input */
	options.c_oflag     &= ~OPOST;  /* Choosing raw output */
	options.c_cc[VMIN]  = 0;
	/* options.c_cc[VTIME] = 10; */ //100ms
	options.c_cc[VTIME] = 0;
	
	/* set the options */
	tcsetattr(fd, TCSANOW, &options);

    switch( nParity )
    {
    case 0:
        cParity = 'N';
        break;
    case 1:
        cParity = 'O';
        break;
    case 2:
        cParity = 'E';
        break;
    case 3:
        cParity = 'S';
        break;
    default:
        fprintf(stderr, "Unsupported parity.\n");
        return -1;
    }
	
	ret = oss_uart_set_parity(fd, nByteSize, nStopBits, cParity);
	if(ret == -1)
	{
		close(fd);
		return -1;
	}
	
    //extern int sem_init __P ((sem_t *__sem, int __pshared, unsigned int __value));
	//pshared is 0, means it only share for all thread
	sem_init(&uart_read_res_sem,0,1);  //initial resource number is 1
	
	return (fd);

#endif
}
   
/////////////////////////////////////////////////////////////////////////////
int oss_uart_close(int fd)
{
#ifdef WIN32
    return CloseHandle((HANDLE)fd);
#else
	close(fd);
	return;
#endif
}
   
/////////////////////////////////////////////////////////////////////////////
int oss_uart_write(int fd, char * pData, int nLength)
{
#ifdef WIN32
    COMSTAT ComStat;
    DWORD dwErrorFlags;
    DWORD dwNumWrite;    
    BOOL  ret;

    ClearCommError((HANDLE)fd, &dwErrorFlags, &ComStat);
    ret = WriteFile((HANDLE)fd, pData, (DWORD)nLength, &dwNumWrite, NULL);
    PurgeComm((HANDLE)fd, PURGE_RXABORT | PURGE_TXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR);
	return ret;

#else

	int nByte;
    sem_wait(&uart_read_res_sem);  //sem_wait will be blocked until sem>0
	nByte = write(fd, pData, nLength);
	sem_post(&uart_read_res_sem);
	if(nByte > 0)
	{
		return nByte;
	}
	else
	{
		tcflush(fd, TCOFLUSH); 
		return -1;
	}

#endif
}
   
/////////////////////////////////////////////////////////////////////////////
int oss_uart_read(int fd, char *atCmd, char * pData, int nLength)
{
#ifdef WIN32
    COMSTAT ComStat;
    DWORD dwErrorFlags;
    DWORD dwNumRead;    
    BOOL  ret;

    ClearCommError((HANDLE)fd, &dwErrorFlags, &ComStat);
	nLength = ComStat.cbInQue;
	if (nLength <= 0) 
	{
		printf("fd:%d, no data in serial port when reading, \n", fd);
		return 0;
	}
	
    ret = ReadFile((HANDLE)fd, pData, (DWORD)nLength, &dwNumRead, NULL);
	if (!ret) 
	{
		printf("fd:%d, serial port read failed, \n", fd);
		return 0;
	} 
	
    return (int)dwNumRead;

#else
    int readByte = 0; 
    int len = 0; 
	int i;
	int cnt = 0;
	char read_buf[1024];
	
    sem_wait(&uart_read_res_sem);  //sem_wait will be blocked until sem>0
	bzero(read_buf, sizeof(read_buf));
	while(1) 
	{
	    if( (readByte = read(fd, read_buf, sizeof(read_buf))) > 0 )
		{
			for(i = len; i < (len + readByte); i++)
			{
			    pData[i] = read_buf[i - len];
			}
			len += readByte;
			//if "OK" or "ERROR" is read, break
			if (strstr(pData, atCmd) != NULL) break;
			if (strstr(pData, "ERROR") != NULL)
			{
				len = 0;
				tcflush(fd, TCIFLUSH); 
			    break;
			}
		}
		//printf("[oss_uart_read] atCmd: %s, readByte:%d \n", atCmd, readByte);
		cnt++;
		/* if (1)no data is read; (2)"OK" or "ERROR" isn't found in data, 
		   when cnt is up to 32, exit from loop */
		if (cnt > 31) break; 
		oss_delay(5); 
	}
	pData[len] = '\0'; 
    sem_post(&uart_read_res_sem);
	return len; //len>0, means data is read from uart port
	//readByte = read(fd, pData, nLength);
	//return readByte;

#endif
}


/////////////////////////////////////////////////////////////////////////////
int oss_uart_set_parity(int fd, int databits, int stopbits, char parity)
{
#ifdef WIN32

	return 1;

#else

    struct termios Opt;
    if(tcgetattr(fd, &Opt) != 0)
    {
        perror("tcgetattr fd");
        return FALSE;
    }
    Opt.c_cflag |= (CLOCAL | CREAD);        //涓€閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷风帿璋嬮敓琛楋拷

    switch(databits)        //閿熸枻鎷烽敓鏂ゆ嫹閿熸枻鎷烽敓鏂ゆ嫹浣嶉敓鏂ゆ嫹
    {
    case 7:
        Opt.c_cflag &= ~CSIZE;
        Opt.c_cflag |= CS7;
        break;
    case 8:
        Opt.c_cflag &= ~CSIZE;
        Opt.c_cflag |= CS8;
        break;
    default:
        fprintf(stderr, "Unsupported data size.\n");
        return -1;
    }

    switch(parity)            //閿熸枻鎷烽敓鏂ゆ嫹鏍￠敓鏂ゆ嫹浣�
    {
    case 'n':
    case 'N':
        Opt.c_cflag &= ~PARENB;        //閿熸枻鎷烽敓鍙綇鎷烽敓杞匡拷
        Opt.c_iflag &= ~INPCK;        //enable parity checking
        break;
    case 'o':
    case 'O':
        Opt.c_cflag |= PARENB;        //enable parity
        Opt.c_cflag |= PARODD;        //閿熸枻鎷锋牎閿熸枻鎷�
        Opt.c_iflag |= INPCK;         //disable parity checking
        break;
    case 'e':
    case 'E':
        Opt.c_cflag |= PARENB;        //enable parity
        Opt.c_cflag &= ~PARODD;        //鍋舵牎閿熸枻鎷�
        Opt.c_iflag |= INPCK;            //disable pairty checking
        break;
    case 's':
    case 'S':
        Opt.c_cflag &= ~PARENB;        //閿熸枻鎷烽敓鍙綇鎷烽敓杞匡拷
        Opt.c_cflag &= ~CSTOPB;        //??????????????
        Opt.c_iflag |= INPCK;            //disable pairty checking
        break;
    default:
        fprintf(stderr, "Unsupported parity.\n");
        return -1;    
    }

    switch(stopbits)        //閿熸枻鎷烽敓鏂ゆ嫹鍋滄浣�
    {
    case 1:
        Opt.c_cflag &= ~CSTOPB;
        break;
    case 2:
        Opt.c_cflag |= CSTOPB;
        break;
    default:
        fprintf(stderr, "Unsupported stopbits.\n");
        return -1;
    }

/*
    opt.c_cflag |= (CLOCAL | CREAD);

    opt.c_lflag &= ~(ICANON | ECHO | ECHOE | ISIG);
 
    opt.c_oflag &= ~OPOST;
    opt.c_oflag &= ~(ONLCR | OCRNL);    
 
    opt.c_iflag &= ~(ICRNL | INLCR);
    opt.c_iflag &= ~(IXON | IXOFF | IXANY);    

    tcflush(fd, TCIFLUSH);
    Opt.c_cc[VTIME] = 0;        //閿熸枻鎷烽敓鐭鎷锋椂
    Opt.c_cc[VMIN]  = 0;        //Update the Opt and do it now
*/
    Opt.c_cflag &= ~CRTSCTS; /* Disable hardware flow control */
    Opt.c_iflag &= ~(IXON | IXOFF | IXANY);  /* Disable software flow control */
	tcflush(fd, TCIFLUSH);

    if(tcsetattr(fd, TCSANOW, &Opt) != 0)
    {
        perror("tcsetattr fd");
        return -1;
    }

    return 1;

#endif
}

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

    oss_delay(1000);

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
	int tryCount = 5;

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
