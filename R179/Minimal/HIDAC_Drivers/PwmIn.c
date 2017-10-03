/*****************************************************************************
*
* Copyright (C) 2010 Bombardier
*
* File Name: PwmIn.c
*
* Revision History:
*   03/18/2014 - steeriks - Created
*
******************************************************************************/
///   \file
///   This file contains functions that read from the PWM input channels

/*--------------------------------------------------------------------------
                              INCLUDE FILES
  --------------------------------------------------------------------------*/
/* _DEBUG is defined in Microsoft Visual Studio 2008: UNIT TEST ONLY */
#ifndef _DEBUG
#include <c166.h>
#include <reg167.h>
#endif
#include "Types.h"
#include "PwmIn.h"

/*--------------------------------------------------------------------------
                             MODULE CONSTANTS
  --------------------------------------------------------------------------*/

// Read only
#define   PWMIN_PULSE_PERIOD_0			(*(UINT_16 *)(0x500012))
// Read only
#define   PWMIN_PULSE_WIDTH_0			(*(UINT_16 *)(0x50000A))
// Write only
#define   PWMIN_MIN_PULSE_WIDTH_0		(*(UINT_16 *)(0x500016))
// Read only
#define   PWMIN_MIN_STATUS_REG_0		(*(UINT_16 *)(0x500016))

// Read only
#define   PWMIN_PULSE_PERIOD_1			(*(UINT_16 *)(0x500014))
// Read only
#define   PWMIN_PULSE_WIDTH_1			(*(UINT_16 *)(0x50000C))
// Write only
#define   PWMIN_MIN_PULSE_WIDTH_1		(*(UINT_16 *)(0x500018))
// Read only
#define   PWMIN_MIN_STATUS_REG_1		(*(UINT_16 *)(0x500018))


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

//--------------------------------------------------------------------------
// Module:
//   PWMI_SetMinPulseWidth
//
///   This function sets the minimum pulse period (1 quanta = 1 usec) for the
///   selected PWM. The PWM period must exceed the minimum pulse period
///   for an interrupt to be generated to the CPU.
///
///	  \param aId - the id of the PWM input channel
///	  \param aEnable - the minimum pulse period
///
///   \returns BOOLEAN - TRUE if valid PWM input specified
///
//--------------------------------------------------------------------------
BOOLEAN PWMI_SetMinPulseWidth (ePwmInId aId, UINT_16 aMinPulsePeriod)
{
    BOOLEAN validId = FALSE;

    switch (aId)
    {
        case PWMIN_0:
            PWMIN_MIN_PULSE_WIDTH_0 = aMinPulsePeriod;
            validId = TRUE;
            break;

        case PWMIN_1:
            PWMIN_MIN_PULSE_WIDTH_1 = aMinPulsePeriod;
            validId = TRUE;
            break;

        default:
            validId = FALSE;
            break;

    }

    return validId;

}

//--------------------------------------------------------------------------
// Module:
//   PWMI_GetStatus
//
///   This function is an accessor to the status for the selected PWM input
///   channel.
///
///	  \param aId - the id of the PWM input channel
///   \param aStatus - pointer to a variable that will hold the PWM in status
///
///   \returns BOOLEAN - TRUE if valid PWM input specified
//
//--------------------------------------------------------------------------
BOOLEAN PWMI_GetStatus (ePwmInId aId, UINT_16 *aStatus)
{
    BOOLEAN validId = FALSE;

    switch (aId)
    {
        case PWMIN_0:
            *aStatus = PWMIN_MIN_STATUS_REG_0;
            validId = TRUE;
            break;

        case PWMIN_1:
            *aStatus = PWMIN_MIN_STATUS_REG_1;
            validId = TRUE;
            break;

        default:
            validId = FALSE;
            break;

    }

    return validId;
}

//--------------------------------------------------------------------------
// Module:
//   PWMI_GetPeriod
//
///   This function is an accessor to the pulse period (1 quanta = 1 usec)
///   for the selected PWM input channel.
///
///	  \param aId - the id of the PWM input channel
///   \param aPeriod - pointer to a variable that will hold the PWM period
///
///   \returns BOOLEAN - TRUE if valid PWM input specified
//
//--------------------------------------------------------------------------
BOOLEAN PWMI_GetPeriod (ePwmInId aId, UINT_16 *aPeriod)
{
    BOOLEAN validId = FALSE;

    switch (aId)
    {
        case PWMIN_0:
            *aPeriod = PWMIN_PULSE_PERIOD_0;
            validId = TRUE;
            break;

        case PWMIN_1:
            *aPeriod = PWMIN_PULSE_PERIOD_1;
            validId = TRUE;
            break;

        default:
            validId = FALSE;
            break;

    }

    return validId;
}

//--------------------------------------------------------------------------
// Module:
//   PWMI_GetPulseWidth
//
///   This function is an accessor to the pulse width (1 quanta = 1 usec)
///   for the selected PWM input channel.
///
///	  \param aId - the id of the PWM input channel
///   \param aPulseWidth - pointer to a variable that will hold the PWM pulse width
///
///   \returns BOOLEAN - TRUE if valid PWM input specified
//
//--------------------------------------------------------------------------
BOOLEAN PWMI_GetPulseWidth (ePwmInId aId, UINT_16 *aPulseWidth)
{
    BOOLEAN validId = FALSE;

    switch (aId)
    {
        case PWMIN_0:
            *aPulseWidth = PWMIN_PULSE_WIDTH_0;
            validId = TRUE;
            break;

        case PWMIN_1:
            *aPulseWidth = PWMIN_PULSE_WIDTH_1;
            validId = TRUE;
            break;

        default:
            validId = FALSE;
            break;

    }

    return validId;
}
