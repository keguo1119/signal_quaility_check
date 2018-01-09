#include "oss.h"
#include "protocol_common.h"
#include "modem.h"
#include "wg_common.h"
#include "gsm_cfg.h"
#include "gsm_sm.h"
#include "gsm_encode.h"
#include "gsm.h"
#include "gsm_oam.h"

uint8_t g_encode_buf[MAX_ENCODE_BUF_SIZE + 1];
////////////////////////////////////////////////////////////////////////
void gsm_msg_proc(int8_t *buf, int32_t len);
/////////////////////////////////////////////////////////////////////////
void gsm_state_set(gsm_state_t *gsm_state, uint32_t state)
{
	gsm_state->state = state;
}

/////////////////////////////////////////////////////////////////////////
void wg_gsm_heartbeat_send(gsm_state_t *state)
{
	wg_msg_send(state->peer_ip_addr, state->peer_port, state->channel, state->trx, EVENT_WG_GSM_HEARTBEAT, NULL, 0);
}
/////////////////////////////////////////////////////////////////////////
void wg_gsm_heartbeat_proc(gsm_state_t *state, uint8_t *buf, uint32_t len)
{
	wg_msg_heartbeat_t *heartbeat = (wg_msg_heartbeat_t *)buf;

	switch(heartbeat->status)
	{
	case BOARD_STATUS_IDLE:
	break;

	case BOARD_STATUS_IMSI_CACTHER:
	break;

	case BOARD_STATUS_LOCATION:
	break;

	case BOARD_STATUS_AUTH:
	break;

	case BOARD_STATUS_SW_ERROR:
		wg_dev_reset(state->peer_ip_addr, state->peer_port);
	break;

	case BOARD_STATUS_HW_ERROR:
		wg_dev_reset(state->peer_ip_addr, state->peer_port);
	break;
	
	default:
	break;
	}
}
/////////////////////////////////////////////////////////////////////////
void wg_gsm_para_set_req(gsm_state_t *state, uint8_t *buf, uint32_t len)
{
	uint32_t ret ;
	oam_gsm_para_set_req_t *req = (oam_gsm_para_set_req_t *)buf;

	state->arfcn0 = req->arfcn0;
	state->arfcn1 = req->arfcn1;
	state->bsic   = req->bsic;
	state->cid    = req->cid;
	state->lac    = req->lac;
	state->mcc    = req->mcc;
	state->mnc    = req->mnc;

	ret = wg_gsm_para_encode(state, g_encode_buf, MAX_ENCODE_BUF_SIZE);

	wg_msg_send(state->peer_ip_addr, state->peer_port, state->channel, state->trx, EVENT_WG_GSM_PARA_SET_REQ, g_encode_buf, ret);

}
/////////////////////////////////////////////////////////////////////////
void wg_gsm_para_set_rsp_proc(gsm_state_t *state, uint8_t *buf, uint32_t len)
{
	wg_msg_common_rsp_t * rsp = wg_msg_common_rsp_decode(buf);


}
////////////////////////////////////////////////////////////////////////
void wg_gsm_alarm_set_req(gsm_state_t *state, uint8_t *buf, uint32_t len)
{
	uint32_t ret ;
	oam_gsm_alarm_set_req_t *req = (oam_gsm_alarm_set_req_t *)buf;
	
	strcpy(state->mobile_imsi, req->imsi);

	state->id_flag = 0;

	ret = wg_gsm_alarm_encode(state, g_encode_buf, MAX_ENCODE_BUF_SIZE);

	wg_msg_send(state->peer_ip_addr, state->peer_port, state->channel, state->trx, EVENT_WG_GSM_ALARM_SET_REQ, g_encode_buf, ret);
}
/////////////////////////////////////////////////////////////////////////
void wg_gsm_alarm_set_rsp_proc(gsm_state_t *state, uint8_t *buf, uint32_t len)
{
	wg_msg_alarm_rsp_t * rsp = (wg_msg_alarm_rsp_t *)wg_msg_common_rsp_decode(buf);

	if(rsp->ret == WG_FAIL)
	{

	}
	else if(rsp->ret == WG_OK)
	{
		state->alarm_space = rsp->left_space;
	}
}
////////////////////////////////////////////////////////////////////////
void wg_gsm_whitelist_set_req(gsm_state_t *state, uint8_t *buf, uint32_t len)
{
	uint32_t ret;
	oam_gsm_whitelist_set_req_t	*req = (oam_gsm_whitelist_set_req_t	*)buf;

	strcpy(state->mobile_imsi, req->imsi);

	ret = wg_gsm_whitelist_encode(state, g_encode_buf, MAX_ENCODE_BUF_SIZE);

	wg_msg_send(state->peer_ip_addr, state->peer_port, state->channel, state->trx, EVENT_WG_GSM_WHITELIST_SET_REQ, g_encode_buf, ret);
}
/////////////////////////////////////////////////////////////////////////
void wg_gsm_whitelist_set_rsp_proc(gsm_state_t *state, uint8_t *buf, uint32_t len)
{
	wg_msg_whitelist_rsp_t * rsp = (wg_msg_whitelist_rsp_t *)wg_msg_common_rsp_decode(buf);

	if(rsp->ret == WG_FAIL)
	{

	}
	else if(rsp->ret == WG_OK)
	{
		state->whitelist_space = rsp->left_space;
	}
}
////////////////////////////////////////////////////////////////////////
void wg_gsm_sms_send_req(gsm_state_t *state, uint8_t *buf, uint32_t len)
{
	uint32_t ret = wg_gsm_sms_encode(state, g_encode_buf, MAX_ENCODE_BUF_SIZE);

	wg_msg_send(state->peer_ip_addr, state->peer_port, state->channel, state->trx, EVENT_WG_GSM_SMS_SEND_REQ, g_encode_buf, ret);

}
/////////////////////////////////////////////////////////////////////////
void wg_gsm_sms_send_rsp_proc(gsm_state_t *state, uint8_t *buf, uint32_t len)
{
	wg_msg_sms_send_rsp_t * rsp = wg_msg_common_rsp_decode(buf);

	if(rsp->ret == WG_FAIL)
	{

	}
	else if(rsp->ret == WG_OK)
	{

	}
}
////////////////////////////////////////////////////////////////////////
void wg_gsm_power_set_req(gsm_state_t *state, uint8_t *buf, uint32_t len)
{
	oam_gsm_set_power_req_t  *req = (oam_gsm_set_power_req_t *)buf;
	uint32_t ret ;
	uint16_t power_table[10] = {0x1414, 0x0d0d, 0x0a0a, 0x0707, 0x0505, 0x0000, 0x0};
	
	if(req->power < 6)
	{
		state->power = power_table[req->power];
	}
	else
	{
		state->power = power_table[3];
	}

	ret = wg_gsm_power_encode(state, g_encode_buf, MAX_ENCODE_BUF_SIZE);

	wg_msg_send(state->peer_ip_addr, state->peer_port, state->channel, state->trx, EVENT_WG_GSM_POWER_SET_REQ, g_encode_buf, ret);

}
/////////////////////////////////////////////////////////////////////////
void wg_gsm_power_set_rsp_proc(gsm_state_t *state, uint8_t *buf, uint32_t len)
{
	wg_msg_power_set_rsp_t * rsp = wg_msg_common_rsp_decode(buf);

	if(rsp->ret == WG_FAIL)
	{

	}
	else if(rsp->ret == WG_OK)
	{
		// change status
	}
}
////////////////////////////////////////////////////////////////////////
void wg_gsm_rf_set_req(gsm_state_t *state, uint8_t *buf, uint32_t len)
{
	uint32_t ret = wg_gsm_rf_encode(state, g_encode_buf, MAX_ENCODE_BUF_SIZE);

	wg_msg_send(state->peer_ip_addr, state->peer_port, state->channel, state->trx, EVENT_WG_GSM_RF_ON_REQ, g_encode_buf, ret);

}
////////////////////////////////////////////////////////////////////////
void wg_gsm_inner_amp_set_req(gsm_state_t *state, uint8_t *buf, uint32_t len)
{
	uint32_t ret = wg_gsm_inner_amp_encode(state, g_encode_buf, MAX_ENCODE_BUF_SIZE);

	wg_msg_send(state->peer_ip_addr, state->peer_port, 0, 0, EVENT_WG_DEV_AMP_SET_REQ, g_encode_buf, ret);
}

/////////////////////////////////////////////////////////////////////////
void wg_gsm_rf_on_rsp_proc(gsm_state_t *state, uint8_t *buf, uint32_t len)
{
	wg_msg_rf_on_rsp_t * rsp = wg_msg_common_rsp_decode(buf);

	if(rsp->ret == WG_FAIL)
	{

	}
	else if(rsp->ret == WG_OK)
	{

	}
}

////////////////////////////////////////////////////////////////////////
void wg_gsm_rf_off_req(gsm_state_t *state)
{
	uint32_t len = wg_gsm_rf_encode(state, g_encode_buf, MAX_ENCODE_BUF_SIZE);

	wg_msg_send(state->peer_ip_addr, state->peer_port, state->channel, state->trx, EVENT_WG_GSM_RF_OFF_REQ, g_encode_buf, len);

}
/////////////////////////////////////////////////////////////////////////
void wg_gsm_rf_off_rsp_proc(gsm_state_t *state, uint8_t *buf, uint32_t len)
{
	wg_msg_rf_off_rsp_t * rsp = wg_msg_common_rsp_decode(buf);

	if(rsp->ret == WG_FAIL)
	{

	}
	else if(rsp->ret == WG_OK)
	{

	}
}
////////////////////////////////////////////////////////////////////////
void wg_gsm_lbs_set_req(gsm_state_t *state, uint8_t *buf, uint32_t len)
{
	uint32_t ret = wg_gsm_lbs_start_encode(state, g_encode_buf, MAX_ENCODE_BUF_SIZE);

	wg_msg_send(state->peer_ip_addr, state->peer_port, state->channel, state->trx, EVENT_WG_GSM_LBS_START_REQ, g_encode_buf, ret);

	oss_set_timer_ex(TIME_WG_GSM_LBS_START, gsm_msg_proc, EVENT_WG_GSM_LBS_START_TIMEOUT, (uint32_t)state);
}
////////////////////////////////////////////////////////////////////////
void wg_gsm_lbs_start_rsp_proc(gsm_state_t *state, uint8_t *buf, uint32_t len)
{
	wg_msg_lbs_start_rsp_t * rsp = wg_msg_common_rsp_decode(buf);

	if(rsp->ret == WG_FAIL)
	{

	}
	else if(rsp->ret == WG_OK)
	{

	}
}
////////////////////////////////////////////////////////////////////////
void wg_gsm_lbs_stop_req(gsm_state_t *state, uint8_t *buf, uint32_t len)
{
	uint32_t ret = wg_gsm_lbs_stop_encode(state, g_encode_buf, MAX_ENCODE_BUF_SIZE);

	wg_msg_send(state->peer_ip_addr, state->peer_port, state->channel, state->trx, EVENT_WG_GSM_LBS_STOP_REQ, g_encode_buf, ret);
}
////////////////////////////////////////////////////////////////////////
void wg_gsm_lbs_stop_rsp_proc(gsm_state_t *state, uint8_t *buf, uint32_t len)
{
	wg_msg_lbs_stop_rsp_t * rsp = wg_msg_common_rsp_decode(buf);

	if(rsp->ret == WG_FAIL)
	{

	}
	else if(rsp->ret == WG_OK)
	{

	}
}
////////////////////////////////////////////////////////////////////////
void wg_gsm_ring_req(gsm_state_t *state, uint8_t *buf, uint32_t len)
{
	uint32_t ret = wg_gsm_ring_encode(state, g_encode_buf, MAX_ENCODE_BUF_SIZE);

	wg_msg_send(state->peer_ip_addr, state->peer_port, state->channel, state->trx, EVENT_WG_GSM_RING_REQ, g_encode_buf, ret);

}
////////////////////////////////////////////////////////////////////////
void wg_gsm_ring_rsp_proc(gsm_state_t *state, uint8_t *buf, uint32_t len)
{
	wg_msg_ring_rsp_t * rsp = wg_msg_common_rsp_decode(buf);

	if(rsp->ret == WG_FAIL)
	{

	}
	else if(rsp->ret == WG_OK)
	{

	}
}

////////////////////////////////////////////////////////////////////////
void wg_gsm_auth_req(gsm_state_t *state, uint8_t *buf, uint32_t len)
{
	wg_msg_send(state->peer_ip_addr, state->peer_port, state->channel, state->trx, EVENT_WG_GSM_MOBILE_AUTH_REQ, NULL, 0);

}
////////////////////////////////////////////////////////////////////////
void wg_gsm_mobile_power_set_req(gsm_state_t *state, uint8_t *buf, uint32_t len)
{
	oam_gsm_mobile_power_set_req_t  * req = (oam_gsm_mobile_power_set_req_t *)buf;

	uint32_t ret;
	
	state->mobile_power = req->power;

	ret = wg_gsm_mobile_power_set_encode(state, g_encode_buf, MAX_ENCODE_BUF_SIZE);

	wg_msg_send(state->peer_ip_addr, state->peer_port, state->channel, state->trx, EVENT_WG_GSM_MOBILE_POWER_SET_REQ, g_encode_buf, ret);

}
////////////////////////////////////////////////////////////////////////
void wg_gsm_mobile_power_set_rsp_proc(gsm_state_t *state, uint8_t *buf, uint32_t len)
{
	wg_msg_mobile_power_set_rsp_t * rsp = wg_msg_common_rsp_decode(buf);

	if(rsp->ret == WG_FAIL)
	{

	}
	else if(rsp->ret == WG_OK)
	{

	}
}
////////////////////////////////////////////////////////////////////////
void wg_gsm_lbs_ring_req(gsm_state_t *state, uint8_t *buf, uint32_t len)
{
	uint32_t ret = wg_gsm_lbs_ring_encode(state, g_encode_buf, MAX_ENCODE_BUF_SIZE);

	wg_msg_send(state->peer_ip_addr, state->peer_port, state->channel, state->trx, EVENT_WG_GSM_LBS_RING_REQ, g_encode_buf, ret);

}
////////////////////////////////////////////////////////////////////////
void wg_gsm_lbs_ring_rsp_proc(gsm_state_t *state, uint8_t *buf, uint32_t len)
{
	wg_msg_lbs_ring_rsp_t * rsp = wg_msg_common_rsp_decode(buf);

	if(rsp->ret == WG_FAIL)
	{

	}
	else if(rsp->ret == WG_OK)
	{

	}
}
////////////////////////////////////////////////////////////////////////
void wg_gsm_time_set_req(gsm_state_t *state, uint8_t *buf, uint32_t len)
{
	SYSTEMTIME st;
	uint32_t ret;

	GetLocalTime(&st);

	sprintf(state->day, "%02d", st.wDay);
	sprintf(state->hour , "%02d", st.wHour);
	sprintf(state->minute , "%02d",  st.wMinute);
	sprintf(state->month , "%02d",  st.wMonth);
	sprintf(state->second, "%02d",  st.wSecond);
	sprintf(state->year , "%d",  st.wYear);

	ret = wg_gsm_time_set_encode(state, g_encode_buf, MAX_ENCODE_BUF_SIZE);

	if(state->trx > 0)
	{
		wg_msg_send(state->peer_ip_addr, state->peer_port, state->channel, state->trx, EVENT_WG_GSM_TIME_SET_REQ, g_encode_buf, ret);
	}
	if(state->trx_second > 0)
	{
		wg_msg_send(state->peer_ip_addr, state->peer_port, state->channel, state->trx_second, EVENT_WG_GSM_TIME_SET_REQ, g_encode_buf, ret);
	}
}
////////////////////////////////////////////////////////////////////////
void wg_gsm_time_set_rsp_proc(gsm_state_t *state, uint8_t *buf, uint32_t len)
{
	wg_msg_time_set_rsp_t * rsp = wg_msg_common_rsp_decode(buf);

	if(rsp->ret == WG_FAIL)
	{

	}
	else if(rsp->ret == WG_OK)
	{

	}
}

/////////////////////////////////////////////////////////////////////////
void wg_gsm_heartbeat_timeout(gsm_state_t *gsm_state)
{
	// setup time out proc
	oss_set_timer_ex(O_GSM_HEARTBEAT_TIMER, gsm_msg_proc, EVENT_WG_GSM_HEARTBEAT_TIMEOUT, (uint32_t)gsm_state);

	gsm_state->heartbeat_lost++;

	if(gsm_state->heartbeat_lost > MAX_HEARTBEAT_LOST_NUM)
	{
		gsm_state->state = GSM_SYS_STAT_DISCONN;
	}
}
//////////////////////////////////////////////////////////////////////
void gsm_heartbeat_rsp(gsm_state_t *gsm_state, gsm_heartbeat_rsp_t *ptHeartBeatRsp)
{
	gsm_state->heartbeat_lost = 0;
}

////////////////////////////////////////////////////////////////////////
void wg_gsm_imsi_report_proc(gsm_state_t *state, uint8_t *buf, uint32_t len)
{
	wg_msg_head_t * head = (wg_msg_head_t *)buf;
	uint8_t *ptr = buf + sizeof(wg_msg_head_t);
	uint8_t  local_buf[128];
	uint32_t tmsi, rssi, ta;

	//imsi
	memcpy(local_buf, ptr, IMSI_LEN);
	local_buf[IMSI_LEN] = 0;
	printf("[imsi report from channel=%d trx=%d]%s \n", head->channel, head->trx, local_buf);

	//imei
	ptr += IMSI_LEN;
	ptr++;
	memcpy(local_buf, ptr, IMSI_LEN);
	local_buf[IMSI_LEN] = 0;
	printf("imei=%s ", local_buf);

	//tmsi & RSSI & TA
	ptr += IMEI_LEN;
	ptr++;
	if(ptr == '#')
	{
		sscanf(ptr, "#%x#%x", &rssi, &ta);
		printf("tmsi=0 rssi=%x ta=%x ", rssi, ta);
	}
	else
	{
		sscanf(ptr, "%x#%x#%x", &tmsi, &rssi, &ta);
		printf("tmsi=%x rssi=%x ta=%x ", tmsi, rssi, ta);
	}

	printf("\n");
}
////////////////////////////////////////////////////////////////////////
void wg_gsm_mobile_signal_report_proc(gsm_state_t *state, uint8_t *buf, uint32_t len)
{
	wg_msg_head_t * head = (wg_msg_head_t *)buf;

	state->ta = atoi(buf + sizeof(wg_msg_head_t) + 2);
	buf[2] = 0;
	state->rssi = atoi(buf + sizeof(wg_msg_head_t));
}
/******************************************************************************/
void gsm_sm_idle(gsm_state_t *gsm_state, uint8_t *buf, uint32_t len)
{
	wg_msg_head_t *ptMsg    = (wg_msg_head_t *)buf;

	switch(ptMsg->msgType)
	{
	case EVENT_WG_GSM_HEARTBEAT:
		wg_gsm_heartbeat_proc(gsm_state, buf, len);
	break;
	
	case EVENT_WG_WCDMA_HEARTBEAT:
	break;

	case EVENT_WG_GSM_HEARTBEAT_TIMEOUT:
		wg_gsm_heartbeat_timeout(gsm_state);
	break;

	case EVENT_WG_DEV_CAP_GET_TIMEOUT:
		gsm_sm_start(gsm_state);
	break;

	case EVENT_WG_DEV_CAP_RSP:
		wg_dev_cap_rsp_proc(buf, len);
		oss_del_timer(gsm_state->timer_id, 0 , EVENT_WG_DEV_CAP_GET_TIMEOUT);
		
		//shutdown power amp
		gsm_state->inner_amp_flag = 0;
		wg_gsm_inner_amp_set_req(gsm_state, NULL, 0);
	break;

	case EVENT_WG_DEV_INNER_AMP_SET_RSP:
		wg_gsm_time_set_req(gsm_state, NULL, 0);
	break;

	case EVENT_WG_GSM_TIME_SET_RSP:
		gsm_state_set(gsm_state, GSM_SYS_STAT_RUNNING);
	break;

	case EVENT_WG_DEV_RESET_RSP:
		wg_dev_reset_rsp_proc(buf, len);
	break;

	case 	EVENT_WG_DEV_IP_SET_RSP:
		wg_dev_ip_set_rsp_proc(buf, len);
	break;
		
	default:
	break;
	}
}
/******************************************************************************/
void gsm_sm_dataready(gsm_state_t *gsm_state, uint8_t *buf, uint32_t len)
{
	wg_msg_head_t *ptMsg    = (wg_msg_head_t *)buf;
	wg_msg_heartbeat_t *heartbeat;

	switch(ptMsg->msgType)
	{
	case EVENT_WG_GSM_HEARTBEAT:
		heartbeat = (wg_msg_heartbeat_t   *)(buf);
		wg_gsm_heartbeat_proc(gsm_state, buf, len);
	break;
	
	case EVENT_WG_GSM_HEARTBEAT_TIMEOUT:
		wg_gsm_heartbeat_timeout(gsm_state);
	break;

	case EVENT_WG_GSM_PARA_SET_RSP:
		wg_gsm_para_set_rsp_proc(gsm_state, buf, len);
	break;

	default:
	break;
	}
}
/******************************************************************************/
void gsm_sm_running(gsm_state_t *gsm_state, uint8_t *buf, uint32_t len)
{
	wg_msg_head_t *ptMsg    = (wg_msg_head_t *)buf;
	wg_msg_heartbeat_t  *heartbeat;
	uint16_t  msg_type = 	wg_msg_type_get(buf);

	switch(msg_type)
	{
	case EVENT_WG_GSM_HEARTBEAT:
		heartbeat = (wg_msg_heartbeat_t   *)(buf);
		wg_gsm_heartbeat_proc(gsm_state, buf, len);
	break;
	
	/////////// OAM MSG ///////////////////////////
	case EVENT_OAM_GSM_RESET_REQ:
		wg_dev_reset(gsm_state->peer_ip_addr, gsm_state->peer_port);
	break;

	case EVENT_OAM_GSM_SET_POWER_REQ:
		wg_gsm_power_set_req(gsm_state, buf, len);
	break;

	case EVENT_OAM_GSM_MOBILE_POWER_SET_REQ:
		wg_gsm_mobile_power_set_req(gsm_state, buf, len);
	break;

	case EVENT_OAM_GSM_ALARM_SET_REQ:
		wg_gsm_alarm_set_req(gsm_state, buf, len);
	break;

	case EVENT_OAM_GSM_WHITELIST_SET_REQ:
		wg_gsm_whitelist_set_req(gsm_state, buf, len);
	break;

	case EVENT_OAM_GSM_RF_SET_REQ:
		wg_dev_net_map_uart(gsm_state->sock_of_uart, gsm_state->uart_ip_addr);
		gsm_state->work_mode = 2;
		wg_gsm_rf_set_req(gsm_state, buf, len);
	break;

	case EVENT_OAM_INNER_AMP_SET_REQ:
		wg_gsm_inner_amp_set_req(gsm_state, NULL, 0);
	break;

	case EVENT_OAM_GSM_LBS_SET_REQ:
		wg_dev_net_map_uart(gsm_state->sock_of_uart, gsm_state->uart_ip_addr);
		wg_gsm_lbs_set_req(gsm_state, buf, len);
	break;

	case EVENT_OAM_GSM_SMS_SEND_REQ:
		wg_gsm_sms_send_req(gsm_state, buf, len);
	break;

	case EVENT_OAM_GSM_PARA_SET_REQ:
		wg_gsm_para_set_req(gsm_state, buf, len);
	break;

	case EVENT_OAM_GSM_TIME_SET_REQ:
//		gsm_oam_time_set(gsm_state);
	break;

	/////////// timer MSG ///////////////////////////
	case EVENT_WG_GSM_POWER_SET_TIMEOUT:
	break;

	case EVENT_WG_GSM_HEARTBEAT_TIMEOUT:
		wg_gsm_heartbeat_timeout(gsm_state);
	break;

	/////////// resp messages ///////////////////////////
	case EVENT_WG_GSM_PARA_SET_RSP:
		wg_gsm_para_set_rsp_proc(gsm_state, buf, len);
		wg_dev_net_map_uart(gsm_state->sock_of_uart, gsm_state->uart_ip_addr);
	break;

	case EVENT_WG_GSM_ALARM_SET_RSP:
		wg_gsm_alarm_set_rsp_proc(gsm_state, buf, len);
	break;

	case EVENT_WG_GSM_WHITELIST_RSP:
		wg_gsm_whitelist_set_rsp_proc(gsm_state, buf, len);
	break;
	
	case EVENT_WG_GSM_SMS_SEND_RSP:
		wg_gsm_sms_send_rsp_proc(gsm_state, buf, len);
	break;

	case EVENT_WG_GSM_POWER_SET_RSP:
		wg_gsm_power_set_rsp_proc(gsm_state, buf, len);
	break;

	case EVENT_WG_GSM_RF_ON_RSP:
		wg_gsm_rf_on_rsp_proc(gsm_state, buf, len);
	break;

	case EVENT_WG_GSM_RF_OFF_RSP:
		wg_gsm_rf_off_rsp_proc(gsm_state, buf, len);
		gsm_state->inner_amp_flag = 0;
		wg_gsm_inner_amp_set_req(gsm_state, NULL, 0);
	break;

	case EVENT_WG_GSM_LBS_START_RSP:
		wg_gsm_lbs_start_rsp_proc(gsm_state, buf, len);
	break;

	case EVENT_WG_GSM_LBS_STOP_RSP:
		wg_gsm_lbs_stop_rsp_proc(gsm_state, buf, len);
	break;

	case EVENT_WG_GSM_RING_RSP:
		wg_gsm_ring_rsp_proc(gsm_state, buf, len);
	break;

	case EVENT_WG_GSM_MOBILE_AUTH_RSP:
	break;

	case EVENT_WG_GSM_MOBILE_POWER_SET_RSP:
		wg_gsm_mobile_power_set_rsp_proc(gsm_state, buf, len);
	break;

	case EVENT_WG_GSM_LBS_RING_RSP:
		wg_gsm_ring_rsp_proc(gsm_state, buf, len);
	break;

	case EVENT_WG_GSM_TIME_SET_RSP:
		wg_gsm_time_set_rsp_proc(gsm_state, buf, len);
	break;

	case EVENT_WG_GSM_MOBILE_IMSI_REPORT:
		wg_gsm_imsi_report_proc(gsm_state, buf, len);
	break;

	case EVENT_WG_GSM_SMS_REPORT:
	break;

	case EVENT_WG_GSM_CALL_REPORT:
	break;

	case EVENT_WG_GSM_MOBILE_SIGNAL_REPORT:
		wg_gsm_mobile_signal_report_proc(gsm_state, buf, len);
	break;

	/////////// dev board resp MSG ///////////////////////////
	case 	EVENT_WG_DEV_CAP_RSP:
	break;

	case 	EVENT_WG_DEV_RESET_RSP:
		wg_dev_reset_rsp_proc(buf, len);
	break;

	case 	EVENT_WG_DEV_IP_SET_RSP:
	break;

	default:
	break;
	}
}
/******************************************************************************/
gsm_machine_t g_gsm_state_machine[] = {
	{BOARD_STATUS_IDLE, gsm_sm_idle},
	{BOARD_STATUS_IMSI_CACTHER, gsm_sm_running},
	{BOARD_STATUS_LOCATION, gsm_sm_running},
	{BOARD_STATUS_AUTH, gsm_sm_running},
	{BOARD_STATUS_HW_ERROR, gsm_sm_idle},
	{BOARD_STATUS_SW_ERROR, gsm_sm_idle},
	{-1, NULL}
};
/******************************************************************************/
void gsm_state_machine(gsm_state_t *gsm_state, uint8_t *buf, uint32_t len)
{
	wg_msg_head_t *ptMsg    = (wg_msg_head_t *)buf;
	uint16_t  msg_type = 	wg_msg_type_get(buf);

	if((msg_type != 0x1) && (msg_type != 0x77))
	{
		printf("local state=%d remote state=%d event=%d len=%d\n", gsm_state->state, gsm_state->remote_state, msg_type, len);
	}

	if(gsm_state->state > BOARD_STATUS_HW_ERROR)
	{
		return;
	}

	g_gsm_state_machine[gsm_state->state].func(gsm_state, buf, len);
}
/******************************************************************************/
int gsm_timeout_event(uint32_t event)
{
	if(    (event <= EVENT_WG_GSM_TIMEOUT_END)
		&& (event >= EVENT_WG_GSM_TIMEOUT_START))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/******************************************************************************/
oam_msg_head_t  gsm_oam_msg;
void gsm_msg_proc(int8_t *buf, int32_t len)
{
	oss_msg_head_t *msg_head = (oss_msg_head_t *)buf;
	msg_timeout_t  *msg_timeout;

	if(gsm_timeout_event(msg_head->msg_type))
	{
		memset(&gsm_oam_msg, 0, sizeof(gsm_oam_msg));
		msg_timeout = (msg_timeout_t *)(msg_head + 1);
		gsm_oam_msg.msgType = msg_head->msg_type;
//		msg_head->msg_type = ntohs(msg_head->msg_type);
		gsm_state_machine((gsm_state_t *)msg_timeout->dwData, (uint8_t *)&gsm_oam_msg, len);
	}
	else
	{

	}
}

/******************************************************************************/
void gsm_sm_start(gsm_state_t *gsm_state)
{
    gsm_state_set(gsm_state, BOARD_STATUS_IDLE);
	wg_dev_cap_req(gsm_state->peer_ip_addr, gsm_state->peer_port);
	gsm_state->timer_id = oss_set_timer_ex(TIME_WG_GSM_CAP_GET, gsm_msg_proc, EVENT_WG_DEV_CAP_GET_TIMEOUT, (uint32_t)gsm_state);
}

