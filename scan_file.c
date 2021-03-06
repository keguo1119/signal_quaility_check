#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "scan_cfg.h" 
#include "oss.h"
#include "config.h"
//#include "protocol_common.h"
#include "main.h"
#include "scan_modem.h"
#include "scan_logic_control.h"

#define MAX_INFO_LEN 128

char *file_path         = "./file/";
char *back_file_path    = "./back_file/";

char *err_info_file_name = "err_info.txt";
char *log_info_file_name = "log.txt";
static char *file_head_name = "4G_signal_scan";

static FILE *info_file_fp = NULL;
static FILE *err_info_file_fp = NULL;
static FILE *log_info_file_fp = NULL;

static int modem_info_num = 0;
//////////////////////////////////////////////////////////////////
void scan_local_time_get(char *time_buf)
{
    struct tm *timenow;
    time_t now;
    
    time(&now);

    timenow = localtime(&now);
    snprintf(time_buf,128,"%d-%d-%02d-%02d-%02d.txt",  1900+timenow->tm_year, timenow->tm_mon+1, timenow->tm_mday,  timenow->tm_hour,timenow->tm_min);
}

//longitude : 经度�? latitude: �?��
static void scan_gps_info_get(float *lon , float *lat)
{
    *lon = 119.4324;
    *lat = 39.2323;
}

///////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////
static void scan_file_name_get(char *name_buf)
{
    char time_buf[MAX_INFO_LEN];

    scan_local_time_get(time_buf);

    snprintf(name_buf,MAX_INFO_LEN, "%s%s-%s", file_path, file_head_name, time_buf);
}
//////////////////////////////////////////////////////////////////
void scan_file_save_file_close()
{
    if(info_file_fp != NULL) {
       fclose(info_file_fp);
       info_file_fp = NULL; 
    } 
}
///////////////////////////////////////////////////////////////////
void scan_file_info_save(char *info_buf, TModemLocal *pm, int num)
{
    char file_name_buf[MAX_INFO_LEN];
    char tmp_buf[MAX_INFO_LEN];
    int i;
    float lat_t, lon_t;

    memset(tmp_buf, 0, MAX_INFO_LEN);

    if (NULL == info_file_fp ) {
        modem_info_num = 0;     //初�?�?
        memset(file_name_buf, 0, MAX_INFO_LEN);
        scan_file_name_get(file_name_buf);
        info_file_fp = fopen(file_name_buf, "a+");
        if(NULL == info_file_fp) {
            printf("created info save file failed\n");
            return ;
        }

         for( i = 0; i < num; i++) {
            if(YUGA == pm->manu_id)
                snprintf(tmp_buf + strlen(tmp_buf), MAX_INFO_LEN, "Modem %d is YUGA,  ", pm->index);
            else if(HUAWEI == pm->manu_id)
                snprintf(tmp_buf + strlen(tmp_buf), MAX_INFO_LEN, "Modem %d is HUAWEI,  ",pm->index);
            
            pm++;
        }
        pm = pm - i;

        snprintf(tmp_buf + strlen(tmp_buf), MAX_INFO_LEN, "\n");
        fputs(tmp_buf,  info_file_fp); 

        memset(tmp_buf, 0, MAX_INFO_LEN);
        snprintf(tmp_buf + strlen(tmp_buf), MAX_INFO_LEN, "NUM,");
        for( i = 0; i < num; i++) {
            printf("%s: index=%d,oper=%d,mode=%d\n", __func__, pm->index, pm->oper, pm->mode);
            switch (pm->oper) {
            case NONE : 
                snprintf(tmp_buf + strlen(tmp_buf), MAX_INFO_LEN, "NONE-");
                break;
            case CMCC :
                snprintf(tmp_buf + strlen(tmp_buf), MAX_INFO_LEN, "CMCC-");
                break;
            case CUCC :
                snprintf(tmp_buf + strlen(tmp_buf), MAX_INFO_LEN, "CUCC-");
                break;
            case CTCC :
                snprintf(tmp_buf + strlen(tmp_buf), MAX_INFO_LEN, "CTCC-");
                break;
            }

            switch (pm->mode) {
            case  NO_MODE:
                snprintf(tmp_buf + strlen(tmp_buf), MAX_INFO_LEN, "NO_MODE,");
                break;
            case LTE : 
                snprintf(tmp_buf + strlen(tmp_buf), MAX_INFO_LEN, "LTE,");
                break;
            case TD_SCDMA : 
                snprintf(tmp_buf + strlen(tmp_buf), MAX_INFO_LEN, "TD_SCDMA,");
                break;
            case WCDMA : 
                snprintf(tmp_buf + strlen(tmp_buf), MAX_INFO_LEN, "WCDMA,");
                break;
            case CDMA : 
                snprintf(tmp_buf + strlen(tmp_buf), MAX_INFO_LEN, "CDMA,");
                break;
            case GSM: 
                snprintf(tmp_buf + strlen(tmp_buf), MAX_INFO_LEN, "GSM,");
                break;
            }
            
            pm++;
        }


        snprintf(tmp_buf + strlen(tmp_buf), MAX_INFO_LEN, "lat,lon\n");
        fputs(tmp_buf,  info_file_fp); 
    }

    modem_info_num++;
//    scan_gps_info_get(&lon_t, &lat_t);
    scan_logic_gps_get(&lat_t, &lon_t);
    snprintf(tmp_buf, MAX_INFO_LEN,"%d,%s%f,%f\n" ,modem_info_num, info_buf, lat_t, lon_t);
    
    fputs(tmp_buf,  info_file_fp); 
    fflush(info_file_fp);
}

void scan_file_error_info_save(char *err_buf)
{
    if (NULL == err_info_file_fp ) {
        err_info_file_fp = fopen(err_info_file_name, "w+");
        if(NULL == err_info_file_fp) {
            printf("created  err_info_file_fp file failed\n");
            return ;
        }
    }

    fputs(err_buf,  err_info_file_fp); 

    fflush(err_info_file_fp);
}

void scan_file_log_info_save(char *log_buf)
{
    char name_buf[128];
    char time_buf[128];

    if (NULL == log_info_file_fp ) {
        scan_local_time_get(time_buf);
        snprintf(name_buf, 128, "log/log-%s", time_buf);
        log_info_file_fp = fopen(name_buf, "w+");
        if(NULL == log_info_file_fp) {
            printf("created  log_info_file_fp file failed\n");
            return ;
        }
    }

    fputs(log_buf,  log_info_file_fp); 

    fflush(log_info_file_fp);
}

/*
    if(!ptModem->isvaild)
        return ;

    switch (ptModem->oper) {
    case CMCC: 
        snprintf(name_buf, MAX_INFO_LEN, "CMCC_");
        break;
        case CMCC: 
        snprintf(name_buf, MAX_INFO_LEN, "CMCC_");
        break;
    }
    */