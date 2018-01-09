#include "oss.h"
#include "oss_uart.h"
#include "protocol_common.h"
#include "modem.h"
#include "lte_cfg.h"
#include "lte_sm.h"
#include "lte.h"

/////////////////////////////////////////////////////////////////////////
int lte_cfg_init(lte_state_t *lte_state, uint8_t dst_bsId, uint8_t dst_modId, uint8_t src_bsId, uint8_t src_modId, uint32_t peer_ip_addr, uint16_t peer_port)
{
	uint8_t band, rejCause, relCause;
	uint16_t arfcn, pci;
	uint16_t earfcn_array[16];
	uint8_t band_array[16];

	lte_load_cfg_rsp_t *lte_cfg;

	lte_state->dst_bsId  = dst_bsId; 
	lte_state->dst_modId = dst_modId; 
	lte_state->src_bsId  = src_bsId;
	lte_state->src_modId = src_modId;

	lte_state->session_id = 0;
	lte_state->peer_ip_addr = peer_ip_addr;
	lte_state->peer_port    = peer_port;

    lte_state->work_mode = O_LTE_USER_CFG_WORKMODE;
	lte_state->syn_mode = O_LTE_NO_SYNC; //O_LTE_SYNC
	
	lte_cfg = &lte_state->lte_cfg;

	//setup config 
	//lte_cfg->u16WorkMode = O_LTE_USER_CFG_WORKMODE;
	//lte_cfg->u16SynMode  = O_LTE_NO_SYNC; //O_LTE_SYNC

	lte_cfg->strPbsPara.u8InitialValueTag = 4; // MIB update ind

	// setup snifffer param
	earfcn_array[0] = 1650;
	band_array[0] = 3;
	lte_sniffer_cfg(lte_cfg, 1, &band_array[0], &earfcn_array[0]);

	//setup phs param
	lte_pbs_cfg_set(&lte_cfg->strPbsPara, 460, 1, 3, 1650, 5, 2, 131, 4);

	//setup sync param
	arfcn = 0;
	pci = 0;
	band = 3;
	lte_sync_cfg(&lte_cfg->strSynPara, band, arfcn, pci);

	//setup sync param
	band     = 0;
	arfcn    = 11;
	rejCause = 0;
	relCause = 0;
	lte_rel_cfg(&lte_cfg->strRelPara, band, arfcn, rejCause, relCause);

    lte_state->drive_to_gsm = 1; //default: drive to 2G
    lte_state->state = LTE_SYS_STAT_CELL_GET;
	return 1;
}

/////////////////////////////////////////////////////////////////////////
void lte_pbs_cfg_set(lte_pbs_para_t *ptPbsCfg, uint16_t mcc, uint8_t mnc, uint8_t band, uint16_t earfcn, uint16_t tac, uint16_t cellid, uint16_t pci, uint8_t pwr)
{
	ptPbsCfg->u16CellId = cellid;
	ptPbsCfg->u16Earfcn = earfcn;
	ptPbsCfg->u16MCC    = mcc;
	ptPbsCfg->u8Mnc     = mnc;

	ptPbsCfg->u16PCI    = pci;
	ptPbsCfg->u16Tac    = tac;
	ptPbsCfg->u8Band    = band;
	ptPbsCfg->u8pwr     = pwr;
}

/////////////////////////////////////////////////////////////////////////
void lte_sync_cfg(lte_syn_para_t *ptSyncCfg, uint8_t band, uint16_t earfcn, uint16_t pci)
{
	ptSyncCfg->u16Earfcn = earfcn;
	ptSyncCfg->u16PCI    = pci;
	ptSyncCfg->u8Band    = band;
}
/////////////////////////////////////////////////////////////////////////
void lte_sniffer_cfg(lte_load_cfg_rsp_t *lte_cfg, uint8_t num, uint8_t *band, uint16_t *earfcn)
{
	lte_sniffer_para_t *sniff_para;
	int loop;

	lte_cfg->u32SnifferParaNum = num;
	for(loop=0; loop < num; loop++)
	{
		sniff_para = &lte_cfg->strSnifferPara[loop];
		sniff_para->u8Band    = band[loop];
		sniff_para->u16Earfcn = earfcn[loop];
		sniff_para->u8Rsv = 0;
	}

}

/////////////////////////////////////////////////////////////////////////
void lte_rel_cfg(lte_rel_para_t *ptRelCfg, uint8_t band, uint16_t arfcn, uint8_t rejCause, uint8_t relCause)
{
	ptRelCfg->u8Band   = band;
	ptRelCfg->u16Arfcn = arfcn;
	ptRelCfg->u8RejectCause  = rejCause;
	ptRelCfg->u8ReleaseCause = relCause;
}

//////////////////////////////////////////////////////////////////////
