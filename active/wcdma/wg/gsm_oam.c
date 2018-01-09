#include "oss.h"
#include "protocol_common.h"
#include "modem.h"
#include "wg_common.h"
#include "gsm_cfg.h"
#include "gsm_sm.h"
#include "gsm_encode.h"
#include "gsm.h"
#include "gsm_oam.h"

/////////////////////////////////////////////////////////////////////////
void gsm_oam_power_set(gsm_state_t *gsm_state, uint8_t power)
{
	oam_gsm_set_power_req_t req;

	memset(&req, 0, sizeof(req));

	req.msgHeader.msgType = EVENT_OAM_GSM_SET_POWER_REQ;
	req.msgHeader.msgLen  = htons(sizeof(req) - sizeof(wg_msg_head_t));
	req.power          = power;

	gsm_state_machine(gsm_state, (uint8_t *)&req, sizeof(req));
}
/////////////////////////////////////////////////////////////////////////
void gsm_oam_mobile_power_set(gsm_state_t *gsm_state, uint16_t power)
{
	oam_gsm_mobile_power_set_req_t req;

	memset(&req, 0, sizeof(req));

	req.msgHeader.msgType = EVENT_OAM_GSM_MOBILE_POWER_SET_REQ;
	req.msgHeader.msgLen  = htons(sizeof(req) - sizeof(wg_msg_head_t));
	req.power             = power;

	gsm_state_machine(gsm_state, (uint8_t *)&req, sizeof(req));
}

/////////////////////////////////////////////////////////////////////////
void gsm_oam_reset(gsm_state_t *gsm_state)
{
	oam_gsm_reset_req_t	req;

	memset(&req, 0, sizeof(req));

	req.msgHeader.msgType = EVENT_OAM_GSM_RESET_REQ;
	req.msgHeader.msgLen  = htons(sizeof(req) - sizeof(wg_msg_head_t));

	gsm_state_machine(gsm_state, (uint8_t *)&req, sizeof(req));
}
/////////////////////////////////////////////////////////////////////////
void gsm_oam_rf_set(gsm_state_t *gsm_state, uint8_t flag)
{
	oam_gsm_rf_set_req_t	req;

	memset(&req, 0, sizeof(req));

	req.msgHeader.msgType = EVENT_OAM_GSM_RF_SET_REQ;
	req.msgHeader.msgLen  = htons(sizeof(req) - sizeof(wg_msg_head_t));
	req.flag  = flag;

	gsm_state_machine(gsm_state, (uint8_t *)&req, sizeof(req));
}
/////////////////////////////////////////////////////////////////////////
void gsm_oam_inner_amp_set(gsm_state_t *gsm_state, uint8_t flag)
{
	oam_gsm_amp_set_req_t	req;
	int index;

	memset(&req, 0, sizeof(req));

	gsm_state->inner_amp_flag = flag;
//	req.msgHeader.msgType = EVENT_WG_DEV_AMP_SET_REQ;
	req.msgHeader.msgType = EVENT_OAM_INNER_AMP_SET_REQ;
	memset(req.cmd, 0, 256);
/*
	if(flag)
	{
		sprintf(req.cmd, "en_paa=%d\n", 011);
	}
	else
	{
		sprintf(req.cmd, "en_paa=000\n");
	}
*/
	index = strlen(req.cmd);
	req.cmd[index] = 0xf0;
	req.cmd[index + 1] = 0xf0;
	req.msgHeader.msgLen  = strlen(req.cmd);

	gsm_state_machine(gsm_state, (uint8_t *)&req, sizeof(req));
}
/////////////////////////////////////////////////////////////////////////
void gsm_oam_lbs_set(gsm_state_t *gsm_state, uint8_t flag)
{
	oam_gsm_lbs_set_req_t	req;

	memset(&req, 0, sizeof(req));

	req.msgHeader.msgType = EVENT_OAM_GSM_LBS_SET_REQ;
	req.msgHeader.msgLen  = htons(sizeof(req) - sizeof(wg_msg_head_t));
	req.flag = flag;

	gsm_state_machine(gsm_state, (uint8_t *)&req, sizeof(req));
}

/////////////////////////////////////////////////////////////////////////
void gsm_oam_alarm_set(gsm_state_t *gsm_state, uint8_t *imsi)
{
	oam_gsm_alarm_set_req_t	req;

	memset(&req, 0, sizeof(req));

	req.msgHeader.msgType = EVENT_OAM_GSM_ALARM_SET_REQ;
	req.msgHeader.msgLen  = htons(sizeof(req) - sizeof(wg_msg_head_t));
	strcpy(req.imsi, imsi);

	gsm_state_machine(gsm_state, (uint8_t *)&req, sizeof(req));
}
/////////////////////////////////////////////////////////////////////////
void gsm_oam_whitelist_set(gsm_state_t *gsm_state, uint8_t *imsi)
{
	oam_gsm_whitelist_set_req_t	req;

	memset(&req, 0, sizeof(req));

	req.msgHeader.msgType = EVENT_OAM_GSM_WHITELIST_SET_REQ;
	req.msgHeader.msgLen  = htons(sizeof(req) - sizeof(wg_msg_head_t));
	strcpy(req.imsi, imsi);

	gsm_state_machine(gsm_state, (uint8_t *)&req, sizeof(req));
}
/////////////////////////////////////////////////////////////////////////
void gsm_oam_sms_send(gsm_state_t *gsm_state, uint8_t *imsi, uint8_t *phone, uint8_t *sms)
{
	oam_gsm_sms_send_req_t	req;

	memset(&req, 0, sizeof(req));

	req.msgHeader.msgType = EVENT_OAM_GSM_SMS_SEND_REQ;
	req.msgHeader.msgLen  = htons(sizeof(req) - sizeof(wg_msg_head_t));
	strcpy(req.imsi, imsi);
	strcpy(req.content, sms);
	strcpy(req.phone, phone);

	gsm_state_machine(gsm_state, (uint8_t *)&req, sizeof(req));
}
/////////////////////////////////////////////////////////////////////////
int cmcc = 1;
void gsm_oam_para_set(gsm_state_t *gsm_state, uint16_t arfcn)
{
	oam_gsm_para_set_req_t	req;

	memset(&req, 0, sizeof(req));

	req.msgHeader.msgType = EVENT_OAM_GSM_PARA_SET_REQ;
	req.msgHeader.msgLen  = htons(sizeof(req) - sizeof(wg_msg_head_t));

	if(cmcc)
	{
		req.mnc    = 00;
		req.arfcn0 = arfcn;
		req.arfcn1 = 0;   //only a channel 
	}
	else
	{
		req.mnc    = 01;
		req.arfcn0 = 120;
		req.arfcn1 = 0;
	}

	req.bsic   = 27;
	req.cid    = 8897;
	req.lac    = 25263;
	req.mcc    = 460;
	req.nb_cell_num = 0;

	gsm_state_machine(gsm_state, (uint8_t *)&req, sizeof(req));
}

/////////////////////////////////////////////////////////////////////////
void gsm_oam_time_set(gsm_state_t *gsm_state)
{
	oam_gsm_time_set_req_t	req;

	memset(&req, 0, sizeof(req));

	req.msgHeader.msgType = EVENT_OAM_GSM_TIME_SET_REQ;
	req.msgHeader.msgLen  = htons(sizeof(req) - sizeof(wg_msg_head_t));

	strcpy(req.year, "2016"); 
	strcpy(req.month, "01"); 
	strcpy(req.day, "10"); 
	strcpy(req.hour, "11"); 
	strcpy(req.minute, "23"); 
	strcpy(req.second, "16"); 

	gsm_state_machine(gsm_state, (uint8_t *)&req, sizeof(req));
}
