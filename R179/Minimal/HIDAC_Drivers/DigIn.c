/*****************************************************************************
*
* Copyright (C) 2010 Bombardier
*
* File Name: DigIn.c
*
* Revision History:
*   08/31/2009 - das - Created
*
******************************************************************************/
///   \file
///   This file contains functions that read the digital input banks

#define digin_C

/*--------------------------------------------------------------------------
                              INCLUDE FILES
  --------------------------------------------------------------------------*/
/* _DEBUG is defined in Microsoft Visual Studio 2008: UNIT TEST ONLY */
#ifndef _DEBUG
    #include <c166.h>
    #include <reg167.h>
#endif
#include "Types.h"
#include "DigIn.h"

/*--------------------------------------------------------------------------
                             MODULE CONSTANTS
  --------------------------------------------------------------------------*/
/*-------------------------------------------------*/
/*---------- User configurable parameters ---------*/
#define SAVE_PREVIOUS_STATE  /* define if application wishes to maintain
                                the previous state of the digital input bank */

#ifdef SAVE_PREVIOUS_STATE
    #define DETECT_BIT_CHANGES    /* define if application wishes to detect changes
                                 between the previous read and the current read */
#endif
/*-------------------------------------------------*/

/*--------------------------------------------------------------------------
                              MODULE MACROS
  --------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------
                             MODULE DATA TYPES
  --------------------------------------------------------------------------*/
typedef struct
{
    UINT_16  *hwPtr;         /* pointer to the hardware bank */
    UINT_16  invertMask;     /* if bit in mask is set, the bit is inverted */
    UINT_16  currentState;   /* current state of the digital input bank */
    UINT_16  previousState;  /* previous state of the digital input bank */
    UINT_16  bitChanged;     /* bit is set if the bit changed from the previous
                                sample */
} DigInBankInfo_t;

typedef struct
{
    eDigInBank  diginBank;   /* Bank Id */
    UINT_16     mask;        /* Bit location of the digital input in the bank */
} DigInBitToBank_t;

/*--------------------------------------------------------------------------
                              MODULE VARIABLES
  --------------------------------------------------------------------------*/
#ifdef _DEBUG
static UINT_16 DigInBankData[NUM_DIGIN_BANKS];
#endif

static DigInBankInfo_t DigInBankInfo[NUM_DIGIN_BANKS] =
{
#ifdef _DEBUG
    {&DigInBankData[0], 0, 0, 0, 0},
    {&DigInBankData[1], 0, 0, 0, 0},
    {&DigInBankData[2], 0, 0, 0, 0},
    {&DigInBankData[3], 0, 0, 0, 0}
#else
    {(UINT_16 *)(0x500000L), 0, 0, 0, 0},
    {(UINT_16 *)(0x500002L), 0, 0, 0, 0},
    {(UINT_16 *)(0x500004L), 0, 0, 0, 0},
    {(UINT_16 *)(0x500010L), 0, 0, 0, 0}
#endif
};

/* Note: The order must be maintained because this array is accessed by
   the eDigInId enumeration */
const static DigInBitToBank_t DigInMap[] = 
{
    {DIGIN_BANK3, 0x0040},  /* DIGIN_BIT00  ISO_DIN0:	P1-D32 */
    {DIGIN_BANK3, 0x0080},  /* DIGIN_BIT01  ISO_DIN1:	P1-Z32 */
    {DIGIN_BANK0, 0x0004},  /* DIGIN_BIT02  ISO_DIN2:	P1-Z30 */
    {DIGIN_BANK0, 0x0008},  /* DIGIN_BIT03  ISO_DIN3:	P1-D30 */
    {DIGIN_BANK0, 0x0010},  /* DIGIN_BIT04  ISO_DIN4:	P1-D28 */
    {DIGIN_BANK0, 0x0020},  /* DIGIN_BIT05  ISO_DIN5:	P1-Z28 */
    {DIGIN_BANK0, 0x0040},  /* DIGIN_BIT06  ISO_DIN6:	P1-D26 */
    {DIGIN_BANK0, 0x0080},  /* DIGIN_BIT07  ISO_DIN7:	P1-Z26 */
    {DIGIN_BANK0, 0x0100},  /* DIGIN_BIT08  ISO_DIN8:	P1-D24 */
    {DIGIN_BANK0, 0x0200},  /* DIGIN_BIT09  ISO_DIN9:	P1-Z24 */
    {DIGIN_BANK0, 0x0400},  /* DIGIN_BIT10  ISO_DIN10:	P1-D22 */
    {DIGIN_BANK0, 0x0800},  /* DIGIN_BIT11  ISO_DIN11:	P1-Z22 */
    {DIGIN_BANK0, 0x1000},  /* DIGIN_BIT12  ISO_DIN12:	P1-D20 */
    {DIGIN_BANK0, 0x2000},  /* DIGIN_BIT13  ISO_DIN13:	P1-Z20 */
    {DIGIN_BANK0, 0x4000},  /* DIGIN_BIT14  ISO_DIN14:	P1-D18 */
    {DIGIN_BANK0, 0x8000},  /* DIGIN_BIT15  ISO_DIN15:	P1-Z18 */
										
    {DIGIN_BANK1, 0x0001},  /* DIGIN_BIT16  ISO_DIN16:	P1-D16 */
    {DIGIN_BANK1, 0x0002},  /* DIGIN_BIT17  ISO_DIN17:	P1-Z16 */
    {DIGIN_BANK1, 0x0004},  /* DIGIN_BIT18  ISO_DIN18:	P1-D14 */
    {DIGIN_BANK1, 0x0008},  /* DIGIN_BIT19  ISO_DIN19:	P1-Z14 */
    {DIGIN_BANK1, 0x0010},  /* DIGIN_BIT20  ISO_DIN20:	P1-D12 */
    {DIGIN_BANK1, 0x0020},  /* DIGIN_BIT21  ISO_DIN21:	P1-Z12 */
    {DIGIN_BANK1, 0x0040},  /* DIGIN_BIT22  ISO_DIN22:	P1-D10 */
    {DIGIN_BANK1, 0x0080},  /* DIGIN_BIT23  ISO_DIN23:	P1-Z10 */
    {DIGIN_BANK1, 0x0100},  /* DIGIN_BIT24  ISO_DIN24:	P1-D8 */
    {DIGIN_BANK1, 0x0200},  /* DIGIN_BIT25  ISO_DIN25:	P1-Z8 */
    {DIGIN_BANK1, 0x0400},  /* DIGIN_BIT26  ISO_DIN26:	P1-D6 */
    {DIGIN_BANK1, 0x0800},  /* DIGIN_BIT27  ISO_DIN27:	P1-Z6 */
    {DIGIN_BANK1, 0x1000},  /* DIGIN_BIT28  ISO_DIN28:	P1-D4 */
    {DIGIN_BANK1, 0x2000},  /* DIGIN_BIT29  ISO_DIN29:	P1-Z4 */
    {DIGIN_BANK1, 0x4000},  /* DIGIN_BIT30  ISO_DIN30:	P1-D2 */
    {DIGIN_BANK1, 0x8000},  /* DIGIN_BIT31  ISO_DIN31:	P1-Z2 */

    {DIGIN_BANK2, 0x0001},  /* DIGIN_BIT32  NISO_DIN0: P3-C25 */
    {DIGIN_BANK2, 0x0002},  /* DIGIN_BIT33  NISO_DIN1: P3-A24 */
    {DIGIN_BANK2, 0x0004},  /* DIGIN_BIT34  NISO_DIN2: P3-C24 */
    {DIGIN_BANK2, 0x0008},  /* DIGIN_BIT35  NISO_DIN3: P3-B24 */
    {DIGIN_BANK2, 0x0010},  /* DIGIN_BIT36  NISO_DIN4: P3-A22 */
    {DIGIN_BANK2, 0x0020},  /* DIGIN_BIT37  NISO_DIN5: P3-B22 */
    {DIGIN_BANK2, 0x0040},  /* DIGIN_BIT38  NISO_DIN6: P3-C21 */
    {DIGIN_BANK2, 0x0080},  /* DIGIN_BIT39  NISO_DIN7: P3-B21 */
    {DIGIN_BANK2, 0x0100},  /* DIGIN_BIT40  NISO_DIN8: P3-B26 */
    {DIGIN_BANK2, 0x0200},  /* DIGIN_BIT41  NISO_DIN9: P3-C26 */
    {DIGIN_BANK2, 0x0400},  /* DIGIN_BIT42  NISO_DIN10: P3-A25 */
    {DIGIN_BANK2, 0x0800},  /* DIGIN_BIT43  NISO_DIN11: P3-B25 */
    {DIGIN_BANK2, 0x1000},  /* DIGIN_BIT44  NISO_DIN12: P3-A23 */
    {DIGIN_BANK2, 0x2000},  /* DIGIN_BIT45  NISO_DIN13: P3-C23 */
    {DIGIN_BANK2, 0x4000},  /* DIGIN_BIT46  NISO_DIN14: P3-B23 */
    {DIGIN_BANK2, 0x8000}   /* DIGIN_BIT47  NISO_DIN15: P3-C22 */

};

/*--------------------------------------------------------------------------
                             MODULE PROTOTYPES
  --------------------------------------------------------------------------*/

//--------------------------------------------------------------------------
// Module:
//  DI_Init
//   
///   This function sets the invert mask of a digital input bank. This allows 
///   the application software to invert the state of a digital input bit. 
/// 
///   \param aBank - digital input bank id
///   \param aInvertMask - "1" in a bit postion indicates the state of the 
///                        digital input bit will be inverted after every 
///                        read. 
/// 
//--------------------------------------------------------------------------
void DI_SetInvertMask(eDigInBank aBank, UINT_16 aInvertMask)
{
    DigInBankInfo[aBank].invertMask = aInvertMask;
}

//--------------------------------------------------------------------------
// Module:
//   DI_ReadBanks
//   
///   This function reads all of the digital input banks. 
/// 
//--------------------------------------------------------------------------
void DI_ReadBanks(void)
{
    eDigInBank i;

    for (i = 0; i < NUM_DIGIN_BANKS; i++)
    {
        DI_ReadBank(i,0xFFFF);
    }
}

//--------------------------------------------------------------------------
// Module:
//   DI_ReadBank
//   
///   This function reads a digital input bank. If SAVE_PREVIOUS_STATE is 
///   defined, it saves the previous state of the bank. It then reads the 
///   digital input bank and stores the value in currentState. If 
///   DETECT_BIT_CHANGES and SAVE_PREVIOUS_STATE are defined, it then 
///   detects any changes from the previous state (exculsive OR's the 
///   previousState and currentState) and stores it in bitChanged.
/// 
///   \param aBank - digital input bank id
/// 
//--------------------------------------------------------------------------
void DI_ReadBank(eDigInBank aBank, UINT_16 aMask)
{
	UINT_16 currentValue;

    /* Save the previous state state */
#ifdef SAVE_PREVIOUS_STATE
    DigInBankInfo[aBank].previousState = DigInBankInfo[aBank].currentState;
#endif

    /* Read the digital input bank and invert the use the invert mask to
       invert bits. Also, mask the care bits */
	currentValue =  (*(DigInBankInfo[aBank].hwPtr) ^ DigInBankInfo[aBank].invertMask) & aMask;

    DigInBankInfo[aBank].currentState = 
		(DigInBankInfo[aBank].currentState & ~aMask) | currentValue;

    /* Detect the individual bit changes from previous sample */
#if defined(DETECT_BIT_CHANGES) && defined(SAVE_PREVIOUS_STATE)
    DigInBankInfo[aBank].bitChanged = 
    DigInBankInfo[aBank].currentState ^ DigInBankInfo[aBank].previousState;
#endif

}

//--------------------------------------------------------------------------
// Module:
//   DI_GetCurrent
//   
///   This function reads the currentState attribute and returns it. 
/// 
///   \param aBank - the id of the bank 
///
///   \return UINT_16 - the current state of the digital input bank
/// 
//--------------------------------------------------------------------------
UINT_16 DI_GetCurrent(eDigInBank aBank)
{
    return DigInBankInfo[aBank].currentState;
}


//--------------------------------------------------------------------------
// Module:
//   DI_GetCurrentWithMask
//   
///   This function reads the currentState attribute, masks it and returns 
///   the value. 
/// 
///   \param aBank - the id of the bank 
///   \param aMask -  "0" in any bit postion indicates that that bit 
///                   is a don't care. 
///   \return UINT_16 - the current state of the digital input bank
/// 
//--------------------------------------------------------------------------
UINT_16 DI_GetCurrentWithMask(eDigInBank aBank, UINT_16 aMask)
{
    return DigInBankInfo[aBank].currentState & aMask;
}

//--------------------------------------------------------------------------
// Module:
//   DI_GetPreviousWithMask
//   
///   This function reads the previousState attribute and returns it. 
/// 
///   \param aBank - the id of the bank 
/// 
///   \return UINT_16 - the previous state of the digital input bank
/// 
//--------------------------------------------------------------------------
UINT_16 DI_GetPrevious(eDigInBank aBank)
{
    return DigInBankInfo[aBank].previousState;
}

//--------------------------------------------------------------------------
// Module:
//   DI_GetPreviousWithMask
//   
///   This function reads the previousState attribute, masks it and 
///   returns the value. 
/// 
///   \param aBank - the id of the bank 
///   \param aMask -  "0" in any bit postion indicates that that bit 
///                   is a don't care. 
///   \return UINT_16 - the previous state of the digital input bank
/// 
//--------------------------------------------------------------------------
UINT_16 DI_GetPreviousWithMask(eDigInBank aBank, UINT_16 aMask)
{
    return DigInBankInfo[aBank].previousState & aMask;
}

//--------------------------------------------------------------------------
// Module:
//   DI_GetBitChanged
//   
///   This function reads the bitChanged attribute and returns it. The bitChanged 
///   attribute informs the calling function what bits changed state between 
///   the current read of the digital input bank and the previous read of the 
///   digital input bank.
/// 
///   \param aBank - the id of the bank 
///
///   \return UINT_16 - "1" in any bit position indicates a changed state
/// 
//--------------------------------------------------------------------------
UINT_16 DI_GetBitChanged(eDigInBank aBank)
{
    return DigInBankInfo[aBank].bitChanged;
}

//--------------------------------------------------------------------------
// Module:
//   DI_GetBitChangedWithMask
//   
///   This function reads the bitChanged attribute and returns it. The bitChanged 
///   attribute informs the calling function what bits changed state between 
///   the current read of the digital input bank and the previous read of the 
///   digital input bank.
/// 
///   \param aBank - the id of the bank 
///   \param aMask -  "0" in any bit postion indicates that a bit change is a 
///                    don't care. 
///   \return UINT_16 - "1" in any bit position indicates a changed state
/// 
//--------------------------------------------------------------------------
UINT_16 DI_GetBitChangedWithMask(eDigInBank aBank, UINT_16 aMask)
{
    return DigInBankInfo[aBank].bitChanged & aMask;
}

//--------------------------------------------------------------------------
// Module:
//   DI_ReadBit
//   
///   This function reads the state of a digital input bit and returns it. 
///   Note: This function reads the currentState member of DigInBankInfo[];
///   therefore the bank associated with the digital input should be read 
///   prior to calling this function. 
/// 
///   \param aBit - the id of the digital iuput bit
///   \return eDigInBitState - HIGH (1) or LOW (0)
/// 
//--------------------------------------------------------------------------
eDigInBitState DI_ReadBit(eDigInId aBit)
{
    /* Get the bank data and mask it */
    if (DigInBankInfo[DigInMap[aBit].diginBank].currentState & DigInMap[aBit].mask)
    {
        return HIGH;
    }
    else
    {
        return LOW;
    }
}

#ifdef _DEBUG
void DI_SetBankValue(eDigInBank aBank, UINT_16 aValue)
{
    DigInBankData[aBank] = aValue;
}
#endif 