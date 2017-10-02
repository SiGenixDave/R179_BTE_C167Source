/******************************************************************************
*
* Copyright (C) 2014-15 Bombardier
*
* File Name: HexLED.c
*
* Revision History:
*   08/31/2014 - das - Created
*
******************************************************************************/
///   \file
///   This file contains the code that updates the Hex LED display with the
///   desired value.

#define hexled_C

/*--------------------------------------------------------------------------
                              INCLUDE FILES
  --------------------------------------------------------------------------*/

#include "Types.h"
#include "DigOut.h"

/*--------------------------------------------------------------------------
                             MODULE CONSTANTS
  --------------------------------------------------------------------------*/

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
//  HexLEDUpdate
//
///   This function updates the Hex LED display with the desired value
///
///   \param aValue - the desired value to be displayed
///
//--------------------------------------------------------------------------
void HexLEDUpdate (UINT_16 aValue)
{
    UINT_16 index;

    /* The order is important: based on HIDAC hardware interface to the
       Hex LED */
	const eDigOutBitId displayBit[] = {DISP_09, DISP_06, DISP_07, DISP_04};

    /* Enable Display */
    DO_ResetBitWithId (DISP_05);
    DO_ResetBitWithId (DISP_08);

    /* Set LS Nibble Latch low */
    DO_ResetBitWithId (DISP_10);
    for (index = 0; index < 4; index++)
    {
        if (aValue & 1)
        {
            DO_SetBitWithId (displayBit[index]);
        }
        else
        {
            DO_ResetBitWithId (displayBit[index]);
        }
        aValue >>= 1;
    }
    /* Set LS Nibble Latch High */
    DO_SetBitWithId (DISP_10);


    DO_ResetBitWithId (DISP_03);
    for (index = 0; index < 4; index++)
    {
        if (aValue & 1)
        {
            DO_SetBitWithId (displayBit[index]);
        }
        else
        {
            DO_ResetBitWithId (displayBit[index]);
        }
        aValue >>= 1;
    }
    DO_SetBitWithId (DISP_03);

}
