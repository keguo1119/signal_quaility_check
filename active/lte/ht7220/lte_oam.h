#ifndef __LTE_OAM_H__
#define __LTE_OAM_H__

/////////////////////////////////////////////////////////////////////////
// ERROR CODE 
#define SYS_ERR_CODE_BASE                     (0)
#define GET_CELL_PARA_ERROR                   (SYS_ERR_CODE_BASE + 1) 
#define SEND_START_REQ_ERROR                  (SYS_ERR_CODE_BASE + 2)

/////////////////////////////////////////////////////////////////////////
#define ENV_BANDS_SCAN                        (1)
#define SERV_CELL_SCAN                        (2)

/////////////////////////////////////////////////////////////////////////
#define TIME_STEP_NUM                         (4)
/////////////////////////////////////////////////////////////////////////
#pragma pack (1)

typedef struct tag_oam_msg_head
{
    uint8_t    channel;
	uint8_t    sub_ch;
	uint8_t    op_code;
	uint8_t    ex_code;
    uint16_t   msgLen;
} oam_msg_head_t;

typedef struct tag_oam_heartbeat_req
{
    oam_msg_head_t msgHeader; 
	uint8_t    local_state;
    uint8_t    bts_state;
} oam_heartbeat_req_t; 

typedef struct tag_oam_heartbeat_rsp
{
    oam_msg_head_t msgHeader; 
	uint8_t    ex_code;
    uint8_t    u8Rsv1;
} oam_heartbeat_rsp_t;

typedef struct tag_oam_eNB_para_req
{
    oam_msg_head_t msgHeader; 
	uint16_t	u16MCC;	
	uint8_t		u8Mnc;
	uint8_t		band;
} oam_eNB_para_req_t; 

typedef struct tag_oam_cmd_rsp
{
    oam_msg_head_t msgHeader; 
	uint8_t    flag;  /* 0: accept, 1: refuse */
} oam_cmd_rsp_t;

typedef struct tag_oam_start_req
{
    oam_msg_head_t msgHeader; 
	uint8_t		flag;  /* 0: stop, 1: start */
} oam_start_req_t;

typedef struct tag_oam_sniffer_req
{
    oam_msg_head_t msgHeader; 
} oam_sniffer_req_t;

typedef struct tag_oam_reset_req
{
    oam_msg_head_t msgHeader; 
} oam_reset_req_t;

typedef struct tag_oam_power_set_req
{
    oam_msg_head_t msgHeader; 
	uint8_t		powerValue;
} oam_power_set_req_t; 

typedef struct tag_oam_target_set_req
{
    oam_msg_head_t msgHeader; 
	uint8_t        u8IMSI[8];
} oam_target_set_req_t;

typedef struct tag_oam_target_cancel_req
{
    oam_msg_head_t msgHeader; 
} oam_target_cancel_req_t;

typedef struct tag_oam_imsi_ind
{
    oam_msg_head_t msgHeader; 
	uint8_t        u8IMSI[8];
	uint8_t        u8ESN[8];
} oam_imsi_ind_t; 

typedef struct tag_oam_sniff_info_ind
{
    oam_msg_head_t msgHeader; 
	uint8_t		u8Rsv;
	uint8_t 	u8Band;
	uint16_t	u16Earfcn; 
	uint16_t	u16PCI; 
	uint8_t		u8PdschRefPower; 
	uint8_t		u8Rsrp; 
	short	    s16FreqOffset; 
	uint16_t	u16Tac; 
	uint32_t	u32CellId;
} oam_sniff_info_ind_t;

typedef struct tag_oam_para_update_ind
{
    oam_msg_head_t msgHeader; 
    /* pbs parameter */
	uint8_t		u8pwr;
	uint8_t 	u8Band;
	uint16_t	u16Earfcn; 
    uint16_t	u16MCC;	
	uint8_t		u8Mnc;
    uint8_t		u8InitialValueTag;
	uint16_t	u16PCI; 
	uint16_t	u16Tac; 
	uint16_t	u16CellId;
    /* sync parameter */
    uint8_t 	u8Rsv;
    uint8_t		u8SynBand;
    uint16_t	u16SynEarfcn;
    uint16_t	u16SynPCI;
} oam_para_update_ind_t;

typedef struct tag_oam_band_set_req
{
    oam_msg_head_t msgHeader; 
//	uint8_t    cn_oper;
	uint8_t    band; 
} oam_band_set_req_t; //similar to oam_eNB_para_req_t

typedef struct tag_oam_oper_set_req
{
    oam_msg_head_t msgHeader; 
	uint8_t    cn_oper;
} oam_oper_set_req_t;

typedef struct tag_oam_mode_set_req
{
    oam_msg_head_t msgHeader; 
	uint8_t    out_of_sync;
} oam_mode_set_req_t;

typedef struct tag_oam_env_scan_req
{
    oam_msg_head_t msgHeader; 
	uint16_t    time_duration;
} oam_env_scan_req_t;

typedef struct tag_oam_cells_para_req
{
    oam_msg_head_t msgHeader; 
	uint16_t    interval;
} oam_cells_para_req_t;

/////////////////////////////////////////////////////////////////////////
typedef struct tag_env_cell_para
{
    uint16_t	u16MCC;	
	uint8_t		u8Mnc;
	uint16_t	u16Tac; 
	uint32_t	u32CellId;
	uint16_t	u16PCI;
	uint8_t 	u8Band;
	uint16_t	u16Earfcn; 
    uint8_t 	dlbw;
	uint8_t		ulbw;
    short    cell_rsrq;
    short    cell_rsrp;
    short    cell_rssi;
} env_cell_para_t;

typedef struct tag_oam_cell_info_ind
{
    oam_msg_head_t msgHeader; 
    env_cell_para_t ecell_para;
} oam_cell_info_ind_t;

typedef struct tag_oam_CN_oper_switch 
{    
	uint8_t  o_oper;  //old CN operator
	uint8_t  n_oper;  //new CN operator
    void (*func)(void); //sim switch func
} oam_CN_oper_switch_t;

typedef struct tag_oam_env_ver_ind
{
    oam_msg_head_t msgHeader; 
	char   abyVer[32];
} oam_env_ver_ind_t;

typedef struct tag_oam_lte_mode_ind
{
    oam_msg_head_t msgHeader; 
	uint8_t	    u8WorkMode;	   /* 0: FDD, 1: TDD */
} oam_lte_mode_ind_t;

typedef struct tag_oam_err_ind
{
    oam_msg_head_t msgHeader; 
	uint8_t    errNo; 
} oam_err_ind_t;

typedef struct tag_oam_bands_node
{
	uint8_t    workMode; /* 0: FDD, 1: TDD */
	uint8_t    band; 
	uint8_t    priority;
	uint8_t    u8Rsv;
	struct tag_oam_bands_node * band_next;
} oam_bands_node_t;

typedef struct tag_oam_opers_list
{
    uint8_t    cn_oper; 
	uint8_t    u8Rsv;
	oam_bands_node_t * ptBand;
	struct tag_oam_opers_list * oper_next; 
} oam_opers_list_t;

typedef struct Tag_lte_oam_t {
	uint32_t peer_ip_addr;
	uint16_t peer_port;

	uint16_t heartbeat_lost;
    uint32_t p_lte_addr;

	oam_opers_list_t * p_opers_list;
	
	uint16_t  env_scan_interval;  //unit is 1s ???
	uint16_t  cells_para_scan_interval;  //unit is 1s
	
	uint8_t  cn_oper; /* 0:china mobile com, 1:china unicom, 2:china telcom */
	uint8_t  band;    /* 1:band1, 3:band3, 38:band38, 39:band39, 40:band40, 41:band41: */
	uint8_t  u8IMSI[8]; /* for target set */
	uint8_t  index; 
	uint8_t  time_span[TIME_STEP_NUM]; 
} lte_oam_t; 

#pragma pack ()

/////////////////////////////////////////////////////////////////////////
int  lte_oam_init(lte_state_t *lte_state);
void app_oam_msg_proc(lte_oam_t *p_oam_lte, char *buf, int len); 
void oam_timeout_msg_dispatch(char *buf, int len);

void oam_lte_imsi_ind(lte_oam_t *p_lte_oam, uint8_t *p_IMSI); 
void oam_lte_sniff_info_ind(lte_oam_t *p_lte_oam, lte_para_t *ptLtePara);
void oam_lte_reset_rsp(lte_oam_t *p_lte_oam, uint8_t u8ResetInd);
void oam_lte_start_rsp(lte_oam_t *p_lte_oam, uint8_t u8StartInd); 
void oam_lte_set_power_rsp(lte_oam_t *p_lte_oam, uint8_t u8PowerInd);
void oam_lte_sniffer_rsp(lte_oam_t *p_lte_oam);
void oam_lte_mode_ind(lte_oam_t *p_lte_oam, uint8_t mode); 
void oam_lte_err_ind(lte_oam_t *p_lte_oam, uint8_t errNo);
void oam_lte_para_update(lte_state_t *lte_state);
void oam_lte_cell_report(lte_state_t *lte_state, at_cmgrmi_output_t * pOut) ; 
int  oam_set_eNB_para(lte_oam_t *p_lte_oam);
int  oam_set_env_band(TModem *ptModem, uint8_t  band); 
uint8_t oam_get_oper_byCnType(uint16_t mcc, uint8_t  mnc);

#endif 
