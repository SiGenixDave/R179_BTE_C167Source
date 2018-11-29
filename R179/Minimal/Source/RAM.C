/*
 * Ram.c
 *
 *  Created on: Nov 21, 2018
 *      Author: David Smail
 */
#include "Types.h"
#include "CmdProc.h"

typedef struct
{
    const UINT_16 addrOffset;
    const UINT_16 data;
    BOOLEAN enable;
} UpdateTable;

static UpdateTable m_Table[] =
    {
        { 0x00, 0x0000, FALSE },
          { 0x00, 0x0001, FALSE },
          { 0x00, 0x0002, FALSE },
          { 0x00, 0x0004, FALSE },
          { 0x00, 0x0008, FALSE },
          { 0x00, 0x0010, FALSE },
          { 0x00, 0x0020, FALSE },
          { 0x00, 0x0040, FALSE },
          { 0x00, 0x0080, FALSE },
          { 0x00, 0x0100, FALSE },
          { 0x00, 0x0200, FALSE },
          { 0x00, 0x0400, FALSE },
          { 0x00, 0x0800, FALSE },
          { 0x00, 0x1000, FALSE },
          { 0x00, 0x2000, FALSE },
          { 0x00, 0x4000, FALSE },
          { 0x00, 0x8000, FALSE },
          { 0x00, 0xFFFF, FALSE },
          { 0x00, 0xFFFE, FALSE },
          { 0x00, 0xFFFD, FALSE },
          { 0x00, 0xFFFB, FALSE },
          { 0x00, 0xFFF7, FALSE },
          { 0x00, 0xFFEF, FALSE },
          { 0x00, 0xFFDF, FALSE },
          { 0x00, 0xFFBF, FALSE },
          { 0x00, 0xFF7F, FALSE },
          { 0x00, 0xFEFF, FALSE },
          { 0x00, 0xFDFF, FALSE },
          { 0x00, 0xFBFF, FALSE },
          { 0x00, 0xF7FF, FALSE },
          { 0x00, 0xEFFF, FALSE },
          { 0x00, 0xDFFF, FALSE },
          { 0x00, 0xBFFF, FALSE },
          { 0x00, 0x7FFF, FALSE },

    };

static const UINT_16 TABLE_SIZE = sizeof(m_Table) / sizeof(UpdateTable);

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
            m_Table[index].enable = FALSE;
            expectedValue = m_Table[index].data;
            baseAddress[m_Table[index].addrOffset] = expectedValue;
            actualValue = baseAddress[m_Table[index].addrOffset];
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

BOOLEAN RamTableUpdate (char cmdPtr[][MAX_PARAM_LENGTH])
{
    BOOLEAN valid = FALSE;
    UINT_32 tableIndex;

    if (!HexStringToValue (cmdPtr[1], &tableIndex))
    {
        return (FALSE);
    }

    if (tableIndex == 0x00FF)
    {
        UINT_16 index;
        for (index = 0; index < TABLE_SIZE; index++)
        {
            m_Table[index].enable = TRUE;
        }
        valid = TRUE;
    }
    else if (tableIndex < TABLE_SIZE)
    {
        m_Table[tableIndex].enable = TRUE;
        valid = TRUE;
    }
    return (valid);
}

