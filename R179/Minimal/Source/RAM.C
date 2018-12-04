/******************************************************************************
 *
 * Copyright (C) 2018-19 Bombardier
 *
 * File Name: NVRam.c
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
    const UINT_16 data;
    BOOLEAN enable;
} UpdateTable;

/*--------------------------------------------------------------------------
 MODULE VARIABLES
 --------------------------------------------------------------------------*/

static UpdateTable m_Table[] = { { 0x00, 0x0000, FALSE }, { 0x00, 0x0001, FALSE }, { 0x00, 0x0002, FALSE }, {
    0x00, 0x0004, FALSE }, { 0x00, 0x0008, FALSE }, { 0x00, 0x0010, FALSE }, { 0x00, 0x0020, FALSE }, {
    0x00, 0x0040, FALSE }, { 0x00, 0x0080, FALSE }, { 0x00, 0x0100, FALSE }, { 0x00, 0x0200, FALSE }, {
    0x00, 0x0400, FALSE }, { 0x00, 0x0800, FALSE }, { 0x00, 0x1000, FALSE }, { 0x00, 0x2000, FALSE }, {
    0x00, 0x4000, FALSE }, { 0x00, 0x8000, FALSE }, { 0x00, 0xFFFF, FALSE }, { 0x00, 0xFFFE, FALSE }, {
    0x00, 0xFFFD, FALSE }, { 0x00, 0xFFFB, FALSE }, { 0x00, 0xFFF7, FALSE }, { 0x00, 0xFFEF, FALSE }, {
    0x00, 0xFFDF, FALSE }, { 0x00, 0xFFBF, FALSE }, { 0x00, 0xFF7F, FALSE }, { 0x00, 0xFEFF, FALSE }, {
    0x00, 0xFDFF, FALSE }, { 0x00, 0xFBFF, FALSE }, { 0x00, 0xF7FF, FALSE }, { 0x00, 0xEFFF, FALSE }, {
    0x00, 0xDFFF, FALSE }, { 0x00, 0xBFFF, FALSE }, { 0x00, 0x7FFF, FALSE }, };

static const UINT_16 TABLE_SIZE = sizeof(m_Table) / sizeof(UpdateTable);

/*--------------------------------------------------------------------------
 MODULE PROTOTYPES
 --------------------------------------------------------------------------*/

/***************************************************************************
 *
 * Description: Determines if any RAM accesses are enabled and if so, writes
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
void RamService (const char *str)
{
#ifdef _WIN32
    UINT_16 debugBaseArray[100];
    UINT_16 *baseAddress = debugBaseArray;
#else
    UINT_16 *baseAddress = (UINT_16 *)0x210000;
#endif

    UINT_16 actualValue;
    UINT_16 expectedValue;
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
                SendMismatchError (str, expectedValue, actualValue, BIT_WIDTH_16);
            }
            else
            {
                SendTestPassed (str, expectedValue, BIT_WIDTH_16);
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
BOOLEAN RamTableUpdate (char cmdPtr[][MAX_PARAM_LENGTH])
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
