#include "oss.h"
#include "protocol_common.h"
#include "modem.h"
#include "wg_common.h"
#include "wcdma_cfg.h"
#include "wcdma_sm.h"
#include "wcdma_encode.h"
#include "wcdma.h"

uint8_t g_encode_buf[MAX_ENCODE_BUF_SIZE + 1];
/////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
uint32_t wg_wcdma_para_encode(wcdma_state_t *state, uint8_t *buf, uint32_t max_size)
{
	wg_msg_one_param_encode(buf, "MCC", state->mcc, 0);
	wg_msg_one_param_encode(buf, "MNC", state->mnc, 0);
	wg_msg_one_param_encode(buf, "LAC", state->lac, 0);
	wg_msg_one_param_encode(buf, "CI", state->cid, 0);
	wg_msg_one_param_encode(buf, "BSIC", state->bsic, 0);
	wg_msg_one_param_encode(buf, "PSC", state->psc, 0);
	wg_msg_one_param_encode(buf, "RAC", state->rac, 0);
	wg_msg_one_param_encode(buf, "ARFCN", state->arfcn0, 0);	
	wg_msg_one_param_encode(buf, "CPICH", state->cpich_power, 0);	

	return strlen(buf);
}
/////////////////////////////////////////////////////////////////////////
uint32_t wg_wcdma_para_rsp_decode(uint8_t *buf, uint32_t len)
{
	wg_msg_common_rsp_t *rsp = wg_msg_common_rsp_decode(buf);

	return rsp->ret;
}

/////////////////////////////////////////////////////////////////////////
uint32_t wg_wcdma_power_encode(wcdma_state_t *state, uint8_t *buf, uint32_t max_size)
{
	*buf = state->power;

	return ENCODE_POWER_LEN;
}
/////////////////////////////////////////////////////////////////////////
uint32_t wg_wcdma_power_set_rsp_decode(uint8_t *buf, uint32_t len)
{
	wg_msg_common_rsp_t *rsp = wg_msg_common_rsp_decode(buf);

	return rsp->ret;
}
/////////////////////////////////////////////////////////////////////////
uint32_t wg_wcdma_rf_encode(wcdma_state_t *state, uint8_t *buf, uint32_t max_size)
{
	*buf     = state->work_mode;
	return ENCODE_RF_START_LEN;
}
/////////////////////////////////////////////////////////////////////////
uint32_t wg_wcdma_rf_set_rsp_decode(uint8_t *buf, uint32_t len)
{
	wg_msg_common_rsp_t *rsp = wg_msg_common_rsp_decode(buf);

	return rsp->ret;
}
/////////////////////////////////////////////////////////////////////////
uint32_t wg_wcdma_lbs_start_encode(wcdma_state_t *state, uint8_t *buf, uint32_t max_size)
{
	memcpy(buf, state->mobile_imsi, MAX_IMSI_LEN);
	*(buf + MAX_IMSI_LEN) = state->arfcn1>>8;
	*(buf + MAX_IMSI_LEN + 1) = state->arfcn1 & 0xff;

	return ENCODE_LBS_START_LEN;
}
/////////////////////////////////////////////////////////////////////////
uint32_t wg_wcdma_lbs_rsp_decode(uint8_t *buf, uint32_t len)
{
	wg_msg_common_rsp_t *rsp = wg_msg_common_rsp_decode(buf);

	return rsp->ret;
}
/////////////////////////////////////////////////////////////////////////
uint32_t wg_wcdma_lbs_stop_encode(wcdma_state_t *state, uint8_t *buf, uint32_t max_size)
{
	/* nothing to do */
	return ENCODE_LBS_STOP_LEN;
}
