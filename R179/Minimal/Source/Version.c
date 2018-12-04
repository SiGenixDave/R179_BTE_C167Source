/******************************************************************************
 *
 * Copyright (C) 2018-19 Bombardier
 *
 * File Name: RTC.c
 *
 * Revision History:
 *   12/03/2018 - das - Created
 *
 ******************************************************************************/
#include <string.h>

#include "Types.h"
#include "CmdProc.h"
#include "SerComm.h"

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

/***************************************************************************
 *
 * Description: Reports the current software version to the user
 *
 *
 * Parameters:  cmdPtr - array with the command and parameters
 *
 * Returns:     BOOLEAN - TRUE always
 *
 ***************************************************************************/
BOOLEAN VersionUpdate (char cmdPtr[][MAX_PARAM_LENGTH])
{
    char response[50];
    const char *softwareVersion = "1.0";

    strcpy (response, "<");
    strcat (response, cmdPtr[0]);
    strcat (response, ",");
    strcat (response, softwareVersion);
    strcat (response, ">");

    SC_PutsAlways (response);
    return (TRUE);
}
