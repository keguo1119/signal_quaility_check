/**************************************************
*
*serial.h - defines file 
*
***************************************************/

#ifndef __LTE_CELL_SEL_H__
#define __LTE_CELL_SEL_H__

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
	u16     mcc;
	u8      mnc;
	u8      num_mnc_digits;
    u16     earfcn; 
    u16     tac;
    u32     cellId; /* eNBid (20bits) and cellid (8bit) */
    u8      dl_bandwidth; /* Transmission bandwidth configuration of the serving cell on the downlink. Range: 0 to 5 */
    u8      ul_bandwidth; /* Transmission bandwidth configuration of the serving cell on the uplink. Range: 0 to 5 */
    u16     cell_pci; /* Range: 0 to 503 */
    u8      freq_band_ind;
	u8      priority;
    char    serv_rssnr; /* Average reference signal signal-to-noise ratio of the serving cell over 
                           the last measurement period in decibels. Range: -10 to 30 */
    short cell_rsrq;
    short cell_rsrp;
    short cell_rssi; /* Received signal strength indicator. Range: 0 to -120 */
    short cell_idle_srxlev;
    TPlmn   tPlmn; /* mcc, mnc, num_ mnc_digits */
} scell_info_t;

typedef struct TagLteCell { 
    short cell_rsrq; 
    short cell_rsrp; 
    short cell_rssi;  /* Received signal strength indicator. Range: 0 to -120 */
    short cell_idle_srxlev;
    u16     cell_pci; /* Range: 0 to 503 */
    u16     reserved;
} lte_cell_t;

typedef struct TagInterFreqs {
    u16     earfcn;
    u8      resel_priority;
    u8      threshX_high;
    u8      threshX_low;
    u8      num_inter_cells;
	lte_cell_t interCell[MAX_CELL_INTER];
} inter_freqs_t;

typedef struct TagTargetCell { 
    u8		freq_band_ind;
	u16 	earfcn; 
	u16 	tac;
	u16 	cell_pci;
	u32 	cellId; 
	
	u16     mcc;
	u8      mnc;
	u8      num;
    u8   	reserved;
} target_cell_t;

typedef struct TagGsmCell { 
	u16 	arfcn; 	
	u8      mcc;
	u8      mnc;
	u16 	lac; 
	u16 	cellId; /* GSM Cell ID (CI): 0-65535 */
	u8		band;  /* GSM900, DCS1800, EGSM */
    u8   	reserved;
} gsm_cell_t;


typedef struct TagAT_CMGRMI_OUT {
    main_info_t    ueMainInfo;
    scell_info_t   currentSCellInfo;
    BOOL        sib3_received;
    u8          num_intrafreq_cells;
    u8          num_interFreq;
    inter_freqs_t  interfreqs[MAX_FREQ];
    lte_cell_t     intrafreqCell[MAX_CELL_INTRA];
} at_cmgrmi_output_t;

/////////////////////////////////////////////////////////////////////////////

int SerialPortInit(TModem *ptModem);
int GetCellsDataFromAtCmd(TModem * ptModem, int mode);
void buildReselTargetCellPara(target_cell_t * ptargetCell);
int atcmdDataFromFile_test(void); 

int lte_cells_para_acquire(lte_state_t *lte_state); 
int lte_cell_select(lte_state_t *lte_state);
int lte_cell_select_init(TModem * ptModem);
int switchToLteFdd2100(TModem *ptModem); 
int switchToLteTdd2500(TModem *ptModem); 
int switchToLteTdd2300(TModem *ptModem); 
int switchToLteTdd2600(TModem *ptModem); 
int switchToLteFdd1800(TModem *ptModem);
int switchToLteTdd1900(TModem *ptModem);
int cn_operator_lock(TModem *ptModem, int oper);
int set_operator_sel(TModem *ptModem, int oper, int mode); 
int set_preferred_mode(TModem *ptModem, int preferred_mode);
int inquiry_ue_info(TModem *ptModem, char *pData, u16 bySize);

#endif  /* _SERIAL_H */
