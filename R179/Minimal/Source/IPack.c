/******************************************************************************
*
* Copyright (C) 2014-15 Bombardier
*
* File Name: IPack.c
*
* Revision History:
*   08/31/2014 - das - Created
*
******************************************************************************/
///   \file
///   This file contains the code that initializes the MVB IPACK

/*--------------------------------------------------------------------------
                              INCLUDE FILES
  --------------------------------------------------------------------------*/
#ifndef _DEBUG
#include <reg167.h>
#endif

#ifndef _DC
#define O_DC
#endif

#include <stdio.h>
#include "Types.h"
#include "Sercomm.h"
#include "Digital.h"
#include "TaskMvb.h"

#include "cssmisc.h"
#include "tcn_api.h"

struct STR_TCN_LC_INIT as_lc_init;

/*--------------------------------------------------------------------------
                             MODULE CONSTANTS
  --------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------
                              MODULE MACROS
  --------------------------------------------------------------------------*/
#define RST_IP_N				(volatile UINT_16 huge *)(0x00500022L)
#define IPACK_A_ADRS			0x00400000L
#define IPACK_A_IO_REG_CSR		(volatile UINT_16 huge *)(IPACK_A_ADRS + 0x40000)

#define IPACK_A_SCR_DEFAULT		(volatile UINT_16 *)(IPACK_A_ADRS + 0x3f80)
#define IPACK_A_MCR_DEFAULT		(volatile UINT_16 *)(IPACK_A_ADRS + 0x3f84)
#define IPACK_A_DPR_DEFAULT		(volatile UINT_16 *)(IPACK_A_ADRS + 0x3fA8)

#define IPACK_A_SCR_256K_TS		(volatile UINT_16 *)(IPACK_A_ADRS + 0xff80)
#define IPACK_A_MCR_256K_TS		(volatile UINT_16 *)(IPACK_A_ADRS + 0xff84)
#define IPACK_A_DR_256K_TS		(volatile UINT_16 *)(IPACK_A_ADRS + 0xff88)
#define IPACK_A_DPR_256K_TS		(volatile UINT_16 *)(IPACK_A_ADRS + 0xffA8)


#define IPACK_A_PIT_100H_TS		(volatile UINT_16 *)(IPACK_A_ADRS + 0x0100)
#define IPACK_A_PIT_102H_TS		(volatile UINT_16 *)(IPACK_A_ADRS + 0x0102)
#define IPACK_A_PIT_104H_TS		(volatile UINT_16 *)(IPACK_A_ADRS + 0x0104)
#define IPACK_A_PIT_106H_TS		(volatile UINT_16 *)(IPACK_A_ADRS + 0x0106)
#define IPACK_A_PIT_108H_TS		(volatile UINT_16 *)(IPACK_A_ADRS + 0x0108)
#define IPACK_A_PIT_10AH_TS		(volatile UINT_16 *)(IPACK_A_ADRS + 0x010A)
#define IPACK_A_PIT_10CH_TS		(volatile UINT_16 *)(IPACK_A_ADRS + 0x010C)
#define IPACK_A_PIT_10EH_TS		(volatile UINT_16 *)(IPACK_A_ADRS + 0x010E)
#define IPACK_A_PIT_110H_TS		(volatile UINT_16 *)(IPACK_A_ADRS + 0x0110)
#define IPACK_A_PIT_112H_TS		(volatile UINT_16 *)(IPACK_A_ADRS + 0x0112)
#define IPACK_A_PIT_114H_TS		(volatile UINT_16 *)(IPACK_A_ADRS + 0x0114)
#define IPACK_A_PIT_116H_TS		(volatile UINT_16 *)(IPACK_A_ADRS + 0x0116)
#define IPACK_A_PIT_118H_TS		(volatile UINT_16 *)(IPACK_A_ADRS + 0x0118)
#define IPACK_A_PIT_11AH_TS		(volatile UINT_16 *)(IPACK_A_ADRS + 0x011A)
#define IPACK_A_PIT_11CH_TS		(volatile UINT_16 *)(IPACK_A_ADRS + 0x011C)
#define IPACK_A_PIT_11EH_TS		(volatile UINT_16 *)(IPACK_A_ADRS + 0x011E)

#define IPACK_A_PCS0_80H_TS		(volatile UINT_16 *)(IPACK_A_ADRS + 0x30000 + 0x90 + 0)
#define IPACK_A_PCS1_80H_TS		(volatile UINT_16 *)(IPACK_A_ADRS + 0x30000 + 0x90 + 2)
#define IPACK_A_PCS2_80H_TS		(volatile UINT_16 *)(IPACK_A_ADRS + 0x30000 + 0x90 + 4)
#define IPACK_A_PCS3_80H_TS		(volatile UINT_16 *)(IPACK_A_ADRS + 0x30000 + 0x90 + 6)

#define IPACK_A_LA_DA_VP0_80H_TS	(volatile UINT_16 *)(IPACK_A_ADRS + 0x10000 + 0x0110)
#define IPACK_A_LA_DA_VP1_80H_TS	(volatile UINT_16 *)(IPACK_A_ADRS + 0x10000 + 0x0130)

#define IPACK_A_LA_DA_VP0_8AH_TS	(volatile UINT_16 *)(IPACK_A_ADRS + 0x10000 + 0x0100)
#define IPACK_A_LA_DA_VP1_8AH_TS	(volatile UINT_16 *)(IPACK_A_ADRS + 0x10000 + 0x0120)

/*--------------------------------------------------------------------------
                             MODULE DATA TYPES
  --------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------
                              MODULE VARIABLES
  --------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------
                             MODULE PROTOTYPES
  --------------------------------------------------------------------------*/
extern UINT_16 mvb_init (void);
extern void mvb_fast_init (void);


//--------------------------------------------------------------------------
// Module:
//  ReleaseIpackHw
//
///   This function releases the IPack module.
///
//--------------------------------------------------------------------------
void ReleaseIpackHw (void)
{
    *RST_IP_N &= 0xDFFF;
}


//--------------------------------------------------------------------------
// Module:
//  InitMVBIpack
//
///   This function releases the IPack module.
///
//--------------------------------------------------------------------------
void InitMVBIpack (void)
{
    UINT_16 result = 1;
    UINT_16 temp;
    UINT_16 temp1, temp2, temp3, temp4, temp5, temp6, temp7;
    UINT_16 waitCounter = 0;
    char str[60];

    /* MVB hardware init */
    *IPACK_A_IO_REG_CSR = 0x0047; /* Set in Class 2-4 */
    *IPACK_A_IO_REG_CSR = 0x0046; /* Send reset command */

    /* Insert delay */
    for (waitCounter = 0; waitCounter < 32000; waitCounter++)
    {
        temp = temp;
    }

    sprintf (str, "****************************************\n \r");
    SC_PutsAlways (str);

    /* Init MVBC internal registers */
    /* Configure SCR - IL to Configure Mode */
    temp = *IPACK_A_SCR_DEFAULT;
    temp = temp | 0x0001;
    *IPACK_A_SCR_DEFAULT = temp;
    for (waitCounter = 0; waitCounter < 10; waitCounter++)
    {
        temp1 = *IPACK_A_SCR_DEFAULT;
        sprintf (str, "MVBC DEFAULT SCR= 0x%X\n \r", temp1);
        SC_PutsAlways (str);
    }


    /* Insert delay */
    for (waitCounter = 0; waitCounter < 32000; waitCounter++)
    {
        temp = temp;
    }

    /* Check that MVBC is addressed and not memory */
    for (waitCounter = 0; waitCounter < 10; waitCounter++)
    {
        *IPACK_A_DPR_DEFAULT = 0xFFFF;
        temp = *IPACK_A_DPR_DEFAULT;
        sprintf (str, "MVBC DEFAULT DPR= 0x%X\n \r", temp);
        SC_PutsAlways (str);
    }

    /* Configure MCR - 256k TS */
    temp = *IPACK_A_MCR_DEFAULT;
    sprintf (str, "MVBC DEFAULT MCR= 0x%X\n \r", temp);
    SC_PutsAlways (str);
    *IPACK_A_MCR_DEFAULT = temp | 0x0003;
    for (waitCounter = 0; waitCounter < 10; waitCounter++)
    {
        waitCounter = waitCounter;
    }
    temp = *IPACK_A_MCR_256K_TS;
    sprintf (str, "MVBC 256K MCR= 0x%X\n \r", temp);
    SC_PutsAlways (str);


    /* Insert delay */
    for (waitCounter = 0; waitCounter < 32000; waitCounter++)
    {
        temp = temp;
    }

    /* Check that MVBC is addressed and not memory */
    *IPACK_A_DPR_256K_TS = 0xFFFF;
    temp = *IPACK_A_DPR_256K_TS;
    if (temp != 0xFFFC)
    {
        sprintf (str, "MVBC 256K DPR mismatch W/R = 0x%X 0xFFFC\n \r", temp);
        SC_PutsAlways (str);
    }
    else
    {
        sprintf (str, "MVBC 256K DPR= 0x%X\n \r", temp);
        SC_PutsAlways (str);
    }

    for (waitCounter = 0; waitCounter < 10; waitCounter++)
    {
        temp = *IPACK_A_SCR_256K_TS;
        temp1 = *IPACK_A_MCR_256K_TS;
        sprintf (str, "Before: SCR= 0x%X   MCR= 0x%X\n \r", temp, temp1);
        SC_PutsAlways (str);
    }

    /* Initialize MVB software */
    result = mvb_init();


    for (waitCounter = 0; waitCounter < 10; waitCounter++)
    {
        temp = *IPACK_A_SCR_256K_TS;
        temp1 = *IPACK_A_MCR_256K_TS;
        sprintf (str, "After: SCR= 0x%X   MCR= 0x%X\n \r", temp, temp1);
        SC_PutsAlways (str);
    }

    /* Force MVBC into Arbitration mode 00 & 3 Wait State*/
    temp = *IPACK_A_SCR_256K_TS & 0xff3f;
    *IPACK_A_SCR_256K_TS = temp;
    temp1 = *IPACK_A_SCR_256K_TS;
    if (temp != temp1)
    {
        sprintf (str, "MVBC First SCR mismatch W/R = 0x%X 0x%X\n \r", temp, temp1);
        SC_PutsAlways (str);
    }
    else
    {
        sprintf (str, "MVBC First SCR= 0x%X\n \r", temp);
        SC_PutsAlways (str);
    }

    /* Check PIT */
    temp  = *IPACK_A_PIT_100H_TS;
    temp1 = *IPACK_A_PIT_102H_TS;
    temp2 = *IPACK_A_PIT_104H_TS;
    temp3 = *IPACK_A_PIT_106H_TS;
    temp4 = *IPACK_A_PIT_108H_TS;
    temp5 = *IPACK_A_PIT_10AH_TS;
    temp6 = *IPACK_A_PIT_10CH_TS;
    temp7 = *IPACK_A_PIT_10EH_TS;
    sprintf (str, "0X7000100: 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X\n \r", temp, temp1, temp2, temp3, temp4, temp5, temp6, temp7);
    SC_PutsAlways (str);
    temp  = *IPACK_A_PIT_110H_TS;
    temp1 = *IPACK_A_PIT_112H_TS;
    temp2 = *IPACK_A_PIT_114H_TS;
    temp3 = *IPACK_A_PIT_116H_TS;
    temp4 = *IPACK_A_PIT_118H_TS;
    temp5 = *IPACK_A_PIT_11AH_TS;
    temp6 = *IPACK_A_PIT_11CH_TS;
    temp7 = *IPACK_A_PIT_11EH_TS;
    sprintf (str, "0X7000110: 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X 0x%X\n \r", temp, temp1, temp2, temp3, temp4, temp5, temp6, temp7);
    SC_PutsAlways (str);

    /* Check PCS for port 0x80 (128) */
    temp1 = *IPACK_A_PCS0_80H_TS;
    temp2 = *IPACK_A_PCS1_80H_TS;
    temp3 = *IPACK_A_PCS2_80H_TS;
    temp4 = *IPACK_A_PCS3_80H_TS;
    sprintf (str, "\n \rPCS0(80H): 0x%X\n \r", temp1);
    SC_PutsAlways (str);
    sprintf (str, "PCS1(80H): 0x%X\n \r", temp2);
    SC_PutsAlways (str);
    sprintf (str, "PCS2(80H): 0x%X\n \r", temp3);
    SC_PutsAlways (str);
    sprintf (str, "PCS3(80H): 0x%X\n \r", temp4);
    SC_PutsAlways (str);

    /* Check Data Area for port 0x80 (128) */
    temp1 = *IPACK_A_LA_DA_VP0_80H_TS;
    sprintf (str, "LA-DATA(80H, VP=0): 0x%X\n \r", temp1);
    SC_PutsAlways (str);
    temp2 = *IPACK_A_LA_DA_VP1_80H_TS;
    sprintf (str, "LA-DATA(80H, VP=1): 0x%X\n \r", temp2);
    SC_PutsAlways (str);

    /* Initialize MVB accessor functions */
    mvb_fast_init();

    /* Send diagnostic data to serial port */
    sprintf (str, "\n \rMVB Init result = %d \n \r", result);
    SC_PutsAlways (str);

    temp = *IPACK_A_SCR_256K_TS;
    temp1 = *IPACK_A_SCR_256K_TS;
    if (temp != temp1)
    {
        sprintf (str, "MVBC Second SCR mismatch W/R = 0x%X 0x%X\n \r", temp, temp1);
        SC_PutsAlways (str);
    }
    else
    {
        sprintf (str, "MVBC Second SCR= 0x%X\n \r", temp);
        SC_PutsAlways (str);
    }

    temp = *IPACK_A_MCR_256K_TS;
    temp1 = *IPACK_A_MCR_256K_TS;
    if (temp != temp1)
    {
        sprintf (str, "MVBC MCR mismatch = 0x%X 0x%X\n \r", temp, temp1);
        SC_PutsAlways (str);
    }
    else
    {
        sprintf (str, "MVBC MCR= 0x%X\n \r", temp);
        SC_PutsAlways (str);
    }

    /* Check DR register */
    temp1 = *IPACK_A_DR_256K_TS;
    sprintf (str, "MVBC DR= 0x%X\n \r", temp1);
    SC_PutsAlways (str);

    /* Set EMD mode (Trafo) */
    *IPACK_A_DR_256K_TS = temp1 | 0x0020;

    temp1 = *IPACK_A_DR_256K_TS;
    sprintf (str, "MVBC DR= 0x%X\n \r", temp1);
    SC_PutsAlways (str);

    SetMvbReady (TRUE);

    /* check for CLK configuration */
    sprintf (str, "*** RSTCON *****************************\n \r");
    SC_PutsAlways (str);
    temp = P0H;
    sprintf (str, "P0H = 0x%X \n \r", temp);
    SC_PutsAlways (str);
    sprintf (str, "*** RSTCON *****************************\n \r");
    SC_PutsAlways (str);
}
