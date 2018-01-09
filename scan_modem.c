#include "main.h"
#include "modem.h"
#include "oss.h"
#include "scan_cfg.h"
#include "scan_modem.h"
#include "scan_file.h"
#include "scan_gps.h"
#include <unistd.h>
#include <pthread.h>
#include "scan_logic_control.h"

#define  MODEM_NUM       4
#define  MAX_AT_CMD_NUMS 32
#define  MAX_INFO_LEN    1024

static int baud_rate = 115200;
static int parity    = 0;
static int byte_size = 8;
static int stop_bit  = 1;
static int scan_run_flag = 1; 

pthread_t modem_id; 

TModemLocal pModem[MODEM_NUM]; 

TModemAtCmdStr AtCmdStr[MAX_AT_CMD_NUMS] =  {
    {"AT\r",            "OK", ""}, //AT          = 0,
    {"ATE%d\r",         "OK", ""}, //ATE1 开回显,ATE0 关,
    {"AT+CGMI"          "OK", ""},  //查询厂商;
    {"AT+CMEE",         "OK", ""}, //AT_CMEE_SET     , =2 ,当发生错误的时候，显示详细信息
    {"AT+CGREG",        "OK", ""}, ///AT_CGREG_SET   , =2 ,打开PS域注册状态变化时的主动上报功
   
    {"AT+CPIN?\r",      "OK", ""}, //AT_CPIN_QUE     , 查询是否可以识别SIM卡
    {"AT+CGREG?\r",     "OK", ""}, ///AT_CGREG_QUE   , 查询是否注册了网络，
    {"AT^CURC",         "OK", ""}, //AT_CURC_SET_H    , =0 关闭部分主动上报，比如信号强度的上报
    {"AT^STSF",         "OK", ""}, //AT_STSF_SET_H     , =0 关闭STK的主动上报 
    {"AT+CMER=%s",      "OK", ""}, //AT_CMER_SET_Y關閉主動上報, 1,0,0,1打開，0,0,0,0 關閉
   
    {"ATS0",            "OK", ""}, //ATS_SET_H         , =0 关闭自动接听
    {"AT^NVAUTO",       "OK", ""}, //ATS_NVAUTO_SET_Y         , =0 关闭自动接听
    {"AT^HCSQ?\r",      "OK", ""}, //AT_HCSQ_QUE_H, 华为查询信号质量，
    {"AT+CCSQ\r",       "OK", ""}, //AT_CCSQ_QUE_Y, YUGA查询信号质量  
    {"AT+COPS?\r",      "OK", ""},  //运行商制式查询
   
    {"AT^MODECONFIG",   "OK", ""},  //YUGA 模式选择  2 自动， 9 CDMA, 13 GSM, 38 LTE only, 14 WCDMA only, 15 TD-SCDMA only 
    {"AT^NDISDUP=?\r",  "OK", ""}, //AT_NDISDUP_QUE  ,
    {"AT^NDISDUP=1,",   "OK", ""}, //AT_NDISDUP_SET, 0 断开网络连接，1连上网络   
   
};

////////////////////////////////////////////////////////
static int scan_modem_manu_que(TModemLocal *pm)
{
    int ret;
    char rev_buf[128];

    ret = modem_atCmd_w_r(&pm->atModem, AtCmdStr[AT_CURC_SET_H].atCmd, AtCmdStr[AT_CGMI_QUE].atCmdOkResp, rev_buf);
    if(ret != RET_OK)
	{
		return RET_FAILED;
	}

    if(strstr(rev_buf, "Huawei")) {
        pm->manu_id = HUAWEI;
    } else if (strstr(rev_buf, "Yuga")) {
        pm->manu_id = YUGA;
    }

	return RET_OK;

}
/////////////////////////////////////////////////////////////////////////
//AT_CURC_SET_H    , =0 关闭部分主动上报，比如信号强度的上报 HUAWEI
static int scan_modem_curc_set(TModem *ptModem, int mode)
{
    int ret;
    char cmd_buf[128];

    snprintf(cmd_buf, 128, "%s=%d\r", AtCmdStr[AT_CURC_SET_H].atCmd, mode);

    ret = modem_atCmd_w_r(ptModem, cmd_buf, AtCmdStr[AT_CURC_SET_H].atCmdOkResp, NULL);
    if(ret != RET_OK)
	{
        memset(cmd_buf, 0, 128);
        snprintf(cmd_buf, 128, "%s: modem=%d, error!\n", __func__, ptModem->index);
        scan_file_error_info_save(cmd_buf);
		return RET_FAILED;
	}

	return RET_OK;
}
////////////////////////////////////////////////////////
//AT_STSF_SET_H    , =0 关闭STK的主动上报 HUAWEI
static int scan_modem_stsf_set(TModem *ptModem, int mode)
{
    int ret;
    char cmd_buf[128];

    snprintf(cmd_buf, 128, "%s=%d\r", AtCmdStr[AT_STSF_SET_H].atCmd, mode);

    ret = modem_atCmd_w_r(ptModem, cmd_buf, AtCmdStr[AT_STSF_SET_H].atCmdOkResp, NULL);
    if(ret != RET_OK)
	{
        memset(cmd_buf, 0, 128);
        snprintf(cmd_buf, 128, "%s: modem=%d, error!\n", __func__, ptModem->index);
        scan_file_error_info_save(cmd_buf);
		return RET_FAILED;
	}

	return RET_OK;
}
////////////////////////////////////////////////////////
//ATS_SET_H    , 关闭自动接听,  HUAWEI
static int scan_modem_ats_set(TModem *ptModem, int mode)
{
    int ret;
    char cmd_buf[128];

    snprintf(cmd_buf, 128, "%s=%d\r", AtCmdStr[ATS_SET_H].atCmd, mode);

    ret = modem_atCmd_w_r(ptModem, cmd_buf, AtCmdStr[ATS_SET_H].atCmdOkResp, NULL);
    if(ret != RET_OK)
	{
        memset(cmd_buf, 0, 128);
        snprintf(cmd_buf, 128, "%s: modem=%d, error!\n", __func__, ptModem->index);
        scan_file_error_info_save(cmd_buf);
		return RET_FAILED;
	}

	return RET_OK;
}

////////////////////////////////////////////////////////////////////////
///////////////////////////////////////////////////////////////////////
//AT_CMER_SET_Y    , =0 关闭cmer的主动上报 YUGA
static int scan_modem_cmer_set(TModem *ptModem, int mode)
{
    int ret;
    char cmd_buf[128];

    snprintf(cmd_buf, 128, "%s=%d\r", AtCmdStr[AT_CMER_SET_Y].atCmd, mode);

    ret = modem_atCmd_w_r(ptModem, cmd_buf, AtCmdStr[AT_CMER_SET_Y].atCmdOkResp, NULL);
    if(ret != RET_OK)
	{
        memset(cmd_buf, 0, 128);
        snprintf(cmd_buf, 128, "%s: modem=%d, error!\n", __func__, ptModem->index);
        scan_file_error_info_save(cmd_buf);
		return RET_FAILED;
	}

	return RET_OK;
}
////////////////////////////////////////////////////////
//ATS_NVAUTO_SET_Y    , 关闭自动接听,  YUGA
static int scan_modem_nvauto_set(TModem *ptModem, int mode)
{
    int ret;
    char cmd_buf[128];

    snprintf(cmd_buf, 128, "%s=%d\r", AtCmdStr[ATS_NVAUTO_SET_Y].atCmd, mode);

    ret = modem_atCmd_w_r(ptModem, cmd_buf, AtCmdStr[ATS_NVAUTO_SET_Y].atCmdOkResp, NULL);
    if(ret != RET_OK)
	{
		memset(cmd_buf, 0, 128);
        snprintf(cmd_buf, 128, "%s: modem=%d, error!\n", __func__, ptModem->index);
        scan_file_error_info_save(cmd_buf);
		return RET_FAILED;
	}

	return RET_OK;
}

/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
static int scan_modem_sim_status_check(TModem *ptModem)
{
    char *pData,*pEnd;
    int ret;
    char  tmp_buf[MAX_INFO_LEN];
   
    
    ret = modem_atCmd_w_r(ptModem, AtCmdStr[AT_CPIN_QUE].atCmd, AtCmdStr[AT_CPIN_QUE].atCmdOkResp,tmp_buf);
//    printf("0-%s: ret=%d, RET_FAILED=%d, RET_OK=%d\n",  __func__, ret, RET_FAILED, RET_OK);
    if(ret != RET_OK)
	{
        snprintf(tmp_buf, MAX_INFO_LEN, "MODEM %d ERROR: SIM not inserted\n", ptModem->index);
        
        scan_file_error_info_save(tmp_buf);
		return RET_FAILED;
	}

    pData = strstr(tmp_buf, "READY");
    if (pData != NULL) {
        return RET_OK;
    } 

	return RET_OK;
}
/////////////////////////////////////////////////////////////////////////////
// AT+CGREG?  =?,1(5) 表示注册上网络
static int scan_modem_net_register_check(TModem *ptModem)
{
    char *pData,*pEnd;
    int ret;
    char  tmp_buf[MAX_INFO_LEN];
    int  en_flag, connet;

    ret = modem_atCmd_w_r(ptModem, AtCmdStr[AT_CGREG_QUE].atCmd, AtCmdStr[AT_CGREG_QUE].atCmdOkResp,tmp_buf);
    if(ret != RET_OK)
	{
		return RET_FAILED;
	}

    pData = strstr(tmp_buf, "OK");
    if (pData != NULL) {
        return RET_OK;
    } 

    sscanf(tmp_buf, "%s %d,%d", tmp_buf, &en_flag, &connet);
    if(1 == connet || 5 == connet) {
        return RET_OK;
    } else {
        snprintf(tmp_buf, MAX_INFO_LEN, "MODEM %d ERROR:  No Service or No RMB\n", ptModem->index);
        scan_file_error_info_save(tmp_buf);
        return RET_FAILED;
    }
    
	return RET_OK;
}
/////////////////////////////////////////////////////////////////////////////
// sim 卡运营商查询
static int scan_modem_sim_oper_que(TModemLocal *pm)
{
    char *pData,*pEnd;
    int ret;
    char  tmp_buf[MAX_INFO_LEN];
    TModem *ptModem = &pm->atModem;

    ret = modem_atCmd_w_r(ptModem, AtCmdStr[AT_COPS_QUE].atCmd, AtCmdStr[AT_COPS_QUE].atCmdOkResp,tmp_buf);
    if(ret != RET_OK) {
        snprintf(tmp_buf, MAX_INFO_LEN, "MODEM %d ERROR:  No Service or No RMB\n", ptModem->index);
        scan_file_error_info_save(tmp_buf);
        pm->oper = NONE;
        return RET_OK;
    }

    if (strstr(tmp_buf, "CHN-MOBILE")) {
         pm->oper = CMCC;
    } else if( strstr(tmp_buf, "CHN-UNICOM")) {
         pm->oper = CUCC;
    } else if (strstr(tmp_buf, "CHN-CT") || strstr(tmp_buf, "CHINA TELECOM")) {
        pm->oper = CTCC;
    } else {
        pm->oper = NONE;
    }
    printf("%s modem %d : buf=%s, oper=%d\n", __func__, pm->index, tmp_buf, pm->oper);
    return RET_OK;
}
/////////////////////////////////////////////////////////////////////////////
// 3G/4G mode 查询
static int scan_modem_sim_mode_que(TModemLocal *pm)
{
    char *pData,*pEnd;
    int ret;
    char  tmp_buf[MAX_INFO_LEN];
    TModem *ptModem = &pm->atModem;

    ret = modem_atCmd_w_r(ptModem, AtCmdStr[AT_HCSQ_QUE_H].atCmd, AtCmdStr[AT_HCSQ_QUE_H].atCmdOkResp,tmp_buf);
    if(ret != RET_OK) {
        snprintf(tmp_buf, MAX_INFO_LEN, "MODEM %d ERROR:  No Service or No RMB\n", ptModem->index);
        scan_file_error_info_save(tmp_buf);
        pm->mode = NO_MODE;
        return RET_OK;
    }

    if (strstr(tmp_buf, "LTE")) {
         pm->mode = LTE;
    } else if( strstr(tmp_buf, "TD-SCDMA")) {
         pm->mode = TD_SCDMA;
    } else if (strstr(tmp_buf, "WCDMA")) {
        pm->mode = WCDMA;
    } else if ( strstr(tmp_buf, "CDMA")) {
        pm->mode = CDMA;
    } else {
        pm->mode = GSM;
    }
    printf("%s modem %d : buf=%s, mode=%d\n", __func__, pm->index, tmp_buf, pm->mode);
    return RET_OK;
}
/////////////////////////////////////////////////////////
static int scan_modem_signal_quality_info_get(TModem *ptModem, char *buf, eoper_mode mode)
{
    char *pData,*pEnd;
    int ret, i, j;
    char  tmp_buf[MAX_INFO_LEN];
    int rssi, rscp, rsrp, ecio, sinr, rsrq;
    char start_buf[MAX_INFO_LEN/2], end_buf[MAX_INFO_LEN/2];

    ret = modem_atCmd_w_r(ptModem, AtCmdStr[AT_HCSQ_QUE_H].atCmd, AtCmdStr[AT_HCSQ_QUE_H].atCmdOkResp,tmp_buf);
    if(ret != RET_OK)
	{
		return RET_FAILED;
	}

    pData = strstr(tmp_buf, "HCSQ: ");
    if (pData != NULL) 
    {
        pEnd = strchr(pData, '\r');
        if(pEnd == NULL)
        {
            pEnd   = strchr(pData, '\n');
            //return RET_FAILED;
        } 
     
        pData += strlen("HCSQ: ");
//        strncpy(buf, pData, pEnd + 1 - pData);
    }
    *(pEnd + 1) = '\0';
    if (strstr(pData, "LTE")) {
        pData = strchr(pData, ',');
        sscanf(pData+1, "%d,%d,%d,%d", &rssi, &rsrp, &sinr, &rsrq);
        sprintf(buf, "%d,", rsrp);
    } else if (strstr(pData, "CDMA")) {
        pData = strchr(pData, ',');
        sscanf(pData+1, "%d,%d,%d", &rssi, &rscp, &ecio);
        sprintf(buf, "%d,", rscp);
    } else if (strstr(pData, "GSM")) {
        pData = strchr(pData, ',');
        sscanf(pData+1, "%d", &rssi);
        sprintf(buf, "%d,", rssi);
    } else {
        sprintf(buf, "0,");
    }
   
   printf("pData=%s, buf=%s\n",pData, buf);

}
/////////////////////////////////////////////////////////////////////////////
//将模块由3G 改为4G或者4G 改为3G, mode=0 设置为4G,1为3G
int scan_modem_mode_change(int mode)
{
    char cmd[128];
    int  i, ret;
    char err_buf[128];
    char cmd_buf[128];
    
    TModemLocal *pm = pModem;

    for (i = 0; i < MODEM_NUM; i++ ) {
        pm = &pModem[i];

        if(0 == pm->isvaild) {
            continue;
        }
        if(0 == mode ) {
            memset(cmd_buf, 0, 128);
            memset(err_buf, 0 ,128);
            snprintf(cmd_buf, 128, "AT^SYSCFGEX=\"%s\",3FFFFFFF,1,2,7FFFFFFFFFFFFFFF,,\r", "03");
            ret = modem_atCmd_w_r(&pm->atModem, cmd_buf, "OK", NULL);
            if(ret != RET_OK) {
                snprintf(err_buf, 128, "Modem %d set LTE mdoe faield!, cmd_buf=%s\n", pm->index, cmd_buf);
                scan_file_error_info_save(err_buf);
            }
        } else if (1 == mode) {
            memset(cmd_buf, 0, 128);
            memset(err_buf, 0 ,128);
            snprintf(cmd_buf, 128, "AT^SYSCFGEX=\"%s\",3FFFFFFF,1,2,7FFFFFFFFFFFFFFF,,\r", "02");
            ret = modem_atCmd_w_r(&pm->atModem, cmd_buf, "OK", NULL);
            if(ret != RET_OK) {
                snprintf(err_buf, 128, "Modem %d set LTE mdoe faield!, cmd_buf=%s\n", pm->index, cmd_buf);
                scan_file_error_info_save(err_buf);
            }
        }
    }

    oss_delay(15 * 1000);

    return RET_OK;
}
/////////////////////////////////////////////////////////////////////////////
void scan_modem_status_check()
{
    int i;

    TModemLocal *pm = pModem;

    for(i = 0; i < MODEM_NUM - 1; i++) {
        pm = &pModem[i];
        
        if(0 == pm->isvaild) {
            continue;
        }
    
        scan_modem_sim_status_check(&pm->atModem); 
        scan_modem_net_register_check(&pm->atModem);
        scan_modem_sim_oper_que(pm);
        scan_modem_sim_mode_que(pm);
        oss_delay(50);
    }
    oss_delay(5000);
}
////////////////////////////////////////////////////////////////////////////
void scan_modem_run_flag_set(int flag)
{
    scan_run_flag = flag;
}
/////////////////////////////////////////////////////////////////////////////
static void *scan_modem_run_pthread()
{
    int i, j;
    int index;

    char info_buf[MAX_INFO_LEN];

    TModemLocal *pm = pModem;
    printf(" %s is staring ,pm=%p, pm+1=%p, pm+2=%p!\n", __func__, pm, pm+1, pm+2);

    scan_modem_status_check();

    while(1) {
        oss_delay(500);

        if(0 == scan_run_flag)
            continue;

        for (i = 0; i < MODEM_NUM - 1; i++) {
            pm = &pModem[i];

            if(0 == pm->isvaild) {
                printf("modem %d is scan_modem_uart_port_get failed!\n", i);
                continue;
            }
//            printf("0-i=%d, &pm=%p\n", i, pm);
            scan_modem_signal_quality_info_get(&pm->atModem, info_buf + strlen(info_buf), pm->oper);            
            printf("modem %d -> signal quality: %s,oper=%d, mode=%d\n", pm->index, info_buf, pm->oper, pm->mode);
        }

        scan_file_info_save(info_buf, &pModem[0], MODEM_NUM - 1);
        memset(info_buf, 0, MAX_INFO_LEN);

    }

}
/////////////////////////////////////////////////////////////////////////////
void scan_modem_run()
{   
    int ret;

    ret = pthread_create(&modem_id, NULL, (void *)scan_modem_run_pthread, NULL);
    if(ret != 0) {
        printf("%s: created pthread failed!\n", __func__);
    } 
}
////////////////////////////////////////////////////////////////////////////
void scan_modem_init()
{
    int i,j,ret;
    int index;
    int mode;
    TModemLocal *pm = pModem;
    
    memset(pModem, 0, MODEM_NUM * sizeof(TModemLocal));

    printf("&pModem[0]=%p, &pModem[1]=%p, &pModem[2]=%p\n", &pModem[0], &pModem[1], &pModem[2]);

    for (i = 0; i < MODEM_NUM - 1; i++) {
        pm = &pModem[i];;
        pm->index = i;
        pm->atModem.nBaudRate = baud_rate;
        pm->atModem.nParity   = parity;
        pm->atModem.nByteSize = byte_size;
        pm->atModem.nStopBits = stop_bit;
        pm->isvaild           = 1;
        pm->oper              = CUCC;
        pm->mode              = LTE;

        printf("i =%d, pm=%p\n", i, pm);
        ret = scan_modem_uart_port_get(i, pm->atModem.abyUartPort);
        if(ret == RET_FAILED) {
            pm->isvaild = 0;
            printf("modem %d is scan_modem_uart_port_get!\n", i);
            continue ;
        }

        ret = modem_open(&pm->atModem);
        if(ret == RET_FAILED) {
            pm->isvaild = 0;
            printf("modem %d is open failed!\n", i); 
            continue;
        }

        scan_modem_manu_que(pm); //厂家查询
        modem_error_mode(&pm->atModem, 2);

        if(HUAWEI == pm->manu_id) {
            scan_modem_curc_set(&pm->atModem, 0); //AT_CURC_SET_H    , =0 关闭部分主动上报，比如信号强度的上报
            scan_modem_stsf_set(&pm->atModem, 0); //AT_STSF_SET_H, =0 关闭STK的主动上报
            scan_modem_ats_set(&pm->atModem, 0);  //ATS_SET_H    , 关闭自动接听 
        } else if (YUGA == pm->manu_id) {
                scan_modem_cmer_set(&pm->atModem, 0);
                scan_modem_nvauto_set(&pm->atModem, 0);
        }

     
//        scan_modem_status_check(pm);

        printf("i =%d, ret=%d, vaild=%d, fd=%d, oper=%d, mode=%d\n",  
            i, ret, pm->isvaild, pm->atModem.fd, pm->oper, pm->mode);
    }

    printf(" %s is ok!\n", __func__);

    mode = scan_modem_oper_mode();
   
    scan_logic_oper_mode_set(mode);

    ret = scan_modem_mode_change(mode); 
 
}
////////////////////////////////////