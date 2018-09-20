#include "scan_logic_control.h"
#include "scan_cfg.h"
#include "oss.h"
#include "main.h"
#include "scan_cfg.h"
#include "scan_file.h"
#include "scan_modem.h"
#include "scan_net.h"

#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include <time.h>

pthread_t control_t;

static int scan_oper_mode = 0;        //0:4G, 1:3G
static int scan_auto_oper_flag = 0;

static unsigned char g_uav_status  = 0; // ?????????????????
static unsigned char g_main_status = 0; // ?????????????
static unsigned int  g_time_std_s  = 0; 

static unsigned char g_last_uav_status = 0;
static unsigned char file_name_tail[128];
///////////////////////////////////////////////////////
void scan_logic_gps_get(float *lat, float *lon)
{
    //scan_net_gps_info_get(lat, lon);
}

void scan_logic_gps_info_file_name_tail()
{
     scan_local_time_get(file_name_tail);
}


void scan_logic_gps_write_to_file(int lat, int lon)
{
    char cmd_buf[256];

    if(strlen(file_name_tail) == 0)
        scan_logic_gps_info_file_name_tail();

    strcpy(cmd_buf, "date +\"%Y-%m-%d %H:%M:%S\" ");
    snprintf(cmd_buf+strlen(cmd_buf), 256-strlen(cmd_buf), " >> /root/scanner/iperf/gps_info-%s", file_name_tail);
    system(cmd_buf);
    printf("cmd_buf=%s\n",cmd_buf);

    memset(cmd_buf, 0, 256);
    snprintf(cmd_buf, 256, "echo '%f,%f' >> /root/scanner/iperf/gps_info-%s", (float)lat /10000000, (float)lon /10000000, file_name_tail);
    printf("cmd_buf=%s\n",cmd_buf);
    system(cmd_buf);
}
///////////////////////////////////////////
void scan_logic_oper_mode_set(int mode)
{
    printf("%s-mode=%d\n", __func__, mode);
    scan_oper_mode = mode;
}
////////////////////////////////////////////////////////
void scan_logic_uav_status_set(unsigned char status)
{
    g_uav_status = status;
}
unsigned char scan_logic_uav_status_get(void )
{
    return g_uav_status;
}
////////////////////////////////////////////////////////
void scan_logic_main_status_set(unsigned char status)
{
    g_main_status = status;
}
unsigned char scan_logic_main_status_get(void )
{
    return g_main_status;
}
////////////////////////////////////////////////////////
void scan_logic_time_std_set(unsigned int time_std)
{
    g_time_std_s = time_std;
}
unsigned int scan_logic_time_std_get(void )
{
    return g_time_std_s;
}
////////////////////////////////////////////////////////
void scan_logic_set_system_time(time_t l_time)
{
    char cmd_buf[128];
    struct tm *area;

    area = localtime(&l_time);

    snprintf(cmd_buf, 128, "date -s \"%d-%d-%d %d:%d:%d\"", \
      area->tm_year + 1900, area->tm_mon+1, area->tm_mday, area->tm_hour,area->tm_min, area->tm_sec);
    system(cmd_buf);
    
    oss_delay(1*1000);
}
////////////////////////////////////////////////////////
void * scan_logic_control_run_pthread()
{
    int oper_mode;
    int flag;

    printf("%s start\n", __func__);

    while(1) {
        oper_mode   = scan_cfg_modem_oper_mode_get();
        flag        = scan_cfg_modem_auto_operate_flag_get();

        // ????????
        if(flag) // ???????????????????§Ý???????????3G->4G ???? 4G->3G
        {
            if(LC_MS_GOBACK == g_main_status) // ?????????
            {
                oper_mode = !oper_mode;       // ????§Ý?, oper_mode ??¦Í??????????????
//                scan_oper_mode =  oper_mode;
            }
        }

        if( oper_mode != scan_oper_mode) {
            scan_modem_run_stop();
            scan_modem_mode_change(oper_mode);
            scan_modem_status_check();
            scan_file_save_file_close();
            scan_logic_oper_mode_set(oper_mode);
            scan_modem_run_start();
        }

        oss_delay(1 * 1000);
        system("cat $(ls /root/scanner/file/4G_signal_scan*  -tl | awk '{print $9}' | head -n 1) | head -n 2  > /root/scanner/file/signal_info.txt 2>&1 &");
        system("cat $(ls /root/scanner/file/4G_signal_scan*  -tl | awk '{print $9}' | head -n 1) | tail   >> /root/scanner/file/signal_info.txt 2>&1 &");
 
        if(g_uav_status == g_last_uav_status)
               continue;

        // ????????
        printf("0-%s, g_uav_status=%d, LC_SS_TAKEOFF=%d\n", __func__, g_uav_status, LC_SS_TAKEOFF);
        if (LC_SS_TAKEOFF == g_uav_status) {  // ??????????
            g_last_uav_status = g_uav_status;
            
            scan_logic_set_system_time(g_time_std_s); //?????????
            scan_modem_run_start();
            system("sh /root/scanner/run.sh &");  //start iperf
            printf("1-%s, scan_modem_run_start\n", __func__);
        } else if (LC_SS_LANDED_NORMAL == g_uav_status){      // ??????????      
            g_last_uav_status = g_uav_status;
            scan_modem_run_stop();
            system(" kill $(ps | grep iperf | grep -v grep | awk '{print $1}')"); //stop iperf
        }       
    }
}

void scan_logic_control_run()
{
    printf("%s run run\n", __func__);
    if(pthread_create(&control_t, NULL, (void *)scan_logic_control_run_pthread, NULL)) {
        printf("%s: created pthread failed!\n", __func__);
    } 
}
