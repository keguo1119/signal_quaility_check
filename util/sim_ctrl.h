#ifndef SIM_CTRL_H
#define SIM_CTRL_H

typedef enum  {
	SIM_CMD_INIT = '0',
	SIM_CMD_CMCC_START = '1',	
	SIM_CMD_CMCC_STOP = '2',		
	SIM_CMD_CUCC_START = '3',	
	SIM_CMD_CUCC_STOP = '4',		
	SIM_CMD_CTCC_START = '5',	
	SIM_CMD_CTCC_STOP = '6',
    SIM_CRTL_POWER_ON = '7',
    SIM_CRTL_POWER_OFF= '8',
    CMCC_TO_CUCC = 'a',
    CMCC_TO_CTCC = 'b',
    CUCC_TO_CTCC = 'c',
    CUCC_TO_CMCC = 'd',
    CTCC_TO_CMCC = 'e',
    CTCC_TO_CUCC = 'f',
    SIM_MODEM_POWER_ON = 'g',
    SIM_MODEM_POWER_OFF = 'h',
    SIM_MODEM_RESET = 'i',
    FDDTOTDD = 'j',
    TDDTOFDD = 'k',
    mo_invalid,
    SIM_EXIT
}ESimCmd;

//GPIO configuration
#define  CMCC_EN    6
#define  CUCC_EN    4
#define  CTCC_EN    2
#define  OUT_EN     11
#define  CSEL1      4
#define  CSEL2      2
#define  SWDN       3
#define  F_T_SW     37
#define  MODEM_POWER   0
#define  MODEM_RESET   1

typedef struct TagPowerSaveReq{
	char  cmd; //4
	char  len;  //2
}TPowerSaveReq;

typedef struct TagPowerSaveResp{
	char  cmd; //5
	char  len;  //2
}TPowerSaveResp;

/**********************************************************/
void CmccStart(void);
void CmccStop(void);
void CuccStart(void);
void CuccStop(void);
void CtccStart(void);
void CtccStop(void);

void CmccToCucc(void);
void CmccToCtcc(void);
void CuccToCmcc(void);
void CuccToCtcc(void);
void CtccToCmcc(void);
void CtccToCucc(void);

void FDDtoTDDInit(void);
void FDDtoTDD(void);
void TDDtoFDD(void);
void SimCtrlInit(void);

void sim_cmd_handler(ESimCmd cmd);

#endif 

