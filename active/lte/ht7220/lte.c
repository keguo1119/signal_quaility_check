#include "oss.h"
#include "protocol_common.h"
#include "modem.h"
#include "lte_cfg.h"
#include "lte_sm.h"
#include "lte_cell_sel.h"
#include "lte.h"
#include "lte_oam.h"

#define  MAX_MSG_LEN  (4096)
unsigned char abyRecvMsgBuf[MAX_MSG_LEN + 1];

lte_state_t g_lte_state;
/******************************************************************************/
int lte_init()
{
	int ret;

	ret = NetConnectInit(LTE_HOST_UDP_PORT);
	if(ret < 0)
	{
		printf("[main]LteNetConnectInit failed \n");
		return -1;
	}

	memset(&g_lte_state, 0, sizeof(lte_state_t));  //xzluo
	ret = lte_cfg_init(&g_lte_state, 1, MOD_ID_FDD_CNU, 0, 0, ntohl(NetConnectAton("10.0.33.17")), LTE_BOARD_UDP_PORT);
	if(ret < 0)
	{
		printf("[main]lte_cfg_init failed \n");
		return -2;
	}

	ret = lte_cell_select_init(&g_lte_state.tModem);
	if(ret < 0)
	{
		printf("[serial port]lte_cfg_init failed \n");
		return -3;
	}

    lte_oam_init(&g_lte_state);
	
	return 1;
}
/******************************************************************************/
int lte_net_process(lte_state_t *lte_state)
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
		if (peerPort == LTE_APP_UDP_PORT) 
		{
			app_oam_msg_proc((lte_oam_t *)(lte_state->p_oam_addr), abyRecvMsgBuf, len);
		} 
		else 
		{
		    lte_state_machine(lte_state, abyRecvMsgBuf, len); 
		}
	}

	return 1;
}

/******************************************************************************/
void lte_run()
{
    //lte_sm_start(&g_lte_state);

	while(1)
	{
		lte_net_process(&g_lte_state); 

		oss_timer_scan();
	}
}