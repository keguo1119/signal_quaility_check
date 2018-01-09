#include "oss.h"
#include "protocol_common.h"
#include "modem.h"
#include "wg_common.h"
#include "wcdma_cfg.h"
#include "wcdma_sm.h"
#include "wcdma_encode.h"
#include "wcdma.h"
#include "wcdma_oam.h"

/////////////////////////////////////////////////////////////////////////
void wcdma_oam_power_set(wcdma_state_t *wcdma_state, uint16_t power)
{
	oam_wcdma_set_power_req_t req;

	memset(&req, 0, sizeof(req));

	req.msgHeader.msgType = EVENT_OAM_WCDMA_SET_POWER_REQ;
	req.msgHeader.msgLen  = htons(sizeof(req) - sizeof(wg_msg_head_t));
	req.u16Power          = htons(power);

	wcdma_state_machine(wcdma_state, (uint8_t *)&req, sizeof(req));
}
/////////////////////////////////////////////////////////////////////////
void wcdma_oam_reset(wcdma_state_t *wcdma_state)
{
	oam_wcdma_reset_req_t	req;

	memset(&req, 0, sizeof(req));

	req.msgHeader.msgType = EVENT_OAM_WCDMA_RESET_REQ;
	req.msgHeader.msgLen  = htons(sizeof(req) - sizeof(wg_msg_head_t));

	wcdma_state_machine(wcdma_state, (uint8_t *)&req, sizeof(req));
}
/////////////////////////////////////////////////////////////////////////
void wcdma_oam_rf_set(wcdma_state_t *wcdma_state, uint8_t flag)
{
	oam_wcdma_rf_set_req_t	req;

	memset(&req, 0, sizeof(req));

	req.msgHeader.msgType = EVENT_OAM_WCDMA_RF_SET_REQ;
	req.msgHeader.msgLen  = htons(sizeof(req) - sizeof(wg_msg_head_t));
	req.flag = flag;

	wcdma_state_machine(wcdma_state, (uint8_t *)&req, sizeof(req));
}
/////////////////////////////////////////////////////////////////////////
void wcdma_oam_lbs_set(wcdma_state_t *wcdma_state, uint8_t flag)
{
	oam_wcdma_lbs_set_req_t	req;

	memset(&req, 0, sizeof(req));

	req.msgHeader.msgType = EVENT_OAM_WCDMA_LBS_SET_REQ;
	req.msgHeader.msgLen  = htons(sizeof(req) - sizeof(wg_msg_head_t));
	req.flag = flag;

	wcdma_state_machine(wcdma_state, (uint8_t *)&req, sizeof(req));
}

/////////////////////////////////////////////////////////////////////////
void wcdma_oam_para_set(wcdma_state_t *wcdma_state)
{
	oam_wcdma_para_set_req_t	req;

	memset(&req, 0, sizeof(req));

	req.msgHeader.msgType = EVENT_OAM_WCDMA_PARA_SET_REQ;
	req.msgHeader.msgLen  = htons(sizeof(req) - sizeof(wg_msg_head_t));

	req.mnc    = 01;
	req.arfcn0 = 120;

	req.bsic   = 0x42;
	req.cid    = 1234;
	req.lac    = 6000;
	req.mcc    = 460;
	req.rac    = 2;

	wcdma_state_machine(wcdma_state, (uint8_t *)&req, sizeof(req));
}

