/*****************************************************************************
*
* Copyright (C) 2010 Bombardier
*
* File Name: DigIn.h
*
* Abstract: Header file for the digital input driver
*
* Revision History:
*   08/31/2009 - das - Created
*
******************************************************************************/

#ifndef DIGIN_H
#define DIGIN_H

/*--------------------------------------------------------------------------
                             GLOBAL CONSTANTS
  --------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------
                               GLOBAL TYPES
  --------------------------------------------------------------------------*/
typedef enum
{
    DIGIN_00,	/* ISO_DIN0	: P1-D32 */
    DIGIN_01,	/* ISO_DIN1	: P1-Z32 */
    DIGIN_02,	/* ISO_DIN2	: P1-Z30 */
    DIGIN_03,	/* ISO_DIN3	: P1-D30 */
    DIGIN_04,	/* ISO_DIN4	: P1-D28 */
    DIGIN_05,	/* ISO_DIN5	: P1-Z28 */
    DIGIN_06,	/* ISO_DIN6	: P1-D26 */
    DIGIN_07,	/* ISO_DIN7	: P1-Z26 */
    DIGIN_08,	/* ISO_DIN8	: P1-D24 */
    DIGIN_09,	/* ISO_DIN9	: P1-Z24 */
    DIGIN_10,	/* ISO_DIN10: P1-D22 */
    DIGIN_11,	/* ISO_DIN11: P1-Z22 */
    DIGIN_12,	/* ISO_DIN12: P1-D20 */
    DIGIN_13,	/* ISO_DIN13: P1-Z20 */
    DIGIN_14,	/* ISO_DIN14: P1-D18 */
    DIGIN_15,	/* ISO_DIN15: P1-Z18 */
    DIGIN_16,	/* ISO_DIN16: P1-D16 */
    DIGIN_17,	/* ISO_DIN17: P1-Z16 */
    DIGIN_18,	/* ISO_DIN18: P1-D14 */
    DIGIN_19,	/* ISO_DIN19: P1-Z14 */
    DIGIN_20,	/* ISO_DIN20: P1-D12 */
    DIGIN_21,	/* ISO_DIN21: P1-Z12 */
    DIGIN_22,	/* ISO_DIN22: P1-D10 */
    DIGIN_23,	/* ISO_DIN23: P1-Z10 */
    DIGIN_24,	/* ISO_DIN24: P1-D8 */
    DIGIN_25,	/* ISO_DIN25: P1-Z8 */
    DIGIN_26,	/* ISO_DIN26: P1-D6 */
    DIGIN_27,	/* ISO_DIN27: P1-Z6 */
    DIGIN_28,	/* ISO_DIN28: P1-D4 */
    DIGIN_29,	/* ISO_DIN29: P1-Z4 */
    DIGIN_30,	/* ISO_DIN30: P1-D2 */
    DIGIN_31,	/* ISO_DIN31: P1-Z2 */
    DIGIN_32,	/* NISO_DIN0: P3-C25 */	
    DIGIN_33,	/* NISO_DIN1: P3-A24 */
    DIGIN_34,	/* NISO_DIN2: P3-C24 */
    DIGIN_35,	/* NISO_DIN3: P3-B24 */
    DIGIN_36,	/* NISO_DIN4: P3-A22 */
    DIGIN_37,	/* NISO_DIN5: P3-B22 */
    DIGIN_38,	/* NISO_DIN6: P3-C21 */
    DIGIN_39,	/* NISO_DIN7: P3-B21 */
    DIGIN_40,	/* NISO_DIN8: P3-B26 */
    DIGIN_41,	/* NISO_DIN9: P3-C26 */
    DIGIN_42,	/* NISO_DIN10: P3-A25 */
    DIGIN_43,	/* NISO_DIN11: P3-B25 */
    DIGIN_44,	/* NISO_DIN12: P3-A23 */
    DIGIN_45,	/* NISO_DIN13: P3-C23 */
    DIGIN_46,	/* NISO_DIN14: P3-B23 */
    DIGIN_47,	/* NISO_DIN15: P3-C22 */
    NUM_DIGINS

} eDigInId;

typedef enum
{
    DIGIN_BANK0,
    DIGIN_BANK1,
    DIGIN_BANK2,
    DIGIN_BANK3,
    NUM_DIGIN_BANKS
} eDigInBank;

typedef enum
{
    LOW,
    HIGH
} eDigInBitState;

/*--------------------------------------------------------------------------
                              GLOBAL VARIABLES
  --------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
                              GLOBAL PROTOTYPES
  --------------------------------------------------------------------------*/
EXPORT void DI_SetInvertMask(eDigInBank aBank, UINT_16 aInvertMask);
EXPORT void DI_ReadBanks(void);
EXPORT void DI_ReadBank(eDigInBank aBank, UINT_16 aMask);
EXPORT UINT_16 DI_GetCurrent(eDigInBank aBank);
EXPORT UINT_16 DI_GetCurrentWithMask(eDigInBank aBank, UINT_16 aMask);
EXPORT UINT_16 DI_GetPrevious(eDigInBank aBank);
EXPORT UINT_16 DI_GetPreviousWithMask(eDigInBank aBank, UINT_16 aMask);
EXPORT UINT_16 DI_GetBitChanged(eDigInBank aBank);
EXPORT UINT_16 DI_GetBitChangedWithMask(eDigInBank aBank, UINT_16 aMask);
EXPORT eDigInBitState DI_ReadBit(eDigInId aBit);

#ifdef _DEBUG
EXPORT void DI_SetBankValue(eDigInBank aBank, UINT_16 aValue);
#endif

#endif
