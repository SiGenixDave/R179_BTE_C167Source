/*****************************************************************************
*
* Copyright (C) 2014-2015 Bombardier
*
* File Name: Digital.h
*
* Abstract: Header file for the Digital I/O
*
* Revision History:
*   12/31/2014 - das - Created
*
******************************************************************************/
#ifndef DIGITAL_H
#define DIGITAL_H

#include "DigOut.h"

/*--------------------------------------------------------------------------
                             GLOBAL CONSTANTS
  --------------------------------------------------------------------------*/
#define HIDAC_FAIL_LED_ON()		DO_ResetBitWithId(FAIL_LED)
#define HIDAC_FAIL_LED_OFF()	DO_SetBitWithId(FAIL_LED)

#define HIDAC_TEST_LED_ON()		DO_ResetBitWithId(TEST_LED)
#define HIDAC_TEST_LED_OFF()	DO_SetBitWithId(TEST_LED)

/*--------------------------------------------------------------------------
                               GLOBAL TYPES
  --------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
                              GLOBAL VARIABLES
  --------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
                              GLOBAL PROTOTYPES
  --------------------------------------------------------------------------*/
void ReadDigitalInputsUpdateMvb (void);
void ReadMvbUpdateDigitalOutputs (BOOLEAN aCommunicationOk);

#endif