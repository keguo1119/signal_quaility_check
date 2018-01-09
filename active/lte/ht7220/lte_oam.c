#include "oss.h"
#include "oss_time.h" 
#include "oss_gpio.h"
#include "protocol_common.h"
#include "modem.h"
#include "lte_cfg.h"
#include "lte_sm.h"
#include "lte_cell_sel.h"
#include "lte_oam.h"
#include "sim_ctrl.h"

////////////////////////////////////////////////////////////////////////
lte_oam_t g_lte_oam;
//CN operator switch list
oam_CN_oper_switch_t operSwitchList[6] = {
	{CMCC, CUCC, CmccToCucc},
	{CMCC, CTCC, CmccToCtcc},
	{CUCC, CMCC, CuccToCmcc},
	{CUCC, CTCC, CuccToCtcc},
	{CTCC, CMCC, CtccToCmcc},
	{CTCC, CUCC, CtccToCucc}
}; 
//
#ifndef WIN32
pthread_mutex_t sync_mutex;
pthread_cond_t sync_cond;
int sync_var = -1;
int scan_type = ENV_BANDS_SCAN; //(1)env band scan or (2)scell+ncell scan
#endif
/////////////////////////////////////////////////////////////////////////
void app_oam_heartbeat_req(lte_oam_t *p_oam_lte); 
void app_oam_cmd_rsp(lte_oam_t *p_lte_oam, uint8_t op_code, int flag);
int oam_env_bands_scan(lte_oam_t *p_lte_oam);

///////////////////////////////////////////////////////////////////////// 
#ifndef WIN32
void *oam_bands_scan_thread(void *arg) 
{
	lte_oam_t *p_lte_oam = (lte_oam_t *)arg;
	lte_state_t * p_lte_state = (lte_state_t *)(p_lte_oam->p_lte_addr);
	int scanType;
	
	while(1) 
	{
		pthread_mutex_lock(&sync_mutex);
	    while (sync_var<0) 
		{
	        pthread_cond_wait( &sync_cond, &sync_mutex ); 
	        //block here,if receive OP_OAM_ENV_SCAN_REQ msg or timeout, 
	        //pthread_cond_signal() is called and sync_var=0,
	        //so wake up,and check "while (sync_var<0)",it's false,next sentence is executed, 
		}
		
        sync_var=-1; //to avoid oam_env_bands_scan() is called two times for one signal
        scanType = scan_type; 
	    pthread_mutex_unlock(&sync_mutex);
		//here, if interrupted, schedule to main program, global scan_type maybe changed,
		if (scanType == ENV_BANDS_SCAN) 
		{
		    oam_env_bands_scan(p_lte_oam); //take too long time  
		}
		else if (scanType == SERV_CELL_SCAN)
		{
			lte_cells_para_acquire(p_lte_state);
		}
		else 
		{ 
		}
	}
}
#endif
///////////////////////////////////////////////////////////////////////// 
void oam_band_node_insert(oam_bands_node_t ** pptBand, uint8_t mode, uint8_t band, uint8_t prio) 
{
    oam_bands_node_t * pBNode, * prePtr, * currPtr;
	
	pBNode = malloc(sizeof(oam_bands_node_t));
	if (pBNode != NULL) 
	{
	    pBNode->workMode = mode;
		pBNode->band     = band;
		pBNode->priority = prio;
		pBNode->band_next = NULL;

		prePtr = NULL;
		currPtr = *pptBand;
		while (currPtr != NULL) //  tailer insert 
		{
		    prePtr = currPtr;
			currPtr = currPtr->band_next;
		}

		if (prePtr == NULL) 
		{   //header insert
		    pBNode->band_next = *pptBand;
			*pptBand = pBNode;
		} 
		else 
		{
		    prePtr->band_next = pBNode;
			pBNode->band_next = currPtr;
		}
	} 
	else 
	{
	    printf("[LTE OAM]no memory available, insert failed \n");
	}
}

/////////////////////////////////////////////////////////////////////////
void oam_bands_list_insert(oam_opers_list_t * pOList, uint8_t cn_oper, uint8_t mode, uint8_t band, uint8_t prio)  
{
    //oam_bands_node_t * ptBNode;
	
    while (pOList != NULL) 
	{
	    if (pOList->cn_oper == cn_oper)
		{
		    //ptBNode = pOList->ptBand;
			oam_band_node_insert(&pOList->ptBand /*&ptBNode*/, mode, band, prio);
			break;
		} 
		else 
		{
		    pOList = pOList->oper_next;
		}
	}
}

/////////////////////////////////////////////////////////////////////////
void oam_oper_list_insert(oam_opers_list_t ** ppOList, uint8_t cn_oper) 
{
    oam_opers_list_t * pONode, * prePtr, * currPtr;
	
	pONode = malloc(sizeof(oam_opers_list_t));
	if (pONode != NULL) 
	{
	    pONode->cn_oper = cn_oper;
		pONode->ptBand = NULL;  //don't forget, otherwise it'll be random addr value
		pONode->oper_next = NULL;

		prePtr = NULL;
		currPtr = *ppOList;
		while (currPtr != NULL && cn_oper > currPtr->cn_oper) // insert by cn_oper order 
		{
		    prePtr = currPtr;
			currPtr = currPtr->oper_next;
		}

		if (prePtr == NULL) 
		{   //list header
		    pONode->oper_next = *ppOList;
			*ppOList = pONode;
		} 
		else 
		{
		    prePtr->oper_next = pONode;
			pONode->oper_next = currPtr;
		}
	} 
	else 
	{
	    printf("[LTE OAM]cn_oper:%d not inserted,no memory available \n", cn_oper);
	}
}

/////////////////////////////////////////////////////////////////////////
void oam_band_node_delete(oam_bands_node_t ** pptBand, uint8_t band) 
{
    oam_bands_node_t * tmpPtr, * prePtr, * currPtr;
	
	if ((*pptBand)->band == band) 
	{
	    tmpPtr = *pptBand;
		*pptBand = (*pptBand)->band_next;
		free (tmpPtr);
	} 
	else 
	{
	    prePtr = *pptBand;
		currPtr = (*pptBand)->band_next;
		while ((currPtr != NULL) && (currPtr->band != band))  
		{
		    prePtr = currPtr;
			currPtr = currPtr->band_next;  //next node
		}

		if (currPtr != NULL) 
		{   //find
		    tmpPtr = currPtr;
			prePtr->band_next = currPtr->band_next;
			free (tmpPtr);
		} 
	}
}

/////////////////////////////////////////////////////////////////////////
//delete node according to cn_oper and its band
void oam_bands_list_delete(oam_opers_list_t * pOList, uint8_t cn_oper, uint8_t band)  
{
    //loop through the list
    while (pOList != NULL) 
	{
	    if ((pOList->cn_oper == cn_oper) && (pOList->ptBand != NULL))
		{
			oam_band_node_delete(&pOList->ptBand, band);
			break;
		} 
		else 
		{
		    pOList = pOList->oper_next;
		}
	}
}

///////////////////////////////////////////////////////////////////////// 
void oam_oper_list_delete(oam_opers_list_t ** ppOList, uint8_t cn_oper) 
{
    oam_opers_list_t * tmpPtr, * prePtr, * currPtr;
	oam_bands_node_t * ptBand;

	tmpPtr = NULL;
	if ((*ppOList)->cn_oper == cn_oper) 
	{
	    tmpPtr = *ppOList;
		*ppOList = (*ppOList)->oper_next;
	} 
	else 
	{
	    prePtr = *ppOList;
		currPtr = (*ppOList)->oper_next;
		while ((currPtr != NULL) && (currPtr->cn_oper != cn_oper))  
		{
		    prePtr = currPtr;
			currPtr = currPtr->oper_next;  //next node
		}

		if (currPtr != NULL) 
		{   //find
		    tmpPtr = currPtr;
			prePtr->oper_next = currPtr->oper_next;
		} 
	}

	if (tmpPtr != NULL) {
		// firstly, delete its band list
		ptBand = tmpPtr->ptBand;
		while (ptBand != NULL) 
		{
		    oam_band_node_delete(&ptBand, ptBand->band);
			//ptBand = ptBand->band_next;
		}
		free (tmpPtr);
	}
}

/////////////////////////////////////////////////////////////////////////
oam_bands_node_t * oam_band_node_find(oam_bands_node_t ** pptBand, uint8_t band) 
{
    oam_bands_node_t * tmpPtr;

	tmpPtr = *pptBand;
	while (tmpPtr != NULL) 
	{
	    if (tmpPtr->band == band) break;  //find
		tmpPtr = tmpPtr->band_next;
	}
	
	return tmpPtr;
}

/////////////////////////////////////////////////////////////////////////
int oam_band_prio_modify(oam_opers_list_t * pOList, uint8_t cn_oper, uint8_t band, uint8_t u8Prio) 
{
    int ret_val = RET_FAILED;
    oam_bands_node_t * ptBNode = NULL;
	
    while (pOList != NULL) 
	{
	    if (pOList->cn_oper == cn_oper)
		{
			ptBNode = oam_band_node_find(&pOList->ptBand, band);
			//if it's found, replace the priority
			if (ptBNode != NULL) 
			{
			    ptBNode->priority = u8Prio;
				ret_val = RET_OK;
			}
			break;
		} 
		else 
		{
		    pOList = pOList->oper_next;
		}
	}
	
	return ret_val;
}

/////////////////////////////////////////////////////////////////////////
int lte_oam_init(lte_state_t *lte_state) 
{
#ifndef WIN32
	pthread_t thread_id;
	int result;
#endif
	uint16_t mcc = 0;
    uint8_t  mnc = 0;
	uint8_t  hU8, lU8;
    uint8_t  k;

    g_lte_oam.peer_ip_addr = ntohl(NetConnectAton("10.0.33.115"));
	g_lte_oam.peer_port = LTE_APP_UDP_PORT;

	g_lte_oam.heartbeat_lost = 0;
	g_lte_oam.p_lte_addr = (uint32_t)lte_state;

	g_lte_oam.p_opers_list = NULL;
	oam_oper_list_insert(&g_lte_oam.p_opers_list, CMCC);
	oam_oper_list_insert(&g_lte_oam.p_opers_list, CUCC);
	oam_oper_list_insert(&g_lte_oam.p_opers_list, CTCC);
	
    //china mobile, FDD/TDD mode, band, priority
	oam_bands_list_insert(g_lte_oam.p_opers_list,CMCC,TDD,38,7);
	oam_bands_list_insert(g_lte_oam.p_opers_list,CMCC,TDD,41,5);
	oam_bands_list_insert(g_lte_oam.p_opers_list,CMCC,TDD,39,3);
	oam_bands_list_insert(g_lte_oam.p_opers_list,CMCC,TDD,40,0);
    //china unicom, FDD/TDD mode, band, priority
	oam_bands_list_insert(g_lte_oam.p_opers_list,CUCC,FDD,3,7);
	oam_bands_list_insert(g_lte_oam.p_opers_list,CUCC,FDD,1,5);
	oam_bands_list_insert(g_lte_oam.p_opers_list,CUCC,TDD,41,6);
	//oam_bands_list_insert(g_lte_oam.p_opers_list,CUCC,TDD,40,2);
    //china telcom, FDD/TDD mode, band, priority
	oam_bands_list_insert(g_lte_oam.p_opers_list,CTCC,FDD,1,6);
	oam_bands_list_insert(g_lte_oam.p_opers_list,CTCC,FDD,3,5);
	oam_bands_list_insert(g_lte_oam.p_opers_list,CTCC,TDD,41,4);
	//oam_bands_list_insert(g_lte_oam.p_opers_list,CTCC,TDD,40,1);
//	oam_band_prio_modify(g_lte_oam.p_opers_list,CTCC,1,7);

    if (lte_state->tModem.tPlmn.isValid) 
	{ 
	    mcc = ((((lte_state->tModem.tPlmn.abyMcc[0] & 0x0F)*100)/256) << 8) + 
			   (((lte_state->tModem.tPlmn.abyMcc[0] & 0x0F)*100)%256) + 
			((lte_state->tModem.tPlmn.abyMcc[1] & 0x0F)*10) + (lte_state->tModem.tPlmn.abyMcc[2] & 0x0F);
		mnc = ((lte_state->tModem.tPlmn.abyMnc[0] & 0x0F)*10) + (lte_state->tModem.tPlmn.abyMnc[1] & 0x0F);
	    g_lte_oam.cn_oper = oam_get_oper_byCnType(mcc, mnc);
        //IMSI of USIM card
        printf("USIM Card IMSI:");
		for (k = 0; k < 8; k++) 
		{
			hU8 = (lte_state->tModem.abyImsi[2*k] & 0x0F) << 4;
	        lU8 = lte_state->tModem.abyImsi[2*k+1] & 0x0F;
	        g_lte_oam.u8IMSI[k] = hU8 | lU8;
			printf("%02x", (uint8_t)g_lte_oam.u8IMSI[k]);
		}
		printf("\n");
    }
	else 
	{
		printf("No SIM Card is detected, \n");
		/* default value */
		g_lte_oam.cn_oper = CMCC; 
		//g_lte_oam.band = 38;
		memset(g_lte_oam.u8IMSI, 0xFF, 8);
	}
	
	g_lte_oam.env_scan_interval = 0; //no periodical scan
	g_lte_oam.cells_para_scan_interval = 0;
	g_lte_oam.index = 0;
	for (k = 0; k < TIME_STEP_NUM; k++) {
	    g_lte_oam.time_span[k] = 2*(k+1); //step length:2s
	}
	
    lte_state->p_oam_addr = (uint32_t)&g_lte_oam; 

	////////
#ifndef WIN32
	result = pthread_create (&thread_id, NULL, oam_bands_scan_thread, (void *)&g_lte_oam);
	if (result != 0) 
	{
		printf("create pthread failed,\n");
		return -4;
	}
	pthread_mutex_init(&sync_mutex, NULL);
	pthread_cond_init(&sync_cond,NULL);
#endif
	//send heart beat to OAM UI
	app_oam_heartbeat_req(&g_lte_oam);
	
    return 1;
}


///////////////////////////////////////////////////////////////////////// 
int app_oam_send_msg(uint32_t peer_ip_addr, uint16_t peer_port, uint8_t *buf, uint32_t bufLen)
{
	int ret;
	
	ret = NetConnectUdpSend(peer_ip_addr, peer_port, buf, bufLen);

	return ret;
}

///////////////////////////////////////////////////////////////////////// 
void app_oam_msgHeader_init(lte_oam_t *p_lte_oam, oam_msg_head_t *ptHead, uint8_t op_code)
{
	ptHead->channel = 0x04;
	ptHead->sub_ch    = 0xFF;
	ptHead->op_code = op_code;
	ptHead->ex_code = 0;
}

/////////////////////////////////////////////////////////////////////////
int oam_set_env_band(TModem *ptModem, uint8_t  band) 
{
	int ret_val = RET_FAILED;
	
	switch (band) 
	{
	case 1: //band 1
		ret_val = switchToLteFdd2100(ptModem); 
	    break;
	
	case 3: //band 3
		ret_val = switchToLteFdd1800(ptModem); 
	    break;

    case 38: //band38
		ret_val = switchToLteTdd2600(ptModem); 
	    break;

	case 39: //band39
		ret_val = switchToLteTdd1900(ptModem); 
	    break;
		
	case 40: //band40
		ret_val = switchToLteTdd2300(ptModem); 
	    break;
		
	case 41: //band41
		ret_val = switchToLteTdd2500(ptModem); 
	    break;
	
	default: 
		printf("[band set]unknown band! \n");
	    return ret_val;
	}
	
    if (ret_val != RET_OK) 
	{
		printf("[LTE OAM]set band:%d failed, \n", band);
	}
	
	return ret_val;
}

/////////////////////////////////////////////////////////////////////////
int oam_set_eNB_para(lte_oam_t *p_lte_oam) 
{
	lte_state_t *p_lte_state;
	int ret_val;
//	int access_tech_sel = EUTRAN;  //LTE mode
	
    p_lte_state = (lte_state_t *)(p_lte_oam->p_lte_addr);
	ret_val = cn_operator_lock(&(p_lte_state->tModem), p_lte_oam->cn_oper);
    if (ret_val == RET_OK) {
        //ret_val = set_operator_sel(&(p_lte_state->tModem), p_lte_oam->cn_oper, access_tech_sel);
		ret_val = set_preferred_mode(&(p_lte_state->tModem), LTE_ONLY); //???
		if (ret_val == RET_OK) 
		{
			ret_val = oam_set_env_band(&(p_lte_state->tModem), p_lte_oam->band);
		}
		else 
		{
			printf("[LTE OAM]set LTE mode failed, \n");
		}
	}
	else 
	{
		printf("[LTE OAM]lock CN Operator(%d) failed, \n", p_lte_oam->cn_oper);
	}
	
	return ret_val;
}

/////////////////////////////////////////////////////////////////////////
void oam_set_target_imsi(lte_oam_t *p_lte_oam, oam_target_set_req_t *ptTarget) 
{
	int ret = RET_OK;
	int k;
	uint8_t u8IMSI[8];
	uint8_t hU8, lU8;
	
	for (k = 0; k < 8; k++) 
	{
		lU8 = (ptTarget->u8IMSI[k] & 0xF0) >> 4;
        hU8 = (ptTarget->u8IMSI[k] & 0x0F) << 4;
        u8IMSI[k] = hU8 | lU8;
        //sprintf(&(p_lte_oam->u8IMSI[k]),"%02x", (uint8_t)u8IMSI[k]);
        p_lte_oam->u8IMSI[k] = (uint8_t)u8IMSI[k];
	} 

	/* send response to OAM UI */
	app_oam_cmd_rsp(p_lte_oam, OP_OAM_TARGET_SET_RSP, ret);
}

/////////////////////////////////////////////////////////////////////////
void oam_cell_info_ind(lte_oam_t *p_lte_oam, env_cell_para_t *ptCellPara, uint8_t cell_type) 
{
	oam_cell_info_ind_t tCellInfo = {0};

    app_oam_msgHeader_init(p_lte_oam, &(tCellInfo.msgHeader), OP_OAM_CELL_PARA_IND);
	tCellInfo.msgHeader.sub_ch   = cell_type; //scell, intra-freq ncell, inter-freq ncell
	tCellInfo.msgHeader.msgLen = htons(sizeof(oam_cell_info_ind_t) - sizeof(oam_msg_head_t));
	
	tCellInfo.ecell_para.u16MCC    = htons(ptCellPara->u16MCC);
	tCellInfo.ecell_para.u8Mnc     = ptCellPara->u8Mnc;
	tCellInfo.ecell_para.u16Tac    = htons(ptCellPara->u16Tac);
	tCellInfo.ecell_para.u32CellId = htonl(ptCellPara->u32CellId);
	tCellInfo.ecell_para.u16PCI    = htons(ptCellPara->u16PCI);
	tCellInfo.ecell_para.u8Band    = ptCellPara->u8Band;
	tCellInfo.ecell_para.u16Earfcn = htons(ptCellPara->u16Earfcn);
	tCellInfo.ecell_para.dlbw 	   = ptCellPara->dlbw;
	tCellInfo.ecell_para.ulbw 	   = ptCellPara->ulbw;
	tCellInfo.ecell_para.cell_rsrq = htons(ptCellPara->cell_rsrq);
	tCellInfo.ecell_para.cell_rsrp = htons(ptCellPara->cell_rsrp);
	tCellInfo.ecell_para.cell_rssi = htons(ptCellPara->cell_rssi);
	
	app_oam_send_msg(p_lte_oam->peer_ip_addr, p_lte_oam->peer_port, (uint8_t *)&tCellInfo, sizeof(oam_cell_info_ind_t));
}

/////////////////////////////////////////////////////////////////////////
void get_highPrio_band_byOper(lte_oam_t *p_lte_oam, uint8_t cn_oper) 
{
	oam_opers_list_t * pOList;
	oam_bands_node_t * pBNode;
	
	uint8_t  prio = 0;
	pOList = p_lte_oam->p_opers_list;
    while (pOList != NULL) 
	{
		if (pOList->cn_oper == cn_oper) 
		{  //find
		    pBNode = pOList->ptBand;
		    while (pBNode != NULL) 
		    {
		        if (pBNode->priority > prio) 
				{
				    p_lte_oam->cn_oper = cn_oper;
			        p_lte_oam->band = pBNode->band;
			        prio = pBNode->priority;
				}
                //next node
				pBNode = pBNode->band_next;
		    }
			
			break;
		} 
		
		pOList = pOList->oper_next;
	}	
}

/////////////////////////////////////////////////////////////////////////
int get_highPrio_band_byMode(lte_oam_t *p_lte_oam, uint8_t mode) 
{
	oam_opers_list_t * pOList;
	oam_bands_node_t * pBNode;
	int ret_val = RET_FAILED;
	
	uint8_t  prio = 0;
	pOList = p_lte_oam->p_opers_list;
    while (pOList != NULL) 
	{
		//from current CN operator, select high priority band, 
		if (pOList->cn_oper == p_lte_oam->cn_oper ) 
		{
		    pBNode = pOList->ptBand;
		    while (pBNode != NULL) 
		    {
		        if ((pBNode->workMode == mode) && (pBNode->priority > prio)) 
				{
				    //p_lte_oam->cn_oper = pOList->cn_oper;
			        p_lte_oam->band = pBNode->band;
			        prio = pBNode->priority;
					ret_val = RET_OK;
				}
	            //next node
				pBNode = pBNode->band_next;
			}
		}
		pOList = pOList->oper_next;
	}
	
	return ret_val;
}

/////////////////////////////////////////////////////////////////////////
//+CPSI: LTE-LIMITED,Online,460-01,0x18DB,8718111,313,EUTRAN-BAND3,1650,5,5,-147,-958,-612,30
int get_env_scell_data(char *pData, env_cell_para_t *ptCellPara) 
{
	char *pTmp;
	int mcc, mnc, tac, scell_id, pci;
	int band, earfcn, dlbw, ulbw, rsrq, rsrp, rssi;
	int ret_val = RET_FAILED;

    memset(ptCellPara, 0, sizeof(env_cell_para_t));
    pTmp = pData;
	while (*pTmp != '\0') 
	{
		if ((*pTmp == '-') || (*pTmp == ',')) *pTmp = ' ';
		pTmp++;
	}

	pData = strstr(pData, "460 ");
	if (pData != NULL) 
	{
	    sscanf(pData, "%d %d %x %d %d", 
            &mcc,  /* MCC */
            &mnc,  /* MNC */
            &tac,  /* TAC */
            &scell_id,  /* SCellID */
            &pci  /* PCellID */
            ); 

        pData = strstr(pData, "EUTRAN BAND"); 
		if (pData == NULL) return RET_FAILED;
		pData += strlen("EUTRAN BAND");
		sscanf(pData, "%d %d %d %d %d %d %d", 
            &band,  /* Frequency Band */
            &earfcn,  /* earfcn */
            &dlbw,  /* dlbw */
            &ulbw,  /* ulbw */
            &rsrq,  /* RSRQ */  //'-' is deleted
            &rsrp,  /* RSRP */  //'-' is deleted
            &rssi   /* RSSI */  //'-' is deleted
            );
		
		ptCellPara->u16MCC       = mcc;
		ptCellPara->u8Mnc 	      = mnc;
		ptCellPara->u16Tac 	  = tac;
		ptCellPara->u32CellId    = scell_id;
		ptCellPara->u16PCI 	  = pci;
		ptCellPara->u8Band 	  = band;
		ptCellPara->u16Earfcn    = earfcn;
		ptCellPara->dlbw 	      = dlbw;
		ptCellPara->ulbw 	      = ulbw;
		ptCellPara->cell_rsrq  = (0 - rsrq);
		ptCellPara->cell_rsrp  = (0 - rsrp);
		ptCellPara->cell_rssi  = (0 - rssi);

		ret_val = RET_OK;
	}
	else 
	{
		printf("[LTE OAM]pData is null, \n");
	}
	
	return ret_val;
}

///////////////////////////////////////////////////////////////////////// 
int oam_get_scell_info(lte_oam_t *p_lte_oam)

{
    int ret_val;
	char dataOut[128];
	char *pData = dataOut;
	lte_state_t *p_lte_state;
	env_cell_para_t tCellPara;
	uint8_t cell_type;
	//uint8_t cn_oper;

	p_lte_state = (lte_state_t *)(p_lte_oam->p_lte_addr); 
	ret_val = inquiry_ue_info(&(p_lte_state->tModem), pData, 128); 
	//printf("%s", pData);
	pData = strstr(pData, "+CPSI: LTE");
	if (pData == NULL) 
	{
		return RET_FAILED;
	}
    //pData != NULL
	ret_val = get_env_scell_data(pData, &tCellPara);
	if (ret_val == RET_OK) 
	{
		if ((tCellPara.u16PCI == p_lte_state->lte_cfg.strPbsPara.u16PCI) && 
		(tCellPara.u8Band == p_lte_state->lte_cfg.strPbsPara.u8Band) && 
		(tCellPara.u16Earfcn == p_lte_state->lte_cfg.strPbsPara.u16Earfcn)) {
			cell_type = SCELL; 
		} else {
		    cell_type = ENV_NCELL; 
		}

        //cn_oper = oam_get_oper_byCnType(tCellPara.u16MCC, tCellPara.u8Mnc);
		//if (cn_oper == p_lte_oam->cn_oper) 
		//{
		    oam_cell_info_ind(p_lte_oam, &tCellPara, cell_type); 
		//}
		//else { 
			//printf("CN operator(%d) of env ncell info isn't equal to current value(%d) \n", cn_oper, p_lte_oam->cn_oper); 
		//}
	}

	return ret_val;
}

void oam_lte_cell_report(lte_state_t *lte_state, at_cmgrmi_output_t * pOut) 
{
	env_cell_para_t tCellPara;
	lte_oam_t *p_lte_oam;
	int k,h;

	p_lte_oam = (lte_oam_t *)(lte_state->p_oam_addr);
    //scell
    tCellPara.u16MCC    = pOut->currentSCellInfo.mcc;
	tCellPara.u8Mnc     = pOut->currentSCellInfo.mnc;
	tCellPara.u16Tac    = pOut->currentSCellInfo.tac;
	tCellPara.u32CellId = pOut->currentSCellInfo.cellId;
	tCellPara.u16PCI    = pOut->currentSCellInfo.cell_pci;
	tCellPara.u8Band    = pOut->currentSCellInfo.freq_band_ind;
	tCellPara.u16Earfcn = pOut->currentSCellInfo.earfcn;
	tCellPara.dlbw      = pOut->currentSCellInfo.dl_bandwidth;
	tCellPara.ulbw      = pOut->currentSCellInfo.ul_bandwidth;
	tCellPara.cell_rsrq = pOut->currentSCellInfo.cell_rsrq;
	tCellPara.cell_rsrp = pOut->currentSCellInfo.cell_rsrp;
	tCellPara.cell_rssi = pOut->currentSCellInfo.cell_rssi;	
	oam_cell_info_ind(p_lte_oam, &tCellPara, SCELL);
	
	//intra-freq ncell
	for (k = 1; k < pOut->num_intrafreq_cells; k++) 
	{   //k=0, is scell,so ignore here
		tCellPara.u16PCI    = pOut->intrafreqCell[k].cell_pci;
		tCellPara.cell_rsrq = pOut->intrafreqCell[k].cell_rsrq;
	    tCellPara.cell_rsrp = pOut->intrafreqCell[k].cell_rsrp;
	    tCellPara.cell_rssi = pOut->intrafreqCell[k].cell_rssi;	
		oam_cell_info_ind(p_lte_oam, &tCellPara, INTRA_NCELL);
	}
	
	//inter-freq ncell
	for (k = 0; k < pOut->num_interFreq; k++) 
	{
		tCellPara.u16Earfcn = pOut->interfreqs[k].earfcn;
		for (h = 0; h < pOut->interfreqs[k].num_inter_cells; h++) 
		{
			tCellPara.u16PCI    = pOut->interfreqs[k].interCell[h].cell_pci;
		    tCellPara.cell_rsrq = pOut->interfreqs[k].interCell[h].cell_rsrq;
	        tCellPara.cell_rsrp = pOut->interfreqs[k].interCell[h].cell_rsrp;
	        tCellPara.cell_rssi = pOut->interfreqs[k].interCell[h].cell_rssi;	
		    oam_cell_info_ind(p_lte_oam, &tCellPara, INTER_NCELL);
		}
	}	
}

/////////////////////////////////////////////////////////////////////////
#ifndef WIN32
//OP_OAM_ENV_SCAN_REQ msg is received or timeout, 
int oam_band_cell_scan(int type) 
{
	int ret_val;
	
	pthread_mutex_lock(&sync_mutex);
	if (sync_var<0) 
	{
		ret_val = pthread_cond_signal(&sync_cond);
	    if (ret_val != 0) 
		{
			pthread_mutex_unlock(&sync_mutex);
			printf("[APP OAM]ERROR pthread_cond_signal\n"); 
			ret_val = RET_FAILED;
		}

		sync_var=0;
		scan_type = type;
	}
    pthread_mutex_unlock(&sync_mutex);
	return RET_OK;
}
#endif
/////////////////////////////////////////////////////////////////////////
int oam_env_bands_scan(lte_oam_t *p_lte_oam) 
{
    int ret_val;
	int access_tech_sel = EUTRAN;  //LTE mode
	oam_opers_list_t * pOList;
	oam_bands_node_t * pBNode;
	lte_state_t *p_lte_state;

	p_lte_state = (lte_state_t *)(p_lte_oam->p_lte_addr);
	ret_val = set_preferred_mode(&(p_lte_state->tModem), LTE_ONLY); //???
	if (ret_val != RET_OK) 
	{
		printf("[ENV SCAN]set LTE mode failed, \n");
		return ret_val;
	} 

	pOList = p_lte_oam->p_opers_list;
	while (pOList != NULL) 
	{
	    //ret_val = set_operator_sel(&(p_lte_state->tModem), pOList->cn_oper, access_tech_sel); 
	    ret_val = cn_operator_lock(&(p_lte_state->tModem), pOList->cn_oper);
		pBNode = pOList->ptBand;
		while (pBNode != NULL) 
		{
		    ret_val = oam_set_env_band(&(p_lte_state->tModem), pBNode->band);
			if (ret_val == RET_OK) {
				oss_delay(2000); //2s
		        ret_val = oam_get_scell_info(p_lte_oam); 
			}
			else {
				printf("[ENV SCAN]set band(%d)failed, \n", pBNode->band);
			}
			pBNode = pBNode->band_next;
		}
		
		pOList = pOList->oper_next;
	}
	
	return ret_val;
}

/////////////////////////////////////////////////////////////////////////
int oam_band_check(lte_oam_t *p_lte_oam, uint8_t band) 
{
    oam_opers_list_t * pOList;
	oam_bands_node_t * pBNode;
	
	pOList = p_lte_oam->p_opers_list;
    while (pOList != NULL) 
	{
		if (pOList->cn_oper == p_lte_oam->cn_oper ) 
		{
		    pBNode = pOList->ptBand;
		    while (pBNode != NULL) 
		    {
		        if (pBNode->band == band) 
				{
				    //find, return OK
				    p_lte_oam->band = band;
					return RET_OK;
				}
	            //next node
				pBNode = pBNode->band_next;
			}
		}
		pOList = pOList->oper_next;
	}
	
	return RET_FAILED;
}

/////////////////////////////////////////////////////////////////////////
int oam_set_band_req(lte_oam_t *p_lte_oam, oam_band_set_req_t *ptBandSet) 
{
	int ret_val;
	lte_state_t *p_lte_state;
	uint8_t band;

	p_lte_state = (lte_state_t *)(p_lte_oam->p_lte_addr);
	band = ptBandSet->band;
	
    if (p_lte_oam->band == band) 
	{
	    printf("band is the same as current value, \n");
		ret_val = RET_FAILED;
	}
	else 
	{
	    ret_val = oam_band_check(p_lte_oam, band);
		if (ret_val == RET_OK) 
		{
		    ret_val = oam_set_env_band(&(p_lte_state->tModem), band);
		}
	}
	/* send response to OAM UI */
	app_oam_cmd_rsp(p_lte_oam, OP_OAM_BAND_SET_RSP, ret_val);

	return ret_val;
}

/////////////////////////////////////////////////////////////////////////
int oam_set_oper_req(lte_oam_t *p_lte_oam, oam_oper_set_req_t *ptOperSet)
{
	int ret_val;
	int k;
    uint8_t old_oper = p_lte_oam->cn_oper;
	uint8_t old_band = p_lte_oam->band;
	
    if (ptOperSet->cn_oper == old_oper) 
	{ //not change
		printf("CN operator is same,\n");
		return RET_NO_CONFIG;
	}
	
	for (k=0; k<6; k++) 
	{
		if ((operSwitchList[k].o_oper == old_oper) && 
			(operSwitchList[k].n_oper == ptOperSet->cn_oper)) 
		{
			operSwitchList[k].func(); //exec sim switch action
#ifndef WIN32			
			oss_delay(2000); //2s
#endif
			break;
		}
	}
	
	get_highPrio_band_byOper(p_lte_oam, ptOperSet->cn_oper);
	/* set SIMCOM according to CN-operator and band */
	ret_val = oam_set_eNB_para(p_lte_oam);
	if (ret_val != RET_OK) 
	{ //sim cn-oper or band set failed, restore 
		p_lte_oam->cn_oper = old_oper;
		p_lte_oam->band = old_band;
	}
	
	return ret_val;
}

/////////////////////////////////////////////////////////////////////////
uint8_t oam_get_oper_byCnType(uint16_t mcc, uint8_t  mnc)
{
	uint8_t cn_oper = 0xFF;
	
	if (mcc == 460) 
	{
		switch (mnc) 
		{
		case 0:
		case 2: 
		case 7: 
			cn_oper = CMCC;
			break; 
		case 1: 
		case 6:
			cn_oper = CUCC;
			break; 
		case 3: 
		case 5: 
		case 11: 
			cn_oper = CTCC;
			break; 
		default: 
			printf("[oper set]Unknown CN-Operator! \n");
			break;
		}
	}
	
	return cn_oper;
}

///////////////////////////////////////////////////////////////////////// 
void oam_lte_sniff_info_ind(lte_oam_t *p_lte_oam, lte_para_t *ptLtePara)
{
	oam_sniff_info_ind_t tSniffInfo = {0};

	app_oam_msgHeader_init(p_lte_oam, &tSniffInfo.msgHeader, OP_OAM_SNIFFER_INFO_IND);

	tSniffInfo.msgHeader.msgLen = htons(sizeof(oam_sniff_info_ind_t) - sizeof(oam_msg_head_t));

    tSniffInfo.u8Band 			= ptLtePara->u8Band;
	tSniffInfo.u16Earfcn 		= htons(ptLtePara->u16Earfcn); 
	tSniffInfo.u16PCI 			= htons(ptLtePara->u16PCI); 
	tSniffInfo.u8PdschRefPower 	= ptLtePara->u8PdschRefPower; 
	tSniffInfo.u8Rsrp 			= ptLtePara->u8Rsrp; 
	tSniffInfo.s16FreqOffset 	= htons(ptLtePara->s16FreqOffset); 
	tSniffInfo.u16Tac 			= htons(ptLtePara->u16Tac); 
	tSniffInfo.u32CellId 		= htonl(ptLtePara->u32CellId);

	app_oam_send_msg(p_lte_oam->peer_ip_addr, p_lte_oam->peer_port, (uint8_t *)&tSniffInfo, sizeof(oam_sniff_info_ind_t));

}

/////////////////////////////////////////////////////////////////////////
void oam_lte_imsi_ind(lte_oam_t *p_lte_oam, uint8_t *p_IMSI)
{
	oam_imsi_ind_t tImsiInd = {0};

	app_oam_msgHeader_init(p_lte_oam, &tImsiInd.msgHeader, OP_OAM_IMSI_REPORT_IND);

	tImsiInd.msgHeader.msgLen = htons(sizeof(oam_imsi_ind_t) - sizeof(oam_msg_head_t));

    memcpy(tImsiInd.u8IMSI, p_IMSI, 8); 
	memcpy(tImsiInd.u8ESN, (p_IMSI+8), 8);

	app_oam_send_msg(p_lte_oam->peer_ip_addr, p_lte_oam->peer_port, (uint8_t *)&tImsiInd, sizeof(oam_imsi_ind_t));

}

/////////////////////////////////////////////////////////////////////////
void oam_lte_para_update(lte_state_t *lte_state) 
{
	oam_para_update_ind_t tParaUpdate = {0};
    lte_oam_t *p_lte_oam;

	p_lte_oam = (lte_oam_t *)(lte_state->p_oam_addr);
	app_oam_msgHeader_init(p_lte_oam, &tParaUpdate.msgHeader, OP_OAM_PARA_UPDATE_IND);
	tParaUpdate.msgHeader.msgLen = htons(sizeof(oam_para_update_ind_t) - sizeof(oam_msg_head_t));

    tParaUpdate.u8pwr 			  = lte_state->lte_cfg.strPbsPara.u8pwr;
    tParaUpdate.u8Band 			  = lte_state->lte_cfg.strPbsPara.u8Band;
	tParaUpdate.u16Earfcn 		  = htons(lte_state->lte_cfg.strPbsPara.u16Earfcn); 
	tParaUpdate.u16MCC 			  = htons(lte_state->lte_cfg.strPbsPara.u16MCC); 
	tParaUpdate.u8Mnc 			  = lte_state->lte_cfg.strPbsPara.u8Mnc;
	tParaUpdate.u8InitialValueTag = lte_state->lte_cfg.strPbsPara.u8InitialValueTag;
	tParaUpdate.u16PCI 			  = htons(lte_state->lte_cfg.strPbsPara.u16PCI); 
	tParaUpdate.u16Tac 			  = htons(lte_state->lte_cfg.strPbsPara.u16Tac);
	tParaUpdate.u16CellId 		  = htons(lte_state->lte_cfg.strPbsPara.u16CellId);

	tParaUpdate.u8SynBand 	 = lte_state->lte_cfg.strSynPara.u8Band;
	tParaUpdate.u16SynEarfcn = htons(lte_state->lte_cfg.strSynPara.u16Earfcn);
	tParaUpdate.u16SynPCI 	 = htons(lte_state->lte_cfg.strSynPara.u16PCI);

	app_oam_send_msg(p_lte_oam->peer_ip_addr, p_lte_oam->peer_port, (uint8_t *)&tParaUpdate, sizeof(oam_para_update_ind_t));
}

/////////////////////////////////////////////////////////////////////////
void oam_lte_mode_ind(lte_oam_t *p_lte_oam, uint8_t mode)
{
	oam_lte_mode_ind_t tModeInd = {0};

	app_oam_msgHeader_init(p_lte_oam, &tModeInd.msgHeader, OP_OAM_LTE_MODE_IND);
	tModeInd.msgHeader.msgLen = htons(sizeof(oam_lte_mode_ind_t) - sizeof(oam_msg_head_t));

    tModeInd.u8WorkMode = mode;

	app_oam_send_msg(p_lte_oam->peer_ip_addr, p_lte_oam->peer_port, (uint8_t *)&tModeInd, sizeof(oam_lte_mode_ind_t));

}

/////////////////////////////////////////////////////////////////////////
void oam_lte_err_ind(lte_oam_t *p_lte_oam, uint8_t errNo)
{
	oam_err_ind_t tErrInd = {0};

	app_oam_msgHeader_init(p_lte_oam, &tErrInd.msgHeader, OP_OAM_LTE_ERR_IND);
	tErrInd.msgHeader.msgLen = htons(sizeof(oam_err_ind_t) - sizeof(oam_msg_head_t));

    tErrInd.errNo = errNo;

	app_oam_send_msg(p_lte_oam->peer_ip_addr, p_lte_oam->peer_port, (uint8_t *)&tErrInd, sizeof(oam_err_ind_t));

}

///////////////////////////////////////////////////////////////////////// 
void app_oam_heartbeat_req(lte_oam_t *p_lte_oam)
{
	lte_state_t * p_lte_state;
	oam_heartbeat_req_t tHeartbeat;

	p_lte_state = (lte_state_t *)(p_lte_oam->p_lte_addr);
    memset(&tHeartbeat, 0, sizeof(oam_heartbeat_req_t));
	app_oam_msgHeader_init(p_lte_oam, &tHeartbeat.msgHeader, OP_OAM_HEARBEAT_REQ);

	tHeartbeat.msgHeader.msgLen = htons(sizeof(oam_heartbeat_req_t) - sizeof(oam_msg_head_t));
    tHeartbeat.local_state = p_lte_state->state; 
	tHeartbeat.bts_state = p_lte_state->remote_state; 
	// setup time out proc
	oss_set_timer_ex(O_APP_HEARTBEAT_TIMER, oam_timeout_msg_dispatch, EVENT_OAM_HEARTBEAT_TIMEOUT, (uint32_t)p_lte_oam);

	app_oam_send_msg(p_lte_oam->peer_ip_addr, p_lte_oam->peer_port, (uint8_t *)&tHeartbeat, sizeof(oam_heartbeat_req_t));

	p_lte_oam->heartbeat_lost++;
}

///////////////////////////////////////////////////////////////////////////// 
void app_oam_heartbeat_rsp(lte_oam_t *p_lte_oam, oam_heartbeat_rsp_t *ptHBRsp)
{
	//printf("recv HeartBeat from APP\n");
}

/////////////////////////////////////////////////////////////////////////////  
int app_oam_eNB_para_req(lte_oam_t *p_lte_oam, oam_eNB_para_req_t *ptParaReq)
{
	int 	 ret = RET_FAILED;
    uint16_t mcc;	
	uint8_t  mnc;
	uint8_t  cn_oper;
	uint8_t  band;

	mcc = ntohs(ptParaReq->u16MCC);
	mnc = ptParaReq->u8Mnc;
	band = ptParaReq->band;

    /* get CN-operator from (mcc,mnc) */
    cn_oper = oam_get_oper_byCnType(mcc, mnc);
    if (cn_oper != 0xFF) p_lte_oam->cn_oper = cn_oper;  //(cn_oper) should be the same as (p_lte_oam->cn_oper)
	
    /* get band value */
    if (band == 0xFF) //invalid 
	{
		get_highPrio_band_byOper(p_lte_oam, p_lte_oam->cn_oper);		
	} 
	else 
	{
		p_lte_oam->band = band;
	}

    ret = oam_band_check(p_lte_oam, band);
	if (ret == RET_OK) {
	    /* band check valid, set SIMCOM according to CN-operator and band */
	    ret = oam_set_eNB_para(p_lte_oam); 
	}

	return ret;
}

/////////////////////////////////////////////////////////////////////////
void app_oam_cmd_rsp(lte_oam_t *p_lte_oam, uint8_t op_code, int flag) 
{
    oam_cmd_rsp_t tCmdRsp;

	app_oam_msgHeader_init(p_lte_oam, &tCmdRsp.msgHeader, op_code);
	tCmdRsp.msgHeader.msgLen = htons(sizeof(oam_cmd_rsp_t) - sizeof(oam_msg_head_t));
	
	if (flag == RET_OK) 
	{  
	    tCmdRsp.flag = 0; //accept
	}
	else 
	{   
		tCmdRsp.flag = 1; //refuse
	}
	
	app_oam_send_msg(p_lte_oam->peer_ip_addr, p_lte_oam->peer_port, (uint8_t *)&tCmdRsp, sizeof(oam_cmd_rsp_t));
}

///////////////////////////////////////////////////////////////////////// 
void oam_env_ver_ind(lte_oam_t *p_lte_oam) 
{
	lte_state_t * p_lte_state = (lte_state_t *)(p_lte_oam->p_lte_addr);
	oam_env_ver_ind_t tVerInd = {0};

	app_oam_msgHeader_init(p_lte_oam, &tVerInd.msgHeader, OP_OAM_ENV_VER_IND);
	tVerInd.msgHeader.msgLen = htons(sizeof(oam_env_ver_ind_t) - sizeof(oam_msg_head_t)); 

	memcpy(tVerInd.abyVer, p_lte_state->tModem.abyVer, 32); 
	app_oam_send_msg(p_lte_oam->peer_ip_addr, p_lte_oam->peer_port, (uint8_t *)&tVerInd, sizeof(oam_env_ver_ind_t));
}
///////////////////////////////////////////////////////////////////////// 
void app_oam_msg_proc(lte_oam_t *p_lte_oam, char *buf, int len) 
{
	oam_heartbeat_rsp_t   * ptHeartBeatRsp;
	oam_eNB_para_req_t    * pteNBParaReq;
	oam_start_req_t       * ptStartReq;
	oam_power_set_req_t   * ptPowerSetReq;
	oam_target_set_req_t  * ptTargetSetReq;
	oam_band_set_req_t    * ptBandSetReq;
	oam_oper_set_req_t    * ptOperSetReq;
	oam_mode_set_req_t    * ptModeSetReq;
	oam_env_scan_req_t    * ptEnvScanReq;
	oam_cells_para_req_t  * ptCellParaReq;
	lte_state_t * p_lte_state = (lte_state_t *)(p_lte_oam->p_lte_addr);
    oam_msg_head_t *ptMsg = (oam_msg_head_t *)buf;
	int ret = RET_OK;
	

	//printf("APP OP_CODE: %d \n", ptMsg->op_code);

	switch(ptMsg->op_code)
	{
	case OP_OAM_HEARBEAT_RSP:
		ptHeartBeatRsp = (oam_heartbeat_rsp_t *)ptMsg;
		app_oam_heartbeat_rsp(p_lte_oam, ptHeartBeatRsp);
	    break;
	
	case OP_OAM_PARA_SET_REQ:
		pteNBParaReq = (oam_eNB_para_req_t *)ptMsg;
		ret = app_oam_eNB_para_req(p_lte_oam, pteNBParaReq);
		if (ret == RET_OK) 
		{ //start timer, when it expiry, read data from Uart port
		    oss_set_timer_ex(O_LTE_CELL_GET_TIMER, lte_msg_proc, EVENT_CELL_GET_TIMEOUT, (uint32_t)p_lte_state);
		}
		app_oam_cmd_rsp(p_lte_oam, OP_OAM_PARA_SET_RSP, ret);
	    break;

	case OP_OAM_LTE_START_REQ:
		ptStartReq = (oam_start_req_t *)ptMsg;
		if (!(ptStartReq->flag) || 
			((ptStartReq->flag) && (p_lte_state->state == LTE_SYS_STAT_DATAREADY))) 
		{  //if (1)it's stop cmd, or (2)it's start cmd and state is 2, send start cmd to LTE board
		    lte_oam_start(p_lte_state, (uint16_t)ptStartReq->flag); 
		}
		else 
		{   //it's start cmd, but state isn't 2, save it
			p_lte_state->startReq_send = 1;
		}
	    break;

    case OP_OAM_LTE_SNIFFER_REQ:
		lte_oam_sniffer(p_lte_state);
	    break;
		
	case OP_OAM_LTE_RESET_REQ:
		lte_oam_reset(p_lte_state);
	    break;

	case OP_OAM_POWER_SET_REQ:
		ptPowerSetReq = (oam_power_set_req_t *)ptMsg;
		if (ptPowerSetReq->powerValue > POWER_MAX) 
		{
			ptPowerSetReq->powerValue = POWER_MAX;
		}
		else if (ptPowerSetReq->powerValue < POWER_MIN)
		{
			ptPowerSetReq->powerValue = POWER_MIN;
		}
		lte_oam_set_power(p_lte_state, (uint16_t)ptPowerSetReq->powerValue);
	    break;
		
	case OP_OAM_TARGET_SET_REQ:
		ptTargetSetReq = (oam_target_set_req_t *)ptMsg;
		oam_set_target_imsi(p_lte_oam, ptTargetSetReq);
		p_lte_state->target_set = 1;
	    break;

	case OP_OAM_TARGET_CANCEL_REQ:
		memset(p_lte_oam->u8IMSI, 0xFF, 8);
		p_lte_state->target_set = 0;
		/* send response to OAM UI */
		app_oam_cmd_rsp(p_lte_oam, OP_OAM_TARGET_CANCEL_RSP, ret);
	    break;

    case OP_OAM_DRIVE_TO_GSM_REQ:
		p_lte_state->drive_to_gsm = 1;
		/* send response to OAM UI */
		app_oam_cmd_rsp(p_lte_oam, OP_OAM_DRIVE_TO_GSM_RSP, ret);
		break;
		
	case OP_OAM_BAND_SET_REQ:
		ptBandSetReq = (oam_band_set_req_t *)ptMsg;
		ret = oam_set_band_req(p_lte_oam, ptBandSetReq);
		if (ret == RET_OK) 
		{
		    p_lte_oam->band = ptBandSetReq->band; //update current band
		    lte_oam_reset(p_lte_state);  //reset LTE board to update its cell parameters,  
		}
		break;

	case OP_OAM_OPER_SET_REQ:
		ptOperSetReq = (oam_oper_set_req_t *)ptMsg;
		ret = oam_set_oper_req(p_lte_oam, ptOperSetReq);
		if (ret == RET_OK) 
		{
			lte_oam_reset(p_lte_state);  //reset LTE board to update its cell parameters,
		} 
		/* send response to OAM UI */
	    app_oam_cmd_rsp(p_lte_oam, OP_OAM_OPER_SET_RSP, ret);
		break;
		
	case OP_OAM_MODE_SET_REQ:
		ptModeSetReq = (oam_mode_set_req_t *)ptMsg;
		printf("recv MODE SET REQ from APP, flag:%d\n", ptModeSetReq->out_of_sync);
		if (!(ptModeSetReq->out_of_sync)) 
		{  //sync
			if (p_lte_state->tddfdd_mode) 
			{ //now it's TDD, change to FDD
			    ret = get_highPrio_band_byMode(p_lte_oam, FDD);
			} 
			else  
			{
				ret = get_highPrio_band_byMode(p_lte_oam, TDD);
			}
	        //for CHINA MOBILE, TDD->FDD isn't allowed,  'ret' is RET_FAILED
			if (ret == RET_OK) {
			    ret = oam_set_eNB_para(p_lte_oam); 
			}
			
			if (ret == RET_OK) {
				lte_oam_reset(p_lte_state);  //reset LTE board to update its cell parameters,
				//mode switch
				if (p_lte_state->tddfdd_mode) 
				{ //now it's TDD, 
				    TDDtoFDD();
				}
				else 
				{ //now it's FDD, 
					FDDtoTDD();
				}
			} 
		}
        else 
		{ //non-sync
	        if (p_lte_state->tddfdd_mode) 
			{ //From scell data, it's TDD, but HW is FDD, 
			    FDDtoTDD();
			}
			else 
			{ //From scell data, it's FDD, but HW is TDD, so switch to FDD
				TDDtoFDD();
			} 
		}
		/* send response to OAM UI */
        app_oam_cmd_rsp(p_lte_oam, OP_OAM_MODE_SET_RSP, ret);
		break;
		
	case OP_OAM_LTE_LOCATION_REQ:
		p_lte_state->drive_to_gsm = 0; //locate in LTE mode
		/* send response to OAM UI */
		app_oam_cmd_rsp(p_lte_oam, OP_OAM_LTE_LOCATION_RSP, ret);
		break;
		
	case OP_OAM_ENV_SCAN_REQ:
		ptEnvScanReq = (oam_env_scan_req_t *)ptMsg;
		if (ntohs(ptEnvScanReq->time_duration))
		{
		    p_lte_oam->env_scan_interval = ntohs(ptEnvScanReq->time_duration);
		    oss_set_timer_ex(1000*(p_lte_oam->env_scan_interval), oam_timeout_msg_dispatch, EVENT_OAM_ENV_SCAN_TIMEOUT, (uint32_t)p_lte_oam);
		}
//#ifdef WIN32		
		ret = oam_env_bands_scan(p_lte_oam);
//#else
		//long time to handle it,so put it into thread
//		ret = oam_band_cell_scan(ENV_BANDS_SCAN);
//#endif
		app_oam_cmd_rsp(p_lte_oam, OP_OAM_ENV_SCAN_RSP, ret);
		break;
		
	case OP_OAM_CELLSPARA_SCAN_REQ:
		ptCellParaReq = (oam_cells_para_req_t *)ptMsg;
		
		if (ntohs(ptCellParaReq->interval)) 
		{
			p_lte_oam->cells_para_scan_interval = ntohs(ptCellParaReq->interval);
			oss_set_timer_ex(1000*(p_lte_oam->cells_para_scan_interval), oam_timeout_msg_dispatch, EVENT_OAM_CELLSPARA_SCAN_TIMEOUT, (uint32_t)p_lte_oam);
		}
#ifdef WIN32	
        lte_cells_para_acquire(p_lte_state);
#else 
		oam_band_cell_scan(SERV_CELL_SCAN);
#endif
		break;

	case OP_OAM_ENV_VER_REQ: 
		oam_env_ver_ind(p_lte_oam);
		break;
		
	default:
		printf("[APP_OAM]Unknown OAM msg, op_code =%d \n", ptMsg->op_code);
	    break;
	}
}

/////////////////////////////////////////////////////////////////////////
void oam_timeout_msg_proc(lte_oam_t *p_lte_oam, int8_t *buf, int32_t len) 
{
    oss_msg_head_t *ptMsg    = (oss_msg_head_t *)buf;
    lte_state_t * p_lte_state = (lte_state_t *)(p_lte_oam->p_lte_addr);
	//printf("OAM timer[0x%x] expiry, \n", ntohs(ptMsg->msg_type));

	switch(ntohs(ptMsg->msg_type))
	{
	case EVENT_OAM_HEARTBEAT_TIMEOUT:
		app_oam_heartbeat_req(p_lte_oam);
		break;
		
	case EVENT_OAM_ENV_SCAN_TIMEOUT:
#ifdef WIN32		
		oam_env_bands_scan(p_lte_oam);
#else
		oam_band_cell_scan(ENV_BANDS_SCAN);
#endif
		oss_set_timer_ex(1000*(p_lte_oam->env_scan_interval), 
		                 oam_timeout_msg_dispatch, EVENT_OAM_ENV_SCAN_TIMEOUT, (uint32_t)p_lte_oam);
		break;
		
	case EVENT_OAM_CELLSPARA_SCAN_TIMEOUT:
#ifdef WIN32		
		lte_cells_para_acquire(p_lte_state);
#else 
        oam_band_cell_scan(SERV_CELL_SCAN);
#endif
        //adjust time span to let ENV SCAN has a chance to run, 
		oss_set_timer_ex(1000*(p_lte_oam->cells_para_scan_interval + p_lte_oam->time_span[p_lte_oam->index++]), 
		                 oam_timeout_msg_dispatch, EVENT_OAM_CELLSPARA_SCAN_TIMEOUT, (uint32_t)p_lte_oam);
        if (p_lte_oam->index == TIME_STEP_NUM) { 
			p_lte_oam->index = 0; 
		}
		break;
		
	default:
		break;
	}
}

///////////////////////////////////////////////////////////////////////////// 
void oam_timeout_msg_dispatch(char *buf, int len)
{
	oss_msg_head_t *msg_head = (oss_msg_head_t *)buf;
	msg_timeout_t  *msg_timeout;

	if(((msg_head->msg_type) >= EVENT_OAM_LTE_START) && 
		((msg_head->msg_type) <= EVENT_OAM_LTE_STOP))
	{
		msg_timeout = (msg_timeout_t *)(msg_head + 1);
		msg_head->msg_type = htons(msg_head->msg_type);
		oam_timeout_msg_proc((lte_oam_t *)msg_timeout->dwData, buf, len);
	}
	else
	{
	;
	}
}

///////////////////////////////////////////////////////////////////////////// 
void oam_lte_set_power_rsp(lte_oam_t *p_lte_oam, uint8_t u8PowerInd) 
{
    oam_cmd_rsp_t tCmdRsp;

	app_oam_msgHeader_init(p_lte_oam, &tCmdRsp.msgHeader, OP_OAM_POWER_SET_RSP);
	tCmdRsp.msgHeader.msgLen = htons(sizeof(oam_cmd_rsp_t) - sizeof(oam_msg_head_t));
	
	if (!u8PowerInd) 
	{  //refuse
	    tCmdRsp.flag = 1;
	}
	else 
	{
		tCmdRsp.flag = 0; //accept
	}
	
	app_oam_send_msg(p_lte_oam->peer_ip_addr, p_lte_oam->peer_port, (uint8_t *)&tCmdRsp, sizeof(oam_cmd_rsp_t));
}

///////////////////////////////////////////////////////////////////////////// 
void oam_lte_start_rsp(lte_oam_t *p_lte_oam, uint8_t u8StartInd) 
{
    oam_cmd_rsp_t tCmdRsp;

	app_oam_msgHeader_init(p_lte_oam, &tCmdRsp.msgHeader, OP_OAM_LTE_START_RSP);
	tCmdRsp.msgHeader.msgLen = htons(sizeof(oam_cmd_rsp_t) - sizeof(oam_msg_head_t));
	
	if (!u8StartInd) 
	{  //refuse
	    tCmdRsp.flag = 1;
	}
	else 
	{
		tCmdRsp.flag = 0; //accept
	}
	
	app_oam_send_msg(p_lte_oam->peer_ip_addr, p_lte_oam->peer_port, (uint8_t *)&tCmdRsp, sizeof(oam_cmd_rsp_t));
}

/////////////////////////////////////////////////////////////////////////////
void oam_lte_sniffer_rsp(lte_oam_t *p_lte_oam) 
{
    oam_cmd_rsp_t tCmdRsp;

	app_oam_msgHeader_init(p_lte_oam, &tCmdRsp.msgHeader, OP_OAM_LTE_SNIFFER_RSP);
	tCmdRsp.msgHeader.msgLen = htons(sizeof(oam_cmd_rsp_t) - sizeof(oam_msg_head_t));
	
    tCmdRsp.flag = 0; //accept
	
	app_oam_send_msg(p_lte_oam->peer_ip_addr, p_lte_oam->peer_port, (uint8_t *)&tCmdRsp, sizeof(oam_cmd_rsp_t));
}

///////////////////////////////////////////////////////////////////////////// 
void oam_lte_reset_rsp(lte_oam_t *p_lte_oam, uint8_t u8ResetInd) 
{
    oam_cmd_rsp_t tResetRsp;

	app_oam_msgHeader_init(p_lte_oam, &tResetRsp.msgHeader, OP_OAM_LTE_RESET_RSP);
	tResetRsp.msgHeader.msgLen = htons(sizeof(oam_cmd_rsp_t) - sizeof(oam_msg_head_t));
	
	if (!u8ResetInd) 
	{  //refuse
	    tResetRsp.flag = 1;
	}
	else 
	{
		tResetRsp.flag = 0; //accept
	}
	
	app_oam_send_msg(p_lte_oam->peer_ip_addr, p_lte_oam->peer_port, (uint8_t *)&tResetRsp, sizeof(oam_cmd_rsp_t));
#ifndef WIN32	
	if (u8ResetInd == 1) 
	{ //controller board reboot 
		//sys_reboot();
	}
#endif	
}

///////////////////////////////////////////////////////////////////////////// 
#ifndef WIN32
int oam_rf_switch_req(void) 
{
	oss_gpio_t gpio;
    gpio.number = 4;
    gpio.flags = OSS_GPIO_OUT;
    oss_gpio_switch_set(&gpio, 1);

	return 1;
}
#endif

