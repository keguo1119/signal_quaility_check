#ifndef __LTE_SM_H__
#define __LTE_SM_H__

/////////////////////////////////////////////////////////////////////////
typedef struct Tag_lte_state_machine {
	uint8_t dst_bsId;
	uint8_t dst_modId;
	
	uint8_t src_bsId;
	uint8_t src_modId;

	uint16_t session_id;
	uint16_t startReq_send; 

	uint32_t peer_ip_addr;
	uint16_t peer_port;

	uint16_t work_mode;
	uint16_t syn_mode;
	uint16_t mcc;
	uint32_t heartbeat_lost;
    uint32_t cellselect_fail;
	
	lte_load_cfg_rsp_t lte_cfg;

	TModem  tModem;

    uint32_t startreq_fail;
    uint32_t p_oam_addr;
	uint32_t state;
	uint32_t remote_state;
	uint8_t  tddfdd_mode; /* 0: FDD, 1: TDD */ 
	uint8_t  drive_to_gsm;
	uint16_t start_value; /* 0：停止，1：开始*/
	uint16_t target_set;  // 1:目标定位，0:定位取消
}lte_state_t;

/////////////////////////////////////////////////////////////////////////
void lte_state_machine(lte_state_t *lte_state, char *buf, int len);
void lte_state_set(lte_state_t *lte_state, uint32_t state);
void lte_sm_start(lte_state_t *lte_state);
void lte_msg_proc(char *buf, int len);
void lte_oam_set_power(lte_state_t *lte_state, uint16_t power);
void lte_oam_reset(lte_state_t *lte_state);
void lte_oam_start(lte_state_t *lte_state, uint16_t start);
void lte_oam_sniffer(lte_state_t *lte_state);

#endif 