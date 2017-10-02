/******************************************************************************
*
* Copyright (C) 2014-15 Bombardier
*
* File Name: SpareISR.c
*
* Revision History:
*   08/31/2014 - das - Created
*
******************************************************************************/
///   \file
///   This file contains the code that handles the Battery RAM data and health
///   checks for the Real Time Clock.

#define spareisr_C

/*--------------------------------------------------------------------------
                              INCLUDE FILES
  --------------------------------------------------------------------------*/
#ifndef _DEBUG
#include <reg167.h>
#endif
#include "Types.h"
#include "DateTime.h"
#include "VEC_TBL.h"

/*--------------------------------------------------------------------------
                             MODULE CONSTANTS
  --------------------------------------------------------------------------*/
#define UNEXPECTED_ISR_LOG_SIZE				10

/*--------------------------------------------------------------------------
                              MODULE MACROS
  --------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
                             MODULE DATA TYPES
  --------------------------------------------------------------------------*/
typedef struct
{
	UINT_16 unexpectedIntr;
	UINT_16 month;
	UINT_16 day;
	UINT_16 year;
	UINT_16 hour;
	UINT_16 minute;
	UINT_16 second;
} UnExpectedISR_t;

/*--------------------------------------------------------------------------
                              MODULE VARIABLES
  --------------------------------------------------------------------------*/
static RTC_t rtc;

#pragma class fb=NonVolatileData
#pragma noclear
static UINT_16 unExpectedISRCount;
static UnExpectedISR_t unExpectedISR[UNEXPECTED_ISR_LOG_SIZE];

#pragma default_attributes
#pragma clear
/*--------------------------------------------------------------------------
                             MODULE PROTOTYPES
  --------------------------------------------------------------------------*/
static void ServiceSpuriousISR(UINT_16 aId);


//--------------------------------------------------------------------------
// Module:
//  SpareISRVector01
//
///   This function handles a spurious interrupt at Vector 01
///
//--------------------------------------------------------------------------
interrupt (0x01) using ( SPAREISR_RB ) void SpareISRVector01(void)
{
	ServiceSpuriousISR(0x01);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector02
//
///   This function handles a spurious interrupt at Vector 02 (NMI)
///
//--------------------------------------------------------------------------
interrupt (0x02) using ( SPAREISR_RB ) void SpareISRVector02(void)
{
	NMI = 0;
	ServiceSpuriousISR(0x02);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector03
//
///   This function handles a spurious interrupt at Vector 03
///
//--------------------------------------------------------------------------
interrupt (0x03) using ( SPAREISR_RB ) void SpareISRVector03(void)
{
	ServiceSpuriousISR(0x03);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector04
//
///   This function handles a spurious interrupt at Vector 04 (Stack
///   Overflow)
///
//--------------------------------------------------------------------------
interrupt (0x04) using ( SPAREISR_RB ) void SpareISRVector04(void)
{
	STKOF = 0;
	ServiceSpuriousISR(0x04);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector05
//
///   This function handles a spurious interrupt at Vector 05
///
//--------------------------------------------------------------------------
interrupt (0x05) using ( SPAREISR_RB ) void SpareISRVector05(void)
{
	ServiceSpuriousISR(0x05);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector06
//
///   This function handles a spurious interrupt at Vector 06 (Stack
///   Underflow)
///
//--------------------------------------------------------------------------
interrupt (0x06) using ( SPAREISR_RB ) void SpareISRVector06(void)
{
	STKUF = 0;
	ServiceSpuriousISR(0x06);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector07
//
///   This function handles a spurious interrupt at Vector 07
///
//--------------------------------------------------------------------------
interrupt (0x07) using ( SPAREISR_RB ) void SpareISRVector07(void)
{
	ServiceSpuriousISR(0x07);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector08
//
///   This function handles a spurious interrupt at Vector 08
///
//--------------------------------------------------------------------------
interrupt (0x08) using ( SPAREISR_RB ) void SpareISRVector08(void)
{
	ServiceSpuriousISR(0x08);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector09
//
///   This function handles a spurious interrupt at Vector 09
///
//--------------------------------------------------------------------------
interrupt (0x09) using ( SPAREISR_RB ) void SpareISRVector09(void)
{
	ServiceSpuriousISR(0x09);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector0a
//
///   This function handles a spurious interrupt at Vector 0a
///
//--------------------------------------------------------------------------
interrupt (0x0a) using ( SPAREISR_RB ) void SpareISRVector0a(void)
{

    if ( UNDOPC )
    {
        // undefined_opcode++;
        UNDOPC = 0 ;
    }

    if ( PRTFLT )
    {
        // protected_instruction_fault++;
        PRTFLT = 0 ;
    }

    if ( ILLOPA )
    {
        // illegal_word_operand_access++;
        ILLOPA = 0;
    }

    if ( ILLINA )
    {
        // illegal_instruction_access++;
        ILLINA = 0;
    }

    if ( ILLBUS )
    {
        // illegal_external_bus_access++;
        ILLBUS = 0;
    }

	ServiceSpuriousISR(0x0a);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector0b
//
///   This function handles a spurious interrupt at Vector 0b
///
//--------------------------------------------------------------------------
interrupt (0x0b) using ( SPAREISR_RB ) void SpareISRVector0b(void)
{
	ServiceSpuriousISR(0x0b);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector0c
//
///   This function handles a spurious interrupt at Vector 0c
///
//--------------------------------------------------------------------------
interrupt (0x0c) using ( SPAREISR_RB ) void SpareISRVector0c(void)
{
	ServiceSpuriousISR(0x0c);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector0d
//
///   This function handles a spurious interrupt at Vector 0d
///
//--------------------------------------------------------------------------
interrupt (0x0d) using ( SPAREISR_RB ) void SpareISRVector0d(void)
{
	ServiceSpuriousISR(0x0d);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector0e
//
///   This function handles a spurious interrupt at Vector 0e
///
//--------------------------------------------------------------------------
interrupt (0x0e) using ( SPAREISR_RB ) void SpareISRVector0e(void)
{
	ServiceSpuriousISR(0x0e);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector0f
//
///   This function handles a spurious interrupt at Vector 0f
///
//--------------------------------------------------------------------------
interrupt (0x0f) using ( SPAREISR_RB ) void SpareISRVector0f(void)
{
	ServiceSpuriousISR(0x0f);
}



/*----------------------------------------------------------------------------
   Module:
    SpareISRVector10
  
     This function handles a spurious interrupt at Vector 10
----------------------------------------------------------------------------*/
interrupt (CAPCOM_REGISTER_0) using ( SPAREISR_RB ) void SpareISRVector10(void)
{
	ServiceSpuriousISR(CAPCOM_REGISTER_0);
}


//--------------------------------------------------------------------------
// Module:
//  SpareISRVector11
//
///   This function handles a spurious interrupt at Vector 11
///
//--------------------------------------------------------------------------
interrupt (CAPCOM_REGISTER_1) using ( SPAREISR_RB ) void SpareISRVector11(void)
{
	ServiceSpuriousISR(CAPCOM_REGISTER_1);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector12
//
///   This function handles a spurious interrupt at Vector 12
///
//--------------------------------------------------------------------------
interrupt (CAPCOM_REGISTER_2) using ( SPAREISR_RB ) void SpareISRVector12(void)
{
	ServiceSpuriousISR(CAPCOM_REGISTER_2);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector13
//
///   This function handles a spurious interrupt at Vector 13
///
//--------------------------------------------------------------------------
interrupt (CAPCOM_REGISTER_3) using ( SPAREISR_RB ) void SpareISRVector13(void)
{
	ServiceSpuriousISR(CAPCOM_REGISTER_3);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector14
//
///   This function handles a spurious interrupt at Vector 14
///
//--------------------------------------------------------------------------
interrupt (CAPCOM_REGISTER_4) using ( SPAREISR_RB ) void SpareISRVector14(void)
{
	ServiceSpuriousISR(CAPCOM_REGISTER_4);
}

#ifdef OCCUPIED_ISR

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector15
//
///   This function handles a spurious interrupt at Vector 15
///
//--------------------------------------------------------------------------
interrupt (CAPCOM_REGISTER_5) using ( SPAREISR_RB ) void SpareISRVector15(void)
{
	ServiceSpuriousISR(CAPCOM_REGISTER_5);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector16
//
///   This function handles a spurious interrupt at Vector 16
///
//--------------------------------------------------------------------------
interrupt (CAPCOM_REGISTER_6) using ( SPAREISR_RB ) void SpareISRVector16(void)
{
	ServiceSpuriousISR(CAPCOM_REGISTER_6);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector17
//
///   This function handles a spurious interrupt at Vector 17
///
//--------------------------------------------------------------------------
interrupt (CAPCOM_REGISTER_7) using ( SPAREISR_RB ) void SpareISRVector17(void)
{
	ServiceSpuriousISR(CAPCOM_REGISTER_7);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector18
//
///   This function handles a spurious interrupt at Vector 18
///
//--------------------------------------------------------------------------
interrupt (CAPCOM_REGISTER_8) using ( SPAREISR_RB ) void SpareISRVector18(void)
{
	ServiceSpuriousISR(CAPCOM_REGISTER_8);
}
#endif
//--------------------------------------------------------------------------
// Module:
//  SpareISRVector19
//
///   This function handles a spurious interrupt at Vector 19
///
//--------------------------------------------------------------------------
interrupt (CAPCOM_REGISTER_9) using ( SPAREISR_RB ) void SpareISRVector19(void)
{
	ServiceSpuriousISR(CAPCOM_REGISTER_9);
}

#ifdef OCCUPIED_ISR
//--------------------------------------------------------------------------
// Module:
//  SpareISRVector1a
//
///   This function handles a spurious interrupt at Vector 1a
///
//--------------------------------------------------------------------------
interrupt (CAPCOM_REGISTER_10) using ( SPAREISR_RB ) void SpareISRVector1a(void)
{
	ServiceSpuriousISR(CAPCOM_REGISTER_10);
}
#endif
//--------------------------------------------------------------------------
// Module:
//  SpareISRVector1b
//
///   This function handles a spurious interrupt at Vector 1b
///
//--------------------------------------------------------------------------
interrupt (CAPCOM_REGISTER_11) using ( SPAREISR_RB ) void SpareISRVector1b(void)
{
	ServiceSpuriousISR(CAPCOM_REGISTER_11);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector1c
//
///   This function handles a spurious interrupt at Vector 1c
///
//--------------------------------------------------------------------------
interrupt (CAPCOM_REGISTER_12) using ( SPAREISR_RB ) void SpareISRVector1c(void)
{
	ServiceSpuriousISR(CAPCOM_REGISTER_12);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector1d
//
///   This function handles a spurious interrupt at Vector 1d
///
//--------------------------------------------------------------------------
interrupt (CAPCOM_REGISTER_13) using ( SPAREISR_RB ) void SpareISRVector1d(void)
{
	ServiceSpuriousISR(CAPCOM_REGISTER_13);
}
#ifdef OCCUPIED_ISR
//--------------------------------------------------------------------------
// Module:
//  SpareISRVector1e
//
///   This function handles a spurious interrupt at Vector 1e
///
//--------------------------------------------------------------------------
interrupt (CAPCOM_REGISTER_14) using ( SPAREISR_RB ) void SpareISRVector1e(void)
{
	ServiceSpuriousISR(CAPCOM_REGISTER_14);
}
#endif

#ifdef OCCUPIED_ISR
//--------------------------------------------------------------------------
// Module:
//  SpareISRVector1f
//
///   This function handles a spurious interrupt at Vector 1f
///
//--------------------------------------------------------------------------
interrupt (CAPCOM_REGISTER_15) using ( SPAREISR_RB ) void SpareISRVector1f(void)
{
	ServiceSpuriousISR(CAPCOM_REGISTER_15);
}
#endif

#ifdef OCCUPIED_ISR
//--------------------------------------------------------------------------
// Module:
//  SpareISRVector20
//
///   This function handles a spurious interrupt at Vector 20
///
//--------------------------------------------------------------------------
interrupt (CAPCOM_TIMER_0) using ( SPAREISR_RB ) void SpareISRVector20(void)
{
	ServiceSpuriousISR(CAPCOM_TIMER_0);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector21
//
///   This function handles a spurious interrupt at Vector 21
///
//--------------------------------------------------------------------------
interrupt (CAPCOM_TIMER_1) using ( SPAREISR_RB ) void SpareISRVector21(void)
{
	ServiceSpuriousISR(CAPCOM_TIMER_1);
}
#endif

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector22
//
///   This function handles a spurious interrupt at Vector 22
///
//--------------------------------------------------------------------------
interrupt (0x22) using ( SPAREISR_RB ) void SpareISRVector22(void)
{
	ServiceSpuriousISR(0x22);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector23
//
///   This function handles a spurious interrupt at Vector 23
///
//--------------------------------------------------------------------------
interrupt (0x23) using ( SPAREISR_RB ) void SpareISRVector23(void)
{
	ServiceSpuriousISR(0x23);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector24
//
///   This function handles a spurious interrupt at Vector 24
///
//--------------------------------------------------------------------------
interrupt (0x24) using ( SPAREISR_RB ) void SpareISRVector24(void)
{
	ServiceSpuriousISR(0x24);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector25
//
///   This function handles a spurious interrupt at Vector 25
///
//--------------------------------------------------------------------------
interrupt (0x25) using ( SPAREISR_RB ) void SpareISRVector25(void)
{
	ServiceSpuriousISR(0x25);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector26
//
///   This function handles a spurious interrupt at Vector 26
///
//--------------------------------------------------------------------------
interrupt (0x26) using ( SPAREISR_RB ) void SpareISRVector26(void)
{
	ServiceSpuriousISR(0x26);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector27
//
///   This function handles a spurious interrupt at Vector 27
///
//--------------------------------------------------------------------------
interrupt (0x27) using ( SPAREISR_RB ) void SpareISRVector27(void)
{
	ServiceSpuriousISR(0x27);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector28
//
///   This function handles a spurious interrupt at Vector 28
///
//--------------------------------------------------------------------------
interrupt (0x28) using ( SPAREISR_RB ) void SpareISRVector28(void)
{
	ServiceSpuriousISR(0x28);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector29
//
///   This function handles a spurious interrupt at Vector 29
///
//--------------------------------------------------------------------------
interrupt (0x29) using ( SPAREISR_RB ) void SpareISRVector29(void)
{
	ServiceSpuriousISR(0x29);
}

#ifdef OCCUPIED_ISR
//--------------------------------------------------------------------------
// Module:
//  SpareISRVector2a
//
///   This function handles a spurious interrupt at Vector 2a
///
//--------------------------------------------------------------------------
interrupt (0x2a) using ( SPAREISR_RB ) void SpareISRVector2a(void)
{
	ServiceSpuriousISR(interrupt );
}
#endif

#ifdef OCCUPIED_ISR
//--------------------------------------------------------------------------
// Module:
//  SpareISRVector2b
//
///   This function handles a spurious interrupt at Vector 2b
///
//--------------------------------------------------------------------------
interrupt (0x2b) using ( SPAREISR_RB ) void SpareISRVector2b(void)
{
	ServiceSpuriousISR(0x2b);
}
#endif

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector2c
//
///   This function handles a spurious interrupt at Vector 2c
///
//--------------------------------------------------------------------------
interrupt (0x2c) using ( SPAREISR_RB ) void SpareISRVector2c(void)
{
	ServiceSpuriousISR(0x2c);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector2d
//
///   This function handles a spurious interrupt at Vector 2d
///
//--------------------------------------------------------------------------
interrupt (0x2d) using ( SPAREISR_RB ) void SpareISRVector2d(void)
{
	ServiceSpuriousISR(0x2d);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector2e
//
///   This function handles a spurious interrupt at Vector 2e
///
//--------------------------------------------------------------------------
interrupt (0x2e) using ( SPAREISR_RB ) void SpareISRVector2e(void)
{
	ServiceSpuriousISR(0x2e);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector2f
//
///   This function handles a spurious interrupt at Vector 2f
///
//--------------------------------------------------------------------------
interrupt (0x2f) using ( SPAREISR_RB ) void SpareISRVector2f(void)
{
	ServiceSpuriousISR(0x2f);
}


//--------------------------------------------------------------------------
// Module:
//  SpareISRVector30
//
///   This function handles a spurious interrupt at Vector 30
///
//--------------------------------------------------------------------------
interrupt (CAPCOM_REGISTER_16) using ( SPAREISR_RB ) void SpareISRVector30(void)
{
	ServiceSpuriousISR(CAPCOM_REGISTER_16);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector31
//
///   This function handles a spurious interrupt at Vector 31
///
//--------------------------------------------------------------------------
interrupt (CAPCOM_REGISTER_17) using ( SPAREISR_RB ) void SpareISRVector31(void)
{
	ServiceSpuriousISR(CAPCOM_REGISTER_17);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector32
//
///   This function handles a spurious interrupt at Vector 32
///
//--------------------------------------------------------------------------
interrupt (CAPCOM_REGISTER_18) using ( SPAREISR_RB ) void SpareISRVector32(void)
{
	ServiceSpuriousISR(CAPCOM_REGISTER_18);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector33
//
///   This function handles a spurious interrupt at Vector 33
///
//--------------------------------------------------------------------------
interrupt (CAPCOM_REGISTER_19) using ( SPAREISR_RB ) void SpareISRVector33(void)
{
	ServiceSpuriousISR(CAPCOM_REGISTER_19);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector34
//
///   This function handles a spurious interrupt at Vector 34
///
//--------------------------------------------------------------------------
interrupt (CAPCOM_REGISTER_20) using ( SPAREISR_RB ) void SpareISRVector34(void)
{
	ServiceSpuriousISR(CAPCOM_REGISTER_20);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector35
//
///   This function handles a spurious interrupt at Vector 35
///
//--------------------------------------------------------------------------
interrupt (CAPCOM_REGISTER_21) using ( SPAREISR_RB ) void SpareISRVector35(void)
{
	ServiceSpuriousISR(CAPCOM_REGISTER_21);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector36
//
///   This function handles a spurious interrupt at Vector 36
///
//--------------------------------------------------------------------------
interrupt (CAPCOM_REGISTER_22) using ( SPAREISR_RB ) void SpareISRVector36(void)
{
	ServiceSpuriousISR(CAPCOM_REGISTER_22);
}

#ifdef OCCUPIED_ISR
//--------------------------------------------------------------------------
// Module:
//  SpareISRVector37
//
///   This function handles a spurious interrupt at Vector 37
///
//--------------------------------------------------------------------------
interrupt (CAPCOM_REGISTER_23) using ( SPAREISR_RB ) void SpareISRVector37(void)
{
	ServiceSpuriousISR(CAPCOM_REGISTER_23);
}
#endif

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector38
//
///   This function handles a spurious interrupt at Vector 38
///
//--------------------------------------------------------------------------
interrupt (CAPCOM_REGISTER_24) using ( SPAREISR_RB ) void SpareISRVector38(void)
{
	ServiceSpuriousISR(CAPCOM_REGISTER_24);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector39
//
///   This function handles a spurious interrupt at Vector 39
///
//--------------------------------------------------------------------------
interrupt (CAPCOM_REGISTER_25) using ( SPAREISR_RB ) void SpareISRVector39(void)
{
	ServiceSpuriousISR(CAPCOM_REGISTER_25);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector3a
//
///   This function handles a spurious interrupt at Vector 3a
///
//--------------------------------------------------------------------------
interrupt (CAPCOM_REGISTER_26) using ( SPAREISR_RB ) void SpareISRVector3a(void)
{
	ServiceSpuriousISR(CAPCOM_REGISTER_26);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector3b
//
///   This function handles a spurious interrupt at Vector 3b
///
//--------------------------------------------------------------------------
interrupt (CAPCOM_REGISTER_27) using ( SPAREISR_RB ) void SpareISRVector3b(void)
{
	ServiceSpuriousISR(CAPCOM_REGISTER_27);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector3c
//
///   This function handles a spurious interrupt at Vector 3c
///
//--------------------------------------------------------------------------
interrupt (CAPCOM_REGISTER_28) using ( SPAREISR_RB ) void SpareISRVector3c(void)
{
	ServiceSpuriousISR(CAPCOM_REGISTER_28);
}

#ifdef OCCUPIED_ISR
//--------------------------------------------------------------------------
// Module:
//  SpareISRVector3d
//
///   This function handles a spurious interrupt at Vector 3d
///
//--------------------------------------------------------------------------
interrupt (CAPCOM_TIMER_7) using ( SPAREISR_RB ) void SpareISRVector3d(void)
{
	ServiceSpuriousISR(CAPCOM_TIMER_7);
}
#endif 

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector3e
//
///   This function handles a spurious interrupt at Vector 3e
///
//--------------------------------------------------------------------------
interrupt (CAPCOM_TIMER_8) using ( SPAREISR_RB ) void SpareISRVector3e(void)
{
	ServiceSpuriousISR(CAPCOM_TIMER_8);
}


//--------------------------------------------------------------------------
// Module:
//  SpareISRVector3f
//
///   This function handles a spurious interrupt at Vector 3f
///
//--------------------------------------------------------------------------
interrupt (0x3f) using ( SPAREISR_RB ) void SpareISRVector3f(void)
{
	ServiceSpuriousISR(0x3f);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector40
//
///   This function handles a spurious interrupt at Vector 40
///
//--------------------------------------------------------------------------
interrupt (0x40) using ( SPAREISR_RB ) void SpareISRVector40(void)
{
	ServiceSpuriousISR(0x40);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector41
//
///   This function handles a spurious interrupt at Vector 41
///
//--------------------------------------------------------------------------
interrupt (0x41) using ( SPAREISR_RB ) void SpareISRVector41(void)
{
	ServiceSpuriousISR(0x41);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector42
//
///   This function handles a spurious interrupt at Vector 42
///
//--------------------------------------------------------------------------
interrupt (0x42) using ( SPAREISR_RB ) void SpareISRVector42(void)
{
	ServiceSpuriousISR(0x42);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector43
//
///   This function handles a spurious interrupt at Vector 43
///
//--------------------------------------------------------------------------
interrupt (0x43) using ( SPAREISR_RB ) void SpareISRVector43(void)
{
	ServiceSpuriousISR(0x43);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector44
//
///   This function handles a spurious interrupt at Vector 44
///
//--------------------------------------------------------------------------
interrupt (0x44) using ( SPAREISR_RB ) void SpareISRVector44(void)
{
	ServiceSpuriousISR(0x44);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector45
//
///   This function handles a spurious interrupt at Vector 45
///
//--------------------------------------------------------------------------
interrupt (0x45) using ( SPAREISR_RB ) void SpareISRVector45(void)
{
	ServiceSpuriousISR(0x45);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector46
//
///   This function handles a spurious interrupt at Vector 46
///
//--------------------------------------------------------------------------
interrupt (0x46) using ( SPAREISR_RB ) void SpareISRVector46(void)
{
	ServiceSpuriousISR(0x46);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector47
//
///   This function handles a spurious interrupt at Vector 47
///
//--------------------------------------------------------------------------
interrupt (0x47) using ( SPAREISR_RB ) void SpareISRVector47(void)
{
	ServiceSpuriousISR(0x47);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector48
//
///   This function handles a spurious interrupt at Vector 48
///
//--------------------------------------------------------------------------
interrupt (0x48) using ( SPAREISR_RB ) void SpareISRVector48(void)
{
	ServiceSpuriousISR(0x48);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector49
//
///   This function handles a spurious interrupt at Vector 49
///
//--------------------------------------------------------------------------
interrupt (0x49) using ( SPAREISR_RB ) void SpareISRVector49(void)
{
	ServiceSpuriousISR(0x49);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector4a
//
///   This function handles a spurious interrupt at Vector 4a
///
//--------------------------------------------------------------------------
interrupt (0x4a) using ( SPAREISR_RB ) void SpareISRVector4a(void)
{
	ServiceSpuriousISR(0x4a);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector4b
//
///   This function handles a spurious interrupt at Vector 4b
///
//--------------------------------------------------------------------------
interrupt (0x4b) using ( SPAREISR_RB ) void SpareISRVector4b(void)
{
	ServiceSpuriousISR(0x4b);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector4c
//
///   This function handles a spurious interrupt at Vector 4c
///
//--------------------------------------------------------------------------
interrupt (0x4c) using ( SPAREISR_RB ) void SpareISRVector4c(void)
{
	ServiceSpuriousISR(0x4c);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector4d
//
///   This function handles a spurious interrupt at Vector 4d
///
//--------------------------------------------------------------------------
interrupt (0x4d) using ( SPAREISR_RB ) void SpareISRVector4d(void)
{
	ServiceSpuriousISR(0x4d);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector4e
//
///   This function handles a spurious interrupt at Vector 4e
///
//--------------------------------------------------------------------------
interrupt (0x4e) using ( SPAREISR_RB ) void SpareISRVector4e(void)
{
	ServiceSpuriousISR(0x4e);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector4f
//
///   This function handles a spurious interrupt at Vector 4f
///
//--------------------------------------------------------------------------
interrupt (0x4f) using ( SPAREISR_RB ) void SpareISRVector4f(void)
{
	ServiceSpuriousISR(0x4f);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector50
//
///   This function handles a spurious interrupt at Vector 50
///
//--------------------------------------------------------------------------
interrupt (0x50) using ( SPAREISR_RB ) void SpareISRVector50(void)
{
	ServiceSpuriousISR(0x50);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector51
//
///   This function handles a spurious interrupt at Vector 51
///
//--------------------------------------------------------------------------
interrupt (0x51) using ( SPAREISR_RB ) void SpareISRVector51(void)
{
	ServiceSpuriousISR(0x51);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector52
//
///   This function handles a spurious interrupt at Vector 52
///
//--------------------------------------------------------------------------
interrupt (0x52) using ( SPAREISR_RB ) void SpareISRVector52(void)
{
	ServiceSpuriousISR(0x52);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector53
//
///   This function handles a spurious interrupt at Vector 53
///
//--------------------------------------------------------------------------
interrupt (0x53) using ( SPAREISR_RB ) void SpareISRVector53(void)
{
	ServiceSpuriousISR(0x53);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector54
//
///   This function handles a spurious interrupt at Vector 54
///
//--------------------------------------------------------------------------
interrupt (0x54) using ( SPAREISR_RB ) void SpareISRVector54(void)
{
	ServiceSpuriousISR(0x54);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector55
//
///   This function handles a spurious interrupt at Vector 55
///
//--------------------------------------------------------------------------
interrupt (0x55) using ( SPAREISR_RB ) void SpareISRVector55(void)
{
	ServiceSpuriousISR(0x55);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector56
//
///   This function handles a spurious interrupt at Vector 56
///
//--------------------------------------------------------------------------
interrupt (0x56) using ( SPAREISR_RB ) void SpareISRVector56(void)
{
	ServiceSpuriousISR(0x56);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector57
//
///   This function handles a spurious interrupt at Vector 57
///
//--------------------------------------------------------------------------
interrupt (0x57) using ( SPAREISR_RB ) void SpareISRVector57(void)
{
	ServiceSpuriousISR(0x57);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector58
//
///   This function handles a spurious interrupt at Vector 58
///
//--------------------------------------------------------------------------
interrupt (0x58) using ( SPAREISR_RB ) void SpareISRVector58(void)
{
	ServiceSpuriousISR(0x58);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector59
//
///   This function handles a spurious interrupt at Vector 59
///
//--------------------------------------------------------------------------
interrupt (0x59) using ( SPAREISR_RB ) void SpareISRVector59(void)
{
	ServiceSpuriousISR(0x59);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector5a
//
///   This function handles a spurious interrupt at Vector 5a
///
//--------------------------------------------------------------------------
interrupt (0x5a) using ( SPAREISR_RB ) void SpareISRVector5a(void)
{
	ServiceSpuriousISR(0x5a);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector5b
//
///   This function handles a spurious interrupt at Vector 5b
///
//--------------------------------------------------------------------------
interrupt (0x5b) using ( SPAREISR_RB ) void SpareISRVector5b(void)
{
	ServiceSpuriousISR(0x5b);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector5c
//
///   This function handles a spurious interrupt at Vector 5c
///
//--------------------------------------------------------------------------
interrupt (0x5c) using ( SPAREISR_RB ) void SpareISRVector5c(void)
{
	ServiceSpuriousISR(0x5c);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector5d
//
///   This function handles a spurious interrupt at Vector 5d
///
//--------------------------------------------------------------------------
interrupt (0x5d) using ( SPAREISR_RB ) void SpareISRVector5d(void)
{
	ServiceSpuriousISR(0x5d);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector5e
//
///   This function handles a spurious interrupt at Vector 5e
///
//--------------------------------------------------------------------------
interrupt (0x5e) using ( SPAREISR_RB ) void SpareISRVector5e(void)
{
	ServiceSpuriousISR(0x5e);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector5f
//
///   This function handles a spurious interrupt at Vector 5f
///
//--------------------------------------------------------------------------
interrupt (0x5f) using ( SPAREISR_RB ) void SpareISRVector5f(void)
{
	ServiceSpuriousISR(0x5f);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector60
//
///   This function handles a spurious interrupt at Vector 60
///
//--------------------------------------------------------------------------
interrupt (0x60) using ( SPAREISR_RB ) void SpareISRVector60(void)
{
	ServiceSpuriousISR(0x60);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector61
//
///   This function handles a spurious interrupt at Vector 61
///
//--------------------------------------------------------------------------
interrupt (0x61) using ( SPAREISR_RB ) void SpareISRVector61(void)
{
	ServiceSpuriousISR(0x61);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector62
//
///   This function handles a spurious interrupt at Vector 62
///
//--------------------------------------------------------------------------
interrupt (0x62) using ( SPAREISR_RB ) void SpareISRVector62(void)
{
	ServiceSpuriousISR(0x62);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector63
//
///   This function handles a spurious interrupt at Vector 63
///
//--------------------------------------------------------------------------
interrupt (0x63) using ( SPAREISR_RB ) void SpareISRVector63(void)
{
	ServiceSpuriousISR(0x63);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector64
//
///   This function handles a spurious interrupt at Vector 64
///
//--------------------------------------------------------------------------
interrupt (0x64) using ( SPAREISR_RB ) void SpareISRVector64(void)
{
	ServiceSpuriousISR(0x64);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector65
//
///   This function handles a spurious interrupt at Vector 65
///
//--------------------------------------------------------------------------
interrupt (0x65) using ( SPAREISR_RB ) void SpareISRVector65(void)
{
	ServiceSpuriousISR(0x65);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector66
//
///   This function handles a spurious interrupt at Vector 66
///
//--------------------------------------------------------------------------
interrupt (0x66) using ( SPAREISR_RB ) void SpareISRVector66(void)
{
	ServiceSpuriousISR(0x66);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector67
//
///   This function handles a spurious interrupt at Vector 67
///
//--------------------------------------------------------------------------
interrupt (0x67) using ( SPAREISR_RB ) void SpareISRVector67(void)
{
	ServiceSpuriousISR(0x67);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector68
//
///   This function handles a spurious interrupt at Vector 68
///
//--------------------------------------------------------------------------
interrupt (0x68) using ( SPAREISR_RB ) void SpareISRVector68(void)
{
	ServiceSpuriousISR(0x68);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector69
//
///   This function handles a spurious interrupt at Vector 69
///
//--------------------------------------------------------------------------
interrupt (0x69) using ( SPAREISR_RB ) void SpareISRVector69(void)
{
	ServiceSpuriousISR(0x69);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector6a
//
///   This function handles a spurious interrupt at Vector 6a
///
//--------------------------------------------------------------------------
interrupt (0x6a) using ( SPAREISR_RB ) void SpareISRVector6a(void)
{
	ServiceSpuriousISR(0x6a);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector6b
//
///   This function handles a spurious interrupt at Vector 6b
///
//--------------------------------------------------------------------------
interrupt (0x6b) using ( SPAREISR_RB ) void SpareISRVector6b(void)
{
	ServiceSpuriousISR(0x6b);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector6c
//
///   This function handles a spurious interrupt at Vector 6c
///
//--------------------------------------------------------------------------
interrupt (0x6c) using ( SPAREISR_RB ) void SpareISRVector6c(void)
{
	ServiceSpuriousISR(0x6c);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector6d
//
///   This function handles a spurious interrupt at Vector 6d
///
//--------------------------------------------------------------------------
interrupt (0x6d) using ( SPAREISR_RB ) void SpareISRVector6d(void)
{
	ServiceSpuriousISR(0x6d);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector6e
//
///   This function handles a spurious interrupt at Vector 6e
///
//--------------------------------------------------------------------------
interrupt (0x6e) using ( SPAREISR_RB ) void SpareISRVector6e(void)
{
	ServiceSpuriousISR(0x6e);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector6f
//
///   This function handles a spurious interrupt at Vector 6f
///
//--------------------------------------------------------------------------
interrupt (0x6f) using ( SPAREISR_RB ) void SpareISRVector6f(void)
{
	ServiceSpuriousISR(0x6f);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector70
//
///   This function handles a spurious interrupt at Vector 70
///
//--------------------------------------------------------------------------
interrupt (0x70) using ( SPAREISR_RB ) void SpareISRVector70(void)
{
	ServiceSpuriousISR(0x70);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector71
//
///   This function handles a spurious interrupt at Vector 71
///
//--------------------------------------------------------------------------
interrupt (0x71) using ( SPAREISR_RB ) void SpareISRVector71(void)
{
	ServiceSpuriousISR(0x71);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector72
//
///   This function handles a spurious interrupt at Vector 72
///
//--------------------------------------------------------------------------
interrupt (0x72) using ( SPAREISR_RB ) void SpareISRVector72(void)
{
	ServiceSpuriousISR(0x72);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector73
//
///   This function handles a spurious interrupt at Vector 73
///
//--------------------------------------------------------------------------
interrupt (0x73) using ( SPAREISR_RB ) void SpareISRVector73(void)
{
	ServiceSpuriousISR(0x73);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector74
//
///   This function handles a spurious interrupt at Vector 74
///
//--------------------------------------------------------------------------
interrupt (0x74) using ( SPAREISR_RB ) void SpareISRVector74(void)
{
	ServiceSpuriousISR(0x74);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector75
//
///   This function handles a spurious interrupt at Vector 75
///
//--------------------------------------------------------------------------
interrupt (0x75) using ( SPAREISR_RB ) void SpareISRVector75(void)
{
	ServiceSpuriousISR(0x75);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector76
//
///   This function handles a spurious interrupt at Vector 76
///
//--------------------------------------------------------------------------
interrupt (0x76) using ( SPAREISR_RB ) void SpareISRVector76(void)
{
	ServiceSpuriousISR(0x76);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector77
//
///   This function handles a spurious interrupt at Vector 77
///
//--------------------------------------------------------------------------
interrupt (0x77) using ( SPAREISR_RB ) void SpareISRVector77(void)
{
	ServiceSpuriousISR(0x77);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector78
//
///   This function handles a spurious interrupt at Vector 78
///
//--------------------------------------------------------------------------
interrupt (0x78) using ( SPAREISR_RB ) void SpareISRVector78(void)
{
	ServiceSpuriousISR(0x78);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector79
//
///   This function handles a spurious interrupt at Vector 79
///
//--------------------------------------------------------------------------
interrupt (0x79) using ( SPAREISR_RB ) void SpareISRVector79(void)
{
	ServiceSpuriousISR(0x79);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector7a
//
///   This function handles a spurious interrupt at Vector 7a
///
//--------------------------------------------------------------------------
interrupt (0x7a) using ( SPAREISR_RB ) void SpareISRVector7a(void)
{
	ServiceSpuriousISR(0x7a);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector7b
//
///   This function handles a spurious interrupt at Vector 7b
///
//--------------------------------------------------------------------------
interrupt (0x7b) using ( SPAREISR_RB ) void SpareISRVector7b(void)
{
	ServiceSpuriousISR(0x7b);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector7c
//
///   This function handles a spurious interrupt at Vector 7c
///
//--------------------------------------------------------------------------
interrupt (0x7c) using ( SPAREISR_RB ) void SpareISRVector7c(void)
{
	ServiceSpuriousISR(0x7c);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector7d
//
///   This function handles a spurious interrupt at Vector 7d
///
//--------------------------------------------------------------------------
interrupt (0x7d) using ( SPAREISR_RB ) void SpareISRVector7d(void)
{
	ServiceSpuriousISR(0x7d);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector7e
//
///   This function handles a spurious interrupt at Vector 7e
///
//--------------------------------------------------------------------------
interrupt (0x7e) using ( SPAREISR_RB ) void SpareISRVector7e(void)
{
	ServiceSpuriousISR(0x7e);
}

//--------------------------------------------------------------------------
// Module:
//  SpareISRVector7f
//
///   This function handles a spurious interrupt at Vector 7f
///
//--------------------------------------------------------------------------
interrupt (0x7f) using ( SPAREISR_RB ) void SpareISRVector7f(void)
{
	ServiceSpuriousISR(0x7f);
}

static void ServiceSpuriousISR(UINT_16 aId)
{
	/* Diable all interrupts and allow a watchdog reset to happen */
	DISABLE_ALL_INTERRUPTS();

	DT_ReadUsingRTC(&rtc);

	unExpectedISR[unExpectedISRCount].unexpectedIntr = aId;
    unExpectedISR[unExpectedISRCount].month = rtc.month;
    unExpectedISR[unExpectedISRCount].day = rtc.day;
    unExpectedISR[unExpectedISRCount].year = rtc.year;
    unExpectedISR[unExpectedISRCount].hour = rtc.hour;
    unExpectedISR[unExpectedISRCount].minute = rtc.minute;
    unExpectedISR[unExpectedISRCount].second = rtc.second;

	unExpectedISRCount++;
	if (unExpectedISRCount >= UNEXPECTED_ISR_LOG_SIZE)
	{
		unExpectedISRCount = 0;
	}

	/* Wait for a watchdog reset */
	while(1);
}

