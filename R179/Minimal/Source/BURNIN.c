/******************************************************************************
*
* Copyright (C) 2014-15 Bombardier
*
* File Name: Burnin.c
*
* Revision History:
*   08/31/2009 - das - Created
*
******************************************************************************/
///   \file
///   This file contains the code that handles all of the Burn-In tests

#define burnin_C

/*--------------------------------------------------------------------------
                              INCLUDE FILES
  --------------------------------------------------------------------------*/

#ifndef _DEBUG
#include <reg167.h>
#endif

#include <stdio.h>
#include <string.h>
#include "Types.h"
#include "Timer.h"
#include "DigOut.h"
#include "DigIn.h"
#include "HWInit.h"
#include "HexLED.h"
#include "DateTime.h"
#include "AlogOut.h"
#include "AnalogIn.h"
#include "mvbvar.h"
#include "Debug.h"
#include "SerComm.h"


/*--------------------------------------------------------------------------
                             MODULE CONSTANTS
  --------------------------------------------------------------------------*/
#define HIDAC_FAIL_LED_ON()		DO_ResetBitWithId(FAIL_LED)
#define HIDAC_FAIL_LED_OFF()	DO_SetBitWithId(FAIL_LED)

#define BATTERY_VOLTAGE			ANALOGIN_14
#define SSR_IN2					PROT_01
#define SSR_IN1					PROT_07


#define HI_DAC_VOLTAGE_MV		5000
#define LO_DAC_VOLTAGE_MV		0

#define MAX_QUANTA_AT_1P25V		(256 + 25)
#define MIN_QUANTA_AT_1P25V		(256 - 25)

#define MAX_QUANTA_AT_3P75V		(768 + 25)
#define MIN_QUANTA_AT_3P75V		(768 - 25)

/* Serial loop back test */
#define	ERR_INVALID_SERIAL_CHAR	0x01
#define	ERR_NO_SERIAL_CHAR_RX	0x02

/* Digital I/O */
#define	ERR_DIGITAL_IO_10		0x10
#define	ERR_DIGITAL_IO_11		0x11
#define	ERR_DIGITAL_IO_12		0x12
#define	ERR_DIGITAL_IO_13		0x13
#define	ERR_DIGITAL_IO_14		0x14
#define	ERR_DIGITAL_IO_15		0x15
#define	ERR_DIGITAL_IO_16		0x16
#define	ERR_DIGITAL_IO_17		0x17

#define	ERR_DIGITAL_IO_20		0x20
#define	ERR_DIGITAL_IO_21		0x21
#define	ERR_DIGITAL_IO_22		0x22
#define	ERR_DIGITAL_IO_23		0x23
#define	ERR_DIGITAL_IO_24		0x24
#define	ERR_DIGITAL_IO_25		0x25
#define	ERR_DIGITAL_IO_26		0x26
#define	ERR_DIGITAL_IO_27		0x27

#define	ERR_DIGITAL_IO_30		0x30
#define	ERR_DIGITAL_IO_31		0x31
#define	ERR_DIGITAL_IO_32		0x32
#define	ERR_DIGITAL_IO_33		0x33
#define	ERR_DIGITAL_IO_34		0x34
#define	ERR_DIGITAL_IO_35		0x35
#define	ERR_DIGITAL_IO_36		0x36
#define	ERR_DIGITAL_IO_37		0x37

#define	ERR_DIGITAL_IO_40		0x40
#define	ERR_DIGITAL_IO_41		0x41
#define	ERR_DIGITAL_IO_42		0x42
#define	ERR_DIGITAL_IO_43		0x43
#define	ERR_DIGITAL_IO_44		0x44
#define	ERR_DIGITAL_IO_45		0x45
#define	ERR_DIGITAL_IO_46		0x46
#define	ERR_DIGITAL_IO_47		0x47

#define LOW_SIDE_ERROR			0x60
#define	ERR_DIGITAL_IO_50		0x50
#define	ERR_DIGITAL_IO_51		0x51
#define	ERR_DIGITAL_IO_52		0x52
#define	ERR_DIGITAL_IO_53		0x53
#define	ERR_DIGITAL_IO_54		0x54
#define	ERR_DIGITAL_IO_55		0x55
#define	ERR_DIGITAL_IO_56		0x56
#define	ERR_DIGITAL_IO_57		0x57
#define	ERR_DIGITAL_IO_58		0x58
#define	ERR_DIGITAL_IO_59		0x59
#define	ERR_DIGITAL_IO_5A		0x5A
#define	ERR_DIGITAL_IO_5B		0x5B
#define	ERR_DIGITAL_IO_5C		0x5C
#define	ERR_DIGITAL_IO_5D		0x5D
#define	ERR_DIGITAL_IO_5E		0x5E
#define	ERR_DIGITAL_IO_5F		0x5F

/* VDrive Errors */
#define VDRIVE_2X1X				0xB4
#define VDRIVE_2X1O				0xB5
#define VDRIVE_2O1X				0xB6
#define VDRIVE_2O1O				0xB7

/* Analog errors */
#define	MOTOR_CURR_1_2_NEG_ERR	0xC0
#define	MOTOR_CURR_3_4_NEG_ERR	0xC1
#define	MOTOR_CURR_1_2_POS_ERR	0xC2
#define	MOTOR_CURR_3_4_POS_ERR	0xC3
#define	LINE_VOLT_NEG_ERR	    0xD0
#define	RETURN_CURR_NEG_ERR	    0xD1
#define	LINE_VOLT_POS_ERR	    0xD2
#define	RETURN_CURR_POS_ERR	    0xD3
#define	B_SWITCH_NEG_ERR	    0xE0
#define	BRAKE_PRESSURE_POS_ERR	0xE1
#define	LOAD_WEIGHT_NEG_ERR		0xE2
#define	B_SWITCH_POS_ERR	    0xE3
#define	BRAKE_PRESSURE_NEG_ERR	0xE4
#define	LOAD_WEIGHT_POS_ERR	    0xE5
#define	PS_PLUS_15_ERR	        0xF0
#define	VDRIVE_PLUS_15_ERR	    0xF1
#define	PS_MINUS_15_ERR	        0xF2
#define	BATTERY_VOLTAGE_ERR	    0xF3

/* MVB errors */
#define MVB_ERR                 0xF4

#define	HIGH_MASK	            0x00
#define	LOW_MASK	            0xFF
#define U40_port_a                     (*(volatile UINT_16 *) (0x500022L))
#define U40_port_b                     (*(volatile UINT_16 *) (0x500024L))

/* Tach Processor */
#define U35_tach_reader_1              (*(volatile UINT_16 *) (0x500100L))
#define U35_tach_counter_1             (*(volatile UINT_16 *) (0x500102L))
#define U35_tach_reader_2              (*(volatile UINT_16 *) (0x500104L))
#define U35_tach_counter_2             (*(volatile UINT_16 *) (0x500106L))
#define U35_port_a                     (*(volatile UINT_16 *) (0x500108L))
#define U35_port_b                     (*(volatile UINT_16 *) (0x50010aL))

#define U34_tach_reader_1              (*(volatile UINT_16 *) (0x500200L))
#define U34_tach_counter_1             (*(volatile UINT_16 *) (0x500202L))
#define U34_tach_reader_2              (*(volatile UINT_16 *) (0x500204L))
#define U34_tach_counter_2             (*(volatile UINT_16 *) (0x500206L))
#define U34_port_a                     (*(volatile UINT_16 *) (0x500208L))
#define U34_port_b                     (*(volatile UINT_16 *) (0x50020aL))
#define  CAPCOM_Register_8             0x18
#define  CAPCOM_Register_7             0x17
#define  CAPCOM_Register_6             0x16
#define  CAPCOM_Register_5             0x15

/* MVB */
#define RST_IP_N	         (volatile UINT_16 huge *)(0x00500022L)
#define IPAC_A_ADRS			 0x00400000L
#define IPAC_A_IO_REG_CSR    (volatile UINT_16 huge *)(IPAC_A_ADRS + 0x40000)

#define IPAC_A_SCR_DEFAULT   (volatile UINT_16*)(IPAC_A_ADRS + 0x3f80)
#define IPAC_A_MCR_DEFAULT   (volatile UINT_16*)(IPAC_A_ADRS + 0x3f84)
#define IPAC_A_DPR_DEFAULT   (volatile UINT_16*)(IPAC_A_ADRS + 0x3fA8)

#define IPAC_A_SCR_256K_TS   (volatile UINT_16*)(IPAC_A_ADRS + 0xff80)
#define IPAC_A_MCR_256K_TS   (volatile UINT_16*)(IPAC_A_ADRS + 0xff84)
#define IPAC_A_DR_256K_TS    (volatile UINT_16*)(IPAC_A_ADRS + 0xff88)
#define IPAC_A_DPR_256K_TS   (volatile UINT_16*)(IPAC_A_ADRS + 0xffA8)


#define IPAC_A_PIT_100H_TS   (volatile UINT_16*)(IPAC_A_ADRS + 0x0100)
#define IPAC_A_PIT_102H_TS   (volatile UINT_16*)(IPAC_A_ADRS + 0x0102)
#define IPAC_A_PIT_104H_TS   (volatile UINT_16*)(IPAC_A_ADRS + 0x0104)
#define IPAC_A_PIT_106H_TS   (volatile UINT_16*)(IPAC_A_ADRS + 0x0106)
#define IPAC_A_PIT_108H_TS   (volatile UINT_16*)(IPAC_A_ADRS + 0x0108)
#define IPAC_A_PIT_10AH_TS   (volatile UINT_16*)(IPAC_A_ADRS + 0x010A)
#define IPAC_A_PIT_10CH_TS   (volatile UINT_16*)(IPAC_A_ADRS + 0x010C)
#define IPAC_A_PIT_10EH_TS   (volatile UINT_16*)(IPAC_A_ADRS + 0x010E)
#define IPAC_A_PIT_110H_TS   (volatile UINT_16*)(IPAC_A_ADRS + 0x0110)
#define IPAC_A_PIT_112H_TS   (volatile UINT_16*)(IPAC_A_ADRS + 0x0112)
#define IPAC_A_PIT_114H_TS   (volatile UINT_16*)(IPAC_A_ADRS + 0x0114)
#define IPAC_A_PIT_116H_TS   (volatile UINT_16*)(IPAC_A_ADRS + 0x0116)
#define IPAC_A_PIT_118H_TS   (volatile UINT_16*)(IPAC_A_ADRS + 0x0118)
#define IPAC_A_PIT_11AH_TS   (volatile UINT_16*)(IPAC_A_ADRS + 0x011A)
#define IPAC_A_PIT_11CH_TS   (volatile UINT_16*)(IPAC_A_ADRS + 0x011C)
#define IPAC_A_PIT_11EH_TS   (volatile UINT_16*)(IPAC_A_ADRS + 0x011E)

#define IPAC_A_PCS0_80H_TS   (volatile UINT_16*)(IPAC_A_ADRS + 0x30000 + 0x90 + 0)
#define IPAC_A_PCS1_80H_TS   (volatile UINT_16*)(IPAC_A_ADRS + 0x30000 + 0x90 + 2)
#define IPAC_A_PCS2_80H_TS   (volatile UINT_16*)(IPAC_A_ADRS + 0x30000 + 0x90 + 4)
#define IPAC_A_PCS3_80H_TS   (volatile UINT_16*)(IPAC_A_ADRS + 0x30000 + 0x90 + 6)

#define IPAC_A_LA_DA_VP0_80H_TS   (volatile UINT_16*)(IPAC_A_ADRS + 0x10000 + 0x0110)
#define IPAC_A_LA_DA_VP1_80H_TS   (volatile UINT_16*)(IPAC_A_ADRS + 0x10000 + 0x0130)

#define IPAC_A_LA_DA_VP0_8AH_TS   (volatile UINT_16*)(IPAC_A_ADRS + 0x10000 + 0x0100)
#define IPAC_A_LA_DA_VP1_8AH_TS   (volatile UINT_16*)(IPAC_A_ADRS + 0x10000 + 0x0120)


extern void mvb_fast_read_T16 (void);
extern void mvb_fast_write_T16 (void);
/*--------------------------------------------------------------------------
                              MODULE MACROS
  --------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
                             MODULE DATA TYPES
  --------------------------------------------------------------------------*/
typedef struct
{
    eDigOutBitId	outBit;
    eDigInId		inBit;
    eDigInBitState  expectedFBDriveHigh;
    UINT_8			errorId;
} DigitalTest_t;

typedef struct
{
    INT_16 *pAddr;
    INT_16 pValue;
    INT_16 *nAddr;
    INT_16 nValue;
    UINT_16 validLo;
    UINT_16 validHi;
    UINT_16 a2dId;
    UINT_8 errCode;
} BurninAlog_t;


/* Function pointer to either DO_ResetBitWithId() or DO_SetBitWithId() */
typedef void (*VdrvFnPtr) (eDigOutBitId);

typedef struct
{
    VdrvFnPtr fnPtrVdrive2;	/* Controls Vdrive2 */
    VdrvFnPtr fnPtrVdrive1;	/* Controls Vdrive1 */
    UINT_16 hiAllowedValue;	/* Max allowed A/D value (mV) */
    UINT_16 loAllowedValue;	/* Min allowed A/D value (mV) */
    UINT_8  errorCode;		/* Error Code  if failure detected */
} VDriveTest_t;


/*--------------------------------------------------------------------------
                              MODULE VARIABLES
  --------------------------------------------------------------------------*/
static INT_16 burninDAC[8];
static UINT_8 burnin_error_code;
static BOOLEAN burninConfig;
UINT_16 u40_port_a_out;
UINT_16 u34_port_a_out;
UINT_16 u35_port_a_out;
volatile UINT_16 tach_done_1;
volatile UINT_16 tach_done_2;
volatile UINT_16 tach_done_3;
volatile UINT_16 tach_done_4;
UINT_16 u34_port_b_in;
UINT_16 u35_port_b_in;
UINT_16 u35_tach_reader_1;
UINT_16 u35_tach_reader_2;
UINT_16 u34_tach_reader_1;
UINT_16 u34_tach_reader_2;


/*--------------------------------------------------------------------------
                             MODULE PROTOTYPES
  --------------------------------------------------------------------------*/
static void BurnInInitAnalogs (void);
static void BurninAnalogTests (void);
static void BurninDigitalTests (void);
static void BurninDigitalTests2 (void);
static void BurninSerialPortTest (void);
static void GiveSettleTime (void);
static void ZeroAnalogOutputs (void);
static void BurninVDriveTest (void);
static void BurninMvbTest (void);

CHAR failureStr[2000];


//#define CONTINUOUS
//--------------------------------------------------------------------------
// Module:
//  BurnInCode
//
///   This function is responsible for conducting the burn-in testing. It calls
///   lower level tests forever until an error occurs. It displays the error
///   code on the Hex LED display if an error occurs and the LED on the box
///   is forced to remain illuminated (instead of blink).
///
//--------------------------------------------------------------------------
void BurnInCode (void)
{
	UINT_32 iteration = 0;
	CHAR str[20];

	SC_PutsAlways ("\n\r\n\r\n\r");
	SC_PutsAlways ("***********************************************************************\n\r");
	SC_PutsAlways ("***********************************************************************\n\r");
    SC_PutsAlways ("**** R179 HiDAC BURNIN software activated. Will remain in BURNIN\n\r");
    SC_PutsAlways ("**** mode until either power is cycled or a reset.\n\r");
	SC_PutsAlways ("***********************************************************************\n\r");
	SC_PutsAlways ("***********************************************************************\n\r");
	SC_PutsAlways ("\n\r\n\r\n\r");

    /* Initialize variables */
    burnin_error_code = 0;

	DebugPrintSoftwareVersion();
    BurnInInitAnalogs();

    /* This call turns the P7 port into digital outputs on the C167 */
    DO_Init (0xFF);

    DO_WriteBank (DIGOUT_BANK0, 0xFFFF);    
	DO_WriteBank (DIGOUT_BANK1, 0xFFFF);
    DO_WriteBank (DIGOUT_BANK2, 0xFFFF);
	
	burninConfig = TRUE;

    /* Stay in this loop until a test fails */
    while (burnin_error_code == 0)
    {
#ifdef CONTINUOUS
        HexLEDUpdate (0x02);
        BurninVDriveTest();

		HexLEDUpdate (0x03);
        BurninDigitalTests();

		HexLEDUpdate (0x04);
		BurninDigitalTests2();

		HexLEDUpdate (0x05);
        BurninAnalogTests();

		HexLEDUpdate (0x07);
        BurninMvbTest();
#else
		failureStr[0] = 0;
        HexLEDUpdate (0x02);
        /* Test the ability to turn on and off both VDrives (SSR_OUTs) */
        if (burnin_error_code == 0)
        {
            BurninVDriveTest();
        }
        HexLEDUpdate (0x03);
        /* Test all digital outputs and inputs */
        if (burnin_error_code == 0)
        {
            BurninDigitalTests();
        }
        HexLEDUpdate (0x04);
        if (burnin_error_code == 0)
        {
            BurninDigitalTests2();
        }
        HexLEDUpdate (0x05);
        /* Test the analog inputs by driving the chart recorder outputs */
        if (burnin_error_code == 0)
        {
            BurninAnalogTests();
        }
        HexLEDUpdate (0x07);
		GiveSettleTime ();
        if (burnin_error_code == 0)
        {
            BurninMvbTest();
        }

        if (burnin_error_code == 0)
        {
			SC_PutsAlways ("--- BurnIn Test iteration complete... ");
			sprintf (str,"%d without failures\n\r",++iteration);
			SC_PutsAlways (str);
		}
#endif
    }

	SC_PutsAlways ("\n\r\n\r\n\r");
	SC_PutsAlways ("***********************************************************************\n\r");
	SC_PutsAlways ("***********************************************************************\n\r");
    SC_PutsAlways ("**** R179 HiDAC BURNIN failure occurred. Please check error code ******\n\r");
    SC_PutsAlways ("**************** displayed on HEX LED on front panel. *****************\n\r");
	SC_PutsAlways ("***********************************************************************\n\r");
	SC_PutsAlways ("***********************************************************************\n\r");
	SC_PutsAlways ("\n\r\n\r\n\r");

    /* Display error code on HEX DISPLAY */
    HexLEDUpdate (burnin_error_code);

    /* Turn on FAIL LED */
    HIDAC_FAIL_LED_ON();

	/* Ensure all output FETs are off so that minimal current is pulled through
	   Burnin Box pullup resistors */
    DO_WriteBank (DIGOUT_BANK0, 0xFFFF);    
	DO_WriteBank (DIGOUT_BANK1, 0xFFFF);
    DO_WriteBank (DIGOUT_BANK2, 0xFFFF);

    /* Disable the RTC so that the user can check what time the Burnin
       test failed */
    DT_EnableOscillator (FALSE);

    /* Turn the LED on always, indicates a burn-in failure */
    DO_ResetBitWithId (DIGOUT_BIT09);

	sprintf (str,"%d without failures\n\r",++iteration);
	strcat(failureStr, str);
    /* Loop forever here while displaying the error code on the LED
       HEX Display */
    while (1)
    {
		TM_Wait (5000);
		SC_PutsAlways(failureStr);
	
	}
}

//--------------------------------------------------------------------------
// Module:
//  BurnInISR
//
///   This function is responsible for performing any necessary tasks
///   by the six millisecond interrupt that is not already being
///   performed in the normal ISR to support Burn-In.
///
//--------------------------------------------------------------------------
void BurnInISR (void)
{
    static UINT_8 scheduler_counter;

    scheduler_counter++;

	if (burnin_error_code == 0)
	{
		/* Toggle the LED on the Burn-In box every 768 msecs */
		if (scheduler_counter == 0x80)
		{
			DO_SetBitWithId (DIGOUT_BIT09);
		}
		else if (scheduler_counter == 0x0)
		{
			DO_ResetBitWithId (DIGOUT_BIT09);
		}
	}

    /* Update the analog outputs */
    AO_Service();
	AI_Service();
	DI_ReadBanks();

}

//--------------------------------------------------------------------------
// Module:
//  GetBurninConfig
//
///   This function is an accessor for the burninConfig variable.
///
///   return BOOLEAN - TRUE if burn-in code is activated; FALSE otherwise
///
//--------------------------------------------------------------------------
BOOLEAN GetBurninConfig (void)
{
	return burninConfig;
}

//--------------------------------------------------------------------------
// Module:
//  BurnInInitAnalogs
//
///   This function is responsible for initializing the analog
///   outputs and the analog inputs for Burn In testing. The analog
///   outputs are fed back into the analog inputs.
///
//--------------------------------------------------------------------------
static void BurnInInitAnalogs (void)
{

    AO_Init();

    /* Set the chart recording scaling */
    AO_SetScaling (ALOG_OUT0, HI_DAC_VOLTAGE_MV, LO_DAC_VOLTAGE_MV);
    AO_SetScaling (ALOG_OUT1, HI_DAC_VOLTAGE_MV, LO_DAC_VOLTAGE_MV);
    AO_SetScaling (ALOG_OUT2, HI_DAC_VOLTAGE_MV, LO_DAC_VOLTAGE_MV);
    AO_SetScaling (ALOG_OUT3, HI_DAC_VOLTAGE_MV, LO_DAC_VOLTAGE_MV);
    AO_SetScaling (ALOG_OUT4, HI_DAC_VOLTAGE_MV, LO_DAC_VOLTAGE_MV);
    AO_SetScaling (ALOG_OUT5, HI_DAC_VOLTAGE_MV, LO_DAC_VOLTAGE_MV);
    AO_SetScaling (ALOG_OUT6, HI_DAC_VOLTAGE_MV, LO_DAC_VOLTAGE_MV);
    AO_SetScaling (ALOG_OUT7, HI_DAC_VOLTAGE_MV, LO_DAC_VOLTAGE_MV);

    /* Set the variables for the chart recorder driver software to monitor */
    AO_SetVariable (ALOG_OUT0, &burninDAC[0], INT_16_TYPE);
    AO_SetVariable (ALOG_OUT1, &burninDAC[1], INT_16_TYPE);
    AO_SetVariable (ALOG_OUT2, &burninDAC[2], INT_16_TYPE);
    AO_SetVariable (ALOG_OUT3, &burninDAC[3], INT_16_TYPE);
    AO_SetVariable (ALOG_OUT4, &burninDAC[4], INT_16_TYPE);
    AO_SetVariable (ALOG_OUT5, &burninDAC[5], INT_16_TYPE);
    AO_SetVariable (ALOG_OUT6, &burninDAC[6], INT_16_TYPE);
    AO_SetVariable (ALOG_OUT7, &burninDAC[7], INT_16_TYPE);

    /* Enable the channels of the chart recorder */
    AO_SetEnable (ALOG_OUT0, TRUE);
    AO_SetEnable (ALOG_OUT1, TRUE);
    AO_SetEnable (ALOG_OUT2, TRUE);
    AO_SetEnable (ALOG_OUT3, TRUE);
    AO_SetEnable (ALOG_OUT4, TRUE);
    AO_SetEnable (ALOG_OUT5, TRUE);
    AO_SetEnable (ALOG_OUT6, TRUE);
    AO_SetEnable (ALOG_OUT7, TRUE);


    /* Initialize the analog inputs */
    AI_Init();

    /* Only used to test VDrive, not used in application */
    AI_SetScaleFactors (ANALOGIN_10, MAX_ADC_QUANTA, MIN_ADC_QUANTA,
                        5000, 0);
    AI_SetEnable (ANALOGIN_10, TRUE);

    AI_SetScaleFactors (ANALOGIN_00, MAX_ADC_QUANTA, MIN_ADC_QUANTA,
                        1, -1);
    AI_SetEnable (ANALOGIN_00, TRUE);

    AI_SetScaleFactors (ANALOGIN_01, MAX_ADC_QUANTA, MIN_ADC_QUANTA,
                        1, -1);
    AI_SetEnable (ANALOGIN_01, TRUE);

    AI_SetScaleFactors (ANALOGIN_02, MAX_ADC_QUANTA, MIN_ADC_QUANTA,
                        1, -1);
    AI_SetEnable (ANALOGIN_02, TRUE);

    AI_SetScaleFactors (ANALOGIN_03, MAX_ADC_QUANTA, MIN_ADC_QUANTA,
                        1, -1);
    AI_SetEnable (ANALOGIN_03, TRUE);

    AI_SetScaleFactors (ANALOGIN_04, MAX_ADC_QUANTA, MIN_ADC_QUANTA,
                        1, -1);
    AI_SetEnable (ANALOGIN_04, TRUE);

    AI_SetScaleFactors (ANALOGIN_05, MAX_ADC_QUANTA, MIN_ADC_QUANTA,
                        1, -1);
    AI_SetEnable (ANALOGIN_05, TRUE);

    AI_SetScaleFactors (ANALOGIN_06, MAX_ADC_QUANTA, MIN_ADC_QUANTA,
                        1, -1);
    AI_SetEnable (ANALOGIN_06, TRUE);

    AI_SetScaleFactors (ANALOGIN_07, MAX_ADC_QUANTA, MIN_ADC_QUANTA,
                        1, -1);
    AI_SetEnable (ANALOGIN_07, TRUE);

    AI_SetScaleFactors (ANALOGIN_08, MAX_ADC_QUANTA, MIN_ADC_QUANTA,
                        1, -1);
    AI_SetEnable (ANALOGIN_08, TRUE);

    AI_SetScaleFactors (ANALOGIN_09, MAX_ADC_QUANTA, MIN_ADC_QUANTA,
                        1, -1);
    AI_SetEnable (ANALOGIN_09, TRUE);

    AI_SetScaleFactors (ANALOGIN_11, MAX_ADC_QUANTA, MIN_ADC_QUANTA,
                        1, -1);
    AI_SetEnable (ANALOGIN_11, TRUE);

    AI_SetScaleFactors (ANALOGIN_12, MAX_ADC_QUANTA, MIN_ADC_QUANTA,
                        1, -1);
    AI_SetEnable (ANALOGIN_12, TRUE);

    AI_SetScaleFactors (ANALOGIN_13, MAX_ADC_QUANTA, MIN_ADC_QUANTA,
                        1, -1);
    AI_SetEnable (ANALOGIN_13, TRUE);


    AI_SetScaleFactors (BATTERY_VOLTAGE, MAX_ADC_QUANTA, MIN_ADC_QUANTA,
                        1, -1);
    AI_SetEnable (BATTERY_VOLTAGE, TRUE);


}



//--------------------------------------------------------------------------
// Module:
//  GiveSettleTime
//
///   This function is used to delay the sampling of data after changing
///   digital outputs or chart drive outputs that are used by the test box
///   to control relays.
///
//--------------------------------------------------------------------------
static void GiveSettleTime (void)
{
    TM_Wait (500);
}

//--------------------------------------------------------------------------
// Module:
//  BurninSerialPortTest
//
///   This function is used to transmit data via the RS232 port. It assumes
///   that a loop-back to the receive pin of the RS232 is connected. The test
///   will fail if no data is received "ERR_NO_SERIAL_CHAR_RX" or if the byte
///   transmitted <> byte received "ERR_INVALID_SERIAL_CHAR".  The variable
///   uart_counter is incremented after every successful test so that different
///   values are transmitted and received through the RS232 port.
///
//--------------------------------------------------------------------------
static void BurninSerialPortTest (void)
{
    UINT_16 rxChar = (UINT_16)EOF;	/* Needs to be UINT_16 instead of a CHAR to avoid
									   a problem when 0xff is transmitted and EOF(-1)
									   is returned */

    BOOLEAN character_received;

    static UINT_8 txChar = 255;

    Timer *tm;

    /* Transmit uart_counter value to RS-232 port*/
    while (!SC_PutChar ((CHAR)txChar));

    /* Wait for character for no more than 1 second */
    character_received = FALSE;
    tm = TM_Allocate (0, NULL);
    while (TM_GetExpiredTime (tm) < 1000)
    {
        rxChar = SC_GetChar();
        /* EOF is returned if there is no character in the serial receive
           buffer */
        if (rxChar != (UINT_16)EOF)
        {
            character_received = TRUE;
            break;
        }
    }
    TM_Free (tm);

    /* Verify; set burnin_error_code if TX <> RX or timeout */
    if ((character_received == TRUE) && ((UINT_8)rxChar != txChar))
    {
        burnin_error_code = ERR_INVALID_SERIAL_CHAR;
    }
    else if (character_received == FALSE)
    {
        burnin_error_code = ERR_NO_SERIAL_CHAR_RX;
    }
    /* Update the transmit character for the next go of this test */
    txChar++;
}

//--------------------------------------------------------------------------
// Module:
//  BurninDigitalTests
//
///   This function turns on/off the output drives on the HIDAC so that B+ (through
///   a resistor) or ground is presented to the digital inputs. Whenever an output is made
///   high, the output transistor is shut off. The signal in the test box
///   is pulled up and therefore B+ is presented to the digital input. If the output
///   is made low, the output transistor is turned on and the digital input is presented
///   ground. This function sets the bit, verifies the input is high, and then resets
///   the bit and verifies the input is low. Whenever a "low" failure is detected,
///   LOW_SIDE_ERROR is added to the error code so that the user can discern the
///   difference between a HIGH and LOW failure.
///
//--------------------------------------------------------------------------
static void BurninDigitalTests (void)
{
    const DigitalTest_t digTest[] =
    {
        {DIGOUT_BIT00,	DIGIN_00, HIGH, ERR_DIGITAL_IO_10},
        {DIGOUT_BIT01,	DIGIN_01, HIGH, ERR_DIGITAL_IO_11},
        {DIGOUT_BIT02,	DIGIN_02, HIGH, ERR_DIGITAL_IO_12},
        {DIGOUT_BIT03,	DIGIN_03, HIGH, ERR_DIGITAL_IO_13},
        {DIGOUT_BIT04,	DIGIN_04, HIGH, ERR_DIGITAL_IO_14},
        {DIGOUT_BIT05,	DIGIN_05, HIGH, ERR_DIGITAL_IO_15},
        {DIGOUT_BIT06,	DIGIN_06, HIGH, ERR_DIGITAL_IO_16},
        {DIGOUT_BIT07,	DIGIN_07, HIGH, ERR_DIGITAL_IO_17},

        {DIGOUT_BIT08,	DIGIN_08, HIGH, ERR_DIGITAL_IO_20},
        /* DIGOUT_BIT09 is checked separately because it is used to drive
           the Burn-In box LED
        {DIGOUT_BIT09,	DIGIN_09, HIGH, ERR_DIGITAL_IO_21}, */
        {DIGOUT_BIT10,	DIGIN_10, HIGH, ERR_DIGITAL_IO_22},
        {DIGOUT_BIT11,	DIGIN_11, HIGH, ERR_DIGITAL_IO_23},
        {DIGOUT_BIT12,	DIGIN_12, HIGH, ERR_DIGITAL_IO_24},
        {DIGOUT_BIT13,	DIGIN_13, HIGH, ERR_DIGITAL_IO_25},
        {DIGOUT_BIT14,	DIGIN_14, HIGH, ERR_DIGITAL_IO_26},
        {DIGOUT_BIT15,	DIGIN_15, HIGH, ERR_DIGITAL_IO_27},

        {DIGOUT_BIT16,	DIGIN_16, HIGH, ERR_DIGITAL_IO_30},
        {DIGOUT_BIT17,	DIGIN_17, HIGH, ERR_DIGITAL_IO_31},
        {DIGOUT_BIT18,	DIGIN_18, HIGH, ERR_DIGITAL_IO_32},
        {DIGOUT_BIT19,	DIGIN_19, HIGH, ERR_DIGITAL_IO_33},
        {DIGOUT_BIT20,	DIGIN_20, HIGH, ERR_DIGITAL_IO_34},
        {DIGOUT_BIT21,	DIGIN_21, HIGH, ERR_DIGITAL_IO_35},
        {DIGOUT_BIT22,	DIGIN_22, HIGH, ERR_DIGITAL_IO_36},
        {DIGOUT_BIT23,	DIGIN_23, HIGH, ERR_DIGITAL_IO_37},

        {DIGOUT_BIT24,	DIGIN_24, HIGH, ERR_DIGITAL_IO_40},
        {DIGOUT_BIT25,	DIGIN_25, HIGH, ERR_DIGITAL_IO_41},
        {DIGOUT_BIT26,	DIGIN_26, HIGH, ERR_DIGITAL_IO_42},
        {DIGOUT_BIT27,	DIGIN_27, HIGH, ERR_DIGITAL_IO_43},
        {DIGOUT_BIT28,	DIGIN_28, HIGH, ERR_DIGITAL_IO_44},
        {DIGOUT_BIT29,	DIGIN_29, HIGH, ERR_DIGITAL_IO_45},
        {DIGOUT_BIT30,	DIGIN_30, HIGH, ERR_DIGITAL_IO_46},
        {DIGOUT_BIT31,	DIGIN_31, HIGH, ERR_DIGITAL_IO_47},

    };

    UINT_16 i;
    eDigInBitState din9;
    eDigInBitState drv9;

    for (i = 0; i < sizeof (digTest) / sizeof (DigitalTest_t); i++)
    {

        /* Set the Digital Outputs */
        DO_SetBitWithId (digTest[i].outBit);

        /* Give time for the output to settle */
        GiveSettleTime();

        /* Read the Input bit and verify */
        if (DI_ReadBit (digTest[i].inBit) != digTest[i].expectedFBDriveHigh)
        {

            /* An error code was detected, set the error code and return */
            burnin_error_code = digTest[i].errorId;
            DO_ResetBitWithId (digTest[i].outBit);
#ifndef CONTINUOUS
			return;
#endif
		}

        /* Turn the drive off */
        DO_ResetBitWithId (digTest[i].outBit);

        /* Give time for the output to settle */
        GiveSettleTime();

        /* Read the Input bit after resetting the output bit and verify
           that it is the opposite when setting the bit */
        if (DI_ReadBit (digTest[i].inBit) == digTest[i].expectedFBDriveHigh)
        {
            /* An error code was detected, set the error code and return */
            burnin_error_code = digTest[i].errorId + LOW_SIDE_ERROR;

            /* Set the Outputs to minimize current draw in the burn-in box resistor */
            DO_SetBitWithId (digTest[i].outBit);
#ifndef CONTINUOUS
			return;
#endif
        }

        /* Set the output high (FET is high impedance) to minimize the current
           draw in the burn-in box pull-up resistor */
        DO_SetBitWithId (digTest[i].outBit);

    }

    /* Check DRV9 and DIN9; the Burn-In box LED. Cycle twice to allow the
       LED to change states. */
    for (i = 0; i < 2; i++)
    {

        /* Read the output state and the input state DRV9 and DIN9
           Burn-In Box LED */
        DISABLE_ALL_INTERRUPTS();
        drv9 = DO_ReadBitState (DIGOUT_BIT09) ? HIGH : LOW;
        din9 = DI_ReadBit (DIGIN_09);
        ENABLE_ALL_INTERRUPTS();

        if (drv9 != din9)
        {
            if (drv9)
            {
                burnin_error_code = (UINT_8)(ERR_DIGITAL_IO_21);
            }
            else
            {
                burnin_error_code = (UINT_8)(ERR_DIGITAL_IO_21) + (UINT_8)(LOW_SIDE_ERROR);
            }
#ifndef CONTINUOUS
			return;
#endif
        }

        /* Give time for the LED to toggle */
        TM_Wait (1000);
    }

}


//--------------------------------------------------------------------------
// Module:
//  BurninAnalogTests
//
///   This function is used to test the analog inputs by setting the unused
///   high precision DAC outputs to predetermined full range levels (0V to 5V)
///   to present a +/10V differential input to the analog inputs. The power
///   supply outputs are also checked (status bits that would be set by an
///   ISR) as well as the actual battery voltage level.
///
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
// Module:
//  BurninVDriveTest
//
///   This function turns on/off the Vdrive signals (SSR_OUT1 & SSR_OUT2).
///   In the burnin box, each Vdrive signal (+15 volts) has a voltage
///   divider circuit as follows:
///
///   VDrive ---- 50k --------- 50k -------- GND
///                       |
///                       |
///                    AIN_10(P or N)
///
///   A wire is attached between the two 50k resistors and this wire is
///   wired to AIN_10P & AIN_10N respectively on the HIDAC. Given this circuit
///   and the HIDAC analog input circuit, a known value can be expected at the
///   A/D for AIN_10 when VDRIVEs are present or not or a combination of. Since
///   VDrive can vary, the software allows Vdrive to range from 13 to 17 VDC when
///   a VDrive is enabled and the test can still pass.
///
///	  VDrive1 is tied to P input of AIN_10, VDrive2 is tied to N input of AIN_10
///
//--------------------------------------------------------------------------
static void BurninVDriveTest (void)
{
    UINT_16 index;
    UINT_16 aValue;
    CHAR str[50];

    static const VDriveTest_t vdrive[] =
    {
        /* Vdrive2 (state)	Vdrive1 (state)		max hi(mV)	max lo(mV)	error code */
        {DO_ResetBitWithId, DO_ResetBitWithId,	3785,		3385,		VDRIVE_2X1X},
        {DO_ResetBitWithId, DO_SetBitWithId,    4150,       3750,       VDRIVE_2X1O}, 
        {DO_SetBitWithId,	DO_ResetBitWithId,  1715,       1315,       VDRIVE_2O1X}, 
        {DO_SetBitWithId,	DO_SetBitWithId,    2085,       1685,       VDRIVE_2O1O},
    };

    index = 0;
    while (index < sizeof (vdrive) / sizeof (VDriveTest_t))
    {

        /* Turn on or off Vdrive2 */
        vdrive[index].fnPtrVdrive2 (SSR_IN2);

        /* Turn on or off Vdrive1 */
        vdrive[index].fnPtrVdrive1 (SSR_IN1);
        GiveSettleTime();

        sprintf (str, "Test %u\n\r", index);
		strcat(failureStr, str);
        SC_PutsAlways (str);
        aValue = AI_GetValue (ANALOGIN_10);
        sprintf (str, "A10 value =  %u\n\r", aValue);
		strcat(failureStr, str);
        SC_PutsAlways (str);

        /* Verify the A/D value is not outside the allowed range */
        if ((aValue > vdrive[index].hiAllowedValue) ||
                (aValue < vdrive[index].loAllowedValue))
        {

            /* Failure occurred; report and exit immediately */
            burnin_error_code = vdrive[index].errorCode;

			DO_ResetBitWithId (SSR_IN1);
			DO_ResetBitWithId (SSR_IN2);
#ifndef CONTINUOUS
			return;
#endif
        }

        index++;
    }
}

//--------------------------------------------------------------------------
// Module:
//  ZeroAnalogOutputs
//
///   This function ensures all of the analog outputs are set to 0.
///
//--------------------------------------------------------------------------
static void ZeroAnalogOutputs (void)
{
	UINT_16 i; 

	for (i = 0; i < 8; i++)
	{
		burninDAC[i] = 0;
	}

}

//--------------------------------------------------------------------------
// Module:
//  BurninAnalogTests
//
///   This function tests all of the analog inputs by setting the analog 
///   output voltages, which are wrapped back into the analog inputs, to
///   known values and then verifying the analog input ADC readings.
///
//--------------------------------------------------------------------------
static void BurninAnalogTests (void)
{
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
	// For some reason, the analog tests fail at high temperature (above 60 C)
	// when the static keyword is removed. The static keyword ensures that the 
	// structure gets copied to RAM at startup, but w/o it, the structure is 
	// maintained in FLASH... yet another HiDAC mystery!!!!
	//!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
    static const BurninAlog_t alogBurnTest[] =
    {
        {
            &burninDAC[0], 2400, &burninDAC[1], 1000,
            632 - 30, 632 + 30, ANALOGIN_04, 0xC0
        },
        {
            &burninDAC[0], 1000, &burninDAC[1], 2400,
            381 - 30, 381 + 30, ANALOGIN_04, 0xC1
        },
        {
            &burninDAC[0], 2400, &burninDAC[1], 1000,
            632 - 30, 632 + 30, ANALOGIN_05, 0xC2
        },
        {
            &burninDAC[0], 1000, &burninDAC[1], 2400,
            381 - 30, 381 + 30, ANALOGIN_05, 0xC3
        },
        {
            &burninDAC[0], 2400, &burninDAC[1], 1000,
            531 - 30, 531 + 30, ANALOGIN_06, 0xC4
        },
        {
            &burninDAC[0], 1000, &burninDAC[1], 2400,
            121 - 30, 121 + 30, ANALOGIN_06, 0xC5
        },
        {
            &burninDAC[2], 2400, &burninDAC[3], 1000,
            531 - 30, 531 + 30, ANALOGIN_07, 0xC6
        },
        {
            &burninDAC[2], 1000, &burninDAC[3], 2400,
            121 - 30, 121 + 30, ANALOGIN_07, 0xC7
        },
        {
            &burninDAC[2], 1000, &burninDAC[3], 2400,
            491 - 30, 491 + 30, ANALOGIN_08, 0xC8
        },
        {
            &burninDAC[2], 2400, &burninDAC[3], 1000,
            729 - 30, 729 + 30, ANALOGIN_08, 0xC9
        },
        {
            &burninDAC[2], 1000, &burninDAC[3], 2400,
            491 - 30, 491 + 30, ANALOGIN_09, 0xCA
        },
        {
            &burninDAC[2], 2400, &burninDAC[3], 1000,
            729 - 30, 729 + 30, ANALOGIN_09, 0xCB
        },
        {
            &burninDAC[4], 5000, &burninDAC[5], 1000,
            795 - 30, 795 + 30, ANALOGIN_11, 0xCC
        },
        {
            &burninDAC[4], 1000, &burninDAC[5], 5000,
            108 - 30, 108 + 30, ANALOGIN_11, 0xCD
        },
        {
            &burninDAC[4], 5000, &burninDAC[5], 1000,
            795 - 30, 795 + 30, ANALOGIN_12, 0xCE
        },
        {
            &burninDAC[4], 1000, &burninDAC[5], 5000,
            108 - 30, 108 + 30, ANALOGIN_12, 0xCF
        },
        {
            &burninDAC[4], 5000, &burninDAC[5], 1000,
            795 - 30, 795 + 30, ANALOGIN_13, 0xD0
        },
        {
            &burninDAC[4], 1000, &burninDAC[5], 5000,
            108 - 30, 108 + 30, ANALOGIN_13, 0xD1
        },
        {
            &burninDAC[6], 2400, &burninDAC[7], 1000,
            632 - 30, 632 + 30, ANALOGIN_00, 0xD2
        },
        {
            &burninDAC[6], 1000, &burninDAC[7], 2400,
            381 - 30, 381 + 30, ANALOGIN_00, 0xD3
        },
        {
            &burninDAC[6], 2400, &burninDAC[7], 1000,
            632 - 30, 632 + 30, ANALOGIN_01, 0xD4
        },
        {
            &burninDAC[6], 1000, &burninDAC[7], 2400,
            381 - 30, 381 + 30, ANALOGIN_01, 0xD5
        },
        {
            &burninDAC[6], 2400, &burninDAC[7], 1000,
            632 - 30, 632 + 30, ANALOGIN_02, 0xD6
        },
        {
            &burninDAC[6], 1000, &burninDAC[7], 2400,
            381 - 30, 381 + 30, ANALOGIN_02, 0xD7
        },
        {
            &burninDAC[6], 2400, &burninDAC[7], 1000,
            632 - 30, 632 + 30, ANALOGIN_03, 0xD8
        },
        {
            &burninDAC[6], 1000, &burninDAC[7], 2400,
            381 - 30, 381 + 30, ANALOGIN_03, 0xD9
        },
    };

    INT_32 analog_value;
    UINT_16 index = 0;
    const BurninAlog_t *ptr;


    while (index < sizeof (alogBurnTest) / sizeof (BurninAlog_t))
    {

        CHAR str[50];
		UINT_16 max, min;
		INT_16 dacP, dacN;

        ptr = &alogBurnTest[index];

		dacP = ptr->pValue;
		dacN = ptr->nValue;

        * (ptr->pAddr) = dacP;
        * (ptr->nAddr) = dacN;

		max = ptr->validHi;
		min = ptr->validLo;

        GiveSettleTime();

        analog_value = AI_GetDataValueADC (ptr->a2dId);

        sprintf (str, "AlogTest %u", index);
		strcat(failureStr, str);
        SC_PutsAlways (str);
        sprintf (str, " inVal =  %u -- ", analog_value);
		strcat(failureStr, str);
		SC_PutsAlways (str);
        sprintf (str, "[Min,Max] = [%u , %u] -- ", min,max);
        strcat(failureStr, str);
		SC_PutsAlways (str);
		sprintf (str, "DAC = [%d , %d] \n\r", dacP,dacN);
		strcat(failureStr, str);
        SC_PutsAlways (str);

        if (analog_value < min || analog_value > max)
        {
            burnin_error_code = ptr->errCode;
			SC_PutsAlways ("********Above Analog Test Failed*********\n\r");
#ifdef HALT_TEST
			SC_PutsAlways ("Hit any key to continue\n\r");
			SC_FlushStream(RX_STREAM);
			while (SC_GetChar() == EOF)
			{}
#endif
#ifndef CONTINUOUS
			return;
#endif
        }

        index++;
    }

	ZeroAnalogOutputs();

    /*****************************/
    /* Check +/- 15 volt supplies and the VDRIVE */
    if (Get15VoltBad() & PLUS_15_BAD_MASK)
    {
        burnin_error_code = PS_PLUS_15_ERR;
        return;
    }

    if (Get15VoltBad() & MINUS_15_BAD_MASK)
    {
        burnin_error_code = PS_MINUS_15_ERR;
        return;
    }

	// Check VDrive status
    if (!(DI_GetCurrent(DIGIN_BANK0) & 0x0001))
    {
        burnin_error_code = VDRIVE_PLUS_15_ERR;
        return;
    }
    /*****************************/


    /*****************************/
    /* Make sure Battery voltage is between 34 & 38 volts
       Since the B+ range at the A/D input +/-50 volts
       The desired quantized values can be calculated as follows

        Q = ((Volts - (-50)) / (50 - -50)) * 1024

    	So, for 34 volts, the A/D quanta is 860
            for 38 volts, the A/D quanta is 901

        */

    analog_value = AI_GetDataValueADC (BATTERY_VOLTAGE);
    if ((analog_value < 860) || (analog_value > 917))
    {
        burnin_error_code = BATTERY_VOLTAGE_ERR;
        return;
    }

}



//--------------------------------------------------------------------------
// Module:
//  BurninDigitalTests
//
///   This function turns on/off the output drives on the HIDAC so that B+ (through
///   a resistor) or ground is presented to the digital inputs. Whenever an output is made
///   high, the output transistor is shut off. The signal in the test box
///   is pulled up and therefore B+ is presented to the digital input. If the output
///   is made low, the output transistor is turned on and the digital input is presented
///   ground. This function sets the bit, verifies the input is high, and then resets
///   the bit and verifies the input is low. Whenever a "low" failure is detected,
///   LOW_SIDE_ERROR is added to the error code so that the user can discern the
///   difference between a HIGH and LOW failure.
///
//--------------------------------------------------------------------------
static void BurninDigitalTests2 (void)
{
    const DigitalTest_t digTest[] =
    {

        {DIGOUT_BIT08,	DIGIN_32, HIGH, ERR_DIGITAL_IO_50},

        /* DIGOUT_BIT09 is checked separately because it is used to drive
           the Burn-In box LED
        {DIGOUT_BIT09,	DIGIN_09, HIGH, ERR_DIGITAL_IO_21}, */
        {DIGOUT_BIT24,	DIGIN_33, HIGH, ERR_DIGITAL_IO_51},
        {DIGOUT_BIT10,	DIGIN_34, HIGH, ERR_DIGITAL_IO_52},
        {DIGOUT_BIT11,	DIGIN_35, HIGH, ERR_DIGITAL_IO_53},
        {DIGOUT_BIT12,	DIGIN_36, HIGH, ERR_DIGITAL_IO_54},
        {DIGOUT_BIT13,	DIGIN_37, HIGH, ERR_DIGITAL_IO_55},
        {DIGOUT_BIT14,	DIGIN_38, HIGH, ERR_DIGITAL_IO_56},
        {DIGOUT_BIT15,	DIGIN_39, HIGH, ERR_DIGITAL_IO_57},

        {DIGOUT_BIT16,	DIGIN_40, HIGH, ERR_DIGITAL_IO_58},
        {DIGOUT_BIT17,	DIGIN_41, HIGH, ERR_DIGITAL_IO_59},
        {DIGOUT_BIT18,	DIGIN_42, HIGH, ERR_DIGITAL_IO_5A},
        {DIGOUT_BIT19,	DIGIN_43, HIGH, ERR_DIGITAL_IO_5B},
        {DIGOUT_BIT20,	DIGIN_44, HIGH, ERR_DIGITAL_IO_5C},
        {DIGOUT_BIT21,	DIGIN_45, HIGH, ERR_DIGITAL_IO_5D},
        {DIGOUT_BIT22,	DIGIN_46, HIGH, ERR_DIGITAL_IO_5E},
        {DIGOUT_BIT23,	DIGIN_47, HIGH, ERR_DIGITAL_IO_5F},
    };

    UINT_16 i;

    for (i = 0; i < sizeof (digTest) / sizeof (DigitalTest_t); i++)
    {

        /* Set the Digital Outputs */
        DO_SetBitWithId (digTest[i].outBit);

        /* Give time for the output to settle */
        GiveSettleTime();

        /* Read the Input bit and verify */
        if (DI_ReadBit (digTest[i].inBit) != digTest[i].expectedFBDriveHigh)
        {

            /* An error code was detected, set the error code and return */
            burnin_error_code = digTest[i].errorId;
            DO_ResetBitWithId (digTest[i].outBit);
#ifndef CONTINUOUS
			return;
#endif
        }

        /* Turn the drive off */
        DO_ResetBitWithId (digTest[i].outBit);

        /* Give time for the output to settle */
        GiveSettleTime();

        /* Read the Input bit after resetting the output bit and verify
           that it is the opposite when setting the bit */
        if (DI_ReadBit (digTest[i].inBit) == digTest[i].expectedFBDriveHigh)
        {
            /* An error code was detected, set the error code and return */
            burnin_error_code = digTest[i].errorId + 10;

            /* Set the Outputs to minimize current draw in the burn-in box resistor */
            DO_SetBitWithId (digTest[i].outBit);
#ifndef CONTINUOUS
			return;
#endif
        }

        /* Set the output high (FET is high impedance) to minimize the current
           draw in the burn-in box pull-up resistor */
        DO_SetBitWithId (digTest[i].outBit);

    }

 
}


//--------------------------------------------------------------------------
// Module:
//  BurninMvbTest
//
///   This function sends a variable in a MVB source telegram (Port Address 651)
///   and expects to receive the variable incremented by 1 in a MVB sink
///   telegram (Port Address 658).
///
///   The incremented variable has to be received within 100ms after the
///   send variable has been incremented. Otherwise there will be an error
///   and error code F4 (MVB_ERR) will be set on the HEX-display
///
//--------------------------------------------------------------------------
static void BurninMvbTest (void)
{
    static UINT_16 counterVariable = 0;
    BOOLEAN counterIncremented;
    Timer *tm1;
    Timer *tm2;
    INT_32 expiredTime = 0;
    char str[60];

    /* Increment send variable value, do not use value 0xffff */
    if (counterVariable == 0xfffe)
    {
        counterVariable = 1;
    }
    else
    {
        counterVariable++;
    }

    /* Copy value to MVB variable */
    HDVCU_651_240_UINT16 = counterVariable;

    /* Write send variable to MVB traffic store */
    mvb_fast_write_T16();

    /* 16ms cycle time for telegrams on MVB */
    /* 8ms task cycle time in VCU-C */
    /* Worst case calculation to receive updated telegram: 16 + 8 + 16 = 40 ms */
    /* Wait for character for no more than 100ms */
    counterIncremented = FALSE;
    tm1 = TM_Allocate (0, NULL);
    tm2 = TM_Allocate (0, NULL);
    if ((tm1 == NULL) || (tm2 == NULL))
    {
        sprintf (str, "TIMERS: tm1= 0x%X   tm2= 0x%X\n \r", expiredTime);
        SC_PutsAlways (str);
    }

    /* Incremented variable value must be received within 100ms */
    while (TM_GetExpiredTime (tm1) < 100)
    {
        mvb_fast_read_T16();
        if (VCUHD_658_240_UINT16 == (HDVCU_651_240_UINT16 + 1))
        {
            counterIncremented = TRUE;
            break;
        }
    }
    TM_Free (tm1);

    if (counterIncremented == FALSE)
    {
        burnin_error_code = MVB_ERR;
        expiredTime = TM_GetExpiredTime (tm2);
        sprintf (str, "Expired time= %d\n \r", expiredTime);
		strcat(failureStr, str);
        SC_PutsAlways (str);
    }
    TM_Free (tm2);

}
