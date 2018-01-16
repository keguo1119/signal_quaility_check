#include "scan_logic_control.h"
#include "scan_cfg.h"
//#include "scan_modem.h"

#include "oss.h"
#include <stdio.h>
#include <stdlib.h>
#include <pthread.h>
#include "main.h"
#include "scan_cfg.h"
#include "scan_file.h"
#include "scan_modem.h"
#include "scan_net.h"
pthread_t control_t;

static int scan_oper_mode = 0;        //0:4G, 1:3G
static int scan_auto_oper_flag = 0;

void scan_logic_gps_get(float *lat, float *lon)
{
    scan_net_gps_info_get(lat, lon);
}

void scan_logic_oper_mode_set(int mode)
{
    printf("%s-mode=%d\n", __func__, mode);
    scan_oper_mode = mode;
}

void * scan_logic_control_run_pthread()
{
    int ret;
    int flag;


    while(1) {
        ret  = scan_cfg_modem_oper_mode_get();
        flag = scan_cfg_modem_auto_operate_flag_get();

        if( ret != scan_oper_mode) {
            scan_modem_run_flag_set(0);
            scan_modem_mode_change(ret);
            scan_modem_status_check();
            scan_file_save_file_close();
            scan_logic_oper_mode_set(ret);
            scan_modem_run_flag_set(1);
        }
        oss_delay(5 * 1000);
    }
}

void scan_logic_control_run()
{
    if(pthread_create(&control_t, NULL, (void *)scan_logic_control_run_pthread, NULL)) {
        printf("%s: created pthread failed!\n", __func__);
    } 
}
