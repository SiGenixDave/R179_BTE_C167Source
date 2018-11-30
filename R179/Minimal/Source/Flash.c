/*
 * Flash.c
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
	  { 0x01, 0x0001, FALSE },
	  { 0x02, 0x0002, FALSE },
	  { 0x03, 0x0004, FALSE },
	  { 0x04, 0x0008, FALSE },
	  { 0x05, 0x0010, FALSE },
	  { 0x06, 0x0020, FALSE },
	  { 0x07, 0x0040, FALSE },
	  { 0x08, 0x0080, FALSE },
	  { 0x09, 0x0100, FALSE },
	  { 0x0A, 0x0200, FALSE },
	  { 0x0B, 0x0400, FALSE },
	  { 0x0C, 0x0800, FALSE },
	  { 0x0D, 0x1000, FALSE },
	  { 0x0E, 0x2000, FALSE },
	  { 0x0F, 0x4000, FALSE },
	  { 0x10, 0x8000, FALSE },
	  { 0x11, 0xFFFF, FALSE },
	  { 0x12, 0xFFFE, FALSE },
	  { 0x13, 0xFFFD, FALSE },
	  { 0x14, 0xFFFB, FALSE },
	  { 0x15, 0xFFF7, FALSE },
	  { 0x16, 0xFFEF, FALSE },
	  { 0x17, 0xFFDF, FALSE },
	  { 0x18, 0xFFBF, FALSE },
	  { 0x19, 0xFF7F, FALSE },
	  { 0x1A, 0xFEFF, FALSE },
	  { 0x1B, 0xFDFF, FALSE },
	  { 0x1C, 0xFBFF, FALSE },
	  { 0x1D, 0xF7FF, FALSE },
	  { 0x1E, 0xEFFF, FALSE },
	  { 0x1F, 0xDFFF, FALSE },
	  { 0x20, 0xBFFF, FALSE },
	  { 0x21, 0x7FFF, FALSE } };

static const UINT_16 TABLE_SIZE = sizeof(m_Table) / sizeof(UpdateTable);

void FlashService(const char *str)
{
#ifdef _WIN32
    UINT_16 debugBaseArray[100] = {0x0000, 0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0xABCD, 0xABCD, 0xABCD, 0xABCD,
                                   0x1234, 0x3456, 0x0023, 0x0234, 0x0008};
    UINT_16 *baseAddress = debugBaseArray;
#else
    UINT_16 *baseAddress = (UINT_16 *)0x110000;
#endif

	UINT_16 actualValue;
	UINT_16 expectedValue;
	UINT_16 index;

	for (index = 0; index < TABLE_SIZE; index++)
	{
		if (m_Table[index].enable)
		{
			m_Table[index].enable = FALSE;
			actualValue = baseAddress[m_Table[index].addrOffset];
			expectedValue = m_Table[index].data;

			if (actualValue != expectedValue)
			{
				SendMismatchError(str, expectedValue, actualValue, BIT_WIDTH_16);
			}
			else
			{
				SendTestPassed(str, expectedValue, BIT_WIDTH_16);
			}
		}
	}
}

BOOLEAN FlashTableUpdate(char cmdPtr[][MAX_PARAM_LENGTH])
{
	BOOLEAN valid = FALSE;
	UINT_32 tableIndex;

	if (!HexStringToValue(cmdPtr[1], &tableIndex))
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
