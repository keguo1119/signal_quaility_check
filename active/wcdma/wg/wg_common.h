#ifndef __WG_COMMON_H__
#define __WG_COMMON_H__

#define  MAX_CELL_NUM        (6)
#define  MAX_MSG_LEN         (4096)
#define  MAX_ENCODE_BUF_SIZE  (1024)
extern unsigned char abyRecvMsgBuf[MAX_MSG_LEN + 1];
///////////////////////////////////////////////////////////////////////////////
typedef enum e_wg_msg_type {
	EVENT_WG_GSM_HEARTBEAT = 1,

	EVENT_WG_GSM_PARA_SET_REQ = 2,
	EVENT_WG_GSM_PARA_SET_RSP = 3,

	EVENT_WG_GSM_ALARM_SET_REQ = 4,
	EVENT_WG_GSM_ALARM_SET_RSP = 5,

	EVENT_WG_GSM_WHITELIST_SET_REQ = 6,
	EVENT_WG_GSM_WHITELIST_RSP = 7,

	EVENT_WG_GSM_MOBILE_IMSI_REPORT = 8,

	EVENT_WG_GSM_SMS_SEND_REQ = 9,
	EVENT_WG_GSM_SMS_SEND_RSP = 10,

	EVENT_WG_GSM_POWER_SET_REQ = 11,
	EVENT_WG_GSM_POWER_SET_RSP = 12,

	EVENT_WG_GSM_SMS_REPORT = 13,
	EVENT_WG_GSM_CALL_REPORT = 14,

	EVENT_WG_GSM_RF_ON_REQ = 15,
	EVENT_WG_GSM_RF_ON_RSP = 16,
	EVENT_WG_GSM_RF_OFF_REQ = 17,
	EVENT_WG_GSM_RF_OFF_RSP = 18,

	EVENT_WG_GSM_LBS_START_REQ = 19,
	EVENT_WG_GSM_LBS_START_RSP = 20,
	EVENT_WG_GSM_LBS_STOP_REQ = 21,
	EVENT_WG_GSM_LBS_STOP_RSP = 22,

	EVENT_WG_GSM_RING_REQ = 23,
	EVENT_WG_GSM_RING_RSP = 24,

	EVENT_WG_GSM_MOBILE_SIGNAL_REPORT = 25,

	EVENT_WG_GSM_MOBILE_AUTH_REQ = 26,
	EVENT_WG_GSM_MOBILE_AUTH_RSP = 27,

	EVENT_WG_GSM_MOBILE_POWER_SET_REQ = 28,
	EVENT_WG_GSM_MOBILE_POWER_SET_RSP = 29,

	EVENT_WG_GSM_LBS_RING_REQ = 30,
	EVENT_WG_GSM_LBS_RING_RSP = 31,

	EVENT_WG_GSM_TIME_SET_REQ = 32,
	EVENT_WG_GSM_TIME_SET_RSP = 33,

	//////////WCDNA //////////////////////
	EVENT_WG_WCDMA_PARA_SET_REQ = 101,
	EVENT_WG_WCDMA_PARA_SET_RSP = 102,

	EVENT_WG_WCDMA_POWER_SET_REQ = 103,
	EVENT_WG_WCDMA_POWER_SET_RSP = 104,

	EVENT_WG_WCDMA_RF_ON_REQ = 105,
	EVENT_WG_WCDMA_RF_ON_RSP = 106,

	EVENT_WG_WCDMA_TARGET_TO_GSM_REQ = 107,
	EVENT_WG_WCDMA_TARGET_TO_GSM_RSP = 108,
	EVENT_WG_WCDMA_TO_GSM_REQ = 109,
	EVENT_WG_WCDMA_TO_GSM_RSP = 110,

	EVENT_WG_WCDMA_LBS_START_REQ = 111,
	EVENT_WG_WCDMA_LBS_START_RSP = 112,
	EVENT_WG_WCDMA_LBS_STOP_REQ = 113,
	EVENT_WG_WCDMA_LBS_STOP_RSP = 114,

	EVENT_WG_WCDMA_MOBILE_TARGET_REPORT = 115,
	EVENT_WG_WCDMA_MOBILE_IMSI_REPORT = 116,

	EVENT_WG_WCDMA_HEARTBEAT = 119,

	EVENT_WG_DEV_CAP_REQ = 201,
	EVENT_WG_DEV_CAP_RSP = 202,
	EVENT_WG_DEV_RESET_REQ = 203,
	EVENT_WG_DEV_RESET_RSP = 204,
	EVENT_WG_DEV_IP_SET_REQ = 205,
	EVENT_WG_DEV_IP_SET_RSP = 206,
	EVENT_WG_DEV_AMP_SET_REQ = 207,
	EVENT_WG_DEV_INNER_AMP_SET_RSP = 208,
	EVENT_WG_DEV_OUTER_AMP_SET_RSP = 209,
	
	EVENT_WG_OSS_START = 0x80, //
	WG_MSG_TYPE_INVALID = 255
}wg_msg_type_e;

#pragma pack (1)
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////

typedef struct tag_wg_msg_dev_reset_rsp
{
   uint8_t  msgType;
   uint16_t msgLen;
   uint8_t  ret;
}wg_msg_dev_reset_rsp_t;
///////////////////////////////////////////////////////////////////////////////
typedef struct tag_wg_msg_dev_ip_set_req
{
   uint8_t  msgType;
   uint16_t msgLen;
}wg_msg_dev_ip_set_req_t;

typedef struct tag_wg_msg_dev_ip_set_rsp
{
   uint8_t  msgType;
   uint16_t msgLen;
   uint8_t  ret;
}wg_msg_dev_ip_set_rsp_t;
///////////////////////////////////////////////////////////////////////////////
typedef struct tag_wg_msg_head
{
	uint8_t  channel;
	uint8_t  trx;
    uint8_t  msgType;
    uint16_t msgLen;
}wg_msg_head_t;

typedef wg_msg_head_t wg_msg_dev_cap_req_t;
typedef wg_msg_head_t wg_msg_dev_reset_req_t;

typedef wg_msg_head_t oam_msg_head_t;

#pragma pack ()


void wg_msg_head_init(wg_msg_head_t *ptHead, uint8_t channel, uint8_t trx, uint8_t msgType);
void wg_msg_len_set(wg_msg_head_t *ptHead, uint16_t len);
int wg_msg_send(uint32_t peer_ip_addr, uint16_t peer_port, uint8_t channel, uint8_t trx, uint8_t msgType, uint8_t *content, uint32_t content_size);

void  wg_msg_one_param_encode(uint8_t *out, uint8_t *name, uint32_t value, uint8_t last);
void  wg_msg_one_param_decode(uint8_t *in, uint8_t *name, uint8_t *out);
///////////////////////////////////////////////////////////////////////////////
uint16_t  wg_msg_type_get(uint8_t *msg);

#define  IS_OSS_MSG(x)    (x >= EVENT_WG_OSS_START)
#define  IS_DEV_MSG(x)    ((x >= EVENT_WG_DEV_CAP_REQ) && (x < EVENT_WG_OSS_START))
#define  IS_WCDMA_MSG(x)  ((x >= EVENT_WG_WCDMA_PARA_SET_REQ) && (x < EVENT_WG_DEV_CAP_REQ))
#define  IS_GSM_MSG(x)    ((x > 0) && (x < EVENT_WG_WCDMA_PARA_SET_REQ))
///////////////////////////////////////////////////////////////////////////////
typedef enum e_board_status {
	BOARD_STATUS_IDLE,
	BOARD_STATUS_IMSI_CACTHER,
	BOARD_STATUS_LOCATION,
	BOARD_STATUS_AUTH,
	BOARD_STATUS_SW_ERROR,
	BOARD_STATUS_HW_ERROR
}board_status_e;

typedef struct tag_wg_msg_heartbeat
{
	wg_msg_head_t   msg_head;
	board_status_e  status;
}wg_msg_heartbeat_t;

//////////////////////////////////////////////
typedef struct tag_wg_msg_param_set_rsp
{
	wg_msg_head_t   msg_head;
	uint8_t         ret;
}wg_msg_common_rsp_t;
//////////////////////////////////////////////
typedef wg_msg_common_rsp_t  wg_msg_param_set_rsp_t;

//////////////////////////////////////////////
typedef struct tag_wg_msg_alarm_rsp
{
	wg_msg_head_t   msg_head;
	uint8_t         ret;
	uint8_t         left_space;
}wg_msg_alarm_rsp_t;

#define  WG_OK    (0)
#define  WG_FAIL  (1)

wg_msg_common_rsp_t *wg_msg_common_rsp_decode(uint8_t *buf);
//////////////////////////////////////////////
typedef wg_msg_alarm_rsp_t wg_msg_whitelist_rsp_t;

typedef wg_msg_common_rsp_t  wg_msg_sms_send_rsp_t;

typedef wg_msg_common_rsp_t  wg_msg_power_set_rsp_t;

typedef wg_msg_common_rsp_t  wg_msg_rf_on_rsp_t;

typedef wg_msg_common_rsp_t  wg_msg_rf_off_rsp_t;

typedef wg_msg_common_rsp_t  wg_msg_lbs_start_rsp_t;

typedef wg_msg_common_rsp_t  wg_msg_lbs_stop_rsp_t;

typedef wg_msg_common_rsp_t  wg_msg_ring_rsp_t;

typedef wg_msg_common_rsp_t  wg_msg_mobile_power_set_rsp_t;

typedef wg_msg_common_rsp_t  wg_msg_lbs_ring_rsp_t;

typedef wg_msg_common_rsp_t  wg_msg_time_set_rsp_t;
///////////////////////////////////////////////////////////////////////////////
#define  GSM_HOST_UDP_PORT       (18990)
#define  GSM_BOARD_UDP_PORT      (10002) 
#define  WCDMA_HOST_UDP_PORT     (18990)
#define  WCDMA_BOARD_UDP_PORT    (10002) 
#define  UART_UDP_PORT    (8080) 

///////////////////////////////////////////////////////////////////////////////
#define EVENT_WCDMA_TIMEOUT_START             (EVENT_WG_OSS_START)
#define EVENT_WCDMA_HEARTBEAT_TIMEOUT         EVENT_WCDMA_TIMEOUT_START
#define EVENT_WCDMA_CELL_GET_TIMEOUT          (EVENT_WCDMA_TIMEOUT_START + 2)
#define EVENT_WCDMA_STARTUP_TIMEOUT           (EVENT_WCDMA_TIMEOUT_START + 3)
#define EVENT_WCDMA_POWER_SET_TIMEOUT         (EVENT_WCDMA_TIMEOUT_START + 4)
#define EVENT_WCDMA_TIMEOUT_END               (EVENT_WCDMA_TIMEOUT_START + 0xf)

// from UI to wcdma process
#define EVENT_OAM_WCDMA_START                 (EVENT_WG_OSS_START + 0x10)
#define EVENT_OAM_WCDMA_START_REQ             (EVENT_OAM_WCDMA_START)
#define EVENT_OAM_WCDMA_RESET_REQ             (EVENT_OAM_WCDMA_START + 1)
#define EVENT_OAM_WCDMA_SET_POWER_REQ         (EVENT_OAM_WCDMA_START + 2)
#define EVENT_OAM_WCDMA_RF_SET_REQ            (EVENT_OAM_WCDMA_START + 6)
#define EVENT_OAM_WCDMA_LBS_SET_REQ           (EVENT_OAM_WCDMA_START + 7)
#define EVENT_OAM_WCDMA_PARA_SET_REQ          (EVENT_OAM_WCDMA_START + 9)
#define EVENT_OAM_WCDMA_STOP                  (EVENT_OAM_WCDMA_START + 0x1f)

// from os to wg_gsm process
#define EVENT_WG_GSM_TIMEOUT_START          (EVENT_WG_OSS_START + 0x20)
#define EVENT_WG_GSM_HEARTBEAT_TIMEOUT      EVENT_WG_GSM_TIMEOUT_START
#define EVENT_WG_DEV_CAP_GET_TIMEOUT        (EVENT_WG_GSM_TIMEOUT_START + 2)
#define EVENT_WG_DEV_IP_SET_TIMEOUT         (EVENT_WG_GSM_TIMEOUT_START + 3)
#define EVENT_WG_DEV_RESET_TIMEOUT          (EVENT_WG_GSM_TIMEOUT_START + 4)
#define EVENT_WG_GSM_LBS_START_TIMEOUT      (EVENT_WG_GSM_TIMEOUT_START + 5)
#define EVENT_WG_GSM_POWER_SET_TIMEOUT      (EVENT_WG_GSM_TIMEOUT_START + 6)
#define EVENT_WG_GSM_TIMEOUT_END            (EVENT_WG_GSM_TIMEOUT_START + 0x2f)

// from UI to wg process
#define EVENT_OAM_GSM_START                 (EVENT_WG_OSS_START + 0x30)
#define EVENT_OAM_GSM_START_REQ             (EVENT_OAM_GSM_START)
#define EVENT_OAM_GSM_RESET_REQ             (EVENT_OAM_GSM_START + 1)
#define EVENT_OAM_GSM_SET_POWER_REQ         (EVENT_OAM_GSM_START + 2)
#define EVENT_OAM_GSM_ALARM_SET_REQ        (EVENT_OAM_GSM_START + 3)
#define EVENT_OAM_GSM_WHITELIST_SET_REQ    (EVENT_OAM_GSM_START + 4)
#define EVENT_OAM_GSM_SMS_SEND_REQ         (EVENT_OAM_GSM_START + 5)
#define EVENT_OAM_GSM_RF_SET_REQ            (EVENT_OAM_GSM_START + 6)
#define EVENT_OAM_GSM_LBS_SET_REQ            (EVENT_OAM_GSM_START + 7)
#define EVENT_OAM_GSM_MOBILE_POWER_SET_REQ  (EVENT_OAM_GSM_START + 8)
#define EVENT_OAM_GSM_PARA_SET_REQ            (EVENT_OAM_GSM_START + 9)
#define EVENT_OAM_GSM_TIME_SET_REQ            (EVENT_OAM_GSM_START + 10)
#define EVENT_OAM_INNER_AMP_SET_REQ         (EVENT_OAM_GSM_START + 11)
#define EVENT_OAM_GSM_CFG_CHANGE            (EVENT_OAM_GSM_START + 15)
#define EVENT_OAM_GSM_STOP                  (EVENT_OAM_GSM_START + 0x3f)

///////////////////////////////////////////////////////////////////////////////
void wg_dev_cap_req(uint32_t peer_ip_addr, uint16_t peer_port);
int wg_dev_reset(uint32_t peer_ip_addr, uint16_t peer_port);
void wg_dev_ip_set(uint32_t ip, uint16_t port, uint32_t new_ip);

void wg_dev_cap_rsp_proc(uint8_t *buf, uint32_t len);
void wg_dev_reset_rsp_proc(uint8_t *buf, uint32_t len);
void wg_dev_ip_set_rsp_proc(uint8_t *buf, uint32_t len);
///////////////////////////////////////////////////////////////////////////////

#endif