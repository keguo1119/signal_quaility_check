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
    {"AT+CGMI\r"          "OK", ""},  //查询厂商;
    {"AT+CMEE",         "OK", ""}, //AT_CMEE_SET     , =2 ,当发生错误的时候，显示详细信息
    {"AT+CGREG",        "OK", ""}, ///AT_CGREG_SET   , =2 ,打开PS域注册状态变化时的主动上报功
   
    {"AT+CPIN?\r",      "OK", ""}, //AT_CPIN_QUE     , 查询是否可以识别SIM卡
    {"AT+CGREG?\r",     "OK", ""}, ///AT_CGREG_QUE   , 查询是否注册了网络，
    {"AT^CURC",         "OK", ""}, //AT_CURC_SET_H    , =0 关闭部分主动上报，比如信号强度的上报
    {"AT^STSF",         "OK", ""}, //AT_STSF_SET_H     , =0 关闭STK的主动上报 
    {"AT+CMER",      "OK", ""}, //AT_CMER_SET_Y 關閉主動上報, 1,0,0,1打開，0,0,0,0 關閉
   
    {"ATS0",            "OK", ""}, //ATS_SET_H         , =0 关闭自动接听
    {"AT^NVAUTO",       "OK", ""}, //ATS_NVAUTO_SET_Y         , =0 关闭自动接听
    {"AT^HCSQ?\r",      "OK", ""}, //AT_HCSQ_QUE_H, 华为查询信号质量，
    {"AT+CCSQ\r",       "OK", ""}, //AT_CCSQ_QUE_Y, YUGA查询信号质量  
    {"AT+COPS?\r",      "OK", ""},  //运行商制式查询
   
    {"AT^MODECONFIG",   "OK", ""},  //YUGA 模式选择  2 自动， 9 CDMA, 13 GSM, 38 LTE only, 14 WCDMA only, 15 TD-SCDMA only 
    {"AT^NDISDUP=?\r",  "OK", ""}, //AT_NDISDUP_QUE  ,
    {"AT^NDISDUP=1,",   "OK", ""}, //HUAWEI,AT_NDISDUP_SET, 0 断开网络连接，1连上网络   
    {"AT$QCRMCALL",     "OK", ""}, //YUGA, AT_QCRMCALL_SET_H, 0 断开网络连接，1连上网络 
};

////////////////////////////////////////////////////////
static int scan_modem_manu_que(TModemLocal *pm)
{
    int ret;
    char rev_buf[128];
    char info_buf[128];

    ret = modem_atCmd_w_r(&pm->atModem, AtCmdStr[AT_CGMI_QUE].atCmd, AtCmdStr[AT_CGMI_QUE].atCmdOkResp, rev_buf);
    if(ret != RET_OK)
	{
		return RET_FAILED;
	}

    if(strstr(rev_buf, "Huawei")) {
        pm->manu_id = HUAWEI;
        sprintf(info_buf, "Modem %d MANU is HUAWEI, manu_id=%d\n", pm->index,  pm->manu_id);
    } else if (strstr(rev_buf, "Yuga")) {
        pm->manu_id = YUGA;
        sprintf(info_buf, "Modem %d MANU is YUGA, manu_id=%d\n", pm->index,  pm->manu_id);
    }

    scan_file_error_info_save(info_buf);

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

    if(!mode)
        snprintf(cmd_buf, 128, "%s=%s\r", AtCmdStr[AT_CMER_SET_Y].atCmd, "0,0,0,0"); //关
    else 
        snprintf(cmd_buf, 128, "%s=%s\r", AtCmdStr[AT_CMER_SET_Y].atCmd, "1,0,0,1"); //开

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
////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
// AT+CGREG?  =?,1(5) 表示注册上网络
static int scan_modem_net_register_check(TModemLocal *pm )
{
    char *pData,*pEnd;
    int ret;
    char  tmp_buf[MAX_INFO_LEN];
    int  en_flag, connet;

    ret = modem_atCmd_w_r(&pm->atModem, AtCmdStr[AT_CGREG_QUE].atCmd, AtCmdStr[AT_CGREG_QUE].atCmdOkResp,tmp_buf);
    if(ret != RET_OK)
	{
		snprintf(tmp_buf, MAX_INFO_LEN, "%s: MODEM %d ERROR\n", __func__, pm->index);
        scan_file_error_info_save(tmp_buf);
        return RET_FAILED;
	}

    pData = strstr(tmp_buf, "OK");
    if (pData != NULL) {
        pData = strchr(tmp_buf, ':');
        pData++;
    } else {
        return RET_FAILED;
    } 

    sscanf(pData, "%d,%d", &en_flag, &connet);
    if(1 == connet || 5 == connet) {
        snprintf(tmp_buf, MAX_INFO_LEN, "INFO : MODEM %d :  connet the Net\n", pm->index);
        scan_file_error_info_save(tmp_buf);
        pm->regis_ok = 1;
        return RET_OK;
    } else {
        pm->regis_ok = 0;
        snprintf(tmp_buf, MAX_INFO_LEN, "MODEM %d ERROR:  No Service or No RMB\n", pm->index);
        scan_file_error_info_save(tmp_buf);
        return RET_FAILED;
    }
    printf("%s: pData=%s\n", __func__, pData);
	return RET_OK;
}
////////////////////////////////////////////////////////////////////////////////////////////
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

    if (strstr(tmp_buf, "CHN-MOBILE") || strstr(tmp_buf, "CHINA MOBILE") || strstr(tmp_buf, "CMCC")) {
         pm->oper = CMCC;
    } else if( strstr(tmp_buf, "CHN-UNICOM") || strstr(tmp_buf, "CHINA UNICOM") || strstr(tmp_buf, "CUCC") ) {
         pm->oper = CUCC;
    } else if (strstr(tmp_buf, "CHN-CT") || strstr(tmp_buf, "CHINA TELECOM") || strstr(tmp_buf, "CTCC")) {
        pm->oper = CTCC;
    } else {
        pm->oper = NONE;
    }
    printf("%s modem %d : buf=%s, oper=%d\n", __func__, pm->index, tmp_buf, pm->oper);
    return RET_OK;
}
/////////////////////////////////////////////////////////////////////////////
// 3G/4G mode 查询， HUAWEI
static estand_mode scan_modem_sim_mode_que_H(TModemLocal *pm)
{
    char *pData,*pEnd;
    int ret;
    char  tmp_buf[MAX_INFO_LEN];
    TModem *ptModem = &pm->atModem;

    if(HUAWEI != pm->manu_id) {
        return NO_MODE;
    } 

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
    return pm->mode;
}
/////////////////////////////////////////////////////////////////////////////
// 3G/4G mode 查询， YUGA
static estand_mode scan_modem_sim_mode_que_Y(TModemLocal *pm)
{
    int  i, ret;
    char *pData,*pEnd;
    char err_buf[128];
    char cmd_buf[128];
    char rev_buf[128];
    int mode = 0;

    if(YUGA != pm->manu_id) {
        return NO_MODE;
    } 

    snprintf(cmd_buf, 128, "%s?\r", AtCmdStr[AT_MODECONFIG_SET_Y].atCmd);
    ret = modem_atCmd_w_r(&pm->atModem, cmd_buf, "OK", rev_buf);
    if(ret != RET_OK) {
        snprintf(err_buf, 128, "Modem %d operater query failed!, cmd_buf=%s\n", pm->index, cmd_buf);
        scan_file_error_info_save(err_buf);
    }   

    pData = strchr(rev_buf, ':');
    if(!pData) {
         scan_file_error_info_save("scan_modem_sim_mode_que_Y error!");
        return RET_FAILED;
    } else {
        pData++;
        pEnd = strchr(pData, '\r');
        if(pEnd == NULL)
        {
            pEnd   = strchr(pData, '\n');
        } 
    }  
    *(pEnd + 1) = '\0';

    sscanf(pData, "%d", &mode);

    switch (mode) {
        case 9:
            pm->mode = CDMA;
            break;
        case 14:
             pm->mode = WCDMA;
            break;
        case 15:
             pm->mode = TD_SCDMA;
            break;
        case 38:
            pm->mode = LTE;
            break;
        default:
            pm->mode = NO_MODE;
            break;    
    }
    printf("%s: modem %d mode = %d, pData=%s, rev_buf=%s\n", __func__, pm->index, pm->mode, pData, rev_buf);
    return pm->mode;
       
}
/////////////////////////////////////////////////////////////////////////////////////////
/////////HUAWEI 信号质量获取
static int scan_modem_signal_quality_info_get_H(TModem *ptModem, char *buf, eoper_mode mode)
{
    char *pData,*pEnd;
    int ret, i, j;
    char  tmp_buf[MAX_INFO_LEN];
    int rssi, rscp, rsrp, ecio, sinr, rsrq;

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

        if( rsrp >= 0 && rsrp <=96 ) {
            rsrp = rsrp - 140;
        } else if ( 97 >= rsrp ){
            rsrp = -44;
        } else {
            rsrp = -250;
        }
        sprintf(buf, "%d,", rsrp);
    } else if (strstr(pData, "CDMA")) {
        pData = strchr(pData, ',');
        sscanf(pData+1, "%d,%d,%d", &rssi, &rscp, &ecio);

        if( rscp >= 0 && rscp <=95 ) {
            rscp = rscp - 120;
        } else if ( 96 >= rscp ){
            rscp = -25;
        } else {
            rscp = -250;
        }        

        sprintf(buf, "%d,", rscp);
    } else if (strstr(pData, "GSM")) {
        pData = strchr(pData, ',');
        sscanf(pData+1, "%d", &rssi);
        sprintf(buf, "%d,", rssi);
    } else {
        sprintf(buf, "0,");
    }
    
//   printf("pData=%s, buf=%s\n",pData, buf);

}
//////////////////////////////////////////////////////////////////////////////////////
////YUGA 信号质量获取
static int scan_modem_signal_quality_info_get_Y(TModem *ptModem, char *buf, eoper_mode mode)
{
    char *pData,*pEnd;
    int ret, i, j;
    char  tmp_buf[MAX_INFO_LEN];
    int rssi, ber;

    ret = modem_atCmd_w_r(ptModem, AtCmdStr[AT_CCSQ_QUE_Y].atCmd, AtCmdStr[AT_CCSQ_QUE_Y].atCmdOkResp,tmp_buf);
    if(ret != RET_OK)
	{
        scan_file_error_info_save("scan_modem_signal_quality_info_get_Y no info get\n");
		return RET_FAILED;
	}
    pData = strchr(tmp_buf, ':');
    if(!pData) {
        scan_file_error_info_save("scan_modem_signal_quality_info_get_Y no info get 1\n");
        return RET_FAILED;
    } else {
        pData++;
         pEnd = strchr(pData, '\r');
        if(pEnd == NULL)
        {
            pEnd   = strchr(pData, '\n');
        } 
    }
   
    *(pEnd + 1) = '\0';

    sscanf(pData, "%d %d", &rssi, &ber);
 //   printf("pData=%s, rssi=%d, ber=%d\n", pData, rssi, ber);

    if(31 == rssi) {
        rssi = -51;
    } else if(0 == rssi) {
        rssi = -113;
    } else if (99 == rssi){
        rssi = 0;
    }else {
        rssi = -113 + 2 * rssi;
    }

    sprintf(buf, "%d,", rssi);    
}
/////////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////
//将模块由3G 改为4G或者4G 改为3G, mode=0 设置为4G,1为3G HUAWEI
static int scan_modem_mode_change_H(int mode)
{
    char cmd[128];
    int  i, ret;
    char err_buf[128];
    char cmd_buf[128];
    char count;
     estand_mode cur_mode;

    TModemLocal *pm = pModem;

    for (i = 0; i < MODEM_NUM; i++ ) {
        pm = &pModem[i];

        if(0 == pm->isvaild) {
            continue;           
        }

        if(HUAWEI != pm->manu_id) {
            continue;
        } 

        memset(cmd_buf, 0, 128);
        memset(err_buf, 0 ,128);

        cur_mode = pm->mode;
        printf("%s: index=%d, cur_mode = %d, mode=%d\n", __func__, i, cur_mode, mode);
        if(0 == mode ) {
            if(cur_mode == LTE)
                continue;

            snprintf(cmd_buf, 128, "AT^SYSCFGEX=\"%s\",3FFFFFFF,1,2,7FFFFFFFFFFFFFFF,,\r", "03");
            ret = modem_atCmd_w_r(&pm->atModem, cmd_buf, "OK", NULL);
            if(ret != RET_OK) {
                snprintf(err_buf, 128, "Modem %d set LTE mdoe faield!, cmd_buf=%s\n", pm->index, cmd_buf);
                scan_file_error_info_save(err_buf);
                continue;
            }
            count++;
        } else if (1 == mode) {
            if(cur_mode == TD_SCDMA || cur_mode == WCDMA || cur_mode == CDMA)
                continue;

            snprintf(cmd_buf, 128, "AT^SYSCFGEX=\"%s\",3FFFFFFF,1,2,7FFFFFFFFFFFFFFF,,\r", "02");
            ret = modem_atCmd_w_r(&pm->atModem, cmd_buf, "OK", NULL);
            if(ret != RET_OK) {
                snprintf(err_buf, 128, "Modem %d set 3G mdoe faield!, cmd_buf=%s\n", pm->index, cmd_buf);
                scan_file_error_info_save(err_buf);
            }
            count++;
        }

        oss_delay(50);
    }

    

    return count;
}
/////////////////////////////////////////////////////////////////////////////
//将模块由3G 改为4G或者4G 改为3G, mode=0 设置为4G,1为3G YUGA
static int scan_modem_mode_change_Y(int mode)
{
    char cmd[128];
    int  i, ret,  count = 0;
    char err_buf[128];
    char cmd_buf[128];
    estand_mode cur_mode;

    int  md ;
    char mode_info[128];

    TModemLocal *pm = pModem;

    for (i = 0; i < MODEM_NUM; i++ ) {
        pm = &pModem[i];

        if(0 == pm->isvaild) {
            continue;
        }

        if(YUGA != pm->manu_id) {
            continue;
        } 
        
        memset(cmd_buf, 0, 128);
        memset(err_buf, 0 ,128);

        cur_mode = pm ->mode;
         printf("%s: index=%d, cur_mode = %d, mode=%d\n", __func__, i, cur_mode, mode);
    
        if(0 == mode) {
            if(cur_mode == LTE)
                continue;

            md = 38; //LTE only
            strcpy(mode_info, "LTE only");
        } else if( 1 == mode) {
            if(cur_mode == TD_SCDMA || cur_mode == WCDMA || cur_mode == CDMA)
                continue;

            switch (pm->oper) {
            case CMCC: 
                md = 15; //LTE only
                strcpy(mode_info, "TD-SCDMA only");
                break;
            case CUCC: 
                md = 14; //LTE only
                strcpy(mode_info, "WCDMA only");
                break;
            case CTCC: 
                md = 9; //LTE only
                strcpy(mode_info, "CDMA only");
                break;
            default :
                continue;
                break;
            }
        }

        snprintf(cmd_buf, 128, "%s=%d\r", AtCmdStr[AT_MODECONFIG_SET_Y].atCmd, md);       
        ret = modem_atCmd_w_r(&pm->atModem, cmd_buf, "OK", NULL);
        if(ret != RET_OK) {
            snprintf(err_buf, 128, "Modem %d set %s mdoe faield!, cmd_buf=%s\n", pm->index, mode_info);
            scan_file_error_info_save(err_buf);
            continue;
        } 
        count++; 
        oss_delay(50);   
    }

    return count;
}
/////////////////////////////////////////////////////////////////////////////
//华为拨号, flag = 1, connecting, flag = 0, disconnect
static int scan_modem_connet_net_H(TModemLocal *pm, int flag)
{
     char cmd[128];
    int  i, ret,  count = 0;
    char err_buf[128];
    char cmd_buf[128];
    estand_mode cur_mode;

    int  md ;
    char mode_info[128];

    if(0 == pm->isvaild) {
       return RET_FAILED;
    }

    if(YUGA == pm->manu_id) {
        return RET_FAILED;
    }
    
    if(!pm->regis_ok) {
        return RET_FAILED;
    }
    
    memset(cmd_buf, 0, 128);
    memset(err_buf, 0 ,128);

    snprintf(cmd_buf, 128, "%s%d\r", AtCmdStr[AT_NDISDUP_SET_H].atCmd, flag);       
    ret = modem_atCmd_w_r(&pm->atModem, cmd_buf, "OK", NULL);
    if(ret != RET_OK) {
        snprintf(err_buf, 128, "ERROR: Modem %d %s set %d faield!\n", pm->index, scan_modem_connet_net_H, flag);
        scan_file_error_info_save(err_buf);
        return RET_FAILED;
    }   
    return RET_OK;    
}
/////////////////////////////////////////////////////////////
//AT_QCRMCALL_SET_H
//YUGA, flag = 1, connecting, flag = 0, disconnect //
static int scan_modem_connet_net_Y(TModemLocal *pm, int flag)
{
     char cmd[128];
    int  i, ret,  count = 0;
    char err_buf[128];
    char cmd_buf[128];
    estand_mode cur_mode;

    int  md ;
    char mode_info[128];

    if(0 == pm->isvaild) {
        return RET_FAILED;
    }

    if(HUAWEI == pm->manu_id) {
        return RET_FAILED;
    }
    
    if(!pm->regis_ok) {
        return RET_FAILED;
    }
        
    memset(cmd_buf, 0, 128);
    memset(err_buf, 0 ,128);

    snprintf(cmd_buf, 128, "%s=1,%d,1,2,1\r", AtCmdStr[AT_QCRMCALL_SET_H].atCmd, flag);       
    ret = modem_atCmd_w_r(&pm->atModem, cmd_buf, "$QCRMCALL: 1, V4", err_buf);
    printf("%s: cmd_buf=%s, rev_buf=%s\n", __func__, cmd_buf, err_buf);
    if(ret != RET_OK) {
        snprintf(err_buf, 128, "ERROR: Modem %d connect faield!\n", pm->index);
        scan_file_error_info_save(err_buf);
        return RET_FAILED;
    } 
    printf("%s: cmd_buf=%s\n", __func__, cmd_buf);
    return RET_OK;        
}
///////////////////////////////////////////////////////////////////////////////
int scan_modem_sim_mode_que(TModemLocal *pm)
{
    scan_modem_sim_mode_que_H(pm);
    scan_modem_sim_mode_que_Y(pm);
}
/////////////////////////////////////////////////////////////////////////////
int scan_modem_mode_change(int mode)
{
    int count_H, count_Y;
    count_H = scan_modem_mode_change_H(mode);
    count_Y = scan_modem_mode_change_Y(mode);
    
    if(count_H > 0 || count_Y > 0)
        oss_delay(15 * 1000);
}
////////////////////////////////////////////////////////////////////////////////////
int scan_modem_signal_quality_info_get(TModem *ptModem, char *buf, eoper_mode mode)
{
    scan_modem_signal_quality_info_get_H(ptModem, buf, mode);
    scan_modem_signal_quality_info_get_Y(ptModem, buf, mode);
}
/////////////////////////////////////////////////////////////////////////////
int scan_modem_connet_net(TModemLocal *pm, int conn_flag)
{
    int ret, i;
    char cmd_buf[128];

    for(i = 0; i< MODEM_NUM -1; i++)
    {
        if(NULL == pm)
            return RET_OK;

        sprintf(cmd_buf, "udhcpc -i usb%d &",pm->index);

        ret = scan_modem_connet_net_H(pm, conn_flag);
        if(RET_OK == ret){
            sleep(5);
            system(cmd_buf);
        }
            
        
        ret = scan_modem_connet_net_Y(pm, conn_flag);
        if(RET_OK == ret){ 
            sleep(2);
            system(cmd_buf);
            sleep(3);
        }
        
        printf("%s : cmd_buf=%s\n", __func__, cmd_buf);

        pm++;
    }

}
/////////////////////////////////////////////////////////////////////////////
//查找网络这次情况
void scan_modem_status_check()
{
    int i;

    TModemLocal *pm = pModem;

    for(i = 0; i < MODEM_NUM - 1; i++) {
        pm = &pModem[i];
        
        if(0 == pm->isvaild) {
            continue;
        }
        
        scan_modem_sim_oper_que(pm);
        scan_modem_net_register_check(pm);
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
    scan_modem_connet_net(pm, 1);

    while(1) {
        oss_delay(500);

        if(0 == scan_run_flag)
            continue;

        for (i = 0; i < MODEM_NUM - 1; i++) {
            pm = &pModem[i];

            if(0 == pm->isvaild) {
                printf("modem %d is scan_cfg_modem_uart_port_get failed!\n", i);
                continue;
            }
            if(pm->manu_id == HUAWEI) {
                scan_modem_signal_quality_info_get_H(&pm->atModem, info_buf + strlen(info_buf), pm->oper);
            } else if (pm->manu_id == YUGA) {
                scan_modem_signal_quality_info_get_Y(&pm->atModem, info_buf + strlen(info_buf), pm->oper);
            }
//            printf("modem %d -> signal quality: %s,oper=%d, mode=%d, regis_ok=%d\n", \
                pm->index, info_buf, pm->oper, pm->mode, pm->regis_ok);
        }

        scan_file_info_save(info_buf, pModem, MODEM_NUM - 1);
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
        ret = scan_cfg_modem_uart_port_get(i, pm->atModem.abyUartPort);
        if(ret == RET_FAILED) {
            pm->isvaild = 0;
            printf("modem %d is scan_cfg_modem_uart_port_get!\n", i);
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
        
        ret = scan_modem_sim_status_check(&pm->atModem); // 
        // if(ret) {
        //     scan_modem_sim_oper_que(pm);  //sim 卡运营商查询
        // }


        printf("i =%d, ret=%d, vaild=%d, fd=%d, oper=%d, mode=%d\n",  
            i, ret, pm->isvaild, pm->atModem.fd, pm->oper, pm->mode);
    }

    printf(" %s is ok!\n", __func__);
    scan_modem_status_check(pm);
    mode = scan_cfg_modem_oper_mode_get();
   
    scan_logic_oper_mode_set(mode);
    
    ret = scan_modem_mode_change(mode); 


 
}
////////////////////////////////////