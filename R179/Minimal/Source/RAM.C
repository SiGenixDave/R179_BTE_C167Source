/******************************************************************************
*
* Copyright (C) 2014-15 Bombardier
*
* File Name: RAM.c
*
* Revision History:
*   08/31/2014 - das - Created
*
******************************************************************************/
///   \file
///   This file contains the code that verifies the external RAM integrity.

#define ram_C

/*--------------------------------------------------------------------------
                              INCLUDE FILES
  --------------------------------------------------------------------------*/
#ifndef _DEBUG
#include <reg167.h>
#endif

#include "Types.h"
#include "SerComm.h"
#include "HWInit.h"
#include "HexLED.h"

/*--------------------------------------------------------------------------
                             MODULE CONSTANTS
  --------------------------------------------------------------------------*/
#ifndef _DEBUG
#define HUGE huge
#else
#define HUGE
#endif

#ifndef _DEBUG
#define RAM_START				(0x230000UL)
#define RAM_END					(0x23FFFFUL)
#define NVRAM_START				(0x800000UL)
#define NVRAM_END				(0x80FFFFUL)
#else
UINT_16 memory[65536];
#define RAM_START               (UINT_32)&memory[0]
#define RAM_END					(UINT_32)&memory[2000 - 1]
#define NVRAM_START             (UINT_32)&memory[0]
#define NVRAM_END				(UINT_32)&memory[2000 - 1]
#endif

/*--------------------------------------------------------------------------
                              MODULE MACROS
  --------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
                             MODULE DATA TYPES
  --------------------------------------------------------------------------*/
typedef enum
{
    TEST_STUCK_DATA_RAM,
    TEST_STUCK_DATA_NVRAM,
    TEST_STUCK_ADDR_RAM,
    TEST_STUCK_ADDR_NVRAM,
    TEST_CELL_RAM,
    TEST_CELL_NVRAM

} eRuntimeRAMTestState;


/*--------------------------------------------------------------------------
                              MODULE VARIABLES
  --------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
                             MODULE PROTOTYPES
  --------------------------------------------------------------------------*/
static BOOLEAN RAMCellTest (UINT_32 aBaseAddress);
static BOOLEAN RAMStuckAddressTest (UINT_32 aBaseAddress);
static BOOLEAN RAMStuckDataTest (UINT_32 aBaseAddress);

//--------------------------------------------------------------------------
// Module:
//  VerifyRAM
//
///   This function executes the entire RAM test and verifies that
///   RAM can be written to and read from without failure.
///
///   returns BOOLEAN - TRUE if test passed
///
//--------------------------------------------------------------------------
BOOLEAN VerifyRAM (void)
{
	////////////////////////////////////////////////////////////////////////
	// NOTE: NVRAM tests eliminated because NVRAM is not used on R179
	////////////////////////////////////////////////////////////////////////
	UINT_32 ramAddress = RAM_START;

    HexLEDUpdate (0xCA);

    if (!RAMStuckDataTest (RAM_START))
    {
        return FALSE;
    }

    if (!RAMStuckAddressTest (RAM_START))
    {
        return FALSE;
    }

    while (ramAddress < RAM_END)
    {
        if (!RAMCellTest (ramAddress))
        {
            return FALSE;
        }
        ramAddress += 2;
    }

    return TRUE;
}

//--------------------------------------------------------------------------
// Module:
//  RAMCellTest
//
///   This function tests each memory cell and can detect a stuck or
///   not connected data bit.
///
///   \param aBaseAddress - the memory address to test
///
///   returns BOOLEAN - TRUE if test passed
///
//--------------------------------------------------------------------------
static BOOLEAN RAMCellTest (UINT_32 aBaseAddress)
{
    register UINT_16 *addr = (UINT_16 *)aBaseAddress;

    register UINT_16 ramOrigValue;

    register BOOLEAN retVal = TRUE;

    DISABLE_ALL_INTERRUPTS();
    ToggleCPUWatchdog();

    ramOrigValue = *addr;

    *addr = 0x5555;
    if (*addr != 0x5555)
    {
        retVal = FALSE;
    }

    *addr = 0xAAAA;
    if (*addr != 0xAAAA)
    {
        retVal =  FALSE;
    }

    *addr = ramOrigValue;

    ENABLE_ALL_INTERRUPTS();

    return retVal;
}

//--------------------------------------------------------------------------
// Module:
//  RAMStuckDataTest
//
///   This function tests for a stuck data bit by walking 0's
///   and walking 1's through one memory address.
///
///   \param aBaseAddress - the memory address to test
///
///   returns BOOLEAN - TRUE if test passed
///
//--------------------------------------------------------------------------
static BOOLEAN RAMStuckDataTest (UINT_32 aBaseAddress)
{
    register UINT_16 HUGE *baseAddress = (UINT_16 *)aBaseAddress;
    register BOOLEAN retVal = TRUE;
    register UINT_16 data;
    register UINT_16 origContents;

    DISABLE_ALL_INTERRUPTS();

    /* Save the original contents; will be restored after
       the test is over */
    origContents = baseAddress[0];

    for (data = 1; data != 0; data <<= 1)
    {
        ToggleCPUWatchdog();
        /* Walk a 1 the entire length of the data bus */
        baseAddress[0] = data;
        /* Verify */
        if (baseAddress[0] != data)
        {
            retVal = FALSE;
        }
        /* Walk a 0 the entire length of the data bus */
        baseAddress[0] = (UINT_16)~data;
        /* Verify */
        if (baseAddress[0] != (UINT_16)~data)
        {
            retVal = FALSE;
        }
    }

    /* Restore the contents */
    baseAddress[0] = origContents;

    ENABLE_ALL_INTERRUPTS();

    return retVal;
}


//--------------------------------------------------------------------------
// Module:
//  RAMStuckAddressTest
//
///   This function tests for a stuck address bit by walking 0's
///   and walking 1's on the address bus. This function tests
///   only the lower 16 bits (64k of memory) of the address bus.
///
///   \param aBaseAddress - the memory address to test
///
///   returns BOOLEAN - TRUE if test passed
///
//--------------------------------------------------------------------------
static BOOLEAN RAMStuckAddressTest (UINT_32 aBaseAddress)
{
    register UINT_16 HUGE *baseAddress = (UINT_16 *)aBaseAddress;
    register UINT_16 offset;
    register UINT_16 memUnderTestContents;
    register UINT_16 baseContents;
    register BOOLEAN retVal = TRUE;
    const UINT_16 pattern = 0x5555;

    DISABLE_ALL_INTERRUPTS();    /* Store the original contents */
    baseContents = baseAddress[0];
    /* Write a fixed pattern to the base */
    baseAddress[0] = pattern;
    /* Look for stuck low address bits */
    /* Walk address of 1's */
    for (offset = 1; offset != 0x8000; offset <<= 1)
    {
        ToggleCPUWatchdog();
        /* Save contents of address */
        memUnderTestContents = baseAddress[offset];
        /* Write the opposite of the test pattern */
        baseAddress[offset] = ~pattern;
        /* Verify the base hasn't been corrupted */
        if (baseAddress[0] != pattern)
        {
            retVal = FALSE;
        }
        baseAddress[offset] = memUnderTestContents;
    }
    /* Restore the original contents */
    baseAddress[0] = baseContents;
    /* Now start at the high end of the 64k memory  */
    baseContents = baseAddress[0x7FFF];
    /* Write the pattern */
    baseAddress[0x7FFF] = pattern;
    /* Look for stuck high address bits */
    /* Walk address of 0's */
    for (offset = 1; offset != 0x8000; offset <<= 1)
    {
        /* Save contents of address. By inverting offset,
           0's are walked */
        memUnderTestContents = baseAddress[ (UINT_16)~offset];
        /* Write the opposite of the test pattern */
        baseAddress[ (UINT_16)~offset] = ~pattern;
        /* Verify the base hasn't been corrupted */
        if (baseAddress[0x7FFF] != pattern)
        {
            retVal = FALSE;
        }
        baseAddress[ (UINT_16)~offset] = memUnderTestContents;
    }
    /* Restore the original contents */
    baseAddress[0x7FFF] = baseContents;
    ENABLE_ALL_INTERRUPTS();
    return retVal;
}