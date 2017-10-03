/*****************************************************************************
*
* Copyright (C) 2010 Bombardier
*
* File Name: PwmIn.h
*
* Abstract: Header file for the digital input driver
*
* Revision History:
*   03/18/2014 - steeriks - Created
*
******************************************************************************/

#ifndef PWMIN_H
#define PWMIN_H

/*--------------------------------------------------------------------------
                             GLOBAL CONSTANTS
  --------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------
                               GLOBAL TYPES
  --------------------------------------------------------------------------*/
typedef enum
{
    PWMIN_0,	/* PWM Input 0 : P1-D32 */
    PWMIN_1,	/* PWM Input 1 : P1-Z32 */
    NUM_PWMIN
} ePwmInId;


/*--------------------------------------------------------------------------
                              GLOBAL VARIABLES
  --------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
                              GLOBAL PROTOTYPES
  --------------------------------------------------------------------------*/
EXPORT BOOLEAN PWMI_SetMinPulseWidth (ePwmInId aId, UINT_16 aMinPulsePeriod);
EXPORT BOOLEAN PWMI_GetStatus (ePwmInId aId, UINT_16 *aStatus);
EXPORT BOOLEAN PWMI_GetPeriod (ePwmInId aId, UINT_16 *aPeriod); 
EXPORT BOOLEAN PWMI_GetPulseWidth (ePwmInId aId, UINT_16 *aPulseWidth); 

#endif
