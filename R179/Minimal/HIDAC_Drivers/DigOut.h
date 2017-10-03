/*****************************************************************************
*
* Copyright (C) 2010 Bombardier
*
* File Name: DigOut.h
*
* Abstract: Header file for the digital output driver
*
* Revision History:
*   08/31/2009 - das - Created
*
******************************************************************************/
#ifndef DIGOUT_H
#define DIGOUT_H

/*--------------------------------------------------------------------------
                             GLOBAL CONSTANTS
  --------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
                               GLOBAL TYPES
  --------------------------------------------------------------------------*/
typedef enum 
{
    DIGOUT_BANK0,
    DIGOUT_BANK1,
    DIGOUT_BANK2,
    DIGOUT_BANK3,
    NUM_DIGOUT_BANKS
} eDigOutBankId;

typedef enum 
{
    /* Start: Bank 0 */
    DIGOUT_BIT00,					/* P2-B6 */
    DIGOUT_BIT01,					/* P2-D6 */
    DIGOUT_BIT02,					/* P2-Z8 */
    DIGOUT_BIT03,  /* P7 */			/* P2-B8 */
    DIGOUT_BIT04,					/* P2-D8 */
    DIGOUT_BIT05,					/* P2-D10 */
    DIGOUT_BIT06,					/* P2-Z10 */
    DIGOUT_BIT07,					/* P2-B10 */
    /* End: Bank 0 */

    /* Start: Bank 1 */
    DIGOUT_BIT08,					/* P2-Z12 */
    DIGOUT_BIT09,					/* P2-B12 */
    DIGOUT_BIT10,					/* P2-D14 */
    DIGOUT_BIT11,					/* P2-Z16 */
    DIGOUT_BIT12,					/* P2-D12 */
    DIGOUT_BIT13,					/* P2-Z14 */
    DIGOUT_BIT14,					/* P2-B14 */
    DIGOUT_BIT15,					/* P2-D16 */
    DIGOUT_BIT16,					/* P2-B16 */
    DIGOUT_BIT17,					/* P2-D18 */
    DIGOUT_BIT18,					/* P2-Z20 */
    DIGOUT_BIT19,					/* P2-B20 */
    DIGOUT_BIT20,					/* P2-Z18 */
    DIGOUT_BIT21,					/* P2-B18 */
    DIGOUT_BIT22,					/* P2-D20 */
    DIGOUT_BIT23,					/* P2-B22 */
    /* End: Bank 1 */

    /* Start: Bank 2 */
    PROT_00,						/* VDRIVE enable */
    PROT_01,						/* SSR_IN2 */
    PROT_02,						/* -15 V to Chart Recorder Interface board */
    PROT_03,						/* +15 V to Chart Recorder Interface board */
    PROT_04,						/* +5 V to Chart Recorder Interface board */
    PROT_05,						/* +15 V to ITU Interface board */
    PROT_06,						/* +5 V to ITU Interface board */
    PROT_07,						/* SSR_IN1 */
    DIGOUT_BIT24,					/* P2-D22 */
    DIGOUT_BIT25,				    /* P2-Z24 */
    DIGOUT_BIT26,					/* P2-B24 */
    DIGOUT_BIT27,					/* P2-Z26 */
    DIGOUT_BIT28,					/* P2-Z22 */
    DIGOUT_BIT29,					/* P2-D24 */
    DIGOUT_BIT30,					/* P2-B26 */
    DIGOUT_BIT31,					/* P3-A1 */
    /* End: Bank 2 */

    /* Start: Bank 3 */
    DISP_00,						/* J7-20 */
    DISP_01,						/* J7-7 */
    DISP_02,						/* J7-19 */
    DISP_03,						/* J7-6 */
    DISP_04,						/* J7-18 */
    DISP_05,						/* J7-5 */
    DISP_06,						/* J7-17 */
    DISP_07,						/* J7-4 */
    DISP_08,						/* J7-16 */
    DISP_09,						/* J7-3 */
    DISP_10,						/* J7-15 */
    DISP_11,						/* NO CONNECTION */
	UNUSED_BANK3_12,				/* NO CONNECTION */
	UNUSED_BANK3_13,				/* NO CONNECTION */
	TEST_LED,						/* Yellow LED (Front Stiffener) */
	FAIL_LED,						/* Red LED (Front Stiffener) */
    /* End: Bank 3 */

    NUM_DIGOUT_BITS

} eDigOutBitId;

/*--------------------------------------------------------------------------
                              GLOBAL VARIABLES
  --------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
                              GLOBAL PROTOTYPES
  --------------------------------------------------------------------------*/
EXPORT void DO_Init(UINT_8 aMask);
EXPORT void DO_WriteBank(eDigOutBankId aBankId, UINT_16 aValue);
EXPORT void DO_WriteBankWithMask(eDigOutBankId aBankId, UINT_16 aValue,
                          UINT_16 aMask);
EXPORT void DO_WriteBankWithMaskAndShift(eDigOutBankId aBankId, UINT_16 aValue,
                                  UINT_16 aMask, UINT_16 aShift);
EXPORT void DO_SetAllBits(eDigOutBankId aBankId);
EXPORT void DO_SetBits(eDigOutBankId aBankId, UINT_16 aMask);
EXPORT void DO_SetBitWithId(eDigOutBitId aBitId);
EXPORT void DO_SetBitsWithId(eDigOutBitId aBitId[]);
EXPORT void DO_ResetAllBits(eDigOutBankId aBankId);
EXPORT void DO_ResetBits(eDigOutBankId aBankId, UINT_16 aMask);
EXPORT void DO_ResetBitWithId(eDigOutBitId aBitId);
EXPORT void DO_ResetBitsWithId(eDigOutBitId aBitId[]);
EXPORT UINT_16 DO_ReadBankState(eDigOutBankId aBankId);
EXPORT UINT_16 DO_ReadBitState(eDigOutBitId aBitId);

#endif
