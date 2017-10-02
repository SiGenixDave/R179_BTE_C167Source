/*****************************************************************************
*
* Copyright (C) 2014-15 Bombardier
*
* File Name: Pwm.h
*
* Abstract: Header file for the PWM I/O
*
* Revision History:
*   08/31/2014 - das - Created
*
******************************************************************************/

#ifndef _PWMPROC_H_
#define _PWMPROC_H_

#include "PwmIn.h"

/*--------------------------------------------------------------------------
                             GLOBAL CONSTANTS
  --------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
                               GLOBAL TYPES
  --------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
                              GLOBAL VARIABLES
  --------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
                              GLOBAL PROTOTYPES
  --------------------------------------------------------------------------*/

void PwmInInit (void);
void PwmInEnableISR (void);
void PwmInDisableISR (void);
void PwmInResetDiagnostics (void);
void ReadPwmInputUpdateMvb (void);
void SetMinStablePulseWidth (BOOLEAN aIncrement);
UINT_16 GetMinStablePulseWidth (void);
void ReadMvbUpdatePwmOutputs (BOOLEAN aCommunicationOk);
UINT_16 GetPwmOutFreq (UINT_16 id);
float GetPwmOutDutyCycle (UINT_16 id);
UINT_16 GetPwmInStatus (ePwmInId aId);
UINT_16 GetPwmPeriod (ePwmInId aId);
UINT_16 GetPwmMinPeriod (ePwmInId aId);
UINT_16 GetPwmMaxPeriod (ePwmInId aId);
UINT_16 GetPwmMinPulseWidth (ePwmInId aId);
UINT_16 GetPwmMaxPulseWidth (ePwmInId aId);
UINT_16 GetPwmPulseWidth (ePwmInId aId);
UINT_16 GetPwmInIsrCount (ePwmInId aId);
void PWMInputIsr1 (void);

#endif