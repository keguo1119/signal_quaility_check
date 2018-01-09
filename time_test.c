#include <time.h>
#include <stdio.h>
#include <stdlib.h>

static void scan_local_time_get(struct tm *timenow)
{
char name_buf[128];
    time_t now;
    time(&now);
    timenow = localtime(&now);
	printf("%s",asctime(timenow));

	snprintf(name_buf,128,"4G_signal_scan-%d-%d-%d-%d-%d-%d-%d.txt\n",  timenow->tm_year, timenow->tm_mon, timenow->tm_mday,  timenow->tm_hour,timenow->tm_min, 
        	timenow->tm_sec, timenow->tm_min/ 30);
	printf("%s", name_buf);	
}

int main()
{
char name_buf[128];
	struct tm timenow;
	scan_local_time_get(&timenow);
	printf("%s",asctime(&timenow));
	snprintf(name_buf,128,"4G_signal_scan-%d-%d-%d-%d-%d-%d-%d.txt\n",  timenow.tm_year, timenow.tm_mon, timenow.tm_mday,  timenow.tm_hour,timenow.tm_min,    	timenow.tm_sec, timenow.tm_min/ 30);
	printf("%s", name_buf);	
//　printf("Local   time   is   %s\n",asctime(timenow));
}
