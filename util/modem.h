#ifndef  __SMS_ACK_COMMON_H__
#define  __SMS_ACK_COMMON_H__

//#define   VIRTUAL_MODEM_DEBUG   //virtual modem for debug 

/***************************************************************/
typedef enum EnumModemStatus {
	MODEM_IDLE,
	MODEM_DISCONN,
	MODEM_OPENED,
	MODEM_REG_IN_NET,
	MODEM_ACK_ENABLED,
	MODEM_ACK_DISABLED,
	MODEM_REG_SENDING,
	MODEM_REG_SENT,
	MODEM_READY,
	MODEM_RESTART
}EModemStatus;

#define  READY_TO_IDLE  2
#define  IDLE_TO_READY  1

#define  MAX_SCA_LEN          (32)
/***************************************************************/
typedef enum EnumAtCmdIndex {
	AT_CMD_AT      = 0,
	AT_CMD_AT_CHUP = 1,
	AT_CMD_AT_CMGS = 2,
	AT_CMD_AT_CMGD = 3,
	AT_CMD_AT_CMGL = 4,	 
	AT_CMD_AT_CMGF = 5,	
	AT_CMD_AT_CSCA = 6,
	AT_CMD_AT_CREG = 7,
	AT_CMD_AT_CIMI = 8,
	AT_CMD_ATE = 9,
	AT_CMD_AT_CGMI = 10,
	AT_CMD_AT_CGMM =11,
	AT_CMD_AT_CGMR =12,
	AT_CMD_AT_CNUM =13,
	AT_CMD_AT_CMEE =14,
	AT_CMD_ATH =15,
	AT_CMD_AT_CPMS,
	AT_CMD_AT_CNMI
}EAtCmd;

#define  MAX_AT_CMD_NUM  32
typedef struct TagModemAtCmdStr {
	char  *atCmd;
	char  *atCmdOkResp;
	char  *atCmdFailResp;
}TModemAtCmdStr;

/***************************************************************/			
typedef enum EModemCtrl {
	MODEM_CTRL_IDLE,
	MODEM_CTRL_REG_IN_PROGRESS,
	MODEM_CTRL_REG_OK
}EModemCtrlStatus;


#define MODEM_CTRL_DUMP_STAT_TIMER        (60*3)

#define MODEM_CTRL_REG_RESEND_TIMER       (3)

#define MODEM_CTRL_KEEP_ALIVE_TIMER       (5)
#define MODEM_CTRL_KEEP_ALIVE_MAX_COUNT   (3)

#define MAX_SMS_FAILED_NUM     (5) 
/***************************************************************/
#define  MAX_MSG_NUM  5

typedef struct TagMsgItem {
	unsigned char  abyMsIsdn[MSISDN_LEN];
	unsigned char  byLen;
	unsigned short rsv;
	unsigned char  iSilent;
	unsigned char  strContent[MAX_SMS_LEN];
}TMsgItem;

typedef struct TagFifoQueue {
	unsigned int dwPutIndex;
	unsigned int dwGetIndex;
	TMsgItem   atItem[MAX_MSG_NUM];
}TMsgQueue;

int  ModemMsgPut(TMsgQueue *ptMsgQueue, char *abyMsIsdn, int len, char *strContent, int silent);
TMsgItem * ModemMsgGet();
/***************************************************************************************/

typedef struct TagModem {
	int    index;

	TPlmn  tPlmn;
	char   abyVer[64];
	u8  abyImsi[MAX_IMSI_LEN];
	
	u8  abyUartPort[MAX_UART_PORT_LEN];
	int nBaudRate; 
	int nParity;
	int nByteSize; 
	int nStopBits;

	u32  fd;

	int  iStatus;
	int  iStatusChange;
	TMsgQueue  tMsgQueue;

	struct  timeval tCheckTime;

	u8   bySendCount;
	u8   byFailedCount;

	u32  iSendSilentSms;
	u32  iRecvSilentAck;
	u32  iRecvSilentNack;
}TModem;


/**********************************************************************/
typedef struct TagModemCtrl {
	
	TModem   atModem[MAX_MODEM_NUM_PER_UNIT];
	
//	TPlmn    atPlmn[MAX_MODEM_NUM_PER_UNIT];
	
	u32      peerIpMaster;
	u32      peerIpSlave;
	u16      peerPort;

	HMUTEX   hMutex;

	u32      iStatus;
	u8       iProhibit;  // 0, manager prohibited; 1, no prohibited and used as normally 

	struct  timeval tDumpStatTime;
	struct  timeval tSendRegTime;
	struct  timeval tSendKeepAliveTime;

	u32      iKeepAliveFailed;

	/*  */
	u32      iRecvDetectReq;
	u32      iSendDetectResp;

	u32      iRecvRegResp;
	u32      iSendRegReq;

	u32      iRecvCapReportResp;
	u32      iSendCapReport;

	u32      iRecvKeepAliveResp;
	u32      iSendKeepAliveReq;

}TModemCtrl;

#define   MC_RECV_DETECT_REQ_INC(p)       (p->iRecvDetectReq++)
#define   MC_SEND_DETECT_RESP_INC(p)      (p->iSendDetectResp++)
#define   MC_RECV_REG_RESP_INC(p)         (p->iRecvRegResp++)
#define   MC_SEND_REG_REQ_INC(p)          (p->iSendRegReq++)
#define   MC_RECV_CAP_REPORT_RESP_INC(p)  (p->iRecvCapReportResp++)
#define   MC_SEND_CAP_REPORT_INC(p)       (p->iSendCapReport++)
#define   MC_RECV_KEEP_ALIVE_RESP_INC(p)  (p->iRecvKeepAliveResp++)
#define   MC_SEND_KEEP_ALIVE_REQ_INC(p)   (p->iSendKeepAliveReq++)

#define   MODEM_SEND_SILENT_SMS_INC(p)   (p->iSendSilentSms++)
#define   MODEM_RECV_SILENT_ACK_INC(p)   (p->iRecvSilentAck++)
#define   MODEM_RECV_SILENT_NACK_INC(p)   (p->iRecvSilentNack++)
/***************************************************************/
int  ModemCtrlInit();
int  ModemCtrlRun();

int  ModemInit(TModem *ptModem);
int  ModemRun(TModem *ptModem);
int  ModemStart(TModem *ptModem);

void ModemStatusSet(TModem *ptModem, 	int status);
int	ModemStatusGet(TModem *ptModem);
int ModemIsStatus(TModem *ptModem, 	int status);
int ModemIsStatusChange(TModem *ptModem, 	int status);
void ModemStatusChangeClear(TModem *ptModem);

int ModemSmsSend(TModem *ptModem, char *abyMsIsdn, int len, char *str, int silent);
int ModemSmsQueue(TModem *ptModem, char *abyMsIsdn, int len, char *str, int silent);
int ModemSilentSmsSend(TModem *ptModem, char *abyMsIsdn, int len);
int  ModemCtrlSendDetectResp(u8  *pbyMsisdn, u8  len, char *port, int ret);
int  ModemCtrlSendCreditsResp(u8  *pbyMsisdn, char *port);

void ModemCtrlDumpStat(TModemCtrl *ptCtrl);
int  ModemCtrlDumpStatBuf(TModemCtrl *ptCtrl, char *buf, int len);
void ModemDumpStat(TModem *ptModem);
void ModemDumpStatBuf(TModem *ptModem, char *buf, int len);

int  ModemIsRestart(char *buf);
int ModemClose(TModem *ptModem);

////////////////////////////////////////////////////////////////
int modem_open(TModem *ptModem);
int modem_at_cmd_send(TModem *ptModem, char *atCmd);
int modem_at_cmd_wait_rsp(TModem *ptModem, char *atCmd, char *pbyOut);
int modem_atCmd_w_r(TModem *ptModem, char *atCmd, char *atCmdResp, char *pbyOut);
int modem_echo(TModem *ptModem, int on);
int modem_imsi_get(TModem *ptModem);
int modem_version_get(TModem *ptModem);
int Modem_reset(TModem *ptModem);

#endif
