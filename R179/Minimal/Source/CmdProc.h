/*
 * CmdProc.h
 *
 *  Created on: Nov 21, 2018
 *      Author: David Smail
 */

#ifndef CMDPROC_H_
#define CMDPROC_H_

#define MAX_PARAM_LENGTH    10

typedef enum
{
    BIT_WIDTH_8, BIT_WIDTH_16, BIT_WIDTH_32,
} eDataWidth;

void ApplicationService (void);
void ResetStateMachine (void);
BOOLEAN HexStringToValue (char *ptr, UINT_32 *data);
void ProcessSerialInputChar (char ch);
void SendTestPassed (const char *str, UINT_32 expectedValue, eDataWidth dataWidth);
void SendMismatchError (const char *str, UINT_32 expectedValue, UINT_32 actualValue, eDataWidth dataWidth);

#endif /* CMDPROC_H_ */
