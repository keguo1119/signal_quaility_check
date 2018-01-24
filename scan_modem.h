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

//_H ����Ϊ���_Y���� YUGA
typedef enum at_cmd_index {
    AT          = 0,
    ATE_SET     = 1,
    AT_CGMI_QUE     , //��ѯ����;
    AT_CMEE_SET     , //=2 ,�����������ʱ����ʾ��ϸ��Ϣ
    AT_CGREG_SET    , //=2 ,��PS��ע��״̬�仯ʱ�������ϱ���
    
    AT_CPIN_QUE     , //��ѯ�Ƿ����ʶ��SIM��
    AT_CGREG_QUE    , //��ѯ�Ƿ�ע���˱������磻
    AT_CURC_SET_H     , //�رղ��������ϱ��������ź�ǿ�ȵ��ϱ�
    AT_STSF_SET_H   , //�ر�STK�������ϱ�  
    AT_CMER_SET_Y   , //�P�]�����ψ�
    
    ATS_SET_H       , //�ر��Զ�����
    ATS_NVAUTO_SET_Y , //�ر��Զ�����
    AT_HCSQ_QUE_H   , // ��Ϊ��ѯ�ź���������Ӫ����ʽ��ѯ
    AT_CCSQ_QUE_Y   , // YUGA��ѯ�ź�����
    AT_COPS_QUE     ,  //�����̲�ѯ
   
    AT_MODECONFIG_SET_Y, //������ģʽ���ã���Ӫ��ģʽ��ѯ
    AT_NDISDUP_QUE  ,
    AT_NDISDUP_SET_H,     //��Ϊģ�鲦��
    AT_QCRMCALL_SET_H     //���ģ�鲦��  
}e_at_cmd;

typedef struct TagModemLocal{

    int isvaild;
    int index;
    int regis_ok;
    emanu_id  manu_id;
    eoper_mode oper;  //1 CM �ƶ���2 CN ��ͨ , 3 CT ����
    estand_mode mode;
    TModem atModem;
}TModemLocal;


void scan_modem_init();
void scan_modem_run();
void scan_modem_status_check();
int  scan_modem_mode_change(int mode);

void scan_modem_run_start();
void scan_modem_run_stop();

#endif