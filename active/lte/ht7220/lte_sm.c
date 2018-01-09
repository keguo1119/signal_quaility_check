#include "oss.h"
#include "oss_time.h"
#include "protocol_common.h"
#include "modem.h"
#include "lte_cfg.h"
#include "lte_sm.h"
#include "lte_cell_sel.h"
#include "lte_oam.h"

void lte_cfg_load_rsp(lte_state_t *lte_state, lte_load_cfg_rsp_t *ptCfgLoadRsp);
void lte_para_update_rsp(lte_state_t *lte_state, uint16_t u16UpdateInd);
void lte_reset_req(lte_state_t *lte_state);
void lte_power_set(lte_state_t *lte_state, uint16_t power);

////////////////////////////////////////////////////////////////////////
//void lte_msg_proc(char *buf, int len);
/////////////////////////////////////////////////////////////////////////
void lte_state_set(lte_state_t *lte_state, uint32_t state)
{
	lte_state->state = state;
}
/////////////////////////////////////////////////////////////////////////
void lte_msg_head_init(lte_state_t *lte_state, msg_head_t *ptHead, uint16_t msgType)
{
	ptHead->msgType = htons(msgType);
	
	ptHead->dstMod.bsId  = lte_state->dst_bsId;
	ptHead->dstMod.modId = lte_state->dst_modId;
	
	ptHead->srcMod.bsId  = lte_state->src_bsId;
	ptHead->srcMod.modId = lte_state->src_modId;

	ptHead->sessionId = htons(lte_state->session_id);

	ptHead->rsvd = 0;
}
/////////////////////////////////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////
int lte_send_msg(uint32_t peer_ip_addr, uint16_t peer_port, uint8_t *buf, uint32_t bufLen)
{
	int ret;
	msg_head_t * ptMsgH;
	
	ptMsgH = (msg_head_t *)buf;
	ret = NetConnectUdpSend(peer_ip_addr, peer_port, buf, bufLen);
    if (ret > 0) printf("Send Message:%04x \n", ntohs(ptMsgH->msgType));
	return ret;
}

/////////////////////////////////////////////////////////////////////////
void lte_heartbeat_req(lte_state_t *lte_state)
{
	lte_heartbeat_req_t tHeartbeat;

	lte_msg_head_init(lte_state, &tHeartbeat.msgHeader, EVENT_LTE_HEARBEAT_REQ);

	tHeartbeat.msgHeader.msgLen = htons(sizeof(tHeartbeat) - sizeof(msg_head_t));

	// setup time out proc
	oss_set_timer_ex(O_LTE_HEARTBEAT_TIMER, lte_msg_proc, EVENT_HEARTBEAT_TIMEOUT, (uint32_t)lte_state);

	lte_send_msg(lte_state->peer_ip_addr, lte_state->peer_port, (uint8_t *)&tHeartbeat, sizeof(tHeartbeat));

	lte_state->heartbeat_lost++;

	if(lte_state->heartbeat_lost > MAX_HEARTBEAT_LOST_NUM)
	{
		lte_state->state = LTE_SYS_STAT_DISCONN;
	}
}
//////////////////////////////////////////////////////////////////////
void lte_heartbeat_rsp(lte_state_t *lte_state, lte_heartbeat_rsp_t *ptHeartBeatRsp)
{
	lte_state->remote_state   = ntohs(ptHeartBeatRsp->u16SysMode);
	lte_state->heartbeat_lost = 0;
}

/////////////////////////////////////////////////////////////////////////
void lte_start_req(lte_state_t *lte_state, uint16_t start)
{
	lte_start_req_t tReq;

	lte_msg_head_init(lte_state, &tReq.msgHeader, EVENT_LTE_START_REQ);

	tReq.u16Start = htons(start);	
	tReq.msgHeader.msgLen = htons(sizeof(tReq) - sizeof(msg_head_t));
	
	lte_send_msg(lte_state->peer_ip_addr, lte_state->peer_port, (uint8_t *)&tReq, sizeof(tReq));

	oss_set_timer_ex(O_LTE_STARTUP_TIMER, lte_msg_proc, EVENT_STARTUP_TIMEOUT, (uint32_t)lte_state);
	lte_state->startreq_fail++;
	if (lte_state->startreq_fail > MAX_START_REQ_NUM) 
	{
		oam_lte_err_ind((lte_oam_t *)(lte_state->p_oam_addr), SEND_START_REQ_ERROR);
		lte_reset_req(lte_state);
	}
}

//////////////////////////////////////////////////////////////////////
void lte_start_rsp(lte_state_t *lte_state, lte_start_rsp_t *ptStartRsp)
{
	unsigned char u8StartInd = ptStartRsp->u8StartInd;

    oam_lte_start_rsp((lte_oam_t *)(lte_state->p_oam_addr), u8StartInd);
	
	if(u8StartInd == 0)
	{
		// send msg oam to indicate startup failure. 
		return;
	}

    lte_state->startreq_fail = 0;
    oss_del_timer_ex((uint32_t)EVENT_STARTUP_TIMEOUT); //xzluo

	if (!(lte_state->start_value)) 
	{ //stop
	    lte_state->state = LTE_SYS_STAT_DATAREADY; //state transfer
	    return ;
	}
	
	if(lte_state->work_mode == O_LTE_USER_CFG_WORKMODE)
	{
		lte_state->state = LTE_SYS_STAT_RUNNING;
	}
	else if(lte_state->work_mode == O_LTE_SELF_CFG_WORKMODE)
	{
		lte_state->state = LTE_SYS_STAT_SCAN;
	}

	return;
}

/////////////////////////////////////////////////////////////////////////
void lte_reset_req(lte_state_t *lte_state)
{
	lte_reset_req_t tReq;

	lte_msg_head_init(lte_state, &tReq.msgHeader, EVENT_LTE_RESET_REQ);

	tReq.msgHeader.msgLen = htons(sizeof(tReq) - sizeof(msg_head_t));

	lte_send_msg(lte_state->peer_ip_addr, lte_state->peer_port, (uint8_t *)&tReq, sizeof(tReq));
}
//////////////////////////////////////////////////////////////////////
void lte_reset_rsp(lte_state_t *lte_state, lte_reset_rsp_t *ptResetRsp)
{
	int ret = RET_FAILED;
	int cnt = 0;
	lte_oam_t *p_lte_oam = NULL;
	unsigned char u8ResetInd = ptResetRsp->u8ResetInd;
	
    //if (u8ResetInd) 
	{  //accept and start to reset
	    lte_state->state = LTE_SYS_STAT_DISCONN; 
	}
	
	//after LTE board reset, update lte_cfg.strPbsPara
	p_lte_oam = (lte_oam_t *)(lte_state->p_oam_addr);
	while (ret != RET_OK) 
	{
		//oam_set_eNB_para(p_lte_oam);
		//oss_delay(100); //100ms
        ret = lte_cell_select(lte_state); 
		if ((ret == RET_OK) && (lte_state->lte_cfg.strPbsPara.u8Band == p_lte_oam->band)) 
		{ //get cell's para succ
			break; 
		} 
		else 
		{
		    printf("[LTE SM]After LTE board reset, get cell's para failed, \n");
		    if (ret == RET_OK) ret = RET_FAILED;
			oss_delay(100); //100ms
		}
		cnt++;
		//when cnt is up to 10, exit from loop
		if (cnt >= 10) break; 
	}
	
	//report to OAM UI
	oam_lte_reset_rsp(p_lte_oam, u8ResetInd);
}
//////////////////////////////////////////////////////////////////////
void lte_cfg_set_rsp(lte_state_t *lte_state, lte_set_cfg_rsp_t *ptCfgRsp)
{
	unsigned char u8CfgInd = ptCfgRsp->u8CfgInd;

	return;
}
/////////////////////////////////////////////////////////////////////////
void lte_sniffer_req(lte_state_t *lte_state)
{
	lte_start_sniffer_req_t tReq;

	lte_msg_head_init(lte_state, &tReq.msgHeader, EVENT_LTE_START_SNIFFER_REQ);

	tReq.msgHeader.msgLen = htons(sizeof(tReq) - sizeof(msg_head_t));

	lte_send_msg(lte_state->peer_ip_addr, lte_state->peer_port, (uint8_t *)&tReq, sizeof(tReq));
}
//////////////////////////////////////////////////////////////////////
void lte_sniffer_rsp(lte_state_t *lte_state, lte_start_sniffer_rsp_t *ptSnifferRsp)
{   
    oam_lte_sniffer_rsp((lte_oam_t *)(lte_state->p_oam_addr));
}

/////////////////////////////////////////////////////////////////////////
void lte_power_set(lte_state_t *lte_state, uint16_t power)
{
	lte_set_power_req_t tReq;

	lte_msg_head_init(lte_state, &tReq.msgHeader, EVENT_LTE_SET_POWER_REQ);

	tReq.msgHeader.msgLen = htons(sizeof(tReq) - sizeof(msg_head_t));

	tReq.u16Power = htons(power);
	
	oss_set_timer_ex(O_LTE_POWER_SET_TIMER, lte_msg_proc, EVENT_POWER_SET_TIMEOUT, (uint32_t)lte_state);

	lte_send_msg(lte_state->peer_ip_addr, lte_state->peer_port, (uint8_t *)&tReq, sizeof(tReq));
	
}
/////////////////////////////////////////////////////////////////////////
void lte_power_rsp(lte_state_t *lte_state, lte_set_power_rsp_t *ptPowerRsp)
{
	unsigned char u8PowerInd = ptPowerRsp->u8PowerInd;

	oam_lte_set_power_rsp((lte_oam_t *)(lte_state->p_oam_addr), u8PowerInd);
	
    if (u8PowerInd == 1) 
	{  //success, stop its timer (event=0x104)
		oss_del_timer_ex((uint32_t)EVENT_POWER_SET_TIMEOUT); //xzluo
	}
	return;
}

/////////////////////////////////////////////////////////////////////////
void lte_cfg_load_req(lte_state_t *lte_state, lte_load_cfg_req_t *ptReq)
{
	lte_load_cfg_rsp_t tCfgLoadRsp;

	memset(&tCfgLoadRsp, 0, sizeof(lte_load_cfg_rsp_t));
	lte_cfg_load_rsp(lte_state, &tCfgLoadRsp);
}
//////////////////////////////////////////////////////////////////////
void lte_cfg_load_rsp(lte_state_t *lte_state, lte_load_cfg_rsp_t *ptCfgLoadRsp)
{
	int ret = -1;
	uint32_t loop;
	lte_load_cfg_rsp_t *ptCfg = &lte_state->lte_cfg;
	//unsigned char buf[82] = {0x60,0x03,0x00,0x46,0x00,0x00,0x01,0x23,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x03,0x04,0x06,0x72,0x01,0xcc,0x01,0x04,0x00,0x83,0x00,0x05,0x00,0x02,0x03,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0xa9,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x01,0x03,0x00,0x06,0x72,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00};

	lte_msg_head_init(lte_state, &ptCfgLoadRsp->msgHeader, EVENT_LTE_LOAD_CONFIG_RSP);
	ptCfgLoadRsp->msgHeader.msgLen = htons(sizeof(lte_load_cfg_rsp_t) - sizeof(msg_head_t));

	ptCfgLoadRsp->u16SynMode  = htons(lte_state->syn_mode);
	if(O_LTE_SYNC == lte_state->syn_mode)
	{
		ptCfgLoadRsp->strSynPara.u16Earfcn = htons(ptCfg->strSynPara.u16Earfcn);
		ptCfgLoadRsp->strSynPara.u16PCI    = htons(ptCfg->strSynPara.u16PCI);
		ptCfgLoadRsp->strSynPara.u8Band    = ptCfg->strSynPara.u8Band;
	}

	ptCfgLoadRsp->u16WorkMode = htons(lte_state->work_mode);
	switch(lte_state->work_mode)
	{
	case O_LTE_USER_CFG_WORKMODE:
		ptCfg->strPbsPara.u16Tac = 6300;
		printf("cell para: %d, %d, %d, %d, %d, %d, %d \n", ptCfg->strPbsPara.u8Band, ptCfg->strPbsPara.u16Earfcn, 
			ptCfg->strPbsPara.u16MCC, ptCfg->strPbsPara.u8Mnc, 
			ptCfg->strPbsPara.u16Tac, ptCfg->strPbsPara.u16PCI, ptCfg->strPbsPara.u16CellId);
			
		ptCfgLoadRsp->strPbsPara.u16CellId = htons(ptCfg->strPbsPara.u16CellId);
		ptCfgLoadRsp->strPbsPara.u16Earfcn = htons(ptCfg->strPbsPara.u16Earfcn);
		ptCfgLoadRsp->strPbsPara.u16MCC    = htons(ptCfg->strPbsPara.u16MCC);
		ptCfgLoadRsp->strPbsPara.u16PCI    = htons(ptCfg->strPbsPara.u16PCI);
		ptCfgLoadRsp->strPbsPara.u16Tac    = htons(ptCfg->strPbsPara.u16Tac);
		ptCfgLoadRsp->strPbsPara.u8Band    = ptCfg->strPbsPara.u8Band;
		ptCfgLoadRsp->strPbsPara.u8InitialValueTag  = ptCfg->strPbsPara.u8InitialValueTag;
		ptCfgLoadRsp->strPbsPara.u8Mnc    = ptCfg->strPbsPara.u8Mnc;
		ptCfgLoadRsp->strPbsPara.u8pwr    = ptCfg->strPbsPara.u8pwr;
	break;

	case O_LTE_SELF_CFG_WORKMODE:
		ptCfgLoadRsp->u32SnifferParaNum   = htonl(ptCfg->u32SnifferParaNum);
		for(loop=0; loop < ptCfg->u32SnifferParaNum; loop++)
		{
			ptCfgLoadRsp->strSnifferPara[loop].u16Earfcn = htons(ptCfg->strSnifferPara[loop].u16Earfcn);
			ptCfgLoadRsp->strSnifferPara[loop].u8Band    = ptCfg->strSnifferPara[loop].u8Band;
		}
		
	break;

	default:
	break;
	}

    if (lte_state->drive_to_gsm) 
	{
		ptCfgLoadRsp->strRelPara.u16Arfcn = htons(ptCfg->strRelPara.u16Arfcn);
		ptCfgLoadRsp->strRelPara.u8Band   = ptCfg->strRelPara.u8Band;
		ptCfgLoadRsp->strRelPara.u8RejectCause  = ptCfg->strRelPara.u8RejectCause;
		ptCfgLoadRsp->strRelPara.u8ReleaseCause = ptCfg->strRelPara.u8ReleaseCause; 
	}

	lte_send_msg(lte_state->peer_ip_addr, lte_state->peer_port, (uint8_t *)ptCfgLoadRsp, sizeof(lte_load_cfg_rsp_t));
	//lte_send_msg(lte_state->peer_ip_addr, lte_state->peer_port, (uint8_t *)buf, 82);
}

//////////////////////////////////////////////////////////////////////
void lte_para_rpt_rsp(lte_state_t *lte_state, lte_para_rpt_t *ptLteParaRpt)
{
	uint32_t k, PCI_num;
	lte_para_t	strLtePara[8];
	lte_para_rsp_t tLteParaRsp;
	
	PCI_num = ntohl(ptLteParaRpt->u32PCINum);
	for (k = 0; k < PCI_num; k++) 
	{
		strLtePara[k].u8Band = ptLteParaRpt->strLtePara[k].u8Band; 
		strLtePara[k].u16Earfcn = ntohs(ptLteParaRpt->strLtePara[k].u16Earfcn); 
		strLtePara[k].u16PCI = ntohs(ptLteParaRpt->strLtePara[k].u16PCI); 
		strLtePara[k].u8PdschRefPower = ptLteParaRpt->strLtePara[k].u8PdschRefPower; 
		strLtePara[k].u8Rsrp = ptLteParaRpt->strLtePara[k].u8Rsrp; 
		strLtePara[k].s16FreqOffset = ntohs(ptLteParaRpt->strLtePara[k].s16FreqOffset); 
		strLtePara[k].u16Tac = ntohs(ptLteParaRpt->strLtePara[k].u16Tac); 
		strLtePara[k].u32CellId = ntohl(ptLteParaRpt->strLtePara[k].u32CellId); 
		
		oam_lte_sniff_info_ind((lte_oam_t *)(lte_state->p_oam_addr), &(strLtePara[k]));
	}

	memset(&tLteParaRsp, 0, sizeof(lte_para_rsp_t)); 	
	lte_msg_head_init(lte_state, &(tLteParaRsp.msgHeader), EVENT_LTE_PARAMENTER_RSP);
	tLteParaRsp.msgHeader.msgLen = htons(sizeof(lte_para_rsp_t) - sizeof(msg_head_t));
	
	lte_send_msg(lte_state->peer_ip_addr, lte_state->peer_port, (uint8_t *)&tLteParaRsp, sizeof(lte_para_rsp_t));
}

void lte_para_update_req(lte_state_t *lte_state, lte_para_update_req_t *ptParaUpdateReq)
{
	uint16_t  u16UpdateFlag = 0;
	uint16_t  u16SynMode;
	uint16_t  u16UpdateInd = 0;

    
	u16UpdateFlag = ntohs(ptParaUpdateReq->u16UpdateFlag);
	u16SynMode = ntohs(ptParaUpdateReq->u16SynMode); //??

	if ((u16UpdateFlag) && (ptParaUpdateReq != NULL))  
	{
		lte_state->lte_cfg.strPbsPara.u8Band = ptParaUpdateReq->strPbsPara.u8Band;
		lte_state->lte_cfg.strPbsPara.u8pwr = ptParaUpdateReq->strPbsPara.u8pwr;
		lte_state->lte_cfg.strPbsPara.u16Earfcn = ntohs(ptParaUpdateReq->strPbsPara.u16Earfcn);
		lte_state->lte_cfg.strPbsPara.u16MCC = ntohs(ptParaUpdateReq->strPbsPara.u16MCC);
		lte_state->lte_cfg.strPbsPara.u8Mnc = ptParaUpdateReq->strPbsPara.u8Mnc;
		lte_state->lte_cfg.strPbsPara.u8InitialValueTag = ptParaUpdateReq->strPbsPara.u8InitialValueTag;
		lte_state->lte_cfg.strPbsPara.u16PCI = ntohs(ptParaUpdateReq->strPbsPara.u16PCI);
		lte_state->lte_cfg.strPbsPara.u16Tac = ntohs(ptParaUpdateReq->strPbsPara.u16Tac);
		lte_state->lte_cfg.strPbsPara.u16CellId = ntohs(ptParaUpdateReq->strPbsPara.u16CellId);

		lte_state->lte_cfg.strSynPara.u8Band = ptParaUpdateReq->strSynPara.u8Band;
		lte_state->lte_cfg.strSynPara.u16Earfcn = ntohs(ptParaUpdateReq->strSynPara.u16Earfcn);
		lte_state->lte_cfg.strSynPara.u16PCI = ntohs(ptParaUpdateReq->strSynPara.u16PCI);

		u16UpdateInd = 1; //update successfully
		oam_lte_para_update(lte_state);
	}

	
	lte_para_update_rsp(lte_state, u16UpdateInd);
	
}

void lte_para_update_rsp(lte_state_t *lte_state, uint16_t u16UpdateInd) 
{
	lte_para_update_rsp_t tParaUpdateRsp;
	
	memset(&tParaUpdateRsp, 0, sizeof(lte_para_update_rsp_t));
	
	lte_msg_head_init(lte_state, &(tParaUpdateRsp.msgHeader), EVENT_LTE_PARA_UPDATE_RSP);
	tParaUpdateRsp.msgHeader.msgLen = htons(sizeof(lte_para_update_rsp_t) - sizeof(msg_head_t));
	
	tParaUpdateRsp.u16UpdateInd = htons(u16UpdateInd);

	lte_send_msg(lte_state->peer_ip_addr, lte_state->peer_port, (uint8_t *)&tParaUpdateRsp, sizeof(lte_para_update_rsp_t));
}

void lte_Mobile_Noti_req(lte_state_t *lte_state, lte_mobile_notify_req_t *ptMobileNotiReq)
{
	int i;
	uint8_t     u8IMSI[8];
	uint8_t     hU8, lU8;
	lte_mobile_notify_rsp_t tMobileNotiRsp;
    lte_oam_t *p_lte_oam = (lte_oam_t *)(lte_state->p_oam_addr);
	
    memset(u8IMSI, 0, 8);
	memset(&tMobileNotiRsp, 0, sizeof(lte_mobile_notify_rsp_t));

	printf("IMSI:");
	for (i = 0; i< 8; i++) 
	{
		lU8 = (ptMobileNotiReq->u8IMSI[i] & 0xF0) >> 4;
		hU8 = (ptMobileNotiReq->u8IMSI[i] & 0x0F) << 4;
	    u8IMSI[i] = hU8 | lU8;
		printf("%02x", (uint8_t)u8IMSI[i]);
	}
    printf("\n");
    printf("ESN:");
	for (i = 0; i< 8; i++) 
	{
		printf("%02x", (uint8_t)ptMobileNotiReq->u8ESN[i]);
	}
	printf("\n");
	
	oam_lte_imsi_ind(p_lte_oam, (uint8_t *)(ptMobileNotiReq->u8IMSI));
	
    //send O_AACFBS_LTE_REGISTER_NOTIFY_RSP 0x6006
	lte_msg_head_init(lte_state, &(tMobileNotiRsp.msgHeader), EVENT_LTE_REGISTER_NOTIFY_RSP);
	tMobileNotiRsp.msgHeader.msgLen = htons(sizeof(lte_mobile_notify_rsp_t) - sizeof(msg_head_t));
	if (lte_state->target_set) 
	{ 
		memcpy(tMobileNotiRsp.u8IMSI, p_lte_oam->u8IMSI, 8);
		tMobileNotiRsp.acpt = 4;
	} 
	else 
	{
	    memcpy(tMobileNotiRsp.u8IMSI, ptMobileNotiReq->u8IMSI, 8);
	    tMobileNotiRsp.u8IMSI[7] |= 0xF0; 
	}

	lte_send_msg(lte_state->peer_ip_addr, lte_state->peer_port, (uint8_t *)&tMobileNotiRsp, sizeof(lte_mobile_notify_rsp_t));
	
}
/////////////////////////////////////////////////////////////////////////
void oam_lte_start_req(lte_state_t *lte_state, 	lte_start_req_t	*ptStartReq)
{
    uint16_t u16Start = ntohs(ptStartReq->u16Start);
	lte_state->start_value = u16Start; //save it for re-send when timer expiry
	lte_start_req(lte_state, u16Start);
}

/////////////////////////////////////////////////////////////////////////
void oam_lte_start_sniffer_req(lte_state_t *lte_state, lte_start_sniffer_req_t *ptSniffReq)
{
	lte_sniffer_req(lte_state);
}

/////////////////////////////////////////////////////////////////////////
void oam_lte_reset_req(lte_state_t *lte_state, 	lte_reset_req_t	*ptResetReq)
{
	lte_reset_req(lte_state);
}
/////////////////////////////////////////////////////////////////////////
void oam_lte_set_power_req(lte_state_t *lte_state, 	lte_set_power_req_t	*ptPowerSetReq)
{
	uint16_t u16Power = ntohs(ptPowerSetReq->u16Power);
	lte_state->lte_cfg.strPbsPara.u8pwr = (uint8_t)u16Power;
	lte_power_set(lte_state, u16Power);
}

/////////////////////////////////////////////////////////////////////////
void lte_tddfdd_switch_req(lte_state_t *lte_state)
{
	lte_tddfdd_switch_req_t tTddfdd_switch_req;

	lte_msg_head_init(lte_state, &tTddfdd_switch_req.msgHeader, EVENT_LTE_TDDFDD_SWITCH_REQ);
	tTddfdd_switch_req.msgHeader.msgLen = htons(sizeof(lte_tddfdd_switch_req_t) - sizeof(msg_head_t));

	tTddfdd_switch_req.u8WorkMode = lte_state->tddfdd_mode;
	lte_send_msg(lte_state->peer_ip_addr, lte_state->peer_port, (uint8_t *)&tTddfdd_switch_req, sizeof(lte_tddfdd_switch_req_t));
}

//////////////////////////////////////////////////////////////////////
void lte_tddfdd_switch_rsp(lte_state_t *lte_state, lte_tddfdd_switch_rsp_t *ptTddfddSwitchRsp)
{
	uint8_t u8Flag = ptTddfddSwitchRsp->u8Flag;

	if (u8Flag == 1) 
	{  //set success
	    //lte_heartbeat_req(lte_state);
	    lte_state_set(lte_state, LTE_SYS_STAT_START_CFG);
	} 
	else 
	{
		lte_tddfdd_switch_req(lte_state); //resend TDDFDD_SWITCH_REQ
	}
}

void lte_workmode_req(lte_state_t *lte_state)
{
	lte_workmode_req_t tWorkMode_req;

	lte_msg_head_init(lte_state, &tWorkMode_req.msgHeader, EVENT_LTE_WORKMODE_REQ);

	tWorkMode_req.msgHeader.msgLen = htons(sizeof(lte_workmode_req_t) - sizeof(msg_head_t));

	lte_send_msg(lte_state->peer_ip_addr, lte_state->peer_port, (uint8_t *)&tWorkMode_req, sizeof(lte_workmode_req_t));
}

void lte_workmode_rsp(lte_state_t *lte_state, lte_workmode_rsp_t *ptWorkModeRsp)
{
	if (lte_state->tddfdd_mode == ptWorkModeRsp->u8WorkMode) 
	{   //work mode isn't changed, send HEART BEAT msg
		//lte_heartbeat_req(lte_state);
		lte_state_set(lte_state, LTE_SYS_STAT_START_CFG);
	} 
	else 
	{  
		lte_tddfdd_switch_req(lte_state);
	}
}

/******************************************************************************/
void lte_sm_cell_get(lte_state_t *lte_state, char *buf, int len)
{
	msg_head_t *ptMsg    = (msg_head_t *)buf;
	int ret;

	switch(ntohs(ptMsg->msgType)) 
	{
	case EVENT_CELL_GET_TIMEOUT:
		ret = lte_cell_select(lte_state);
		if(ret != RET_OK)
		{
			// wait and get cell in next time
			oss_set_timer_ex(O_LTE_CELL_GET_TIMER, lte_msg_proc, EVENT_CELL_GET_TIMEOUT, (uint32_t)lte_state); 
			oam_lte_err_ind((lte_oam_t *)(lte_state->p_oam_addr), GET_CELL_PARA_ERROR);
			lte_state->cellselect_fail++;
			if (lte_state->cellselect_fail > MAX_CELL_SEL_NUM) 
			{
				//Modem_reset(&(lte_state->tModem));
				lte_state->cellselect_fail = 0;
			}
			return ;
		}
		
        lte_state->cellselect_fail = 0;
		oam_lte_mode_ind((lte_oam_t *)(lte_state->p_oam_addr), lte_state->tddfdd_mode);
        // go to next status and send a heart beat to lte board
	    lte_state_set(lte_state, LTE_SYS_STAT_DISCONN);
		//lte_workmode_req(lte_state); //look up the work mode of LTE board
		lte_heartbeat_req(lte_state);
		break;
		
	default:
		break;
	} 	
}
/******************************************************************************/
void lte_sm_disconn(lte_state_t *lte_state, char *buf, int len)
{
	msg_head_t *ptMsg    = (msg_head_t *)buf;
	lte_workmode_rsp_t      *ptWorkModeRsp;
	lte_tddfdd_switch_rsp_t *ptTddfddSwitchRsp;
	lte_heartbeat_rsp_t		*ptHeartBeatRsp;
	lte_reset_rsp_t     *ptResetRsp;	

	switch(ntohs(ptMsg->msgType))
	{
	case EVENT_LTE_WORKMODE_RSP:
		ptWorkModeRsp = (lte_workmode_rsp_t *)ptMsg;
		lte_workmode_rsp(lte_state, ptWorkModeRsp);
		break;
		
	case EVENT_LTE_TDDFDD_SWITCH_RSP:
		ptTddfddSwitchRsp = (lte_tddfdd_switch_rsp_t *)ptMsg;
		lte_tddfdd_switch_rsp(lte_state, ptTddfddSwitchRsp);
		break;
		
	case EVENT_LTE_HEARBEAT_RSP:
		ptHeartBeatRsp = (lte_heartbeat_rsp_t   *)ptMsg;
		lte_heartbeat_rsp(lte_state, ptHeartBeatRsp);
		if (lte_state->remote_state == LTE_BOARD_STAT_POWERON) 
		{
			lte_workmode_req(lte_state); //look up the work mode of LTE board
			//lte_state_set(lte_state, LTE_SYS_STAT_START_CFG);
		} 
		else 
		{
			 lte_reset_req(lte_state);
		}
		break;

	/* if LTE board state isn't LTE_BOARD_STAT_POWERON,
       reset it and wait for its response
	*/
	case EVENT_LTE_RESET_RSP:
		ptResetRsp = (lte_reset_rsp_t   *)ptMsg;
		lte_reset_rsp(lte_state, ptResetRsp);
	    break;
		
	case EVENT_HEARTBEAT_TIMEOUT:
		lte_heartbeat_req(lte_state);
		break;
	
	default:
		break;
	}
}
/******************************************************************************/
void lte_sm_start_cfg(lte_state_t *lte_state, char *buf, int len)
{
	msg_head_t *ptMsg    = (msg_head_t *)buf;
	lte_heartbeat_rsp_t		*ptHeartBeatRsp;
	lte_load_cfg_req_t *ptCfgLoadReq;

	switch(ntohs(ptMsg->msgType))
	{
	case EVENT_LTE_HEARBEAT_RSP:
		ptHeartBeatRsp = (lte_heartbeat_rsp_t   *)ptMsg;
		lte_heartbeat_rsp(lte_state, ptHeartBeatRsp);
		break;
	
	case EVENT_HEARTBEAT_TIMEOUT:
		lte_heartbeat_req(lte_state);
		break;

	case EVENT_LTE_LOAD_CONFIG_REQ:
		ptCfgLoadReq = (lte_load_cfg_req_t   *)ptMsg;
		lte_cfg_load_req(lte_state, ptCfgLoadReq);
		lte_state_set(lte_state, LTE_SYS_STAT_DATAREADY);
		lte_heartbeat_req(lte_state);
		//lte_start_req(lte_state, 1);
		break;
		
	default:
		break;
	}
}
/******************************************************************************/
void lte_sm_dataready(lte_state_t *lte_state, char *buf, int len)
{
	msg_head_t *ptMsg    = (msg_head_t *)buf;
	lte_heartbeat_rsp_t	*ptHeartBeatRsp;
	lte_start_req_t	*ptStartReq;
	lte_start_rsp_t	*ptStartRsp;
	lte_reset_rsp_t   *ptResetRsp;
	lte_reset_req_t   *ptResetReq;

	switch(ntohs(ptMsg->msgType))
	{
	case EVENT_LTE_HEARBEAT_RSP:
		ptHeartBeatRsp = (lte_heartbeat_rsp_t   *)ptMsg;
		lte_heartbeat_rsp(lte_state, ptHeartBeatRsp);
// when OAM UI knows local state is LTE_SYS_STAT_DATAREADY(2) from
// heartbeat msg,it will send OP_OAM_LTE_START_REQ to start LTE board,
// so delete the codes as below,
// OAM UI maybe send OP_OAM_LTE_START_REQ in any state, so restore it
		if (lte_state->startReq_send) 
		{
			lte_start_req(lte_state, 1); //1:start
			lte_state->startReq_send = 0;
		} 		
		break;
	
	case EVENT_HEARTBEAT_TIMEOUT:
		lte_heartbeat_req(lte_state);
		break;

	case EVENT_LTE_START_RSP:
		ptStartRsp = (lte_start_rsp_t   *)ptMsg;
		lte_start_rsp(lte_state, ptStartRsp);
		//lte_state_set(lte_state, LTE_SYS_STAT_RUNNING); 
		break;

	case EVENT_OAM_LTE_START_REQ:
		ptStartReq = (lte_start_req_t   *)ptMsg;
		oam_lte_start_req(lte_state, ptStartReq);
		break;		

	case EVENT_OAM_LTE_RESET_REQ:
		ptResetReq = (lte_reset_req_t   *)ptMsg;
		oam_lte_reset_req(lte_state, ptResetReq);
		break;

	case EVENT_LTE_RESET_RSP:
		ptResetRsp = (lte_reset_rsp_t   *)ptMsg;
		lte_reset_rsp(lte_state, ptResetRsp);
	break;

    case EVENT_STARTUP_TIMEOUT:
		lte_start_req(lte_state, lte_state->start_value);
		break;
		
	default:
		break;
	}
}
/******************************************************************************/
void lte_sm_running(lte_state_t *lte_state, char *buf, int len)
{
	msg_head_t *ptMsg    = (msg_head_t *)buf;
	lte_heartbeat_rsp_t  *ptHeartBeatRsp;
	lte_reset_rsp_t     *ptResetRsp;
	lte_reset_req_t     *ptResetReq;
	lte_start_req_t	    *ptStartReq;
	lte_start_rsp_t     *ptStartRsp;
	lte_set_power_rsp_t *ptPowerSetRsp;
	lte_set_power_req_t	*ptPowerSetReq;
	lte_para_update_req_t * ptParaUpdateReq;
	lte_mobile_notify_req_t * ptMobileNotiReq;

	switch(ntohs(ptMsg->msgType))
	{
	case EVENT_LTE_HEARBEAT_RSP:
		ptHeartBeatRsp = (lte_heartbeat_rsp_t   *)ptMsg;
		lte_heartbeat_rsp(lte_state, ptHeartBeatRsp);
		if (lte_state->remote_state == LTE_BOARD_STAT_POWERON) 
		{
			lte_state_set(lte_state, LTE_SYS_STAT_DISCONN);
		}
		break;
	
	case EVENT_HEARTBEAT_TIMEOUT:
		lte_heartbeat_req(lte_state);
		break;

	case EVENT_LTE_START_RSP:
		ptStartRsp = (lte_start_rsp_t   *)ptMsg;
		lte_start_rsp(lte_state, ptStartRsp);
		break;

    case EVENT_OAM_LTE_START_REQ:  //stop
		ptStartReq = (lte_start_req_t   *)ptMsg;
		oam_lte_start_req(lte_state, ptStartReq);
		break;
		
	case EVENT_OAM_LTE_RESET_REQ:
		ptResetReq = (lte_reset_req_t   *)ptMsg;
		oam_lte_reset_req(lte_state, ptResetReq);
		break;

	case EVENT_LTE_RESET_RSP:
		ptResetRsp = (lte_reset_rsp_t   *)ptMsg;
		lte_reset_rsp(lte_state, ptResetRsp);
	break;

	case EVENT_OAM_LTE_SET_POWER_REQ:
		//ptPowerSetReq = (lte_set_power_req_t   *)(ptMsg + 1);
		ptPowerSetReq = (lte_set_power_req_t   *)ptMsg;
		oam_lte_set_power_req(lte_state, ptPowerSetReq);
	break;

	case EVENT_POWER_SET_TIMEOUT:
		lte_power_set(lte_state, (uint16_t)lte_state->lte_cfg.strPbsPara.u8pwr);		
	break;

	case EVENT_LTE_SET_POWER_RSP:
		ptPowerSetRsp = (lte_set_power_rsp_t  *)ptMsg;
		lte_power_rsp(lte_state, ptPowerSetRsp);
	break;

    case EVENT_LTE_PARA_UPDATE_REQ:
		ptParaUpdateReq = (lte_para_update_req_t  *)ptMsg;
		lte_para_update_req(lte_state, ptParaUpdateReq);
	break;

	case EVENT_LTE_REGISTER_NOTIFY_REQ:
		ptMobileNotiReq = (lte_mobile_notify_req_t  *)ptMsg;
		lte_Mobile_Noti_req(lte_state, ptMobileNotiReq);
	break;
	
	default:
	break;
	}
}
/******************************************************************************/
void lte_sm_scan(lte_state_t *lte_state, char *buf, int len)
{
	msg_head_t *ptMsg    = (msg_head_t *)buf;
	lte_heartbeat_rsp_t	*ptHeartBeatRsp;
	lte_start_req_t	*ptStartReq;
	lte_start_rsp_t	*ptStartRsp;
	lte_reset_rsp_t   *ptResetRsp;
	lte_reset_req_t   *ptResetReq;
	lte_para_rpt_t    *ptLteParaRpt;
	lte_start_sniffer_req_t *ptSnifferReq;
	lte_start_sniffer_rsp_t *ptSnifferRsp;

	switch(ntohs(ptMsg->msgType))
	{
	case EVENT_LTE_HEARBEAT_RSP:
		ptHeartBeatRsp = (lte_heartbeat_rsp_t   *)ptMsg;
		lte_heartbeat_rsp(lte_state, ptHeartBeatRsp);
		break;
	
	case EVENT_HEARTBEAT_TIMEOUT:
		lte_heartbeat_req(lte_state);
		break;

	case EVENT_LTE_START_RSP:
		ptStartRsp = (lte_start_rsp_t   *)ptMsg;
		lte_start_rsp(lte_state, ptStartRsp);
		break;

	case EVENT_OAM_LTE_START_REQ:
		ptStartReq = (lte_start_req_t   *)ptMsg;
		oam_lte_start_req(lte_state, ptStartReq);
		break;		

	case EVENT_OAM_LTE_RESET_REQ:
		ptResetReq = (lte_reset_req_t   *)ptMsg;
		oam_lte_reset_req(lte_state, ptResetReq);
		break;

	case EVENT_LTE_RESET_RSP:
		ptResetRsp = (lte_reset_rsp_t   *)ptMsg;
		lte_reset_rsp(lte_state, ptResetRsp);
	    break;

    case EVENT_OAM_LTE_SNIFFER_REQ:
		ptSnifferReq = (lte_start_sniffer_req_t   *)ptMsg;
		oam_lte_start_sniffer_req(lte_state, ptSnifferReq);
		break;
		
	case EVENT_LTE_START_SNIFFER_RSP:
		ptSnifferRsp = (lte_start_sniffer_rsp_t  *)ptMsg;
		lte_sniffer_rsp(lte_state, ptSnifferRsp);
		break;

    case EVENT_LTE_PARAMENTER_RPT:
		ptLteParaRpt = (lte_para_rpt_t *)ptMsg;
		lte_para_rpt_rsp(lte_state, ptLteParaRpt);
	    break;
	
	default:
		break;
	}
}
/******************************************************************************/
void lte_sm_sync(lte_state_t *lte_state, char *buf, int len)
{
	msg_head_t *ptMsg    = (msg_head_t *)buf;
	lte_heartbeat_rsp_t	*ptHeartBeatRsp;
	lte_start_req_t	*ptStartReq;
	lte_start_rsp_t	*ptStartRsp;
	lte_reset_rsp_t   *ptResetRsp;
	lte_reset_req_t   *ptResetReq;
	lte_para_rpt_t    *ptLteParaRpt;
	lte_set_cfg_rsp_t   *ptCfgSetRsp;

	switch(ntohs(ptMsg->msgType))
	{
	case EVENT_LTE_HEARBEAT_RSP:
		ptHeartBeatRsp = (lte_heartbeat_rsp_t   *)ptMsg;
		lte_heartbeat_rsp(lte_state, ptHeartBeatRsp);
	break;
	
	case EVENT_HEARTBEAT_TIMEOUT:
		lte_heartbeat_req(lte_state);
	break;

	case EVENT_LTE_START_RSP:
		ptStartRsp = (lte_start_rsp_t   *)ptMsg;
		lte_start_rsp(lte_state, ptStartRsp);
	break;

	case EVENT_OAM_LTE_START_REQ:
		ptStartReq = (lte_start_req_t   *)ptMsg;
		oam_lte_start_req(lte_state, ptStartReq);
	break;		

	case EVENT_OAM_LTE_RESET_REQ:
		ptResetReq = (lte_reset_req_t   *)ptMsg;
		oam_lte_reset_req(lte_state, ptResetReq);
	break;

	case EVENT_LTE_RESET_RSP:
		ptResetRsp = (lte_reset_rsp_t   *)ptMsg;
		lte_reset_rsp(lte_state, ptResetRsp);
	break;

	case EVENT_LTE_SET_CONFIG_RSP:
		ptCfgSetRsp = (lte_set_cfg_rsp_t   *)ptMsg;
		lte_cfg_set_rsp(lte_state, ptCfgSetRsp);
	break;
	
	case EVENT_LTE_PARAMENTER_RPT:
		ptLteParaRpt = (lte_para_rpt_t *)ptMsg;
		lte_para_rpt_rsp(lte_state, ptLteParaRpt);
	break;
	
	case EVENT_LTE_PARA_UPDATE_RSP:
	break;

	default:
	break;
	}
}
/******************************************************************************/
void lte_sm_nosync(lte_state_t *lte_state, char *buf, int len)
{
	msg_head_t *ptMsg    = (msg_head_t *)buf;
	lte_heartbeat_rsp_t	*ptHeartBeatRsp;
	lte_start_req_t	*ptStartReq;
	lte_start_rsp_t	*ptStartRsp;
	lte_reset_rsp_t   *ptResetRsp;
	lte_reset_req_t   *ptResetReq;

	switch(ntohs(ptMsg->msgType))
	{
	case EVENT_LTE_HEARBEAT_RSP:
		ptHeartBeatRsp = (lte_heartbeat_rsp_t   *)ptMsg;
		lte_heartbeat_rsp(lte_state, ptHeartBeatRsp);
		break;
	
	case EVENT_HEARTBEAT_TIMEOUT:
		lte_heartbeat_req(lte_state);
		break;

	case EVENT_LTE_START_RSP:
		ptStartRsp = (lte_start_rsp_t   *)ptMsg;
		lte_start_rsp(lte_state, ptStartRsp);
		break;

	case EVENT_OAM_LTE_START_REQ:
		ptStartReq = (lte_start_req_t   *)ptMsg;
		oam_lte_start_req(lte_state, ptStartReq);
		break;		

	case EVENT_OAM_LTE_RESET_REQ:
		ptResetReq = (lte_reset_req_t   *)ptMsg;
		oam_lte_reset_req(lte_state, ptResetReq);
		break;

	case EVENT_LTE_RESET_RSP:
		ptResetRsp = (lte_reset_rsp_t   *)ptMsg;
		lte_reset_rsp(lte_state, ptResetRsp);
	break;

	default:
		break;
	}
}
/******************************************************************************/
typedef struct Tag_lte_machine {
	uint32_t  state;
	void (*func)(lte_state_t *lte_state, char *buf, int len);
}lte_machine_t;

lte_machine_t g_lte_state_machine[] = {
	{LTE_SYS_STAT_CELL_GET, lte_sm_cell_get},
	{LTE_SYS_STAT_DISCONN, lte_sm_disconn},
	{LTE_SYS_STAT_START_CFG, lte_sm_start_cfg},
	{LTE_SYS_STAT_DATAREADY, lte_sm_dataready},
	{LTE_SYS_STAT_RUNNING, lte_sm_running},
	{LTE_SYS_STAT_SCAN, lte_sm_scan},
	{LTE_SYS_STAT_SYNC, lte_sm_sync},
	{LTE_SYS_STAT_NOSYNC, lte_sm_nosync},
	{-1, NULL}
};
/******************************************************************************/
void lte_state_machine(lte_state_t *lte_state, char *buf, int len)
{
	msg_head_t *ptMsg    = (msg_head_t *)buf;

	printf("local state=%d remote state=%d event=%04x \n", lte_state->state, lte_state->remote_state, ntohs(ptMsg->msgType)); 
	switch(lte_state->state)
	{
	case LTE_SYS_STAT_CELL_GET:
		lte_sm_cell_get(lte_state, buf, len);
	break;
	
	case LTE_SYS_STAT_DISCONN:
		lte_sm_disconn(lte_state, buf, len);
	break;

	case LTE_SYS_STAT_START_CFG:
		lte_sm_start_cfg(lte_state, buf, len);
	break;

	case LTE_SYS_STAT_DATAREADY:
		lte_sm_dataready(lte_state, buf, len);
	break;

	case LTE_SYS_STAT_RUNNING:
		lte_sm_running(lte_state, buf, len);
	break;
	
	case LTE_SYS_STAT_SCAN:
		lte_sm_scan(lte_state, buf, len);
	break;
	
	case LTE_SYS_STAT_SYNC:
		lte_sm_sync(lte_state, buf, len);
	break;
	
	case LTE_SYS_STAT_NOSYNC:
	break;

	case LTE_SYS_STAT_EMERGENCY:
	break;
	
	case LTE_SYS_STAT_SYNCFAILED:
	break;
	
	default:
	break;
	}

}
/******************************************************************************/
int lte_timeout_event(uint32_t event)
{
	if(    (event <= EVENT_LTE_TIMEOUT_END)
		&& (event >= EVENT_LTE_TIMEOUT_START))
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

/******************************************************************************/
void lte_msg_proc(char *buf, int len)
{
	oss_msg_head_t *msg_head = (oss_msg_head_t *)buf;
	msg_timeout_t  *msg_timeout;

	if(lte_timeout_event(msg_head->msg_type))
	{
		msg_timeout = (msg_timeout_t *)(msg_head + 1);
		msg_head->msg_type = htons(msg_head->msg_type);
		lte_state_machine((lte_state_t *)msg_timeout->dwData, buf, len);
	}
	else
	{

	}
}

/******************************************************************************/
void lte_sm_start(lte_state_t *lte_state)
{
    lte_state_set(lte_state, LTE_SYS_STAT_CELL_GET);
	oss_set_timer_ex(O_LTE_CELL_GET_TIMER, lte_msg_proc, EVENT_CELL_GET_TIMEOUT, (uint32_t)lte_state);
}

/////////////////////////////////////////////////////////////////////////
void lte_oam_set_power(lte_state_t *lte_state, uint16_t power)
{
	lte_set_power_req_t req;

	req.msgHeader.msgType = htons(EVENT_OAM_LTE_SET_POWER_REQ);
	req.msgHeader.msgLen  = htons(sizeof(req) - sizeof(msg_head_t));
	req.u16Power          = htons(power);

	lte_state_machine(lte_state, (uint8_t *)&req, sizeof(req));
}
/////////////////////////////////////////////////////////////////////////
void lte_oam_reset(lte_state_t *lte_state)
{
	lte_reset_req_t	req;

	req.msgHeader.msgType = htons(EVENT_OAM_LTE_RESET_REQ);
	req.msgHeader.msgLen  = htons(sizeof(req) - sizeof(msg_head_t));

	lte_state_machine(lte_state, (uint8_t *)&req, sizeof(req));
}
/////////////////////////////////////////////////////////////////////////
void lte_oam_start(lte_state_t *lte_state, uint16_t start)
{
	lte_start_req_t req;

	req.msgHeader.msgType = htons(EVENT_OAM_LTE_START_REQ);
	req.msgHeader.msgLen  = htons(sizeof(req) - sizeof(msg_head_t));
	req.u16Start          = htons(start);

	lte_state_machine(lte_state, (uint8_t *)&req, sizeof(req));
}
/////////////////////////////////////////////////////////////////////////
void lte_oam_sniffer(lte_state_t *lte_state)
{
	lte_start_sniffer_req_t	req;

	req.msgHeader.msgType = htons(EVENT_OAM_LTE_SNIFFER_REQ);
	req.msgHeader.msgLen  = htons(sizeof(req) - sizeof(msg_head_t));

	lte_state_machine(lte_state, (uint8_t *)&req, sizeof(req));
}
/////////////////////////////////////////////////////////////////////////

