/*****************************************************************************
 *
 * Copyright (C) 2018-19 Bombardier
 *
 * File Name: CmdProc.h
 *
 * Abstract: Header file for the Command Processor
 *
 * Revision History:
 *   12/03/2018 - das - Created
 *
 ******************************************************************************/

#ifndef CMDPROC_H_
#define CMDPROC_H_

/*--------------------------------------------------------------------------
 GLOBAL CONSTANTS
 --------------------------------------------------------------------------*/
#define MAX_PARAM_LENGTH    10

/*--------------------------------------------------------------------------
 GLOBAL TYPES
 --------------------------------------------------------------------------*/
typedef enum
{
    BIT_WIDTH_8, BIT_WIDTH_16, BIT_WIDTH_24, BIT_WIDTH_32
} eDataWidth;

/*--------------------------------------------------------------------------
 GLOBAL VARIABLES
 --------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 GLOBAL PROTOTYPES
 --------------------------------------------------------------------------*/

void ApplicationService (void);
void ResetStateMachine (void);
BOOLEAN HexStringToValue (char *ptr, UINT_32 *value);
void SendAddressDataResponse (const char *str, UINT_32 address, UINT_32 data, eDataWidth dataWidth);
void SendTestPassed (const char *str, UINT_32 expectedValue, eDataWidth dataWidth);
void SendMismatchError (const char *str, UINT_32 expectedValue, UINT_32 actualValue, eDataWidth dataWidth);

#endif /* CMDPROC_H_ */
