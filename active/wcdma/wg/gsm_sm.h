#ifndef __WG_GSM_SM_H__
#define __WG_GSM_SM_H__


#define  MAX_SMS_BUF_LEN  (256)

#define  ID_IMSI  0
#define  ID_IMEI  1
#define  ID_IMSI_AND_IMEI  2
#define  ID_IMSI_OR_IMEI  3
/////////////////////////////////////////////////////////////////////////
typedef struct Tag_GSM_state_machine {
	/* board info */
	uint32_t uart_ip_addr;
	uint32_t peer_ip_addr;
	uint16_t peer_port;
	uint8_t  channel;
	uint8_t  trx;
	uint8_t  trx_second;
	
	uint8_t year[16];
	uint8_t month[16];
	uint8_t day[16];
	uint8_t hour[16];
	uint8_t minute[16];
	uint8_t second[16];

	/* bts info */
	uint16_t mcc;
	uint8_t  mnc;
	uint16_t power;
	uint16_t lac;
	uint16_t cid;
	uint8_t  bsic;
	uint8_t  arfcn_num;
	uint16_t arfcn0;
	uint16_t arfcn1;
	uint8_t  nb_cell_num;
	uint16_t nb_arfcn[MAX_CELL_NUM];
	uint8_t  rach;
	uint32_t tmsi;

	/* service info */
	uint8_t  work_mode;
	uint16_t alarm_space;
	uint16_t whitelist_space;

	/* target mobile info */
	uint16_t mobile_power;
	uint8_t  id_flag; //0, imsi; 1, imei; 2, and; 3, or
	uint8_t  mobile_imsi[MAX_IMSI_LEN];	
	uint8_t  mobile_imei[MAX_IMSI_LEN];	
	uint8_t  caller_no[MAX_IMSI_LEN];	
	uint8_t  sms_content[MAX_SMS_BUF_LEN];
	uint32_t sms_len;  
	uint8_t  rssi;
	uint8_t  ta;

	//
	int     sock_of_uart;

	uint32_t heartbeat_lost;

	gsm_load_cfg_rsp_t gsm_cfg;

	TModem  tModem;

	uint32_t timer_id;
	uint32_t inner_amp_flag;
	uint32_t outer_amp_flag;

	uint32_t state;
	uint32_t remote_state;
}gsm_state_t;

typedef struct Tag_GSM_machine {
	uint32_t  state;
	void (*func)(gsm_state_t *gsm_state, uint8_t *buf, uint32_t len);
}gsm_machine_t;

/////////////////////////////////////////////////////////////////////////
void gsm_state_machine(gsm_state_t *gsm_state, uint8_t *buf, uint32_t len);
void gsm_state_set(gsm_state_t *gsm_state, uint32_t state);
void gsm_sm_start(gsm_state_t *gsm_state);

#endif 