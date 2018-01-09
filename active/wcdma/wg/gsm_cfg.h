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
#define EVENT_GSM_HEARBEAT_REQ            (0x0001)      /*心跳消息*/
#define EVENT_GSM_HEARBEAT_RSP            (0x0001)      /*心跳消息响应*/

#define EVENT_GSM_START_REQ               (0x0002)      /* 开始/停止工作命令*/
#define EVENT_GSM_START_RSP               (0x0002)      /* 开始/停止工作命令响应*/

#define EVENT_GSM_ALERTSMS_REQ            (0x0007)      /* 短信警示命令 */
#define EVENT_GSM_ALERTSMS_RSP            (0x0007)      /* 短信警示命令响应 */

#define	EVENT_GSM_RESET_REQ					(0x000B)    // 基站重启命令
#define	EVENT_GSM_RESET_RSP					(0x000B)    // 基站重启响应

#define EVENT_GSM_SET_CONFIG_REQ            (0x6002)    /*基站工作配置数据设置消息*/
#define EVENT_GSM_SET_CONFIG_RSP            (0x6002)    /*基站工作配置数据设置响应消息*/

#define EVENT_GSM_LOAD_CONFIG_REQ           (0x6003)    /*基站工作配置数据加载请求消息*/
#define EVENT_GSM_LOAD_CONFIG_RSP           (0x6003)    /*基站工作配置数据加载请求响应消息*/

#define EVENT_GSM_SET_POWER_REQ             (0x6004)    /*基站功率设置请求消息*/
#define EVENT_GSM_SET_POWER_RSP             (0x6004)    /*基站功率设置请求响应消息*/

#define EVENT_GSM_PARAMENTER_RPT            (0x6005)    /*基站无线环境报告请求消息*/
#define EVENT_GSM_PARAMENTER_RSP            (0x6005)    /*基站无线环境报告请求响应消息*/

#define EVENT_GSM_REGISTER_NOTIFY_REQ       (0x6006)    /*注册通知消息*/
#define EVENT_GSM_REGISTER_NOTIFY_RSP       (0x6006)    /*注册通知响应消息*/

#define EVENT_GSM_START_SNIFFER_REQ         (0x6007)    /*启动sniffer消息*/
#define EVENT_GSM_START_SNIFFER_RSP         (0x6007)    /*启动sniffer消息*/

#define EVENT_GSM_PARA_UPDATE_REQ           (0x6008)    /*更新"界面"配置请求消息*/
#define EVENT_GSM_PARA_UPDATE_RSP           (0x6008)    /*更新"界面"配置响应消息*/

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
#define GSM_SYS_STAT_RSV           (4)  //4-保留,5-保留，6-保留，7－保留
#define GSM_SYS_STAT_CELL_GET      (15)
/////////////////////////////////////////////////////////////////////////////////////////
typedef struct tag_GSM_heartbeat_req
{
    wg_msg_head_t msgHeader;                               /* 消息头*/
} gsm_heartbeat_req_t;

typedef struct tag_GSM_heartbeat_rsp
{
	wg_msg_head_t msgHeader;                               /* 消息头*/
	uint16_t u16SysMode;                   /* 模块工作状态，1-起电，2-准备（数据加载完成），3-正常运行（ 基站正常运行），4-保留,5-保留，6-保留，7－保留）*，8-应急工作状态，9-LTE扫频状态，10-LTE同步工作，11-LTE非同步工作，12-LTE同步失败*/
} gsm_heartbeat_rsp_t;

typedef struct tag_GSM_reset_req
{
	oam_msg_head_t msgHeader;               /* 消息头*/
} oam_gsm_reset_req_t;

typedef struct tag_GSM_rf_set_req
{
	oam_msg_head_t msgHeader;                               /* 消息头*/
	uint16_t     flag;	
} oam_gsm_rf_set_req_t;

typedef struct tag_GSM_lbs_set_req
{
	oam_msg_head_t msgHeader;                               /* 消息头*/
	uint8_t	      flag;	
} oam_gsm_lbs_set_req_t;

typedef struct tag_GSM_alarm_set_req
{
	oam_msg_head_t msgHeader;                               /* 消息头*/
	uint8_t	      imsi[MAX_IMSI_LEN];	
} oam_gsm_alarm_set_req_t;

typedef oam_gsm_alarm_set_req_t oam_gsm_whitelist_set_req_t;

typedef struct tag_GSM_mobile_power_set_req
{
	oam_msg_head_t msgHeader;                               /* 消息头*/
	uint8_t	      power;	
} oam_gsm_mobile_power_set_req_t;

typedef struct tag_GSM_sms_send_req
{
	oam_msg_head_t msgHeader;                               /* 消息头*/
	uint8_t	      imsi[MAX_IMSI_LEN];	
	uint8_t	      phone[MAX_IMSI_LEN];	
	uint8_t	      content[256];	
} oam_gsm_sms_send_req_t;

typedef struct tag_GSM_amp_set_req
{
	oam_msg_head_t msgHeader;                               /* 消息头*/
	uint8_t	       cmd[256];	
} oam_gsm_amp_set_req_t;


typedef struct tag_GSM_para_set_req
{
	oam_msg_head_t msgHeader;                               /* 消息头*/
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



/* 基站参数*/
typedef struct tag_GSM_pbs_para
{
	uint8_t		u8Band; 	/*频段*/
	uint8_t		u8pwr;   /*功率*/
	uint16_t	u16Earfcn;	/*频点号*/
	uint16_t	u16MCC;	
	uint8_t		u8Mnc;		/*MNC*/
	uint8_t		u8InitialValueTag;	/*初始TAG*/
	uint16_t	u16PCI;	/*物理小区标识,PN*/
	uint16_t	u16Tac;			/*TAC*/
	uint16_t	u16CellId;		/*Cell Id*/
}gsm_pbs_para_t;

/*同步参数*/
typedef struct tag_GSM_syn_para
{
	uint8_t		u8Band; 	/*频段*/
	uint8_t 	u8Rsv;
	uint16_t	u16Earfcn;	/*频点号*/
	uint16_t	u16PCI;	/*物理小区标识*/
	uint16_t	u16Rsv;
}gsm_syn_para_t;

/*驱赶目标参数*/
typedef struct tag_GSM_rel_para
{
	uint8_t		u8Band; 	/*频段*/
	uint8_t 	u8Rsv;
	uint16_t	u16Arfcn;	/*频点号*/
	uint8_t		u8ReleaseCause;		/*release原因*/
	uint8_t		u8RejectCause;		/*reject原因*/
	uint16_t	u16Rsv;
}gsm_rel_para_t;

/*扫频参数*/
typedef struct tag_GSM_sniffer_para
{
	uint8_t		u8Band; 	/*频段*/
	uint8_t 	u8Rsv;
	uint16_t	u16Earfcn;	/*频点号*/
}gsm_sniffer_para_t;

typedef struct tag_GSM_set_cfg_req
{
	wg_msg_head_t msgHeader;       /* 消息头*/
	uint16_t u16WorkMode;		/*工作模式，WorkMode=0（"界面"配置模式），则直接采用"界面"的配置参数；WorkMode=1（扫频配置模式），则根据扫频参数进行扫频，然后启动*/
	uint16_t u16SynMode;        /* 是否同步工作，如果为同步工作，则PBS需要根据同步参数，先同步再启动 */
	gsm_pbs_para_t 	strPbsPara;
	gsm_syn_para_t 	strSynPara;
	gsm_rel_para_t 	strRelPara;
	uint32_t		u32SnifferParaNum;					/*扫频参数数组个数*/
	gsm_sniffer_para_t	strSnifferPara[MAX_SNIFFER_NUM];
} gsm_set_cfg_req_t;

typedef struct tag_GSM_set_cfg_rsp
{
	wg_msg_head_t msgHeader;                              /* 消息头*/
	uint8_t u8CfgInd;                                 /* 成功标识，1－成功，0－失败*/
} gsm_set_cfg_rsp_t;

typedef struct tag_GSM_load_cfg_req
{
    wg_msg_head_t msgHeader;                              /* 消息头*/ 
} gsm_load_cfg_req_t;

typedef struct tag_GSM_load_cfg_rsp
{
	wg_msg_head_t msgHeader;                              /* 消息头*/
	uint16_t u16WorkMode;		/*工作模式，WorkMode=0（"界面"配置模式），则直接采用"界面"的配置参数；WorkMode=1（扫频配置模式），则根据扫频参数进行扫频，然后启动*/	
	uint16_t u16SynMode;         /*是否同步工作，如果为同步工作，则PBS需要根据同步参数，先同步再启动*/
	gsm_pbs_para_t 	strPbsPara;
	gsm_syn_para_t 	strSynPara;
	gsm_rel_para_t 	strRelPara;
	uint32_t		u32SnifferParaNum;					/*扫频参数数组个数*/
	gsm_sniffer_para_t	strSnifferPara[MAX_SNIFFER_NUM];
} gsm_load_cfg_rsp_t;

typedef struct tag_GSM_set_power_req
{
	oam_msg_head_t msgHeader;           /* 消息头*/ 
	uint8_t power; 				/* 功率 */
} oam_gsm_set_power_req_t;

typedef struct tag_GSM_set_power_rsp
{
	wg_msg_head_t msgHeader;           /* 消息头*/
	uint8_t	u8PowerInd;	        //成功失败指示	1－成功，0－失败
	uint8_t	u8FailReasonInd;	//失败原因。待定
} gsm_set_power_rsp_t;

typedef struct tag_GSM_para
{
	uint8_t		u8Band; 				/*频段*/
	uint8_t 	u8Rsv;
	uint16_t	u16Earfcn;			/*频点号*/
	uint16_t	u16PCI;				/*物理小区标识*/
	uint8_t		u8PdschRefPower;		/*PDSCH参考功率*/
	uint8_t		u8Rsrp;				/*接收信号功率*/
	short	s16FreqOffset;			/*频偏，有正有负*/
	uint16_t	u16Tac;				/*TAC*/
	uint32_t	u32CellId;			/*Cell Id*/
}gsm_para_t;

typedef struct tag_GSM_para_rpt
{
    wg_msg_head_t msgHeader;                              /* 消息头*/ 
	uint32_t	u32PCINum;	         /*PCI个数，也就是紧跟其后无线参数数组的个数*/
	gsm_para_t	strLtePara[8];
} gsm_para_rpt_t;

typedef struct tag_GSM_para_rsp
{
	wg_msg_head_t msgHeader;                              /* 消息头*/
} gsm_para_rsp_t;

typedef struct tag_GSM_mobile_notify_req
{
	wg_msg_head_t msgHeader;                 /* 消息头*/
	uint8_t     u8IMSI[8];                   /* Ucd code, 4bit one number IMSI */
	uint8_t     u8ESN[8];
} gsm_mobile_notify_req_t;

typedef struct tag_GSM_mobile_notify_rsp
{
	wg_msg_head_t msgHeader;                  /* 消息头*/
} gsm_mobile_notify_rsp_t;

typedef struct tag_GSM_start_sniffer_req
{
	wg_msg_head_t msgHeader;                 /* 消息头*/
} gsm_start_sniffer_req_t;

typedef struct tag_GSM_start_sniffer_rsp
{
	wg_msg_head_t msgHeader;                  /* 消息头*/
} gsm_start_sniffer_rsp_t;

typedef struct tag_GSM_para_update_req
{
	wg_msg_head_t		msgHeader;                              /* 消息头*/ 
	/*UpdateFlag=1，表示"界面"根据本消息更新参数*；
	  UpdateFlag=0，表示没有搜索到合适的公网环境，"界面"不用更新参数*/
	uint16_t		u16UpdateFlag; 
	/*是否同步工作，如果为同步工作，则PBS需要根据同步参数，先同步再启动*/
	uint16_t		u16SynMode;    
	gsm_pbs_para_t 	strPbsPara;
	gsm_syn_para_t 	strSynPara;
} gsm_para_update_req_t;

typedef struct tag_GSM_para_update_rsp
{
	wg_msg_head_t	msgHeader;                           /* 消息头*/
	uint16_t	u16UpdateInd;						/*更新成功为1，失败为0*/
} gsm_para_update_rsp_t;

/////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
void gsm_pbs_cfg_set(gsm_pbs_para_t *ptPbsCfg, uint16_t mcc, uint8_t mnc, uint8_t band, uint16_t earfcn, uint16_t tac, uint16_t cellid, uint16_t pci, uint8_t pwr);
void gsm_sync_cfg(gsm_syn_para_t *ptSyncCfg, uint8_t band, uint16_t earfcn, uint16_t pci);
void gsm_rel_cfg(gsm_rel_para_t *ptRelCfg, uint8_t band, uint16_t arfcn, uint8_t rejCause, uint8_t relCause);
void gsm_sniffer_cfg(gsm_load_cfg_rsp_t *gsm_cfg, uint8_t num, uint8_t *band, uint16_t *earfcn);


#endif 