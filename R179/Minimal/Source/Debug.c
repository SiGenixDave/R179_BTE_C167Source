/******************************************************************************
*
* Copyright (C) 2014-15 Bombardier
*
* File Name: Debug.c
*
* Revision History:
*   08/31/2014 - das - Created
*
******************************************************************************/
///   \file
///   This file contains the code that services the debug port

#define debug_C

/*--------------------------------------------------------------------------
                              INCLUDE FILES
  --------------------------------------------------------------------------*/
#include <stdio.h>

#include "Types.h"
#include "Debug.h"
#include "SerComm.h"
#include "Timer.h"
#include "mvbvar.h"
#include "Digital.h"
#include "TaskMVB.h"
#include "PwmIn.h"
#include "Pwm.h"
#include "AnalogIn.h"
#include "AlogOut.h"
#include "Analog.h"
#include "CRC.h"
#include "BurnIn.h"
#include "Version.h"
#include "HWInit.h"


/*--------------------------------------------------------------------------
                             MODULE CONSTANTS
  --------------------------------------------------------------------------*/
/* Update this value after establishing the baseline load count. This value should 
   be updated when the above #define is active and a 'l' followed by a "L" is performed */
#define BASELINE_UNLOAED_CPU_COUNT				7143

/*--------------------------------------------------------------------------
                              MODULE MACROS
  --------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
                             MODULE DATA TYPES
  --------------------------------------------------------------------------*/

// Bit field used for enumerating the available debug message types
typedef enum
{
    DEBUG_ANALOG_IN = 0x0001,
    DEBUG_ANALOG_OUT = 0x0002,
    DEBUG_DIGITAL_IN = 0x0004,
    DEBUG_DIGITAL_OUT = 0x0008,
    DEBUG_PWM_IN = 0x0010,
    DEBUG_PWM_OUT = 0x0020,
    DEBUG_MVB_WATCHDOG_IN = 0x0040,
    DEBUG_MVB_WATCHDOG_OUT = 0x0080

} DebugInfo_e;

// prototype for Debug functions...
typedef void (*DebugFnPtr) (void);

typedef struct
{
	DebugInfo_e flag;
	const char *str;
	DebugFnPtr debugFunction;

} DebugString_t;

/*--------------------------------------------------------------------------
                              MODULE VARIABLES
  --------------------------------------------------------------------------*/
static void DebugMVBWatchdogInput (void);
static void DebugMVBWatchdogOutput (void);
static void DebugPwmInput (void);
static void DebugPwmOutput (void);
static void DebugAnalogInput (void);
static void DebugAnalogOutput (void);

static UINT_16 m_EnableFlags;
static BOOLEAN m_DebugTmFlag;
static UINT_16 m_DebugUpdateTime;
static BOOLEAN m_EstablishCPULoadBaseline;

static DebugString_t m_DebugInfo[] =
{
	{DEBUG_DIGITAL_IN,			"Digital Input Debugging",			NULL},					//TODO
	{DEBUG_DIGITAL_OUT,			"Digital Output Debugging",			NULL},					//TODO
	{DEBUG_ANALOG_IN,			"Analog Input Debugging",			DebugAnalogInput},
	{DEBUG_ANALOG_OUT,			"Analog Output Debugging",			DebugAnalogOutput},
	{DEBUG_PWM_IN,				"PWM Input Debugging",				DebugPwmInput},
	{DEBUG_PWM_OUT,				"PWM Output Debugging",				DebugPwmOutput},
	{DEBUG_MVB_WATCHDOG_IN,		"MVB Input Watchdog Debugging",		DebugMVBWatchdogInput},
	{DEBUG_MVB_WATCHDOG_OUT,	"MVB Output Watchdog Debugging",	DebugMVBWatchdogOutput},
};

static UINT_32 m_DebugCPULoad;
static UINT_32 m_CPUSpareProcessingCounter;
static UINT_32 m_TimeStamp;

/*--------------------------------------------------------------------------
                             MODULE PROTOTYPES
  --------------------------------------------------------------------------*/

static void DebugUpdateTimeStamp (void);
static void DebugParseInputChar (UINT_16 aInChar);
static void DebugUpdateUserInterface (void);
static void DebugDisplayCalculatedCPULoad (void);
static void DebugUpdateState (DebugInfo_e aFlag);
static void DebugUpdateTime (BOOLEAN aIncreaseTime);
static void DebugUpdateUserInterface (void);
static void DebugMVBWatchdogInput (void);
static void DebugMVBWatchdogOutput (void);
static void DebugPwmInput (void);
static void DebugPwmOutput (void);
static void DebugAnalogInput (void);
static void DebugAnalogOutput (void);
static void DebugHelp (void);
static void DebugUpdateMinPwmPulseWidth (BOOLEAN aIncreaseTime);
static void DebugResetPwmInDiags (void);

//--------------------------------------------------------------------------
// Module:
//  DebugInit
//
///   This function initializes all of the related variables used for the 
///   debug messages displayed to the user via the  RS-232 port.
///
///   \param establishCPULoadbaseline - TRUE to establish a baseline CPU
///                                     load count, FALSE for normal operation
///
//--------------------------------------------------------------------------
void DebugInit (BOOLEAN establishCPULoadbaseline)
{
    // Clear all of the debug function enable flags
    m_EnableFlags = 0x0000;
    
	// Set the initial update rate
    m_DebugTmFlag = TRUE;
    m_DebugUpdateTime = 1000;

	m_EstablishCPULoadBaseline = establishCPULoadbaseline;

}

//--------------------------------------------------------------------------
// Module:
//  DebugService
//
///   This function is invoked in the background. It is used to poll
///   the RS-232 port for any user input and then display the desired
///   debug information at a user specified period
///
//--------------------------------------------------------------------------
void DebugService (void)
{
    INT_16 debugInChar;

    // Read serial port to determine if any new characters available
    debugInChar = SC_GetChar();

    if (debugInChar != EOF)
    {
        DebugParseInputChar (debugInChar);
    }

	// If timer expired, get desired info and send to serial port
	if (m_DebugTmFlag == TRUE)
	{
		DebugUpdateTimeStamp();
		TM_Allocate (m_DebugUpdateTime, &m_DebugTmFlag);

		// Only send info to the screen cyclically if any of the flags are set
		if (m_EnableFlags)
		{
			DebugUpdateUserInterface();
		}
	}

}

//--------------------------------------------------------------------------
// Module:
//  DebugPrintSoftwareVersion
//
///   This function outputs the software version and CRC to the Debug Port
///
//--------------------------------------------------------------------------
void DebugPrintSoftwareVersion (void)
{
	UINT_16 crc;
    CHAR str[20];

	SC_PutsAlways ("\n\r");
    SC_PutsAlways ("**************************************\n\r");
    SC_PutsAlways ("R179 HiDAC Software Version ");
	SC_PutsAlways (GetSoftwareVersionString());
	SC_PutsAlways (".0\n\r");
	crc = GetCRC();
	sprintf (str, "CRC = 0x%x\n\r", crc);
	SC_PutsAlways (str);
    SC_PutsAlways ("**************************************\n\r");
}

//--------------------------------------------------------------------------
// Module:
//  DebugUpdateLoadCounter
//
///   This function increments the spare processing counter which is used
///   to establish the CPU loading
///
//--------------------------------------------------------------------------
void DebugUpdateLoadCounter (void)
{
	m_CPUSpareProcessingCounter++;
}

//--------------------------------------------------------------------------
// Module:
//  DebugCalculateCpuLoad
//
///   This function is invoked after the one shot timer expires. When establishing
///   a baseline count (BASELINE_LOAD_CALC defined), the amount of counts that have 
///   occurred over the specified time is saved. This value will be used for the 
///   real time CPU load calculation. When performing the real time load calculation 
///   (BASELINE_LOAD_CALC not defined), this value indicates actual CPU load 
///   (percentage of time not spent in the background loop).
///
//--------------------------------------------------------------------------
void DebugCalculateCpuLoad (void)
{
	if (!m_EstablishCPULoadBaseline) 
	{
		// m_DebugCPULoad now is the CPU Load % * 1000. For example
		// if m_DebugCPULoad = 250 is 25.0% CPU Load (75% spare CPU)
		// if m_DebugCPULoad = 500 is 50.0% CPU Load (55% spare CPU)
		// if m_DebugCPULoad = 750 is 75.0% CPU Load (25% spare CPU)
		m_DebugCPULoad = ((BASELINE_UNLOAED_CPU_COUNT - m_CPUSpareProcessingCounter) * 1000) / 
			                                 BASELINE_UNLOAED_CPU_COUNT;
	}
	else
	{
		// Copy to BASELINE_UNLOAED_CPU_COUNT
		m_DebugCPULoad = m_CPUSpareProcessingCounter;
	}

}

//--------------------------------------------------------------------------
// Module:
//  DebugUpdateTimeStamp
//
///   This function updates the HiDAC timestamp, which is used to maintain 
///   the amount of time since power on and is displayed when debugging 
///   information
///
//--------------------------------------------------------------------------
static void DebugUpdateTimeStamp (void)
{
    // Update the timestamp with the time increment
    m_TimeStamp += m_DebugUpdateTime;

}

//--------------------------------------------------------------------------
// Module:
//  DebugUpdateUserInterface
//
///   This function updates the user with the amount of time since
///   the HiDAC was powered on or reset. It also scans the enabled
///   debug functions variable to determine what debug info the user
///   wishes to display and then invokes the proper function via
///   the function pointer list.
///
//--------------------------------------------------------------------------
static void DebugUpdateUserInterface (void)
{

    UINT_16 index;
    CHAR str[20];

    UINT_32 hrs;
    UINT_32 mins;
    UINT_32 secs;
    UINT_32 tenthsOfSecs;

    SC_PutsAlways ("Time since power on = Hrs: ");

    hrs = m_TimeStamp / 3600000;
    sprintf (str, "%lu", hrs);
    SC_PutsAlways (str);

    mins = (m_TimeStamp % 3600000) / 60000;
    sprintf (str, " Mins: %lu", mins);
    SC_PutsAlways (str);

    // secs = seconds * 1000
    secs = (m_TimeStamp % 3600000) % 60000;
    tenthsOfSecs = secs % 1000 / 100;
    sprintf (str, " Secs: %lu.%lu\r\n", secs / 1000, tenthsOfSecs);
    SC_PutsAlways (str);


    // Scan the variable that contains the debug enable flags
    index = 0;
    while (index < sizeof(m_DebugInfo)/sizeof(DebugString_t))
    {
		if ((m_DebugInfo[index].flag & m_EnableFlags) != 0)
        {
			if (m_DebugInfo[index].debugFunction != NULL)
            {
				// Invoke the debug function 
                m_DebugInfo[index].debugFunction();
            }
        }
        index++;
    }
}

//--------------------------------------------------------------------------
// Module:
//  DebugParseInputChar
//
///   This function is responsible for turning on/off debug information.
///
///   \param aInChar - character that indicates the type of debug info to turn on/off
///
//--------------------------------------------------------------------------
static void DebugParseInputChar (UINT_16 aInChar)
{
    CHAR inChar = (CHAR)aInChar;

    switch (inChar)
    {
        case 'd':
            DebugUpdateState (DEBUG_DIGITAL_IN);
            break;

        case 'D':
            DebugUpdateState (DEBUG_DIGITAL_OUT);
            break;

        case 'a':
            DebugUpdateState (DEBUG_ANALOG_IN);
            break;

        case 'A':
            DebugUpdateState (DEBUG_ANALOG_OUT);
            break;

        case 'p':
            DebugUpdateState (DEBUG_PWM_IN);
            break;

        case 'P':
            DebugUpdateState (DEBUG_PWM_OUT);
            break;

        // MVB input watchdog (to HiDAC)
        case 'w':
            DebugUpdateState (DEBUG_MVB_WATCHDOG_IN);
            break;

        // MVB output watchdog (from HiDAC)
        case 'W':
            DebugUpdateState (DEBUG_MVB_WATCHDOG_OUT);
            break;

        case 'h':
        case 'H':
            DebugHelp ();
            break;

        // Output software version
        case 'v':
        case 'V':
			DebugPrintSoftwareVersion();
            break;

        // Output HiDAC errors
        case 'e':
        case 'E':
            SC_PutsAlways ("\n\r");
            SC_PutsAlways ("**************************************\n\r");
			//TODO
			SC_PutsAlways ("**************************************\n\r");
            break;

        // Decrease update time
        case 't':
            DebugUpdateTime (FALSE);
            break;

        // Increase update time
        case 'T':
            DebugUpdateTime (TRUE);
            break;

		// Increase/Decrease min pulse width time
		case '+':
		case '-':
			DebugUpdateMinPwmPulseWidth ((inChar == '+') ? TRUE : FALSE);
			break;

		case '0':
			DebugResetPwmInDiags ();
			break;

		case 'b':
		case 'B':
			BurnInCode ();
			break;

		// Used to reset the load counter and kick off the one shot 1 second timer
		case 'l':
			if (m_EstablishCPULoadBaseline) 
			{
				DisableSystemInterrupts();
			}
			m_CPUSpareProcessingCounter = 0;
			StartTimer0();
			break;

		case 'L':
			DebugDisplayCalculatedCPULoad();
			break;

		default:
            break;
    }


}

//--------------------------------------------------------------------------
// Module:
//  DebugDisplayCalculatedCPULoad
//
///   This function displays either the raw CPU LOAD baseline count when establishing
///   such or the calculated CPU Load (percentage of time spent in ISRs).
///
//--------------------------------------------------------------------------
static void DebugDisplayCalculatedCPULoad (void)
{
	CHAR str[20];

	SC_PutsAlways ("----------------------------------------------------------\n\r");

	if (!m_EstablishCPULoadBaseline)
	{
		SC_PutsAlways ("-- CPU Load Utilization Calculation => ");
		sprintf (str, "%lu.%lu", m_DebugCPULoad / 10, m_DebugCPULoad % 10);
		SC_PutsAlways (str);
		SC_PutsAlways ("%\n\r");
	}
	else
	{
		SC_PutsAlways ("-- CPU Load Utilization Baseline value => ");
		sprintf (str, "%lu", m_DebugCPULoad);
		SC_PutsAlways (str);
		SC_PutsAlways ("\n\r");
	}

	SC_PutsAlways ("----------------------------------------------------------\n\r");

}

//--------------------------------------------------------------------------
// Module:
//  DebugUpdateState
//
///   This function updates the time increment / period for displaying debug
///   information to the user. The change increment is 100 ms.
///
///   \param aDebug - string containing debug information
///
//--------------------------------------------------------------------------
static void DebugUpdateState (DebugInfo_e aFlag)
{
    const CHAR *state;
	BOOLEAN flagFound;
	UINT_16 i;

	// Determine if the flag passed is a member of the list
	i = 0;
	flagFound = FALSE;
	while (i < sizeof(m_DebugInfo)/sizeof(DebugString_t))
	{
		if (aFlag == m_DebugInfo[i].flag)
		{
			flagFound = TRUE;
			break;
		}
		i++;
	}

	// Leave the function and do nothing if the flag wasn't found
	if (flagFound == FALSE)
	{
		return;
	}

	// Invert (toggle) the flag
    m_EnableFlags ^= aFlag;

	// Inform user whether debug capability has been turned on or off
	SC_PutsAlways ("\n\r");
	SC_PutsAlways ("----------------------------------------------------------\n\r");
	SC_PutsAlways (m_DebugInfo[i].str);

	SC_PutsAlways (" Turned");

	state = (m_EnableFlags & aFlag) ? " On" : " Off";

	SC_PutsAlways (state);
	SC_PutsAlways ("\n\r");
	SC_PutsAlways ("----------------------------------------------------------\n\r");

}

//--------------------------------------------------------------------------
// Module:
//  DebugUpdateTime
//
///   This function updates the time increment / period for displaying debug
///   information to the user. The change increment is 100 ms.
///
///   \param aIncreaseTime - TRUE to increase the time; FALSE to decrease
///
//--------------------------------------------------------------------------
static void DebugUpdateTime (BOOLEAN aIncreaseTime)
{
    CHAR str[20];
	const UINT_16 DEBUG_UPDATE_RESOLUTION_MS = 100;

	// Increase the sample time
    if (aIncreaseTime == TRUE)
    {
        m_DebugUpdateTime += DEBUG_UPDATE_RESOLUTION_MS;
    }
    else
    {
		// Decrease the sample time and verify it's not below the minimum
		// resolution. 
        if (m_DebugUpdateTime != DEBUG_UPDATE_RESOLUTION_MS)
        {
            m_DebugUpdateTime -= DEBUG_UPDATE_RESOLUTION_MS;
        }
		// Inform user sample time can't be decreased any more and exit function
        else
        {
            SC_PutsAlways ("\n\r");
            SC_PutsAlways ("*************************************************\n\r");
            SC_PutsAlways ("** Update time can't be decreased below ");
			sprintf (str, "%d ms **\n\r", DEBUG_UPDATE_RESOLUTION_MS);
			SC_PutsAlways (str);
            SC_PutsAlways ("*************************************************\n\r");
            SC_PutsAlways ("\n\r");
            return;
        }
    }

    // Inform user of new m_DebugInfo time/period
    SC_PutsAlways ("\n\r");
    SC_PutsAlways ("**********************************************\n\r");
    SC_PutsAlways ("Update time now set at ");
    sprintf (str, "%d ms\n\r", m_DebugUpdateTime);
    SC_PutsAlways (str);
    SC_PutsAlways ("**********************************************\n\r");
    SC_PutsAlways ("\n\r");

}

//--------------------------------------------------------------------------
// Module:
//  DebugMVBWatchdogInput
//
///   This function displays relevant MVB Watchdog Input information to the
///   user.
///
///
//--------------------------------------------------------------------------
static void DebugMVBWatchdogInput (void)
{
    CHAR str[10];

    SC_PutsAlways ("\n\r");
    SC_PutsAlways ("**********************************************\n\r");
    SC_PutsAlways ("MVB Input Watchdog 1 = ");
    sprintf (str, "0x%x\n\r", VCUHD_658_0_UINT16);
    SC_PutsAlways (str);
    SC_PutsAlways ("MVB Input Watchdog 2 = ");
    sprintf (str, "0x%x\n\r", VCUHD_659_0_UINT16);
    SC_PutsAlways (str);
    SC_PutsAlways ("**********************************************\n\r");
    SC_PutsAlways ("\n\r");
}


//--------------------------------------------------------------------------
// Module:
//  DebugMVBWatchdogOutput
//
///   This function displays relevant MVB Watchdog Input information to the
///   user.
///
///
//--------------------------------------------------------------------------
static void DebugMVBWatchdogOutput (void)
{
    CHAR str[10];

    SC_PutsAlways ("\n\r");
    SC_PutsAlways ("**********************************************\n\r");
    SC_PutsAlways ("MVB Output Watchdog 1 = ");
    sprintf (str, "0x%x\n\r", HDVCU_650_0_UINT16);
    SC_PutsAlways (str);
    SC_PutsAlways ("MVB Output Watchdog 2 = ");
    sprintf (str, "0x%x\n\r", HDVCU_650_0_UINT16);
    SC_PutsAlways (str);
    SC_PutsAlways ("**********************************************\n\r");
    SC_PutsAlways ("\n\r");
}


//--------------------------------------------------------------------------
// Module:
//  DebugPwmInput
//
///   This function displays relevant PWM Input information to the
///   user.
///
///
//--------------------------------------------------------------------------
static void DebugPwmInput (void)
{
    CHAR str[100];

    UINT_16 period;
    UINT_16 width;
    UINT_16 status;
    UINT_16 isrCount;
	UINT_16 minPeriod;
	UINT_16 maxPeriod;
	UINT_16 minWidth;
	UINT_16 maxWidth;

    // Get the period as read by the PWM input processing procedure
    period = GetPwmPeriod (PWMIN_0);
    width = GetPwmPulseWidth (PWMIN_0);
    status = GetPwmInStatus (PWMIN_0);
	minPeriod = GetPwmMinPeriod (PWMIN_0);
	maxPeriod = GetPwmMaxPeriod (PWMIN_0);
	minWidth = GetPwmMinPulseWidth (PWMIN_0);
	maxWidth = GetPwmMaxPulseWidth (PWMIN_0);
    // Get the PWM input ISR count
    isrCount = GetPwmInIsrCount (PWMIN_0);

    // Display the data collected above
    SC_PutsAlways ("\n\r");
    SC_PutsAlways ("**********************************************\n\r");
    SC_PutsAlways ("PWM Input 1 Period [current, max, min] (usecs) = ");
    sprintf (str, "%d, %d, %d\n\r", period, maxPeriod, minPeriod);
    SC_PutsAlways (str);
    SC_PutsAlways ("PWM Input 1 PulseWidth [current, max, min] (usecs) = ");
    sprintf (str, "%d, %d, %d\n\r", width, maxWidth, minWidth);
    SC_PutsAlways (str);
    SC_PutsAlways ("PWM Input 1 Status (bit 0 = PWM signal state) = ");
    sprintf (str, "0x%x\n\r", status);
    SC_PutsAlways (str);
    SC_PutsAlways ("PWM Input 1 ISR Count = ");
    sprintf (str, "%d\n\r", isrCount);
    SC_PutsAlways (str);
    SC_PutsAlways ("MVBVAR 64 (Period) [d,x] = ");
    sprintf (str, "%d , 0x%x\n\r", HDVCU_650_64_UINT16, HDVCU_650_64_UINT16);
    SC_PutsAlways (str);
    SC_PutsAlways ("MVBVAR 80 (Pulse Width) [d,x] = ");
    sprintf (str, "%d , 0x%x\n\r", HDVCU_650_80_UINT16, HDVCU_650_80_UINT16);
    SC_PutsAlways (str);
    SC_PutsAlways ("**********************************************\n\r");
    SC_PutsAlways ("\n\r");

}

//--------------------------------------------------------------------------
// Module:
//  DebugPwmOutput
//
///   This function displays relevant PWM Output information to the
///   user.
///
///
//--------------------------------------------------------------------------
static void DebugPwmOutput (void)
{
    CHAR str[100];
	UINT_16 i;

    SC_PutsAlways ("**********************************************\n\r");
	for (i = 0; i < 4; i++)
	{
		float duty = GetPwmOutDutyCycle (i);
		UINT_16 _integer = duty;
		UINT_16 _frac = (UINT_16)(duty * 100) % 100;
		// NOTE: sprintfs don't support floating point (%f). Tried w/o success in linking LARGE
		// formatting libraries which includes support for floating point 
		sprintf (str, "PWM Output %d Frequency = %d Hz; Duty Cycle = %d.%d \n\r", i, GetPwmOutFreq (i), _integer, _frac);
		SC_PutsAlways (str);
	}
	SC_PutsAlways ("**********************************************\n\r");
    SC_PutsAlways ("\n\r");

}


//--------------------------------------------------------------------------
// Module:
//  DebugAnalogInput
//
///   This function displays relevant Analog Input information to the
///   user.
///
///
//--------------------------------------------------------------------------
static void DebugAnalogInput (void)
{
	UINT_16 i; 
	UINT_16 adcValue;
	CHAR str[50];

    SC_PutsAlways ("\n\r");
    SC_PutsAlways ("**********************************************\n\r");

	for (i = 0; i < NUM_ANALOGINS; i++)
	{
		adcValue = ReadCurrentAnalogInput (i);
		sprintf (str, "Analog Input %d ADC value = %d\n\r", i, adcValue);
		SC_PutsAlways (str);
	}

    SC_PutsAlways ("**********************************************\n\r");
    SC_PutsAlways ("\n\r");

}


//--------------------------------------------------------------------------
// Module:
//  DebugAnalogOutput
//
///   This function displays relevant Analog Output information to the
///   user.
///
///
//--------------------------------------------------------------------------
static void DebugAnalogOutput (void)
{
	UINT_16 i; 
	UINT_16 dacValue;
	CHAR str[50];

    SC_PutsAlways ("\n\r");
    SC_PutsAlways ("**********************************************\n\r");

	for (i = 0; i < NUM_ALOGOUTS; i++)
	{
		dacValue = ReadCurrentAnalogOutput (i);
		sprintf (str, "Analog Output %d DAC value = %d\n\r", i, dacValue);
		SC_PutsAlways (str);
	}

    SC_PutsAlways ("**********************************************\n\r");
    SC_PutsAlways ("\n\r");

}

//--------------------------------------------------------------------------
// Module:
//  DebugHelp
//
///   This function displays relevant help information for the debug port
///
//--------------------------------------------------------------------------
static void DebugHelp (void)
{
	SC_PutsAlways ("\n\r\n\r");
	SC_PutsAlways ("**************************************************************************\n\r");
	SC_PutsAlways ("'a' - Analog Input Debugging Enable / Disable\n\r");
	SC_PutsAlways ("'d' - Digital Input Debugging Enable / Disable\n\r");
	SC_PutsAlways ("'p' - PWM Input Debugging Enable / Disable\n\r");
	SC_PutsAlways ("'w' - MVB Input Watchdog Debugging Enable / Disable\n\r");
	SC_PutsAlways ("'A' - Analog Output Debugging Enable / Disable\n\r");
	SC_PutsAlways ("'D' - Digital Output Debugging Enable / Disable\n\r");
	SC_PutsAlways ("'P' - PWM Output Debugging Enable / Disable\n\r");
	SC_PutsAlways ("'W' - MVB Output Watchdog Debugging Enable / Disable\n\r");
	SC_PutsAlways ("**************************************************************************\n\r");
	SC_PutsAlways ("'v' or 'V' - Display Application S/W CRC and version\n\r");
	SC_PutsAlways ("'b' or 'B' - Enter BURNIN mode... power cycle required to exit this mode\n\r");
	SC_PutsAlways ("'t' - Decrease sample time by 100 ms\n\r");
	SC_PutsAlways ("'T' - Increase sample time by 100 ms\n\r");
	SC_PutsAlways ("**************************************************************************\n\r");
	SC_PutsAlways ("                 Hit <SPACE BAR> to continue debugging\n\r");
	SC_PutsAlways ("**************************************************************************\n\r");

	// Flush any received chars
	SC_FlushStream (RX_STREAM);
	// Wait here until space bar is hit
	while (SC_GetChar () != ' ')
	{}

	SC_PutsAlways ("\n\r\n\r");

}

//--------------------------------------------------------------------------
// Module:
//  DebugUpdateMinPwmPulseWidth
//
///   This function sets and displays the current PWM input minimum pulse
///   width
///
//--------------------------------------------------------------------------
static void DebugUpdateMinPwmPulseWidth (BOOLEAN aIncreaseTime)
{
	CHAR str[50];
	UINT_16 minPulseWidthTime;

	SetMinStablePulseWidth (aIncreaseTime);

    SC_PutsAlways ("\n\r");
    SC_PutsAlways ("**********************************************\n\r");

	minPulseWidthTime = GetMinStablePulseWidth ();
	sprintf (str, "Minimum stable PWM in pulse width time =  %d\n\r", minPulseWidthTime);
	SC_PutsAlways (str);

    SC_PutsAlways ("**********************************************\n\r");
    SC_PutsAlways ("\n\r");

}

//--------------------------------------------------------------------------
// Module:
//  DebugResetPwmInDiags
//
///   This function resets the min PWM In period / pulse width diagnostic
///   info
///
//--------------------------------------------------------------------------
static void DebugResetPwmInDiags (void)
{
    SC_PutsAlways ("\n\r");
    SC_PutsAlways ("**********************************************\n\r");
    SC_PutsAlways ("PWM Input diagnostics was reset\n\r");
    SC_PutsAlways ("**********************************************\n\r");
    SC_PutsAlways ("\n\r");

	PwmInResetDiagnostics();
}