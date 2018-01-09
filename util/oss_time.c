#include "oss.h"
#include "oss_time.h"

#ifndef  WIN32
#define SIGMYTIMER (SIGRTMAX) 
sem_t bin_sem; 
#endif

volatile uint32_t gdwTick = 0;
oss_timer_t   g_timer[MAX_TIMER_CALLBACK];

oss_timer_t   g_timer_event[MAX_TIMER_EVENT];

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
uint32_t oss_current_tick (void)
{
	return gdwTick;
}
/////////////////////////////////////////////////////////////////////////
int oss_timeout(unsigned int start_time, unsigned int hold_time)
{
	unsigned int cur_time = oss_current_tick();

	if((start_time + hold_time) > cur_time)
	{
		return 0;
	}
	
	return 1;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
void oss_reg_timer_proc(int time, void (*func)(char *data, int len), char *data) 
{    
	int loop;

	for(loop=0; loop < MAX_TIMER_CALLBACK; loop++)
	{
		if(g_timer[loop].func == NULL)
		{
			g_timer[loop].interval = time;
			g_timer[loop].lefttime = 0;
			g_timer[loop].dwData   = (unsigned int)data;
			g_timer[loop].func     = func;
		}
	}
}
/*====================================================================

====================================================================*/
int oss_set_timer(int time,  void (*func)(char *data, int len),  uint32_t dwEvent)
{    
	int loop;

	for(loop=0; loop < MAX_TIMER_EVENT; loop++)
	{
		if(g_timer_event[loop].state == OSS_TIMER_IDLE)
		{
			g_timer_event[loop].interval	= time;
			g_timer_event[loop].lefttime	= time;
			g_timer_event[loop].dwEvent		= dwEvent;
			g_timer_event[loop].func		= func;
			g_timer_event[loop].dwData		= 0;
			g_timer_event[loop].state		= OSS_TIMER_BUSY;
			return loop;
		}
	}

	printf("[oss_set_timer]fault settimer failed\n");
	return -1;
}

/*====================================================================

====================================================================*/
int oss_set_timer_ex(int time,	 void (*func)(char *data, int len),  uint32_t dwEvent, uint32_t dwData)
{    
	int loop;

    //set again before the timer expiry
    for(loop=0; loop < MAX_TIMER_EVENT; loop++)
	{
		if( (g_timer_event[loop].state == OSS_TIMER_BUSY) &&  
			(g_timer_event[loop].dwEvent == dwEvent) && 
			(g_timer_event[loop].lefttime > 100)
		  )
		{
			g_timer_event[loop].lefttime	= time;
			return loop;
		}
	}
	//idle
	for(loop=0; loop < MAX_TIMER_EVENT; loop++)
	{
		if(g_timer_event[loop].state == OSS_TIMER_IDLE)
		{
			g_timer_event[loop].interval	= time;
			g_timer_event[loop].lefttime	= time;
			g_timer_event[loop].dwEvent		= dwEvent;
			g_timer_event[loop].func		= func;
			g_timer_event[loop].dwData		= dwData;
			g_timer_event[loop].state		= OSS_TIMER_BUSY;
			return loop;
		}
	}

	return -1;
}

/*=======================================================================*/
void oss_del_timer(uint32_t tid, int time, uint32_t dwEvent)
{    
	int loop;

	for(loop=0; loop < MAX_TIMER_EVENT; loop++)
	{
		if(g_timer_event[loop].state == OSS_TIMER_BUSY)
		{
			if ( (g_timer_event[loop].tid == tid) && (g_timer_event[loop].dwEvent == dwEvent))
			{
				g_timer_event[loop].tid			= 0;
				g_timer_event[loop].func        =  NULL;
				g_timer_event[loop].dwEvent     = 0; 
				g_timer_event[loop].interval	= 0;
				g_timer_event[loop].state		= OSS_TIMER_IDLE;
				break;
			}
		}
	}
}

/*====================================================================

====================================================================*/
void oss_del_timer_ex(/*uint32_t tid, int time,*/ uint32_t dwEvent)
{    
	int loop;

	for(loop=0; loop < MAX_TIMER_EVENT; loop++)
	{
		if(g_timer_event[loop].state == OSS_TIMER_BUSY)
		{
			if ( /*(g_timer_event[loop].tid == tid) &&*/ (g_timer_event[loop].dwEvent == dwEvent))
			{
				g_timer_event[loop].tid			= 0;
				g_timer_event[loop].func        =  NULL;
				g_timer_event[loop].dwEvent     = 0; 
				g_timer_event[loop].interval	= 0;
				g_timer_event[loop].state		= OSS_TIMER_IDLE;
				break;
			}
		}
	}
}

/*====================================================================

====================================================================*/
uint8_t   byTimerBuf[sizeof(oss_msg_head_t) + sizeof(msg_timeout_t) + 256];

void oss_event_scan() 
{    
	int loop;
	oss_msg_head_t *ptMsgHdr = (oss_msg_head_t *)byTimerBuf;
	msg_timeout_t *ptTimeoutMsg = (msg_timeout_t *)(ptMsgHdr + 1);

	for(loop=0; loop < MAX_TIMER_EVENT; loop++)
	{
		if(g_timer_event[loop].state == OSS_TIMER_BUSY)
		{
			if(g_timer_event[loop].lefttime > 100)
			{
				g_timer_event[loop].lefttime -= 100;
			}
			else
			{
				if(g_timer_event[loop].func != NULL)
				{
					ptMsgHdr->msg_type  = g_timer_event[loop].dwEvent;
					ptMsgHdr->msg_len   = sizeof(oss_msg_head_t) + sizeof(msg_timeout_t);
					ptTimeoutMsg->dwData = g_timer_event[loop].dwData;
					(g_timer_event[loop].func)((char *)ptMsgHdr, ptMsgHdr->msg_len);
				}
				g_timer_event[loop].func     = NULL;
				g_timer_event[loop].dwEvent  = 0; 
				g_timer_event[loop].interval = 0;
				g_timer_event[loop].state	 = OSS_TIMER_IDLE;
			}
		}
	}
}
/*====================================================================

  ====================================================================*/
void oss_timer_scan()  
{   
	int loop;
    
#ifdef WIN32

	MSG   msg; 
	if(!GetMessage(&msg,0,0,0)) 
	{ 
		return;
	} 

    gdwTick += 100;

#else

    int ret;
    struct timespec time;
    time.tv_sec  = 0;
    time.tv_nsec = 10 * (1000*1000); /* 10ms */   
//	while ((ret = sem_timedwait(&bin_sem, &time)) == -1 && errno == EINTR)
	while ((ret = sem_wait(&bin_sem)) == -1 && errno == EINTR)
	{
        continue;   	
	}
	
    gdwTick += 20;

#endif
    
	for(loop=0; loop < MAX_TIMER_CALLBACK; loop++)
	{
		if(g_timer[loop].func != NULL)
		{
			if(gdwTick % g_timer[loop].interval != 0)
			{
			    continue;	
			}
			
			(g_timer[loop].func)((char *)g_timer[loop].dwData, 0);
		}
	}
	
	if(gdwTick % 100 == 0)
	{
		oss_event_scan();
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifdef WIN32
void CALLBACK oss_timer_signal(HWND hWnd, uint32_t msgId, uint32_t tid, uint32_t dwTime)
{
	printf("get timer msg \n");
}
#else
void oss_timer_signal(int signo, siginfo_t* info, void* uc) 
{
//    struct timeval tv;
	if (signo != SIGMYTIMER) return; 
	
//	oss_get_cur_time(&tv);
//	printf("current time,sec:usec:%d:%d\n", tv.tv_sec, tv.tv_usec);
//    printf("%s\n", (char *)(info->si_value.sival_ptr));    
    sem_post(&bin_sem);
}
#endif

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
int oss_timer_init()
{    

#ifdef WIN32
	
	int res = 0; 
	res = SetTimer(0, 1, 100, NULL);//(TIMERPROC )oss_timer_signal); 
	printf("[oss_timer_init]Oss SetTimer result ok: %d\n", res);
	
#else
    int ret;
    timer_t nTimerID;
	struct sigaction sysact;
	struct sigevent evp; 
    struct itimerspec value; 
	struct itimerspec ovalue; 
	char *pPara = "abcd";
	 
	/* SIGALRM的中断信号会终止sleep,因为sleep就是用SIGALRM信号量实现的 */
	//signal(SIGALRM, oss_timer_signal);   
	
	//setup signal handler 
	sigemptyset(&sysact.sa_mask); 
	sysact.sa_flags = SA_SIGINFO; //if set sa_flags=0, use sysact.sa_handler to setup signal handler
	sysact.sa_sigaction = oss_timer_signal ; 
	sigaction(SIGMYTIMER, &sysact, NULL); 

	memset(&evp, 0, sizeof(struct sigevent));
    // setup the timer
	evp.sigev_notify = SIGEV_SIGNAL; 
    evp.sigev_signo = SIGMYTIMER; /* 当定时器到期，内核会将sigev_signo所指定的信号SIGMYTIMER传送给进程, 
	   如果evp为NULL，那么定时器到期会产生默认的信号，对 CLOCK_REALTIMER来说，默认信号就是SIGALRM 
	*/
    evp.sigev_value.sival_ptr = pPara; //parameter pointer to be pass into handler func: oss_timer_signal()
    ret = timer_create(CLOCK_REALTIME, &evp, &nTimerID); 
	if(ret != 0)
	{
	    printf("timer_create() failed, \n");
		return -1;
	}
	
	value.it_value.tv_sec = 0; 
	value.it_value.tv_nsec = 20 * (1000 * 1000);  //20ms
	value.it_interval.tv_sec = value.it_value.tv_sec; 
	value.it_interval.tv_nsec = value.it_value.tv_nsec; 
	if (timer_settime(nTimerID, 0, &value, &ovalue) == 0) //success 
	{ 
		printf("set period timer ok\n");
	} 
	else
	{
		printf("set period timer failed\n");
		return -1;
	}

    //semaphore initialization
	if( sem_init(&bin_sem,0,0) != 0 )
    {
    	printf("[oss_timer_init]sem_init \n");
    	return -1;
	}

#endif

    memset(&g_timer[0], 0, sizeof(oss_timer_t) * MAX_TIMER_CALLBACK);
	memset(&g_timer_event[0], 0, sizeof(oss_timer_t) * MAX_TIMER_EVENT);
	
	return 1;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
int oss_delay (uint32_t msec)
{
#ifdef WIN32
    Sleep(msec);
#else
    struct timespec timeout0;
    struct timespec timeout1;
    struct timespec* tmp;
    struct timespec* t0 = &timeout0;
    struct timespec* t1 = &timeout1;
 
    t0->tv_sec = msec / 1000;
    t0->tv_nsec = (msec % 1000) * (1000 * 1000);
    /* 
    int nanosleep(const struct timespec *req,struct timespec *rem);
    这个函数功能是暂停某个进程直到你规定的时间后恢复，参数req就是你要暂停的时间
    由于调用nanosleep是使进程进入TASK_INTERRUPTIBLE，这就意味着有可能会没有等到你规定的时间就因为其它信号而唤醒，
    此时函数返回-1，且剩余的时间会被记录在rem中.
    这个函数的实现是：将其状态设置成TASK_INTERRUPTIBLE，脱离就绪队列，
    然后进行一次进程调度再由内核在规定的时间后发送信号来唤醒这个进程。
   */ 
    while(nanosleep(t0, t1) == -1)
    {
		if(errno == EINTR)
		{ //the nanosleep() function was interrupted by a signal.
			tmp = t0;
            t0 = t1;
            t1 = tmp; 
		} 
		else 
		{
			return -1; 
		}
	}
#endif
    return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////
int  oss_date_set(uint16_t wYear, uint8_t byMonth, uint8_t byDay, uint8_t byHour, uint8_t byMin, uint8_t bySecond)
{
#ifndef WIN32
	uint8_t  abyBuf[256];

	sprintf(abyBuf, "date -s \"%d-%d-%d %d:%d:%d\"", wYear,byMonth,byDay,byHour, byMin, bySecond);
    system(abyBuf);
#endif
	
	return 1;	
}
/******************************************************************************/
void oss_get_cur_time(struct  timeval  *tv)
{
#ifdef WIN32
	tv->tv_sec= GetTickCount()/1000;
	tv->tv_usec= GetTickCount()%1000;
#else
	gettimeofday(tv , NULL);
#endif
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
int  OssLocalTimeGet(struct timeval *tp, void *tzp)
{
#ifndef WIN32
	gettimeofday(tp, tzp);
#endif

	return 1;
}
///////////////////////////////////////////////////////////////////////////////////////////////////////////////
#ifndef WIN32
void GetSystemTime(	SYSTEMTIME *systime)
{
	time_t timer; //time_t就是long int 类型
	struct tm *tblock;

	timer = time(NULL);
	tblock = localtime(&timer);
	systime->wYear = tblock->tm_year;
	systime->wMonth = tblock->tm_mon;	
	systime->wDay = tblock->tm_mday;	
	systime->wHour = tblock->tm_hour;	
	systime->wMinute = tblock->tm_min;	
	systime->wSecond = tblock->tm_sec;		
}
#endif
