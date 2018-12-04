/******************************************************************************
 *
 * Copyright (C) 2018-19 Bombardier
 *
 * File Name: CmdProc.c
 *
 * Revision History:
 *   12/03/2018 - das - Created
 *
 ******************************************************************************/

/*--------------------------------------------------------------------------
 INCLUDE FILES
 --------------------------------------------------------------------------*/
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>

#include "Types.h"
#include "CmdProc.h"
#include "SerComm.h"
#include "Version.h"
#include "Flash.h"
#include "Ram.h"
#include "NVRam.h"
#include "RTC.h"
#include "PeekPoke.h"

/*--------------------------------------------------------------------------
 MODULE CONSTANTS
 --------------------------------------------------------------------------*/
#define MAX_PARAMS          5
#define MAX_CMD_SIZE        55

/*--------------------------------------------------------------------------
 MODULE MACROS
 --------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 MODULE DATA TYPES
 --------------------------------------------------------------------------*/
typedef enum
{
    WAIT_FOR_OPEN_BRACE, WAIT_FOR_ENTIRE_CMD
} SerialInputState;

typedef BOOLEAN (*CmdUpdateFnPtr) (char cmdPtr[][MAX_PARAM_LENGTH]);
typedef void (*ServiceFnPtr) (const char *str);

typedef struct
{
    const char *str; /* Command from User */
    const CmdUpdateFnPtr cmdUpdateFnPtr; /* Function called after command received */
    const ServiceFnPtr serviceFnPtr; /* Function called always to support commands */
} CmdUpdate;

/*--------------------------------------------------------------------------
 MODULE VARIABLES
 --------------------------------------------------------------------------*/
const CmdUpdate m_CmdUpdate[] = {
    { "VER", VersionUpdate, NULL }, { "FLA", FlashTableUpdate, FlashService }, { "RAM", RamTableUpdate, RamService }, {
        "NVR", NVRamTableUpdate, NVRamService }, { "RTC", RTCTableUpdate, RTCService }, {
        "PES", PeekSingle, PeekSingleService }, { "POS", PokeSingle, PokeSingleService }, {
        "PEC", PeekContinuous, PeekContinuousService }, { "POC", PokeContinuous, PokeContinuousService }, {
        "PEK", PeekContinuousKill, NULL }, { "POK", PokeContinuousKill, NULL }, };

SerialInputState m_SerialParsingtate;
UINT_16 m_CmdIndex;

static const UINT_16 NUM_VALID_COMMANDS = sizeof(m_CmdUpdate) / sizeof(CmdUpdate);

/* Stores the command and parameters, <> not included but commas are */
static char m_CmdString[MAX_CMD_SIZE];

/*--------------------------------------------------------------------------
 MODULE PROTOTYPES
 --------------------------------------------------------------------------*/
static void ProcessSerialInputChar (char ch);
static void SendInvalidCommandResponse (void);
static void ParseValidCommand (void);
static char *GetDataString (UINT_32 data, eDataWidth dataWidth);

/***************************************************************************
 *
 * Description: Function is called from the main loop. It continuously
 *              reads the serial port for new characters from the user
 *              and feeds any new characters into the parser. It also
 *              calls all of the device, peek and poke service functions.
 *
 *
 * Parameters:  None
 *
 * Returns:     None
 *
 ***************************************************************************/
void ApplicationService (void)
{
    UINT_16 index;
    INT_16 debugInChar;

    /* Read serial port to determine if any new characters available */
    debugInChar = SC_GetChar ();

    if (debugInChar != EOF)
    {
        /* Valid char, send to parser */
        ProcessSerialInputChar ((char) debugInChar);
    }

    /* Service all of the device, and peek n poke functions */
    for (index = 0; index < NUM_VALID_COMMANDS; index++)
    {
        if (m_CmdUpdate[index].serviceFnPtr != NULL)
        {
            m_CmdUpdate[index].serviceFnPtr (m_CmdUpdate[index].str);
        }
    }
}

/***************************************************************************
 *
 * Description: Converts a hex string to a machine value
 *
 *
 * Parameters:  ptr - pointer to the hex string
 *              value - pointer to the variable where the value is stored
 *
 *
 * Returns:     BOOLEAN - TRUE if conversion successful; FALSE otherwise
 *
 ***************************************************************************/
BOOLEAN HexStringToValue (char *ptr, UINT_32 *value)
{

    UINT_32 number = 0;
    char *endptr = NULL;
    number = (UINT_32) strtol (ptr, &endptr, 16);

    /* Error detection info received from:
     * https://stackoverflow.com/questions/26080829/detecting-strtol-failure */
    if ((ptr == endptr) || (errno == ERANGE && number == LONG_MIN) || (errno == ERANGE && number == LONG_MAX)
                    || (errno == EINVAL) || (errno != 0 && number == 0))
    {
        return (FALSE);
    }

    *value = number;
    return (TRUE);
}

/***************************************************************************
 *
 * Description: Resets all information related to the input character
 *              parser state machine
 *
 *
 * Parameters:  None
 *
 *
 * Returns:     None
 *
 ***************************************************************************/
void ResetStateMachine (void)
{
    m_SerialParsingtate = WAIT_FOR_OPEN_BRACE;
    m_CmdIndex = 0;
    memset (m_CmdString, 0, sizeof(m_CmdString));
}

/***************************************************************************
 *
 * Description: Sends a test pass indication to the user. The format
 *              of a test pass is <device,PASS,expectedValue> where device
 *              is the device that was tested and expected value is the
 *              expected value for the test.
 *
 *
 * Parameters:  str - pointer to the command to be sent
 *              expectedValue - expected value sent (converted to ASCII)
 *              dataWidth - determines amount of hex characters sent in expectedValue
 *
 *
 * Returns:     None
 *
 ***************************************************************************/
void SendTestPassed (const char *str, UINT_32 expectedValue, eDataWidth dataWidth)
{
    char response[50];

    strcpy (response, "<");
    strcat (response, str);
    strcat (response, ",PASS,");

    strcat (response, GetDataString (expectedValue, dataWidth));
    strcat (response, ">");

    SC_PutsAlways (response);
}

/***************************************************************************
 *
 * Description: Sends a test fail indication to the user. The format
 *              of a test pass is <device,PASS,expectedValue,actualValue> where device
 *              is the device that was tested and expected value is the
 *              expected value for the test and actual value is the actual value
 *              for the test (i.e. expected doesn't equal actual)
 *
 *
 * Parameters:  str - pointer to the command to be sent
 *              expectedValue - expected value sent (converted to ASCII)
 *              actualValue - actual value sent (converted to ASCII)
 *              dataWidth - determines amount of hex characters sent in expectedValue & actualValue
 *
 *
 * Returns:     None
 *
 ***************************************************************************/
void SendMismatchError (const char *str, UINT_32 expectedValue, UINT_32 actualValue, eDataWidth dataWidth)
{
    char response[50];

    strcpy (response, "<");
    strcat (response, str);
    strcat (response, ",FAIL,");

    strcat (response, GetDataString (expectedValue, dataWidth));
    strcat (response, ",");
    strcat (response, GetDataString (actualValue, dataWidth));
    strcat (response, ">");

    SC_PutsAlways (response);
}

/***************************************************************************
 *
 * Description: Sends the address and data back to the user
 *
 *
 * Parameters:  str - pointer to the command to be sent
 *              address - 24 bit address (converted to ASCII)
 *              data - 16 or 8 bit data (converted to ASCII)
 *              dataWidth - determines amount of hex characters sent (data)
 *
 *
 * Returns:     None
 *
 ***************************************************************************/
void SendAddressDataResponse (const char *str, UINT_32 address, UINT_32 data, eDataWidth dataWidth)
{
    char response[50];

    strcpy (response, "<");
    strcat (response, str);
    strcat (response, ",");
    strcat (response, GetDataString (address, BIT_WIDTH_24));
    strcat (response, ",");
    strcat (response, GetDataString (data, dataWidth));
    strcat (response, ">");

    SC_PutsAlways (response);

}

/***************************************************************************
 *
 * Description: Parses the serial input stream from the user. Verifies,
 *              that a message begins with '<' and ends with '>'. WHen an
 *              entire message is received, the message is parsed and the
 *              desired action takes place either in the Update function
 *              call or the service function call.
 *
 *
 * Parameters:  ch - serial input character received from the user
 *
 *
 * Returns:     None
 *
 ***************************************************************************/
static void ProcessSerialInputChar (char ch)
{
    SC_PutChar (ch);

    switch (m_SerialParsingtate)
    {
        case WAIT_FOR_OPEN_BRACE:
        default:
            if (ch == '<')
            {
                m_SerialParsingtate = WAIT_FOR_ENTIRE_CMD;
            }
            else
            {
                m_CmdIndex = 0;
                ResetStateMachine ();
                SendInvalidCommandResponse ();
            }
            break;

        case WAIT_FOR_ENTIRE_CMD:
            /* Make sure the buffer can't be overflowed */
            if (m_CmdIndex >= MAX_CMD_SIZE)
            {
                SendInvalidCommandResponse ();
                ResetStateMachine ();
            }
            else if (ch == '>')
            {
                m_CmdString[m_CmdIndex] = '\0';
                ParseValidCommand ();
                ResetStateMachine ();
            }
            else
            {
                m_CmdString[m_CmdIndex] = toupper (ch);
                m_CmdIndex++;
            }
            break;
    }
}

/***************************************************************************
 *
 * Description: Sends a string that informs the user that invalid
 *              command was detected.
 *
 *
 * Parameters:  None
 *
 *
 * Returns:     None
 *
 ***************************************************************************/
static void SendInvalidCommandResponse (void)
{
    SC_Puts ("<INV>");
}

/***************************************************************************
 *
 * Description: Parses the entire string sent by the user and invokes
 *              the update function associated with the command
 *
 *
 * Parameters:  None
 *
 *
 * Returns:     None
 *
 ***************************************************************************/
static void ParseValidCommand (void)
{
    BOOLEAN valid = FALSE;

    UINT_16 cmd = 0;
    UINT_16 cmdIndex = 0;
    UINT_16 index = 0;

    static char parsedCommands[MAX_PARAMS][MAX_PARAM_LENGTH];

    /* Save all of the command parameter info */
    while (m_CmdString[index] != '\0')
    {
        /* Use the ',' as a delimiter and save the command
         * an associated parameters (if any) in subsequent parts
         * of the 2D character array
         */
        if (m_CmdString[index] == ',')
        {
            /* Comma seen, terminate the string  and move onto next parameter */
            parsedCommands[cmd][cmdIndex] = 0;
            cmdIndex = 0;
            cmd++;
            /* Too many commands/parameters, inform user */
            if (cmd >= MAX_PARAMS)
            {
                SendInvalidCommandResponse ();
                return;
            }
        }
        else
        {
            if (cmdIndex >= MAX_PARAM_LENGTH)
            {
                /* The individual command or parameter is too long, abort and
                 * inform the user
                 */
                SendInvalidCommandResponse ();
                return;
            }
            else
            {
                parsedCommands[cmd][cmdIndex] = m_CmdString[index];
                cmdIndex++;
            }
        }
        index++;
    }

    /* NULL Terminate the last command / parameter */
    parsedCommands[cmd][cmdIndex] = '\0';

    /* Get the on board device to change behavior */
    for (index = 0; index < NUM_VALID_COMMANDS; index++)
    {
        if (!strcmp (parsedCommands[0], m_CmdUpdate[index].str))
        {
            break;
        }
    }

    if (index >= NUM_VALID_COMMANDS)
    {
        SendInvalidCommandResponse ();
        return;
    }

    valid = m_CmdUpdate[index].cmdUpdateFnPtr (&parsedCommands[0]);

    if (!valid)
    {
        SendInvalidCommandResponse ();
    }
}

/***************************************************************************
 *
 * Description: Converts a variable's value to a HEX string. The amount of
 *              characters in the string are determined by the data width
 *              0's will pad numbers whose value doesn't occupy all
 *              characters.  This function was created because sprintf()
 *              on teh C167 only supports 2 argumnets.
 *
 *
 * Parameters:  data - value to be converted to hex string
 *              dataWidth - determines number of HEX characters and leading 0's
 *
 *
 * Returns:     char * - HEX string
 *
 ***************************************************************************/
static char *GetDataString (UINT_32 data, eDataWidth dataWidth)
{
    static char value[10];
    UINT_16 i;
    UINT_16 shift;

    /* Clear all of memory so that string will be NULL terminated */
    memset (value, 0, sizeof(value));

    switch (dataWidth)
    {
        case BIT_WIDTH_8:
            shift = 8;
            break;

        case BIT_WIDTH_16:
            shift = 16;
            break;

        case BIT_WIDTH_24:
            shift = 24;
            break;

        case BIT_WIDTH_32:
            shift = 32;
            break;

        default:
            shift = 0;
            break;
    }

    i = 0;
    while (shift != 0)
    {
        shift -= 4;
        /* Convert each nibble, starting with the MS Nibble to ASCII */
        value[i] = '0' + ((data >> shift) & 0xf);
        /* Convert values from A-F */
        if (value[i] > '9')
        {
            value[i] = value[i] + 7;
        }
        i++;
    }

    return (value);
}
