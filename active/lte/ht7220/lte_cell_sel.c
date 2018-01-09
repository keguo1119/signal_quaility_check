/**************************************************
*
*serial.c - defines file 
*
***************************************************/
#include "oss.h"
#include "oss_time.h"
#include "config.h"
#include "protocol_common.h"
#include "modem.h"
#include "lte_cfg.h"
#include "lte_sm.h"
#include "lte_cell_sel.h"
#include "lte_oam.h" 
/////////////////////////////////////////////////////////////////////////////

/* xzluo, 2015-12-16 */ 
extern int gFp;
extern void CfgLock(void);
extern void CfgUnlock(void);

at_cmgrmi_output_t atCmgrmiOut; 
at_cmgrmi_output_t * pAtCmgrmiOut;

/////////////////////////////////////////////////////////////////////////////
// local functions 
static char* join(char *s1, char *s2)  
{  
    char *c1 = malloc(strlen(s1)+strlen(s2)+1);
    if (c1 == NULL) exit (1);  
  
    strcpy(c1, s1);  
    strcat(c1, s2);  
  
    return c1;  
}

/////////////////////////////////////////////////////////////////////////////
// sort by RSRP
static void Dir_Insert_Rsrp(lte_cell_t A[], int N)    
{  
	int i, j;
	lte_cell_t t;
	for(i=1; i<N; i++)  
	{  
		t = A[i];  
		j = i-1;  
		while((j >= 0) && (A[j].cell_rsrp > t.cell_rsrp))  
		{  
			A[j+1] = A[j];  
			j--;  
		}  
		A[j+1] = t;  
	}  
}

/////////////////////////////////////////////////////////////////////////////
// sort by Srxlev
static void Dir_Insert_Srxlev(lte_cell_t A[], int N)    
{  
	int i, j;
	lte_cell_t t;
	for(i=1; i<N; i++)  
	{  
		t = A[i];  
		j = i-1;  
		while((j >= 0) && (A[j].cell_idle_srxlev > t.cell_idle_srxlev))  
		{  
			A[j+1] = A[j];  
			j--;  
		}  
		A[j+1] = t;  
	}  
}

/////////////////////////////////////////////////////////////////////////////
// sort by threshXHigh
static void Dir_Insert_threshXHigh(inter_freqs_t A[], int N)    
{  
	int i, j;
	inter_freqs_t t;
	for(i=1; i<N; i++)  
	{  
		t = A[i];  
		j = i-1;  
		while((j >= 0) && (A[j].threshX_high > t.threshX_high))  
		{  
			A[j+1] = A[j];  
			j--;  
		}  
		A[j+1] = t;  
	}  
}

/////////////////////////////////////////////////////////////////////////////
//+CPSI: LTE-LIMITED,Online,460-11,0x5A22,94472450,205,EUTRAN-BAND1,75,4,4,-200,-1004,-608,29
//75 -> 
//0 EUTRAN_BAND1(UL:1920-1980; DL:2110-2170)
int switchToLteFdd2100(TModem *ptModem) 
{
	char strTmp[16];
	char cmResp[] = "OK";
	char cmd[48];
    int tmp; 
	int ret;
	
    memset(strTmp, 0, 16);
	memset(cmd, 0, 48);
	tmp = (1<<0);
    sprintf(strTmp, "%08x", tmp);
    sprintf(cmd, "%s%s%s\r","AT+CNBP=0x0002000000500380,", "0x00000000", strTmp); 
	//BAND 1: AT+CNBP=0x0002000000500380,0x0000000000000001
	ret = modem_atCmd_w_r(ptModem, cmd, cmResp, NULL);
	if(ret != RET_OK)
	{
	    Log(OS_LOG_ERR, "set LTE FDD 2100 failed", "");
		return RET_FAILED;
	}

	return RET_OK;
}  
/////////////////////////////////////////////////////////////////////////////
//+CPSI: LTE-LIMITED,Online,460-00,0x1877,25435138,305,EUTRAN-BAND41,40540,5,5,-164,-923,-560,31
//40540 -> 2585
//40 EUTRAN_BAND41(UL: 2496-2690; DL: 2496-2690)
int switchToLteTdd2500(TModem *ptModem) 
{
	char strTmp[16];
	char cmResp[] = "OK";
	char cmd[48];
    int tmp; 
	int ret;
	
    memset(strTmp, 0, 16);
	memset(cmd, 0, 48);
	tmp = (1<<(40-32));
    sprintf(strTmp, "%08x", tmp);
    sprintf(cmd, "%s%s%s\r","AT+CNBP=0x0002000000500380,0x", strTmp,"00000000"); 
	//AT+CNBP=0x0002000000500380,0x0000010000000000
	ret = modem_atCmd_w_r(ptModem, cmd, cmResp, NULL);
	if(ret != RET_OK)
	{
	    Log(OS_LOG_ERR, "set LTE TDD 2600 failed", "");
		return RET_FAILED;
	}
	
	return RET_OK;
}

/////////////////////////////////////////////////////////////////////////////
//+CPSI: LTE-LIMITED,Online,460-00,0x1877,29946145,230,EUTRAN-BAND40,38950,5,5,-86,-1095,-809,14
//38950 -> 
//39 EUTRAN_BAND40(UL: 2300-2400; DL: 2300-2400)
int switchToLteTdd2300(TModem *ptModem) 
{
	char strTmp[16];
	char cmResp[] = "OK";
	char cmd[48];
    int tmp; 
	int ret;
	
    memset(strTmp, 0, 16);
	memset(cmd, 0, 48);
	tmp = (1<<(39-32));
    sprintf(strTmp, "%08x", tmp);
    sprintf(cmd, "%s%s%s\r","AT+CNBP=0x0002000000500380,0x", strTmp,"00000000"); 
	//AT+CNBP=0x0002000000500380,0x0000008000000000
	ret = modem_atCmd_w_r(ptModem, cmd, cmResp, NULL);
	if(ret != RET_OK)
	{
	    Log(OS_LOG_ERR, "set LTE TDD 2300 failed", "");
		return RET_FAILED;
	}
	
	return RET_OK;
}

/////////////////////////////////////////////////////////////////////////////
//+CPSI: LTE-LIMITED,Online,460-00,0x1877,26418189,18,EUTRAN-BAND39,38400,5,5,-136,-916,-581,32
//38400 -> 
//38 EUTRAN_BAND39(UL: 1880-1920; DL: 1880-1920)
int switchToLteTdd1900(TModem *ptModem) 
{
	char strTmp[16];
	char cmResp[] = "OK";
	char cmd[48];
    int tmp; 
	int ret;
	
    memset(strTmp, 0, 16);
	memset(cmd, 0, 48);
	tmp = (1<<(38-32));
    sprintf(strTmp, "%08x", tmp);
    sprintf(cmd, "%s%s%s\r","AT+CNBP=0x0002000000500380,0x", strTmp,"00000000"); 
	//AT+CNBP=0x0002000000500380,0x0000004000000000 
	ret = modem_atCmd_w_r(ptModem, cmd, cmResp, NULL);
	if(ret != RET_OK)
	{
	    Log(OS_LOG_ERR, "set LTE TDD 1900 failed", "");
		return RET_FAILED;
	}
	
	return RET_OK;
}

/////////////////////////////////////////////////////////////////////////////
//+CPSI: LTE-LIMITED,Online,460-00,0x1877,26484998,164,EUTRAN-BAND38,38098,5,5,-92,-854,-563,38
//38098 -> 
//37 EUTRAN_BAND38(UL: 2570-2620; DL: 2570-2620)
int switchToLteTdd2600(TModem *ptModem) 
{
	char strTmp[16];
	char cmResp[] = "OK";
	char cmd[48];
    int tmp; 
	int ret;
	
    memset(strTmp, 0, 16);
	memset(cmd, 0, 48);
	tmp = (1<<(37-32));
    sprintf(strTmp, "%08x", tmp);
    sprintf(cmd, "%s%s%s\r","AT+CNBP=0x0002000000500380,0x", strTmp,"00000000"); 
	//AT+CNBP=0x0002000000500380,0x0000002000000000 
	ret = modem_atCmd_w_r(ptModem, cmd, cmResp, NULL);
	if(ret != RET_OK)
	{
	    Log(OS_LOG_ERR, "set LTE TDD 1900 failed", "");
		return RET_FAILED;
	}
	
	return RET_OK;
}

/////////////////////////////////////////////////////////////////////////////
//+CPSI: LTE-LIMITED,Online,460-11,0x5A22,94778929,294,EUTRAN-BAND3,1825,4,4,-110,-937,-635,36
//+CPSI: LTE-LIMITED,Online,460-01,0x18DB,8718111,313,EUTRAN-BAND3,1650,5,5,-147,-958,-612,30
//1825 -> 
// 2 EUTRAN_BAND3(UL:1710-1785; DL:1805-1880) 
int switchToLteFdd1800(TModem *ptModem) 
{
	char strTmp[16];
	char cmResp[] = "OK";
	char cmd[48];
    int tmp; 
	int ret;
	
    memset(strTmp, 0, 16);
	memset(cmd, 0, 48);
	tmp = (1<<2);
    sprintf(strTmp, "%08x", tmp);
    sprintf(cmd, "%s%s%s\r","AT+CNBP=0x0002000000500380,", "0x00000000", strTmp); 
	//AT+CNBP=0x0002000000500380,0x0000000000000004
	ret = modem_atCmd_w_r(ptModem, cmd, cmResp, NULL);
	if(ret != RET_OK)
	{
	    Log(OS_LOG_ERR, "set LTE FDD 1800 failed", "");
		return RET_FAILED;
	}

	return RET_OK;
}  

/////////////////////////////////////////////////////////////////////////////
// 
int set_preferred_mode(TModem *ptModem, int preferred_mode) 
{
	char cmResp[] = "OK";
    char cmd[16];
	int ret;

    memset(cmd, 0, 16); 	
	sprintf(cmd, "AT+CNMP=%d\r", preferred_mode);
	ret = modem_atCmd_w_r(ptModem, cmd, cmResp, NULL);
	
	return ret;
}

/////////////////////////////////////////////////////////////////////////////
// 
int cn_operator_lock(TModem *ptModem, int oper) 
{
	char cmResp[] = "OK";
    char cmd[48];
	int ret;

    memset(cmd, 0, 48);
	switch(oper) 
	{
	case CMCC: //china mobile com
	    sprintf(cmd, "%s\r","AT+CMSSN=46000");
	    break;
	case CUCC: //china unicom
	    sprintf(cmd, "%s\r","AT+CMSSN=46001");
	    break;
	case CTCC: //china telcom
	    sprintf(cmd, "%s\r","AT+CMSSN=46011");
	    break;
	default:
		return RET_FAILED;
	}
	
	ret = modem_atCmd_w_r(ptModem, cmd, cmResp, NULL);

	return ret;
}

/////////////////////////////////////////////////////////////////////////////
// 
int set_operator_sel(TModem *ptModem, int oper, int mode) 
{
	char cmResp[] = "OK";
    char cmd[48];
	char strOper[16];
	int ret;

    memset(cmd, 0, 48);
	switch(oper) 
	{
	case CMCC: //china mobile com
	    sprintf(strOper, "%s","CMCC");
	    break;
	case CUCC: //china unicom
	    sprintf(strOper, "%s","UNICOM");
	    break;
	case CTCC: //china telcom
	    sprintf(strOper, "%s","CTCC");
	    break;
	default:
		return RET_FAILED;
	}
	
    //sprintf(cmd, "%s\r","AT+COPS=0,1,\"UNICOM\",0");
    sprintf(cmd, "%s,\"%s\",%d\r","AT+COPS=0,1",strOper,mode);
	ret = modem_atCmd_w_r(ptModem, cmd, cmResp, NULL);

	return ret;
}
/////////////////////////////////////////////////////////////////////////////
// AT+CPSI?
//+CPSI: LTE-LIMITED,Online,460-01,0x18DB,8718111,313,EUTRAN-BAND3,1650,5,5,-147,-958,-612,30
//+CPSI: LTE,Online,460-01,0x18D9,8752395,398,EUTRAN-BAND3,1650,5,5,-114,-904,-597,35
int inquiry_ue_info(TModem *ptModem, char *pData, u16 bySize) 
{
	char cmResp[] = "OK";
    char cmd[48];
	int  ret;

	memset(cmd, 0, 48);
    sprintf(cmd, "%s\r","AT+CPSI?");
	memset(pData, 0x00, bySize);
	ret = modem_atCmd_w_r(ptModem, cmd, cmResp, pData);

	if(ret != RET_OK)
	{
	    printf("\'AT+CPSI\' get data failed, \n");
	} 

    return ret;
}
/////////////////////////////////////////////////////////////////////////////
// AT+CPSI?
// +CPSI: GSM-LIMITED,Online,460-00,0x1877,57777,19 EGSM 900,-57,0,46-156
// +CPSI: GSM LIMITED Online 460 00 0x1877 57777 19 EGSM 900 57 0 46 156
// pData: 460 00 0x1877 57777 19 EGSM 900 57 0 46 156
int get_rrc_conn_rel_para(TModem *ptModem, uint16_t *pArfcn, uint8_t cn_oper) 
{
	char dataOut[1024];
	char *pData = dataOut;
	gsm_cell_t gsmCell;
	u32 tmp1, tmp2, tmp3, tmp4, tmp5; 
	int ret;

    if (cn_oper == 0xFF || cn_oper == CTCC) 
	{
		printf("[GSM mode]CN operator(%d) is unsupported,\n", cn_oper);
		return RET_FAILED;
	}
	
	//set_operator_sel(ptModem, cn_oper, 0);
	cn_operator_lock(ptModem, cn_oper);
    set_preferred_mode(ptModem, GSM_ONLY);
    oss_delay(1000); //1s
    ret = inquiry_ue_info(ptModem, pData, 1024);    
	set_preferred_mode(ptModem, LTE_ONLY); //restore LTE mode
    if (ret != RET_OK) 
	{
		return RET_FAILED;
	}
	
    while (*pData != '\0') 
	{
		if ((*pData == '-') || (*pData == ',')) *pData = ' ';
		pData++;
	}

	pData = dataOut;
	pData = strstr(pData, "+CPSI: GSM ");
	if (pData != NULL) 
	{
	    //pData += strlen("+CPSI: GSM ");
	    //+CPSI: GSM LIMITED Online 460 00 0x1877 57441 572 DCS 1800 57 0 43 153
	    pData = strstr(pData, "460 ");
		if (pData == NULL) return RET_FAILED;
	    sscanf(pData, "%d %d %x %d %d", 
            &tmp1,  /* mcc */
            &tmp2,  /* mnc */
            &tmp3,  /* lac */
            &tmp4,  /* cellId */
            &tmp5   /* arfcn */
            ); 

        gsmCell.arfcn = tmp5; 
        if (gsmCell.arfcn < 125) 
		{
			ret = GSM900;
		}
		else if (gsmCell.arfcn >= 512 && gsmCell.arfcn <= 885) 
		{
			ret = DCS1800;
		}
		else if (gsmCell.arfcn >= 512 && gsmCell.arfcn <= 885) 
		{
			ret = EGSM;
		}
		else 
		{
			printf("unsupported arfcn: %d, \n", gsmCell.arfcn);
			ret = -1;
		}
		
		
		*pArfcn = gsmCell.arfcn;
	}
	else 
	{
	    return RET_FAILED;
	}
		
	return ret;
}

/////////////////////////////////////////////////////////////////////////////
// read COMM port parameters from config file
static int read_port_conf(int fp, const char *key, TModem *ptModem)
{
	char line[1024];
	char *ptr;
#ifdef WIN32	
	char port[] ="COM1";
#else
    char port[] ="/dev/ttyS1";
#endif

#ifdef WIN32
	fpos_t pos;
#endif
	BOOL bFound = FALSE;
	int nBaudRate; 
	int nByteSize; 
	int nStopBits;
	int nParity;

	if(fp < 0 )
	{
		return -1;
	}

	CfgLock();
	
#ifdef WIN32	
	pos = 0;
	if( fsetpos( (FILE *)fp, &pos ) != 0 )
	{
		CfgUnlock();
		return -1; 
	}
#else
	fseek((FILE *)fp, 0, SEEK_SET); 
#endif	

    ptr = &line[0];
	while ((!feof((FILE *)fp)) && (!bFound))
	{
	    fgets(ptr, 1023, (FILE *)fp);
		
		if(strncmp(ptr, key, sizeof(key)) == 0)
		{
		    ptr += strlen(key);
#ifdef WIN32
			ptr = strstr(ptr, "COM");
#else
			ptr = strstr(ptr, "/dev/ttyS");
#endif
			if (ptr == NULL) 
				continue; //read next line from port conf file
			
			memcpy(ptModem->abyUartPort, ptr, sizeof(port));
			ptr += strlen(port);
			sscanf(ptr, "%d, %d, %d, %d", 
            &nBaudRate,
            &nByteSize,
            &nStopBits,
            &nParity);

			ptModem->nBaudRate = nBaudRate;
			ptModem->nByteSize = nByteSize;
			ptModem->nStopBits = nStopBits;
			ptModem->nParity =   nParity;
			
			bFound = TRUE;
		}
	}

	CfgUnlock();

	return -1;
	
}

/////////////////////////////////////////////////////////////////////////////
// read Main info
/* +CMGRMI: Main_Info,<mode>,<service_available>,<valid>, <idle>,<ra_rnti>,<c_rnti>,<cqi_wb>,<enb_num_tx_antenna> */ 
static int readMainInfo(char * pDataIn) 
{
    u32 tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, tmp8;
	int ret_v = RET_FAILED;
    
    pDataIn = strstr(pDataIn, "+CMGRMI: Main_Info,");
	if (pDataIn != NULL) 
	{
	    pDataIn += strlen("+CMGRMI: Main_Info,");
	    sscanf(pDataIn, "%d,%d,%d,%d,%d,%d,%d,%d", 
            &tmp1, /* mode */
            &tmp2, /* service_available */
            &tmp3, /* valid */
            &tmp4, /* idle */
            &tmp5, /* ra_rnti */
            &tmp6, /* c_rnti */
            &tmp7, /* cqi_wb */
            &tmp8  /* enb_num_tx_antenna */
            );
		
		if ((tmp1 == 4) && (tmp2 == 1)) 
		{
			pAtCmgrmiOut->ueMainInfo.mode = tmp1; 
	        pAtCmgrmiOut->ueMainInfo.service_available = tmp2; 
	        pAtCmgrmiOut->ueMainInfo.valid = tmp3; 
	        pAtCmgrmiOut->ueMainInfo.idle = tmp4; 
	        pAtCmgrmiOut->ueMainInfo.ra_rnti = tmp5; 
	        pAtCmgrmiOut->ueMainInfo.c_rnti = tmp6; 
	        pAtCmgrmiOut->ueMainInfo.cqi_wb = tmp7;
	        pAtCmgrmiOut->ueMainInfo.enb_num_tx_antenna = tmp8; 
			ret_v = RET_OK;
		}
	}
	else 
	{
	    Log(OS_LOG_ERR, "Can't find: +CMGRMI: Main_Info,", "");
	}
	
	return ret_v;
}

/////////////////////////////////////////////////////////////////////////////
// read serving cell info
/* +CMGRMI: Serving_Cell,<earfcn>,<mcc>,<mnc>,<tac>,<num_mnc_digits>,<serving_cell_id>,
                 <freq_band_ind>,<dl_bandwidth>,<ul_bandwidth>,<serv_rssnr>,<cell_pci>,
                 <cell_rsrq>,<cell_rsrp>,<cell_rssi>,<cell_idle_srxlev> */ 
static int readServingCellInfo(char * pDataIn) 
{
    int k, ret_v = RET_FAILED; 
	u32 tmp1, tmp2, tmp3, tmp4, tmp5, tmp6, tmp7, tmp8, tmp9, tmp10; 
	int nni1, nni2, nni3, nni4, nni5; 
	
    pDataIn = strstr(pDataIn, "+CMGRMI: Serving_Cell,");
	if (pDataIn != NULL) 
	{
	    pDataIn += strlen("+CMGRMI: Serving_Cell,"); 
	    sscanf(pDataIn, "%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d,%d",  
	        &tmp1,   /* earfcn */
	        &tmp2,   /* mcc */
	        &tmp3,   /* mnc */
	        &tmp4,   /* tac */
	        &tmp5,   /* num_mnc_digits */
	        &tmp6,   /* serving_cell_id */
	        &tmp7,   /* freq_band_ind */
	        &tmp8,   /* dl_bandwidth */
	        &tmp9,   /* ul_bandwidth */
	        &nni1,   /* serv_rssnr */
	        &tmp10,  /* cell_pci */
	        &nni2,   /* cell_rsrq */
	        &nni3,   /* cell_rsrp */
	        &nni4,   /* cell_rssi */
	        &nni5    /* cell_idle_srxlev */
	        ); 

        if (((u16)tmp1 == 0xFFFF) || ((u16)tmp2 != 0x01cc)) return RET_FAILED;
	    pAtCmgrmiOut->currentSCellInfo.earfcn = tmp1;
		pAtCmgrmiOut->currentSCellInfo.mcc = tmp2;
		pAtCmgrmiOut->currentSCellInfo.mnc = tmp3;
		pAtCmgrmiOut->currentSCellInfo.tac = tmp4;
		pAtCmgrmiOut->currentSCellInfo.num_mnc_digits = tmp5;
		pAtCmgrmiOut->currentSCellInfo.cellId = tmp6;
		pAtCmgrmiOut->currentSCellInfo.freq_band_ind = tmp7;
		pAtCmgrmiOut->currentSCellInfo.dl_bandwidth = tmp8;
		pAtCmgrmiOut->currentSCellInfo.ul_bandwidth = tmp9;
		pAtCmgrmiOut->currentSCellInfo.serv_rssnr = nni1;
		pAtCmgrmiOut->currentSCellInfo.cell_pci = tmp10;
		pAtCmgrmiOut->currentSCellInfo.cell_rsrq = nni2;
		pAtCmgrmiOut->currentSCellInfo.cell_rsrp = nni3;
		pAtCmgrmiOut->currentSCellInfo.cell_rssi = nni4;
		pAtCmgrmiOut->currentSCellInfo.cell_idle_srxlev = nni5;
		
	    k = MCC_LEN;
	    while (tmp2 != 0) 
	    {
	        pAtCmgrmiOut->currentSCellInfo.tPlmn.abyMcc[--k] = tmp2%10;
	        tmp2 /= 10;
	    }
	    
	    k = MNC_LEN;
	    while (tmp3 != 0)
	    {
	        pAtCmgrmiOut->currentSCellInfo.tPlmn.abyMnc[--k] = tmp3%10;
	        tmp3 /= 10;
	    }

	    pAtCmgrmiOut->currentSCellInfo.tPlmn.num = tmp5;
	    pAtCmgrmiOut->currentSCellInfo.tPlmn.isValid = 1; 
		ret_v = RET_OK;
	}
	else 
	{
	    Log(OS_LOG_ERR, "Can't find: +CMGRMI: Serving_Cell,", "");
	}

	return ret_v;
}

/////////////////////////////////////////////////////////////////////////////
// Get intra cell num from "+CMGRMI: LTE_Intra"
/* +CMGRMI: LTE_Intra,<sib3_received>,<earfcn>,<serving_cell_id>,<num_lte_cells> */ 
static int readLteIntraCellsNum(char * pDataIn) 
{
    u32 sib3_received;
	u32 tmp2, tmp3;
	int num_intra_cells = 0;
	
    pDataIn = strstr(pDataIn, "+CMGRMI: LTE_Intra,");
	if (pDataIn != NULL) 
	{
	    pDataIn += strlen("+CMGRMI: LTE_Intra,");
	    sscanf(pDataIn, "%d,%d,%d,%d", 
            &sib3_received, 
            &tmp2,    /* earfcn */
            &tmp3,    /* serving_cell_id */
            &num_intra_cells
            );

		if (sib3_received > 0) 
		{
		    pAtCmgrmiOut->sib3_received = 1;
			pAtCmgrmiOut->num_intrafreq_cells = num_intra_cells;
		}
	}
	else 
	{
	    Log(OS_LOG_ERR, "Can't find: +CMGRMI: LTE_Intra,", "");
	}

	return num_intra_cells;
}

/////////////////////////////////////////////////////////////////////////////
// Get inter freq num from "+CMGRMI: LTE_Inter"
/* +CMGRMI: LTE_Inter,<num_freqs>, */ 
static int readLteInter(char * pDataIn) 
{
    int inter_freq_num = 0;
	
    pDataIn = strstr(pDataIn, "+CMGRMI: LTE_Inter,");
	if (pDataIn != NULL) 
	{
	    pDataIn += strlen("+CMGRMI: LTE_Inter,");
	    sscanf(pDataIn, "%d", 
            &inter_freq_num
            );
	}
	else 
	{
	    Log(OS_LOG_ERR, "Can't find: +CMGRMI: LTE_Inter,", "");
	}

	return inter_freq_num;
}

/////////////////////////////////////////////////////////////////////////////
// read one intra or inter freq cell parameters
/* +CMGRMI: LTE_Intra_Cell1, <cell_pci>,<cell_rsrq>,<cell_rsrp>,<cell_rssi>,<cell_idle_srxlev> */ 
static void readACellPara(char * pDataIn, lte_cell_t * pCell) 
{
    u32 tmp1;
	int nni2, nni3, nni4, nni5; 
	
	if (pDataIn != NULL) 
	{
	    sscanf(pDataIn, "%d,%d,%d,%d,%d", 
	            &tmp1,  /* cell_pci */
	            &nni2,  /* cell_rsrq */
	            &nni3,  /* cell_rsrp */
	            &nni4,  /* cell_rssi */
	            &nni5   /* cell_idle_srxlev */
	            ); 
		
		pCell->cell_pci  = tmp1;
		pCell->cell_rsrq = nni2;
		pCell->cell_rsrp = nni3;
		pCell->cell_rssi = nni4;
		pCell->cell_idle_srxlev = nni5;
	}
	else 
	{
	    Log(OS_LOG_ERR, "read LTE cell parameters failed", "");
	}
}

/////////////////////////////////////////////////////////////////////////////
// For each inter freq, get its parameters
/* Freq1,<earfcn>,<num_lte_cells>,<idle_threshX_low>,<idle_threshX_high>,<idle_cell_resel_priority> */ 
static void readInterFreqs(char * pDataIn, inter_freqs_t * pInterFreq) 
{
    u32 tmp1, tmp2, tmp3, tmp4, tmp5;
	
	if (pDataIn != NULL) 
	{
	    sscanf(pDataIn, "%d,%d,%d,%d,%d", 
	            &tmp1,  /* earfcn */
	            &tmp2,  /* num_lte_cells */
	            &tmp3,  /* idle_threshX_low */
	            &tmp4,  /* idle_threshX_high */
	            &tmp5   /* idle_cell_resel_priority */
	            ); 
		
		pInterFreq->earfcn = tmp1;
		pInterFreq->resel_priority = tmp5;
		pInterFreq->threshX_high = tmp4;
		pInterFreq->threshX_low = tmp3;
		pInterFreq->num_inter_cells = tmp2;
		memset(&(pInterFreq->interCell[0]), 0, sizeof(lte_cell_t)*MAX_CELL_INTER);
	}
	else 
	{
	    Log(OS_LOG_ERR, "read serving cell's inter freqs failed", "");
	}
}

/////////////////////////////////////////////////////////////////////////////
// read all inter-freq cells for one inter-freq
static void readInterCells(char * pDataIn) 
{
	int j, k, n;
	int inter_freq_num;
	char str1[32];
	char str2[32];
	char * pStr;
	inter_freqs_t * pInterFreq;
	lte_cell_t * pLteCell;
	
	
	memset(str1, 0, 32);
	memset(str2, 0, 32);
	inter_freq_num = pAtCmgrmiOut->num_interFreq;
	
	k = 1;
	for (j = 0; j < inter_freq_num; j++)  
	{
		sprintf(str1, "%s%d", "+CMGRMI: LTE_InterFreq", k);
	    pInterFreq = &(pAtCmgrmiOut->interfreqs[j]);	
        for (n = 0; n < pInterFreq->num_inter_cells; n++) 
		{
		    sprintf(str2, "%s%d,","_Cell", (n+1));
			pStr = join(str1, str2);
			pDataIn = strstr(pDataIn, pStr);
	        pDataIn += strlen(pStr);

			pLteCell = &(pInterFreq->interCell[n]);
			readACellPara(pDataIn, pLteCell);
			free(pStr);
		}
		k++;
	}
}

/////////////////////////////////////////////////////////////////////////////
// select an inter-freq cell according to these criteria:  
// (1)reselection priority, (2)threshX-high, (3) cell_idle_srxlev and (4) RSRP 
static BOOL InterFreqCellSelect(target_cell_t * ptargetCell, int interfreqNum) 
{
	BOOL 		ret = FALSE;
	inter_freqs_t  interfreqs[MAX_FREQ];
	inter_freqs_t * pDest;
	inter_freqs_t * pSrc;	
	u8 scellPri; /* INTEGER (0..7), Value 0 means: lowest priority*/
	u8 ncellPri;
	int j, k;
	int same_srxlev = 1;

    pDest = &(interfreqs[0]);
    memset(pDest, 0, sizeof(inter_freqs_t)*MAX_FREQ);
	scellPri = pAtCmgrmiOut->currentSCellInfo.priority; 

    k = 0;
	for (j = 0; j < interfreqNum; j++) 
	{
		pSrc = &(pAtCmgrmiOut->interfreqs[j]);
		ncellPri = pSrc->resel_priority;
	    if (ncellPri > scellPri) // high priority 
		{
			memcpy(pDest, pSrc, sizeof (inter_freqs_t));
			pDest++;
			k++;
		}
	}

    pDest = &(interfreqs[0]);
	//threshX_high sort 
	Dir_Insert_threshXHigh(pDest, k); //min threshX-high, maybe not highest priority
	
	Dir_Insert_Srxlev(&(pDest->interCell[0]), pDest->num_inter_cells);

    
    for (j=1; j < pDest->num_inter_cells; j++) 
	{
		if (pDest->interCell[j].cell_idle_srxlev == pDest->interCell[0].cell_idle_srxlev) 
			same_srxlev++;
	}
	
	if (same_srxlev > 1) 
		Dir_Insert_Rsrp(&(pDest->interCell[0]),same_srxlev);
	
    //update target cell's parameters
    ptargetCell->earfcn = pDest->earfcn;
	ptargetCell->cell_pci = pDest->interCell[0].cell_pci;
	ptargetCell->cellId += 256;
	
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// open COMM port 
int SerialPortInit(TModem *ptModem/*, const char* pPort*/)
{
    int ret;
	int fp = -1;
	char key[] ="SIM7100: ";
	/* SIM7100's uart properties config */
/*    int nBaudRate = 115200; 
	int nParity   = 0;
	int nByteSize = 8; 
	int nStopBits = 1;
*/	
    pAtCmgrmiOut = &atCmgrmiOut;
	memset(pAtCmgrmiOut, 0, sizeof(at_cmgrmi_output_t));
	pAtCmgrmiOut->currentSCellInfo.priority = 4;

	read_port_conf(gFp, key, ptModem);
/*    ptModem->nBaudRate = nBaudRate;
	ptModem->nParity = nParity;
	ptModem->nByteSize = nByteSize;
	ptModem->nStopBits = nStopBits;
	memcpy(ptModem->abyUartPort, pPort, sizeof(pPort));
*/	
	ret = modem_open(ptModem);
	if(ret != RET_OK)
	{
		Log(OS_LOG_ERR, "modem open failed:%s", ptModem->abyUartPort);
	}
	else 
	{
		ret = modem_imsi_get(ptModem);
		ret = modem_version_get(ptModem);
		ret = modem_echo(ptModem, 1); //turn on echo mode
	}

	return ret;
}

/////////////////////////////////////////////////////////////////////////////
// After uart open, write AT CMD and read uart
int GetCellsDataFromAtCmd(TModem * ptModem, int mode) 
{
    int ret;
    char cmd[16];
	char cmResp[16];
	char dataOut[1024];
	int  intra_cell_num;
	int j, k;
	char strTmp[32];
	int inter_freq_num;
	lte_cell_t * pLteCell;
	inter_freqs_t * pInterFreq;
	char * pData = dataOut;

    memset(pData, 0, 1024);
	memset(strTmp, 0, 32);
	
	sprintf(cmd, "AT+CMGRMI=%d\r", mode);
	sprintf(cmResp, "%s", "OK");
	ret = modem_atCmd_w_r(ptModem, cmd, cmResp, pData);
	if(ret != RET_OK)
	{
	    Log(OS_LOG_ERR, "get data from port is failed", "");
		return RET_FAILED;
	}

	if (strncmp(pData, cmd, 11) == 0) //cmd is "AT+CMGRMI=4"
	{
	    ret = readMainInfo(pData);
		if(ret != RET_OK)
		{
			return RET_FAILED;
		}
		
		ret = readServingCellInfo(pData);
		if(ret != RET_OK)
		{
			return RET_FAILED;
		}
		
		intra_cell_num = readLteIntraCellsNum(pData);
		
	    k = 1;
	    for (j = 0; j < intra_cell_num; j++) 
		{
		    sprintf(strTmp, "%s%d,","+CMGRMI: LTE_Intra_Cell",k);
			pData = strstr(pData, strTmp);
	        pData += strlen(strTmp);

			pLteCell = &(pAtCmgrmiOut->intrafreqCell[j]);
			readACellPara(pData, pLteCell);
			k++;
		}

		inter_freq_num = readLteInter(pData);
		pAtCmgrmiOut->num_interFreq = inter_freq_num;

		k = 1;
		memset(strTmp, 0, 32);
	    for (j = 0; j < inter_freq_num; j++) 
		{
		    sprintf(strTmp, "%s%d,","Freq",k);
			pData = strstr(pData, strTmp);
	        pData += strlen(strTmp);

			pInterFreq = &(pAtCmgrmiOut->interfreqs[j]);
			readInterFreqs(pData, pInterFreq);
			k++;
		}

        readInterCells(pData);
		

		return RET_OK;
	}
	else 
	{
	    Log(OS_LOG_ERR, "Can't find: AT+CMGRMI=4, get data failed", "");
		return RET_FAILED;
	}
}

/////////////////////////////////////////////////////////////////////////////
// From uart data, select a cell as target cell  
void buildReselTargetCellPara(target_cell_t * ptargetCell)
{
    int i;
	int intra_cell_num;
	int intra_cell_same_rsrp = 1;
	int inter_freq_num;
	lte_cell_t   intrafreqCell[MAX_CELL_INTRA];
	lte_cell_t * pLteCell;
	BOOL cellSelected = FALSE;

    memset((void *)ptargetCell, 0, sizeof(target_cell_t));
	ptargetCell->freq_band_ind = pAtCmgrmiOut->currentSCellInfo.freq_band_ind;
	ptargetCell->earfcn = pAtCmgrmiOut->currentSCellInfo.earfcn;

    ptargetCell->mcc = pAtCmgrmiOut->currentSCellInfo.mcc;
	ptargetCell->mnc = pAtCmgrmiOut->currentSCellInfo.mnc;
	ptargetCell->num = pAtCmgrmiOut->currentSCellInfo.num_mnc_digits;
	
	ptargetCell->tac = pAtCmgrmiOut->currentSCellInfo.tac + 32; //
	ptargetCell->cell_pci = pAtCmgrmiOut->currentSCellInfo.cell_pci;
	ptargetCell->cellId = pAtCmgrmiOut->currentSCellInfo.cellId;
	
	intra_cell_num = pAtCmgrmiOut->num_intrafreq_cells;
	inter_freq_num = pAtCmgrmiOut->num_interFreq;

	
    if (inter_freq_num > 0) 
	{
		cellSelected = InterFreqCellSelect(ptargetCell, inter_freq_num);
	}
	
	if ((!cellSelected) && 
		(intra_cell_num > 0) ) 
	{
		memcpy (intrafreqCell, pAtCmgrmiOut->intrafreqCell, MAX_CELL_INTRA*sizeof(lte_cell_t));
        
		pLteCell = &(intrafreqCell[0]);
		Dir_Insert_Rsrp(pLteCell, intra_cell_num);
		
		for (i=1; i < intra_cell_num; i++) 
		{
			if (intrafreqCell[i].cell_rsrp == intrafreqCell[0].cell_rsrp) 
				intra_cell_same_rsrp++;
		}
		
		if (intra_cell_same_rsrp > 1) 
			Dir_Insert_Srxlev(&(intrafreqCell[0]),intra_cell_same_rsrp);

		ptargetCell->cell_pci = intrafreqCell[0].cell_pci;
		ptargetCell->cellId += 256;
	}


}

/////////////////////////////////////////////////////////////////////////////
// This function is only used for test
int atcmdDataFromFile_test(void) 
{
	FILE *fp;
	char ch[4096];
	char * pCh = ch;
	char * pData;
	
	char cmd[16];
	int intra_cell_num;
	int inter_freq_num;
	int j, k;
	char strTmp[32];
	lte_cell_t * pLteCell;
	inter_freqs_t * pInterFreq;
	char temp;

	target_cell_t targetCell;
    target_cell_t * ptargetCell = &targetCell;
	memset((void *)ptargetCell, 0, sizeof(target_cell_t));
	memset((void *)pCh, 0, 4096);
	
	//debug, data form atcmgrmi.txt
	fp = fopen("atcmgrmi.txt", "r");
	if(fp == NULL)
	{
		Log(OS_LOG_ERR, "open file: atcmgrmi.txt, error", "");
		return RET_FAILED;
	}
	
	while ((temp=fgetc(fp))!=EOF)  { 
		*pCh++=temp;  //read total file into ch[]
	}

	pData = &ch[0];
	sprintf(cmd, "AT+CMGRMI=%d\n", 4);
	if (strncmp(pData, cmd, 11) == 0) //cmd is "AT+CMGRMI=4"
	{
		readMainInfo(pData);
		readServingCellInfo(pData);
		intra_cell_num = readLteIntraCellsNum(pData);
		
		k = 1;
		for (j = 0; j < intra_cell_num; j++) 
		{
			sprintf(strTmp, "%s%d,","+CMGRMI: LTE_Intra_Cell",k);
			pData = strstr(pData, strTmp);
			pData += strlen(strTmp);

			pLteCell = &(pAtCmgrmiOut->intrafreqCell[j]);
			readACellPara(pData, pLteCell);
			k++;
		}

		inter_freq_num = readLteInter(pData);
		pAtCmgrmiOut->num_interFreq = inter_freq_num;

		k = 1;
		memset(strTmp, 0, 32);
		for (j = 0; j < inter_freq_num; j++) 
		{
			sprintf(strTmp, "%s%d,","Freq",k);
			pData = strstr(pData, strTmp);
			pData += strlen(strTmp);

			pInterFreq = &(pAtCmgrmiOut->interfreqs[j]);
			readInterFreqs(pData, pInterFreq);
			k++;
		}

		readInterCells(pData);
		
		buildReselTargetCellPara(ptargetCell);

		return RET_OK;

	}		
	else 
	{
		Log(OS_LOG_ERR, "can't find string 'AT+CMGRMI=4' in data", "");
		return RET_FAILED;
	}
}

int getDataFromXml_test(lte_pbs_para_t * pPara, int mode) 
{
	switch(mode)
	{
	case 1:  //FDD
	pPara->u8Band = 3;
	pPara->u16Earfcn = 1650;
	pPara->u16Tac = 6395;
	pPara->u16PCI = 398;
	pPara->u16CellId = 2069;
	pPara->u16MCC = 460;
	pPara->u8Mnc = 1;

	pPara->u8pwr = 12; 
	pPara->u8InitialValueTag = 4;
	
	break;

	case 2: //TDD 
	pPara->u8Band = 41;
	pPara->u16Earfcn = 40738;
	pPara->u16Tac = 6263;
	pPara->u16PCI = 164;
	pPara->u16CellId = 6;
	pPara->u16MCC = 460;
	pPara->u8Mnc = 0;
	pPara->u8pwr = 6; 
	pPara->u8InitialValueTag = 3;
	break;

	default:
	break;
	}


	return 0;
}
////////////////////////////////////////////////////////////////
int lte_cell_select_init(TModem * ptModem)
{
	return	SerialPortInit(ptModem);
}
////////////////////////////////////////////////////////////////
int lte_cell_select(lte_state_t *pLte_state)
{
	int ret;
	int tmp = 0;
	uint16_t mcc;
	uint16_t cellid;
	uint16_t arfcn;  //for 2G
	uint8_t mnc;
	uint8_t pwr = POWER_MIN;
	target_cell_t targetCell;

	uint16_t earfcn_array[16];
	uint8_t band_array[16];
	
	memset((void *)&targetCell, 0, sizeof(target_cell_t));
	switch(tmp)
	{
	case 0:  //data from Uart port
		ret = GetCellsDataFromAtCmd(&(pLte_state->tModem), 4);
		if(ret != RET_OK)
		{
			return RET_FAILED;
		}

		buildReselTargetCellPara(&targetCell);

        mcc = targetCell.mcc;
		mnc = targetCell.mnc;
		if (mcc != 460) return RET_FAILED;
		
		cellid = (uint16_t)((targetCell.cellId) & 0x0000FFFF);
		lte_pbs_cfg_set(&pLte_state->lte_cfg.strPbsPara, 
			                mcc, mnc, 
							targetCell.freq_band_ind, 
							targetCell.earfcn, 
							targetCell.tac, cellid, 
							targetCell.cell_pci, 
							pwr);
		pLte_state->lte_cfg.strPbsPara.u8InitialValueTag = 1;
		if (targetCell.freq_band_ind >= 33) 
		{  //TDD
			pLte_state->syn_mode = O_LTE_SYNC;
			lte_sync_cfg(&pLte_state->lte_cfg.strSynPara, pLte_state->lte_cfg.strPbsPara.u8Band, pLte_state->lte_cfg.strPbsPara.u16Earfcn, pAtCmgrmiOut->currentSCellInfo.cell_pci);
			//lte_sync_cfg(&(pLte_state->lte_cfg.strSynPara), 41, 40738, 163);
			// setup snifffer param
		    earfcn_array[0] = 40738;
		    band_array[0] = 41;
		    lte_sniffer_cfg(&pLte_state->lte_cfg, 1, &band_array[0], &earfcn_array[0]);
			lte_rel_cfg(&(pLte_state->lte_cfg.strRelPara), 0, 11, 0, 0);
			pLte_state->dst_bsId = 1;
			pLte_state->dst_modId = MOD_ID_TDD_CMCC1;
			pLte_state->tddfdd_mode = 1; //tdd mode
		} 
		else if (targetCell.freq_band_ind > 0) 
		{  //FDD
			pLte_state->syn_mode = O_LTE_NO_SYNC;
			//lte_sync_cfg(&pLte_state->lte_cfg.strSynPara, targetCell.freq_band_ind, targetCell.earfcn, targetCell.cell_pci); 
			pLte_state->tddfdd_mode = 0; //fdd mode
		}
		else 
		{  //not find 
			return RET_FAILED;
		}
        
		ret = get_rrc_conn_rel_para(&(pLte_state->tModem), &arfcn, oam_get_oper_byCnType(mcc, mnc));
		if (ret >= 0) 
		{ 
			lte_rel_cfg(&(pLte_state->lte_cfg.strRelPara), 0, arfcn, 0, 0); // band:0 ??
		}

		oam_lte_cell_report(pLte_state, pAtCmgrmiOut);
	break;

	case 1: //data from default FDD 
		getDataFromXml_test(&(pLte_state->lte_cfg.strPbsPara), tmp);
		//pLte_state->lte_cfg.u16SynMode = O_LTE_NO_SYNC;
		pLte_state->syn_mode = O_LTE_NO_SYNC;
		//lte_sync_cfg(&pLte_state->lte_cfg.strSynPara, pLte_state->lte_cfg.strPbsPara.u8Band, pLte_state->lte_cfg.strPbsPara.u16Earfcn, pLte_state->lte_cfg.strPbsPara.u16PCI);
		pLte_state->tddfdd_mode = 0; //fdd mode
	break;

	case 2: //data from default TDD 
		getDataFromXml_test(&(pLte_state->lte_cfg.strPbsPara), tmp);
		pLte_state->syn_mode = O_LTE_SYNC;
		//lte_sync_cfg(&pLte_state->lte_cfg.strSynPara, pLte_state->lte_cfg.strPbsPara.u8Band, pLte_state->lte_cfg.strPbsPara.u16Earfcn, pLte_state->lte_cfg.strPbsPara.u16PCI);
		lte_sync_cfg(&(pLte_state->lte_cfg.strSynPara), 41, 40738, 163);
		// setup snifffer param
	    earfcn_array[0] = 40738;
	    band_array[0] = 41;
	    lte_sniffer_cfg(&pLte_state->lte_cfg, 1, &band_array[0], &earfcn_array[0]);
		lte_rel_cfg(&(pLte_state->lte_cfg.strRelPara), 0, 11, 0, 0);
		pLte_state->dst_bsId = 1;
		pLte_state->dst_modId = MOD_ID_TDD_CMCC1;
		pLte_state->tddfdd_mode = 1; //tdd mode
	break;

    case 3: 
		atcmdDataFromFile_test();
	break;
	
	default:
	break;
	}
	//

	return RET_OK;
}

////////////////////////////////////////////////////////////////
int lte_cells_para_acquire(lte_state_t *pLte_state) 
{
	int ret_val;
	uint8_t  cn_oper;
	//
	cn_oper = oam_get_oper_byCnType(pLte_state->lte_cfg.strPbsPara.u16MCC, 
		                            pLte_state->lte_cfg.strPbsPara.u8Mnc);
	ret_val = cn_operator_lock(&(pLte_state->tModem), cn_oper);
	if (ret_val == RET_OK)
	{
		ret_val = set_preferred_mode(&(pLte_state->tModem), LTE_ONLY); //???
		if (ret_val == RET_OK) 
		{
			ret_val = oam_set_env_band(&(pLte_state->tModem), pLte_state->lte_cfg.strPbsPara.u8Band);
		}
	}
	else 
	{
		return RET_FAILED;
	}

	if (ret_val == RET_OK) 
	{
		oss_delay(2000); //2s
		ret_val = GetCellsDataFromAtCmd(&(pLte_state->tModem), 4);
		if(ret_val != RET_OK)
		{
			printf("oper(%d),band(%d),SCELL SCAN periodically:AT+CMGRMI=4, get data failed\n", cn_oper,pLte_state->lte_cfg.strPbsPara.u8Band);
			return RET_FAILED;
		} 
	}
	
	if ((pAtCmgrmiOut->currentSCellInfo.cell_pci == pLte_state->lte_cfg.strPbsPara.u16PCI) && 
		(pAtCmgrmiOut->currentSCellInfo.freq_band_ind == pLte_state->lte_cfg.strPbsPara.u8Band) && 
		(pAtCmgrmiOut->currentSCellInfo.earfcn == pLte_state->lte_cfg.strPbsPara.u16Earfcn)) 
	{ //get data by at-command, it's serving cell
	    oam_lte_cell_report(pLte_state, pAtCmgrmiOut); 
	}
	return RET_OK;
}

