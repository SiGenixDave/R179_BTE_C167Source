/*
 * PeekPoke.c
 *
 *  Created on: Nov 30, 2018
 *      Author: David Smail
 */
#include "Types.h"
#include "CmdProc.h"

typedef enum
{
    PEEK, POKE
} eAction;

const UINT_32 LO_FLASH_ADDR = 0x100000;
const UINT_32 LO_RAM_WRITE_ADDR = 0x210000;
const UINT_32 LO_RAM_READ_ADDR = 0x200000;
const UINT_32 LO_NVRAM_ADDR = 0x800000;
const UINT_32 LO_RTC_ADDR = 0x300000;
const UINT_32 HI_FLASH_ADDR = 0x13FFFE; /* 256 kBytes (only 16 bit accesses) */
const UINT_32 HI_RAM_ADDR = 0x21FFFE;   /* 128 kBytes (only 16 bit accesses) */
const UINT_32 HI_NVRAM_ADDR = 0x87FFFE; /* 512 kBytes (only 16 bit accesses) */
const UINT_32 HI_RTC_ADDR = 0x307FFF;   /*  32 kBytes (only  8 bit accesses) */

static BOOLEAN ValidAddressCheck (UINT_32 address, eAction action);

static BOOLEAN m_SinglePeek = FALSE;
static BOOLEAN m_SinglePoke = FALSE;
static BOOLEAN m_ContinuousPeek = FALSE;
static BOOLEAN m_ContinuousPoke = FALSE;
static UINT_32 m_Address;
static UINT_32 m_Data;
static eDataWidth m_DataType;

static UINT_32 debugVar;

void PeekSingleService (const char *str)
{
    UINT_8 data_8;
    UINT_16 data_16;
    UINT_32 data_32;
    if (m_SinglePeek)
    {
        m_SinglePeek = FALSE;
        switch (m_DataType)
        {
            case BIT_WIDTH_8:
                data_8 = *((UINT_8 *) m_Address);
                SendAddressDataResponse (str, m_Address, data_8, BIT_WIDTH_8);
                break;
            case BIT_WIDTH_16:
                data_16 = *((UINT_16 *) m_Address);
                SendAddressDataResponse (str, m_Address, data_16, BIT_WIDTH_16);
                break;
            case BIT_WIDTH_32:
                data_32 = *((UINT_32 *) m_Address);
                SendAddressDataResponse (str, m_Address, data_32, BIT_WIDTH_32);
                break;
            default:
                break;
        }

    }
}

BOOLEAN PeekSingle (char cmdPtr[][MAX_PARAM_LENGTH])
{
    BOOLEAN valid = FALSE;
    UINT_32 address;

    if (!HexStringToValue (cmdPtr[1], &address))
    {
        return (FALSE);
    }

    valid = ValidAddressCheck (address, PEEK);
    /* Check for valid address and data */
    if (valid)
    {
#ifdef WIN32
        m_Address = (UINT_32) &debugVar;
#else
        m_Address = address;
#endif
        m_SinglePeek = TRUE;
    }

    return (valid);
}

void PeekContinuousService (const char *str)
{
    UINT_8 data_8;
    UINT_16 data_16;
    UINT_32 data_32;
    if (m_ContinuousPeek)
    {
        switch (m_DataType)
        {
            case BIT_WIDTH_8:
                data_8 = *((UINT_8 *) m_Address);
                SendAddressDataResponse (str, m_Address, data_8, BIT_WIDTH_8);
                break;
            case BIT_WIDTH_16:
                data_16 = *((UINT_16 *) m_Address);
                SendAddressDataResponse (str, m_Address, data_16, BIT_WIDTH_16);
                break;
            case BIT_WIDTH_32:
                data_32 = *((UINT_32 *) m_Address);
                SendAddressDataResponse (str, m_Address, data_32, BIT_WIDTH_32);
                break;
            default:
                break;
        }

    }
}

BOOLEAN PeekContinuous (char cmdPtr[][MAX_PARAM_LENGTH])
{
    BOOLEAN valid = FALSE;
    UINT_32 address;

    if (!HexStringToValue (cmdPtr[1], &address))
    {
        return (FALSE);
    }

    valid = ValidAddressCheck (address, PEEK);
    /* Check for valid address and data */
    if (valid)
    {
#ifdef WIN32
        m_Address = (UINT_32) &debugVar;
#else
        m_Address = address;
#endif
        m_ContinuousPeek = TRUE;
    }

    return (valid);
}

void PokeSingleService (const char *str)
{
    if (m_SinglePoke)
    {
        m_SinglePoke = FALSE;
        switch (m_DataType)
        {
            case BIT_WIDTH_8:
                *((UINT_8 *) m_Address) = (UINT_8) m_Data;
                SendAddressDataResponse (str, m_Address, m_Data, BIT_WIDTH_8);
                break;
            case BIT_WIDTH_16:
                *((UINT_16 *) m_Address) = (UINT_16) m_Data;
                SendAddressDataResponse (str, m_Address, m_Data, BIT_WIDTH_16);
                break;
            case BIT_WIDTH_32:
                *((UINT_32 *) m_Address) = (UINT_32) m_Data;
                SendAddressDataResponse (str, m_Address, m_Data, BIT_WIDTH_32);
                break;
            default:
                break;
        }

    }
}

BOOLEAN PokeSingle (char cmdPtr[][MAX_PARAM_LENGTH])
{
    BOOLEAN valid = FALSE;
    UINT_32 address;
    UINT_32 data;

    if (!HexStringToValue (cmdPtr[1], &address))
    {
        return (FALSE);
    }
    if (!HexStringToValue (cmdPtr[2], &data))
    {
        return (FALSE);
    }

    valid = ValidAddressCheck (address, POKE);
    /* Check for valid address and data */
    if (valid)
    {
#ifdef WIN32
        m_Address = (UINT_32) &debugVar;
#else
        m_Address = address;
#endif
        m_Data = data;
        m_SinglePoke = TRUE;
    }

    return (valid);
}

void PokeContinuousService (const char *str)
{
    if (m_ContinuousPoke)
    {
        switch (m_DataType)
        {
            case BIT_WIDTH_8:
                *((UINT_8 *) m_Address) = (UINT_8) m_Data;
                SendAddressDataResponse (str, m_Address, (UINT_8) m_Data, BIT_WIDTH_8);
                break;
            case BIT_WIDTH_16:
                *((UINT_16 *) m_Address) = (UINT_16) m_Data;
                SendAddressDataResponse (str, m_Address, (UINT_16) m_Data, BIT_WIDTH_16);
                break;
            case BIT_WIDTH_32:
                *((UINT_32 *) m_Address) = (UINT_32) m_Data;
                SendAddressDataResponse (str, m_Address, (UINT_32) m_Data, BIT_WIDTH_32);
                break;
            default:
                break;
        }

    }
}

BOOLEAN PokeContinuous (char cmdPtr[][MAX_PARAM_LENGTH])
{
    BOOLEAN valid = FALSE;
    UINT_32 address;
    UINT_32 data;

    if (!HexStringToValue (cmdPtr[1], &address))
    {
        return (FALSE);
    }
    if (!HexStringToValue (cmdPtr[2], &data))
    {
        return (FALSE);
    }

    valid = ValidAddressCheck (address, POKE);
    /* Check for valid address and data */
    if (valid)
    {
#ifdef WIN32
        m_Address = (UINT_32) &debugVar;
#else
        m_Address = address;
#endif
        m_Data = data;
        m_ContinuousPoke = TRUE;
    }

    return (valid);
}

BOOLEAN PeekContinuousKill (char cmdPtr[][MAX_PARAM_LENGTH])
{
    m_ContinuousPeek = FALSE;
    return (TRUE);
}

BOOLEAN PokeContinuousKill (char cmdPtr[][MAX_PARAM_LENGTH])
{
    m_ContinuousPoke = FALSE;
    return (TRUE);
}

static BOOLEAN ValidAddressCheck (UINT_32 address, eAction action)
{
    UINT_32 ramLoAddress = (action == PEEK) ? LO_RAM_READ_ADDR : LO_RAM_WRITE_ADDR;

    if (action == PEEK)
    {
        if (address >= LO_FLASH_ADDR && address <= HI_FLASH_ADDR)
        {
            m_DataType = BIT_WIDTH_16;
            return (TRUE);
        }
    }

    if (address >= ramLoAddress && address <= HI_RAM_ADDR)
    {
        m_DataType = BIT_WIDTH_16;
        return (TRUE);
    }

    if (address >= LO_NVRAM_ADDR && address <= HI_NVRAM_ADDR)
    {
        m_DataType = BIT_WIDTH_16;
        return (TRUE);
    }

    if (address >= LO_RTC_ADDR && address <= HI_RTC_ADDR)
    {
        m_DataType = BIT_WIDTH_8;
        return (TRUE);
    }

    return (FALSE);
}
