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

#include "Types.h"
#include "CmdProc.h"

/*--------------------------------------------------------------------------
 MODULE CONSTANTS
 --------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 MODULE MACROS
 --------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 MODULE DATA TYPES
 --------------------------------------------------------------------------*/

typedef struct
{
    const UINT_16 addrOffset;
    const UINT_8 data;
    BOOLEAN enable;
} UpdateTable;

/*--------------------------------------------------------------------------
 MODULE VARIABLES
 --------------------------------------------------------------------------*/
static UpdateTable m_Table[] = { { 0x00, 0x00, FALSE }, { 0x00, 0x01, FALSE }, { 0x00, 0x02, FALSE }, {
    0x00, 0x04, FALSE }, { 0x00, 0x08, FALSE }, { 0x00, 0x10, FALSE }, { 0x00, 0x20, FALSE }, { 0x00, 0x40, FALSE }, {
    0x00, 0x80, FALSE }, { 0x00, 0xFF, FALSE }, { 0x00, 0xFE, FALSE }, { 0x00, 0xFD, FALSE }, { 0x00, 0xFB, FALSE }, {
    0x00, 0xF7, FALSE }, { 0x00, 0xEF, FALSE }, { 0x00, 0xDF, FALSE }, { 0x00, 0xBF, FALSE }, { 0x00, 0x7F, FALSE },

};

static const UINT_16 TABLE_SIZE = sizeof(m_Table) / sizeof(UpdateTable);

/*--------------------------------------------------------------------------
 MODULE PROTOTYPES
 --------------------------------------------------------------------------*/

/***************************************************************************
 *
 * Description: Determines if any RTC accesses are enabled and if so, writes
 *              to the specified address and reads the data back. It then compares
 *              the value read with the value written. If the values agree,
 *              then a PASS is sent back to the user; otherwise a FAIL is
 *              sent back with expected and actual values reported.
 *
 *
 * Parameters:  str - command string
 *
 * Returns:     None
 *
 ***************************************************************************/
void RTCService (const char *str)
{

#ifdef _WIN32
    UINT_8 debugBaseArray[100];
    UINT_8 *baseAddress = debugBaseArray;
#else
    UINT_8 *baseAddress = (UINT_8 *)0x301000;
#endif

    UINT_8 actualValue;
    UINT_8 expectedValue;
    UINT_16 index;

    for (index = 0; index < TABLE_SIZE; index++)
    {
        if (m_Table[index].enable)
        {
            /* Disable the test (i.e. 1 shot it) */
            m_Table[index].enable = FALSE;
            /* Write the desired value and read it */
            expectedValue = m_Table[index].data;
            baseAddress[m_Table[index].addrOffset] = expectedValue;
            actualValue = baseAddress[m_Table[index].addrOffset];
            /* Compare and indicate a pass or fail to the user */
            if (actualValue != expectedValue)
            {
                SendMismatchError (str, expectedValue, actualValue, BIT_WIDTH_8);
            }
            else
            {
                SendTestPassed (str, expectedValue, BIT_WIDTH_8);
            }
        }
    }

}

/***************************************************************************
 *
 * Description: Gets the desired offset into the table from the user
 *              and sets the enable flag. FF (255) as offset indicates
 *              that the user wants to make all flags in the table TRUE
 *
 *
 * Parameters:  cmdPtr - array with the command and parameters
 *
 * Returns:     BOOLEAN - TRUE if tableIndex is valid; FALSE otherwise
 *
 ***************************************************************************/
BOOLEAN RTCTableUpdate (char cmdPtr[][MAX_PARAM_LENGTH])
{
    BOOLEAN valid = FALSE;
    UINT_32 tableIndex;

    if (!HexStringToValue (cmdPtr[1], &tableIndex))
    {
        return (FALSE);
    }

    /* Set all enable flags to TRUE */
    if (tableIndex == 0x00FF)
    {
        UINT_16 index;
        for (index = 0; index < TABLE_SIZE; index++)
        {
            m_Table[index].enable = TRUE;
        }
        valid = TRUE;
    }
    /* Set only 1 flag if tableIndex is in range */
    else if (tableIndex < TABLE_SIZE)
    {
        m_Table[tableIndex].enable = TRUE;
        valid = TRUE;
    }
    return (valid);
}

