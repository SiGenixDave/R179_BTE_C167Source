/******************************************************************************
*
* Copyright (C) 2014-15 Bombardier
*
* File Name: Main.c
*
* Revision History:
*
******************************************************************************/
///   \file
///   This file contains the code that initializes all HIDAC hardware

/*--------------------------------------------------------------------------
							  INCLUDE FILES
  --------------------------------------------------------------------------*/
#ifndef _DEBUG
#include <reg167.h>
#endif

#include <stdio.h>
#include "Types.h"
#include "HWInit.h"
#include "DigOut.h"
#include "Digital.h"
#include "Pwm.h"
#include "TaskMVB.h"
#include "HexLED.h"
#include "CRC.h"
#include "RAMTest.h"
#include "Debug.h"
#include "Ipack.h"
#include "Timer.h"
#include "CmdProc.h"

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
		  //const UINT_16 _at(0x110000) = 0x55AA;

		  /*--------------------------------------------------------------------------
									   MODULE PROTOTYPES
			--------------------------------------------------------------------------*/

			//--------------------------------------------------------------------------
			// Module:
			//  main
			//
			///   This function is the main program. It provides initialization of hardware
			///   and software components and software. The main loop  portion handles
			///   only the Debug task.
			///
			//--------------------------------------------------------------------------

void main(void)
{
	UINT_16 temp = 0;

	/* Disable all C167 interrupts */
	DISABLE_ALL_INTERRUPTS();

	/* Don't allow any interrupt processing to occur */
	SetStartupSuccessful(FALSE);

	/* Release MVB IPACK hardware */
	ReleaseIpackHw();

	/* Configure Push-Pull digital IOs at P7 to Digital Outputs */
	DO_Init(0xFF);

	/* Initialize the HiDAC outputs */
	InitP8ForDigOuts();
	InitDigitalOutputs();
	InitAnalogOutputs();

	/* Toggle VDrive and CPU watchdogs */
	ToggleVDriveWatchdog();
	ToggleCPUWatchdog();

	/* Set the state of the HIDAC LEDs */
	HIDAC_TEST_LED_ON();
	HIDAC_FAIL_LED_OFF();

	/* Enable SSMR1, SSMR2, VDrive & +5V to Hex LED Display */
	DO_WriteBank(DIGOUT_BANK2, 0x00FF);

	/* Initialize the software to detect a 15 volt failure */
	Init15VoltFailDetect();
	/* This call initializes the 10 ms Timer ISR
*/
	InitTimer0();
	InitTimer1();
	InitTimer7();

	ENABLE_ALL_INTERRUPTS();

	/* Allow the VDrive watchdog to toggle and wait some timer before
	   trying to drive display */
	SetVdriveDisable(FALSE);
	TM_Wait(250);

	/* Display the stored CRC checksum for approximately 1 second at startup */
	DisplayCRConHexLED();

	/* Perform the RAM Test. */
	if (!VerifyRAM())
	{
		HIDAC_FAIL_LED_ON();
	}

	/* Perform the ROM Test. */
	if (!VerifyCRCAtStart())
	{
		HIDAC_FAIL_LED_ON();
	}

	/* Init the RS-232 */
	InitSerialCommunications();

	/* Init the analog inputs */
	InitAnalogInputs();

	/* Init the PWM input hardware */
	PwmInInit();

	/* Init the MVB IPACK interface */
	InitMVBIpack();

	/* DAS This call to InitPwmOutputs needs to be after InitMVBIpack. After Sweden made an
	  an upgrade to the IPACK CPLD to rev 2, for some reason, initializing PWM1 prior to initializing
	  the IPACK causes the IPACK not to initialize properly. Didn't have time to investigate ????? */
	InitPwmOutputs();

	/* stop here if +/- 15VDC supply is low */
	if (Get15VoltBad())
	{
		HexLEDUpdate(0x15);
		HIDAC_FAIL_LED_ON();
	}

	HIDAC_TEST_LED_OFF();
	HIDAC_FAIL_LED_OFF();

	SetStartupSuccessful(TRUE);

	// Setting the argument to FALSE indicates that the baseline CPU loading has
	// already been performed and that the CPU load check, that can be performed via
	// the serial port, will report the CPU loading as a percentage. Setting to TRUE
	// should only be done when capturing the CPU baseline loading and should be returned
	// to FALSE for normal operation
	DebugInit(FALSE);

	HexLEDUpdate(0x00);

	/* This is the background loop */
	while (1)
	{
		ApplicationService();

		// Used to measure CPU loading
		// DebugUpdateLoadCounter();

		/* Code that executes in the main while forever loop. All RealTime critical
			application code is interrupt driven */
			// DebugService();
	}
}