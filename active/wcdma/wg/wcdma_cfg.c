#include "oss.h"
#include "oss_uart.h"
#include "protocol_common.h"
#include "modem.h"
#include "wg_common.h"
#include "wcdma_cfg.h"
#include "wcdma_sm.h"
#include "wcdma.h"

/////////////////////////////////////////////////////////////////////////
int wcdma_cfg_init(wcdma_state_t *wcdma_state, uint8_t channel, uint8_t trx, uint32_t peer_ip_addr, uint16_t peer_port)
{
	uint8_t band, rejCause, relCause;
	uint16_t arfcn, pci;
	uint16_t earfcn_array[16];
	uint8_t band_array[16];

	wcdma_load_cfg_rsp_t *wcdma_cfg;

	wcdma_state->channel  = channel;
	wcdma_state->trx = trx;

	wcdma_state->peer_ip_addr = peer_ip_addr;
	wcdma_state->peer_port    = peer_port;

	wcdma_cfg = &wcdma_state->wcdma_cfg;

	//setup config 
	wcdma_cfg->u16WorkMode = O_WCDMA_USER_CFG_WORKMODE;
	wcdma_cfg->u16SynMode  = O_WCDMA_SYNC; //O_WCDMA_NO_SYNC

	wcdma_cfg->strPbsPara.u8InitialValueTag = 1; // MIB update ind

	// setup snifffer param
	earfcn_array[0] = 11223;
	band_array[0] = 3;
	wcdma_sniffer_cfg(wcdma_cfg, 1, &band_array[0], &earfcn_array[0]);

	//setup phs param
	wcdma_pbs_cfg_set(&wcdma_cfg->strPbsPara, 460, 1, 3, 100, 1, 1, 1, 12);

	//setup sync param
	arfcn = 11223;
	pci = 11;
	band = 3;
	wcdma_sync_cfg(&wcdma_cfg->strSynPara, band, arfcn, pci);

	//setup sync param
	band     = 1;
	arfcn    = 12;
	rejCause = 1;
	relCause = 1;
	wcdma_rel_cfg(&wcdma_cfg->strRelPara, band, arfcn, rejCause, relCause);

	return 1;
}

/////////////////////////////////////////////////////////////////////////
void wcdma_pbs_cfg_set(wcdma_pbs_para_t *ptPbsCfg, uint16_t mcc, uint8_t mnc, uint8_t band, uint16_t earfcn, uint16_t tac, uint16_t cellid, uint16_t pci, uint8_t pwr)
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
void wcdma_sync_cfg(wcdma_syn_para_t *ptSyncCfg, uint8_t band, uint16_t earfcn, uint16_t pci)
{
	ptSyncCfg->u16Earfcn = earfcn;
	ptSyncCfg->u16PCI    = pci;
	ptSyncCfg->u8Band    = band;
}
/////////////////////////////////////////////////////////////////////////
void wcdma_sniffer_cfg(wcdma_load_cfg_rsp_t *wcdma_cfg, uint8_t num, uint8_t *band, uint16_t *earfcn)
{
	wcdma_sniffer_para_t *sniff_para;
	int loop;

	wcdma_cfg->u32SnifferParaNum = num;
	for(loop=0; loop < num; loop++)
	{
		sniff_para = &wcdma_cfg->strSnifferPara[loop];
		sniff_para->u8Band    = band[loop];
		sniff_para->u16Earfcn = earfcn[loop];
		sniff_para->u8Rsv = 0;
	}

}

/////////////////////////////////////////////////////////////////////////
void wcdma_rel_cfg(wcdma_rel_para_t *ptRelCfg, uint8_t band, uint16_t arfcn, uint8_t rejCause, uint8_t relCause)
{
	ptRelCfg->u8Band   = band;
	ptRelCfg->u16Arfcn = arfcn;
	ptRelCfg->u8RejectCause  = rejCause;
	ptRelCfg->u8ReleaseCause = relCause;
}

//////////////////////////////////////////////////////////////////////
