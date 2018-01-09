#include "oss.h"
#include "protocol_common.h"
#include "modem.h"
#include "wg_common.h"
#include "wcdma_cfg.h"
#include "wcdma_sm.h"
#include "wcdma_cell_sel.h"

////////////////////////////////////////////////////////////////////////
void wcdma_msg_proc(int8_t *buf, int32_t len);
/////////////////////////////////////////////////////////////////////////
void wcdma_state_set(wcdma_state_t *wcdma_state, uint32_t state)
{
	wcdma_state->state = state;
}

////////////////////////////////////////////////////////////////////////
void wg_wcdma_para_set_req(wcdma_state_t *state)
{
	wg_msg_send(state->peer_ip_addr, state->peer_port, state->channel, state->trx, EVENT_WG_WCDMA_PARA_SET_REQ, NULL, 0);

}
////////////////////////////////////////////////////////////////////////
void wg_wcdma_power_set_req(wcdma_state_t *state)
{
	wg_msg_send(state->peer_ip_addr, state->peer_port, state->channel, state->trx, EVENT_WG_WCDMA_POWER_SET_REQ, NULL, 0);
}
////////////////////////////////////////////////////////////////////////
void wg_wcdma_rf_on_req(wcdma_state_t *state)
{
	wg_msg_send(state->peer_ip_addr, state->peer_port, state->channel, state->trx, EVENT_WG_WCDMA_RF_ON_REQ, NULL, 0);

}
////////////////////////////////////////////////////////////////////////
void wg_wcdma_target_to_gsm_req(wcdma_state_t *state)
{
	wg_msg_send(state->peer_ip_addr, state->peer_port, state->channel, state->trx, EVENT_WG_WCDMA_TARGET_TO_GSM_REQ, NULL, 0);

}
////////////////////////////////////////////////////////////////////////
void wg_wcdma_to_gsm_req(wcdma_state_t *state)
{
	wg_msg_send(state->peer_ip_addr, state->peer_port, state->channel, state->trx, EVENT_WG_WCDMA_TO_GSM_REQ, NULL, 0);

}
////////////////////////////////////////////////////////////////////////
void wg_wcdma_lbs_start_req(wcdma_state_t *state)
{
	wg_msg_send(state->peer_ip_addr, state->peer_port, state->channel, state->trx, EVENT_WG_WCDMA_LBS_START_REQ, NULL, 0);

}
////////////////////////////////////////////////////////////////////////
void wg_wcdma_lbs_stop_req(wcdma_state_t *state)
{
	wg_msg_send(state->peer_ip_addr, state->peer_port, state->channel, state->trx, EVENT_WG_WCDMA_LBS_STOP_REQ, NULL, 0);

}

/////////////////////////////////////////////////////////////////////////
void wcdma_heartbeat_req(wcdma_state_t *wcdma_state)
{
	// setup time out proc
	oss_set_timer_ex(O_WCDMA_HEARTBEAT_TIMER, wcdma_msg_proc, EVENT_WCDMA_HEARTBEAT_TIMEOUT, (uint32_t)wcdma_state);

	wcdma_state->heartbeat_lost++;

	if(wcdma_state->heartbeat_lost > MAX_HEARTBEAT_LOST_NUM)
	{
		wcdma_state->state = WCDMA_SYS_STAT_DISCONN;
	}
}
//////////////////////////////////////////////////////////////////////
void wcdma_heartbeat_rsp(wcdma_state_t *wcdma_state, wcdma_heartbeat_rsp_t *ptHeartBeatRsp)
{
	wcdma_state->heartbeat_lost = 0;
}

//////////////////////////////////////////////////////////////////////
void wcdma_cfg_set_rsp(wcdma_state_t *wcdma_state, wcdma_set_cfg_rsp_t *ptCfgRsp)
{
	unsigned char u8CfgInd = ptCfgRsp->u8CfgInd;

	return;
}

/////////////////////////////////////////////////////////////////////////
void wcdma_power_set(wcdma_state_t *wcdma_state, uint16_t power)
{
	oam_wcdma_set_power_req_t tReq;

	
	oss_set_timer_ex(O_WCDMA_POWER_SET_TIMER, wcdma_msg_proc, EVENT_WCDMA_POWER_SET_TIMEOUT, (uint32_t)wcdma_state);
	
}
/////////////////////////////////////////////////////////////////////////
void wcdma_power_rsp(wcdma_state_t *wcdma_state, wcdma_set_power_rsp_t *ptPowerRsp)
{
	unsigned char u8PowerInd = ptPowerRsp->u8PowerInd;

	return;
}

/////////////////////////////////////////////////////////////////////////
void oam_wcdma_start_req(wcdma_state_t *wcdma_state, 	wcdma_start_req_t	*ptStartReq)
{

}

/////////////////////////////////////////////////////////////////////////
void oam_wcdma_reset_req(wcdma_state_t *wcdma_state, 	oam_wcdma_reset_req_t	*ptResetReq)
{

}
/////////////////////////////////////////////////////////////////////////
void oam_wcdma_set_power_req(wcdma_state_t *wcdma_state, 	oam_wcdma_set_power_req_t	*ptPowerSetReq)
{
	wcdma_state->wcdma_cfg.strPbsPara.u8pwr = (uint8_t)ptPowerSetReq->u16Power;
}
/******************************************************************************/
void wcdma_sm_idle(wcdma_state_t *wcdma_state, char *buf, int len)
{
	int ret = wcdma_cell_select(wcdma_state);

	if(ret != RET_OK)
	{
		// wait and get cell in next time
		oss_set_timer_ex(O_WCDMA_CELL_GET_TIMER, wcdma_msg_proc, EVENT_WCDMA_CELL_GET_TIMEOUT, (uint32_t)wcdma_state);
		return;
	}

	// go to next status and send a heart beat to lte board
    wcdma_state_set(wcdma_state, WCDMA_SYS_STAT_DISCONN);
	wcdma_heartbeat_req(wcdma_state);
}
/******************************************************************************/
void wcdma_sm_disconn(wcdma_state_t *wcdma_state, char *buf, int len)
{
	wg_msg_head_t *ptMsg    = (wg_msg_head_t *)buf;
	wcdma_heartbeat_rsp_t		*ptHeartBeatRsp;

	switch(ntohs(ptMsg->msgType))
	{
	case EVENT_WCDMA_HEARBEAT_RSP:
		ptHeartBeatRsp = (wcdma_heartbeat_rsp_t   *)(ptMsg + 1);
		wcdma_heartbeat_rsp(wcdma_state, ptHeartBeatRsp);
		wcdma_state_set(wcdma_state, WCDMA_SYS_STAT_START_CFG);
		break;
	
	case EVENT_WCDMA_HEARTBEAT_TIMEOUT:
		wcdma_heartbeat_req(wcdma_state);
		break;
	
	default:
		break;
	}
}
/******************************************************************************/
void wcdma_sm_start_cfg(wcdma_state_t *wcdma_state, char *buf, int len)
{
	wg_msg_head_t *ptMsg    = (wg_msg_head_t *)buf;
	wcdma_heartbeat_rsp_t		*ptHeartBeatRsp;
	wcdma_load_cfg_req_t *ptCfgLoadReq;

	switch(ntohs(ptMsg->msgType))
	{
	case EVENT_WCDMA_HEARBEAT_RSP:
		ptHeartBeatRsp = (wcdma_heartbeat_rsp_t   *)(ptMsg + 1);
		wcdma_heartbeat_rsp(wcdma_state, ptHeartBeatRsp);
		break;
	
	case EVENT_WCDMA_HEARTBEAT_TIMEOUT:
		wcdma_heartbeat_req(wcdma_state);
		break;

	case EVENT_WCDMA_LOAD_CONFIG_REQ:
		ptCfgLoadReq = (wcdma_load_cfg_req_t   *)(ptMsg + 1);
		wcdma_state_set(wcdma_state, WCDMA_SYS_STAT_DATAREADY);
		break;
		
	default:
		break;
	}
}
/******************************************************************************/
void wcdma_sm_dataready(wcdma_state_t *wcdma_state, char *buf, int len)
{
	wg_msg_head_t *ptMsg    = (wg_msg_head_t *)buf;
	wcdma_heartbeat_rsp_t	*ptHeartBeatRsp;

	switch(ntohs(ptMsg->msgType))
	{
	case EVENT_WCDMA_HEARBEAT_RSP:
		ptHeartBeatRsp = (wcdma_heartbeat_rsp_t   *)(ptMsg + 1);
		wcdma_heartbeat_rsp(wcdma_state, ptHeartBeatRsp);
		break;
	
	case EVENT_WCDMA_HEARTBEAT_TIMEOUT:
		wcdma_heartbeat_req(wcdma_state);
		break;


	default:
		break;
	}
}
/******************************************************************************/
void wcdma_sm_running(wcdma_state_t *wcdma_state, char *buf, int len)
{
	wg_msg_head_t *ptMsg    = (wg_msg_head_t *)buf;
	wcdma_heartbeat_rsp_t  *ptHeartBeatRsp;

	switch(ntohs(ptMsg->msgType))
	{
	case EVENT_WCDMA_HEARBEAT_RSP:
		ptHeartBeatRsp = (wcdma_heartbeat_rsp_t   *)(ptMsg + 1);
		wcdma_heartbeat_rsp(wcdma_state, ptHeartBeatRsp);
		break;
	
	case EVENT_WCDMA_HEARTBEAT_TIMEOUT:
		wcdma_heartbeat_req(wcdma_state);
		break;


	case EVENT_WCDMA_POWER_SET_TIMEOUT:

	break;


	case 	EVENT_WG_WCDMA_PARA_SET_RSP:
	case 	EVENT_WG_WCDMA_POWER_SET_RSP:
	case 	EVENT_WG_WCDMA_RF_ON_RSP:
	case 	EVENT_WG_WCDMA_TARGET_TO_GSM_RSP:
	case 	EVENT_WG_WCDMA_TO_GSM_RSP:
	case 	EVENT_WG_WCDMA_LBS_START_RSP:
	case 	EVENT_WG_WCDMA_LBS_STOP_RSP:
	case 	EVENT_WG_WCDMA_MOBILE_TARGET_REPORT:
	case 	EVENT_WG_WCDMA_MOBILE_IMSI_REPORT:
	case 	EVENT_WG_WCDMA_HEARTBEAT:
	break;

	default:
	break;
	}
}
/******************************************************************************/
typedef struct Tag_WCDMA_machine {
	uint32_t  state;
	void (*func)(wcdma_state_t *wcdma_state, char *buf, int len);
}wcdma_machine_t;

wcdma_machine_t g_wcdma_state_machine[] = {
	{BOARD_STATUS_IDLE, wcdma_sm_idle},
	{BOARD_STATUS_IMSI_CACTHER, wcdma_sm_running},
	{BOARD_STATUS_LOCATION, wcdma_sm_running},
	{BOARD_STATUS_AUTH, wcdma_sm_running},
	{BOARD_STATUS_HW_ERROR, wcdma_sm_idle},
	{BOARD_STATUS_SW_ERROR, wcdma_sm_idle},
	{-1, NULL}
};
/******************************************************************************/
void wcdma_state_machine(wcdma_state_t *wcdma_state, char *buf, int len)
{
	wg_msg_head_t *ptMsg    = (wg_msg_head_t *)buf;
	uint16_t  msg_type = 	wg_msg_type_get(buf);

	printf("local state=%d remote state=%d event=%d, msg len=%d \n", wcdma_state->state, wcdma_state->remote_state, msg_type, len);

	if(wcdma_state->state > BOARD_STATUS_HW_ERROR)
	{
		return;
	}

	g_wcdma_state_machine[wcdma_state->state].func(wcdma_state, buf, len);
}
/******************************************************************************/
int wcdma_timeout_event(uint32_t event)
{
	if(    (event <= EVENT_WCDMA_TIMEOUT_END)
		&& (event >= EVENT_WCDMA_TIMEOUT_START))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/******************************************************************************/
void wcdma_msg_proc(int8_t *buf, int32_t len)
{
	oss_msg_head_t *msg_head = (oss_msg_head_t *)buf;
	msg_timeout_t  *msg_timeout;

	if(wcdma_timeout_event(msg_head->msg_type))
	{
		msg_timeout = (msg_timeout_t *)(msg_head + 1);
		msg_head->msg_type = ntohs(msg_head->msg_type);
		wcdma_state_machine((wcdma_state_t *)msg_timeout->dwData, buf, len);
	}
	else
	{

	}
}

/******************************************************************************/
void wcdma_sm_start(wcdma_state_t *wcdma_state)
{
    wcdma_state_set(wcdma_state, WCDMA_SYS_STAT_CELL_GET);
	oss_set_timer_ex(O_WCDMA_CELL_GET_TIMER, wcdma_msg_proc, EVENT_WCDMA_CELL_GET_TIMEOUT, (uint32_t)wcdma_state);
}

