/*****************************************************************************
*
* Copyright (C) 2014-15 Bombardier
*
* File Name: TaskMvb.h
*
* Abstract: Header file for the MVB Task
*
* Revision History:
*   08/31/2014 - das - Created
*
******************************************************************************/

#ifndef _TASKMVB_H_
#define _TASKMVB_H_

/*--------------------------------------------------------------------------
                             GLOBAL CONSTANTS
  --------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
                               GLOBAL TYPES
  --------------------------------------------------------------------------*/
typedef enum
{
	RAM_FAILURE = 0x01,
	ROM_FAILURE = 0x02,
	BAD_15V = 0x04

} HidacError_e;

/*--------------------------------------------------------------------------
                              GLOBAL VARIABLES
  --------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
                              GLOBAL PROTOTYPES
  --------------------------------------------------------------------------*/
void Task8ms (void);
void SetMvbReady (BOOLEAN aMvbReady);
void UpdateHidacError (HidacError_e aErrorMask);
BOOLEAN HidacErrorExists (void);
BOOLEAN MonitorMVBInterrupt (UINT_16 maxDeadCount);

#endif 

