/******************************************************************************
*
* Copyright (C) 2014-15 Bombardier
*
* File Name: Digital.c
*
* Revision History:
*   12/31/2014 - das - Created
*
******************************************************************************/
///   \file
///   This file contains the code that services Digital I/O 
#define digital_C


/*--------------------------------------------------------------------------
                              INCLUDE FILES
  --------------------------------------------------------------------------*/
#include "types.h"
#include "mvbvar.h"
#include "DigIn.h"
#include "DigOut.h"

/*--------------------------------------------------------------------------
                             MODULE CONSTANTS
  --------------------------------------------------------------------------*/
#define VCUHD_658_BIT_16			0x8000      /* DIGOUT_BIT00  */
#define VCUHD_658_BIT_17			0x4000      /* DIGOUT_BIT01  */
#define VCUHD_658_BIT_18			0x2000      /* DIGOUT_BIT02  */
#define VCUHD_658_BIT_19			0x1000      /* DIGOUT_BIT03  */
#define VCUHD_658_BIT_20			0x0800      /* DIGOUT_BIT04  */
#define VCUHD_658_BIT_21			0x0400      /* DIGOUT_BIT05  */
#define VCUHD_658_BIT_22			0x0200      /* DIGOUT_BIT06  */
#define VCUHD_658_BIT_23			0x0100      /* DIGOUT_BIT07  */
#define VCUHD_658_BIT_24			0x0080      /* DIGOUT_BIT08  */
#define VCUHD_658_BIT_25			0x0040      /* DIGOUT_BIT09  */
#define VCUHD_658_BIT_26			0x0020      /* DIGOUT_BIT10  */
#define VCUHD_658_BIT_27			0x0010      /* DIGOUT_BIT11  */
#define VCUHD_658_BIT_28			0x0008      /* DIGOUT_BIT12  */
#define VCUHD_658_BIT_29			0x0004      /* DIGOUT_BIT13  */
#define VCUHD_658_BIT_30			0x0002      /* DIGOUT_BIT14  */
#define VCUHD_658_BIT_31			0x0001      /* DIGOUT_BIT15  */

#define VCUHD_658_BIT_32			0x8000      /* DIGOUT_BIT16  */
#define VCUHD_658_BIT_33			0x4000      /* DIGOUT_BIT17  */
#define VCUHD_658_BIT_34			0x2000      /* DIGOUT_BIT18  */
#define VCUHD_658_BIT_35			0x1000      /* DIGOUT_BIT19  */
#define VCUHD_658_BIT_36			0x0800      /* DIGOUT_BIT20  */
#define VCUHD_658_BIT_37			0x0400      /* DIGOUT_BIT21  */
#define VCUHD_658_BIT_38			0x0200      /* DIGOUT_BIT22  */
#define VCUHD_658_BIT_39			0x0100      /* DIGOUT_BIT23  */
#define VCUHD_658_BIT_40			0x0080      /* DIGOUT_BIT24  */
#define VCUHD_658_BIT_41			0x0040      /* DIGOUT_BIT25  */
#define VCUHD_658_BIT_42			0x0020      /* DIGOUT_BIT26  */
#define VCUHD_658_BIT_43			0x0010      /* DIGOUT_BIT27  */
#define VCUHD_658_BIT_44			0x0008      /* DIGOUT_BIT28  */
#define VCUHD_658_BIT_45			0x0004      /* DIGOUT_BIT29  */
#define VCUHD_658_BIT_46			0x0002      /* DIGOUT_BIT30  */
#define VCUHD_658_BIT_47			0x0001      /* DIGOUT_BIT31  */

#define HDVCU_650_BIT_16			0x8000      /* DIGIN_BIT00  */
#define HDVCU_650_BIT_17			0x4000      /* DIGIN_BIT01  */
#define HDVCU_650_BIT_18			0x2000      /* DIGIN_BIT02  */
#define HDVCU_650_BIT_19			0x1000      /* DIGIN_BIT03  */
#define HDVCU_650_BIT_20			0x0800      /* DIGIN_BIT04  */
#define HDVCU_650_BIT_21			0x0400      /* DIGIN_BIT05  */
#define HDVCU_650_BIT_22			0x0200      /* DIGIN_BIT06  */
#define HDVCU_650_BIT_23			0x0100      /* DIGIN_BIT07  */
#define HDVCU_650_BIT_24			0x0080      /* DIGIN_BIT08  */
#define HDVCU_650_BIT_25			0x0040      /* DIGIN_BIT09  */
#define HDVCU_650_BIT_26			0x0020      /* DIGIN_BIT10  */
#define HDVCU_650_BIT_27			0x0010      /* DIGIN_BIT11  */
#define HDVCU_650_BIT_28			0x0008      /* DIGIN_BIT12  */
#define HDVCU_650_BIT_29			0x0004      /* DIGIN_BIT13  */
#define HDVCU_650_BIT_30			0x0002      /* DIGIN_BIT14  */
#define HDVCU_650_BIT_31			0x0001      /* DIGIN_BIT15  */

#define HDVCU_650_BIT_32			0x8000      /* DIGIN_BIT16  */
#define HDVCU_650_BIT_33			0x4000      /* DIGIN_BIT17  */
#define HDVCU_650_BIT_34			0x2000      /* DIGIN_BIT18  */
#define HDVCU_650_BIT_35			0x1000      /* DIGIN_BIT19  */
#define HDVCU_650_BIT_36			0x0800      /* DIGIN_BIT20  */
#define HDVCU_650_BIT_37			0x0400      /* DIGIN_BIT21  */
#define HDVCU_650_BIT_38			0x0200      /* DIGIN_BIT22  */
#define HDVCU_650_BIT_39			0x0100      /* DIGIN_BIT23  */
#define HDVCU_650_BIT_40			0x0080      /* DIGIN_BIT24  */
#define HDVCU_650_BIT_41			0x0040      /* DIGIN_BIT25  */
#define HDVCU_650_BIT_42			0x0020      /* DIGIN_BIT26  */
#define HDVCU_650_BIT_43			0x0010      /* DIGIN_BIT27  */
#define HDVCU_650_BIT_44			0x0008      /* DIGIN_BIT28  */
#define HDVCU_650_BIT_45			0x0004      /* DIGIN_BIT29  */
#define HDVCU_650_BIT_46			0x0002      /* DIGIN_BIT30  */
#define HDVCU_650_BIT_47			0x0001      /* DIGIN_BIT31  */

#define HDVCU_650_BIT_48			0x8000      /* DIGIN_BIT32  */
#define HDVCU_650_BIT_49			0x4000      /* DIGIN_BIT33  */
#define HDVCU_650_BIT_50			0x2000      /* DIGIN_BIT34  */
#define HDVCU_650_BIT_51			0x1000      /* DIGIN_BIT35  */
#define HDVCU_650_BIT_52			0x0800      /* DIGIN_BIT36  */
#define HDVCU_650_BIT_53			0x0400      /* DIGIN_BIT37  */
#define HDVCU_650_BIT_54			0x0200      /* DIGIN_BIT38  */
#define HDVCU_650_BIT_55			0x0100      /* DIGIN_BIT39  */
#define HDVCU_650_BIT_56			0x0080      /* DIGIN_BIT40  */
#define HDVCU_650_BIT_57			0x0040      /* DIGIN_BIT41  */
#define HDVCU_650_BIT_58			0x0020      /* DIGIN_BIT42  */
#define HDVCU_650_BIT_59			0x0010      /* DIGIN_BIT43  */
#define HDVCU_650_BIT_60			0x0008      /* DIGIN_BIT44  */
#define HDVCU_650_BIT_61			0x0004      /* DIGIN_BIT45  */
#define HDVCU_650_BIT_62			0x0002      /* DIGIN_BIT46  */
#define HDVCU_650_BIT_63			0x0001      /* DIGIN_BIT47  */

/*--------------------------------------------------------------------------
                              MODULE MACROS
  --------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
                             MODULE DATA TYPES
  --------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
                              MODULE VARIABLES
  --------------------------------------------------------------------------*/

static UINT_16 m_DebouncedBank  = 0;
static UINT_16 m_DebouncedBank1 = 0;
static UINT_16 m_DebouncedBank2 = 0;
static UINT_16 m_DebouncedBank3 = 0;

/*--------------------------------------------------------------------------
                             MODULE PROTOTYPES
  --------------------------------------------------------------------------*/

//--------------------------------------------------------------------------
// Module:
//  ReadDigitalInputsUpdateMvb
//
///   This function reads the states of the digital inputs and populates
///   the appropriate MVB variables.
///
//--------------------------------------------------------------------------
void ReadDigitalInputsUpdateMvb (void)
{
    UINT_16 value;
    volatile UINT_16 changed;
    volatile UINT_16 newBank;
    UINT_16 mask[] = {0xFFFF, 0xFFFF, 0xFFFF, 0xFFFF};

    /**************************************************************************
    * Digital input BANK3  (ISO_DIN_0 .. ISO_DIN_1)
    **************************************************************************/
    DI_ReadBank (DIGIN_BANK3, mask[DIGIN_BANK3]);
    newBank = DI_GetCurrent (DIGIN_BANK3);
    changed = DI_GetBitChanged (DIGIN_BANK3);

    /* The 1st condition preserves the bits that went from 0 - 1 - 0 or
       from 1 - 0 - 1 in the previous 3 samples, including the current.
       The 2nd condition saves all bits that remained in the same
       state from the previous sample and zeros those that have changed.
    */
    m_DebouncedBank3 = (changed & m_DebouncedBank3) | (~changed & newBank);

    value = 0;
    /* Mask out DIGIN_BIT00 .. DIGIN_BIT01 values from Bank 3 */
    value |= (m_DebouncedBank3 & 0x0040) ? HDVCU_650_BIT_16 : 0;
    value |= (m_DebouncedBank3 & 0x0080) ? HDVCU_650_BIT_17 : 0;

    /**************************************************************************
    * Digital input BANK0  (ISO_DIN_2 .. ISO_DIN_15)
    **************************************************************************/
    DI_ReadBank (DIGIN_BANK0, mask[DIGIN_BANK0]);
    newBank = DI_GetCurrent (DIGIN_BANK0);
    changed = DI_GetBitChanged (DIGIN_BANK0);

    /* The 1st condition preserves the bits that went from 0 - 1 - 0 or
       from 1 - 0 - 1 in the previous 3 samples, including the current.
       The 2nd condition saves all bits that remained in the same
       state from the previous sample and zeros those that have changed.
    */
    m_DebouncedBank = (changed & m_DebouncedBank) | (~changed & newBank);

    /* Mask out DIGIN_BIT02 .. DIGIN_BIT15 values from Bank 0 */
    value |= (m_DebouncedBank & 0x0004) ? HDVCU_650_BIT_18 : 0;
    value |= (m_DebouncedBank & 0x0008) ? HDVCU_650_BIT_19 : 0;
    value |= (m_DebouncedBank & 0x0010) ? HDVCU_650_BIT_20 : 0;
    value |= (m_DebouncedBank & 0x0020) ? HDVCU_650_BIT_21 : 0;
    value |= (m_DebouncedBank & 0x0040) ? HDVCU_650_BIT_22 : 0;
    value |= (m_DebouncedBank & 0x0080) ? HDVCU_650_BIT_23 : 0;
    value |= (m_DebouncedBank & 0x0100) ? HDVCU_650_BIT_24 : 0;
    value |= (m_DebouncedBank & 0x0200) ? HDVCU_650_BIT_25 : 0;
    value |= (m_DebouncedBank & 0x0400) ? HDVCU_650_BIT_26 : 0;
    value |= (m_DebouncedBank & 0x0800) ? HDVCU_650_BIT_27 : 0;
    value |= (m_DebouncedBank & 0x1000) ? HDVCU_650_BIT_28 : 0;
    value |= (m_DebouncedBank & 0x2000) ? HDVCU_650_BIT_29 : 0;
    value |= (m_DebouncedBank & 0x4000) ? HDVCU_650_BIT_30 : 0;
    value |= (m_DebouncedBank & 0x8000) ? HDVCU_650_BIT_31 : 0;

    /* Write value to MVB variable */
    HDVCU_650_16_BOOLEAN_ARRAY16 = value;

    /**************************************************************************
    * Digital input BANK1  (ISO_DIN_16 .. ISO_DIN_31)
    **************************************************************************/
    DI_ReadBank (DIGIN_BANK1, mask[DIGIN_BANK1]);
    newBank = DI_GetCurrent (DIGIN_BANK1);
    changed = DI_GetBitChanged (DIGIN_BANK1);

    /* The 1st condition preserves the bits that went from 0 - 1 - 0 or
       from 1 - 0 - 1 in the previous 3 samples, including the current.
       The 2nd condition saves all bits that remained in the same
       state from the previous sample and zeros those that have changed.
    */
    m_DebouncedBank1 = (changed & m_DebouncedBank1) | (~changed & newBank);

    value = 0;
    /* Mask out DIGIN_BIT16 .. DIGIN_BIT31 values from Bank 1 */
    value |= (m_DebouncedBank1 & 0x0001) ? HDVCU_650_BIT_32 : 0;
    value |= (m_DebouncedBank1 & 0x0002) ? HDVCU_650_BIT_33 : 0;
    value |= (m_DebouncedBank1 & 0x0004) ? HDVCU_650_BIT_34 : 0;
    value |= (m_DebouncedBank1 & 0x0008) ? HDVCU_650_BIT_35 : 0;
    value |= (m_DebouncedBank1 & 0x0010) ? HDVCU_650_BIT_36 : 0;
    value |= (m_DebouncedBank1 & 0x0020) ? HDVCU_650_BIT_37 : 0;
    value |= (m_DebouncedBank1 & 0x0040) ? HDVCU_650_BIT_38 : 0;
    value |= (m_DebouncedBank1 & 0x0080) ? HDVCU_650_BIT_39 : 0;
    value |= (m_DebouncedBank1 & 0x0100) ? HDVCU_650_BIT_40 : 0;
    value |= (m_DebouncedBank1 & 0x0200) ? HDVCU_650_BIT_41 : 0;
    value |= (m_DebouncedBank1 & 0x0400) ? HDVCU_650_BIT_42 : 0;
    value |= (m_DebouncedBank1 & 0x0800) ? HDVCU_650_BIT_43 : 0;
    value |= (m_DebouncedBank1 & 0x1000) ? HDVCU_650_BIT_44 : 0;
    value |= (m_DebouncedBank1 & 0x2000) ? HDVCU_650_BIT_45 : 0;
    value |= (m_DebouncedBank1 & 0x4000) ? HDVCU_650_BIT_46 : 0;
    value |= (m_DebouncedBank1 & 0x8000) ? HDVCU_650_BIT_47 : 0;

    /* Write value to MVB variable */
    HDVCU_650_32_BOOLEAN_ARRAY16 = value;

    /**************************************************************************
    * Digital input BANK2  (NISO_DIN_0 .. NISO_DIN_15)
    **************************************************************************/
    DI_ReadBank (DIGIN_BANK2, mask[DIGIN_BANK2]);
    newBank = DI_GetCurrent (DIGIN_BANK2);
    changed = DI_GetBitChanged (DIGIN_BANK2);

    /* The 1st condition preserves the bits that went from 0 - 1 - 0 or
       from 1 - 0 - 1 in the previous 3 samples, including the current.
       The 2nd condition saves all bits that remained in the same
       state from the previous sample and zeros those that have changed
    */
    m_DebouncedBank2 = (changed & m_DebouncedBank2) | (~changed & newBank);

    value = 0;
    /* Mask out DIGIN_BIT32 .. DIGIN_BIT47 values from Bank 2 */
    value |= (m_DebouncedBank2 & 0x0001) ? HDVCU_650_BIT_48 : 0;
    value |= (m_DebouncedBank2 & 0x0002) ? HDVCU_650_BIT_49 : 0;
    value |= (m_DebouncedBank2 & 0x0004) ? HDVCU_650_BIT_50 : 0;
    value |= (m_DebouncedBank2 & 0x0008) ? HDVCU_650_BIT_51 : 0;
    value |= (m_DebouncedBank2 & 0x0010) ? HDVCU_650_BIT_52 : 0;
    value |= (m_DebouncedBank2 & 0x0020) ? HDVCU_650_BIT_53 : 0;
    value |= (m_DebouncedBank2 & 0x0040) ? HDVCU_650_BIT_54 : 0;
    value |= (m_DebouncedBank2 & 0x0080) ? HDVCU_650_BIT_55 : 0;
    value |= (m_DebouncedBank2 & 0x0100) ? HDVCU_650_BIT_56 : 0;
    value |= (m_DebouncedBank2 & 0x0200) ? HDVCU_650_BIT_57 : 0;
    value |= (m_DebouncedBank2 & 0x0400) ? HDVCU_650_BIT_58 : 0;
    value |= (m_DebouncedBank2 & 0x0800) ? HDVCU_650_BIT_59 : 0;
    value |= (m_DebouncedBank2 & 0x1000) ? HDVCU_650_BIT_60 : 0;
    value |= (m_DebouncedBank2 & 0x2000) ? HDVCU_650_BIT_61 : 0;
    value |= (m_DebouncedBank2 & 0x4000) ? HDVCU_650_BIT_62 : 0;
    value |= (m_DebouncedBank2 & 0x8000) ? HDVCU_650_BIT_63 : 0;

    /* Write value to MVB variable */
    HDVCU_650_48_BOOLEAN_ARRAY16 = value;

}

//--------------------------------------------------------------------------
// Module:
//  ReadMvbUpdateDigitalOutputs
//
///   This function updates all digital outputs from the values in the 
///   MVB variables. It will set the outputs to default/safe states
///   when MVB communication is down
///
///   \param aCommunicationOk - TRUE if MVB communication OK; FALSE otherwise
///
//--------------------------------------------------------------------------
void ReadMvbUpdateDigitalOutputs (BOOLEAN aCommunicationOk)
{
    UINT_16 value;

	if (aCommunicationOk == TRUE)
    {
        /**************************************************************************
        * Digital output BANK0  (ISO_DOUT_0 .. ISO_DOUT_7)
        **************************************************************************/
        value = 0;

        value |= ((VCUHD_658_16_BOOLEAN_ARRAY16 & VCUHD_658_BIT_16) == 0)
                 ? (0 << DIGOUT_BIT00) : (1 << DIGOUT_BIT00);

        value |= ((VCUHD_658_16_BOOLEAN_ARRAY16 & VCUHD_658_BIT_17) == 0)
                 ? (0 << DIGOUT_BIT01) : (1 << DIGOUT_BIT01);

        value |= ((VCUHD_658_16_BOOLEAN_ARRAY16 & VCUHD_658_BIT_18) == 0)
                 ? (0 << DIGOUT_BIT02) : (1 << DIGOUT_BIT02);

        value |= ((VCUHD_658_16_BOOLEAN_ARRAY16 & VCUHD_658_BIT_19) == 0)
                 ? (0 << DIGOUT_BIT03) : (1 << DIGOUT_BIT03);

        value |= ((VCUHD_658_16_BOOLEAN_ARRAY16 & VCUHD_658_BIT_20) == 0)
                 ? (0 << DIGOUT_BIT04) : (1 << DIGOUT_BIT04);

        value |= ((VCUHD_658_16_BOOLEAN_ARRAY16 & VCUHD_658_BIT_21) == 0)
                 ? (0 << DIGOUT_BIT05) : (1 << DIGOUT_BIT05);

        value |= ((VCUHD_658_16_BOOLEAN_ARRAY16 & VCUHD_658_BIT_22) == 0)
                 ? (0 << DIGOUT_BIT06) : (1 << DIGOUT_BIT06);

        value |= ((VCUHD_658_16_BOOLEAN_ARRAY16 & VCUHD_658_BIT_23) == 0)
                 ? (0 << DIGOUT_BIT07) : (1 << DIGOUT_BIT07);

        /*value = (value | hidacDigitalOutputMaskOr) & hidacDigitalOutputMaskAnd;*/

        /* Write to bank 0 outputs */
        DO_WriteBank (DIGOUT_BANK0, value);

        /**************************************************************************
        * Digital output BANK1  (NISO_DOUT_8 .. NISO_DOUT_23)
        **************************************************************************/
        value = 0;

        value |= ((VCUHD_658_16_BOOLEAN_ARRAY16 & VCUHD_658_BIT_24) == 0)
                 ? (0 << (DIGOUT_BIT08 - DIGOUT_BIT08)) :
                 (1 << (DIGOUT_BIT08 - DIGOUT_BIT08));

        value |= ((VCUHD_658_16_BOOLEAN_ARRAY16 & VCUHD_658_BIT_25) == 0)
                 ? (0 << (DIGOUT_BIT09 - DIGOUT_BIT08)) :
                 (1 << (DIGOUT_BIT09 - DIGOUT_BIT08));

        value |= ((VCUHD_658_16_BOOLEAN_ARRAY16 & VCUHD_658_BIT_26) == 0)
                 ? (0 << (DIGOUT_BIT10 - DIGOUT_BIT08)) :
                 (1 << (DIGOUT_BIT10 - DIGOUT_BIT08));

        value |= ((VCUHD_658_16_BOOLEAN_ARRAY16 & VCUHD_658_BIT_27) == 0)
                 ? (0 << (DIGOUT_BIT11 - DIGOUT_BIT08)) :
                 (1 << (DIGOUT_BIT11 - DIGOUT_BIT08));

        value |= ((VCUHD_658_16_BOOLEAN_ARRAY16 & VCUHD_658_BIT_28) == 0)
                 ? (0 << (DIGOUT_BIT12 - DIGOUT_BIT08)) :
                 (1 << (DIGOUT_BIT12 - DIGOUT_BIT08));

        value |= ((VCUHD_658_16_BOOLEAN_ARRAY16 & VCUHD_658_BIT_29) == 0)
                 ? (0 << (DIGOUT_BIT13 - DIGOUT_BIT08)) :
                 (1 << (DIGOUT_BIT13 - DIGOUT_BIT08));

        value |= ((VCUHD_658_16_BOOLEAN_ARRAY16 & VCUHD_658_BIT_30) == 0)
                 ? (0 << (DIGOUT_BIT14 - DIGOUT_BIT08)) :
                 (1 << (DIGOUT_BIT14 - DIGOUT_BIT08));

        value |= ((VCUHD_658_16_BOOLEAN_ARRAY16 & VCUHD_658_BIT_31) == 0)
                 ? (0 << (DIGOUT_BIT15 - DIGOUT_BIT08)) :
                 (1 << (DIGOUT_BIT15 - DIGOUT_BIT08));

        value |= ((VCUHD_658_32_BOOLEAN_ARRAY16 & VCUHD_658_BIT_32) == 0)
                 ? (0 << (DIGOUT_BIT16 - DIGOUT_BIT08)) :
                 (1 << (DIGOUT_BIT16 - DIGOUT_BIT08));

        value |= ((VCUHD_658_32_BOOLEAN_ARRAY16 & VCUHD_658_BIT_33) == 0)
                 ? (0 << (DIGOUT_BIT17 - DIGOUT_BIT08)) :
                 (1 << (DIGOUT_BIT17 - DIGOUT_BIT08));

        value |= ((VCUHD_658_32_BOOLEAN_ARRAY16 & VCUHD_658_BIT_34) == 0)
                 ? (0 << (DIGOUT_BIT18 - DIGOUT_BIT08)) :
                 (1 << (DIGOUT_BIT18 - DIGOUT_BIT08));

        value |= ((VCUHD_658_32_BOOLEAN_ARRAY16 & VCUHD_658_BIT_35) == 0)
                 ? (0 << (DIGOUT_BIT19 - DIGOUT_BIT08)) :
                 (1 << (DIGOUT_BIT19 - DIGOUT_BIT08));

        value |= ((VCUHD_658_32_BOOLEAN_ARRAY16 & VCUHD_658_BIT_36) == 0)
                 ? (0 << (DIGOUT_BIT20 - DIGOUT_BIT08)) :
                 (1 << (DIGOUT_BIT20 - DIGOUT_BIT08));

        value |= ((VCUHD_658_32_BOOLEAN_ARRAY16 & VCUHD_658_BIT_37) == 0)
                 ? (0 << (DIGOUT_BIT21 - DIGOUT_BIT08)) :
                 (1 << (DIGOUT_BIT21 - DIGOUT_BIT08));

        value |= ((VCUHD_658_32_BOOLEAN_ARRAY16 & VCUHD_658_BIT_38) == 0)
                 ? (0 << (DIGOUT_BIT22 - DIGOUT_BIT08)) :
                 (1 << (DIGOUT_BIT22 - DIGOUT_BIT08));

        value |= ((VCUHD_658_32_BOOLEAN_ARRAY16 & VCUHD_658_BIT_39) == 0)
                 ? (0 << (DIGOUT_BIT23 - DIGOUT_BIT08)) :
                 (1U << (DIGOUT_BIT23 - DIGOUT_BIT08));


        /* Write to bank 1 outputs */
        DO_WriteBank (DIGOUT_BANK1, value);

        /**************************************************************************
        * Digital output BANK2  (NISO_DOUT_24 .. NISO_DOUT_31)
        **************************************************************************/
        value = DO_ReadBankState (DIGOUT_BANK2);

        if ((VCUHD_658_32_BOOLEAN_ARRAY16 & VCUHD_658_BIT_40) == 0)
        {
            DO_ResetBitWithId (DIGOUT_BIT24);
        }
        else
        {
            DO_SetBitWithId (DIGOUT_BIT24);
        }
        if ((VCUHD_658_32_BOOLEAN_ARRAY16 & VCUHD_658_BIT_41) == 0)
        {
            DO_ResetBitWithId (DIGOUT_BIT25);
        }
        else
        {
            DO_SetBitWithId (DIGOUT_BIT25);
        }
        if ((VCUHD_658_32_BOOLEAN_ARRAY16 & VCUHD_658_BIT_42) == 0)
        {
            DO_ResetBitWithId (DIGOUT_BIT26);
        }
        else
        {
            DO_SetBitWithId (DIGOUT_BIT26);
        }
        if ((VCUHD_658_32_BOOLEAN_ARRAY16 & VCUHD_658_BIT_43) == 0)
        {
            DO_ResetBitWithId (DIGOUT_BIT27);
        }
        else
        {
            DO_SetBitWithId (DIGOUT_BIT27);
        }
        if ((VCUHD_658_32_BOOLEAN_ARRAY16 & VCUHD_658_BIT_44) == 0)
        {
            DO_ResetBitWithId (DIGOUT_BIT28);
        }
        else
        {
            DO_SetBitWithId (DIGOUT_BIT28);
        }
        if ((VCUHD_658_32_BOOLEAN_ARRAY16 & VCUHD_658_BIT_45) == 0)
        {
            DO_ResetBitWithId (DIGOUT_BIT29);
        }
        else
        {
            DO_SetBitWithId (DIGOUT_BIT29);
        }
        if ((VCUHD_658_32_BOOLEAN_ARRAY16 & VCUHD_658_BIT_46) == 0)
        {
            DO_ResetBitWithId (DIGOUT_BIT30);
        }
        else
        {
            DO_SetBitWithId (DIGOUT_BIT30);
        }
        if ((VCUHD_658_32_BOOLEAN_ARRAY16 & VCUHD_658_BIT_47) == 0)
        {
            DO_ResetBitWithId (DIGOUT_BIT31);
        }
        else
        {
            DO_SetBitWithId (DIGOUT_BIT31);
        }
    }
    else
    {
        /* Communication lost with VCUC */
        /* Reset all digital outputs */

        /**************************************************************************
        * Digital output BANK0  (ISO_DOUT_0 .. ISO_DOUT_7)
        **************************************************************************/

        /* Reset bank 0 outputs */
        value = 0;
        DO_WriteBank (DIGOUT_BANK0, value);

        /**************************************************************************
        * Digital output BANK1  (NISO_DOUT_8 .. NISO_DOUT_23)
        **************************************************************************/

        /* Set bank 1 outputs */
        value = 0xFFFF;
        DO_WriteBank (DIGOUT_BANK1, value);

        /**************************************************************************
        * Digital output BANK2  (NISO_DOUT_24 .. NISO_DOUT_31)
        **************************************************************************/

        /* Set bank 2 outputs */
        DO_SetBitWithId (DIGOUT_BIT24);
        DO_SetBitWithId (DIGOUT_BIT25);
        DO_SetBitWithId (DIGOUT_BIT26);
        DO_SetBitWithId (DIGOUT_BIT27);
        DO_SetBitWithId (DIGOUT_BIT28);
        DO_SetBitWithId (DIGOUT_BIT29);
        DO_SetBitWithId (DIGOUT_BIT30);
        DO_SetBitWithId (DIGOUT_BIT31);
    }
}
