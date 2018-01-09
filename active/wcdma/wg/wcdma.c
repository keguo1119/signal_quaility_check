#include "oss.h"
#include "protocol_common.h"
#include "modem.h"
#include "wg_common.h"
#include "wcdma_cfg.h"
#include "wcdma_sm.h"
#include "wcdma_cell_sel.h"
#include "wcdma.h"
#include "wcdma_oam.h"

wcdma_state_t g_wcdma_state;
void  wcdma_ui_event(wcdma_state_t *wcdma_state);
/******************************************************************************/
int wcdma_init()
{
	int ret;

	ret = NetConnectInit(WCDMA_HOST_UDP_PORT);
	if(ret < 0)
	{
		printf("[wcdma_init]NetConnectInit failed \n");
		return -1;
	}

	memset(&g_wcdma_state, 0, sizeof(wcdma_state_t));  
	ret = wcdma_cfg_init(&g_wcdma_state, 1, 1, ntohl(NetConnectAton("192.168.1.119")), WCDMA_BOARD_UDP_PORT);
	if(ret < 0)
	{
		printf("[wcdma_init]wcdma_cfg_init failed \n");
		return -2;
	}

	ret = wcdma_cell_select_init(&g_wcdma_state.tModem);
	if(ret < 0)
	{
		printf("[wcdma_init]wcdma_cell_select_init failed \n");
		return -3;
	}

	return 1;
}
/******************************************************************************/
int wcdma_net_process(wcdma_state_t *wcdma_state)
{
	int type, len;
	int sock;
	int peerIp;
	short peerPort;

	NetConnectSleep(100);
	
	//recv msg from lte board  
	len = 0;
	sock = NetRecvMsg(abyRecvMsgBuf, MAX_MSG_LEN, &len, &type, &peerIp, &peerPort);
	if(sock == -1)
	{
		return 0;
	}
	
	if(len > 0)
	{
		wcdma_state_machine(wcdma_state, abyRecvMsgBuf, len);
	}

	return 1;
}

/******************************************************************************/
void wcdma_run()
{
	int reset = 0;
	int set_power = 1;
	uint16_t power = 0;

	wcdma_sm_start(&g_wcdma_state);

	while(1)
	{
		wcdma_net_process(&g_wcdma_state);

		if(g_wcdma_state.state == WCDMA_SYS_STAT_RUNNING)
		{
			wcdma_ui_event(&g_wcdma_state);
		}

		oss_timer_scan();
	}
}
/******************************************************************************/
static uint32_t event = 0;
static uint16_t power = 0;
static uint8_t  rf_flag = 0;
static uint8_t  lbs_flag = 1;
static uint8_t  *imsi = "46001";

void wcdma_ui_event(wcdma_state_t *wcdma_state)
{
	switch(event)
	{
	case EVENT_OAM_WCDMA_RESET_REQ:
		wcdma_oam_reset(wcdma_state);
	break;
 
	case EVENT_OAM_WCDMA_SET_POWER_REQ:
		wcdma_oam_power_set(wcdma_state, power);
	break;
 
	case EVENT_OAM_WCDMA_RF_SET_REQ:
		wcdma_oam_rf_set(wcdma_state, rf_flag);
	break;

	case EVENT_OAM_WCDMA_LBS_SET_REQ:
		wcdma_oam_lbs_set(wcdma_state, lbs_flag);
	break;

	case EVENT_OAM_WCDMA_PARA_SET_REQ:
		wcdma_oam_para_set(wcdma_state);
	break;

	default:
	break;
	}
}
