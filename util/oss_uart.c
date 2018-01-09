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
    Opt.c_cflag |= (CLOCAL | CREAD);        //一锟斤拷锟斤拷锟斤拷玫谋锟街�

    switch(databits)        //锟斤拷锟斤拷锟斤拷锟斤拷位锟斤拷
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

    switch(parity)            //锟斤拷锟斤拷校锟斤拷位
    {
    case 'n':
    case 'N':
        Opt.c_cflag &= ~PARENB;        //锟斤拷锟叫ｏ拷锟轿�
        Opt.c_iflag &= ~INPCK;        //enable parity checking
        break;
    case 'o':
    case 'O':
        Opt.c_cflag |= PARENB;        //enable parity
        Opt.c_cflag |= PARODD;        //锟斤拷校锟斤拷
        Opt.c_iflag |= INPCK;         //disable parity checking
        break;
    case 'e':
    case 'E':
        Opt.c_cflag |= PARENB;        //enable parity
        Opt.c_cflag &= ~PARODD;        //偶校锟斤拷
        Opt.c_iflag |= INPCK;            //disable pairty checking
        break;
    case 's':
    case 'S':
        Opt.c_cflag &= ~PARENB;        //锟斤拷锟叫ｏ拷锟轿�
        Opt.c_cflag &= ~CSTOPB;        //??????????????
        Opt.c_iflag |= INPCK;            //disable pairty checking
        break;
    default:
        fprintf(stderr, "Unsupported parity.\n");
        return -1;    
    }

    switch(stopbits)        //锟斤拷锟斤拷停止位
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
    Opt.c_cc[VTIME] = 0;        //锟斤拷锟矫筹拷时
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