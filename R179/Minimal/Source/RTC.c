/*
 * RTC.c
 *
 *  Created on: Nov 21, 2018
 *      Author: David Smail
 */
#include "Types.h"
#include "CmdProc.h"

typedef struct
{
    const UINT_16 addrOffset;
    const UINT_8 data;
    BOOLEAN enable;
} UpdateTable;

static UpdateTable m_Table[] =
    {
        { 0x00, 0x00, FALSE },
          { 0x00, 0x01, FALSE },
          { 0x00, 0x02, FALSE },
          { 0x00, 0x04, FALSE },
          { 0x00, 0x08, FALSE },
          { 0x00, 0x10, FALSE },
          { 0x00, 0x20, FALSE },
          { 0x00, 0x40, FALSE },
          { 0x00, 0x80, FALSE },
          { 0x00, 0xFF, FALSE },
          { 0x00, 0xFE, FALSE },
          { 0x00, 0xFD, FALSE },
          { 0x00, 0xFB, FALSE },
          { 0x00, 0xF7, FALSE },
          { 0x00, 0xEF, FALSE },
          { 0x00, 0xDF, FALSE },
          { 0x00, 0xBF, FALSE },
          { 0x00, 0x7F, FALSE },

    };

static const UINT_16 TABLE_SIZE = sizeof(m_Table) / sizeof(UpdateTable);

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

BOOLEAN RTCTableUpdate (char cmdPtr[][MAX_PARAM_LENGTH])
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

