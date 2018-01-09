#include "oss.h"
#include "protocol_common.h"
#include "modem.h"
#include "wg_common.h"
#include "gsm_cfg.h"
#include "gsm_sm.h"
#include "gsm.h"

unsigned char abyRecvMsgBuf[MAX_MSG_LEN + 1];
unsigned char g_send_msg_buf[MAX_MSG_LEN + 1];

uint32_t g_last_map_tick = 0;
/////////////////////////////////////////////////////////////////////////
void wg_msg_head_init(wg_msg_head_t *ptHead, uint8_t channel, uint8_t trx, uint8_t msgType)
{
	ptHead->channel = channel;
	ptHead->trx     = trx;
	ptHead->msgType = msgType;
	ptHead->msgLen  = 0;
}
/////////////////////////////////////////////////////////////////////////
void wg_msg_len_set(wg_msg_head_t *ptHead, uint16_t len)
{
	ptHead->msgLen  = htons(len);
}
/////////////////////////////////////////////////////////////////////////
int wg_msg_content_set(wg_msg_head_t *ptHead, uint8_t *content, uint16_t len)
{
	memcpy((uint8_t *)(ptHead+1), content, len);
	ptHead->msgLen  = htons(len);

	return (len + sizeof(wg_msg_head_t));
}
/////////////////////////////////////////////////////////////////////////
uint16_t  wg_msg_type_get(uint8_t *msg)
{
	wg_msg_head_t *head;

/*	if(IS_OSS_MSG(*msg))
	{
		return (uint16_t )(((*msg)<<8) + (*(msg+1)));
	}
*/
	head = (wg_msg_head_t *)msg;
/*
	if(IS_WCDMA_MSG(head->msgType))
	{
		return (uint16_t )head->msgType;
	}

	if(IS_GSM_MSG(head->msgType))
	{
		return (uint16_t )head->msgType;
	}

	return WG_MSG_TYPE_INVALID;
*/	
	return (uint16_t )head->msgType;
}
/////////////////////////////////////////////////////////////////////////
void  wg_msg_one_param_decode(uint8_t *in, uint8_t *name, uint8_t *out)
{
	uint8_t *ptr;

	ptr = strstr(in, name);
	if(!ptr)
	{
		*out = 0;
		return ;
	}

	while((*ptr != '@') && (*ptr != 0))
	{
		*out = *ptr;
		out++;
		ptr++;
	}

	*out = 0;
}
/////////////////////////////////////////////////////////////////////////
void  wg_msg_one_param_encode(uint8_t *out, uint8_t *name, uint32_t value, uint8_t last)
{
	uint8_t  local_buf[64];

	memset(local_buf, 0, 64);

	sprintf(local_buf, "%s#%d@", name, value);

	strcat(out, local_buf);
}
/////////////////////////////////////////////////////////////////////////
wg_msg_common_rsp_t *wg_msg_common_rsp_decode(uint8_t *buf)
{
	wg_msg_common_rsp_t *rsp = (wg_msg_common_rsp_t *)buf;	

	rsp->msg_head.msgLen = ntohs(rsp->msg_head.msgLen);

	printf("[rsp] channel=%d trx=%d msg_type=%d ret=%d \n", 
			rsp->msg_head.channel, rsp->msg_head.trx, rsp->msg_head.msgType, rsp->ret);

	return rsp;
}
/////////////////////////////////////////////////////////////////////////
int wg_msg_send(uint32_t peer_ip_addr, uint16_t peer_port, uint8_t channel, uint8_t trx, uint8_t msgType, uint8_t *content, uint32_t content_size)
{
	int len;
	wg_msg_head_t *head = (wg_msg_head_t *)g_send_msg_buf;

	printf("[send req]msg_type =%d \n", msgType);

	wg_msg_head_init((wg_msg_head_t *)g_send_msg_buf, channel, trx, msgType);

	if((content == NULL) || (content_size == 0))
	{
		head->msgLen = 0;
		len = sizeof(wg_msg_head_t);
	}
	else
	{
		len = wg_msg_content_set((wg_msg_head_t *)g_send_msg_buf, content, content_size);
	}

	len = NetConnectUdpSend(peer_ip_addr, peer_port, g_send_msg_buf, len);

	return len;
}
///////////////////////////////////////////////////////////////////
void wg_dev_net_map_uart(uint32_t sock, uint32_t peer_ip_addr)
{
	struct sockaddr_in addr;
	int ret, len =12;
	uint32_t cur_tick = oss_current_tick();
	uint8_t buf[64] = {0x55, 0xaa, 0xc, 0x6, 0x0, 0x5c, 0x32, 0x30, 0x31, 0x36, 0x0, 0x37};

	if((cur_tick - g_last_map_tick) < 2000) // 8000ms
	{
		return;
	}
	g_last_map_tick = cur_tick;

	memset(&addr, 0, sizeof(struct sockaddr_in));
	addr.sin_family      = AF_INET;
	addr.sin_addr.s_addr = htonl(peer_ip_addr);
	addr.sin_port        = htons(UART_UDP_PORT);

	ret = sendto(sock, buf, len, 0,(struct sockaddr *)&addr, sizeof(struct sockaddr_in));

	return;
}
///////////////////////////////////////////////////////////////////
void wg_dev_cap_req(uint32_t peer_ip_addr, uint16_t peer_port)
{
	int len;
	wg_msg_dev_cap_req_t req;

	memset(&req, 0, sizeof(req));

	req.msgType = EVENT_WG_DEV_CAP_REQ;
	req.msgLen  = 0;

	len = NetConnectUdpSend(peer_ip_addr, peer_port, &req, sizeof(req));

	return;
}
///////////////////////////////////////////////////////////////////
void wg_dev_cap_rsp_proc(uint8_t *buf, uint32_t len)
{

}
///////////////////////////////////////////////////////////////////
int wg_dev_reset(uint32_t peer_ip_addr, uint16_t peer_port)
{
	int len;
	wg_msg_dev_reset_req_t req;

	memset(&req, 0, sizeof(req));

	req.msgType = EVENT_WG_DEV_RESET_REQ;
	req.msgLen  = 0;

	len = NetConnectUdpSend(peer_ip_addr, peer_port, (char *)&req, sizeof(wg_msg_dev_reset_req_t));

	// setup timer

	return len;
}
///////////////////////////////////////////////////////////////////
void wg_dev_reset_rsp_proc(uint8_t *buf, uint32_t len)
{

}
///////////////////////////////////////////////////////////////////
void wg_dev_ip_set(uint32_t ip, uint16_t port, uint32_t new_ip)
{
	wg_msg_dev_ip_set_req_t req;

	req.msgType = EVENT_WG_DEV_IP_SET_REQ;
	req.msgLen  = 0;
}
///////////////////////////////////////////////////////////////////
void wg_dev_ip_set_rsp_proc(uint8_t *buf, uint32_t len)
{

}

