/******************************************************************************
*
* Copyright (C) 2014-15 Bombardier
*
* File Name: Interupt.c
*
* Revision History:
*   08/31/2014 - das - Created
******************************************************************************/
///   \file
///   This file contains the code that handles all of the real time interrupts.

#define interrupt_C

/*--------------------------------------------------------------------------
                              INCLUDE FILES
  --------------------------------------------------------------------------*/
#ifndef _DEBUG
#include <c166.h>
#include <reg167.h>
#endif

#include "types.h"
#include "Digital.h"
#include "HWInit.h"
#include "Timer.h"
#include "VEC_TBL.h"
#include "TaskMVB.h"
#include "Pwm.h"
#include "BurnIn.h"
#include "Debug.h"


/*--------------------------------------------------------------------------
                             MODULE CONSTANTS
  --------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
                              MODULE MACROS
  --------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
                             MODULE DATA TYPES
  --------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
                              MODULE VARIABLES
  --------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
                             MODULE PROTOTYPES
  --------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
*  Module:
*   CpuLoadMeasureISR
*
*     This function is TODO
*
----------------------------------------------------------------------------*/
interrupt (CAPCOM_TIMER_0) using (CPU_LOAD_MEASURE_RB) void CpuLoadMeasureISR (void)
{
	StopTimer0();
	DebugCalculateCpuLoad();
	EnableSystemInterrupts();
}

/*----------------------------------------------------------------------------
*  Module:
*   SchedulerISR
*
*     This function is automatically invoked by the C167 when Timer
*     1 has expired (interrupt service routine).
*
----------------------------------------------------------------------------*/
interrupt (CAPCOM_TIMER_1) using (SCHEDULE_RB) void SchedulerISR (void)
{
	BOOLEAN taskMVBAlive = FALSE;

    /* Service Vdrive Watchdog */
    ServiceVDriveWatchdog();

	/* Monitor the MVB 8 ms interrupt. Allow 5 consecutive dead interrupts
	   before declaring interrupt dead and forcing a CPU reset */
	if (GetStartupSuccessful() == TRUE)
	{
		taskMVBAlive = MonitorMVBInterrupt (5);
		if (!taskMVBAlive)
		{
			SetCPUDisable (TASK_MVB_DEAD);
		}
	}

	/* Service CPU watchdog. Note: Logic exists in this function to disable
		servicing watchdog if above check fails */
	ServiceCPUWatchdog();

    /* -------------------------------------------------------------------------
    Service the timers. CLOCK_TICK should be updated in "timer.h"
    to reflect the periodicity of this interrupt.
    ------------------------------------------------------------------------- */
    TM_Service();

	 _nop();

}

/*----------------------------------------------------------------------------
  Module:
    ISR to service Vehicle related task including the PTU

     This function is automatically invoked by the C167 when Timer
     7 has expired (interrupt service routine).

--------------------------------------------------------------------------  */
interrupt (CAPCOM_TIMER_7) using (VEHICLECONTROLISR_RB) void VehicleControlISR (void)
{
	// ONLY USE FOR CPU LOAD VERIFICATION --- HIDAC_FAIL_LED_ON();
	if (GetBurninConfig () == TRUE)
	{
		BurnInISR ();
	}
    else if (GetStartupSuccessful() == TRUE)
    {
        Task8ms();
    }
	// ONLY USE FOR CPU LOAD VERIFICATION --- HIDAC_FAIL_LED_OFF();
	_nop();

}

/*----------------------------------------------------------------------------
*  Module:
*   BadPlus15VISR
*
*     This function is automatically invoked by the C167 when +15 volts
*     is deemed to be inadequate.
*
----------------------------------------------------------------------------*/
interrupt (CAPCOM_REGISTER_15) using (FIFTEEN_V_RB) void BadPlus15VISR (void)
{
    SetFifteenVoltBad (PLUS_15_BAD_MASK);

    /* Disable this ISR */
    CC15IC = 0;
}

/*----------------------------------------------------------------------------
*  Module:
*   BadMinus15VISR
*
*     This function is automatically invoked by the C167 when -15 volts
*     is deemed to be inadequate.
*
--------------------------------------------------------------------------- */
interrupt (CAPCOM_REGISTER_14) using (FIFTEEN_V_RB) void BadMinus15VISR (void)
{
    SetFifteenVoltBad (MINUS_15_BAD_MASK);

    /* Disable this ISR */
    CC14IC = 0;
}


/*----------------------------------------------------------------------------
*  Module:
*   Capcom23ISR
*
*   This function is triggered automatically when the the value of the T8
*   CAPCOM timer is equal to the value of the CAPCOM23 (CC23) register.
*
--------------------------------------------------------------------------- */
interrupt (CAPCOM_REGISTER_23) using (CAPCOM_RB) void Capcom23ISR (void)
{

    DISABLE_ALL_INTERRUPTS();

    ENABLE_ALL_INTERRUPTS();

}


/*----------------------------------------------------------------------------
*  Module:
*   PWMIn1ISR
*
*     This function is automatically invoked by the C167 when the DONE1 signal
*     is made active low by the U41 CPLD.
*
----------------------------------------------------------------------------*/
interrupt (CAPCOM_REGISTER_10) using (PWMIN1_RB) void PWMIn1ISR (void)
{
	PWMInputIsr1();
}

