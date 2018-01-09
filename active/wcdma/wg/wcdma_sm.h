#ifndef __WCDMA_SM_H__
#define __WCDMA_SM_H__

/////////////////////////////////////////////////////////////////////////
typedef struct Tag_WCDMA_state_machine {
	/* board info */
	uint32_t peer_ip_addr;
	uint16_t peer_port;
	uint8_t  channel;
	uint8_t  trx;

	/* bts info */
	uint16_t mcc;
	uint8_t  mnc;
	uint16_t power;
	uint16_t lac;
	uint16_t cid;
	uint16_t psc;
	uint16_t cpich_power;
	uint8_t  bsic;
	uint8_t  rac;
	uint16_t arfcn0;
	uint16_t arfcn1;

	/* service info */
	uint8_t work_mode;

	/* target mobile info */
	uint8_t  mobile_imsi[MAX_IMSI_LEN];	
	uint8_t  mobile_imei[MAX_IMSI_LEN];	
	uint8_t  caller_no[MAX_IMSI_LEN];	
	uint8_t  rssi;
	uint8_t  ta;

	uint32_t heartbeat_lost;

	TModem  tModem;
	wcdma_load_cfg_rsp_t wcdma_cfg;
	
	uint32_t state;
	uint32_t remote_state;
}wcdma_state_t;

/////////////////////////////////////////////////////////////////////////
void wcdma_state_machine(wcdma_state_t *wcdma_state, char *buf, int len);
void wcdma_state_set(wcdma_state_t *wcdma_state, uint32_t state);
void wcdma_sm_start(wcdma_state_t *wcdma_state);
void wcdma_oam_set_power(wcdma_state_t *wcdma_state, uint16_t power);
void wcdma_oam_reset(wcdma_state_t *wcdma_state);

#endif 