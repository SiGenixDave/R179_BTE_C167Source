/*****************************************************************************
*
* Copyright (C) 2010 Bombardier
*
* File Name: DateTime.c
*
* Revision History:
*   08/31/2009 - das - Created
*
******************************************************************************/
///   \file
///   This file contains functions that access the HIDAC's Real Time Clock

#define datetime_C

/*--------------------------------------------------------------------------
                              INCLUDE FILES
  --------------------------------------------------------------------------*/
/* _DEBUG is defined in Microsoft Visual Studio 2008: UNIT TEST ONLY */
#ifndef _DEBUG
    #include <c166.h>
    #include <reg167.h>
#endif
#include <stdio.h>
#include "Types.h"
#include "DateTime.h"

/*--------------------------------------------------------------------------
                             MODULE CONSTANTS
  --------------------------------------------------------------------------*/
#define STK17T88_READ_BIT             ((UINT_8)(0x01))
#define STK17T88_WRITE_BIT            ((UINT_8)(0x02))
#define STK17T88_ENABLE_OSC_BIT       ((UINT_8)(0x80))

#define SECONDS_PER_MINUTE          60
#define MINUTES_PER_HOUR            60
#define HOURS_PER_DAY               24

#define SECONDS_PER_HOUR            3600
#define SECONDS_PER_DAY             86400


#ifdef _DEBUG
static UINT_8  timeKeeperFlagVar;
    #define timekeepingFlagsControl       timeKeeperFlagVar
static UINT_8  timeKeeperControl;
    #define timekeepingControlNV          timeKeeperControl
#else
    #define timekeepingFlagsControl       (*(UINT_8  *) (0x307ff0L))
    #define timekeepingControlNV          (*(UINT_8  *) (0x307ff8L))
#endif


/*--------------------------------------------------------------------------
                              MODULE MACROS
  --------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
                             MODULE DATA TYPES
  --------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
                              MODULE VARIABLES
  --------------------------------------------------------------------------*/
#ifndef _DEBUG
/* NOTE: Word writes have to start on even boundaries. When writing multiByte s
   structures, the compiler performs word writes */
static RTC_t * const realTimeClockPtr  =  (RTC_t *)(0x307ff8L);
#else
static RTC_t debugRTC;
static RTC_t * const realTimeClockPtr =  &debugRTC;
#endif
/*--------------------------------------------------------------------------
                             MODULE PROTOTYPES
  --------------------------------------------------------------------------*/
static UINT_8 DT_DecimalToBCD(UINT_8 aDecimal);
static UINT_8 DT_TwoCharASCIIToBCD(CHAR aMSByteASCII, CHAR aLSByteASCII) ;
static UINT_8 DT_BCDToDecimal(UINT_8 aBCD);
static void DT_ConvertFromUTC(RTC_t *aDest, UINT_32 aSource);
static UINT_32 DT_ConvertToUTC(RTC_t *aSource);
static void DT_ZeroFilledASCIIClockVal(CHAR *aString, UINT_8 aClockVal);

//--------------------------------------------------------------------------
// Module:
//  DT_EnableOscillator
//   
///   This function enables or disables the real time clock oscillator. To 
///   prevent battery/super cap draining, the oscillator should be disabled 
///   (e.g. shelving the HIDAC). When in operation, the oscillator needs to 
///   be enabled for proper operation of the real time clock
/// 
///	  \param aEnable - TRUE to enable the oscillator; FALSE to disable  
/// 
//--------------------------------------------------------------------------
void DT_EnableOscillator(BOOLEAN aEnable)
{
    timekeepingFlagsControl |= STK17T88_WRITE_BIT;    

	if (aEnable)
    {
        timekeepingControlNV &= ~STK17T88_ENABLE_OSC_BIT;
    }
    else
    {
        timekeepingControlNV |= STK17T88_ENABLE_OSC_BIT;
    }

    /* Reset write bit */
    timekeepingFlagsControl &= ~(STK17T88_WRITE_BIT); 

}

//--------------------------------------------------------------------------
// Module:
//  DT_OscillatorEnabled
//   
///   This function determines if the RTC oscillator is enabled or disabled.
///
///   \return BOOLEAN - TRUE if the oscillator is enabled; FALSE otherwise
/// 
//--------------------------------------------------------------------------
BOOLEAN DT_OscillatorEnabled(void)
{
	
	if (timekeepingControlNV & STK17T88_ENABLE_OSC_BIT)
	{
		return FALSE;
	}
	else
	{
		return TRUE;
	}

}

//--------------------------------------------------------------------------
// Module:
//  DT_UpdateUsingString
//   
///   This function updates the real time clock on the HIDAC. The argument 
///   must be a character string in this form. 
/// 
///   "MMDDYYYYHHMMSS"  where: 
/// 
///   MM is the month from 01 to 12 
///   DD is the day from 01 to 31 
///   YYYY is the year from 2000 to 2099 
/// 
///   HH is the hour from 00 - 23 
///   MM is the minute from 00 - 59 
///   SS is the second fom 00 - 59 
///    
///   No checks are made to verify the validity of any date or time 
///   parameter. No spaces are allowed to separate.
/// 
///	  \param aString - the desired date/time string  
/// 
//--------------------------------------------------------------------------
void DT_UpdateUsingString(CHAR *aString)
{
    RTC_t rtcBCD;

    rtcBCD.month   = DT_TwoCharASCIIToBCD(aString[0],aString[1]);
    rtcBCD.day     = DT_TwoCharASCIIToBCD(aString[2],aString[3]);
    rtcBCD.year    = DT_TwoCharASCIIToBCD(aString[6],aString[7]);
    rtcBCD.hour    = DT_TwoCharASCIIToBCD(aString[8],aString[9]);
    rtcBCD.minute  = DT_TwoCharASCIIToBCD(aString[10],aString[11]);
    rtcBCD.second  = DT_TwoCharASCIIToBCD(aString[12],aString[13]);

    DT_UpdateUsingBCD(&rtcBCD);
}


//--------------------------------------------------------------------------
// Module:
//  DT_UpdateUsingDelimitedString
//   
///   This function updates the real time clock on the HIDAC. The argument 
///   must be a character string in this form. 
/// 
///   "MM/DD/YYYY HH:MM:SS"  where: 
/// 
///   MM is the month from 01 to 12 
///   DD is the day from 01 to 31 
///   YYYY is the year from 2000 to 2099 
/// 
///   HH is the hour from 00 - 23 
///   MM is the minute from 00 - 59 
///   SS is the second fom 00 - 59 
///    
///   No checks are made to verify the validity of any date or time 
///   parameter. Also, the delimiters are not verified... they can 
///   be any valid ASCII character
/// 
///	  \param aString - the desired date/time string  
/// 
//--------------------------------------------------------------------------
void DT_UpdateUsingDelimitedString(CHAR *aString)
{
    RTC_t rtcBCD;

    rtcBCD.month   = DT_TwoCharASCIIToBCD(aString[0],aString[1]);
    rtcBCD.day     = DT_TwoCharASCIIToBCD(aString[3],aString[4]);
    rtcBCD.year    = DT_TwoCharASCIIToBCD(aString[8],aString[9]);
    rtcBCD.hour    = DT_TwoCharASCIIToBCD(aString[11],aString[12]);
    rtcBCD.minute  = DT_TwoCharASCIIToBCD(aString[14],aString[15]);
    rtcBCD.second  = DT_TwoCharASCIIToBCD(aString[17],aString[18]);

    DT_UpdateUsingBCD(&rtcBCD);
}


//--------------------------------------------------------------------------
// Module:
//  DT_UpdateUsingRTC
//   
///   This function sets the real time clock with the parameters set 
///   in the argument. These parameters are in normal integer format. 
///   See description of RTC_t type for valid parameters.
/// 
///	  \param aRtc - pointer to structure with date/time parameters  
/// 
//--------------------------------------------------------------------------
void DT_UpdateUsingRTC(RTC_t *aRtc)
{
    RTC_t rtcBCD;

    rtcBCD.month  = DT_DecimalToBCD(aRtc->month);
    rtcBCD.day    = DT_DecimalToBCD(aRtc->day);
    rtcBCD.year   = DT_DecimalToBCD(aRtc->year);
    rtcBCD.hour   = DT_DecimalToBCD(aRtc->hour);
    rtcBCD.minute = DT_DecimalToBCD(aRtc->minute);
    rtcBCD.second = DT_DecimalToBCD(aRtc->second);

    DT_UpdateUsingBCD(&rtcBCD);

}

//--------------------------------------------------------------------------
// Module:
//  DT_UpdateUsingBCD
//   
///   This function sets the real time clock with the parameters set 
///   in the argument. These paarmeters must be in BCD format. 
///   See description of RTC_t type for valid parameters.
/// 
///	  \param aBCD - pointer to structure with date/time BCD parameters  
/// 
//--------------------------------------------------------------------------
void DT_UpdateUsingBCD(RTC_t *aBCD)
{

    UINT_8 calOsc;

	calOsc = timekeepingControlNV;

	aBCD->calOsc = calOsc;

	/* Set write bit */
    timekeepingFlagsControl |= STK17T88_WRITE_BIT;

    *realTimeClockPtr = *aBCD;

    /* Reset write bit */
    timekeepingFlagsControl &= ~(STK17T88_WRITE_BIT); 

}

//--------------------------------------------------------------------------
// Module:
//  DT_UpdateUsingUTC
//   
///   This function sets the real time clock. The argument is in UTC format. 
///   See the Internet for a detailed description of UTC. 
/// 
///	  \param aUTC - the number of seconds since 12:00:00 Jan 1, 1972   
/// 
//--------------------------------------------------------------------------
void DT_UpdateUsingUTC(UINT_32 aUTC)
{
    RTC_t  rtc;

    /* Convert from UTC. Conversion is stored in rtc. */
    DT_ConvertFromUTC(&rtc, aUTC);

    /* Update the real time clock */
    DT_UpdateUsingRTC(&rtc);
}


//--------------------------------------------------------------------------
// Module:
//  DT_ReadUsingRTC
//   
///   This function reads the real time clock and stores the result in the 
///   argument to the function. The real time clock data is converted from
///   BCD format to decimal (binary).
/// 
///	  \param aRTCData - pointer to the structure where the result is to be 
///                     stored. 
/// 
//--------------------------------------------------------------------------
void DT_ReadUsingRTC(RTC_t *aRTCData)
{

    /* Set read bit */
    timekeepingFlagsControl |= STK17T88_READ_BIT; 

    /* Read the result */
    aRTCData->hour    = DT_BCDToDecimal(realTimeClockPtr->hour   & 0x3F);
    aRTCData->minute  = DT_BCDToDecimal(realTimeClockPtr->minute & 0x7F);
    aRTCData->second  = DT_BCDToDecimal(realTimeClockPtr->second & 0x7F);
    aRTCData->month   = DT_BCDToDecimal(realTimeClockPtr->month  & 0x1F);
    aRTCData->day     = DT_BCDToDecimal(realTimeClockPtr->day    & 0x3F);
    aRTCData->year    = DT_BCDToDecimal(realTimeClockPtr->year   & 0xFF);

    /* Reset read bit */
    timekeepingFlagsControl &= ~(STK17T88_READ_BIT);

}


//--------------------------------------------------------------------------
// Module:
//  DT_ReadUsingBCD
//   
///   This function reads the real time clock and stores the result in the 
///   argument to the function.  
/// 
///	  \param aRTCData - pointer to the structure where the result is to be 
///                     stored. 
/// 
//--------------------------------------------------------------------------
void DT_ReadUsingBCD(RTC_t *aBCDData)
{

    /* Set read bit */
    timekeepingFlagsControl |= STK17T88_READ_BIT; 

    /* Read the result */
    aBCDData->hour    = realTimeClockPtr->hour   & 0x3F;
    aBCDData->minute  = realTimeClockPtr->minute & 0x7F;
    aBCDData->second  = realTimeClockPtr->second & 0x7F;
    aBCDData->month   = realTimeClockPtr->month  & 0x1F;
    aBCDData->day     = realTimeClockPtr->day    & 0x3F;
    aBCDData->year    = realTimeClockPtr->year   & 0xFF;

    /* Reset read bit */
    timekeepingFlagsControl &= ~(STK17T88_READ_BIT);

}

//--------------------------------------------------------------------------
// Module:
//  DT_ReadUsingUTC
//   
///   This function reads the real time clock, converts the result to a UTC 
///   (number of seconds since Jan 1, 1972 at midnight and stores the result 
///   in the argument to the function. 
/// 
///	  \param aUTC - pointer to the UINT_32 where the result is to be 
///                 stored. 
/// 
//--------------------------------------------------------------------------
void DT_ReadUsingUTC(UINT_32 *aUTC)
{
    RTC_t rtc;

    /* Read the real time clock */
    DT_ReadUsingRTC(&rtc);

    /* Convert from RTC format to UTC and store  */
    *aUTC = DT_ConvertToUTC(&rtc);

}


//--------------------------------------------------------------------------
// Module:
//  DT_ReadUsingString
//   
///   This function reads the real time clock, converts the result to a string 
///   and stores the result in the argument to the function. It is up to 
///   the calling function to ensure that aString is sized large enough 
///   to store the result (a minimum of 15 CHARs). The result will be in 
///   the format "MMDDYYYYHHMMSS". 
/// 
///	  \param aString - pointer to the string where the result is to be 
///                    stored. 
/// 
//--------------------------------------------------------------------------
void DT_ReadUsingString(CHAR *aString)
{
    RTC_t rtc;

    /* Read the real time clock */
    DT_ReadUsingRTC(&rtc);

    DT_ZeroFilledASCIIClockVal(&aString[0],rtc.month);
    DT_ZeroFilledASCIIClockVal(&aString[2],rtc.day);
    aString[4] = '2';
    aString[5] = '0';
    DT_ZeroFilledASCIIClockVal(&aString[6],rtc.year);
    DT_ZeroFilledASCIIClockVal(&aString[8],rtc.hour);
    DT_ZeroFilledASCIIClockVal(&aString[10],rtc.minute);
    DT_ZeroFilledASCIIClockVal(&aString[12],rtc.second);

    aString[14] = (CHAR)NULL;

}


//--------------------------------------------------------------------------
// Module:
//  DT_ReadUsingDelimitedString
//   
///   This function reads the real time clock, converts the result to a string 
///   and stores the result in the argument to the function. It is up to 
///   the calling function to ensure that aString is sized large enough 
///   to store the result (a minimum of 20 CHARs). The result will be in 
///   the format "MM/DD/YYYY HH:MM:SS" assuming aDateDelimiter = '/' and 
///   aTimeDelimiter = ':'. 
/// 
///	  \param aString - pointer to the string where the result is to be 
///                    stored. 
/// 
//--------------------------------------------------------------------------
void DT_ReadUsingDelimitedString(CHAR *aString, CHAR aDateDelimiter, CHAR aTimeDelimiter )
{
    RTC_t rtc;

    /* Read the real time clock */
    DT_ReadUsingRTC(&rtc);

    DT_ZeroFilledASCIIClockVal(&aString[0],rtc.month);
    aString[2] = aDateDelimiter;
    DT_ZeroFilledASCIIClockVal(&aString[3],rtc.day);
    aString[5] = aDateDelimiter;
    aString[6] = '2';
    aString[7] = '0';
    DT_ZeroFilledASCIIClockVal(&aString[8],rtc.year);
    aString[10] = ' ';
    DT_ZeroFilledASCIIClockVal(&aString[11],rtc.hour);
    aString[13] = aTimeDelimiter;
    DT_ZeroFilledASCIIClockVal(&aString[14],rtc.minute);
    aString[16] = aTimeDelimiter;
    DT_ZeroFilledASCIIClockVal(&aString[17],rtc.second);

    aString[19] = (CHAR)NULL;

}

//--------------------------------------------------------------------------
// Module:
//  DT_ConvertToUTC
//   
///   This function converts the parameters stored in the RTC_t structure 
///   to UTC format and returns the result. 
/// 
///	  \param aSource - pointer 
/// 
///   \return UINT_32 - the number of seconds since midnight, Jan 1, 1972 
/// 
//--------------------------------------------------------------------------
static UINT_32 DT_ConvertToUTC(RTC_t *aSource)
{
    UINT_32 utc;

    UINT_16 adjustedYear;

    adjustedYear = aSource->year + 2000;

    if (aSource->month < 3)
    {
        /* Adjust month and year so that months are 3 - 14 and the
        "year" starts in March.  This simplifies adjusting for leap
        years by having the extra day at the end of the "year". */
        aSource->month += 12;

        adjustedYear -= 1;
    }

    /* count number of days since beginning of month */
    utc = aSource->day;

    /* add number of days from previous months */
    utc += (979 * aSource->month - 2918) >> 5;

    /* add number of days from previous years */
    utc += adjustedYear * (UINT_32)365;

    /* compensate for leap years */
    utc += (adjustedYear >> 2);   /* leap year every four years... */

    utc -= 15; /*
                 * except for century years...
                 * unless the century is divisible by 400.
                 * We can use -15 instead of
                 * -adjustedYear/100 + adjustedYear/400
                 * if we assume that the date is between
                 * 1900 and 2099.
                 */

    /* utc is now a count of days with 3/1/0000 being day 1 */

    /* subtract offset to give us the number of days since 1/1/1972 */
    utc -= 720199;

    /* convert the count of days since 1/1/1972 to the count of seconds
       since midnight, 1/1/1972 */

    utc *= SECONDS_PER_DAY;

    /* add in the time of day in seconds */
    utc += aSource->hour * (UINT_32)SECONDS_PER_HOUR;
    utc += aSource->minute * SECONDS_PER_MINUTE;
    utc += aSource->second;

    if (utc > 0x4121F400)
    {
        utc++;  
    }

    /* utc is now a count of seconds since midnight, 1/1/1972 */
    return utc;
}

//--------------------------------------------------------------------------
// Module:
//  DT_ConvertFromUTC
//   
///   This function converts . 
/// 
///	  \param aDest - pointer to the structure where to store date and time
///   \param aSource - the number of seconds since midnight, Jan 1, 1972
/// 
//--------------------------------------------------------------------------
static void DT_ConvertFromUTC(RTC_t *aDest, UINT_32 aSource)
{
    UINT_32 adjustedYear;
    UINT_32 adjustedMonth;
    UINT_32 dayOfYear;
    UINT_16 dayOfMonth;

    /* First get the time of day */
    aDest->hour   = (aSource % SECONDS_PER_DAY) / SECONDS_PER_HOUR;
    aDest->minute = (aSource % SECONDS_PER_HOUR) / SECONDS_PER_MINUTE;
    aDest->second = (aSource % SECONDS_PER_MINUTE);

    /* Convert aSource from seconds since midnight, 1/1/1972 to days
       since 1/1/1972 */
    aSource /= SECONDS_PER_DAY;

    /* Change this to a count of days with 3/1/0000 being day 1 */
    aSource += 720199;

    /* Calculate the "years" (with "years" starting in March) */
    adjustedYear = aSource * 100 - 25;  /* aSource - .25 */

    /* Adjust for leap years that occur on centuries.  We can use +15
       instead of calculating it if we assume that the date is between
       1900 and 2099. */
    adjustedYear += 15 * 100;  

    adjustedYear /= 36525;

    /* Days since March 1st */
    dayOfYear = 15 + aSource - 365 * adjustedYear - (adjustedYear >> 2);

    /* Calculate the "month" with Mar = 3, Apr = 4, ... Jan = 13, Feb = 14 */
    adjustedMonth = (535 * dayOfYear + 48950) >> 14;

    /* Calculate the day of the month */
    dayOfMonth = dayOfYear - ((979 * adjustedMonth - 2918) >> 5);

    if (adjustedMonth > 12)
    {
        adjustedYear += 1;
        adjustedMonth -= 12;
    }

    aDest->day   = (UINT_8)dayOfMonth;
    aDest->month = (UINT_8)adjustedMonth;
    aDest->year  = (UINT_8)(adjustedYear % 100);
}


//--------------------------------------------------------------------------
// Module:
//  DT_DecimalToBCD
//   
///   This function converts the argument to Binary Coded Decimal (BCD). 
///   Since all parameters associated with the real time clock are between 
///   0 and 9, the function does not deal with hexadecimal numbers. 
/// 
///	  \param aDecimal - the decimal number 
/// 
///   \return UINT_8 - the BCD value of aDecimal 
/// 
//--------------------------------------------------------------------------
static UINT_8 DT_DecimalToBCD(UINT_8 aDecimal)
{
    return((aDecimal/10) << 4) | (aDecimal % 10);
}

//--------------------------------------------------------------------------
// Module:
//  DT_TwoCharASCIIToBCD
//   
///   This function converts the ASCII arguments to Binary Coded Decimal (BCD). 
///   Since all parameters associated with the real time clock are between 
///   0 and 9, the function does not deal with hexadecimal numbers.  
/// 
///	  \param aMSByteASCII - The ASCII most significant byte
///	  \param aLSByteASCII - The ASCII least significant byte
/// 
///   \return UINT_8 - The converted BCD value  
/// 
//--------------------------------------------------------------------------
static UINT_8 DT_TwoCharASCIIToBCD(CHAR aMSByteASCII, CHAR aLSByteASCII) 
{
    return((aMSByteASCII - '0') << 4) + (aLSByteASCII - '0');
}


//--------------------------------------------------------------------------
// Module:
//  DT_BCDToDecimal
//   
///   This function converts a binary coded decimal to decimal value. 
/// 
///	  \param aBCD - the binary coded decimal to be converted
/// 
///   \return UINT_8 -  the converted value
/// 
//--------------------------------------------------------------------------
static UINT_8 DT_BCDToDecimal(UINT_8 aBCD)
{
    return ((aBCD & 0xf0) >> 4) * 10  + (aBCD & 0x0f);
}


//--------------------------------------------------------------------------
// Module:
//  DT_ZeroFilledASCIIClockVal
//   
///   This function converts a real time clock parameter from a decimal 
///   number to a two character ASCII representation. If a number from 
///   0 - 9 is being converted, the first character is always filled 
///   with '0'. Since the real time clock does not deal with hexadecimal 
///   values (i.e. A-F), no support is included in this function. 
/// 
///	  \param aString - pointer to where the ASCII representation is stored 
///                    must be capable of storing 2 ASCII CHARs 
///	  \param aClockVal - value to be converted ASCII
/// 
//--------------------------------------------------------------------------
static void DT_ZeroFilledASCIIClockVal(CHAR *aString, UINT_8 aClockVal)
{
    if (aClockVal >= 10)
    {
        aString[0] = (aClockVal / 10) + '0';
    }
    /* Fill the first character with '0' since the number is less than 10. */
    else
    {
        aString[0] = '0';
    }
    /* Fill the second CHAR */
    aString[1] = (aClockVal % 10) + '0';
}



