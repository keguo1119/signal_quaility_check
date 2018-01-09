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
#define EVENT_WCDMA_HEARBEAT_REQ            (0x0001)      /*心跳消息*/
#define EVENT_WCDMA_HEARBEAT_RSP            (0x0001)      /*心跳消息响应*/

#define EVENT_WCDMA_START_REQ               (0x0002)      /* 开始/停止工作命令*/
#define EVENT_WCDMA_START_RSP               (0x0002)      /* 开始/停止工作命令响应*/

#define EVENT_WCDMA_ALERTSMS_REQ            (0x0007)      /* 短信警示命令 */
#define EVENT_WCDMA_ALERTSMS_RSP            (0x0007)      /* 短信警示命令响应 */

#define	EVENT_WCDMA_RESET_REQ					(0x000B)    // 基站重启命令
#define	EVENT_WCDMA_RESET_RSP					(0x000B)    // 基站重启响应

#define EVENT_WCDMA_SET_CONFIG_REQ            (0x6002)    /*基站工作配置数据设置消息*/
#define EVENT_WCDMA_SET_CONFIG_RSP            (0x6002)    /*基站工作配置数据设置响应消息*/

#define EVENT_WCDMA_LOAD_CONFIG_REQ           (0x6003)    /*基站工作配置数据加载请求消息*/
#define EVENT_WCDMA_LOAD_CONFIG_RSP           (0x6003)    /*基站工作配置数据加载请求响应消息*/

#define EVENT_WCDMA_SET_POWER_REQ             (0x6004)    /*基站功率设置请求消息*/
#define EVENT_WCDMA_SET_POWER_RSP             (0x6004)    /*基站功率设置请求响应消息*/

#define EVENT_WCDMA_PARAMENTER_RPT            (0x6005)    /*基站无线环境报告请求消息*/
#define EVENT_WCDMA_PARAMENTER_RSP            (0x6005)    /*基站无线环境报告请求响应消息*/

#define EVENT_WCDMA_REGISTER_NOTIFY_REQ       (0x6006)    /*注册通知消息*/
#define EVENT_WCDMA_REGISTER_NOTIFY_RSP       (0x6006)    /*注册通知响应消息*/

#define EVENT_WCDMA_START_SNIFFER_REQ         (0x6007)    /*启动sniffer消息*/
#define EVENT_WCDMA_START_SNIFFER_RSP         (0x6007)    /*启动sniffer消息*/

#define EVENT_WCDMA_PARA_UPDATE_REQ           (0x6008)    /*更新"界面"配置请求消息*/
#define EVENT_WCDMA_PARA_UPDATE_RSP           (0x6008)    /*更新"界面"配置响应消息*/

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
#define WCDMA_SYS_STAT_RSV           (4)  //4-保留,5-保留，6-保留，7－保留
#define WCDMA_SYS_STAT_CELL_GET      (15)
/////////////////////////////////////////////////////////////////////////////////////////
typedef struct tag_WCDMA_heartbeat_req
{
    wg_msg_head_t msgHeader;                               /* 消息头*/
} wcdma_heartbeat_req_t;

typedef struct tag_WCDMA_heartbeat_rsp
{
	wg_msg_head_t msgHeader;                               /* 消息头*/
	uint16_t u16SysMode;                   /* 模块工作状态，1-起电，2-准备（数据加载完成），3-正常运行（ 基站正常运行），4-保留,5-保留，6-保留，7－保留）*，8-应急工作状态，9-LTE扫频状态，10-LTE同步工作，11-LTE非同步工作，12-LTE同步失败*/
} wcdma_heartbeat_rsp_t;
/*
状态"10-LTE同步工作，11-LTE非同步工作"，指基站根据sniffer结果，正常工作后的状态。由于基站可能sniffer无结果，只能工作在非同步状态。
状态"12-LTE同步失败"，指基站被要求同步时（如运行在基站配置模式且被要求同步公网），但又同步不上时，应该向"界面"告警。
*/

typedef struct tag_WCDMA_start_req
{
	wg_msg_head_t msgHeader;      /* 消息头*/
	uint16_t u16Start; /* 0：停止，1：开始*/
} wcdma_start_req_t;

typedef struct tag_WCDMA_start_rsp
{
	wg_msg_head_t msgHeader;                 /* 消息头*/
	uint8_t	u8StartInd;					//成功指示，1－成功，0－失败
	uint8_t	u8FailReason;					//失败原因，待定
} wcdma_start_rsp_t;

typedef struct tag_WCDMA_reset_req
{
	oam_msg_head_t msgHeader;               /* 消息头*/
} oam_wcdma_reset_req_t;


typedef struct tag_WCDMA_reset_rsp
{
	wg_msg_head_t msgHeader;                               /* 消息头*/
	uint8_t	u8ResetInd;						//重启接受指示，1－接受并准备重启，0－拒绝
	uint8_t	u8FailReason;					//拒绝重启原因，待定
} wcdma_reset_rsp_t;

typedef struct tag_WCDMA_rf_set_req
{
	oam_msg_head_t msgHeader;                 /* 消息头*/
	uint8_t	flag;
} oam_wcdma_rf_set_req_t;

typedef struct tag_WCDMA_lbs_set_req
{
	oam_msg_head_t msgHeader;                 /* 消息头*/
	uint8_t	flag;
} oam_wcdma_lbs_set_req_t;

typedef struct tag_WCDMA_para_set_req
{
	oam_msg_head_t msgHeader;                 /* 消息头*/
	uint16_t mcc;
	uint8_t  mnc;
	uint16_t power;
	uint16_t lac;
	uint16_t cid;
	uint8_t  bsic;
	uint8_t  rac;
	uint16_t arfcn0;
} oam_wcdma_para_set_req_t;


/* 基站参数*/
typedef struct tag_WCDMA_pbs_para
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
}wcdma_pbs_para_t;

/*同步参数*/
typedef struct tag_WCDMA_syn_para
{
	uint8_t		u8Band; 	/*频段*/
	uint8_t 	u8Rsv;
	uint16_t	u16Earfcn;	/*频点号*/
	uint16_t	u16PCI;	/*物理小区标识*/
	uint16_t	u16Rsv;
}wcdma_syn_para_t;

/*驱赶目标参数*/
typedef struct tag_WCDMA_rel_para
{
	uint8_t		u8Band; 	/*频段*/
	uint8_t 	u8Rsv;
	uint16_t	u16Arfcn;	/*频点号*/
	uint8_t		u8ReleaseCause;		/*release原因*/
	uint8_t		u8RejectCause;		/*reject原因*/
	uint16_t	u16Rsv;
}wcdma_rel_para_t;

/*扫频参数*/
typedef struct tag_WCDMA_sniffer_para
{
	uint8_t		u8Band; 	/*频段*/
	uint8_t 	u8Rsv;
	uint16_t	u16Earfcn;	/*频点号*/
}wcdma_sniffer_para_t;

typedef struct tag_WCDMA_set_cfg_req
{
	wg_msg_head_t msgHeader;       /* 消息头*/
	uint16_t u16WorkMode;		/*工作模式，WorkMode=0（"界面"配置模式），则直接采用"界面"的配置参数；WorkMode=1（扫频配置模式），则根据扫频参数进行扫频，然后启动*/
	uint16_t u16SynMode;        /* 是否同步工作，如果为同步工作，则PBS需要根据同步参数，先同步再启动 */
	wcdma_pbs_para_t 	strPbsPara;
	wcdma_syn_para_t 	strSynPara;
	wcdma_rel_para_t 	strRelPara;
	uint32_t		u32SnifferParaNum;					/*扫频参数数组个数*/
	wcdma_sniffer_para_t	strSnifferPara[MAX_SNIFFER_NUM];
} wcdma_set_cfg_req_t;

typedef struct tag_WCDMA_set_cfg_rsp
{
	wg_msg_head_t msgHeader;                              /* 消息头*/
	uint8_t u8CfgInd;                                 /* 成功标识，1－成功，0－失败*/
} wcdma_set_cfg_rsp_t;

typedef struct tag_WCDMA_load_cfg_req
{
    wg_msg_head_t msgHeader;                              /* 消息头*/ 
} wcdma_load_cfg_req_t;

typedef struct tag_WCDMA_load_cfg_rsp
{
	wg_msg_head_t msgHeader;                              /* 消息头*/
	uint16_t u16WorkMode;		/*工作模式，WorkMode=0（"界面"配置模式），则直接采用"界面"的配置参数；WorkMode=1（扫频配置模式），则根据扫频参数进行扫频，然后启动*/	
	uint16_t u16SynMode;         /*是否同步工作，如果为同步工作，则PBS需要根据同步参数，先同步再启动*/
	wcdma_pbs_para_t 	strPbsPara;
	wcdma_syn_para_t 	strSynPara;
	wcdma_rel_para_t 	strRelPara;
	uint32_t		u32SnifferParaNum;					/*扫频参数数组个数*/
	wcdma_sniffer_para_t	strSnifferPara[MAX_SNIFFER_NUM];
} wcdma_load_cfg_rsp_t;

typedef struct tag_WCDMA_set_power_req
{
	oam_msg_head_t msgHeader;           /* 消息头*/ 
	uint16_t u16Power; 				/* 功率 */
} oam_wcdma_set_power_req_t;

typedef struct tag_WCDMA_set_power_rsp
{
	wg_msg_head_t msgHeader;           /* 消息头*/
	uint8_t	u8PowerInd;	        //成功失败指示	1－成功，0－失败
	uint8_t	u8FailReasonInd;	//失败原因。待定
} wcdma_set_power_rsp_t;

typedef struct tag_WCDMA_para
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
}wcdma_para_t;

typedef struct tag_WCDMA_para_rpt
{
    wg_msg_head_t msgHeader;                              /* 消息头*/ 
	uint32_t	u32PCINum;	         /*PCI个数，也就是紧跟其后无线参数数组的个数*/
	wcdma_para_t	strLtePara[8];
} wcdma_para_rpt_t;

typedef struct tag_WCDMA_para_rsp
{
	wg_msg_head_t msgHeader;                              /* 消息头*/
} wcdma_para_rsp_t;

typedef struct tag_WCDMA_mobile_notify_req
{
	wg_msg_head_t msgHeader;                 /* 消息头*/
	uint8_t     u8IMSI[8];                   /* Ucd code, 4bit one number IMSI */
	uint8_t     u8ESN[8];
} wcdma_mobile_notify_req_t;

typedef struct tag_WCDMA_mobile_notify_rsp
{
	wg_msg_head_t msgHeader;                  /* 消息头*/
} wcdma_mobile_notify_rsp_t;

typedef struct tag_WCDMA_start_sniffer_req
{
	wg_msg_head_t msgHeader;                 /* 消息头*/
} wcdma_start_sniffer_req_t;

typedef struct tag_WCDMA_start_sniffer_rsp
{
	wg_msg_head_t msgHeader;                  /* 消息头*/
} wcdma_start_sniffer_rsp_t;

typedef struct tag_WCDMA_para_update_req
{
	wg_msg_head_t		msgHeader;                              /* 消息头*/ 
	/*UpdateFlag=1，表示"界面"根据本消息更新参数*；
	  UpdateFlag=0，表示没有搜索到合适的公网环境，"界面"不用更新参数*/
	uint16_t		u16UpdateFlag; 
	/*是否同步工作，如果为同步工作，则PBS需要根据同步参数，先同步再启动*/
	uint16_t		u16SynMode;    
	wcdma_pbs_para_t 	strPbsPara;
	wcdma_syn_para_t 	strSynPara;
} wcdma_para_update_req_t;

typedef struct tag_WCDMA_para_update_rsp
{
	wg_msg_head_t	msgHeader;                           /* 消息头*/
	uint16_t	u16UpdateInd;						/*更新成功为1，失败为0*/
} wcdma_para_update_rsp_t;

/////////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
void wcdma_pbs_cfg_set(wcdma_pbs_para_t *ptPbsCfg, uint16_t mcc, uint8_t mnc, uint8_t band, uint16_t earfcn, uint16_t tac, uint16_t cellid, uint16_t pci, uint8_t pwr);
void wcdma_sync_cfg(wcdma_syn_para_t *ptSyncCfg, uint8_t band, uint16_t earfcn, uint16_t pci);
void wcdma_rel_cfg(wcdma_rel_para_t *ptRelCfg, uint8_t band, uint16_t arfcn, uint8_t rejCause, uint8_t relCause);
void wcdma_sniffer_cfg(wcdma_load_cfg_rsp_t *wcdma_cfg, uint8_t num, uint8_t *band, uint16_t *earfcn);


#endif 