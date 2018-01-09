#ifndef __LTE_CFG_H__
#define __LTE_CFG_H__

////////////////////////////////////////////////////////////////////////////////////
#define CMCC  1
#define CUCC  2
#define CTCC  3
////////////////////////////////////////////////////////////////////////////////////
#define FDD     (0)
#define TDD     (1)
////////////////////////////////////////////////////////////////////////////////////
//����������Ϣ�Ĳ���ֵ�ĺ��������仯�������ǰ�5�����ã�ÿ����Ų�3dB��
//��5��ʱ����ߣ�������-10dBm����
#define POWER_MIN   (1)
#define POWER_MAX   (5)
////////////////////////////////////////////////////////////////////////////////////
#define SCELL       (0)
#define INTRA_NCELL (1)
#define INTER_NCELL (2)
#define ENV_NCELL   (3)
////////////////////////////////////////////////////////////////////////////////////
#define GSM900   0   //1~124
#define DCS1800  1   //512~885
#define EGSM     2   //975~1023 
////////////////////////////////////////////////////////////////////////////////////
/* Access technology selected */
#define  GSM  		 0 
#define  GSM_COMPACT 1 
#define  UTRAN 		 2 
#define  EUTRAN 	 7 

////////////////////////////////////////////////////////////////////////////////////
/* preferred mode selection */
#define AUTOMATIC 	2
#define GSM_ONLY 	13
#define WCDMA_ONLY 	14
#define LTE_ONLY 	38
#define TDS_CDMA_ONLY 	59
#define CDMA_ONLY 	9
#define EVDO_ONLY 	10

//////////////////////////////////////////////////////////////////////////////////// 
#define  LTE_HOST_UDP_PORT       (9090)
#define  LTE_BOARD_UDP_PORT      (6630) //(2036)
#define  LTE_HOST_IMSI_UDP_PORT  (6630)
#define  LTE_APP_UDP_PORT        (7050) 
////////////////////////////////////////////////////////////////////////////////////
#define MOD_ID_TDD_CMCC1 33
#define MOD_ID_TDD_CMCC2 34
#define MOD_ID_FDD_CNU 35
#define MOD_ID_TDD_C2K 36
////////////////////////////////////////////////////////////////////////////////////
#define O_LTE_HEARTBEAT_TIMER  ((uint32_t)3000)
#define O_LTE_COMM_TIMER       ((uint32_t)3000)
#define O_LTE_CELL_GET_TIMER   ((uint32_t)5000)
#define O_LTE_STARTUP_TIMER    ((uint32_t)5000)
#define O_LTE_POWER_SET_TIMER  ((uint32_t)5000)
////////////////////////////////////////////////////////////////////////////////////
#define MAX_HEARTBEAT_LOST_NUM   (35)  //when tddfdd mode switch, the board will be reboot, it's about 53s,
#define MAX_SNIFFER_NUM          (8)

#define O_LTE_CFG_OK      1
#define O_LTE_CFG_FAILED  0

#define O_LTE_SYNC       1
#define O_LTE_NO_SYNC    0

#define O_LTE_START_OK       1
#define O_LTE_START_FAILED   0

#define O_LTE_RESET_ACCEPT  1
#define O_LTE_RESET_REJECT  0

#define O_LTE_USER_CFG_WORKMODE  0
#define O_LTE_SELF_CFG_WORKMODE  1

////////////////////////////////////////////////////////////////////////////////////
#define EVENT_LTE_HEARBEAT_REQ            (0x0001)      /*������Ϣ*/
#define EVENT_LTE_HEARBEAT_RSP            (0x0001)      /*������Ϣ��Ӧ*/

#define EVENT_LTE_START_REQ               (0x0002)      /* ��ʼ/ֹͣ��������*/
#define EVENT_LTE_START_RSP               (0x0002)      /* ��ʼ/ֹͣ����������Ӧ*/

#define EVENT_LTE_ALERTSMS_REQ            (0x0007)      /* ���ž�ʾ���� */
#define EVENT_LTE_ALERTSMS_RSP            (0x0007)      /* ���ž�ʾ������Ӧ */

#define	EVENT_LTE_RESET_REQ					(0x000B)    // ��վ��������
#define	EVENT_LTE_RESET_RSP					(0x000B)    // ��վ������Ӧ

#define EVENT_LTE_GET_CONFIG_RSP            (0x6001)    /* ��վ�����������ݲ�ѯ��Ӧ��Ϣ */

#define EVENT_LTE_SET_CONFIG_REQ            (0x6002)    /*��վ������������������Ϣ*/
#define EVENT_LTE_SET_CONFIG_RSP            (0x6002)    /*��վ������������������Ӧ��Ϣ*/

#define EVENT_LTE_LOAD_CONFIG_REQ           (0x6003)    /*��վ�����������ݼ���������Ϣ*/
#define EVENT_LTE_LOAD_CONFIG_RSP           (0x6003)    /*��վ�����������ݼ���������Ӧ��Ϣ*/

#define EVENT_LTE_SET_POWER_REQ             (0x6004)    /*��վ��������������Ϣ*/
#define EVENT_LTE_SET_POWER_RSP             (0x6004)    /*��վ��������������Ӧ��Ϣ*/

#define EVENT_LTE_PARAMENTER_RPT            (0x6005)    /*��վ���߻�������������Ϣ*/
#define EVENT_LTE_PARAMENTER_RSP            (0x6005)    /*��վ���߻�������������Ӧ��Ϣ*/

#define EVENT_LTE_REGISTER_NOTIFY_REQ       (0x6006)    /*ע��֪ͨ��Ϣ*/
#define EVENT_LTE_REGISTER_NOTIFY_RSP       (0x6006)    /*ע��֪ͨ��Ӧ��Ϣ*/

#define EVENT_LTE_START_SNIFFER_REQ         (0x6007)    /*����sniffer��Ϣ*/
#define EVENT_LTE_START_SNIFFER_RSP         (0x6007)    /*����sniffer��Ϣ*/

#define EVENT_LTE_PARA_UPDATE_REQ           (0x6008)    /*����"����"����������Ϣ*/
#define EVENT_LTE_PARA_UPDATE_RSP           (0x6008)    /*����"����"������Ӧ��Ϣ*/

#define EVENT_LTE_TDDFDD_SWITCH_REQ         (0x6010)    /* TDD/FDD�л�������Ϣ */
#define EVENT_LTE_TDDFDD_SWITCH_RSP         (0x6010)    /* TDD/FDD�л�������Ӧ��Ϣ */

#define EVENT_LTE_WORKMODE_REQ              (0x6011)    /* �忨����ģʽ��ѯ������Ϣ */
#define EVENT_LTE_WORKMODE_RSP              (0x6011)    /* �忨����ģʽ��ѯ��Ӧ��Ϣ */
//////////////////////////////////////////////////////
// from os to lte process
#define EVENT_LTE_TIMEOUT_START             (0x100)
#define EVENT_HEARTBEAT_TIMEOUT             EVENT_LTE_TIMEOUT_START
#define EVENT_COMM_TIMEOUT                  (EVENT_LTE_TIMEOUT_START + 1)
#define EVENT_CELL_GET_TIMEOUT              (EVENT_LTE_TIMEOUT_START + 2)
#define EVENT_STARTUP_TIMEOUT               (EVENT_LTE_TIMEOUT_START + 3)
#define EVENT_POWER_SET_TIMEOUT             (EVENT_LTE_TIMEOUT_START + 4)
#define EVENT_LTE_TIMEOUT_END               (0x120)

// from UI to lte process
#define EVENT_OAM_LTE_START                 (0x200)
#define EVENT_OAM_LTE_START_REQ             (EVENT_OAM_LTE_START)
#define EVENT_OAM_LTE_RESET_REQ             (EVENT_OAM_LTE_START + 1)
#define EVENT_OAM_LTE_SET_POWER_REQ         (EVENT_OAM_LTE_START + 2)
#define EVENT_OAM_LTE_SNIFFER_REQ           (EVENT_OAM_LTE_START + 3)

#define EVENT_OAM_HEARTBEAT_TIMEOUT          (EVENT_OAM_LTE_START + 20)
#define EVENT_OAM_ENV_SCAN_TIMEOUT           (EVENT_OAM_LTE_START + 21)
#define EVENT_OAM_CELLSPARA_SCAN_TIMEOUT     (EVENT_OAM_LTE_START + 22)

#define EVENT_OAM_LTE_STOP                  (0x220)

int lte_timeout_event(uint32_t event);
/////////////////////////////////////////////////////////////////////////////////////////
#define LTE_BOARD_STAT_INIT       		(0)
#define LTE_BOARD_STAT_POWERON    		(1)
#define LTE_BOARD_STAT_READY      		(2)
#define LTE_BOARD_STAT_RUNNING    		(3)
#define LTE_BOARD_STAT_EMERGENCY  		(8)
#define LTE_BOARD_STAT_SEARCH_FREQ 		(9)
#define LTE_BOARD_STAT_SYNC_WORKING 	(10)
#define LTE_BOARD_STAT_NOSYNC_WORKINIG  (11)
#define LTE_BOARD_STAT_SYNC_FAILURE     (12)

/////////////////////////////////////////////////////////////////////////////////////////
#define LTE_SYS_STAT_DISCONN       (0)
#define LTE_SYS_STAT_START_CFG     (1)
#define LTE_SYS_STAT_DATAREADY     (2)
#define LTE_SYS_STAT_RUNNING       (3) 
#define LTE_SYS_STAT_EMERGENCY     (8)
#define LTE_SYS_STAT_SCAN          (9) 
#define LTE_SYS_STAT_SYNC          (10)
#define LTE_SYS_STAT_NOSYNC        (11) 
#define LTE_SYS_STAT_SYNCFAILED    (12)
#define LTE_SYS_STAT_RSV           (4)  //4-����,5-������6-������7������
#define LTE_SYS_STAT_CELL_GET      (15)
///////////////////////////////////////////////////////////////////////////////////////// 
#define O_APP_HEARTBEAT_TIMER  ((uint32_t)1000)
///////////////////////////////////////////////////////////////////////////////////////// 
#define OP_OAM_LTE_START          (200)

#define OP_OAM_HEARBEAT_REQ       (OP_OAM_LTE_START + 0)
#define OP_OAM_HEARBEAT_RSP       (OP_OAM_LTE_START + 0)

#define OP_OAM_PARA_SET_REQ       (OP_OAM_LTE_START + 1)
#define OP_OAM_PARA_SET_RSP       (OP_OAM_LTE_START + 1)

#define OP_OAM_LTE_START_REQ      (OP_OAM_LTE_START + 2)
#define OP_OAM_LTE_START_RSP      (OP_OAM_LTE_START + 2)

#define OP_OAM_POWER_SET_REQ      (OP_OAM_LTE_START + 3)
#define OP_OAM_POWER_SET_RSP      (OP_OAM_LTE_START + 3)

#define OP_OAM_TARGET_SET_REQ     (OP_OAM_LTE_START + 5)
#define OP_OAM_TARGET_SET_RSP     (OP_OAM_LTE_START + 5)
#define OP_OAM_TARGET_CANCEL_REQ  (OP_OAM_LTE_START + 6)
#define OP_OAM_TARGET_CANCEL_RSP  (OP_OAM_LTE_START + 6)

#define OP_OAM_LTE_SNIFFER_REQ    (OP_OAM_LTE_START + 7)
#define OP_OAM_LTE_SNIFFER_RSP    (OP_OAM_LTE_START + 7)

#define OP_OAM_LTE_RESET_REQ      (OP_OAM_LTE_START + 8)
#define OP_OAM_LTE_RESET_RSP      (OP_OAM_LTE_START + 8)

#define OP_OAM_SNIFFER_INFO_IND   (OP_OAM_LTE_START + 9)
#define OP_OAM_IMSI_REPORT_IND    (OP_OAM_LTE_START + 10)
#define OP_OAM_TARGET_INFO_IND    (OP_OAM_LTE_START + 11)

#define OP_OAM_DRIVE_TO_GSM_REQ   (OP_OAM_LTE_START + 12)
#define OP_OAM_DRIVE_TO_GSM_RSP   (OP_OAM_LTE_START + 12)

#define OP_OAM_BAND_SET_REQ     (OP_OAM_LTE_START + 15)
#define OP_OAM_BAND_SET_RSP     (OP_OAM_LTE_START + 15)
#define OP_OAM_OPER_SET_REQ     (OP_OAM_LTE_START + 16)
#define OP_OAM_OPER_SET_RSP     (OP_OAM_LTE_START + 16)
#define OP_OAM_MODE_SET_REQ     (OP_OAM_LTE_START + 17)
#define OP_OAM_MODE_SET_RSP     (OP_OAM_LTE_START + 17)

#define OP_OAM_LTE_LOCATION_REQ (OP_OAM_LTE_START + 18)
#define OP_OAM_LTE_LOCATION_RSP (OP_OAM_LTE_START + 18)

//public scell and ncell parameters fresh
#define OP_OAM_CELLSPARA_SCAN_REQ   (OP_OAM_LTE_START + 19)
#define OP_OAM_CELLSPARA_SCAN_RSP   (OP_OAM_LTE_START + 19)

#define OP_OAM_PARA_UPDATE_IND  (OP_OAM_LTE_START + 20)

#define OP_OAM_CELL_PARA_IND    (OP_OAM_LTE_START + 21)
//Environment scan 
#define OP_OAM_ENV_SCAN_REQ     (OP_OAM_LTE_START + 22)
#define OP_OAM_ENV_SCAN_RSP     (OP_OAM_LTE_START + 22)

//LTE board FDD/TDD mode inquiry
#define OP_OAM_LTE_MODE_REQ     (OP_OAM_LTE_START + 28)
#define OP_OAM_LTE_MODE_IND     (OP_OAM_LTE_START + 28)

//SIM7100C version
#define OP_OAM_ENV_VER_REQ      (OP_OAM_LTE_START + 29)
#define OP_OAM_ENV_VER_IND      (OP_OAM_LTE_START + 29)

#define OP_OAM_LTE_ERR_IND      (OP_OAM_LTE_START + 30)

/////////////////////////////////////////////////////////////////////////////////////////
#define MAX_CELL_SEL_NUM   15
#define MAX_START_REQ_NUM  10

///////////////////////////////////////////////////////////////////////////////////////// 
#pragma pack (1)

typedef struct
{
    uint8_t bsId;
    uint8_t modId;
}mod_desc_t;

typedef struct tag_msg_head
{
    uint16_t msgType;
    uint16_t msgLen;
    mod_desc_t srcMod;
    mod_desc_t dstMod;
    uint16_t sessionId;
    uint16_t rsvd;
}msg_head_t;

typedef struct tag_lte_heartbeat_req
{
    msg_head_t msgHeader;                               /* ��Ϣͷ*/
} lte_heartbeat_req_t;

typedef struct tag_lte_heartbeat_rsp
{
	msg_head_t msgHeader;                               /* ��Ϣͷ*/
	uint16_t u16SysMode;                   /* ģ�鹤��״̬��1-��磬2-׼�������ݼ�����ɣ���3-�������У� ��վ�������У���4-����,5-������6-������7��������*��8-Ӧ������״̬��9-LTEɨƵ״̬��10-LTEͬ��������11-LTE��ͬ��������12-LTEͬ��ʧ��*/
} lte_heartbeat_rsp_t;
/*
״̬"10-LTEͬ��������11-LTE��ͬ������"��ָ��վ����sniffer����������������״̬�����ڻ�վ����sniffer�޽����ֻ�ܹ����ڷ�ͬ��״̬��
״̬"12-LTEͬ��ʧ��"��ָ��վ��Ҫ��ͬ��ʱ���������ڻ�վ����ģʽ�ұ�Ҫ��ͬ��������������ͬ������ʱ��Ӧ����"����"�澯��
*/

typedef struct tag_lte_start_req
{
	msg_head_t msgHeader;      /* ��Ϣͷ*/
	uint16_t u16Start; /* 0��ֹͣ��1����ʼ*/
} lte_start_req_t;

typedef struct tag_lte_start_rsp
{
	msg_head_t msgHeader;                 /* ��Ϣͷ*/
	uint8_t	u8StartInd;					//�ɹ�ָʾ��1���ɹ���0��ʧ��
	uint8_t	u8FailReason;					//ʧ��ԭ�򣬴���
} lte_start_rsp_t;

typedef struct tag_lte_reset_req
{
	msg_head_t msgHeader;               /* ��Ϣͷ*/
} lte_reset_req_t;


typedef struct tag_lte_reset_rsp
{
	msg_head_t msgHeader;                               /* ��Ϣͷ*/
	uint8_t	u8ResetInd;						//��������ָʾ��1�����ܲ�׼��������0���ܾ�
	uint8_t	u8FailReason;					//�ܾ�����ԭ�򣬴���
} lte_reset_rsp_t;

/* ��վ����*/
typedef struct tag_lte_pbs_para
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
}lte_pbs_para_t;

/*ͬ������*/
typedef struct tag_lte_syn_para
{
	uint8_t		u8Band; 	/*Ƶ��*/
	uint8_t 	u8Rsv;
	uint16_t	u16Earfcn;	/*Ƶ���*/
	uint16_t	u16PCI;	/*����С����ʶ*/
	uint16_t	u16Rsv;
}lte_syn_para_t;

/*����Ŀ�����*/
typedef struct tag_lte_rel_para
{
	uint8_t		u8Band; 	/*Ƶ��*/
	uint8_t 	u8Rsv;
	uint16_t	u16Arfcn;	/*Ƶ���*/
	uint8_t		u8ReleaseCause;		/*releaseԭ��*/
	uint8_t		u8RejectCause;		/*rejectԭ��*/
	uint16_t	u16Rsv;
}lte_rel_para_t;

/*ɨƵ����*/
typedef struct tag_lte_sniffer_para
{
	uint8_t		u8Band; 	/*Ƶ��*/
	uint8_t 	u8Rsv;
	uint16_t	u16Earfcn;	/*Ƶ���*/
}lte_sniffer_para_t;

typedef struct tag_lte_set_cfg_req
{
	msg_head_t msgHeader;       /* ��Ϣͷ*/
	uint16_t u16WorkMode;		/*����ģʽ��WorkMode=0��"����"����ģʽ������ֱ�Ӳ���"����"�����ò�����WorkMode=1��ɨƵ����ģʽ���������ɨƵ��������ɨƵ��Ȼ������*/
	uint16_t u16SynMode;        /* �Ƿ�ͬ�����������Ϊͬ����������PBS��Ҫ����ͬ����������ͬ�������� */
	lte_pbs_para_t 	strPbsPara;
	lte_syn_para_t 	strSynPara;
	lte_rel_para_t 	strRelPara;
	uint32_t		u32SnifferParaNum;					/*ɨƵ�����������*/
	lte_sniffer_para_t	strSnifferPara[MAX_SNIFFER_NUM];
} lte_set_cfg_req_t;

typedef struct tag_lte_set_cfg_rsp
{
	msg_head_t msgHeader;                              /* ��Ϣͷ*/
	uint8_t u8CfgInd;                                 /* �ɹ���ʶ��1���ɹ���0��ʧ��*/
} lte_set_cfg_rsp_t;

typedef struct tag_lte_load_cfg_req
{
    msg_head_t msgHeader;                              /* ��Ϣͷ*/ 
} lte_load_cfg_req_t;

typedef struct tag_lte_load_cfg_rsp
{
	msg_head_t msgHeader;                              /* ��Ϣͷ*/
	uint16_t u16WorkMode;		/*����ģʽ��WorkMode=0��"����"����ģʽ������ֱ�Ӳ���"����"�����ò�����WorkMode=1��ɨƵ����ģʽ���������ɨƵ��������ɨƵ��Ȼ������*/	
	uint16_t u16SynMode;         /*�Ƿ�ͬ�����������Ϊͬ����������PBS��Ҫ����ͬ����������ͬ��������*/
	lte_pbs_para_t 	strPbsPara;
	lte_syn_para_t 	strSynPara;
	lte_rel_para_t 	strRelPara;
	uint32_t		u32SnifferParaNum;					/*ɨƵ�����������*/
	lte_sniffer_para_t	strSnifferPara[MAX_SNIFFER_NUM];
} lte_load_cfg_rsp_t;

typedef struct tag_lte_set_power_req
{
	msg_head_t msgHeader;           /* ��Ϣͷ*/ 
	uint16_t u16Power; 				/* ���� */
} lte_set_power_req_t;

typedef struct tag_lte_set_power_rsp
{
	msg_head_t msgHeader;           /* ��Ϣͷ*/
	uint8_t	u8PowerInd;	        //�ɹ�ʧ��ָʾ	1���ɹ���0��ʧ��
	uint8_t	u8FailReasonInd;	//ʧ��ԭ�򡣴���
} lte_set_power_rsp_t;

typedef struct tag_lte_para
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
}lte_para_t;

typedef struct tag_lte_para_rpt
{
    msg_head_t msgHeader;                              /* ��Ϣͷ*/ 
	uint32_t	u32PCINum;	         /*PCI������Ҳ���ǽ���������߲�������ĸ���*/
	lte_para_t	strLtePara[8];
} lte_para_rpt_t;

typedef struct tag_lte_para_rsp
{
	msg_head_t msgHeader;                              /* ��Ϣͷ*/
} lte_para_rsp_t;

typedef struct tag_lte_mobile_notify_req
{
	msg_head_t msgHeader;                 /* ��Ϣͷ*/
	uint16_t	type;                   //xzluo according to udp packet
	uint8_t     u8IMSI[8];                   /* Ucd code, 4bit one number IMSI */
	uint8_t     u8ESN[8];
} lte_mobile_notify_req_t;

typedef struct tag_lte_mobile_notify_rsp
{
	msg_head_t msgHeader;                  /* ��Ϣͷ*/
	uint8_t     u8IMSI[8];                 /* Ucd code, 4bit one number IMSI */
	uint8_t 	acpt; /* 4Ϊ��λ */
	uint8_t 	u8Rsv2;
} lte_mobile_notify_rsp_t;

typedef struct tag_lte_start_sniffer_req
{
	msg_head_t msgHeader;                 /* ��Ϣͷ*/
} lte_start_sniffer_req_t;

typedef struct tag_lte_start_sniffer_rsp
{
	msg_head_t msgHeader;                  /* ��Ϣͷ*/
} lte_start_sniffer_rsp_t;

typedef struct tag_lte_para_update_req
{
	msg_head_t		msgHeader;                              /* ��Ϣͷ*/ 
	/*UpdateFlag=1����ʾ"����"���ݱ���Ϣ���²���*��
	  UpdateFlag=0����ʾû�����������ʵĹ���������"����"���ø��²���*/
	uint16_t		u16UpdateFlag; 
	/*�Ƿ�ͬ�����������Ϊͬ����������PBS��Ҫ����ͬ����������ͬ��������*/
	uint16_t		u16SynMode;    
	lte_pbs_para_t 	strPbsPara;
	lte_syn_para_t 	strSynPara;
} lte_para_update_req_t;

typedef struct tag_lte_para_update_rsp
{
	msg_head_t	msgHeader;                           /* ��Ϣͷ*/
	uint16_t	u16UpdateInd;						/*���³ɹ�Ϊ1��ʧ��Ϊ0*/
} lte_para_update_rsp_t;

//add xzluo 2016-2-24
typedef struct tag_lte_tddfdd_switch_req
{
	msg_head_t		msgHeader; 
    uint8_t         u8WorkMode;   /* 0: FDD, 1: TDD */
} lte_tddfdd_switch_req_t;

typedef struct tag_lte_tddfdd_switch_rsp
{
	msg_head_t	msgHeader; 
    uint8_t     u8Rsv1;
	uint8_t	    u8Flag;	   /* 1: success, 0: failure */
} lte_tddfdd_switch_rsp_t;

typedef struct tag_lte_workmode_req
{
	msg_head_t		msgHeader; 
} lte_workmode_req_t;

typedef struct tag_lte_workmode_rsp
{
	msg_head_t	msgHeader; 
    uint8_t     u8Rsv1;
	uint8_t	    u8WorkMode;	   /* 0: FDD, 1: TDD */
} lte_workmode_rsp_t;

#pragma pack ()

/////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
void lte_pbs_cfg_set(lte_pbs_para_t *ptPbsCfg, uint16_t mcc, uint8_t mnc, uint8_t band, uint16_t earfcn, uint16_t tac, uint16_t cellid, uint16_t pci, uint8_t pwr);
void lte_sync_cfg(lte_syn_para_t *ptSyncCfg, uint8_t band, uint16_t earfcn, uint16_t pci);
void lte_rel_cfg(lte_rel_para_t *ptRelCfg, uint8_t band, uint16_t arfcn, uint8_t rejCause, uint8_t relCause);
void lte_sniffer_cfg(lte_load_cfg_rsp_t *lte_cfg, uint8_t num, uint8_t *band, uint16_t *earfcn);


#endif 
