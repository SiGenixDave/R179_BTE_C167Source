/*****************************************************************************
*
* Copyright (C) 2010 Bombardier
*
* File Name: SerComm.h
*
* Abstract: Header file for the serial communications driver
*
* Revision History:
*   08/31/2009 - das - Created
*
******************************************************************************/

#ifndef SERCOMM_H
#define SERCOMM_H

/*--------------------------------------------------------------------------
                             GLOBAL CONSTANTS
  --------------------------------------------------------------------------*/
#ifndef _DEBUG
#ifdef EOF
#undef EOF
#endif
#define EOF							-1
#endif
/*--------------------------------------------------------------------------
                               GLOBAL TYPES
  --------------------------------------------------------------------------*/
typedef enum
{
    RX_STREAM,
    TX_STREAM
} eStreamType;

typedef enum
{
    BAUD_9600,
    BAUD_19200,
    BAUD_38400
} eBaudRate;

typedef enum
{
    SPACE_AVAILABLE,
    SPACE_OCCUPIED
} eBufferStatus;


/*--------------------------------------------------------------------------
                              GLOBAL VARIABLES
  --------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
                              GLOBAL PROTOTYPES
  --------------------------------------------------------------------------*/
EXPORT void SC_Init(eBaudRate aBaud, UINT_16 aTxPriority, UINT_16 aRxPriority);
EXPORT CHAR *SC_Gets(CHAR *aBuffer, UINT_16 aBufferSize);
EXPORT UINT_16 SC_GetChar(void);
EXPORT UINT_16 SC_GetChars(CHAR *aBuffer, UINT_16 aMaxAmount);
EXPORT CHAR *SC_Puts(const CHAR *aBuffer);
EXPORT void SC_PutsAlways(const CHAR *aBuffer);
EXPORT BOOLEAN SC_PutChar(CHAR aChar);
EXPORT void SC_PutCharAlways(CHAR aChar);
EXPORT CHAR *SC_PutChars(CHAR *aBuffer, UINT_16 aSize);
EXPORT UINT_16 SC_BufferStatus(eStreamType aStream, eBufferStatus aStatusType);
EXPORT void SC_FlushStream(eStreamType aStream);
EXPORT void SC_FlushAllStreams(void);


/* _DEBUG is defined when performing unit tests in Visual Studio 2008 */
#ifdef _DEBUG
EXPORT void SC_RxIsr(void);
EXPORT void SC_TxIsr(void);
EXPORT void SC_SimulateReceivingChar(CHAR aChar);
#endif

#endif
