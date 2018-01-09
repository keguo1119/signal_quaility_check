#include "oss.h"
#include "oss_uart.h"
#include "protocol_common.h"
#include "modem.h"
#include "wg_common.h"
#include "gsm_cfg.h"
#include "gsm_sm.h"
#include "gsm.h"

/////////////////////////////////////////////////////////////////////////
int gsm_cfg_init(gsm_state_t *gsm_state, uint8_t channel, uint8_t trx, 
				 uint32_t peer_ip_addr, uint16_t peer_port, uint32_t uart_ip_addr)
{
	uint8_t band, rejCause, relCause;
	uint16_t arfcn, pci;
	uint16_t earfcn_array[16];
	uint8_t band_array[16];

	gsm_load_cfg_rsp_t *gsm_cfg;

	gsm_state->channel  = channel;
	gsm_state->trx      = trx;

	gsm_state->uart_ip_addr = uart_ip_addr;
	gsm_state->peer_ip_addr = peer_ip_addr;
	gsm_state->peer_port    = peer_port;

	gsm_cfg = &gsm_state->gsm_cfg;

	//setup sync param
	arfcn = 11223;
	pci = 11;
	band = 3;
	gsm_sync_cfg(&gsm_cfg->strSynPara, band, arfcn, pci);

	//setup sync param
	band     = 1;
	arfcn    = 12;
	rejCause = 1;
	relCause = 1;
	gsm_rel_cfg(&gsm_cfg->strRelPara, band, arfcn, rejCause, relCause);

	return 1;
}

/////////////////////////////////////////////////////////////////////////
void gsm_pbs_cfg_set(gsm_pbs_para_t *ptPbsCfg, uint16_t mcc, uint8_t mnc, uint8_t band, uint16_t earfcn, uint16_t tac, uint16_t cellid, uint16_t pci, uint8_t pwr)
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
void gsm_sync_cfg(gsm_syn_para_t *ptSyncCfg, uint8_t band, uint16_t earfcn, uint16_t pci)
{
	ptSyncCfg->u16Earfcn = earfcn;
	ptSyncCfg->u16PCI    = pci;
	ptSyncCfg->u8Band    = band;
}
/////////////////////////////////////////////////////////////////////////
void gsm_sniffer_cfg(gsm_load_cfg_rsp_t *gsm_cfg, uint8_t num, uint8_t *band, uint16_t *earfcn)
{
	gsm_sniffer_para_t *sniff_para;
	int loop;

	gsm_cfg->u32SnifferParaNum = num;
	for(loop=0; loop < num; loop++)
	{
		sniff_para = &gsm_cfg->strSnifferPara[loop];
		sniff_para->u8Band    = band[loop];
		sniff_para->u16Earfcn = earfcn[loop];
		sniff_para->u8Rsv = 0;
	}

}

/////////////////////////////////////////////////////////////////////////
void gsm_rel_cfg(gsm_rel_para_t *ptRelCfg, uint8_t band, uint16_t arfcn, uint8_t rejCause, uint8_t relCause)
{
	ptRelCfg->u8Band   = band;
	ptRelCfg->u16Arfcn = arfcn;
	ptRelCfg->u8RejectCause  = rejCause;
	ptRelCfg->u8ReleaseCause = relCause;
}

//////////////////////////////////////////////////////////////////////
