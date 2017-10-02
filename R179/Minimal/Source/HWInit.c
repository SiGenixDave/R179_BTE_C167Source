/******************************************************************************
*
* Copyright (C) 2014-15 Bombardier
*
* File Name: HWInit.c
*
* Revision History:
*   08/31/2014 - das -	Created
*
*
******************************************************************************/
///   \file
///   This file contains the code that initializes the HiDAC CPU

#define hwinit_C

/*--------------------------------------------------------------------------
                              INCLUDE FILES
  --------------------------------------------------------------------------*/
#ifndef _DEBUG
#include <reg167.h>
#endif

#include "Types.h"
#include "SerComm.h"
#include "Timer.h"
#include "AlogOut.h"
#include "DigOut.h"
#include "PwmOut.h"
#include "AnalogIn.h"
#include "Digital.h"
#include "HWInit.h"
#include "Pwm.h"

/*--------------------------------------------------------------------------
                             MODULE CONSTANTS
  --------------------------------------------------------------------------*/
#define  TIMER_RATE_1_6				3		/*  1.6 usec */
#define  TIMER_RATE_25_6			7		/* 25.6 usec */
#define  TIMER_MODE					0		/* Set mode to timer */
#define  TIMER_RUN					1		/* Timer run */
#define  T1SETCON					0x04c	/* T1 set interrupt control register */

/*--------------------------------------------------------------------------
                              MODULE MACROS
  --------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
                             MODULE DATA TYPES
  --------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
                              MODULE VARIABLES
  --------------------------------------------------------------------------*/
static BOOLEAN m_DisableVdrive = FALSE;
static BOOLEAN m_DisableCPUWatchdog = FALSE;
static UINT_8  m_FifteenVoltBad = 0;
static eDisableCPUWDog m_DisableCPUReason;
static BOOLEAN m_StartUpSuccessful;


/* In order to use the timer features of the HIDAC driver software, this
   variable must be named exactly "CLOCK_TICK". The value of this variable is the
   periodic rate (in msecs) that the TM_Service() is called. */
const UINT_16 CLOCK_TICK = 10;


/*--------------------------------------------------------------------------
                             MODULE PROTOTYPES
  --------------------------------------------------------------------------*/

//--------------------------------------------------------------------------
// Module:
//  InitTimer0
//
///   This function initializes Timer 0 to trigger an interrupt
///   every 1 second. Note: This function just initializes the timer,
///   but the timer run bit isn't set. Timer 0 is used exclusively to perform
///   a load measurement.
///
//--------------------------------------------------------------------------
void InitTimer0 (void)
{
    TM_Init();

    /* Calculate reload value */
    /* overflow value minus the input frequency */
    /* divided by 1k times number ms/clock tick */

    /* 25.6 usecs per timer tick */
    /* So 1.000 / 0.0000256 = 39062.5 */
    //DAS T0REL = 0xffff - 39062;	/* 1.0 sec task for timer*/
    T0REL = 0xffff - 1953;	/* 50 msecs */

    /* set interrupt control register */
    T0IC = 0x6C;

    /* dont set timer 0 run bit (StartTimer0() below does that)  */
    T01CON |= (TIMER_MODE << 3) |
              (TIMER_RATE_25_6 << 0);

}

//--------------------------------------------------------------------------
// Module:
//  InitTimer0
//
///   This function starts Timer 0. The timer 0 interrupt will occur whenever
///   Timer 0 expires based on the T0 initial value
///
//--------------------------------------------------------------------------
void StartTimer0 (void)
{
	T0 = T0REL;
	T01CON |= (TIMER_RUN << 6);
}

//--------------------------------------------------------------------------
// Module:
//  StopTimer0
//
///   This function stops Timer 0 from running
///
//--------------------------------------------------------------------------
void StopTimer0 (void)
{
	// UINT_16 typecast required to avoid compiler warning
	T01CON &= (UINT_16)(~(TIMER_RUN << 6));
}


//--------------------------------------------------------------------------
// Module:
//  InitTimer1
//
///   This function initializes Timer 1 to generate an interrupt
///   every and CLOCK_TICK milliseconds. 
///
//--------------------------------------------------------------------------
void InitTimer1 (void)
{
    TM_Init();

    /* Set starting timer values*/
    T1 = 0xfff0;

    /* Calculate reload value */
    /* overflow value minus the input frequency */
    /* divided by 1k times number ms/clock tick */

    /* 1.6 usecs per timer tick */
    /* So 0.010 / 0.0000016 = 6250 */
    T1REL = 0xffff - 6250;	/* 10 ms task for timer*/

    /* set interrupt control register */
    T1IC = 0x64;

    /* set timer 1 run bit to enable */
    T01CON |= (TIMER_MODE << 11) |
             (TIMER_RATE_1_6 << 8) |
			 (TIMER_RUN << 14) ;


}


//--------------------------------------------------------------------------
// Module:
//  InitTimer7
//
///   This function initializes timer 7 to generate an interrupt
///   every 8 milliseconds. 
///
//--------------------------------------------------------------------------
void InitTimer7 ()
{

    T7 = 0xfff0;

    /* Calculate reload value */
    /* overflow value minus the input frequency */
    /* divided by 1k times number ms/clock tick */

    /* 1.6 usecs per timer tick */
    /* So 0.008 / 0.0000016 = 5000 */
    T7REL = 0xffff - 5000;	/* 8 ms task for MVB */


    /* set interrupt control register */
    T7IC = 0x68;

    /* set timer 7 run bit to enable */
    T78CON |= (TIMER_MODE << 3) |
              (TIMER_RATE_1_6) | 
			  (TIMER_RUN << 6);

}

//--------------------------------------------------------------------------
// Module:
//  DisableSystemInterrupts
//
///   This function disables all interrupt processing. This function 
///   is only called when establishing a CPU load baseline.
///
//--------------------------------------------------------------------------
void DisableSystemInterrupts (void)
{
    T1IC = 0x00;
	T7IC = 0x00;
	PwmInDisableISR();
}

//--------------------------------------------------------------------------
// Module:
//  EnableSystemInterrupts
//
///   This function enables all interrupt processing. This function 
///   is only called when establishing a CPU load baseline.
///
//--------------------------------------------------------------------------
void EnableSystemInterrupts (void)
{
	T1 = 0xFFFE;
    T1IC = 0x64;
	T7 = 0xFFFE;
	T7IC = 0x68;
	PwmInEnableISR();
}



//--------------------------------------------------------------------------
// Module:
//  Init15VoltFailDetect
//
///   This function initializes CAPCOM registers to detect
///
//--------------------------------------------------------------------------
void Init15VoltFailDetect (void)
{
    /* Capture/Compare Mode Registers for the CAPCOM1 Unit (CC0...CC15) (page 14-9) */
    /* CCM3
    0............... = ACC15 allocated to Timer T0
    .010............ = CCMOD15 capture on falling edge at pin CC15IO (+15VLow)
    ....0........... = ACC14 allocated to Timer T0
    .....001........ = CCMOD14 capture on rising edge at pin CC14IO (-15VLow)
    ........0....... = ACC13 allocated to Timer T0
    .........000.... = CCMOD13 capture on falling edge at pin CC13IO (Not used)
    ............0... = ACC12 allocated to Timer T0
    .............000 = CCMOD12 capture on falling edge at pin CC12IO (IPack IRQ0)
    0010000100000000 = 0x2100
    */
    CCM3 = 0x2100;

    /* Enable +15 volt low interrupt P2.15 => 0x1f => CAPCOM reg 15 */
    /* CC15IC
    00000000........ = Reserved
    ........0....... = IR:   Interrupt Request flag
    .........1...... = IE:   Interrupt Enable control bit
    ..........1001.. = ILVL: Interrupt Priority Level 9
    ..............01 = GLVL: Group Level 1
    0000000001100101 = 0x0065
    */
    CC15IC = 0x0065;

    /* Enable -15 volt low interrupt P2.14 => 0x1e => CAPCOM reg 14 */
    /* CC14IC
    00000000........ = Reserved
    ........0....... = IR:   Interrupt Request flag
    .........1...... = IE:   Interrupt Enable control bit
    ..........1001.. = ILVL: Interrupt Priority Level 9
    ..............00 = GLVL: Group Level 0
    0000000001100100 = 0x0065
    */
    CC14IC = 0x0064;
}


//--------------------------------------------------------------------------
// Module:
//  InitP8ForDigOuts
//
///   This function initializes the C167 P8 port to be all digital outputs.
///
//--------------------------------------------------------------------------
void InitP8ForDigOuts (void)
{
    P8 = 0x0000;
    DP8 = 0x00ff;
    P8 ^= 1;
}


//--------------------------------------------------------------------------
// Module:
//  InitSerialCommunications
//
///   This function initializes the serial communications port for a baud
///   rate of 19200. The receive and transmit interrupt priorities are also
///   set.
///
//--------------------------------------------------------------------------
void InitSerialCommunications (void)
{
    SC_Init (BAUD_19200, 4, 8);
}

//--------------------------------------------------------------------------
// Module:
//  InitAnalogInputs
//
///   This function initializes all analog inputs and specifies their
///   scaling factors.
///
//--------------------------------------------------------------------------
void InitAnalogInputs (void)
{

    AI_Init();

    /* Enable all the remaining analog inputs */
    AI_SetEnable (ANALOGIN_00, TRUE);
    AI_SetEnable (ANALOGIN_01, TRUE);
    AI_SetEnable (ANALOGIN_02, TRUE);
    AI_SetEnable (ANALOGIN_02, TRUE);
    AI_SetEnable (ANALOGIN_03, TRUE);
    AI_SetEnable (ANALOGIN_04, TRUE);
    AI_SetEnable (ANALOGIN_05, TRUE);
    AI_SetEnable (ANALOGIN_06, TRUE);
    AI_SetEnable (ANALOGIN_07, TRUE);
    AI_SetEnable (ANALOGIN_08, TRUE);
    AI_SetEnable (ANALOGIN_09, TRUE);
    AI_SetEnable (ANALOGIN_10, TRUE);
    AI_SetEnable (ANALOGIN_11, TRUE);
    AI_SetEnable (ANALOGIN_12, TRUE);
    AI_SetEnable (ANALOGIN_13, TRUE);
    AI_SetEnable (ANALOGIN_14, TRUE);
    AI_SetEnable (ANALOGIN_15, TRUE);
}

//--------------------------------------------------------------------------
// Module:
//  InitAnalogInputs
//
///   This function initializes all analog outputs and specifies their
///   scaling factors.
///
//--------------------------------------------------------------------------
void InitAnalogOutputs (void)
{

    /* Init the analog outputs */
    AO_Init();

    /* analog output 0 ------------------------------------- */
    AO_SetState (ALOG_OUT0, ALOGOUT_QUANTA);
    AO_SetScaling (ALOG_OUT0, 4095, 0);
    AO_SetEnable (ALOG_OUT0, TRUE);
	AO_SetQuanta (ALOG_OUT0, 0);

    /* analog output 1 ------------------------------------- */
    AO_SetState (ALOG_OUT1, ALOGOUT_QUANTA);
    AO_SetScaling (ALOG_OUT1, 4095, 0);
    AO_SetEnable (ALOG_OUT1, TRUE);
	AO_SetQuanta (ALOG_OUT1, 0);

    /* analog output 2 ------------------------------------- */
    AO_SetState (ALOG_OUT2, ALOGOUT_QUANTA);
    AO_SetScaling (ALOG_OUT2, 4095, 0);
    AO_SetEnable (ALOG_OUT2, TRUE);
	AO_SetQuanta (ALOG_OUT2, 0);

    /* analog output 3 ------------------------------------- */
    AO_SetState (ALOG_OUT3, ALOGOUT_QUANTA);
    AO_SetScaling (ALOG_OUT3, 4095, 0);
    AO_SetEnable (ALOG_OUT3, TRUE);
	AO_SetQuanta (ALOG_OUT3, 0);

    /* analog output 4 ------------------------------------- */
    AO_SetState (ALOG_OUT4, ALOGOUT_QUANTA);
    AO_SetScaling (ALOG_OUT4, 4095, 0);
    AO_SetEnable (ALOG_OUT4, TRUE);
	AO_SetQuanta (ALOG_OUT4, 0);

    /* analog output 5 ------------------------------------- */
    AO_SetState (ALOG_OUT5, ALOGOUT_QUANTA);
    AO_SetScaling (ALOG_OUT5, 4095, 0);
    AO_SetEnable (ALOG_OUT5, TRUE);
	AO_SetQuanta (ALOG_OUT5, 0);

    /* analog output 6 ------------------------------------- */
    AO_SetState (ALOG_OUT6, ALOGOUT_QUANTA);
    AO_SetScaling (ALOG_OUT6, 4095, 0);
    AO_SetEnable (ALOG_OUT6, TRUE);
	AO_SetQuanta (ALOG_OUT6, 0);

    /* analog output 7 ------------------------------------- */
    AO_SetState (ALOG_OUT7, ALOGOUT_QUANTA);
    AO_SetScaling (ALOG_OUT7, 4095, 0);
    AO_SetEnable (ALOG_OUT7, TRUE);
	AO_SetQuanta (ALOG_OUT7, 0);

    /* Write to Analog output hardware (All 8 DACs updated here) */
    AO_Service();
}



//--------------------------------------------------------------------------
// Module:
//  InitDigitalOutputs
//
///   This function initializes all digital outputs to logic level 1.
///
//--------------------------------------------------------------------------
void InitDigitalOutputs (void)
{
    DO_SetAllBits (DIGOUT_BANK3);
}


//--------------------------------------------------------------------------
// Module:
//  InitPwmOutputs
//
///   This function initializes all PWM outputs
///
//--------------------------------------------------------------------------
void InitPwmOutputs (void)
{
    /* Select CLKcpu (40MHz) and start PWM timers */
    /* PWMCON0
    0............... = PIR3: No interrupt request from channel 3
    .0.............. = PIR2: No interrupt request from channel 2
    ..0............. = PIR1: No interrupt request from channel 1
    ...0............ = PIR0: No interrupt request from channel 0
    ....0........... = PIE3: Interrupt from channel 3 disabled
    .....0.......... = PIE2: Interrupt from channel 2 disabled
    ......0......... = PIE1: Interrupt from channel 1 disabled
    .......0........ = PIE0: Interrupt from channel 0 disabled
    ........0....... = PTI3: Timer PT3 clocked with CLKcpu
    .........0...... = PTI2: Timer PT2 clocked with CLKcpu
    ..........0..... = PTI1: Timer PT1 clocked with CLKcpu
    ...........0.... = PTI0: Timer PT0 clocked with CLKcpu
    ............1... = PTR3: Timer PT3 is running
    .............1.. = PTR2: Timer PT2 is running
    ..............1. = PTR1: Timer PT1 is running
    ...............1 = PTR0: Timer PT0 is running
    0000000000001111 = 0x000F */
    
	PWMCON0 = 0x000F;

    PWMO_SetEnable (PWMOUT_0, FALSE);
    // Important note: calling this function prior with params PWMOUT_1 & FALSE
	// somehow prevent the IPACK from initializing correctly. Investigation should
	// occur as to why. As of now, the 
	PWMO_SetEnable (PWMOUT_1, FALSE);
    PWMO_SetEnable (PWMOUT_2, FALSE);
    PWMO_SetEnable (PWMOUT_3, FALSE);
}


//--------------------------------------------------------------------------
// Module:
//  ToggleCPUWatchdog
//
///   This function toggles the CPU watchdog. A call to this function
///   prevents the CPU from resetting. Sixteen calls to this function
///   changes the state of the Run LED. Also, the watchdog timer will
///   not timeout if a jumper is not placed on JMP1.
///
//--------------------------------------------------------------------------
// make sure there is no space pr=ROM_FUNCTIONS
#pragma class pr=ROM_FUNCTIONS
void ToggleCPUWatchdog (void)
{
    (* (volatile UINT_16 *) (0x500020L)) = 1;
}
#pragma default_attributes


//--------------------------------------------------------------------------
// Module:
//  ToggleVDriveWatchdog
//
///   This function toggles the VDrive watchdog. VDrive will become disabled
///   if this function is not called within TBD milliseconds.
///
//--------------------------------------------------------------------------
void ToggleVDriveWatchdog (void)
{
    P8 ^= 1;
}


//--------------------------------------------------------------------------
// Module:
//  Get15VoltBad
//
///   This function is an accessor for the variable that indicates if there
///   is a +/- 15 volt supply problem.
///
///	  \returns UINT_8 - non-zero if either +/- 15 volt supply is inadequate
///
//--------------------------------------------------------------------------
UINT_8 Get15VoltBad (void)
{
    return m_FifteenVoltBad;
}


//--------------------------------------------------------------------------
// Module:
//  Set15VoltBad
//
///   This function is a mutator for the variable that indicates if there
///   is a +/- 15 volt supply problem.
///
///	  \param aReason - non-zero if either +/- 15 volt supply is inadequate
///
//--------------------------------------------------------------------------
void SetFifteenVoltBad (UINT_8 aReason)
{
    m_FifteenVoltBad |= aReason;
}

//--------------------------------------------------------------------------
// Module:
//  SetVdriveDisable
//
///   This function is a mutator that can enable or disable VDrive.
///
///   \param aDisable - set TRUE to disable the VDrive, FALSE to enable it
///
//--------------------------------------------------------------------------
void SetVdriveDisable (BOOLEAN aDisable)
{
    m_DisableVdrive = aDisable;
}

//--------------------------------------------------------------------------
// Module:
//  ServiceVDriveWatchdog
//
///   This function determines whether or not to service the VDrive
/// watchdog based on the state of "m_DisableVdrive".
///
///
//--------------------------------------------------------------------------
void ServiceVDriveWatchdog (void)
{
    /* Toggle the VDrive watchdog if it hasn't been disabled */
    if (!m_DisableVdrive)
    {
        ToggleVDriveWatchdog();
    }
}

//--------------------------------------------------------------------------
// Module:
//  ServiceCPUWatchdog
//
///   This function determines whether or not to service the CPU
/// watchdog based on the state of "m_DisableCPUWatchdog".
///
///
//--------------------------------------------------------------------------
void ServiceCPUWatchdog (void)
{
    /* Toggle the CPU watchdog if it hasn't been disabled */
    if (!m_DisableCPUWatchdog)
    {
        ToggleCPUWatchdog();
    }

}

//--------------------------------------------------------------------------
// Module:
//  SetCPUDisable
//
///   This function is a mutator that disables the CPU watchdog.
///
///   \param aReason - the reason why the CPU watchdog is disabled
///
//--------------------------------------------------------------------------
void SetCPUDisable (eDisableCPUWDog aReason)
{
	switch (aReason)
    {
		case TASK_MVB_DEAD:
			//DAS SC_DebugPrint("CPUDIS: 15 Volts\n\r");
            break;

    }

    m_DisableCPUWatchdog = TRUE;
    m_DisableCPUReason = aReason;

}

//--------------------------------------------------------------------------
// Module:
//  SetStartupSuccessful
//
///   This function is a mutator that indicates the startup code executed
///   without error.
///
///   \param aValue - TRUE if startup is complete; FALSE otherwise
///
//--------------------------------------------------------------------------
void SetStartupSuccessful (BOOLEAN aValue)
{
    m_StartUpSuccessful = aValue;
}

//--------------------------------------------------------------------------
// Module:
//  GetStartupSuccessful
//
///   This function is an accessor that indicates the startup code executed
///   without error.
///
///   \returns BOOLEAN - TRUE if startup is complete; FALSE otherwise
///
//--------------------------------------------------------------------------
BOOLEAN GetStartupSuccessful (void)
{
    return m_StartUpSuccessful;
}