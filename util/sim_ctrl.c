#include "oss.h"
#include "oss_time.h"
#include "oss_gpio.h"
#include "sim_ctrl.h"
#include "stdlib.h"
#include <string.h>
#include <stdio.h>

#define  TMP_STRING_LEN     125

/*****************************************************/
int  gBatLevel = 0;
int  gCurSlot = 0; //CMCC:0; CUCC, 1; CTCC, 2
/*****************************************************/
void CmccEnable(void);
void CmccDisable(void);

void CuccEnable(void);
void CuccDisable(void);

void CtccEnable(void);
void CtccDisable(void);

void SimCtrlPowerOn(void);
void SimCtrlPowerOff(void);

void IoOutputEnable(int io);
void IoPinSet(int io);
void LedOff(int io);

void ModemReset(void);

void FDDtoTDDInit(void);
//void FDDtoTDD(void);
//void TDDtoFDD(void);

static ESimCmd show_menu(void );

/*****************************************************/
void IoOutputEnable(int io)
{
    oss_gpio_init(io);
    oss_gpio_output_mode(io);
}

void IoInputEnable(int io)
{
//  oss_gpio_input_mode(io);
}

/*****************************************************/
void IoPinSet(int io)
{
  oss_gpio_pin_set(io);
}

void IoPinClear(int io)
{
  oss_gpio_pin_clear(io);
}
/*****************************************************/
void CmccSelect(void)
{
	IoPinClear(CSEL2);	
	IoPinSet(OUT_EN);
	IoPinClear(CSEL1);
}

void CuccSelect(void)
{
	IoPinClear(CSEL2);			
	IoPinSet(OUT_EN);
	IoPinSet(CSEL1);
}

void CtccSelect(void)
{
	IoPinClear(OUT_EN);
	IoPinClear(CSEL1);
	IoPinSet(CSEL2);	
}
/*****************************************************/
void CmccEnable(void)
{
	IoPinSet(CMCC_EN);
}
/*****************************************************/
void CmccDisable(void)
{
	IoPinClear(CMCC_EN);
}

/*****************************************************/
void CuccEnable(void)
{
	IoPinSet(CUCC_EN);
}

/*****************************************************/
void CuccDisable(void)
{
	IoPinClear(CUCC_EN);
}

/*****************************************************/
void CtccEnable(void)
{
	IoPinSet(CTCC_EN);	
}

/*****************************************************/
void CtccDisable(void)
{
	IoPinClear(CTCC_EN);
}

/*****************************************************/
void SimCtrlPowerOn(void)
{
    oss_delay(500);
    IoPinClear(SWDN);
    oss_delay(500);
}

/*****************************************************/
void SimCtrlPowerOff(void)
{
    oss_delay(1000);
    IoPinSet(SWDN);
    oss_delay(1000);
}
/*****************************************************/
void ModemPowerOn(void)
{
	IoPinSet(MODEM_POWER);
	oss_delay(500);	// at least 100ms, typical 500ms		
	IoPinClear(MODEM_POWER);	
}

/*****************************************************/
void ModemPowerOff(void)
{
//#if 0	
	IoPinSet(MODEM_POWER);	
	oss_delay(3000);  // at least 2500ms, typical 3000ms	
	IoPinClear(MODEM_POWER);	
/*#else
	ModemReset();
#endif*/	
}
/*****************************************************/
void ModemReset(void)
{
	IoPinSet(MODEM_RESET);
	oss_delay(100);			// at least 50ms, typical 100ms, max 500ms
	IoPinClear(MODEM_RESET);		
}

/*****************************************************/
void CmccStart(void)
{
	SimCtrlPowerOff();
	
	CmccSelect();
	
	SimCtrlPowerOn();	
	CmccEnable();
	
	gCurSlot = 0;
}

void CmccStop(void)
{
	CmccDisable();	
}
/*****************************************************/
void CuccStart(void)
{
	SimCtrlPowerOff();
	
	CuccSelect();
	
	SimCtrlPowerOn();	
	CuccEnable();
	
	gCurSlot = 1;	
}

void CuccStop(void)
{
	CuccDisable();	
}
/*****************************************************/
void CtccStart(void)
{
	SimCtrlPowerOff();
	
	CtccSelect();
	
	SimCtrlPowerOn();	
	CtccEnable();
	
	gCurSlot = 2;	
}

void CtccStop(void)
{
	CtccDisable();	
}
/*****************************************************/
void CmccToCucc(void)
{
	CmccStop();

	CuccStart();
	ModemReset();
#ifdef DEBUG	
	printf("oper switch: CMCC To CUCC\n");
#endif	
}

/*********************************IoPinClear********************/
void CmccToCtcc(void)
{
	CmccStop();
	
	CtccStart();
	ModemReset();
#ifdef DEBUG	
		printf("oper switch: CMCC To CTCC\n");
#endif	
}

/*****************************************************/
void CuccToCmcc(void)
{
	CuccStop();
	
	CmccStart();
	ModemReset();
#ifdef DEBUG
		printf("oper switch: CUCC To CMCC\n");
#endif	
}

/*****************************************************/
void CuccToCtcc(void)
{
	CuccStop();

	CtccStart();
	ModemReset();
#ifdef DEBUG
		printf("oper switch: CUCC To CTCC\n");
#endif	
}
/*****************************************************/
void CtccToCucc(void)
{
	CtccStop();

	CuccStart();
	ModemReset();
#ifdef DEBUG
		printf("oper switch: CTCC To CUCC\n");
#endif
}
/*****************************************************/
void CtccToCmcc(void)
{
	CtccStop();

	CmccStart();	
	ModemReset();
#ifdef DEBUG
		printf("oper switch: CTCC To CMCC\n");
#endif
}
/*****************************************************/

void FDDtoTDDInit(void)
{
    IoOutputEnable(F_T_SW);
}

void FDDtoTDD(void)
{
    IoPinClear(F_T_SW);
}

void TDDtoFDD(void)
{
    IoPinSet(F_T_SW);
}

void SimCtrlInit(void)
{
	IoOutputEnable(CMCC_EN);
	IoOutputEnable(CUCC_EN);
	IoOutputEnable(CSEL1);	
	IoOutputEnable(OUT_EN);
	IoOutputEnable(CTCC_EN);
	IoOutputEnable(CSEL2);
	
	IoOutputEnable(MODEM_POWER);
	IoOutputEnable(MODEM_RESET);
	
	IoOutputEnable(SWDN);	
  	
	IoPinClear(CMCC_EN);
	IoPinClear(CUCC_EN);
	IoPinClear(CSEL1);	
	IoPinClear(OUT_EN);
	IoPinClear(CTCC_EN);
	IoPinClear(CSEL2);	
	
	// init to cmcc simcard slot
	CmccStart();		

	ModemReset();

	ModemPowerOn();

}

/*********************************************************/
static ESimCmd show_menu(void )
{
    char tmp_str[TMP_STRING_LEN+1];
    ESimCmd option_chosen = mo_invalid;

    while (option_chosen == mo_invalid)
    {
        printf("0 - SIM_CMD_INIT\n");
        printf("1 - CMCC_START\n");
        printf("2 - CMCC_STOP\n");
        printf("3 - CUCC_START\n");
        printf("4 - CUCC_STOP\n");
        printf("5 - CTCC_START\n");
        printf("6 - CTCC_STOP\n");
        printf("7 - SIM_CRTL_POWER_ON\n");
        printf("8 - SIM_CRTL_POWER_OFF\n");
        printf("a - CMCC_TO_CUCC\n");
        printf("b - CMCC_TO_CTCC\n");
        printf("c - CUCC_TO_CTCC\n");
        printf("d - CUCC_TO_CMCC\n");
        printf("e - CTCC_TO_CMCC\n");
        printf("f - CTCC_TO_CUCC\n");
        printf("g - SIM_MODEM_POWER_ON\n");
        printf("h - SIM_MODEM_POWER_OFF\n");
        printf("i - SIM_MODEM_RESET\n");
	printf("j - FDDTOTDD\n");
	printf("k - TDDTOFDD\n");
        printf("q - quit\n");



        fgets(tmp_str, TMP_STRING_LEN, stdin);

        switch (tmp_str[0]) {
        case '0':
            option_chosen = SIM_CMD_INIT;
            break;
        case '1':
            option_chosen = SIM_CMD_CMCC_START;
            break;
        case '2':
            option_chosen = SIM_CMD_CMCC_STOP;
            break;
        case '3':
            option_chosen = SIM_CMD_CUCC_START;
            break;
        case '4':
            option_chosen = SIM_CMD_CUCC_STOP;
            break;
        case '5':
            option_chosen = SIM_CMD_CTCC_START;
            break;
        case '6':
            option_chosen = SIM_CMD_CTCC_STOP;
            break;
        case '7':
            option_chosen = SIM_CRTL_POWER_ON;
            break;
        case '8':
            option_chosen = SIM_CRTL_POWER_OFF;
            break;
        case 'a':
            option_chosen = CMCC_TO_CUCC;
            break;
        case 'b':
            option_chosen = CMCC_TO_CTCC;
            break;
        case 'c':
            option_chosen = CUCC_TO_CTCC;
            break;
        case 'd':
            option_chosen = CUCC_TO_CMCC;
            break;
        case 'e':
            option_chosen = CTCC_TO_CMCC;
            break;
        case 'f':
            option_chosen = CTCC_TO_CUCC;
            break;
        case 'g':
            option_chosen = SIM_MODEM_POWER_ON;
            break;
        case 'h':
            option_chosen = SIM_MODEM_POWER_OFF;
            break;
        case 'i':
            option_chosen = SIM_MODEM_RESET;
            break;
        case 'j':
            option_chosen = FDDTOTDD ;
            break;
        case 'k':
            option_chosen = TDDTOFDD ;
            break;
        case 'q':
            option_chosen = SIM_EXIT;
            break;

        default:
            break;
        }
    }
    return option_chosen;
}


/*********************************************************/

/*********************************************************/
void sim_cmd_handler(ESimCmd cmd)
{
	switch(cmd)
	{
	case SIM_CMD_INIT:
		SimCtrlInit();
#ifdef DEBUG
        printf("SIM control init\n");
#endif
	break;
	
	case	SIM_MODEM_POWER_ON:
		ModemPowerOn();
#ifdef DEBUG
        printf("ModemPowerOn\n");
#endif
	break;
	
	case SIM_MODEM_POWER_OFF:
		ModemPowerOff();
#ifdef DEBUG
        printf("ModemPowerOn\n");
#endif
	break;
	
	case SIM_MODEM_RESET:
		ModemReset();
#ifdef DEBUG
        printf("ModemPowerOn\n");
#endif
	break;
	
	case SIM_CMD_CMCC_START:
		CmccStart();
#ifdef DEBUG
        printf("CmccStart\n");
#endif
	break;

	case SIM_CMD_CMCC_STOP:
		CmccStop();
#ifdef DEBUG
        printf("CmccStop\n");
#endif
	break;

	case SIM_CMD_CUCC_START:
		CuccStart();
#ifdef DEBUG
        printf("CuccStart\n");
#endif
	break;

	case SIM_CMD_CUCC_STOP:
		CuccStop();
#ifdef DEBUG
        printf("CuccStop\n");
#endif
	break;
	
	case SIM_CMD_CTCC_START:
		CtccStart();
#ifdef DEBUG
        printf("CtccStart\n");
#endif
	break;

	case SIM_CMD_CTCC_STOP:
		CtccStop();
#ifdef DEBUG
        printf("CtccStop\n");
#endif
	break;

    case SIM_CRTL_POWER_ON:
        SimCtrlPowerOn();
#ifdef DEBUG
        printf("SIM_CRTL_POWER_ON\n");
#endif
    break;

    case SIM_CRTL_POWER_OFF:
        SimCtrlPowerOff();
#ifdef DEBUG
        printf("SIM_CRTL_POWER_OFF\n");
#endif
    break;

    case CMCC_TO_CUCC:
        CmccToCucc();
#ifdef DEBUG
        printf("CMCC_TO_CUCC\n");
#endif
    break;

    case CMCC_TO_CTCC:
        CmccToCtcc();
#ifdef DEBUG
        printf("CMCC_TO_CTCC\n");
#endif
    break;

    case CUCC_TO_CMCC:
        CuccToCmcc();
#ifdef DEBUG
        printf("CUCC_TO_CMCC\n");
#endif
    break;

    case CUCC_TO_CTCC:
        CuccToCtcc();
#ifdef DEBUG
        printf("CUCC_TO_CTCC\n");
#endif
    break;

    case CTCC_TO_CMCC:
        CtccToCmcc();
#ifdef DEBUG
        printf("CTCC_TO_CMCC\n");
#endif
    break;

    case CTCC_TO_CUCC:
        CtccToCucc();
#ifdef DEBUG
        printf("CTCC_TO_CUCC\n");
#endif
    break;

    case FDDTOTDD:
        FDDtoTDD();
#ifdef DEBUG
        printf("FDDtoTDD\n");
#endif
    break;

    case TDDTOFDD:
        TDDtoFDD();
#ifdef DEBUG
        printf("TDDtoFDD\n");
#endif
    break;

    case SIM_EXIT:
#ifdef DEBUG
        printf("Exit\n");
#endif
    break;

	default:
		break;
	}

}
/*
int main()
{
    ESimCmd current_option;
    current_option = mo_invalid;
    FDDtoTDDInit();
    SimCtrlInit();
    while(current_option != SIM_EXIT)
    {
        current_option = show_menu();
        sim_cmd_handler(current_option);

    }
}
*/
