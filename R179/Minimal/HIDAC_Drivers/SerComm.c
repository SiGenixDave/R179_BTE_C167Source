/*****************************************************************************
*
* Copyright (C) 2010 Bombardier
*
* File Name: SerComm.c
*
* Abstract: Functions 
*
* Revision History:
*   08/31/2009 - das - Created
*
******************************************************************************/
///   \file
///   This file contains functions that support the serial communications. 
///   All functions assume the serial reception and transmission is interrupt 
///   driven.

#define sercomm_C

/*--------------------------------------------------------------------------
                              INCLUDE FILES
  --------------------------------------------------------------------------*/
/* _DEBUG is defined in Microsoft Visual Studio 2008: UNIT TEST ONLY */
#ifndef _DEBUG
    #include <c166.h>
    #include <reg167.h>
#endif

#include <string.h>
#include <stdio.h>
#include "Types.h"
#include "SerComm.h"

/*--------------------------------------------------------------------------
                             MODULE CONSTANTS
  --------------------------------------------------------------------------*/
#ifdef _DEBUG
    #define SERCOMM_TX_SIZE		        20
    #define SERCOMM_RX_SIZE			    20    
#else
    #define SERCOMM_TX_SIZE		      2500    /* User can change this */
    #define SERCOMM_RX_SIZE			    80    /* User can change this */
/* Note: Without going into a complicated explanation, the true size of
   each buffer is 1 CHAR less than specified i each #define. This avoids
   the code checking flags for each buffer (comparing head and tail) */
#endif

#define END_OF_STRING_DELIMITER     '\n'  /* User can change this */

/* User defines this (i.e. removes comments around it) whenever it is 
   desired to remove both a carriage return ('\r' - 0x0d) and linefeed
   ('\n' - 0x0a) from incoming strings. */
/* #define REMOVE_CR   */


#ifdef _DEBUG
static UINT_8 tx;
static UINT_8 rx;
    #define TX_REGISTER             tx
    #define RX_REGISTER             rx
#else
    #define TX_REGISTER				S0TBUF
    #define RX_REGISTER				S0RBUF
#endif

#define  ASC0_Transmit              0x2A
#define  ASC0_Receive               0x2B

/*--------------------------------------------------------------------------
                              MODULE MACROS
  --------------------------------------------------------------------------*/
#ifdef _DEBUG
    #define DISABLE_RX_INTERRUPT()
    #define ENABLE_RX_INTERRUPT()
    #define DISABLE_TX_INTERRUPT()
    #define ENABLE_TX_INTERRUPT()
#else
/* It is highly desirable under most situations to enable or disable
   interrupts specific to the serial port (S0TIC, S0RIC) as opposed
   to the global interrupt (IEN). However, when the ASC0 interrupts are
   disabled via S0RIC and/or S0TIC, it prevents the respective IR flag from
   being raised by the hardware if the TX buffer becomes empty or an RX
   character is present, even after the interrupt is reenabled (not just a
   mask of the interrupt). Ergo, RX characters can be lost and TX streams
   of data may halt (empirical evidence) during the short period of time
   the interrupt is disabled.  See C167 manual (page 5-7) for description
   of xxIE and xxIR. Therefore, a minimum time is spent in this module
   having interrupts disabled. */
    #define DISABLE_RX_INTERRUPT()	DISABLE_ALL_INTERRUPTS()
    #define ENABLE_RX_INTERRUPT()	ENABLE_ALL_INTERRUPTS()
    #define DISABLE_TX_INTERRUPT()  DISABLE_ALL_INTERRUPTS()
    #define ENABLE_TX_INTERRUPT()	ENABLE_ALL_INTERRUPTS()
#endif


/*--------------------------------------------------------------------------
                             MODULE DATA TYPES
  --------------------------------------------------------------------------*/
typedef struct
{
    UINT_16 rxHead;     /* RX ISR increments after char received */
    UINT_16 txHead;     /* increments when application places a char(s) in txBuffer */
    UINT_16 rxTail;     /* increments after char extracted from rxBuffer */
    UINT_16 txTail;     /* TX ISR increments after transmitting a char */
    UINT_8  rxBuffer[SERCOMM_RX_SIZE];  /* Receive buffer */
    UINT_8  txBuffer[SERCOMM_TX_SIZE];  /* Transmit buffer */
    BOOLEAN txBufferTransmitting;       /* Used to trigger a transmit when 
                                           transmitter is idle (no chars being
                                           transmitted when chars have been written
                                           to txBuffer[] by application s/w */
} SerComm_t;


/*--------------------------------------------------------------------------
                              MODULE VARIABLES
  --------------------------------------------------------------------------*/
static SerComm_t port;

/*--------------------------------------------------------------------------
                             MODULE PROTOTYPES
  --------------------------------------------------------------------------*/
static void SC_TxChar(void);
static void SC_TxCharISR(void);
static void SC_RxChar(void);
static void SC_QueueTxStream(CHAR *aBuffer, UINT_16 aSize);

//--------------------------------------------------------------------------
// Module:
//  SC_Init
//
///   This function initializes the C167's ASC0 port It sets the port 
///   with 8 bits, no parity, and the desired baud rate. Interrupts are 
///   used to receive and buffer incoming characters and to transmit 
///   outgoing character(s). If either priority parameter is 0, the 
///   respective interrupt is disabled.
///
///   \param aBaud - the baud rate of the ASC0 port  
///   \param aTxPriority - the priority of the ASC0 transmit interrupt  
///   \param aRxPriority - the priority of the ASC0 receive interrupt  
///                    
//--------------------------------------------------------------------------
void SC_Init(eBaudRate aBaud, UINT_16 aTxPriority, UINT_16 aRxPriority)
{
    /* _DEBUG is defined when performing unit tests in Visual Studio */
    /* Configure P3.10 to use the alternate function TxD */
    P3 = 0x0400;
    DP3 = 0x0400;

    /* ASC0 Control Register */
    S0CON = 0x8011;

    /* Set the baud rate (default is 19200) */
    switch (aBaud)
    {
        case BAUD_38400:
            S0BG = 0x0020;
            break;

        case BAUD_19200:
        default:
            S0BG = 0x0040;
            break;

		case BAUD_9600:
            S0BG = 0x0080;
            break;

	}

    /* Enable Receive Interrupt */
    if (aRxPriority)
    {
        /* Enable Transmit Buffer Interrupt */
        S0RIC = 0x0040 | aRxPriority;
    }

    if (aTxPriority)
    {
        /* Enable Transmit Buffer Interrupt */
        S0TIC = 0x0040 | aTxPriority;
    }
} 


//--------------------------------------------------------------------------
// Module:
//  SC_Gets
//
///   This function is used to gather user input via the ASC0 port. 
///   It first determines how many characters are present in the rxBuffer. 
///   If no characters are present NULL is immediately returned. It then 
///   scans the characters present and searches for a END_OF_STRING_DELIMITER. 
///   If not found, NULL is returned. If a END_OF_STRING_DELIMITER is found, 
///   then aBufferSize is compared to the string size to determine if the 
///   entire string can be copied into aBuffer. If it can't, NULL is returned. 
///   Otherwise, the string is copied into aBuffer and the 
///   END_OF_STRING_DELIMITER is replaced with NULL. 
///   Note: Any received character is always echoed back since it is a "gets()"
///         type function.
///
///   \param aBuffer - buffer where the string is to be copied; must be maintained 
///                    through function calls 
///   \param aBufferSize - the maximum string size that aBuffer can handle
///
///   \return CHAR * - aBuffer if string present and was copied into aBuffer; 
///                    NULL otherwise 
///                    
//--------------------------------------------------------------------------
CHAR *SC_Gets(CHAR *aBuffer, UINT_16 aBufferSize)
{
    UINT_16 charsInRxBuffer = SC_BufferStatus(RX_STREAM, SPACE_OCCUPIED);

    UINT_16 index = 0;

    static UINT_16 aBufferIndex = 0;  /* Maintains where the last character 
                                         was inserted */

    BOOLEAN endOfLineFound = FALSE;

    /* Receive buffer is empty */
    if (charsInRxBuffer == 0)
    {
        return NULL;
    }

    /* Search for the newline */
    while (index < charsInRxBuffer)
    {
        aBuffer[aBufferIndex] = (CHAR)SC_GetChar();
        /* Echo the received character */
        SC_PutChar(aBuffer[aBufferIndex]);

        if (aBuffer[aBufferIndex] == END_OF_STRING_DELIMITER)
        {
            endOfLineFound = TRUE;
            break;
        }
        aBufferIndex++;
        index++;
    }

    /* Couldn't find a line terminator or line is too long */
    if (!endOfLineFound || index > aBufferSize)
    {
        return NULL;
    }


    /* Terminate the string with a NULL (0) */
    aBuffer[aBufferIndex] = 0x0000;

#ifdef REMOVE_CR
    if (aBuffer[aBufferIndex - 1] == '\r')
    {
        aBuffer[aBufferIndex] = 0x0000;
    }
#endif

    aBufferIndex = 0;
    return aBuffer;

}


//--------------------------------------------------------------------------
// Module:
//  SC_GetChar
//
///   Returns the next character in the serial port receive buffer.
///
///   \return UINT_16 - the next character from the buffer or EOF
//--------------------------------------------------------------------------
UINT_16 SC_GetChar(void)
{
    UINT_16 c;

    /* Is a character available */
    if (port.rxHead != port.rxTail)
    {
        /* Get the character and increment the tail*/
        c = port.rxBuffer[port.rxTail++];
        if (port.rxTail >= SERCOMM_RX_SIZE)
        {
            port.rxTail = 0;
        }
    }
    /* No character available */
    else
    {
        c = EOF;
    }

    return c;
}

//--------------------------------------------------------------------------
// Module:
//  SC_GetChars
//
///   This function determines if there are characters in the receive buffer. 
///   If there are, it copies the smaller of either 
///      1) the aMaxAmount of 
///      2)	the actual amount of characters in the receive buffer.
/// 
///	  \param aBuffer - the buffer where the received CHARs will be copied
///   \param aMaxAmount -  the maximum amount of characters to be extracted 
/// 
///   \return UINT_16 - the actual amount of received characters extracted
//--------------------------------------------------------------------------
UINT_16 SC_GetChars(CHAR *aBuffer, UINT_16 aMaxAmount)
{
    /* Get the number of available chars */
    UINT_16 rxCharsAvail = SC_BufferStatus(RX_STREAM, SPACE_OCCUPIED);

    UINT_16 index = 0;

    /* No characters available */
    if (rxCharsAvail == 0)
    {
        return 0;
    }

    /* Extract the smaller amount of either the requested amount or
       the actual amount of characters in the receive buffer */
    while (index < aMaxAmount && index < rxCharsAvail)
    {
        aBuffer[index] = (CHAR)SC_GetChar();
        index++;
    }

    /* Return the amount of characters extracted */
    return index;

}

//--------------------------------------------------------------------------
// Module:
//  SC_Puts
//
///   This function outputs the string pointed to by aBuffer. It fisrt verifies
///   that the string can be queued to the txBuffer by determining the length of 
///   the string and determining the amount of CHARs available in the txBuffer. 
///   If the string can't be queued, NULL is immediately returned. Otherwise,
///   the string is queued to txBuffer with SC_QueueTxStream().
///
///   \param aBuffer - pointer to the string
/// 
///   \return CHAR * - aBuffer if string was queued to txBuffer; NULL otherwise 
/// 
//--------------------------------------------------------------------------
CHAR *SC_Puts(const CHAR *aBuffer)
{
    UINT_16 index = 0;

    /* Verify there is enough room in the tx buffer to place the data by
       first determining the string length. */
    UINT_16 strSize = strlen((const CHAR *)aBuffer);

    /* Get Available space in the txBuffer */
    UINT_16 spaceInBuffer = SC_BufferStatus(TX_STREAM, SPACE_AVAILABLE);

    /* String won't fit */
    if (strSize > spaceInBuffer)
    {
        return NULL;
    }

    /* String can be queued. Queue it and return aBuffer */
    SC_QueueTxStream((CHAR *)aBuffer, strSize);
    return (CHAR *)aBuffer;
}

//--------------------------------------------------------------------------
// Module:
//  SC_Puts
//
///   This function outputs the string pointed to by aBuffer. The function
///   does not return until the SC_Puts can copy the entire string into
///   the serial transmit buffer.
///
///   \param aBuffer - pointer to the string
/// 
//--------------------------------------------------------------------------
void SC_PutsAlways(const CHAR *aBuffer)
{
	while (!SC_Puts(aBuffer));
}


//--------------------------------------------------------------------------
// Module:
//  SC_PutChar
//
///   This function attempts to queue aChar to the txBuffer. It first determines
///   if the txBuffer is not full. If it is, FALSE is immediately returned.
///   Otherwise, aChar is queued to txBuffer.
///
///   \param aChar - the CHAR to be placed in the txBuffer
/// 
///   \return BOOLEAN - TRUE if aChar placed in txBuffer; FALSE otherwise 
/// 
//--------------------------------------------------------------------------
BOOLEAN SC_PutChar(CHAR aChar)
{
    /* Get Available space */
    UINT_16 spaceInBuffer = SC_BufferStatus(TX_STREAM, SPACE_AVAILABLE);

    if (!spaceInBuffer)
    {
        return FALSE;
    }
    /* CHAR can be queued. Queue it and return aBuffer */
    SC_QueueTxStream(&aChar , 1);
    return TRUE;
}

//--------------------------------------------------------------------------
// Module:
//  SC_PutChar
//
///   This function places aChar in the txBuffer queue. The function does
///   not return until the character is successfully copied to the 
///   serial transmit buffer queue.
///
///   \param aChar - the CHAR to be placed in the txBuffer
/// 
/// 
//--------------------------------------------------------------------------
void SC_PutCharAlways(CHAR aChar)
{
	while (!SC_PutChar(aChar));
}


//--------------------------------------------------------------------------
// Module:
//  SC_PutChars
//
///   This function attempts to queue all CHARs pointed to be aBuffer into 
///   txBuffer. The amount of CHARs to be queued is determined by aSize. It 
///   first determines if txBuffer is capable of storing the amount of CHARs
///   specified by aSize. If it is, FALSE is immediately returned.
///   Otherwise, aChar is queued to txBuffer.
///
///   \param aBuffer - pointer to CHARs to be queued in txBuffer
///   \param aSize - the amount of CHARs to be queued in txBuffer
/// 
///   \return CHAR * - aBuffer if all CHARs were queued to txBuffer; FALSE
///                    otherwise 
/// 
//--------------------------------------------------------------------------
CHAR *SC_PutChars(CHAR *aBuffer, UINT_16 aSize)
{
    UINT_16 index = 0;

    /* Get Available space */
    UINT_16 spaceInBuffer = SC_BufferStatus(TX_STREAM, SPACE_AVAILABLE);

    /* Verify there is enough room in the tx buffer to place the data */
    if (aSize > spaceInBuffer)
    {
        return NULL;
    }

    /* String can be queued. Queue it and return aBuffer */
    SC_QueueTxStream(aBuffer,aSize);
    return aBuffer;

}

//--------------------------------------------------------------------------
// Module:
//  SC_BufferStatus
//
///   This function determines the amount of available CHARS in the specified
///   stream or the amount of occupied space.  This function is called when
///   queuing CHARs to the transmit stream and when polling if any CHARs are 
///   available in the receive stream.
///
///   \param aStream - either the transmit or receive stream
///   \param aStatusType - either AVAILABLE or OCCUPIED
/// 
///   \return UINT_16 - amount of space available or the amount of space 
///                     occupied in the specified stream 
///                     
//--------------------------------------------------------------------------
UINT_16 SC_BufferStatus(eStreamType aStream, eBufferStatus aStatusType)
{
    UINT_16 head;
    UINT_16 tail;
    UINT_16 bufferSize;

    /* Set all of the parameters based on the stream being polled. */
    if (aStream == RX_STREAM)
    {
        DISABLE_RX_INTERRUPT();
        head = port.rxHead;
        tail = port.rxTail;
        bufferSize = SERCOMM_RX_SIZE;
        ENABLE_RX_INTERRUPT();
    }
    else
    {
        DISABLE_TX_INTERRUPT();
        head = port.txHead;
        tail = port.txTail;
        bufferSize = SERCOMM_TX_SIZE;
        ENABLE_TX_INTERRUPT();
    }

    if (aStatusType == SPACE_AVAILABLE)
    {
        /* Always subtract 1 from the result to avoid overwriting data
           in the stream. */
        if (head >= tail)
        {
            return bufferSize - (head - tail) - 1;
        }
        else
        {
            return tail - head - 1;
        }
    }
    else
    {
        if (head >= tail)
        {
            return(head - tail);
        }
        else
        {
            return bufferSize + head - tail;
        }
    }

}


//--------------------------------------------------------------------------
// Module:
//  SC_FlushStream
//
///   This function flushes all CHARs from the specified stream. It does so
///   by setting the head equal to the tail.
///
///   \param aStream - either the transmit or receive stream
/// 
//--------------------------------------------------------------------------
void SC_FlushStream(eStreamType aStream)
{
    if (aStream == RX_STREAM)
    {
        DISABLE_RX_INTERRUPT();
        port.rxHead = port.rxTail;
        ENABLE_RX_INTERRUPT();
    }
    else
    {
        DISABLE_TX_INTERRUPT();
        port.txHead = port.txTail;
        port.txBufferTransmitting = FALSE;
        ENABLE_TX_INTERRUPT();
    }

}

//--------------------------------------------------------------------------
// Module:
//  SC_FlushAllStreams
//
///   This function flushes both the RX_STREAM and TX_STREAM.
///
//--------------------------------------------------------------------------
void SC_FlushAllStreams(void)
{
    SC_FlushStream(RX_STREAM);
    SC_FlushStream(TX_STREAM);
}

void SC_Test(void)
{

}

//--------------------------------------------------------------------------
// Module:
//  SC_TxIsr
//
///   This function is an interrupt service routine and services the serial
///   transmit register becoming empty.
///
//--------------------------------------------------------------------------
#ifndef _DEBUG
interrupt (ASC0_Transmit) using (ASC0_TX_RB) void SC_TxIsr(void)
#else
void SC_TxIsr(void)
#endif
{
    SC_TxCharISR();
}

//--------------------------------------------------------------------------
// Module:
//  SC_RxIsr
//
///   This function is an interrupt service routine and services the serial
///   receive register receiving a character.
///
//--------------------------------------------------------------------------
#ifndef _DEBUG
interrupt (ASC0_Receive) using (ASC0_RX_RB) void SC_RxIsr(void)
#else
void SC_RxIsr(void)
#endif
{
    SC_RxChar();
}


//--------------------------------------------------------------------------
// Module:
//  SC_RxChar
//
///   This function is responsible for reading the serial port receive
///   register and queue the received character into rxBuffer. It will
///   do so as long as rxBuffer is not full.
///
//--------------------------------------------------------------------------
static void SC_RxChar(void)
{
    /* Create a copy of Head. This is necessary so as to queue the
       received character into the first available position in rxBuffer. */
    UINT_16 rxHead = port.rxHead + 1;
    CHAR rxReg;

    if (rxHead >= SERCOMM_RX_SIZE)
    {
        rxHead = 0;
    }

    /* Always read the register just in case a read is required to clear
       the interrupt. */
    rxReg = RX_REGISTER;

    /* Store the CHAR if the buffer isn't full. */
    if (rxHead != port.rxTail)
    {
        port.rxBuffer[port.rxHead] = rxReg;
        /* Set the new head (where the next received CHAR will be queued.*/
        port.rxHead = rxHead;
    }
}


//--------------------------------------------------------------------------
// Module:
//  SC_TxChar
//
///   This function is responsible for taking the next available character
///   in txBuffer and writing it to the serial transmit register. It first 
///   determines if any CHARs are available in txBuffer. If so, it writes
///   the next CHAR to the serial transmit register, sets txBufferTransmitting,
///   and adjusts the tail. If no CHARs are available, txBufferTransmitting
///   is reset.
///
//--------------------------------------------------------------------------
static void SC_TxChar(void)
{
    port.txBufferTransmitting = TRUE;
    /* Update the serial transmit register */
    TX_REGISTER = port.txBuffer[port.txTail++];
#ifdef _DEBUG
    putchar(TX_REGISTER);
#endif
    /* Adjust the tail */
    if (port.txTail >= SERCOMM_TX_SIZE)
    {
        port.txTail = 0;
    }

}


//--------------------------------------------------------------------------
// Module:
//  SC_TxCharISR
//
///   This function whenever the asynchronous serial transmit buffer 
///   interrupt fires. It determines if there are any more characters 
///   in the software transmit buffer. If so, it writes the next character
///   to the ASC0 transmit register. If none are available, it does nothing
///   except reset a software flag which indicates the asynchronous transmit
///   port is idle (not transmitting anything).
///
//--------------------------------------------------------------------------
static void SC_TxCharISR(void)
{
    /* Is a CHAR available to be sent. */
    if (port.txHead != port.txTail)
    {
        /* Update the serial transmit register */
        TX_REGISTER = port.txBuffer[port.txTail++];
#ifdef _DEBUG
        putchar(TX_REGISTER);
#endif

        /* Adjust the tail */
        if (port.txTail >= SERCOMM_TX_SIZE)
        {
            port.txTail = 0;
        }
    }
    else
    {
        /* No CHARs available */
        port.txBufferTransmitting = FALSE;
    }

}


//--------------------------------------------------------------------------
// Module:
//  SC_QueueTxStream
//
///   This function is responsible queuing CHARs pointed to be aBuffer
///   into txBuffer. A precondition to calling this function is to check
///   if there is enough available space in the txBuffer to queue aBuffer.
///   If this check is not made, existing CHARs in txBuffer will be overwritten
///   and txBuffer will be corrupted. After all CHARs from aBuffer have been
///   queued, trigger the transmission of data if none is currently in progress.
/// 
///   \param aBuffer - pointer the array of CHARs to be queued
///   \param aSize - the amount of CHARs from aBuffer to be queued
///
//--------------------------------------------------------------------------
static void SC_QueueTxStream(CHAR *aBuffer, UINT_16 aSize)
{
    UINT_16 index = 0;


	/* Queue all CHARs from aBuffer into txBuffer. */
    while (index < aSize)
    {
        DISABLE_TX_INTERRUPT();
        port.txBuffer[port.txHead++] = aBuffer[index];
        if (port.txHead >= SERCOMM_TX_SIZE)
        {
            port.txHead = 0;
        }
        ENABLE_TX_INTERRUPT();
        index++;
    }

    DISABLE_TX_INTERRUPT();
    /* If the serial transmitter register is idle (i.e. no preexisting CHARs in
       txBuffer prior to this function call), dequeue the first character and
       send it. */
    if (!port.txBufferTransmitting)
    {
        SC_TxChar();
    }
    ENABLE_TX_INTERRUPT();



}

#ifdef _DEBUG
void SC_SimulateReceivingChar(CHAR aChar)
{
    rx = aChar;
    SC_RxIsr();
}
#endif

