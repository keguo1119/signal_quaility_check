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
//功率设置消息的参数值的含义有所变化，现在是按5档设置，每档大概差3dB，
//填5的时候最高，裸板输出-10dBm左右
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
#define EVENT_LTE_HEARBEAT_REQ            (0x0001)      /*心跳消息*/
#define EVENT_LTE_HEARBEAT_RSP            (0x0001)      /*心跳消息响应*/

#define EVENT_LTE_START_REQ               (0x0002)      /* 开始/停止工作命令*/
#define EVENT_LTE_START_RSP               (0x0002)      /* 开始/停止工作命令响应*/

#define EVENT_LTE_ALERTSMS_REQ            (0x0007)      /* 短信警示命令 */
#define EVENT_LTE_ALERTSMS_RSP            (0x0007)      /* 短信警示命令响应 */

#define	EVENT_LTE_RESET_REQ					(0x000B)    // 基站重启命令
#define	EVENT_LTE_RESET_RSP					(0x000B)    // 基站重启响应

#define EVENT_LTE_GET_CONFIG_RSP            (0x6001)    /* 基站工作配置数据查询响应消息 */

#define EVENT_LTE_SET_CONFIG_REQ            (0x6002)    /*基站工作配置数据设置消息*/
#define EVENT_LTE_SET_CONFIG_RSP            (0x6002)    /*基站工作配置数据设置响应消息*/

#define EVENT_LTE_LOAD_CONFIG_REQ           (0x6003)    /*基站工作配置数据加载请求消息*/
#define EVENT_LTE_LOAD_CONFIG_RSP           (0x6003)    /*基站工作配置数据加载请求响应消息*/

#define EVENT_LTE_SET_POWER_REQ             (0x6004)    /*基站功率设置请求消息*/
#define EVENT_LTE_SET_POWER_RSP             (0x6004)    /*基站功率设置请求响应消息*/

#define EVENT_LTE_PARAMENTER_RPT            (0x6005)    /*基站无线环境报告请求消息*/
#define EVENT_LTE_PARAMENTER_RSP            (0x6005)    /*基站无线环境报告请求响应消息*/

#define EVENT_LTE_REGISTER_NOTIFY_REQ       (0x6006)    /*注册通知消息*/
#define EVENT_LTE_REGISTER_NOTIFY_RSP       (0x6006)    /*注册通知响应消息*/

#define EVENT_LTE_START_SNIFFER_REQ         (0x6007)    /*启动sniffer消息*/
#define EVENT_LTE_START_SNIFFER_RSP         (0x6007)    /*启动sniffer消息*/

#define EVENT_LTE_PARA_UPDATE_REQ           (0x6008)    /*更新"界面"配置请求消息*/
#define EVENT_LTE_PARA_UPDATE_RSP           (0x6008)    /*更新"界面"配置响应消息*/

#define EVENT_LTE_TDDFDD_SWITCH_REQ         (0x6010)    /* TDD/FDD切换命令消息 */
#define EVENT_LTE_TDDFDD_SWITCH_RSP         (0x6010)    /* TDD/FDD切换命令响应消息 */

#define EVENT_LTE_WORKMODE_REQ              (0x6011)    /* 板卡工作模式查询请求消息 */
#define EVENT_LTE_WORKMODE_RSP              (0x6011)    /* 板卡工作模式查询响应消息 */
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
#define LTE_SYS_STAT_RSV           (4)  //4-保留,5-保留，6-保留，7－保留
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
    msg_head_t msgHeader;                               /* 消息头*/
} lte_heartbeat_req_t;

typedef struct tag_lte_heartbeat_rsp
{
	msg_head_t msgHeader;                               /* 消息头*/
	uint16_t u16SysMode;                   /* 模块工作状态，1-起电，2-准备（数据加载完成），3-正常运行（ 基站正常运行），4-保留,5-保留，6-保留，7－保留）*，8-应急工作状态，9-LTE扫频状态，10-LTE同步工作，11-LTE非同步工作，12-LTE同步失败*/
} lte_heartbeat_rsp_t;
/*
状态"10-LTE同步工作，11-LTE非同步工作"，指基站根据sniffer结果，正常工作后的状态。由于基站可能sniffer无结果，只能工作在非同步状态。
状态"12-LTE同步失败"，指基站被要求同步时（如运行在基站配置模式且被要求同步公网），但又同步不上时，应该向"界面"告警。
*/

typedef struct tag_lte_start_req
{
	msg_head_t msgHeader;      /* 消息头*/
	uint16_t u16Start; /* 0：停止，1：开始*/
} lte_start_req_t;

typedef struct tag_lte_start_rsp
{
	msg_head_t msgHeader;                 /* 消息头*/
	uint8_t	u8StartInd;					//成功指示，1－成功，0－失败
	uint8_t	u8FailReason;					//失败原因，待定
} lte_start_rsp_t;

typedef struct tag_lte_reset_req
{
	msg_head_t msgHeader;               /* 消息头*/
} lte_reset_req_t;


typedef struct tag_lte_reset_rsp
{
	msg_head_t msgHeader;                               /* 消息头*/
	uint8_t	u8ResetInd;						//重启接受指示，1－接受并准备重启，0－拒绝
	uint8_t	u8FailReason;					//拒绝重启原因，待定
} lte_reset_rsp_t;

/* 基站参数*/
typedef struct tag_lte_pbs_para
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
}lte_pbs_para_t;

/*同步参数*/
typedef struct tag_lte_syn_para
{
	uint8_t		u8Band; 	/*频段*/
	uint8_t 	u8Rsv;
	uint16_t	u16Earfcn;	/*频点号*/
	uint16_t	u16PCI;	/*物理小区标识*/
	uint16_t	u16Rsv;
}lte_syn_para_t;

/*驱赶目标参数*/
typedef struct tag_lte_rel_para
{
	uint8_t		u8Band; 	/*频段*/
	uint8_t 	u8Rsv;
	uint16_t	u16Arfcn;	/*频点号*/
	uint8_t		u8ReleaseCause;		/*release原因*/
	uint8_t		u8RejectCause;		/*reject原因*/
	uint16_t	u16Rsv;
}lte_rel_para_t;

/*扫频参数*/
typedef struct tag_lte_sniffer_para
{
	uint8_t		u8Band; 	/*频段*/
	uint8_t 	u8Rsv;
	uint16_t	u16Earfcn;	/*频点号*/
}lte_sniffer_para_t;

typedef struct tag_lte_set_cfg_req
{
	msg_head_t msgHeader;       /* 消息头*/
	uint16_t u16WorkMode;		/*工作模式，WorkMode=0（"界面"配置模式），则直接采用"界面"的配置参数；WorkMode=1（扫频配置模式），则根据扫频参数进行扫频，然后启动*/
	uint16_t u16SynMode;        /* 是否同步工作，如果为同步工作，则PBS需要根据同步参数，先同步再启动 */
	lte_pbs_para_t 	strPbsPara;
	lte_syn_para_t 	strSynPara;
	lte_rel_para_t 	strRelPara;
	uint32_t		u32SnifferParaNum;					/*扫频参数数组个数*/
	lte_sniffer_para_t	strSnifferPara[MAX_SNIFFER_NUM];
} lte_set_cfg_req_t;

typedef struct tag_lte_set_cfg_rsp
{
	msg_head_t msgHeader;                              /* 消息头*/
	uint8_t u8CfgInd;                                 /* 成功标识，1－成功，0－失败*/
} lte_set_cfg_rsp_t;

typedef struct tag_lte_load_cfg_req
{
    msg_head_t msgHeader;                              /* 消息头*/ 
} lte_load_cfg_req_t;

typedef struct tag_lte_load_cfg_rsp
{
	msg_head_t msgHeader;                              /* 消息头*/
	uint16_t u16WorkMode;		/*工作模式，WorkMode=0（"界面"配置模式），则直接采用"界面"的配置参数；WorkMode=1（扫频配置模式），则根据扫频参数进行扫频，然后启动*/	
	uint16_t u16SynMode;         /*是否同步工作，如果为同步工作，则PBS需要根据同步参数，先同步再启动*/
	lte_pbs_para_t 	strPbsPara;
	lte_syn_para_t 	strSynPara;
	lte_rel_para_t 	strRelPara;
	uint32_t		u32SnifferParaNum;					/*扫频参数数组个数*/
	lte_sniffer_para_t	strSnifferPara[MAX_SNIFFER_NUM];
} lte_load_cfg_rsp_t;

typedef struct tag_lte_set_power_req
{
	msg_head_t msgHeader;           /* 消息头*/ 
	uint16_t u16Power; 				/* 功率 */
} lte_set_power_req_t;

typedef struct tag_lte_set_power_rsp
{
	msg_head_t msgHeader;           /* 消息头*/
	uint8_t	u8PowerInd;	        //成功失败指示	1－成功，0－失败
	uint8_t	u8FailReasonInd;	//失败原因。待定
} lte_set_power_rsp_t;

typedef struct tag_lte_para
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
}lte_para_t;

typedef struct tag_lte_para_rpt
{
    msg_head_t msgHeader;                              /* 消息头*/ 
	uint32_t	u32PCINum;	         /*PCI个数，也就是紧跟其后无线参数数组的个数*/
	lte_para_t	strLtePara[8];
} lte_para_rpt_t;

typedef struct tag_lte_para_rsp
{
	msg_head_t msgHeader;                              /* 消息头*/
} lte_para_rsp_t;

typedef struct tag_lte_mobile_notify_req
{
	msg_head_t msgHeader;                 /* 消息头*/
	uint16_t	type;                   //xzluo according to udp packet
	uint8_t     u8IMSI[8];                   /* Ucd code, 4bit one number IMSI */
	uint8_t     u8ESN[8];
} lte_mobile_notify_req_t;

typedef struct tag_lte_mobile_notify_rsp
{
	msg_head_t msgHeader;                  /* 消息头*/
	uint8_t     u8IMSI[8];                 /* Ucd code, 4bit one number IMSI */
	uint8_t 	acpt; /* 4为定位 */
	uint8_t 	u8Rsv2;
} lte_mobile_notify_rsp_t;

typedef struct tag_lte_start_sniffer_req
{
	msg_head_t msgHeader;                 /* 消息头*/
} lte_start_sniffer_req_t;

typedef struct tag_lte_start_sniffer_rsp
{
	msg_head_t msgHeader;                  /* 消息头*/
} lte_start_sniffer_rsp_t;

typedef struct tag_lte_para_update_req
{
	msg_head_t		msgHeader;                              /* 消息头*/ 
	/*UpdateFlag=1，表示"界面"根据本消息更新参数*；
	  UpdateFlag=0，表示没有搜索到合适的公网环境，"界面"不用更新参数*/
	uint16_t		u16UpdateFlag; 
	/*是否同步工作，如果为同步工作，则PBS需要根据同步参数，先同步再启动*/
	uint16_t		u16SynMode;    
	lte_pbs_para_t 	strPbsPara;
	lte_syn_para_t 	strSynPara;
} lte_para_update_req_t;

typedef struct tag_lte_para_update_rsp
{
	msg_head_t	msgHeader;                           /* 消息头*/
	uint16_t	u16UpdateInd;						/*更新成功为1，失败为0*/
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
