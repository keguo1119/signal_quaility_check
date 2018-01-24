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

pthread_t control_t;

static int scan_oper_mode = 0;        //0:4G, 1:3G
static int scan_auto_oper_flag = 0;

static unsigned char g_uav_status  = 0;
static unsigned char g_main_status = 0;
static unsigned int  g_time_std_s  = 0; 

void scan_logic_gps_get(float *lat, float *lon)
{
    scan_net_gps_info_get(lat, lon);
}

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
void * scan_logic_control_run_pthread()
{
    int oper_mode;
    int flag;


    while(1) {
        oper_mode   = scan_cfg_modem_oper_mode_get();
        flag        = scan_cfg_modem_auto_operate_flag_get();

        if(flag) // 如果自动模式，则在返航时切换运行商模式，3G->4G 或者 4G->3G
        {
            if(LC_MS_GOBACK == g_main_status) // 主状态为返航
            {
                oper_mode = !oper_mode;       // 制式切换, oper_mode 每次进入的值都是一样的
//                scan_oper_mode =  oper_mode;
            }
        }

        if( oper_mode != scan_oper_mode) {
            scan_modem_run_stop();
            scan_modem_mode_change(oper_mode);
            scan_modem_status_check();
            scan_file_save_file_close();
            scan_logic_oper_mode_set(ret);
            scan_modem_run_start();
        }
        oss_delay(5 * 1000);
        system("cat $(ls /root/scanner/file/4G_signal_scan*  -tl | awk '{print $9}' | head -n 1) | head -n 2  > /root/scanner/file/signal_info.txt");
        system("cat $(ls /root/scanner/file/4G_signal_scan*  -tl | awk '{print $9}' | head -n 1) | tail   >> /root/scanner/file/signal_info.txt");
 
    }
}

void scan_logic_control_run()
{
    if(pthread_create(&control_t, NULL, (void *)scan_logic_control_run_pthread, NULL)) {
        printf("%s: created pthread failed!\n", __func__);
    } 
}
