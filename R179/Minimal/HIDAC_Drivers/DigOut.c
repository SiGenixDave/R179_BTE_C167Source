/*****************************************************************************
*
* Copyright (C) 2010 Bombardier
*
* File Name: DigOut.c
*
* Revision History:
*   08/31/2009 - das - Created
*
******************************************************************************/
///   \file
///   This file contains functions write/update the digital output banks


#define digout_C

/*--------------------------------------------------------------------------
                              INCLUDE FILES
  --------------------------------------------------------------------------*/
/* _DEBUG is defined in Microsoft Visual Studio 2008: UNIT TEST ONLY */
#ifndef _DEBUG
    #include <c166.h>
    #include <reg167.h>
#endif
#include "Types.h"
#include "DigOut.h"

/*--------------------------------------------------------------------------
                             MODULE CONSTANTS
  --------------------------------------------------------------------------*/
#define DIGOUT_BANK_WIDTH		16

/*--------------------------------------------------------------------------
                              MODULE MACROS
  --------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------
                             MODULE DATA TYPES
  --------------------------------------------------------------------------*/
typedef struct
{
    UINT_16  *shadowPtr;
    volatile UINT_16  *hwPtr;
} DigOut_t;

/*--------------------------------------------------------------------------
                              MODULE VARIABLES
  --------------------------------------------------------------------------*/
static UINT_16 shadowReg[NUM_DIGOUT_BANKS];  /* Stores a copy of the digital 
                                                output bank */

#ifdef _DEBUG
/* Use for unit test only */
static UINT_16 hwReg[NUM_DIGOUT_BANKS];
#endif


/* Digital Output pointer info*/
const DigOut_t DigOut[] = 
{
    {
        &shadowReg[0], 
#ifdef _DEBUG
        &hwReg[0],
#else
        /* 
        x.......  = P7.7  => DRV07
        .x......  = P7.6  => DRV06
        ..x.....  = P7.5  => DRV05
        ...x....  = P7.4  => DRV04
        ....x...  = P7.3  => DRV03
        .....x..  = P7.2  => DRV02
        ......x.  = P7.1  => DRV01
        .......x  = P7.0  => DRV00
        00000000  = 0x00 */
        (volatile UINT_16 *)(0xFFD0L) /* 16 bit physical address of P7;
                                compiler does not like &P7 */
#endif
        
    },
    {
        &shadowReg[1], 
#ifdef _DEBUG
        &hwReg[1],
#else
        /* U41_port_e input definition
        x...............  = DRV23
        .x..............  = DRV22
        ..x.............  = DRV21
        ...x............  = DRV20
        ....x...........  = DRV19
        .....x..........  = DRV18
        ......x.........  = DRV17
        .......x........  = DRV16
        ........x.......  = DRV15
        .........x......  = DRV14
        ..........x.....  = DRV13
        ...........x....  = DRV12
        ............x...  = DRV11
        .............x..  = DRV10
        ..............x.  = DRV9
        ...............x  = DRV8
        */
        (volatile UINT_16 *)(0x500006L)
#endif
        
    },
    {
        &shadowReg[2],
#ifdef _DEBUG
        &hwReg[2],
#else
        /* U41_port_f output configuration
        1...............  = DRV31 => ISO_DOUT31 is OFF
        .1..............  = DRV30 => ISO_DOUT30 is OFF
        ..1.............  = DRV29 => ISO_DOUT29 is OFF
        ...1............  = DRV28 => ISO_DOUT28 is OFF
        ....1...........  = DRV27 => ISO_DOUT27 is OFF
        .....1..........  = DRV26 => ISO_DOUT26 is OFF
        ......1.........  = DRV25 => ISO_DOUT25 is OFF
        .......1........  = DRV24 => ISO_DOUT24 is OFF
        ........0.......  = PROT7	=> SSR_IN1 disabled
        .........0......  = PROT6	=> +5 V to ITU Interface board disabled
        ..........0.....  = PROT5	=> +15 V to ITU Interface board disabled
        ...........0....  = PROT4	=> +5 V to Chart Recorder Interface board disabled
        ............0...  = PROT3	=> +15 V to Chart Recorder Interface board disabled
        .............0..  = PROT2	=> -15 V to Chart Recorder Interface board disabled
        ..............0.  = PROT1   => SSR_IN2 disabled
        ...............1  = PROT0	=> VDRIVE enable
        1111111100000001  = 0xff01 */
        (volatile UINT_16 *)(0x500008L)
#endif
        
    },

    {
        &shadowReg[3],
#ifdef _DEBUG
        &hwReg[3],
#else
        /* Remote display 	
        ....x...........  = DISP_11
        .....x..........  = DISP_10
        ......x.........  = DISP_09
        .......x........  = DISP_08
        ........x.......  = DISP_07
        .........x......  = DISP_06
        ..........x.....  = DISP_05
        ...........x....  = DISP_04
        ............x...  = DISP_03
        .............x..  = DISP_02
        ..............x.  = DISP_01
        ...............x  = DISP_00
        */      
        (volatile UINT_16 *)(0x500022L)
#endif
        
    }
};

/*--------------------------------------------------------------------------
                             MODULE PROTOTYPES
  --------------------------------------------------------------------------*/

//--------------------------------------------------------------------------
// Module:
//  DO_Init
/// 
///   This function intializes the P7 port so that the desired bits become 
///   outputs.
///   
///   \param aMask - a "1" in a bit position indicates that the CPU pin 
///                  (PORT7.7 -> PORT7.0) will be configured as an output
///
//--------------------------------------------------------------------------
void DO_Init(UINT_8 aMask)
{
#ifndef _DEBUG
    P7 = 0x0000;  
    DP7 = aMask;
#endif
}
//--------------------------------------------------------------------------
// Module:
//  DO_WriteBank
//
///   This function writes a specific value to a digital output bank. All bits
///   in the bank are updated.
///
///   \param aBankId - id of the digital output bank
///   \param aValue - new value of the digital output bank 
/// 
//--------------------------------------------------------------------------
void DO_WriteBank(eDigOutBankId aBankId, UINT_16 aValue)
{
    DISABLE_ALL_INTERRUPTS();

    *(DigOut[aBankId].shadowPtr) = aValue;
    *(DigOut[aBankId].hwPtr) = aValue;

    ENABLE_ALL_INTERRUPTS();
}


//--------------------------------------------------------------------------
// Module:
//  DO_WriteBankWithMask
//
///   This function writes a specific value to a digital output bank. All bits
///   in the bank within the mask are updated. The other bits outside of the 
///   mask are preserved.
///
///   \param aBankId - id of the digital output bank
///   \param aValue - new value of the digital output bank 
///   \param aMask - the bits in which the new value will be updated 
/// 
//--------------------------------------------------------------------------
void DO_WriteBankWithMask(eDigOutBankId aBankId, UINT_16 aValue, UINT_16 aMask)
{
    UINT_16 tmpVal;

    DISABLE_ALL_INTERRUPTS();

    /* Preserve the bits outside of the mask. Make all bits inside of the 
       mask "0". */
    tmpVal = *(DigOut[aBankId].shadowPtr) & ~aMask;

    /* Ensure any bits are set outside of the mask are "0" and update the new
       value */
    tmpVal |= aValue & aMask; 

    *(DigOut[aBankId].shadowPtr) = tmpVal;
    *(DigOut[aBankId].hwPtr) = tmpVal;

    ENABLE_ALL_INTERRUPTS();
}

//--------------------------------------------------------------------------
// Module:
//  DO_WriteBankWithMaskAndShift
//
///   This function writes a specific value to a digital output bank. All bits
///   in the bank within the mask are updated. The other bits outside of the 
///   mask are preserved. The mask and value are shifted left by the desired
///   amount prior to updating the bank.
///
///   \param aBankId - id of the digital output bank
///   \param aValue - new value of the digital output bank 
///   \param aMask - the bits in which the new value will be updated 
///   \param aShift - the number of bits to shift left the mask and value
/// 
//--------------------------------------------------------------------------
void DO_WriteBankWithMaskAndShift(eDigOutBankId aBankId, UINT_16 aValue,
                                  UINT_16 aMask, UINT_16 aShift)
{
    UINT_16 tmpVal;

    /* Shift the mask over to the left */
    UINT_16 tmpMask = aMask << aShift;

    DISABLE_ALL_INTERRUPTS();

    /* Preserve the bits outside of the mask. Make all bits inside of the 
       mask "0". */
    tmpVal = *(DigOut[aBankId].shadowPtr) & ~tmpMask;

    /* Ensure any bits are set outside of the mask are "0" and update the new
       value */
    tmpVal |= (aValue << aShift) & tmpMask; 

    *(DigOut[aBankId].shadowPtr) = tmpVal;
    *(DigOut[aBankId].hwPtr) = tmpVal;

    ENABLE_ALL_INTERRUPTS();
}

//--------------------------------------------------------------------------
// Module:
//  DO_SetAllBits
//
///   This function sets all of the bits of the specified digital output bank.
///
///   \param aBankId - id of the digital output bank
/// 
//--------------------------------------------------------------------------
void DO_SetAllBits(eDigOutBankId aBankId)
{

    DISABLE_ALL_INTERRUPTS();

    *(DigOut[aBankId].shadowPtr) = 0xFFFF;
    *(DigOut[aBankId].hwPtr) = 0xFFFF;

    ENABLE_ALL_INTERRUPTS();
}

//--------------------------------------------------------------------------
// Module:
//  DO_SetBits
//
///   This function sets all of the bits of the specified digital output bank
///   with the mask value. Those bits not set in the mask maintain their
///   current state.
///
///   \param aBankId - id of the digital output bank
///   \param aMask - bit mask of the bits to be set 
/// 
//--------------------------------------------------------------------------
EXPORT void DO_SetBits(eDigOutBankId aBankId, UINT_16 aMask)
{
    UINT_16 tmpVal;

    DISABLE_ALL_INTERRUPTS();

    tmpVal = *(DigOut[aBankId].shadowPtr) | aMask;

    *(DigOut[aBankId].shadowPtr) = tmpVal;
    *(DigOut[aBankId].hwPtr) = tmpVal;

    ENABLE_ALL_INTERRUPTS();
}

//--------------------------------------------------------------------------
// Module:
//  DO_SetBitWithId
//
///   This function sets the bit of the specified digital output bank
///   based on the id passed into the function. 
///
///   \param aBitId - id of the bit to set in a digital output bank
/// 
//--------------------------------------------------------------------------
void DO_SetBitWithId(eDigOutBitId aBitId)
{
    UINT_16 tmpVal;
    UINT_16 bankId;

    DISABLE_ALL_INTERRUPTS();

    /* Update bank 0 */
    if (aBitId <= DIGOUT_BIT07)
    {
        tmpVal = (1 << aBitId);
        bankId = 0;
    }
    /* Update bank 1 */
    else if (aBitId <= DIGOUT_BIT23)
    {
        tmpVal = (1 << (aBitId - DIGOUT_BIT08));
        bankId = 1;
    }
    /* Update bank 2 */
    else if (aBitId <= DIGOUT_BIT31)
    {
        tmpVal = (1 << (aBitId - PROT_00));
        bankId = 2;
    }
    /* Update bank 3 */
    else
    {
        tmpVal = (1 << (aBitId - DISP_00));
        bankId = 3;
    }

    //DISABLE_ALL_INTERRUPTS();

    *(DigOut[bankId].shadowPtr) |= tmpVal;
	/* Set equal to the updated shadow register instead of "|="
	   in case hardware register does not maintain previous value */
    *(DigOut[bankId].hwPtr) = *(DigOut[bankId].shadowPtr);

    ENABLE_ALL_INTERRUPTS();
}

//--------------------------------------------------------------------------
// Module:
//  DO_SetBitsWithId
//
///   This function sets all of the bits of the specified digital output bank
///   based on the ids passed into the function. In order for the updating to
///   terminate, the array must be terminated with NUM_DIGOUT_BITS.
///
///   \param aBitId[] - id(s) of the bits to set in a digital output bank(s)
/// 
//--------------------------------------------------------------------------
void DO_SetBitsWithId(eDigOutBitId aBitId[])
{
    UINT_16 tmpVal[NUM_DIGOUT_BANKS];
    UINT_16 i;

    /* Initialize the scratch banks */
    for (i = 0; i < NUM_DIGOUT_BANKS; i++)
    {
        tmpVal[i] = 0x0000;
    }


    i = 0;
    /* Scan through all of the ids until NUM_DIGOUT_BITS is reached */
    while (aBitId[i] < NUM_DIGOUT_BITS)
    {
        /* Update bank 0 */
        if (aBitId[i] <= DIGOUT_BIT07)
        {
            tmpVal[0] |= (1 << aBitId[i]);
        }
        /* Update bank 1 */
        else if (aBitId[i] <= DIGOUT_BIT23)
        {
            tmpVal[1] |= (1 << (aBitId[i] - DIGOUT_BIT08));
        }
        /* Update bank 2 */
        else if (aBitId[i] <= DIGOUT_BIT31)
        {
            tmpVal[2] |= (1 << (aBitId[i] - PROT_00));
        }
        /* Update bank 3 */
        else
        {
            tmpVal[3] |= (1 << (aBitId[i] - DISP_00));
        }
        i++;
    }

    DISABLE_ALL_INTERRUPTS();

    for (i = 0; i < NUM_DIGOUT_BANKS; i++)
    {
        *(DigOut[i].shadowPtr) |= tmpVal[i];
		/* Set equal to the updated shadow register instead of "|="
		   in case hardware register does not maintain previous value */
        *(DigOut[i].hwPtr) = *(DigOut[i].shadowPtr);
    }

    ENABLE_ALL_INTERRUPTS();
}


//--------------------------------------------------------------------------
// Module:
//  DO_ResetAllBits
//
///   This function resets all of the bits of the specified digital output bank.
///
///   \param aBankId - id of the digital output bank
/// 
//--------------------------------------------------------------------------
void DO_ResetAllBits(eDigOutBankId aBankId)
{

    DISABLE_ALL_INTERRUPTS();

    *(DigOut[aBankId].shadowPtr) = 0x0000;
    *(DigOut[aBankId].hwPtr) = 0x0000;

    ENABLE_ALL_INTERRUPTS();
}

//--------------------------------------------------------------------------
// Module:
//  DO_ResetBits
//
///   This function resets all of the bits of the specified digital output bank
///   with the mask value. Those bits not set in the mask maintain their
///   current state. Note: In order to reset a bit, the bit in the mask must
///   be set to a "1".
///
///   \param aBankId - id of the digital output bank
///   \param aMask - bit mask of the bits to be reset 
/// 
//--------------------------------------------------------------------------
void DO_ResetBits(eDigOutBankId aBankId, UINT_16 aMask)
{
    UINT_16 tmpVal;

    DISABLE_ALL_INTERRUPTS();

    tmpVal = *(DigOut[aBankId].shadowPtr) & ~aMask;

    *(DigOut[aBankId].shadowPtr) = tmpVal;
    *(DigOut[aBankId].hwPtr) = tmpVal;

    ENABLE_ALL_INTERRUPTS();
}

//--------------------------------------------------------------------------
// Module:
//  DO_ResetBitWithId
//
///   This function resets the bit of the specified digital output bank
///   based on the id passed into the function. 
///
///   \param aBitId - id of the bit to reset in a digital output bank
/// 
//--------------------------------------------------------------------------
void DO_ResetBitWithId(eDigOutBitId aBitId)
{
    UINT_16 tmpVal;
    UINT_16 bankId;

    DISABLE_ALL_INTERRUPTS();

    /* Update bank 0 */
    if (aBitId <= DIGOUT_BIT07)
    {
        tmpVal = ~(1 << aBitId);
        bankId = 0;
    }
    /* Update bank 1 */
    else if (aBitId <= DIGOUT_BIT23)
    {
        tmpVal = ~(1 << (aBitId - DIGOUT_BIT08));
        bankId = 1;
    }
    /* Update bank 2 */
    else if (aBitId <= DIGOUT_BIT31)
    {
        tmpVal = ~(1 << (aBitId - PROT_00));
        bankId = 2;
    }
    /* Update bank 3 */
    else
    {
        tmpVal = ~(1 << (aBitId - DISP_00));
        bankId = 3;
    }


    *(DigOut[bankId].shadowPtr) &= tmpVal;
	/* Set equal to the updated shadow register instead of "&="
	   in case hardware register does not maintain previous value */
    *(DigOut[bankId].hwPtr) = *(DigOut[bankId].shadowPtr);

    ENABLE_ALL_INTERRUPTS();
}


//--------------------------------------------------------------------------
// Module:
//  DO_ResetBitsWithId
//
///   This function resets all of the bits of the specified digital output bank
///   based on the ids passed into the function. In order for the updating to
///   terminate, the array must be terminated with NUM_DIGOUT_BITS.
///
///   \param aBitId[] - id(s) of the bits to reset in a digital output bank(s)
/// 
//--------------------------------------------------------------------------
void DO_ResetBitsWithId(eDigOutBitId aBitId[])
{
    UINT_16 tmpVal[NUM_DIGOUT_BANKS];
    UINT_16 i;

    /* Initialize the scratch banks */
    for (i = 0; i < NUM_DIGOUT_BANKS; i++)
    {
        tmpVal[i] = 0xFFFF;
    }

    i = 0;
    /* Scan through all of the ids until NUM_DIGOUT_BITS is reached */
    while (aBitId[i] < NUM_DIGOUT_BITS)
    {
        /* Update bank 0 */
        if (aBitId[i] <= DIGOUT_BIT07)
        {
            tmpVal[0] &= ~(1 << aBitId[i]);
        }
        /* Update bank 1 */
        else if (aBitId[i] <= DIGOUT_BIT23)
        {
            tmpVal[1] &= ~(1 << (aBitId[i] - DIGOUT_BIT08));
        }
        /* Update bank 2 */
        else if (aBitId[i] <= DIGOUT_BIT31)
        {
            tmpVal[2] &= ~(1 << (aBitId[i] - PROT_00));
        }
        /* Update bank 3 */
        else
        {
            tmpVal[3] &= ~(1 << (aBitId[i] - DISP_00));
        }
        i++;
    }

    DISABLE_ALL_INTERRUPTS();

    for (i = 0; i < NUM_DIGOUT_BANKS; i++)
    {
        *(DigOut[i].shadowPtr) &= tmpVal[i];
		/* Set equal to the updated shadow register instead of "&="
		   in case hardware register does not maintain previous value */
        *(DigOut[i].hwPtr) = *(DigOut[i].shadowPtr);
    }

    ENABLE_ALL_INTERRUPTS();
}

//--------------------------------------------------------------------------
// Module:
//  DO_ReadBankState
//
///   This function reads the current state of the output bank and returns the
///   result.
///
///   \param aBankId - id of the bank to be read
///
///   \return UINT_16 - current state of the output bank 
/// 
//--------------------------------------------------------------------------
UINT_16 DO_ReadBankState(eDigOutBankId aBankId)
{
	UINT_16 bankState;
	
	DISABLE_ALL_INTERRUPTS();

    bankState = *(DigOut[aBankId].shadowPtr);

    ENABLE_ALL_INTERRUPTS();

	return bankState;

}


//--------------------------------------------------------------------------
// Module:
//  DO_ReadBitState
//
///   This function reads the current state of the output bit and returns the
///   result. The returned result will be non-zero if the bit is set. If the bit
///   is set, the returned result will reflect the bit position of the bit
///   in the bank. For example, if the bit is and in position 14, the returned 
///   result will be 0x4000.
///
///   \param aBitId - id of the bit to be read
///
///   \return UINT_16 - the state of the desired bit
///                     
/// 
//--------------------------------------------------------------------------
UINT_16 DO_ReadBitState(eDigOutBitId aBitId)
{
    UINT_16 bankState;
    UINT_16 bankId;
	UINT_16 mask;

    /* Update bank 0 */
    if (aBitId <= DIGOUT_BIT07)
    {
        bankId = 0;
		mask = 1 << aBitId;
    }
    /* Update bank 1 */
    else if (aBitId <= DIGOUT_BIT23)
    {
        bankId = 1;
		mask = 1 << (aBitId - DIGOUT_BIT08);
    }
    /* Update bank 2 */
    else if (aBitId <= DIGOUT_BIT31)
    {
        bankId = 2;
		mask = 1 << (aBitId - PROT_00);
    }
    /* Update bank 3 */
    else
    {
        bankId = 3;
		mask = 1 << (aBitId - DISP_00);
    }

    DISABLE_ALL_INTERRUPTS();

    bankState = *(DigOut[bankId].shadowPtr) & mask;

    ENABLE_ALL_INTERRUPTS();

	return bankState;

}


