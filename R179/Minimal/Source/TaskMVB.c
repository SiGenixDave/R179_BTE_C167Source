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
#include "Timer.h"

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

static void ToggleTestLed(void);

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
void Task8ms(void)
{
	ToggleTestLed();
}

//--------------------------------------------------------------------------
// Module:
//  ToggleTestLed
//
///   This function is responsible for toggling the state of the test (Yellow)
///   LED every 250 milliseconds (2 Hz).
///
//--------------------------------------------------------------------------
static void ToggleTestLed(void)
{
	static BOOLEAN tmFlag = TRUE;
	static UINT_16 testLEDState = 0;

	/* Has the timer expired */
	if (tmFlag == TRUE)
	{
		/* Reset the time and the expiration flag */
		TM_Allocate(250, &tmFlag);
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