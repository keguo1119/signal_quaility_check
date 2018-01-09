#ifndef  __OSS_TIME_H_
#define  __OSS_TIME_H_

#define  MAX_TIMER_CALLBACK   8

#define  MAX_TIMER_EVENT      16 //128

/**************************************************************************************/
typedef enum TagTimerState {
	OSS_TIMER_IDLE,
	OSS_TIMER_BUSY
}ETimerState;

typedef struct TagTimer 
{    
	uint32_t state;
    uint32_t interval;  
    uint32_t lefttime;   
	uint32_t tid;
	uint16_t dwInstId;
	uint16_t dwEvent;
    void (*func)(char *data, int len);
	uint32_t  dwData;
}oss_timer_t;  

typedef struct TagTimeoutMsg
{    
	uint32_t dwData;
}msg_timeout_t;

/**************************************************************************************/
void	oss_event_scan();
void	oss_timer_scan(void);
uint32_t oss_current_tick(void);
void	oss_reg_timer_proc(int time, void (*func)(char *data, int len), char *data);
int     oss_delay (uint32_t msec); 
int		oss_timer_init();

/**************************************************************************************/
int oss_set_timer_ex(int time, void (*func)(char *data, int len),  uint32_t dwEvent, uint32_t dwData);
int oss_set_timer(int time, void (*func)(char *data, int len),  uint32_t dwEvent);

void oss_del_timer(uint32_t tid, int time, uint32_t dwEvent);
void oss_del_timer_ex(/*uint32_t tid, int time,*/ uint32_t dwEvent);
/**************************************************************************************/
int  oss_date_set(uint16_t wYear, uint8_t byMonth, uint8_t byDay, uint8_t byHour, uint8_t byMin, uint8_t bySecond);

void oss_get_cur_time(struct  timeval  *tv);
BOOL IsTimeout(struct  timeval lastTime, struct  timeval currTime, int timeout);
int oss_timeout(unsigned int start_time, unsigned int hold_time);

#endif