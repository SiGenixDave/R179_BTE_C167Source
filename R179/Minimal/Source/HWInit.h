/*****************************************************************************
*
* Copyright (C) 2014-15 Bombardier
*
* File Name: HWInit.h
*
* Abstract: Header file for the hardware initialization
*
* Revision History:
*   08/31/2014 - das - Created
*
******************************************************************************/
#ifndef HWINIT_H
#define HWINIT_H

/*--------------------------------------------------------------------------
                             GLOBAL CONSTANTS
  --------------------------------------------------------------------------*/
#define PLUS_15_BAD_MASK	0x01
#define MINUS_15_BAD_MASK	0x02

/*--------------------------------------------------------------------------
                               GLOBAL TYPES
  --------------------------------------------------------------------------*/
typedef enum
{
	TASK_MVB_DEAD

} eDisableCPUWDog;

/*--------------------------------------------------------------------------
                              GLOBAL VARIABLES
  --------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
                              GLOBAL PROTOTYPES
  --------------------------------------------------------------------------*/
void Init15VoltFailDetect (void);
void InitSerialCommunications (void);
void InitAnalogInputs (void);
void InitAnalogOutputs (void);
void InitDigitalOutputs (void);
void InitPwmOutputs (void);
void InitTimer0 (void);
void StartTimer0 (void);
void StopTimer0 (void);
void InitTimer1 (void);
void InitTimer7 (void);
void DisableSystemInterrupts (void);
void EnableSystemInterrupts (void);

void ToggleCPUWatchdog (void);
void ToggleVDriveWatchdog (void);
void InitP8ForDigOuts (void);
UINT_8 Get15VoltBad (void);
void Set15VoltBad (UINT_8 aReason);
void SetVdriveDisable (BOOLEAN aDisable);
void SetCPUDisable (eDisableCPUWDog aReason);
void ServiceVDriveWatchdog (void);
void ServiceCPUWatchdog (void);
void SetFifteenVoltBad (UINT_8 aReason);
void SetStartupSuccessful (BOOLEAN aValue);
BOOLEAN GetStartupSuccessful (void);

#endif