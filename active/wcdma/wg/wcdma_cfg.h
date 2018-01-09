#ifndef __WCDMA_CFG_H__
#define __WCDMA_CFG_H__

////////////////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////////////////
#define MOD_ID_TDD_CMCC1 33
#define MOD_ID_TDD_CMCC2 34
#define MOD_ID_FDD_CNU 35
#define MOD_ID_TDD_C2K 36
////////////////////////////////////////////////////////////////////////////////////
#define O_WCDMA_HEARTBEAT_TIMER  ((uint32_t)3000)
#define O_WCDMA_COMM_TIMER       ((uint32_t)3000)
#define O_WCDMA_CELL_GET_TIMER   ((uint32_t)5000)
#define O_WCDMA_STARTUP_TIMER    ((uint32_t)5000)
#define O_WCDMA_POWER_SET_TIMER  ((uint32_t)5000)
////////////////////////////////////////////////////////////////////////////////////
#define MAX_HEARTBEAT_LOST_NUM   (5)
#define MAX_SNIFFER_NUM          (8)

#define O_WCDMA_CFG_OK      1
#define O_WCDMA_CFG_FAILED  0

#define O_WCDMA_SYNC       1
#define O_WCDMA_NO_SYNC    0

#define O_WCDMA_START_OK       1
#define O_WCDMA_START_FAILED   0

#define O_WCDMA_RESET_ACCEPT  1
#define O_WCDMA_RESET_REJECT  0

#define O_WCDMA_USER_CFG_WORKMODE  0
#define O_WCDMA_SELF_CFG_WORKMODE  1

////////////////////////////////////////////////////////////////////////////////////
#define EVENT_WCDMA_HEARBEAT_REQ            (0x0001)      /*������Ϣ*/
#define EVENT_WCDMA_HEARBEAT_RSP            (0x0001)      /*������Ϣ��Ӧ*/

#define EVENT_WCDMA_START_REQ               (0x0002)      /* ��ʼ/ֹͣ��������*/
#define EVENT_WCDMA_START_RSP               (0x0002)      /* ��ʼ/ֹͣ����������Ӧ*/

#define EVENT_WCDMA_ALERTSMS_REQ            (0x0007)      /* ���ž�ʾ���� */
#define EVENT_WCDMA_ALERTSMS_RSP            (0x0007)      /* ���ž�ʾ������Ӧ */

#define	EVENT_WCDMA_RESET_REQ					(0x000B)    // ��վ��������
#define	EVENT_WCDMA_RESET_RSP					(0x000B)    // ��վ������Ӧ

#define EVENT_WCDMA_SET_CONFIG_REQ            (0x6002)    /*��վ������������������Ϣ*/
#define EVENT_WCDMA_SET_CONFIG_RSP            (0x6002)    /*��վ������������������Ӧ��Ϣ*/

#define EVENT_WCDMA_LOAD_CONFIG_REQ           (0x6003)    /*��վ�����������ݼ���������Ϣ*/
#define EVENT_WCDMA_LOAD_CONFIG_RSP           (0x6003)    /*��վ�����������ݼ���������Ӧ��Ϣ*/

#define EVENT_WCDMA_SET_POWER_REQ             (0x6004)    /*��վ��������������Ϣ*/
#define EVENT_WCDMA_SET_POWER_RSP             (0x6004)    /*��վ��������������Ӧ��Ϣ*/

#define EVENT_WCDMA_PARAMENTER_RPT            (0x6005)    /*��վ���߻�������������Ϣ*/
#define EVENT_WCDMA_PARAMENTER_RSP            (0x6005)    /*��վ���߻�������������Ӧ��Ϣ*/

#define EVENT_WCDMA_REGISTER_NOTIFY_REQ       (0x6006)    /*ע��֪ͨ��Ϣ*/
#define EVENT_WCDMA_REGISTER_NOTIFY_RSP       (0x6006)    /*ע��֪ͨ��Ӧ��Ϣ*/

#define EVENT_WCDMA_START_SNIFFER_REQ         (0x6007)    /*����sniffer��Ϣ*/
#define EVENT_WCDMA_START_SNIFFER_RSP         (0x6007)    /*����sniffer��Ϣ*/

#define EVENT_WCDMA_PARA_UPDATE_REQ           (0x6008)    /*����"����"����������Ϣ*/
#define EVENT_WCDMA_PARA_UPDATE_RSP           (0x6008)    /*����"����"������Ӧ��Ϣ*/

//////////////////////////////////////////////////////
// from os to lte process

int wcdma_timeout_event(uint32_t event);
/////////////////////////////////////////////////////////////////////////////////////////
#define WCDMA_SYS_STAT_DISCONN       (0)
#define WCDMA_SYS_STAT_START_CFG     (1)
#define WCDMA_SYS_STAT_DATAREADY     (2)
#define WCDMA_SYS_STAT_RUNNING       (3) 
#define WCDMA_SYS_STAT_EMERGENCY     (8)
#define WCDMA_SYS_STAT_SCAN          (9) 
#define WCDMA_SYS_STAT_SYNC          (10)
#define WCDMA_SYS_STAT_NOSYNC        (11) 
#define WCDMA_SYS_STAT_SYNCFAILED    (12)
#define WCDMA_SYS_STAT_RSV           (4)  //4-����,5-������6-������7������
#define WCDMA_SYS_STAT_CELL_GET      (15)
/////////////////////////////////////////////////////////////////////////////////////////
typedef struct tag_WCDMA_heartbeat_req
{
    wg_msg_head_t msgHeader;                               /* ��Ϣͷ*/
} wcdma_heartbeat_req_t;

typedef struct tag_WCDMA_heartbeat_rsp
{
	wg_msg_head_t msgHeader;                               /* ��Ϣͷ*/
	uint16_t u16SysMode;                   /* ģ�鹤��״̬��1-��磬2-׼�������ݼ�����ɣ���3-�������У� ��վ�������У���4-����,5-������6-������7��������*��8-Ӧ������״̬��9-LTEɨƵ״̬��10-LTEͬ��������11-LTE��ͬ��������12-LTEͬ��ʧ��*/
} wcdma_heartbeat_rsp_t;
/*
״̬"10-LTEͬ��������11-LTE��ͬ������"��ָ��վ����sniffer����������������״̬�����ڻ�վ����sniffer�޽����ֻ�ܹ����ڷ�ͬ��״̬��
״̬"12-LTEͬ��ʧ��"��ָ��վ��Ҫ��ͬ��ʱ���������ڻ�վ����ģʽ�ұ�Ҫ��ͬ��������������ͬ������ʱ��Ӧ����"����"�澯��
*/

typedef struct tag_WCDMA_start_req
{
	wg_msg_head_t msgHeader;      /* ��Ϣͷ*/
	uint16_t u16Start; /* 0��ֹͣ��1����ʼ*/
} wcdma_start_req_t;

typedef struct tag_WCDMA_start_rsp
{
	wg_msg_head_t msgHeader;                 /* ��Ϣͷ*/
	uint8_t	u8StartInd;					//�ɹ�ָʾ��1���ɹ���0��ʧ��
	uint8_t	u8FailReason;					//ʧ��ԭ�򣬴���
} wcdma_start_rsp_t;

typedef struct tag_WCDMA_reset_req
{
	oam_msg_head_t msgHeader;               /* ��Ϣͷ*/
} oam_wcdma_reset_req_t;


typedef struct tag_WCDMA_reset_rsp
{
	wg_msg_head_t msgHeader;                               /* ��Ϣͷ*/
	uint8_t	u8ResetInd;						//��������ָʾ��1�����ܲ�׼��������0���ܾ�
	uint8_t	u8FailReason;					//�ܾ�����ԭ�򣬴���
} wcdma_reset_rsp_t;

typedef struct tag_WCDMA_rf_set_req
{
	oam_msg_head_t msgHeader;                 /* ��Ϣͷ*/
	uint8_t	flag;
} oam_wcdma_rf_set_req_t;

typedef struct tag_WCDMA_lbs_set_req
{
	oam_msg_head_t msgHeader;                 /* ��Ϣͷ*/
	uint8_t	flag;
} oam_wcdma_lbs_set_req_t;

typedef struct tag_WCDMA_para_set_req
{
	oam_msg_head_t msgHeader;                 /* ��Ϣͷ*/
	uint16_t mcc;
	uint8_t  mnc;
	uint16_t power;
	uint16_t lac;
	uint16_t cid;
	uint8_t  bsic;
	uint8_t  rac;
	uint16_t arfcn0;
} oam_wcdma_para_set_req_t;


/* ��վ����*/
typedef struct tag_WCDMA_pbs_para
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
}wcdma_pbs_para_t;

/*ͬ������*/
typedef struct tag_WCDMA_syn_para
{
	uint8_t		u8Band; 	/*Ƶ��*/
	uint8_t 	u8Rsv;
	uint16_t	u16Earfcn;	/*Ƶ���*/
	uint16_t	u16PCI;	/*����С����ʶ*/
	uint16_t	u16Rsv;
}wcdma_syn_para_t;

/*����Ŀ�����*/
typedef struct tag_WCDMA_rel_para
{
	uint8_t		u8Band; 	/*Ƶ��*/
	uint8_t 	u8Rsv;
	uint16_t	u16Arfcn;	/*Ƶ���*/
	uint8_t		u8ReleaseCause;		/*releaseԭ��*/
	uint8_t		u8RejectCause;		/*rejectԭ��*/
	uint16_t	u16Rsv;
}wcdma_rel_para_t;

/*ɨƵ����*/
typedef struct tag_WCDMA_sniffer_para
{
	uint8_t		u8Band; 	/*Ƶ��*/
	uint8_t 	u8Rsv;
	uint16_t	u16Earfcn;	/*Ƶ���*/
}wcdma_sniffer_para_t;

typedef struct tag_WCDMA_set_cfg_req
{
	wg_msg_head_t msgHeader;       /* ��Ϣͷ*/
	uint16_t u16WorkMode;		/*����ģʽ��WorkMode=0��"����"����ģʽ������ֱ�Ӳ���"����"�����ò�����WorkMode=1��ɨƵ����ģʽ���������ɨƵ��������ɨƵ��Ȼ������*/
	uint16_t u16SynMode;        /* �Ƿ�ͬ�����������Ϊͬ����������PBS��Ҫ����ͬ����������ͬ�������� */
	wcdma_pbs_para_t 	strPbsPara;
	wcdma_syn_para_t 	strSynPara;
	wcdma_rel_para_t 	strRelPara;
	uint32_t		u32SnifferParaNum;					/*ɨƵ�����������*/
	wcdma_sniffer_para_t	strSnifferPara[MAX_SNIFFER_NUM];
} wcdma_set_cfg_req_t;

typedef struct tag_WCDMA_set_cfg_rsp
{
	wg_msg_head_t msgHeader;                              /* ��Ϣͷ*/
	uint8_t u8CfgInd;                                 /* �ɹ���ʶ��1���ɹ���0��ʧ��*/
} wcdma_set_cfg_rsp_t;

typedef struct tag_WCDMA_load_cfg_req
{
    wg_msg_head_t msgHeader;                              /* ��Ϣͷ*/ 
} wcdma_load_cfg_req_t;

typedef struct tag_WCDMA_load_cfg_rsp
{
	wg_msg_head_t msgHeader;                              /* ��Ϣͷ*/
	uint16_t u16WorkMode;		/*����ģʽ��WorkMode=0��"����"����ģʽ������ֱ�Ӳ���"����"�����ò�����WorkMode=1��ɨƵ����ģʽ���������ɨƵ��������ɨƵ��Ȼ������*/	
	uint16_t u16SynMode;         /*�Ƿ�ͬ�����������Ϊͬ����������PBS��Ҫ����ͬ����������ͬ��������*/
	wcdma_pbs_para_t 	strPbsPara;
	wcdma_syn_para_t 	strSynPara;
	wcdma_rel_para_t 	strRelPara;
	uint32_t		u32SnifferParaNum;					/*ɨƵ�����������*/
	wcdma_sniffer_para_t	strSnifferPara[MAX_SNIFFER_NUM];
} wcdma_load_cfg_rsp_t;

typedef struct tag_WCDMA_set_power_req
{
	oam_msg_head_t msgHeader;           /* ��Ϣͷ*/ 
	uint16_t u16Power; 				/* ���� */
} oam_wcdma_set_power_req_t;

typedef struct tag_WCDMA_set_power_rsp
{
	wg_msg_head_t msgHeader;           /* ��Ϣͷ*/
	uint8_t	u8PowerInd;	        //�ɹ�ʧ��ָʾ	1���ɹ���0��ʧ��
	uint8_t	u8FailReasonInd;	//ʧ��ԭ�򡣴���
} wcdma_set_power_rsp_t;

typedef struct tag_WCDMA_para
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
}wcdma_para_t;

typedef struct tag_WCDMA_para_rpt
{
    wg_msg_head_t msgHeader;                              /* ��Ϣͷ*/ 
	uint32_t	u32PCINum;	         /*PCI������Ҳ���ǽ���������߲�������ĸ���*/
	wcdma_para_t	strLtePara[8];
} wcdma_para_rpt_t;

typedef struct tag_WCDMA_para_rsp
{
	wg_msg_head_t msgHeader;                              /* ��Ϣͷ*/
} wcdma_para_rsp_t;

typedef struct tag_WCDMA_mobile_notify_req
{
	wg_msg_head_t msgHeader;                 /* ��Ϣͷ*/
	uint8_t     u8IMSI[8];                   /* Ucd code, 4bit one number IMSI */
	uint8_t     u8ESN[8];
} wcdma_mobile_notify_req_t;

typedef struct tag_WCDMA_mobile_notify_rsp
{
	wg_msg_head_t msgHeader;                  /* ��Ϣͷ*/
} wcdma_mobile_notify_rsp_t;

typedef struct tag_WCDMA_start_sniffer_req
{
	wg_msg_head_t msgHeader;                 /* ��Ϣͷ*/
} wcdma_start_sniffer_req_t;

typedef struct tag_WCDMA_start_sniffer_rsp
{
	wg_msg_head_t msgHeader;                  /* ��Ϣͷ*/
} wcdma_start_sniffer_rsp_t;

typedef struct tag_WCDMA_para_update_req
{
	wg_msg_head_t		msgHeader;                              /* ��Ϣͷ*/ 
	/*UpdateFlag=1����ʾ"����"���ݱ���Ϣ���²���*��
	  UpdateFlag=0����ʾû�����������ʵĹ���������"����"���ø��²���*/
	uint16_t		u16UpdateFlag; 
	/*�Ƿ�ͬ�����������Ϊͬ����������PBS��Ҫ����ͬ����������ͬ��������*/
	uint16_t		u16SynMode;    
	wcdma_pbs_para_t 	strPbsPara;
	wcdma_syn_para_t 	strSynPara;
} wcdma_para_update_req_t;

typedef struct tag_WCDMA_para_update_rsp
{
	wg_msg_head_t	msgHeader;                           /* ��Ϣͷ*/
	uint16_t	u16UpdateInd;						/*���³ɹ�Ϊ1��ʧ��Ϊ0*/
} wcdma_para_update_rsp_t;

/////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
void wcdma_pbs_cfg_set(wcdma_pbs_para_t *ptPbsCfg, uint16_t mcc, uint8_t mnc, uint8_t band, uint16_t earfcn, uint16_t tac, uint16_t cellid, uint16_t pci, uint8_t pwr);
void wcdma_sync_cfg(wcdma_syn_para_t *ptSyncCfg, uint8_t band, uint16_t earfcn, uint16_t pci);
void wcdma_rel_cfg(wcdma_rel_para_t *ptRelCfg, uint8_t band, uint16_t arfcn, uint8_t rejCause, uint8_t relCause);
void wcdma_sniffer_cfg(wcdma_load_cfg_rsp_t *wcdma_cfg, uint8_t num, uint8_t *band, uint16_t *earfcn);


#endif 