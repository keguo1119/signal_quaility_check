/**************************************************
*
*serial.c - defines file 
*
***************************************************/
#include "oss.h"
#include "config.h"
#include "protocol_common.h"
#include "modem.h"
#include "wg_common.h"
#include "wcdma_cfg.h"
#include "wcdma_sm.h"
#include "wcdma_cell_sel.h"

/////////////////////////////////////////////////////////////////////////////

/* xzluo, 2015-12-16 */ 
extern char * strConfigFilePath;
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
static void Dir_Insert_Rsrp(wcdma_cell_t A[], int N)    
{  
	int i, j;
	wcdma_cell_t t;
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
static void Dir_Insert_Srxlev(wcdma_cell_t A[], int N)    
{  
	int i, j;
	wcdma_cell_t t;
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
// read COMM port parameters from config file
static int read_port_conf(int fp, const char *key, TModem *ptModem)
{
	char line[1024];
	char *ptr;
	char port[] ="COM1";
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
	fseek(fp, 0, SEEK_SET); 
#endif	

    ptr = &line[0];
	while ((!feof((FILE *)fp)) && (!bFound))
	{
	    fgets(ptr, 1023, (FILE *)fp);
		
		if(strncmp(ptr, key, sizeof(key)) == 0)
		{
		    ptr += strlen(key);
			ptr = strstr(ptr, "COM");
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
// send AT CMD and get LTE cell info
static int LteCellsInfoGet(TModem *ptModem,char *atCmd, char *atCmdResp, char *pbyOut)
{
	int ret;
	int tryCount = 5;

	while(tryCount)
	{
		NetConnectSleep(1000);

		tryCount--;
		ret = modem_at_cmd_send(ptModem, atCmd);
		if(ret != RET_OK)
		{
			continue;
		}
		
		ret = modem_at_cmd_wait_rsp(ptModem, atCmdResp, pbyOut);
		if(ret != RET_OK)
		{
			continue;	
		}
		
		Log(OS_LOG_INFO, "get LTE cells info is done: %s ", ptModem->abyUartPort);

		return RET_OK;
	}

	Log(OS_LOG_ERR, "get LTE cells info is failed: %s ", ptModem->abyUartPort);

	return RET_FAILED;
}

/////////////////////////////////////////////////////////////////////////////
// read Main info
static void readMainInfo(char * pDataIn) 
{
    pDataIn = strstr(pDataIn, "+CMGRMI: Main_Info,");
	if (pDataIn != NULL) 
	{
	    pDataIn += strlen("+CMGRMI: Main_Info,");
	    sscanf(pDataIn, "%d, %d, %d, %d, %d, %d, %d, %d", 
            &(pAtCmgrmiOut->ueMainInfo.mode),
            &(pAtCmgrmiOut->ueMainInfo.service_available),
            &(pAtCmgrmiOut->ueMainInfo.valid),
            &(pAtCmgrmiOut->ueMainInfo.idle),
            &(pAtCmgrmiOut->ueMainInfo.ra_rnti),
            &(pAtCmgrmiOut->ueMainInfo.c_rnti),
            &(pAtCmgrmiOut->ueMainInfo.cqi_wb),
            &(pAtCmgrmiOut->ueMainInfo.enb_num_tx_antenna)
            );
	}
	else 
	{
	    Log(OS_LOG_ERR, "Can't find: +CMGRMI: Main_Info,", "");
	}
}

/////////////////////////////////////////////////////////////////////////////
// read serving cell info
static void readServingCellInfo(char * pDataIn) 
{
    int 	k;
	u16 	earfcn;
    u16 	mcc;
    u8  	mnc;
    u8  	num_mcc_mnc;
	u16 	tac;
	u16 	cell_pci;
	u32 	cellId;
	u8		dl_bandwidth; 
	u8		ul_bandwidth;
	u8		freq_band_ind;
	int8_t	serv_rssnr;

	int16_t cell_rsrp;
	int16_t cell_rsrq;
	int16_t cell_rssi; 
	int16_t cell_idle_srxlev;
	
    pDataIn = strstr(pDataIn, "+CMGRMI: Serving_Cell,");
	if (pDataIn != NULL) 
	{
	    pDataIn += strlen("+CMGRMI: Serving_Cell,");
	    sscanf(pDataIn, "%d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, %d", 
	        //&(pAtCmgrmiOut->currentSCellInfo.earfcn),
	        &earfcn,
	        &mcc,
	        &mnc,
	        &tac,
	        &num_mcc_mnc,
	        &cellId,
	        &freq_band_ind,
	        &dl_bandwidth,
	        &ul_bandwidth,
	        &serv_rssnr,
	        &cell_pci,
	        &cell_rsrq,
	        &cell_rsrp,
	        &cell_rssi,
	        &cell_idle_srxlev
	        );

	    pAtCmgrmiOut->currentSCellInfo.earfcn = earfcn;
		pAtCmgrmiOut->currentSCellInfo.tac = tac;
		pAtCmgrmiOut->currentSCellInfo.cellId = cellId;
		pAtCmgrmiOut->currentSCellInfo.freq_band_ind = freq_band_ind;
		pAtCmgrmiOut->currentSCellInfo.dl_bandwidth = dl_bandwidth;
		pAtCmgrmiOut->currentSCellInfo.ul_bandwidth = ul_bandwidth;
		pAtCmgrmiOut->currentSCellInfo.serv_rssnr = serv_rssnr;
		pAtCmgrmiOut->currentSCellInfo.cell_pci = cell_pci;
		pAtCmgrmiOut->currentSCellInfo.cell_rsrq = cell_rsrq;
		pAtCmgrmiOut->currentSCellInfo.cell_rsrp = cell_rsrp;
		pAtCmgrmiOut->currentSCellInfo.cell_rssi = cell_rssi;
		pAtCmgrmiOut->currentSCellInfo.cell_idle_srxlev = cell_idle_srxlev;
		
	    k = MCC_LEN;
	    while (mcc != 0) 
	    {
	        pAtCmgrmiOut->currentSCellInfo.tPlmn.abyMcc[--k] = mcc%10;
	        mcc /= 10;
	    }
	    
	    k = MNC_LEN;
	    while (mnc != 0)
	    {
	        pAtCmgrmiOut->currentSCellInfo.tPlmn.abyMnc[--k] = mnc%10;
	        mnc /= 10;
	    }

	    pAtCmgrmiOut->currentSCellInfo.tPlmn.num = num_mcc_mnc;
	    pAtCmgrmiOut->currentSCellInfo.tPlmn.isValid = 1; 
	}
	else 
	{
	    Log(OS_LOG_ERR, "Can't find: +CMGRMI: Serving_Cell,", "");
	}
}

/////////////////////////////////////////////////////////////////////////////
// Get intra cell num from "+CMGRMI: wcdma_Intra"
static int readLteIntraCellsNum(char * pDataIn) 
{
	u8      sib3_rcvd = 0;
	u8      num_intra_cells = 0;
	u16     earfcn;
	u32     cellId;
	
    pDataIn = strstr(pDataIn, "+CMGRMI: wcdma_Intra,");
	if (pDataIn != NULL) 
	{
	    pDataIn += strlen("+CMGRMI: wcdma_Intra,");
	    sscanf(pDataIn, "%d, %d, %d, %d", 
            &sib3_rcvd,
            &earfcn,
            &cellId,
            &num_intra_cells
            );

		if (sib3_rcvd > 0) 
		{
		    pAtCmgrmiOut->sib3_received = 1;
			pAtCmgrmiOut->num_intrafreq_cells = num_intra_cells;
		}
	}
	else 
	{
	    Log(OS_LOG_ERR, "Can't find: +CMGRMI: wcdma_Intra,", "");
	}

	return num_intra_cells;
}

/////////////////////////////////////////////////////////////////////////////
// Get inter freq num from "+CMGRMI: wcdma_Inter"
static int readLteInter(char * pDataIn) 
{
    u8 inter_freq_num = 0;
	
    pDataIn = strstr(pDataIn, "+CMGRMI: wcdma_Inter,");
	if (pDataIn != NULL) 
	{
	    pDataIn += strlen("+CMGRMI: wcdma_Inter,");
	    sscanf(pDataIn, "%d", 
            &inter_freq_num
            );
	}
	else 
	{
	    Log(OS_LOG_ERR, "Can't find: +CMGRMI: wcdma_Inter,", "");
	}

	return inter_freq_num;
}

/////////////////////////////////////////////////////////////////////////////
// read one intra or inter freq cell parameters
static void readACellPara(char * pDataIn, wcdma_cell_t * pCell) 
{
    int32_t cell_rsrp; 
    int32_t cell_rssi; 
    int32_t cell_rsrq;
    int32_t cell_idle_srxlev;
    u16     cell_pci;
	
	if (pDataIn != NULL) 
	{
	    sscanf(pDataIn, "%d, %d, %d, %d, %d", 
	            &cell_pci,
	            &cell_rsrq,
	            &cell_rsrp,
	            &cell_rssi,
	            &cell_idle_srxlev
	            ); 
		
		pCell->cell_pci  = cell_pci;
		pCell->cell_rsrp = cell_rsrp;
		pCell->cell_rsrq = cell_rsrq;
		pCell->cell_rssi = cell_rssi;
		pCell->cell_idle_srxlev = cell_idle_srxlev;
	}
	else 
	{
	    Log(OS_LOG_ERR, "read LTE cell parameters failed", "");
	}
}

/////////////////////////////////////////////////////////////////////////////
// For each inter freq, get its parameters
static void readInterFreqs(char * pDataIn, inter_freqs_t * pInterFreq) 
{
    u16     earfcn;
    u8      resel_priority;
    u8      threshX_high;
    u8      threshX_low;
    u8      num_inter_cells;
	
	if (pDataIn != NULL) 
	{
	    sscanf(pDataIn, "%d, %d, %d, %d, %d", 
	            &earfcn,
	            &num_inter_cells,
	            &threshX_low,
	            &threshX_high,
	            &resel_priority
	            ); 
		
		pInterFreq->earfcn = earfcn;
		pInterFreq->resel_priority = resel_priority;
		pInterFreq->threshX_high = threshX_high;
		pInterFreq->threshX_low = threshX_low;
		pInterFreq->num_inter_cells = num_inter_cells;
		memset(&(pInterFreq->interCell[0]), 0, sizeof(wcdma_cell_t)*MAX_CELL_INTER);
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
	wcdma_cell_t * pLteCell;
	
	
	memset(str1, 0, 32);
	memset(str2, 0, 32);
	inter_freq_num = pAtCmgrmiOut->num_interFreq;
	
	k = 1;
	for (j = 0; j < inter_freq_num; j++)  
	{
		sprintf(str1, "%s%d", "+CMGRMI: wcdma_InterFreq", k);
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

    fp = open_conf_file(strConfigFilePath);
    if(fp < 0)
	{
		Log(OS_LOG_ERR, "open config file: ctrlcfg.txt failed", "");
	}

	read_port_conf(fp, key, ptModem);
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
	int intra_cell_num;
	int j, k;
	char strTmp[32];
	int inter_freq_num;
	wcdma_cell_t * pLteCell;
	inter_freqs_t * pInterFreq;
	char * pData = dataOut;

    memset(pData, 0, 1024);
	memset(strTmp, 0, 32);
	
	sprintf(cmd, "AT+CMGRMI=%d\r", mode);
	sprintf(cmResp, "%s", "OK");
	ret = LteCellsInfoGet(ptModem, cmd, cmResp, pData);
	if(ret != RET_OK)
	{
	    Log(OS_LOG_ERR, "get data from port is failed", "");
		return RET_FAILED;
	}

	if (strncmp(pData, cmd, 11) == 0) //cmd is "AT+CMGRMI=4"
	{
	    readMainInfo(pData);
		readServingCellInfo(pData);
		intra_cell_num = readLteIntraCellsNum(pData);
		
	    k = 1;
	    for (j = 0; j < intra_cell_num; j++) 
		{
		    sprintf(strTmp, "%s%d,","+CMGRMI: wcdma_Intra_Cell",k);
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
	wcdma_cell_t   intrafreqCell[MAX_CELL_INTRA];
	wcdma_cell_t * pLteCell;
	BOOL cellSelected = FALSE;

    memset((void *)ptargetCell, 0, sizeof(target_cell_t));
	ptargetCell->freq_band_ind = pAtCmgrmiOut->currentSCellInfo.freq_band_ind;
	ptargetCell->earfcn = pAtCmgrmiOut->currentSCellInfo.earfcn;

	memcpy (ptargetCell->Mcc, pAtCmgrmiOut->currentSCellInfo.tPlmn.abyMcc, MCC_LEN);
	memcpy (ptargetCell->Mnc, pAtCmgrmiOut->currentSCellInfo.tPlmn.abyMnc, MNC_LEN);
	ptargetCell->num = pAtCmgrmiOut->currentSCellInfo.tPlmn.num;

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
		memcpy (intrafreqCell, pAtCmgrmiOut->intrafreqCell, MAX_CELL_INTRA*sizeof(wcdma_cell_t));
        
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
	wcdma_cell_t * pLteCell;
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
			sprintf(strTmp, "%s%d,","+CMGRMI: wcdma_Intra_Cell",k);
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

int getDataFromXml_test(wcdma_pbs_para_t * pPara) 
{
	pPara->u8Band = 41;
	pPara->u16Earfcn = 40340;
	pPara->u16Tac = 6295;
	pPara->u16PCI = 282;
	pPara->u16CellId = 1264;//?
	pPara->u16MCC = 460;
	pPara->u8Mnc = 1;

	pPara->u8pwr = 12;
	pPara->u8InitialValueTag = 1;

	return 0;
}
////////////////////////////////////////////////////////////////
int wcdma_cell_select_init(TModem * ptModem)
{
	return	SerialPortInit(ptModem);
}
////////////////////////////////////////////////////////////////
int wcdma_cell_select(wcdma_state_t *pwcdma_state)
{
	int ret;
	int tmp = 1;
	uint16_t mcc;
	uint16_t cellid;
	uint8_t mnc;
	uint8_t pwr = 12;
	target_cell_t targetCell;
	memset((void *)&targetCell, 0, sizeof(target_cell_t));

	switch(tmp)
	{
	case 0:
		ret = GetCellsDataFromAtCmd(&(pwcdma_state->tModem), 4);
		if(ret != RET_OK)
		{
			return RET_FAILED;
		}

		buildReselTargetCellPara(&targetCell);

        mcc = (targetCell.Mcc[0]*100) + (targetCell.Mcc[1]*10) + targetCell.Mcc[2];
		mnc = (targetCell.Mnc[0]*10) + targetCell.Mnc[1];
		cellid = (uint16_t)((targetCell.cellId) & 0x000000FF);
		wcdma_pbs_cfg_set(&pwcdma_state->wcdma_cfg.strPbsPara, 
			                mcc, mnc, 
							targetCell.freq_band_ind, 
							targetCell.earfcn, 
							targetCell.tac, cellid, 
							targetCell.cell_pci, 
							pwr);
		pwcdma_state->wcdma_cfg.strPbsPara.u8InitialValueTag = 1;
		
		pwcdma_state->wcdma_cfg.u16SynMode = 1;
		wcdma_sync_cfg(&pwcdma_state->wcdma_cfg.strSynPara, targetCell.freq_band_ind, targetCell.earfcn, targetCell.cell_pci);
	break;

	case 1:
		getDataFromXml_test(&(pwcdma_state->wcdma_cfg.strPbsPara));
		pwcdma_state->wcdma_cfg.u16SynMode = 1;
		wcdma_sync_cfg(&pwcdma_state->wcdma_cfg.strSynPara, pwcdma_state->wcdma_cfg.strPbsPara.u8Band, pwcdma_state->wcdma_cfg.strPbsPara.u16Earfcn, pwcdma_state->wcdma_cfg.strPbsPara.u16PCI);
	break;

	case 2:
	break;

	default:
	break;
	}
	//

	return RET_OK;
}

