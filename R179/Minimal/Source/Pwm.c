/*****************************************************************************
*
* Copyright (C) 2014-15 Bombardier
*
* File Name: Pwm.c
*
* Revision History:
*   08/22/2014 - das - Created
*
******************************************************************************/
///   \file
///   This file contains functions that process PWM I/O

/*--------------------------------------------------------------------------
                              INCLUDE FILES
  --------------------------------------------------------------------------*/
#ifndef _DEBUG
#include <reg167.h>
#endif
#include "Types.h"
#include "mvbvar.h"
#include "PwmIn.h"
#include "PwmOut.h" 
#include "Pwm.h"

#include "VEC_TBL.h"

/*--------------------------------------------------------------------------
                             MODULE CONSTANTS
  --------------------------------------------------------------------------*/
#define NUM_PWM_INPUTS							1

#define ENABLE_CAPCOM_REGISTER_10_INTERRUPT		0x60
#define DISABLE_CAPCOM_REGISTER_10_INTERRUPT	0x00

#define MAX_FILTER_COUNT						100

/*--------------------------------------------------------------------------
                              MODULE MACROS
  --------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------
                             MODULE DATA TYPES
  --------------------------------------------------------------------------*/
typedef struct
{
	UINT_16 period;						// most recent period read from HiDAC CPLD
	UINT_16 pulseWidth;					// most recent pulse width read from HiDAC CPLD
	UINT_16 status;						// most recent status read from HiDAC CPLD
	UINT_16 interruptCounter;			// maintains the amount of PWM input interrupts
	UINT_16 prevInterruptCounter;		// used for comparison against interruptCounter
	UINT_16 noInterruptRxCounter;		// amount of consecutive calls where no ISR occurring
	BOOLEAN deadPWM;					// TRUE if PWM input ISR not occurring
	UINT_16 maxWidth;					// maintain the max pulse width (diagnostic only)
	UINT_16 minWidth;					// maintain the min pulse width (diagnostic only)
	UINT_16 maxPeriod;					// maintain the max period (diagnostic only)
	UINT_16 minPeriod;					// maintain the min period (diagnostic only)

} PwmIn_t;

//////////////////////////////////////////////////////////////////////////
////////////////////////////// UNUSED ////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
typedef enum
{
	VALID_SIGNAL_LENGTH,				// Initial state & min PWM component width exceeded
	INVALID_SIGNAL_LENGTH,				// min PWM component width is less than expected but hasn't time out
	INVALID_SIGNAL_LENGTH_MAX_EXCEEDED  // min PWM component width exceeded for too many consecutive samples 
} ePwmFilter;

typedef struct
{
	UINT_16 filterCounter;				// maintains consecutive "bad" samples
	UINT_16 currentWidth;				// maintains the current "good" width
	ePwmFilter filterState;				// maintains state machine
	const UINT_16 MIN_WIDTH;			// width needs to exceed this time (in usec) to be considered "good"
} PwmFilter_t;
//////////////////////////////////////////////////////////////////////////

/*--------------------------------------------------------------------------
                              MODULE VARIABLES
  --------------------------------------------------------------------------*/
static PwmIn_t m_PwmInputInfo[NUM_PWM_INPUTS];
static PwmFilter_t m_PulseWidth = {0, 0, VALID_SIGNAL_LENGTH, 20};
static PwmFilter_t m_Period = {0, 0, VALID_SIGNAL_LENGTH, 7600};
static UINT_16 minStablePulseWidth = 16;

/*--------------------------------------------------------------------------
                             MODULE PROTOTYPES
  --------------------------------------------------------------------------*/
static UINT_16 MvbPwmOutToDutyCycle (UINT_16 aMvbValue);
static BOOLEAN PwmInProcess (ePwmInId aId, UINT_16 aMaxDeadCount, UINT_16 aMinPeriod);

//////////////////////////////////////////////////////////////////////////
////////////////////////////// UNUSED ////////////////////////////////////
//////////////////////////////////////////////////////////////////////////
static UINT_16 PWMIn_Filter (UINT_16 aCurrentMeasurement, PwmFilter_t *aPWMComponent);
//////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------
// Module:
//  PwmInInit
//
///   This function initializes the HiDAC registers and module variables 
///   that allow PWM input operation.
///
//--------------------------------------------------------------------------
void PwmInInit (void)
{
	UINT_16 i;

	// CC10 capture on falling edge at pin P2.10 (PWM Rx1)
	CCM2 = 0x0200;

	PwmInEnableISR();
	
	for (i = 0; i < NUM_PWM_INPUTS; i++)
	{
		m_PwmInputInfo[i].period = 0;
		m_PwmInputInfo[i].pulseWidth = 0;
		m_PwmInputInfo[i].status = 0;
		m_PwmInputInfo[i].interruptCounter = 0;
		m_PwmInputInfo[i].prevInterruptCounter = 0;
		m_PwmInputInfo[i].noInterruptRxCounter = 0;
		m_PwmInputInfo[i].deadPWM = TRUE;
		m_PwmInputInfo[i].maxWidth = 0;
		m_PwmInputInfo[i].minWidth = 65535;
		m_PwmInputInfo[i].maxPeriod = 0;
		m_PwmInputInfo[i].minPeriod = 65535;
	}

}

//--------------------------------------------------------------------------
// Module:
//  PwmInEnableISR
//
///   This function enables the PWM input ISR
///
//--------------------------------------------------------------------------
void PwmInEnableISR (void)
{
	// Enable PWM_1 input interrupt P2.10 from DONE1* 
	CC10IC = ENABLE_CAPCOM_REGISTER_10_INTERRUPT;
}

//--------------------------------------------------------------------------
// Module:
//  PwmInDisableISR
//
///   This function disables the PWM input ISR
///
//--------------------------------------------------------------------------
void PwmInDisableISR (void)
{
	// Enable PWM_1 input interrupt P2.10 from DONE1* 
	CC10IC = DISABLE_CAPCOM_REGISTER_10_INTERRUPT;
}


//--------------------------------------------------------------------------
// Module:
//  ReadPwmInputUpdateMvb
//
///   This function reads the PWM input to determine the latest duty
///   cycle and period. If the PWM is functioning properly, the required MVB 
///   variables are updated. If not, then the appropriate error bit is set indicating 
///   either potential noise or a dead PWM.
///
///
//--------------------------------------------------------------------------
void ReadPwmInputUpdateMvb (void)
{
    BOOLEAN deadPwm;
    UINT_16 status;
	UINT_16 period;
	UINT_16 pulseWidth;


    // The CPLD has been changed to filter out noise, especially around the edges. Therefore,
	// the 3rd argument to this function is now the minimum consecutive time (usecs) that the signal 
	// has to be stable in order for the PWM input to be considered either high or low. all filtering 
	// has been removed
    deadPwm = PwmInProcess (PWMIN_0, 5, minStablePulseWidth);

    if (deadPwm == FALSE)
    {
		period = GetPwmPeriod (PWMIN_0) & 0x7FFF; 
		pulseWidth = GetPwmPulseWidth (PWMIN_0) & 0x7FFF; 

		// Update MVB with values read after the most recent interrupt
        HDVCU_650_64_UINT16 = period;
        HDVCU_650_80_UINT16 = pulseWidth;
    }
    else
    {
        // Indicate that the PWM input either stuck high or low
        HDVCU_650_64_UINT16 = 0x8000;        
		// Get the current state of the PWM and set / reset the upper bit
		PWMI_GetStatus (PWMIN_0, &status);
        if (status & 0x0001)
        {
            // Indicate that the minimum period of the PWM input is not being exceeded (possible noise on signal)
            HDVCU_650_80_UINT16 = 0x8000;
        }
        else
        {
            // Indicate that the minimum period of the PWM input is not being exceeded (possible noise on signal)
            HDVCU_650_80_UINT16 = 0x0000;
        }

    }

}

//--------------------------------------------------------------------------
// Module:
//  SetMinStablePulseWidth
//
///   This function is a mutator for the minimum stable pulse width.
///   This variable is used by the CPLD to filter out noise, especially
///   at or near signal transitions. 
///
///   \param BOOLEAN - TRUE to increase the minimum stable pulse width by 1
///                    usec; FALSE to decrease by 1 usec.
///
//--------------------------------------------------------------------------
void SetMinStablePulseWidth (BOOLEAN aIncrement)
{
	if (aIncrement == TRUE)
	{
		// The CPLD register is only 8 bits so don't allow the value to increase above
		// 255 (0xFF)
		if (minStablePulseWidth < 255)
		{
			minStablePulseWidth++;
		}
	}
	else
	{
		// According to Tom Schneider, the CPLD register must be 1 or greater in order for the filter 
		// to operate properly.
		if (minStablePulseWidth > 1)
		{
			minStablePulseWidth--;
		}
	}
}

//--------------------------------------------------------------------------
// Module:
//  GetMinStablePulseWidth
//
///   This function is an accessor for the minimum stable pulse width.
///   This variable is used by the CPLD to filter out noise, especially
///   at or near signal transitions. 
///
///   \returns UINT_16 - minimum stable pulse width used by CPLD
///
//--------------------------------------------------------------------------
UINT_16 GetMinStablePulseWidth (void)
{
	return minStablePulseWidth;
}

//--------------------------------------------------------------------------
// Module:
//  ReadMvbUpdatePwmOutputs
//
///   This function updates all PWM outputs from the values in the 
///   MVB variables. It will set the outputs to default/safe states
///   when MVB communication is down
///
///   \param aCommunicationOk - TRUE if MVB communication OK; FALSE otherwise
///
//--------------------------------------------------------------------------
void ReadMvbUpdatePwmOutputs (BOOLEAN aCommunicationOk)
{
    if (aCommunicationOk == TRUE)
    {
        PWMO_SetPwmOutput (PWMOUT_0,  VCUHD_658_48_UINT16,   MvbPwmOutToDutyCycle (VCUHD_658_64_UINT16));
        PWMO_SetPwmOutput (PWMOUT_1,  VCUHD_658_80_UINT16,   MvbPwmOutToDutyCycle (VCUHD_658_96_UINT16));
        PWMO_SetPwmOutput (PWMOUT_2,  VCUHD_658_112_UINT16,  MvbPwmOutToDutyCycle (VCUHD_658_128_UINT16));
        PWMO_SetPwmOutput (PWMOUT_3,  VCUHD_658_144_UINT16,  MvbPwmOutToDutyCycle (VCUHD_658_160_UINT16));
    }
    else
    {
        PWMO_SetEnable (PWMOUT_0, FALSE);
        PWMO_SetEnable (PWMOUT_1, FALSE);
        PWMO_SetEnable (PWMOUT_2, FALSE);
        PWMO_SetEnable (PWMOUT_3, FALSE);
    }
}

//--------------------------------------------------------------------------
// Module:
//  GetPwmOutFreq
//
///   This function reads the module variable that indicates 
///   the PWM Output frequency (Hz)
///
///   \param id - PWM output id
///
///   \returns UINT_16 - commanded PWM output frequency (Hz) 
///
//--------------------------------------------------------------------------
UINT_16 GetPwmOutFreq (UINT_16 id)
{
	UINT_16 val = 0xffff;
	const UINT_16 *freq[] = 
	{
		&VCUHD_658_48_UINT16,
		&VCUHD_658_80_UINT16,
		&VCUHD_658_112_UINT16,
		&VCUHD_658_144_UINT16
	};
	
	if (id < NUM_PWMOUT)
	{
		val = *freq[id];
	}

	return val;
}

//--------------------------------------------------------------------------
// Module:
//  GetPwmOutDutyCycle
//
///   This function reads the module variable that indicates 
///   the status of the selected PWM input. 
///
///   \param id - PWM output id
///
///   \returns float - commanded duty cycle (%) 
///
//--------------------------------------------------------------------------
float GetPwmOutDutyCycle (UINT_16 id)
{
	float val = -1.0;

	const UINT_16 *dutyCycle[] = 
	{
		&VCUHD_658_64_UINT16,
		&VCUHD_658_96_UINT16,
		&VCUHD_658_128_UINT16,
		&VCUHD_658_160_UINT16,
	};
	
	// Convert the duty cycle to a float. The value returned from
	// the function is the duty cycle (%) * 100. (e.g. 50% is 5000)
	if (id < NUM_PWMOUT)
	{
		// NOTE: in order to accommodate the polarity inversion needed
		// for the PWM output drive signal (due to the opto-isolator), 
		// the MvbPwmOutToDutyCycle function returns 10000 - the requested 
		// duty cycle, so the returned value is subtracted from 10000 
		// to show the actual duty cycle
		val = (10000.0 - MvbPwmOutToDutyCycle (*dutyCycle[id])) / 100.0;
	}

	return val;

}


//--------------------------------------------------------------------------
// Module:
//  GetPwmInStatus
//
///   This function reads the module variable that indicates 
///   the status of the selected PWM input. 
///
///   \param aId - PWM input id
///
///   \returns UINT_16 - PWM input status
///
//--------------------------------------------------------------------------
UINT_16 GetPwmInStatus (ePwmInId aId)
{
	return m_PwmInputInfo[aId].status;
}

//--------------------------------------------------------------------------
// Module:
//  GetPwmPeriod
//
///   This function reads the module variable that indicates 
///   the period (in usecs) of the selected PWM input. 
///
///   \param aId - PWM input id
///
///   \returns UINT_16 - PWM input period
///
//--------------------------------------------------------------------------
UINT_16 GetPwmPeriod (ePwmInId aId)
{
	return m_PwmInputInfo[aId].period;
}


//--------------------------------------------------------------------------
// Module:
//  GetPwmMinPeriod
//
///   This function reads the module variable that indicates 
///   the minimum period (in usecs) detected of the selected PWM input. 
///
///   \param aId - PWM input id
///
///   \returns UINT_16 - minimum PWM input period
///
//--------------------------------------------------------------------------
UINT_16 GetPwmMinPeriod (ePwmInId aId)
{
	return m_PwmInputInfo[aId].minPeriod;
}

//--------------------------------------------------------------------------
// Module:
//  GetPwmMaxPeriod
//
///   This function reads the module variable that indicates 
///   the maximum period (in usecs) detected of the selected PWM input. 
///
///   \param aId - PWM input id
///
///   \returns UINT_16 - maximum PWM input period
///
//--------------------------------------------------------------------------
UINT_16 GetPwmMaxPeriod (ePwmInId aId)
{
	return m_PwmInputInfo[aId].maxPeriod;
}



//--------------------------------------------------------------------------
// Module:
//  GetPwmPulseWidth
//
///   This function reads the module variable that indicates 
///   the pulse width (in usecs) of the selected PWM input. 
///
///   \param aId - PWM input id
///
///   \returns UINT_16 - PWM input pulse width
///
//--------------------------------------------------------------------------
UINT_16 GetPwmPulseWidth (ePwmInId aId)
{
	return m_PwmInputInfo[aId].pulseWidth;
}


//--------------------------------------------------------------------------
// Module:
//  GetPwmMinPulseWidth
//
///   This function reads the module variable that indicates 
///   the minimum pulse width (in usecs) of the selected PWM input. 
///
///   \param aId - PWM input id
///
///   \returns UINT_16 - minimum PWM input pulse width
///
//--------------------------------------------------------------------------
UINT_16 GetPwmMinPulseWidth (ePwmInId aId)
{
	return m_PwmInputInfo[aId].minWidth;
}

//--------------------------------------------------------------------------
// Module:
//  GetPwmMaxPulseWidth
//
///   This function reads the module variable that indicates 
///   the maximum pulse width (in usecs) of the selected PWM input. 
///
///   \param aId - PWM input id
///
///   \returns UINT_16 - maximum PWM input pulse width
///
//--------------------------------------------------------------------------
UINT_16 GetPwmMaxPulseWidth (ePwmInId aId)
{
	return m_PwmInputInfo[aId].maxWidth;
}



//--------------------------------------------------------------------------
// Module:
//  GetPwmInIsrCount
//
///   This function reads the module variable that indicates 
///   the amount of interrupt occurrences caused by the PWM input signal. 
///
///   \param aId - PWM input id
///
///   \returns UINT_16 - PWM input interrupt service count
///
//--------------------------------------------------------------------------
UINT_16 GetPwmInIsrCount (ePwmInId aId)
{
	return m_PwmInputInfo[aId].interruptCounter;
}

//--------------------------------------------------------------------------
// Module:
//  PWMInputIsr1
//
///   This function is invoked when the PWM Input interrupt (DONE1*) occurs. 
///
///
//--------------------------------------------------------------------------
void PWMInputIsr1 (void)
{
	UINT_16 period;
	UINT_16 pulseWidth;

	// Given the current PWM CPLD implementation, this code is used to filter out noise
	// spikes that occur after either edge (rising of falling) of the PWM signal. This is seen
	// in the power lab during EMI testing.
	PWMI_GetPeriod (PWMIN_0, &period);
	m_PwmInputInfo[0].period = period;

	PWMI_GetPulseWidth (PWMIN_0, &pulseWidth);
	m_PwmInputInfo[0].pulseWidth = pulseWidth;

	PWMI_GetStatus (PWMIN_0, &m_PwmInputInfo[0].status);

	// Measure the min and max period/width
	////////////////////////////////////////////////////////////////
	if (period > m_PwmInputInfo[0].maxPeriod)
	{
		m_PwmInputInfo[0].maxPeriod = period;
	}
	if (period < m_PwmInputInfo[0].minPeriod)
	{
		m_PwmInputInfo[0].minPeriod = period;
	}
	////////////////////////////////////////////////////////////////
	if (pulseWidth > m_PwmInputInfo[0].maxWidth)
	{
		m_PwmInputInfo[0].maxWidth = pulseWidth;
	}
	if (pulseWidth < m_PwmInputInfo[0].minWidth)
	{
		m_PwmInputInfo[0].minWidth = pulseWidth;
	}
	////////////////////////////////////////////////////////////////

	m_PwmInputInfo[0].interruptCounter++;
}

//--------------------------------------------------------------------------
// Module:
//  PwmInResetDiagnostics
//
///   This function resets the PWM input diagnostic info.
///
//--------------------------------------------------------------------------
void PwmInResetDiagnostics (void)
{
	m_PwmInputInfo[0].maxPeriod = 0;
	m_PwmInputInfo[0].minPeriod = 65535;
	m_PwmInputInfo[0].maxWidth = 0;
	m_PwmInputInfo[0].minWidth = 65535;
}

//--------------------------------------------------------------------------
// Module:
//  MvbPwmOutToDutyCycle
//
///   This function is used to convert the desired duty cycle from an
///   MVB variable to a duty cycle used by the PWM output driver (duty cycle
///   * 10000). For example, 32768 will convert to 50% or 0.5 * 10000 = 
///   5000.
///
///   \param aMvbValue - 0 = 0%; 65535 = 100%; linear between
///
///   \returns UINT_16 - value between 10000 and 0
///
//--------------------------------------------------------------------------
static UINT_16 MvbPwmOutToDutyCycle (UINT_16 aMvbValue)
{
    /* MVB Variable ranges from 0 (0%) to 65535 (100%) */
    float dutyCycle;

    /* Convert value to a percentage */
    dutyCycle = (65535.0 - aMvbValue) / 65535.0;

    /* Multiply by 10000 which means 50% duty cycle will be 5000 */
    dutyCycle *= 10000.0;

    return (UINT_16) (dutyCycle);

}

//--------------------------------------------------------------------------
// Module:
//  PwmInProcess
//
///   This function processes the desired PWM input. It ensures the PWM interrupt
///   is occurring at the expected rate. 
///
///   \param aId - PWM input id
///   \param aMaxDeadCount - maximum amount of successive calls to this function
///                          where no change is 
///   \param aMinPulseWidth - the minimum width that a PWM input must have in order 
///							  for the signal to recognized as stable
///
///   \returns BOOLEAN - TRUE if the PWM input is deemed dead; FALSE indicates all is well
///
//--------------------------------------------------------------------------
static BOOLEAN PwmInProcess (ePwmInId aId, UINT_16 aMaxDeadCount, UINT_16 aMinPulseWidth)
{

	PWMI_SetMinPulseWidth (aId, aMinPulseWidth);

	// Determine if the PWM Input interrupt is occurring
	if (m_PwmInputInfo[aId].prevInterruptCounter == m_PwmInputInfo[aId].interruptCounter)
	{
		// No interrupt detected (no change in the interrupt counter)
		if (m_PwmInputInfo[aId].noInterruptRxCounter < aMaxDeadCount)
		{
			m_PwmInputInfo[aId].noInterruptRxCounter++;
			// has the max dead count been exceeded
			if (m_PwmInputInfo[aId].noInterruptRxCounter >= aMaxDeadCount)
			{
				m_PwmInputInfo[aId].deadPWM = TRUE;
			}
		}
	}
	else
	{
		// Reset all error variables since the interrupt is occurring
		m_PwmInputInfo[aId].deadPWM = FALSE;
		m_PwmInputInfo[aId].noInterruptRxCounter = 0;
	}


	// save the interrupt counter for the next function call
	m_PwmInputInfo[aId].prevInterruptCounter = m_PwmInputInfo[aId].interruptCounter;
	
	// return the PWM input status
	return m_PwmInputInfo[aId].deadPWM;

}
//////////////////////////////////////////////////////////////////////////
////////////////////////////// UNUSED ////////////////////////////////////
//////////////////////////////////////////////////////////////////////////

//--------------------------------------------------------------------------
// Module:
//  PWMIn_Filter
//
///   This function is used to filter glitches on the PWM input signal. It is
///   used for both pulse width and period. This feature was generated in response 
///   to spikes induced on the PWM signal due to the slow fall time of the PWM
///   signal presented to the HiDAC.
///
///   \param aCurrentMeasurement - either pulse width or period
///   \param aPWMComponent - pointer to structure that maintains pulse width / period info
///
///   \returns UINT_16 - current pulse width or period if it exceeds the minimum 
///                      length, most recent "good" reading if required length sis
///                      not exceeded, or "bad" value if bad values persist.
///
//--------------------------------------------------------------------------
static UINT_16 PWMIn_Filter (UINT_16 aCurrentMeasurement, PwmFilter_t *aPWMComponent)
{
	switch (aPWMComponent->filterState)
	{
		case VALID_SIGNAL_LENGTH:
		default:
			if (aCurrentMeasurement > aPWMComponent->MIN_WIDTH)
			{
				// All is well; minimum exceeded
				aPWMComponent->currentWidth = aCurrentMeasurement;
			}
			else
			{
				// spike detected; increment filter count and return the 
				// most recent good value (i.e. don't update aPWMComponent->currentWidth)
				(aPWMComponent->filterCounter)++;
				aPWMComponent->filterState = INVALID_SIGNAL_LENGTH;
			}
			break;

		case INVALID_SIGNAL_LENGTH:
			if (aCurrentMeasurement > aPWMComponent->MIN_WIDTH)
			{
				// All is well; minimum exceeded, reset the filter count and return to the good state
				aPWMComponent->currentWidth = aCurrentMeasurement;
				aPWMComponent->filterCounter = 0;
				aPWMComponent->filterState = VALID_SIGNAL_LENGTH;
			}
			else
			{
				// All isn't well, determine if the max consecutive samples of bad values
				// has been exceeded
				(aPWMComponent->filterCounter)++;
				if (aPWMComponent->filterCounter >= MAX_FILTER_COUNT)
				{
					// Max consecutive bad samples have been exceeded, start returning the 
					// bad values
					aPWMComponent->filterState = INVALID_SIGNAL_LENGTH_MAX_EXCEEDED;
					aPWMComponent->filterCounter = 0;
					aPWMComponent->currentWidth = aCurrentMeasurement;
				}
				else
				{
					// INTENTIONALLY DO NOTHING
					// and don't update aPWMComponent->currentWidth (still return the most recent good sample
				}

			}
			break;

		case INVALID_SIGNAL_LENGTH_MAX_EXCEEDED:
			if (aCurrentMeasurement > aPWMComponent->MIN_WIDTH)
			{
				// all is well with the signal again
				aPWMComponent->filterState = VALID_SIGNAL_LENGTH;
			}
			aPWMComponent->currentWidth = aCurrentMeasurement;
			break;

	}

	return aPWMComponent->currentWidth;

}

//////////////////////////////////////////////////////////////////////////

