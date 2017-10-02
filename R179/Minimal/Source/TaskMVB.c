/******************************************************************************
*
* Copyright (C) 2014-15 Bombardier
*
* File Name: TaskMVB.c
*
* Revision History:
*   08/31/2014 - das - Created
*
******************************************************************************/
///   \file
///   This file contains the code that handles the MVB interface 
#define taskmvb_C


/*--------------------------------------------------------------------------
                              INCLUDE FILES
  --------------------------------------------------------------------------*/
#ifndef _DEBUG
#include <reg167.h>
#endif

#include "types.h"
#include "Digital.h"
#include "Analog.h"

#include "PwmOut.h"
#include "PwmIn.h"
#include "TaskMVB.h"
#include "mvbvar.h"
#include "Timer.h"
#include "SerComm.h"
#include "Pwm.h"
#include "BurnIn.h"

/*--------------------------------------------------------------------------
                             MODULE CONSTANTS
  --------------------------------------------------------------------------*/

#define VALID_MVB_ACTIVE_COUNT		5		/* 16 * 5 = 80 msecs */
#define VALID_MVB_INACTIVE_COUNT	5		/* 16 * 5 = 80 msecs */

/*--------------------------------------------------------------------------
                              MODULE MACROS
  --------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
                             MODULE DATA TYPES
  --------------------------------------------------------------------------*/
// Describes the state of the MVB input watchdog
typedef enum
{
    MVB_WDOG_WAIT_FOR_ACTIVE,
    MVB_WDOG_ACTIVE
} MVBWatchDogState_e;

// Structure to support determining if MVB input watchdog is functioning
typedef struct
{
    UINT_16 *currentValue;
    UINT_16 previousValue;
    UINT_16 activeCount;
    UINT_16 inactiveCount;
    MVBWatchDogState_e state;

} MVBWatchDog_t;

/*--------------------------------------------------------------------------
                              MODULE VARIABLES
  --------------------------------------------------------------------------*/

// MVB Input Watchdog variable 1
MVBWatchDog_t mvbWdog1 =
{
    &VCUHD_658_0_UINT16,
    0,
    0,
    0,
    MVB_WDOG_WAIT_FOR_ACTIVE
};

// MVB Input Watchdog variable 2
MVBWatchDog_t mvbWdog2 =
{
    &VCUHD_659_0_UINT16,
    0,
    0,
    0,
    MVB_WDOG_WAIT_FOR_ACTIVE
};


static BOOLEAN m_MvbReady;
static UINT_16 m_HidacErrorStatus = 0;
static UINT_16 m_TaskCounter = 0;

/*--------------------------------------------------------------------------
                             MODULE PROTOTYPES
  --------------------------------------------------------------------------*/

extern void mvb_fast_read_T16 (void);
extern void mvb_fast_write_T16 (void);

static void MonitorMVBWatchdog (MVBWatchDog_t *aVar);
static void ToggleTestLed (void);


//--------------------------------------------------------------------------
// Module:
//  Task8ms
//
///   This function is invoked by an interrupt that occurs every 8 milliseconds.
///   It is responsible for reading MVB input variables and updating the 
///   HiDAC outputs as well reading HiDAC inputs and updating MVB output 
///   variables.
///
//--------------------------------------------------------------------------
void Task8ms (void)
{
    static BOOLEAN monitorMVB1 = FALSE;
    BOOLEAN communicationOk = FALSE;

    if (m_MvbReady == TRUE)
    {
        /* Read MVB input variables */
        mvb_fast_read_T16();

        /* Alternate readings of the MVB watchdog every cycle */
        if (monitorMVB1 == TRUE)
        {
            MonitorMVBWatchdog (&mvbWdog1);
            HDVCU_651_208_UINT16 = * (mvbWdog1.currentValue);
            HDVCU_651_224_UINT16 = mvbWdog1.previousValue;
            monitorMVB1 = FALSE;
        }
        else
        {
            MonitorMVBWatchdog (&mvbWdog2);
            monitorMVB1 = TRUE;
        }

        /* Toggle the TEST (Yellow) LED if both watchdogs are active. Otherwise, turn the TEST LED off. */
        if ((mvbWdog1.state == MVB_WDOG_ACTIVE) && (mvbWdog2.state == MVB_WDOG_ACTIVE))
        {
            communicationOk = TRUE;
            ToggleTestLed();
        }
        else
        {
            communicationOk = FALSE;
            HIDAC_TEST_LED_OFF();
        }

        /* Write MVB variable to Digital Output */
        ReadMvbUpdateDigitalOutputs (communicationOk);

        /* Write MVB variable to Analog Output */
        ReadMvbUpdateAnalogOutputs (communicationOk);

        /* Write PWM output */
        ReadMvbUpdatePwmOutputs (communicationOk);

        /* Increment Watchdogs for output telegrams 0x650 and 0x651 */
        HDVCU_650_0_UINT16++;
        HDVCU_651_0_UINT16++;

        /* Write PWM Inputs to MVB variables */
        ReadPwmInputUpdateMvb();

        /* Write Analog Inputs to MVB variables */
        ReadAnalogInputsUpdateMvb();

        /* Write Digital Inputs to MVB variables */
        ReadDigitalInputsUpdateMvb();

        /* Write MVB outputs */
        mvb_fast_write_T16();
    }

	m_TaskCounter++;
}

//--------------------------------------------------------------------------
// Module:
//  SetMvbReady
//
///   Mutator function for m_MvbReady variable. TRUE is only accepted as 
///   a parameter.
///
///   \param aMvbReady - TRUE to indicate MVB is done initializing
///
//--------------------------------------------------------------------------
void SetMvbReady (BOOLEAN aMvbReady)
{
	if (aMvbReady == TRUE)
	{
		m_MvbReady = TRUE;
	}
}

//--------------------------------------------------------------------------
// Module:
//  UpdateHidacError
//
///   Updates the HiDAC error status
///
///   \param aErrorMask - bit-field indicating the type of HiDAC error 
///
//--------------------------------------------------------------------------
void UpdateHidacError (HidacError_e aErrorMask)
{
    m_HidacErrorStatus |= (UINT_16) (aErrorMask);
}


//--------------------------------------------------------------------------
// Module:
//  HidacErrorExists
//
///   Mutator function for m_MvbReady variable. TRUE is only accepted as 
///   a parameter.
///
///   \param am_MvbReady - TRUE to indicate MVB is done initializing
///
//--------------------------------------------------------------------------
BOOLEAN HidacErrorExists (void)
{
    if (m_HidacErrorStatus != 0)
    {
        return TRUE;
    }
    return FALSE;
}

//--------------------------------------------------------------------------
// Module:
//  MonitorMVBInterrupt
//
///   Determines if the 8 ms MVB interrupt is occurring. Must be called from
///   another interrupt. 
///
///   \param maxDeadCount - maximum amount of consecutive calls that 8 ms interrupt
///                         hasn't executed
///
///   \returns BOOLEAN - TRUE if 8 ms MVB interrupt is occurring; FALSE otherwise
///
//--------------------------------------------------------------------------
BOOLEAN MonitorMVBInterrupt (UINT_16 maxDeadCount)
{
	static UINT_16 previousTaskCounter = 0;
	static UINT_16 consecutiveTaskCounts = 0;
	BOOLEAN retVal = FALSE;

	/* Determine if the task counter has incremented */
	if (previousTaskCounter != m_TaskCounter)
	{
		consecutiveTaskCounts = 0;
		retVal = TRUE;
	}
	else
	{
		/* No life... determine if the dead count has been exceeded */
		consecutiveTaskCounts++;
		if (consecutiveTaskCounts >= maxDeadCount)
		{
			/* Dead */
			retVal = FALSE;
		}
		else
		{
			/* Not quite dead yet */
			retVal = TRUE;
		}
	}

	previousTaskCounter = m_TaskCounter;

	return retVal;

}

//--------------------------------------------------------------------------
// Module:
//  MonitorMVBWatchdog
//
///   This function is used to monitor an MVB Watchdog variable. Variable must 
///   be increasing	in value for the watchdog to be considered active. There is 
///   hysteresis built in to avoid false trips in either the active or 
///   inactive direction
///
///   \param aVar - pointer to the watchdog variable structure
///
//--------------------------------------------------------------------------
static void MonitorMVBWatchdog (MVBWatchDog_t *aVar)
{

    switch (aVar->state)
    {
        case MVB_WDOG_WAIT_FOR_ACTIVE:
        default:
            /* Counter should be increasing */
            if (*(aVar->currentValue) > aVar->previousValue)
            {
                (aVar->activeCount)++;
            }
            else
            {
                aVar->activeCount = 0;
            }

            /* IF the watchdog counter has increased in value for VALID_MVB_ACTIVE_COUNT
               consecutive counts, then declare the watchdog value active */
            if (aVar->activeCount >= VALID_MVB_ACTIVE_COUNT)
            {
                aVar->state = MVB_WDOG_ACTIVE;
                aVar->inactiveCount = 0;
            }
            break;

        case MVB_WDOG_ACTIVE:
            /* Counter should be increasing */
            if (*(aVar->currentValue) > aVar->previousValue)
            {
                aVar->inactiveCount = 0;
            }
            else
            {
                (aVar->inactiveCount)++;
            }
            /* If the counter stops increasing for VALID_MVB_INACTIVE_COUNT
               consecutive counts, then declare the watchdog value inactive */
            if (aVar->inactiveCount >= VALID_MVB_INACTIVE_COUNT)
            {
                aVar->state = MVB_WDOG_WAIT_FOR_ACTIVE;
                aVar->activeCount = 0;
            }
            break;

    }

    /* Store the current watchdog value into the previous for comparison next time */
    aVar->previousValue =  *(aVar->currentValue);

}

//--------------------------------------------------------------------------
// Module:
//  ToggleTestLed
//
///   This function is responsible for toggling the state of the test (Yellow) 
///   LED every 250 milliseconds (2 Hz).
///
//--------------------------------------------------------------------------
static void ToggleTestLed (void)
{
    static BOOLEAN tmFlag = TRUE;
    static UINT_16 testLEDState = 0;

    /* Has the timer expired */
    if (tmFlag == TRUE)
    {
        /* Reset the time and the expiration flag */
        TM_Allocate (250, &tmFlag);
        /* Toggle the LED state */
        if (testLEDState == 0)
        {
            HIDAC_TEST_LED_ON();
        }
        else
        {
            HIDAC_TEST_LED_OFF();
        }
        /* Invert the LED state signal */
        testLEDState ^= 0x0001;
    }
}



