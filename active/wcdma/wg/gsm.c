#include "oss.h"
#include "protocol_common.h"
#include "modem.h"
#include "wg_common.h"
#include "gsm_cfg.h"
#include "gsm_sm.h"
#include "gsm.h"
#include "gsm_oam.h"

/******************************************************************************/
#define  RUN_FIRST_BTS  1
#define  RUN_SECOND_BTS 2
#define  RUN_TWO_BTS    3

uint32_t bts_mode = RUN_TWO_BTS;
uint8_t  power = 4;
uint16_t mobile_power = 4;
uint8_t  rf_flag = 0;
uint8_t  lbs_flag = 1;
uint8_t  *imsi = "460004730699325";
uint8_t  *caller_phone = "13564769776";
uint8_t  whitelist_imsi[MAX_IMSI_LEN] = {4,6,0,0,0,4,7,3,0,6,9,9,3,2,5};
uint8_t *sms = "it is a sms test";
uint16_t arfcn = 19;
uint8_t amp_flag = 1;
/******************************************************************************/
typedef struct tag_event {
	uint8_t channel;
	uint8_t trx;
	uint32_t event;
}event_t;

event_t  event_first_bts[30] = {
	{0, 0, 0},
	{0, 0, 0},
	{0, 0, 0},

	{1, 1, EVENT_OAM_GSM_PARA_SET_REQ},
	{0, 0, 0},
	{1, 1, EVENT_OAM_GSM_ALARM_SET_REQ},
	{0, 0, 0},
	{1, 1, EVENT_OAM_GSM_SET_POWER_REQ},
	{0, 0, 0},
	{1, 1, EVENT_OAM_GSM_RF_SET_REQ},
	{0, 0, 0},
	{1, 1, EVENT_OAM_INNER_AMP_SET_REQ},
	{0, 0, 0},

	{0, 0, 0xffffffff},
};

event_t  event_second_bts[30] = {
	{0, 0, 0},
	{0, 0, 0},
	{0, 0, 0},

	{1, 2, EVENT_OAM_GSM_PARA_SET_REQ},
	{0, 0, 0},
	{1, 2, EVENT_OAM_GSM_ALARM_SET_REQ},
	{0, 0, 0},
	{1, 2, EVENT_OAM_GSM_SET_POWER_REQ},
	{0, 0, 0},
	{1, 2, EVENT_OAM_GSM_RF_SET_REQ},
	{0, 0, 0},
	{1, 1, EVENT_OAM_INNER_AMP_SET_REQ},

	{0, 0, 0xffffffff},
};
/******************************************************************************/
event_t  event_two_bts[30] = {
	{0, 0, 0},
	{0, 0, 0},
	{0, 0, 0},

	{1, 1, EVENT_OAM_GSM_PARA_SET_REQ},
	{0, 0, 0},
	{1, 1, EVENT_OAM_GSM_ALARM_SET_REQ},
	{0, 0, 0},
	{1, 1, EVENT_OAM_GSM_SET_POWER_REQ},
	{0, 0, 0},
	{1, 1, EVENT_OAM_GSM_RF_SET_REQ},
	{0, 0, 0},

	{1, 1, EVENT_OAM_GSM_CFG_CHANGE},
	{0, 0, 0},

	{1, 2, EVENT_OAM_GSM_PARA_SET_REQ},
	{0, 0, 0},
	{1, 2, EVENT_OAM_GSM_ALARM_SET_REQ},
	{0, 0, 0},
	{1, 2, EVENT_OAM_GSM_SET_POWER_REQ},
	{0, 0, 0},
	{1, 2, EVENT_OAM_GSM_RF_SET_REQ},
	{0, 0, 0},
	{1, 1, EVENT_OAM_INNER_AMP_SET_REQ},

	{0, 0, 0xffffffff},
};
/******************************************************************************/
gsm_state_t g_gsm_state;
void  gsm_ui_event(gsm_state_t *gsm_state, uint32_t event);
void  gsm_run_event(event_t *event);
/******************************************************************************/
int gsm_init()
{
	int ret;

	ret = NetConnectInit(GSM_HOST_UDP_PORT);
	if(ret < 0)
	{
		printf("[gsm_init]NetConnectInit failed \n");
		return -1;
	}

	memset(&g_gsm_state, 0, sizeof(gsm_state_t));  
	ret = gsm_cfg_init(&g_gsm_state, 1, 1, ntohl(NetConnectAton("192.168.1.119")), GSM_BOARD_UDP_PORT, ntohl(NetConnectAton("192.168.1.254")));
	if(ret < 0)
	{
		printf("[gsm_init]gsm_cfg_init failed \n");
		return -2;
	}

	g_gsm_state.sock_of_uart = NetConnectUdpInit(0, UART_UDP_PORT);
	if(g_gsm_state.sock_of_uart < 0)
	{
		printf("[gsm_init]uart udp port failed\n");
		return -3;
	}

	return 1;
}
/******************************************************************************/
int gsm_net_process(gsm_state_t *gsm_state)
{
	int type, len;
	int sock;
	int peerIp, loop = 3;
	short peerPort;

	NetConnectSleep(10);

	while(loop > 0)
	{
		loop--;
		//recv msg from lte board  
		len = 0;
		sock = NetRecvMsg(abyRecvMsgBuf, MAX_MSG_LEN, &len, &type, &peerIp, &peerPort);
		if(sock == -1)
		{
			return 0;
		}
		
		if(len > 0)
		{
			gsm_state_machine(gsm_state, abyRecvMsgBuf, len);
		}
	}

	return 1;
}

void gsm_oam_bts_cfg_change()
{
	arfcn = 14;
}
/******************************************************************************/
void gsm_run()
{
	switch(bts_mode)
	{
	case RUN_FIRST_BTS:
		g_gsm_state.channel = 1;
		g_gsm_state.trx     = 1;
		g_gsm_state.trx_second  = 0;
	break;
		
	case RUN_SECOND_BTS:
		g_gsm_state.channel = 1;
		g_gsm_state.trx     = 0;
		g_gsm_state.trx_second  = 2;
	break;

	case RUN_TWO_BTS:
		g_gsm_state.channel = 1;
		g_gsm_state.trx     = 1;
		g_gsm_state.trx_second  = 2;
	break;

	default:
	break;
	}

	gsm_sm_start(&g_gsm_state);

	while(1)
	{
		gsm_net_process(&g_gsm_state);

		switch(bts_mode)
		{
		case RUN_FIRST_BTS:
			gsm_run_event(event_first_bts);
		break;
			
		case RUN_SECOND_BTS:
			gsm_run_event(event_second_bts);
		break;

		case RUN_TWO_BTS:
			gsm_run_event(event_two_bts);
		break;

		default:
		break;
		}

		oss_timer_scan();
	}
}
/******************************************************************************/
uint32_t event_index = 0;
void gsm_run_event(event_t *event)
{
	if(g_gsm_state.state == GSM_SYS_STAT_RUNNING)
	{
		event += event_index;
		if(0xffffffff == event->event)
		{
			return;
		}
		event_index++;
		g_gsm_state.channel = event->channel;
		g_gsm_state.trx     = event->trx;
		gsm_ui_event(&g_gsm_state, event->event);
	}
}

/******************************************************************************/
void gsm_ui_event(gsm_state_t *gsm_state, uint32_t event)
{
	if(event == 0)
	{
		oss_delay(50);
		return;
	}

	switch(event)
	{
	case EVENT_OAM_GSM_RESET_REQ:
		gsm_oam_reset(gsm_state);
	break;
 
	case EVENT_OAM_GSM_SET_POWER_REQ:
		gsm_oam_power_set(gsm_state, power);
	break;
 
	case EVENT_OAM_GSM_ALARM_SET_REQ:
		gsm_oam_alarm_set(gsm_state, imsi);
	break;

	case EVENT_OAM_GSM_WHITELIST_SET_REQ:
		gsm_oam_whitelist_set(gsm_state, whitelist_imsi);
	break;

	case EVENT_OAM_GSM_SMS_SEND_REQ:
		gsm_oam_sms_send(gsm_state, imsi, caller_phone, sms);
	break;

	case EVENT_OAM_GSM_RF_SET_REQ:
		gsm_oam_rf_set(gsm_state, rf_flag);
	break;

	case EVENT_OAM_INNER_AMP_SET_REQ:
		gsm_oam_inner_amp_set(gsm_state, amp_flag);
	break;

	case EVENT_OAM_GSM_LBS_SET_REQ:
		gsm_oam_lbs_set(gsm_state, lbs_flag);
	break;

	case EVENT_OAM_GSM_MOBILE_POWER_SET_REQ:
		gsm_oam_mobile_power_set(gsm_state, mobile_power);
	break;

	case EVENT_OAM_GSM_CFG_CHANGE:
		gsm_oam_bts_cfg_change();
	break;

	case EVENT_OAM_GSM_PARA_SET_REQ:
		gsm_oam_para_set(gsm_state, arfcn);
	break;

	case EVENT_OAM_GSM_TIME_SET_REQ:
		gsm_oam_time_set(gsm_state);
	break;

	default:
	break;
	}
}

