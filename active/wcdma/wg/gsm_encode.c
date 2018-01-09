#include "oss.h"
#include "protocol_common.h"
#include "modem.h"
#include "wg_common.h"
#include "gsm_cfg.h"
#include "gsm_sm.h"
#include "gsm_encode.h"
#include "gsm.h"

uint8_t g_encode_buf[MAX_ENCODE_BUF_SIZE + 1];
/////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////
uint32_t wg_gsm_para_encode(gsm_state_t *state, uint8_t *buf, uint32_t max_size)
{
	uint32_t loop;
	uint8_t  local_buf[64];

	memset(buf, 0, max_size);

	wg_msg_one_param_encode(buf, "MCC", state->mcc, 0);
	
	memset(local_buf, 0, 64);
	sprintf(local_buf, "%s#%02d@", "MNC", state->mnc);
	strcat(buf, local_buf);

	wg_msg_one_param_encode(buf, "LAC", state->lac, 0);
	wg_msg_one_param_encode(buf, "CI", state->cid, 0);
	wg_msg_one_param_encode(buf, "BSIC", state->bsic, 0);
	wg_msg_one_param_encode(buf, "ARFCN0", state->arfcn0, 0);
	wg_msg_one_param_encode(buf, "ARFCN1", state->arfcn1, 0);

	memset(local_buf, 0, 64);
	for(loop=0; loop < state->nb_cell_num; loop++)
	{
		sprintf(local_buf, "NRF%d", loop+1);
		wg_msg_one_param_encode(buf, local_buf, state->nb_arfcn[loop], 0);
	}

	wg_msg_one_param_encode(buf, "RACH", state->rach, 1);

	// no TMSI mode


	return strlen(buf);
}
/////////////////////////////////////////////////////////////////////////
uint32_t wg_gsm_para_rsp_decode(uint8_t *buf, uint32_t len)
{
	wg_msg_common_rsp_t *rsp = wg_msg_common_rsp_decode(buf);

	return rsp->ret;
}
/////////////////////////////////////////////////////////////////////////
uint32_t wg_gsm_alarm_encode(gsm_state_t *state, uint8_t *buf, uint32_t max_size)
{
	uint32_t len = wg_gsm_whitelist_encode(state, buf, max_size);

	return len;
}
/////////////////////////////////////////////////////////////////////////
uint32_t wg_gsm_alarm_rsp_decode(uint8_t *buf, uint32_t len)
{
	wg_msg_common_rsp_t *rsp = wg_msg_common_rsp_decode(buf);

	return rsp->ret;
}
/////////////////////////////////////////////////////////////////////////
uint32_t wg_gsm_whitelist_encode(gsm_state_t *state, uint8_t *buf, uint32_t max_size)
{
	memset(buf, 0, ENCODE_WHITELIST_LEN + 1);

	switch(state->id_flag)
	{
	case ID_IMSI_AND_IMEI:
	case ID_IMSI_OR_IMEI:
		memcpy(buf, state->mobile_imsi, MAX_IMSI_LEN);
		memcpy(buf + MAX_IMSI_LEN, state->mobile_imei, MAX_IMSI_LEN);
	break;

	case ID_IMEI:
		memcpy(buf + MAX_IMSI_LEN, state->mobile_imei, MAX_IMSI_LEN);
	break;

	case ID_IMSI:
		memcpy(buf, state->mobile_imsi, MAX_IMSI_LEN);
	break;
	
	default:
	break;
	}

	*(buf + 2 * MAX_IMSI_LEN) = state->id_flag;

	return ENCODE_WHITELIST_LEN;
}
/////////////////////////////////////////////////////////////////////////
uint32_t wg_gsm_whiltelist_rsp_decode(uint8_t *buf, uint32_t len)
{
	wg_msg_common_rsp_t *rsp = wg_msg_common_rsp_decode(buf);

	return rsp->ret;
}
/////////////////////////////////////////////////////////////////////////
uint32_t wg_gsm_sms_encode(gsm_state_t *state, uint8_t *buf, uint32_t max_size)
{
	memcpy(buf, state->mobile_imsi, MAX_IMSI_LEN);
	memcpy(buf + MAX_IMSI_LEN, state->caller_no, MAX_IMSI_LEN);

	memcpy(buf + MAX_IMSI_LEN*2, state->sms_content, state->sms_len);

	return (state->sms_len + ENCODE_SMS_LEN);
}
/////////////////////////////////////////////////////////////////////////
uint32_t wg_gsm_sms_send_rsp_decode(uint8_t *buf, uint32_t len)
{
	wg_msg_common_rsp_t *rsp = wg_msg_common_rsp_decode(buf);

	return rsp->ret;
}
/////////////////////////////////////////////////////////////////////////
uint32_t wg_gsm_power_encode(gsm_state_t *state, uint8_t *buf, uint32_t max_size)
{
	*buf       = (state->power >> 8) & 0xff;
	*(buf + 1) = state->power & 0xff;

	return ENCODE_POWER_LEN;
}
/////////////////////////////////////////////////////////////////////////
uint32_t wg_gsm_power_set_rsp_decode(uint8_t *buf, uint32_t len)
{
	wg_msg_common_rsp_t *rsp = wg_msg_common_rsp_decode(buf);

	return rsp->ret;
}
/////////////////////////////////////////////////////////////////////////
uint32_t wg_gsm_rf_encode(gsm_state_t *state, uint8_t *buf, uint32_t max_size)
{
	*buf     = state->work_mode;
	* (buf+1) = state->arfcn_num;
	return ENCODE_RF_START_LEN;
}
/////////////////////////////////////////////////////////////////////////
uint32_t wg_gsm_inner_amp_encode(gsm_state_t *state, uint8_t *buf, uint32_t max_size)
{
	uint32_t index;

	memset(buf, 0, max_size);
	if(state->inner_amp_flag)
	{
		sprintf(buf, "en_paa=011\n");
	}
	else
	{
		sprintf(buf, "en_paa=000\n");
	}
	index = strlen(buf);
	buf[index] = 0xf0;
	buf[index + 1] = 0xf0;

	return strlen(buf);;
}
/////////////////////////////////////////////////////////////////////////
uint32_t wg_gsm_rf_set_rsp_decode(uint8_t *buf, uint32_t len)
{
	wg_msg_common_rsp_t *rsp = wg_msg_common_rsp_decode(buf);

	return rsp->ret;
}
/////////////////////////////////////////////////////////////////////////
uint32_t wg_gsm_lbs_start_encode(gsm_state_t *state, uint8_t *buf, uint32_t max_size)
{
	memcpy(buf, state->mobile_imsi, MAX_IMSI_LEN);
	*(buf + MAX_IMSI_LEN) = state->arfcn1>>8;
	*(buf + MAX_IMSI_LEN + 1) = state->arfcn1 & 0xff;

	return ENCODE_LBS_START_LEN;
}
/////////////////////////////////////////////////////////////////////////
uint32_t wg_gsm_lbs_rsp_decode(uint8_t *buf, uint32_t len)
{
	wg_msg_common_rsp_t *rsp = wg_msg_common_rsp_decode(buf);

	return rsp->ret;
}
/////////////////////////////////////////////////////////////////////////
uint32_t wg_gsm_lbs_stop_encode(gsm_state_t *state, uint8_t *buf, uint32_t max_size)
{
	/* nothing to do */
	return ENCODE_LBS_STOP_LEN;
}
/////////////////////////////////////////////////////////////////////////
uint32_t wg_gsm_ring_encode(gsm_state_t *state, uint8_t *buf, uint32_t max_size)
{
	memcpy(buf, state->mobile_imsi, MAX_IMSI_LEN);
	memcpy(buf + MAX_IMSI_LEN, state->caller_no, MAX_IMSI_LEN);

	return ENCODE_RING_LEN;
}
/////////////////////////////////////////////////////////////////////////
uint32_t wg_gsm_ring_rsp_decode(uint8_t *buf, uint32_t len)
{
	wg_msg_common_rsp_t *rsp = wg_msg_common_rsp_decode(buf);

	return rsp->ret;
}
/////////////////////////////////////////////////////////////////////////
uint32_t wg_gsm_mobile_power_set_encode(gsm_state_t *state, uint8_t *buf, uint32_t max_size)
{
	*buf = state->mobile_power;

	return ENCODE_POWER_LEN;
}
/////////////////////////////////////////////////////////////////////////
uint32_t wg_gsm_mobile_power_set_rsp_decode(uint8_t *buf, uint32_t len)
{
	wg_msg_common_rsp_t *rsp = wg_msg_common_rsp_decode(buf);

	return rsp->ret;
}
////////////////////////////////////////////////////////////////////////
uint32_t wg_gsm_lbs_ring_encode(gsm_state_t *state, uint8_t *buf, uint32_t max_size)
{
	return wg_gsm_ring_encode(state, buf, max_size);

}
/////////////////////////////////////////////////////////////////////////
uint32_t wg_gsm_lbs_ring_rsp_decode(uint8_t *buf, uint32_t len)
{
	wg_msg_common_rsp_t *rsp = wg_msg_common_rsp_decode(buf);

	return rsp->ret;
}
////////////////////////////////////////////////////////////////////////
uint32_t wg_gsm_time_set_encode(gsm_state_t *state, uint8_t *buf, uint32_t max_size)
{
	memset(buf, 0, max_size);

	strcat(buf, state->year);
	strcat(buf, state->month);
	strcat(buf, state->day);
	strcat(buf, state->hour);
	strcat(buf, state->minute);
	strcat(buf, state->second);

	return ENCODE_TIME_LEN;
}
/////////////////////////////////////////////////////////////////////////
uint32_t wg_gsm_time_set_rsp_decode(uint8_t *buf, uint32_t len)
{
	wg_msg_common_rsp_t *rsp = wg_msg_common_rsp_decode(buf);

	return rsp->ret;
}