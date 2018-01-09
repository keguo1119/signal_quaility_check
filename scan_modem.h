#ifndef __SCAN_MODEM_H__
#define __SCAN_MODEM_H__
#include "modem.h"

typedef enum Eoper_mode {
    NONE = 0,
    CMCC = 1,
    CUCC = 2,
    CTCC = 3
}eoper_mode;

typedef enum Estand_mode {
    NO_MODE     = 0,
    LTE         = 1,
    TD_SCDMA    = 2,
    WCDMA       = 3,
    CDMA           ,
    GSM             

}estand_mode;

typedef enum Emanu_id {
    HUAWEI  = 0,
    YUGA    = 1
}emanu_id;

//_H 代表华为命令，_Y代表 YUGA
typedef enum at_cmd_index {
    AT          = 0,
    ATE_SET     = 1,
    AT_CGMI_QUE     , //查询厂商;
    AT_CMEE_SET     , //=2 ,当发生错误的时候，显示详细信息
    AT_CGREG_SET    , //=2 ,打开PS域注册状态变化时的主动上报功
    
    AT_CPIN_QUE     , //查询是否可以识别SIM卡
    AT_CGREG_QUE    , //查询是否注册了本地网络；
    AT_CURC_SET_H     , //关闭部分主动上报，比如信号强度的上报
    AT_STSF_SET_H   , //关闭STK的主动上报  
    AT_CMER_SET_Y   , //關閉主動上報
    
    ATS_SET_H       , //关闭自动接听
    ATS_NVAUTO_SET_Y , //关闭自动接听
    AT_HCSQ_QUE_H   , // 华为查询信号质量，运营商制式查询
    AT_CCSQ_QUE_Y   , // YUGA查询信号质量
    AT_COPS_QUE     ,  //运行商查询
   
    AT_MODECONFIG_SET_Y, //运行商模式设置，运营商模式查询
    AT_NDISDUP_QUE  ,
    AT_NDISDUP_SET   
}e_at_cmd;

typedef struct TagModemLocal{

    int isvaild;
    int index;
    emanu_id  manu_id;
    eoper_mode oper;  //1 CM 移动，2 CN 联通 , 3 CT 电信
    estand_mode mode;
    TModem atModem;
}TModemLocal;


void scan_modem_init();
void scan_modem_run();
void scan_modem_status_check();
int  scan_modem_mode_change(int mode);
void scan_modem_run_flag_set(int flag);

#endif