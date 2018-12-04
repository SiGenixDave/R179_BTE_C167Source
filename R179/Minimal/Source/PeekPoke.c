/******************************************************************************
 *
 * Copyright (C) 2018-19 Bombardier
 *
 * File Name: PeekPoke.c
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
const UINT_32 LO_FLASH_ADDR = 0x100000;
const UINT_32 LO_RAM_WRITE_ADDR = 0x210000;
const UINT_32 LO_RAM_READ_ADDR = 0x200000;
const UINT_32 LO_NVRAM_ADDR = 0x800000;
const UINT_32 LO_RTC_ADDR = 0x300000;
const UINT_32 HI_FLASH_ADDR = 0x13FFFE; /* 256 kBytes (only 16 bit accesses) */
const UINT_32 HI_RAM_ADDR = 0x21FFFE; /* 128 kBytes (only 16 bit accesses) */
const UINT_32 HI_NVRAM_ADDR = 0x87FFFE; /* 512 kBytes (only 16 bit accesses) */
const UINT_32 HI_RTC_ADDR = 0x307FFF; /*  32 kBytes (only  8 bit accesses) */

/*--------------------------------------------------------------------------
 MODULE MACROS
 --------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 MODULE DATA TYPES
 --------------------------------------------------------------------------*/
typedef enum
{
    PEEK, POKE
} eAction;

/*--------------------------------------------------------------------------
 MODULE VARIABLES
 --------------------------------------------------------------------------*/
static BOOLEAN m_SinglePeek = FALSE;
static BOOLEAN m_SinglePoke = FALSE;
static BOOLEAN m_ContinuousPeek = FALSE;
static BOOLEAN m_ContinuousPoke = FALSE;
static UINT_32 m_PeekAddress;
static UINT_32 m_PokeAddress;
static UINT_32 m_PokeData;
static eDataWidth m_DataType;

#ifdef WIN32
static UINT_32 debugVar;
#endif

/*--------------------------------------------------------------------------
 MODULE PROTOTYPES
 --------------------------------------------------------------------------*/
static void PeekAddressReportResult (const char * str);
static void PokeAddressReportResult (const char * str);
static BOOLEAN ValidAddressCheck (UINT_32 address, eAction action);

/***************************************************************************
 *
 * Description: Determines if any single Peek (read) is requested. If so
 *              it reads the data from the address requested by the user
 *              and reports the result.
 *
 *
 * Parameters:  str - command string
 *
 * Returns:     None
 *
 ***************************************************************************/
void PeekSingleService (const char *str)
{
    if (m_SinglePeek)
    {
        m_SinglePeek = FALSE;
        PeekAddressReportResult (str);
    }
}

/***************************************************************************
 *
 * Description: Determines if the requested address is valid and if
 *              so, set the flag to indicate a single peek is requested.
 *
 *
 * Parameters:  cmdPtr - array with the command and parameters
 *
 * Returns:     BOOLEAN - TRUE if requested address is valid; FALSE otherwise
 *
 ***************************************************************************/
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
        m_PeekAddress = (UINT_32) &debugVar;
#else
        m_PeekAddress = address;
#endif
        m_SinglePeek = TRUE;
    }

    return (valid);
}

/***************************************************************************
 *
 * Description: Determines if a continuous Peek (read) is requested. If so
 *              it reads the data from the address requested by the user
 *              and reports the result until the user "kills" the continuous
 *              peeking via the "PEK" command.
 *
 *
 * Parameters:  str - command string
 *
 * Returns:     None
 *
 ***************************************************************************/
void PeekContinuousService (const char *str)
{
    if (m_ContinuousPeek)
    {
        PeekAddressReportResult (str);
    }
}

/***************************************************************************
 *
 * Description: Determines if the requested address is valid and if
 *              so, set the flag to indicate a continuous peek is requested.
 *
 *
 * Parameters:  cmdPtr - array with the command and parameters
 *
 * Returns:     BOOLEAN - TRUE if requested address is valid; FALSE otherwise
 *
 ***************************************************************************/
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
        m_PeekAddress = (UINT_32) &debugVar;
#else
        m_PeekAddress = address;
#endif
        m_ContinuousPeek = TRUE;
    }

    return (valid);
}

/***************************************************************************
 *
 * Description: Determines if a single poke (write) is requested. If so
 *              it writes the requested data to the address requested by the
 *              user.
 *
 *
 * Parameters:  str - command string
 *
 * Returns:     None
 *
 ***************************************************************************/
void PokeSingleService (const char *str)
{
    if (m_SinglePoke)
    {
        m_SinglePoke = FALSE;
        PokeAddressReportResult (str);
    }
}

/***************************************************************************
 *
 * Description: Determines if the requested address and data is valid and if
 *              so, set the flag to indicate a single poke is requested.
 *
 *
 * Parameters:  cmdPtr - array with the command and parameters
 *
 * Returns:     BOOLEAN - TRUE if requested address is valid; FALSE otherwise
 *
 ***************************************************************************/
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
        m_PokeAddress = (UINT_32) &debugVar;
#else
        m_PokeAddress = address;
#endif
        m_PokeData = data;
        m_SinglePoke = TRUE;
    }

    return (valid);
}

/***************************************************************************
 *
 * Description: Determines if a continuous poke (write) is requested. If so
 *              it writes the requested data to the address requested by the
 *              user.
 *
 *
 * Parameters:  str - command string
 *
 * Returns:     None
 *
 ***************************************************************************/
void PokeContinuousService (const char *str)
{
    if (m_ContinuousPoke)
    {
        PokeAddressReportResult (str);
    }
}

/***************************************************************************
 *
 * Description: Determines if the requested address and data is valid and if
 *              so, set the flag to indicate a continuous poke is requested.
 *
 *
 * Parameters:  cmdPtr - array with the command and parameters
 *
 * Returns:     BOOLEAN - TRUE if requested address is valid; FALSE otherwise
 *
 ***************************************************************************/
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
        m_PokeAddress = (UINT_32) &debugVar;
#else
        m_PokeAddress = address;
#endif
        m_PokeData = data;
        m_ContinuousPoke = TRUE;
    }

    return (valid);
}

/***************************************************************************
 *
 * Description: Kills continuous peeking (reading)
 *
 *
 * Parameters:  cmdPtr - array with the command and parameters
 *
 * Returns:     BOOLEAN - TRUE always
 *
 ***************************************************************************/
BOOLEAN PeekContinuousKill (char cmdPtr[][MAX_PARAM_LENGTH])
{
    m_ContinuousPeek = FALSE;
    return (TRUE);
}

/***************************************************************************
 *
 * Description: Kills continuous poking (reading)
 *
 *
 * Parameters:  cmdPtr - array with the command and parameters
 *
 * Returns:     BOOLEAN - TRUE always
 *
 ***************************************************************************/
BOOLEAN PokeContinuousKill (char cmdPtr[][MAX_PARAM_LENGTH])
{
    m_ContinuousPoke = FALSE;
    return (TRUE);
}

/***************************************************************************
 *
 * Description: Peeks (reads) the requested address and reports the result.
 *
 *
 * Parameters:  str - command string
 *
 * Returns:     None
 *
 ***************************************************************************/
static void PeekAddressReportResult (const char * str)
{
    UINT_8 data_8;
    UINT_16 data_16;
    UINT_32 data_32;

    /* Data type is set based on the device (address) being read from */
    switch (m_DataType)
    {
        case BIT_WIDTH_8:
            data_8 = *((UINT_8 *) m_PeekAddress);
            SendAddressDataResponse (str, m_PeekAddress, data_8, BIT_WIDTH_8);
            break;
        case BIT_WIDTH_16:
            data_16 = *((UINT_16 *) m_PeekAddress);
            SendAddressDataResponse (str, m_PeekAddress, data_16, BIT_WIDTH_16);
            break;
        case BIT_WIDTH_32:
            data_32 = *((UINT_32 *) m_PeekAddress);
            SendAddressDataResponse (str, m_PeekAddress, data_32, BIT_WIDTH_32);
            break;
        default:
            break;
    }
}

/***************************************************************************
 *
 * Description: Pokes (writes) the requested data to the requested address
 *              and reports the result.
 *
 *
 * Parameters:  str - command string
 *
 * Returns:     None
 *
 ***************************************************************************/
static void PokeAddressReportResult (const char * str)
{
    switch (m_DataType)
    {
        case BIT_WIDTH_8:
            *((UINT_8 *) m_PokeAddress) = (UINT_8) m_PokeData;
            SendAddressDataResponse (str, m_PokeAddress, m_PokeData, BIT_WIDTH_8);
            break;
        case BIT_WIDTH_16:
            *((UINT_16 *) m_PokeAddress) = (UINT_16) m_PokeData;
            SendAddressDataResponse (str, m_PokeAddress, m_PokeData, BIT_WIDTH_16);
            break;
        case BIT_WIDTH_32:
            *((UINT_32 *) m_PokeAddress) = (UINT_32) m_PokeData;
            SendAddressDataResponse (str, m_PokeAddress, m_PokeData, BIT_WIDTH_32);
            break;
        default:
            break;
    }

}

/***************************************************************************
 *
 * Description: Pokes (writes) the requested data to the requested address
 *              and reports the result.
 *
 *
 * Parameters:  address - requested address to either peek or poke
 *              action - peek or poke (valid addresses change based on action)
 *
 * Returns:     TRUE if requested address is valid; FALSE otherwise
 *
 ***************************************************************************/
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
