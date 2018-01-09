#ifndef __WG_GSM_CFG_H__
#define __WG_GSM_CFG_H__

////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////
#define MOD_ID_TDD_CMCC1 33
#define MOD_ID_TDD_CMCC2 34
#define MOD_ID_FDD_CNU 35
#define MOD_ID_TDD_C2K 36
////////////////////////////////////////////////////////////////////////////////////
#define O_GSM_HEARTBEAT_TIMER  ((uint32_t)3000)
#define O_GSM_COMM_TIMER       ((uint32_t)3000)
#define O_GSM_CELL_GET_TIMER   ((uint32_t)5000)
#define O_GSM_STARTUP_TIMER    ((uint32_t)5000)
#define O_GSM_POWER_SET_TIMER  ((uint32_t)5000)

#define TIME_WG_GSM_CAP_GET    ((uint32_t)5000)
#define TIME_WG_GSM_LBS_START  ((uint32_t)5000)
////////////////////////////////////////////////////////////////////////////////////
#define MAX_HEARTBEAT_LOST_NUM   (5)
#define MAX_SNIFFER_NUM          (8)

#define O_GSM_CFG_OK      1
#define O_GSM_CFG_FAILED  0

#define O_GSM_SYNC       1
#define O_GSM_NO_SYNC    0

#define O_GSM_START_OK       1
#define O_GSM_START_FAILED   0

#define O_GSM_RESET_ACCEPT  1
#define O_GSM_RESET_REJECT  0

#define O_GSM_USER_CFG_WORKMODE  0
#define O_GSM_SELF_CFG_WORKMODE  1

////////////////////////////////////////////////////////////////////////////////////
#define EVENT_GSM_HEARBEAT_REQ            (0x0001)      /*������Ϣ*/
#define EVENT_GSM_HEARBEAT_RSP            (0x0001)      /*������Ϣ��Ӧ*/

#define EVENT_GSM_START_REQ               (0x0002)      /* ��ʼ/ֹͣ��������*/
#define EVENT_GSM_START_RSP               (0x0002)      /* ��ʼ/ֹͣ����������Ӧ*/

#define EVENT_GSM_ALERTSMS_REQ            (0x0007)      /* ���ž�ʾ���� */
#define EVENT_GSM_ALERTSMS_RSP            (0x0007)      /* ���ž�ʾ������Ӧ */

#define	EVENT_GSM_RESET_REQ					(0x000B)    // ��վ��������
#define	EVENT_GSM_RESET_RSP					(0x000B)    // ��վ������Ӧ

#define EVENT_GSM_SET_CONFIG_REQ            (0x6002)    /*��վ������������������Ϣ*/
#define EVENT_GSM_SET_CONFIG_RSP            (0x6002)    /*��վ������������������Ӧ��Ϣ*/

#define EVENT_GSM_LOAD_CONFIG_REQ           (0x6003)    /*��վ�����������ݼ���������Ϣ*/
#define EVENT_GSM_LOAD_CONFIG_RSP           (0x6003)    /*��վ�����������ݼ���������Ӧ��Ϣ*/

#define EVENT_GSM_SET_POWER_REQ             (0x6004)    /*��վ��������������Ϣ*/
#define EVENT_GSM_SET_POWER_RSP             (0x6004)    /*��վ��������������Ӧ��Ϣ*/

#define EVENT_GSM_PARAMENTER_RPT            (0x6005)    /*��վ���߻�������������Ϣ*/
#define EVENT_GSM_PARAMENTER_RSP            (0x6005)    /*��վ���߻�������������Ӧ��Ϣ*/

#define EVENT_GSM_REGISTER_NOTIFY_REQ       (0x6006)    /*ע��֪ͨ��Ϣ*/
#define EVENT_GSM_REGISTER_NOTIFY_RSP       (0x6006)    /*ע��֪ͨ��Ӧ��Ϣ*/

#define EVENT_GSM_START_SNIFFER_REQ         (0x6007)    /*����sniffer��Ϣ*/
#define EVENT_GSM_START_SNIFFER_RSP         (0x6007)    /*����sniffer��Ϣ*/

#define EVENT_GSM_PARA_UPDATE_REQ           (0x6008)    /*����"����"����������Ϣ*/
#define EVENT_GSM_PARA_UPDATE_RSP           (0x6008)    /*����"����"������Ӧ��Ϣ*/

//////////////////////////////////////////////////////

int gsm_timeout_event(uint32_t event);
/////////////////////////////////////////////////////////////////////////////////////////
#define GSM_SYS_STAT_DISCONN       (0)
#define GSM_SYS_STAT_START_CFG     (1)
#define GSM_SYS_STAT_DATAREADY     (2)
#define GSM_SYS_STAT_RUNNING       (3) 
#define GSM_SYS_STAT_EMERGENCY     (8)
#define GSM_SYS_STAT_SCAN          (9) 
#define GSM_SYS_STAT_SYNC          (10)
#define GSM_SYS_STAT_NOSYNC        (11) 
#define GSM_SYS_STAT_SYNCFAILED    (12)
#define GSM_SYS_STAT_RSV           (4)  //4-����,5-������6-������7������
#define GSM_SYS_STAT_CELL_GET      (15)
/////////////////////////////////////////////////////////////////////////////////////////
typedef struct tag_GSM_heartbeat_req
{
    wg_msg_head_t msgHeader;                               /* ��Ϣͷ*/
} gsm_heartbeat_req_t;

typedef struct tag_GSM_heartbeat_rsp
{
	wg_msg_head_t msgHeader;                               /* ��Ϣͷ*/
	uint16_t u16SysMode;                   /* ģ�鹤��״̬��1-��磬2-׼�������ݼ�����ɣ���3-�������У� ��վ�������У���4-����,5-������6-������7��������*��8-Ӧ������״̬��9-LTEɨƵ״̬��10-LTEͬ��������11-LTE��ͬ��������12-LTEͬ��ʧ��*/
} gsm_heartbeat_rsp_t;

typedef struct tag_GSM_reset_req
{
	oam_msg_head_t msgHeader;               /* ��Ϣͷ*/
} oam_gsm_reset_req_t;

typedef struct tag_GSM_rf_set_req
{
	oam_msg_head_t msgHeader;                               /* ��Ϣͷ*/
	uint16_t     flag;	
} oam_gsm_rf_set_req_t;

typedef struct tag_GSM_lbs_set_req
{
	oam_msg_head_t msgHeader;                               /* ��Ϣͷ*/
	uint8_t	      flag;	
} oam_gsm_lbs_set_req_t;

typedef struct tag_GSM_alarm_set_req
{
	oam_msg_head_t msgHeader;                               /* ��Ϣͷ*/
	uint8_t	      imsi[MAX_IMSI_LEN];	
} oam_gsm_alarm_set_req_t;

typedef oam_gsm_alarm_set_req_t oam_gsm_whitelist_set_req_t;

typedef struct tag_GSM_mobile_power_set_req
{
	oam_msg_head_t msgHeader;                               /* ��Ϣͷ*/
	uint8_t	      power;	
} oam_gsm_mobile_power_set_req_t;

typedef struct tag_GSM_sms_send_req
{
	oam_msg_head_t msgHeader;                               /* ��Ϣͷ*/
	uint8_t	      imsi[MAX_IMSI_LEN];	
	uint8_t	      phone[MAX_IMSI_LEN];	
	uint8_t	      content[256];	
} oam_gsm_sms_send_req_t;

typedef struct tag_GSM_amp_set_req
{
	oam_msg_head_t msgHeader;                               /* ��Ϣͷ*/
	uint8_t	       cmd[256];	
} oam_gsm_amp_set_req_t;


typedef struct tag_GSM_para_set_req
{
	oam_msg_head_t msgHeader;                               /* ��Ϣͷ*/
	uint16_t mcc;
	uint8_t  mnc;
	uint16_t power;
	uint16_t lac;
	uint16_t cid;
	uint8_t  bsic;
	uint16_t arfcn0;
	uint16_t arfcn1;
	uint8_t  nb_cell_num;
	uint16_t nb_arfcn[MAX_CELL_NUM];
	uint8_t  rach;
	uint32_t tmsi;
} oam_gsm_para_set_req_t;

typedef struct tag_GSM_time_set_req
{
	oam_msg_head_t msgHeader;     
	uint8_t year[16];
	uint8_t month[16];
	uint8_t day[16];
	uint8_t hour[16];
	uint8_t minute[16];
	uint8_t second[16];
}oam_gsm_time_set_req_t;



/* ��վ����*/
typedef struct tag_GSM_pbs_para
{
	uint8_t		u8Band; 	/*Ƶ��*/
	uint8_t		u8pwr;   /*����*/
	uint16_t	u16Earfcn;	/*Ƶ���*/
	uint16_t	u16MCC;	
	uint8_t		u8Mnc;		/*MNC*/
	uint8_t		u8InitialValueTag;	/*��ʼTAG*/
	uint16_t	u16PCI;	/*����С����ʶ,PN*/
	uint16_t	u16Tac;			/*TAC*/
	uint16_t	u16CellId;		/*Cell Id*/
}gsm_pbs_para_t;

/*ͬ������*/
typedef struct tag_GSM_syn_para
{
	uint8_t		u8Band; 	/*Ƶ��*/
	uint8_t 	u8Rsv;
	uint16_t	u16Earfcn;	/*Ƶ���*/
	uint16_t	u16PCI;	/*����С����ʶ*/
	uint16_t	u16Rsv;
}gsm_syn_para_t;

/*����Ŀ�����*/
typedef struct tag_GSM_rel_para
{
	uint8_t		u8Band; 	/*Ƶ��*/
	uint8_t 	u8Rsv;
	uint16_t	u16Arfcn;	/*Ƶ���*/
	uint8_t		u8ReleaseCause;		/*releaseԭ��*/
	uint8_t		u8RejectCause;		/*rejectԭ��*/
	uint16_t	u16Rsv;
}gsm_rel_para_t;

/*ɨƵ����*/
typedef struct tag_GSM_sniffer_para
{
	uint8_t		u8Band; 	/*Ƶ��*/
	uint8_t 	u8Rsv;
	uint16_t	u16Earfcn;	/*Ƶ���*/
}gsm_sniffer_para_t;

typedef struct tag_GSM_set_cfg_req
{
	wg_msg_head_t msgHeader;       /* ��Ϣͷ*/
	uint16_t u16WorkMode;		/*����ģʽ��WorkMode=0��"����"����ģʽ������ֱ�Ӳ���"����"�����ò�����WorkMode=1��ɨƵ����ģʽ���������ɨƵ��������ɨƵ��Ȼ������*/
	uint16_t u16SynMode;        /* �Ƿ�ͬ�����������Ϊͬ����������PBS��Ҫ����ͬ����������ͬ�������� */
	gsm_pbs_para_t 	strPbsPara;
	gsm_syn_para_t 	strSynPara;
	gsm_rel_para_t 	strRelPara;
	uint32_t		u32SnifferParaNum;					/*ɨƵ�����������*/
	gsm_sniffer_para_t	strSnifferPara[MAX_SNIFFER_NUM];
} gsm_set_cfg_req_t;

typedef struct tag_GSM_set_cfg_rsp
{
	wg_msg_head_t msgHeader;                              /* ��Ϣͷ*/
	uint8_t u8CfgInd;                                 /* �ɹ���ʶ��1���ɹ���0��ʧ��*/
} gsm_set_cfg_rsp_t;

typedef struct tag_GSM_load_cfg_req
{
    wg_msg_head_t msgHeader;                              /* ��Ϣͷ*/ 
} gsm_load_cfg_req_t;

typedef struct tag_GSM_load_cfg_rsp
{
	wg_msg_head_t msgHeader;                              /* ��Ϣͷ*/
	uint16_t u16WorkMode;		/*����ģʽ��WorkMode=0��"����"����ģʽ������ֱ�Ӳ���"����"�����ò�����WorkMode=1��ɨƵ����ģʽ���������ɨƵ��������ɨƵ��Ȼ������*/	
	uint16_t u16SynMode;         /*�Ƿ�ͬ�����������Ϊͬ����������PBS��Ҫ����ͬ����������ͬ��������*/
	gsm_pbs_para_t 	strPbsPara;
	gsm_syn_para_t 	strSynPara;
	gsm_rel_para_t 	strRelPara;
	uint32_t		u32SnifferParaNum;					/*ɨƵ�����������*/
	gsm_sniffer_para_t	strSnifferPara[MAX_SNIFFER_NUM];
} gsm_load_cfg_rsp_t;

typedef struct tag_GSM_set_power_req
{
	oam_msg_head_t msgHeader;           /* ��Ϣͷ*/ 
	uint8_t power; 				/* ���� */
} oam_gsm_set_power_req_t;

typedef struct tag_GSM_set_power_rsp
{
	wg_msg_head_t msgHeader;           /* ��Ϣͷ*/
	uint8_t	u8PowerInd;	        //�ɹ�ʧ��ָʾ	1���ɹ���0��ʧ��
	uint8_t	u8FailReasonInd;	//ʧ��ԭ�򡣴���
} gsm_set_power_rsp_t;

typedef struct tag_GSM_para
{
	uint8_t		u8Band; 				/*Ƶ��*/
	uint8_t 	u8Rsv;
	uint16_t	u16Earfcn;			/*Ƶ���*/
	uint16_t	u16PCI;				/*����С����ʶ*/
	uint8_t		u8PdschRefPower;		/*PDSCH�ο�����*/
	uint8_t		u8Rsrp;				/*�����źŹ���*/
	short	s16FreqOffset;			/*Ƶƫ�������и�*/
	uint16_t	u16Tac;				/*TAC*/
	uint32_t	u32CellId;			/*Cell Id*/
}gsm_para_t;

typedef struct tag_GSM_para_rpt
{
    wg_msg_head_t msgHeader;                              /* ��Ϣͷ*/ 
	uint32_t	u32PCINum;	         /*PCI������Ҳ���ǽ���������߲�������ĸ���*/
	gsm_para_t	strLtePara[8];
} gsm_para_rpt_t;

typedef struct tag_GSM_para_rsp
{
	wg_msg_head_t msgHeader;                              /* ��Ϣͷ*/
} gsm_para_rsp_t;

typedef struct tag_GSM_mobile_notify_req
{
	wg_msg_head_t msgHeader;                 /* ��Ϣͷ*/
	uint8_t     u8IMSI[8];                   /* Ucd code, 4bit one number IMSI */
	uint8_t     u8ESN[8];
} gsm_mobile_notify_req_t;

typedef struct tag_GSM_mobile_notify_rsp
{
	wg_msg_head_t msgHeader;                  /* ��Ϣͷ*/
} gsm_mobile_notify_rsp_t;

typedef struct tag_GSM_start_sniffer_req
{
	wg_msg_head_t msgHeader;                 /* ��Ϣͷ*/
} gsm_start_sniffer_req_t;

typedef struct tag_GSM_start_sniffer_rsp
{
	wg_msg_head_t msgHeader;                  /* ��Ϣͷ*/
} gsm_start_sniffer_rsp_t;

typedef struct tag_GSM_para_update_req
{
	wg_msg_head_t		msgHeader;                              /* ��Ϣͷ*/ 
	/*UpdateFlag=1����ʾ"����"���ݱ���Ϣ���²���*��
	  UpdateFlag=0����ʾû�����������ʵĹ���������"����"���ø��²���*/
	uint16_t		u16UpdateFlag; 
	/*�Ƿ�ͬ�����������Ϊͬ����������PBS��Ҫ����ͬ����������ͬ��������*/
	uint16_t		u16SynMode;    
	gsm_pbs_para_t 	strPbsPara;
	gsm_syn_para_t 	strSynPara;
} gsm_para_update_req_t;

typedef struct tag_GSM_para_update_rsp
{
	wg_msg_head_t	msgHeader;                           /* ��Ϣͷ*/
	uint16_t	u16UpdateInd;						/*���³ɹ�Ϊ1��ʧ��Ϊ0*/
} gsm_para_update_rsp_t;

/////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
void gsm_pbs_cfg_set(gsm_pbs_para_t *ptPbsCfg, uint16_t mcc, uint8_t mnc, uint8_t band, uint16_t earfcn, uint16_t tac, uint16_t cellid, uint16_t pci, uint8_t pwr);
void gsm_sync_cfg(gsm_syn_para_t *ptSyncCfg, uint8_t band, uint16_t earfcn, uint16_t pci);
void gsm_rel_cfg(gsm_rel_para_t *ptRelCfg, uint8_t band, uint16_t arfcn, uint8_t rejCause, uint8_t relCause);
void gsm_sniffer_cfg(gsm_load_cfg_rsp_t *gsm_cfg, uint8_t num, uint8_t *band, uint16_t *earfcn);


#endif 