/**************************************************
*
*serial.h - defines file 
*
***************************************************/

#ifndef __WCDMA_CELL_SEL_H__
#define __WCDMA_CELL_SEL_H__

#define  MAX_CELL_INTRA     16 /* TS36.331, maximum number of neighbouring inter-frequency cells listed in SIB type 5 */
#define  MAX_CELL_INTER     16 /* TS36.331, maximum number of neighbouring intra-frequency cells listed in SIB type 4 */
#define  MAX_FREQ           8  /* TS36.331, maximum number of carrier frequencies */
/////////////////////////////////////////////////////////////////////////////

typedef struct TagMainInfo {
    u8   mode; /* 4 ¨C LTE */
    u8   service_available; /* 0 ¨C service not available 
                               1 ¨C service is available */
    u8   valid; 
    u8   idle;
    u16  ra_rnti; /* Random access radio network temporary ID */
    u16  c_rnti;
    u8   cqi_wb; /* channel quality indication, measured by UE */
    u8   enb_num_tx_antenna; /* Number of Tx antenna on an LTE base station */

} main_info_t;

typedef struct TagSCellInfo {    
    u16     earfcn; 
    u16     tac;
    u32     cellId; /* eNBid (20bits) and cellid (8bit) */
    u8      dl_bandwidth; /* Transmission bandwidth configuration of the serving cell on the downlink. Range: 0 to 5 */
    u8      ul_bandwidth; /* Transmission bandwidth configuration of the serving cell on the uplink. Range: 0 to 5 */
    u16     cell_pci; /* Range: 0 to 503 */
    u8      freq_band_ind;
	u8      priority;
    int8_t  serv_rssnr; /* Average reference signal signal-to-noise ratio of the serving cell over 
                           the last measurement period in decibels. Range: -10 to 30 */
    int16_t cell_rsrp;
    int16_t cell_rsrq;
    int16_t cell_rssi; /* Received signal strength indicator. Range: 0 to -120 */
    int16_t cell_idle_srxlev;
    TPlmn   tPlmn; /* mcc, mnc, num_ mnc_digits */
} scell_info_t;

typedef struct TagLteCell { 
    int32_t cell_rsrp; 
    int32_t cell_rssi; /* Received signal strength indicator. Range: 0 to -120 */
    int32_t cell_rsrq;
    int32_t cell_idle_srxlev;
    u16     cell_pci; /* Range: 0 to 503 */
    u16     reserved;
} wcdma_cell_t;

typedef struct TagInterFreqs {
    u16     earfcn;
    u8      resel_priority;
    u8      threshX_high;
    u8      threshX_low;
    u8      num_inter_cells;
	wcdma_cell_t interCell[MAX_CELL_INTER];
} inter_freqs_t;

typedef struct TagTargetCell { 
    u8		freq_band_ind;
	u16 	earfcn; 
	u16 	tac;
	u16 	cell_pci;
	u32 	cellId; 
	
	u8   	Mcc[MCC_LEN];
	u8   	Mnc[MNC_LEN];
	u8   	num;
    u8   	reserved;
} target_cell_t;

typedef struct TagAT_CMGRMI_OUT {
    main_info_t    ueMainInfo;
    scell_info_t   currentSCellInfo;
    BOOL        sib3_received;
    u8          num_intrafreq_cells;
    u8          num_interFreq;
    inter_freqs_t  interfreqs[MAX_FREQ];
    wcdma_cell_t     intrafreqCell[MAX_CELL_INTRA];
} at_cmgrmi_output_t;

/////////////////////////////////////////////////////////////////////////////

int SerialPortInit(TModem *ptModem);
int GetCellsDataFromAtCmd(TModem * ptModem, int mode);
void buildReselTargetCellPara(target_cell_t * ptargetCell);
int atcmdDataFromFile_test(void); 

int wcdma_cell_select(wcdma_state_t *wcdma_state);
int wcdma_cell_select_init(TModem * ptModem);

#endif  /* _SERIAL_H */
