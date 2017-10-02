/*****************************************************************************
*
* Copyright (C) 2010 Bombardier
*
* File Name: AlogOut.h
*
* Abstract: Header file for the tachometer processing driver
*
* Revision History:
*   05/31/2010 - das - Created
*
******************************************************************************/
#ifndef TACHO_H
#define TACHO_H

/*--------------------------------------------------------------------------
                             GLOBAL CONSTANTS
  --------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
                               GLOBAL TYPES
  --------------------------------------------------------------------------*/
typedef enum		/* Connect to channel A if using single phase tach input */
{					/*		CHANNEL A				CHANNEL B */
	TACH1,			/* P3-C28(+) P3-B28(-)		P3-C30(+) P3-B29(-) */
	TACH2,			/* P3-C32(+) P3-A30(-)		P3-A32(+) P3-B32(-) */
	TACH3,			/* P3-A28(+) P3-A27(-)		P3-A29(+) P3-C29(-) */
	TACH4,			/* P3-C31(+) P3-B30(-)		P3-A31(+) P3-B31(-) */

	NUM_OF_TACHS

} eTachId;

typedef enum
{
	TACH_PERIOD,
	TACH_FREQUENCY

} eTachMode;

typedef enum
{
	TACH_A_LEADS_B,
	TACH_B_LEADS_A

} eTachDirection;

typedef struct
{
	eTachId		id;				/* tachometer id */
	eTachMode	mode;			/* either frequency or period */
	UINT_16		isrPriority;	/* ISR priority */
	UINT_16		intervalCount;	/* time between DONE signals (in usecs) */
	BOOLEAN		quad;			/* TRUE if quadrature tachs are used */
	BOOLEAN		test;			/* TRUE if in test mode. */
} TachConfig;


typedef struct
{
	UINT_16			value;		/* raw value read from the tach reader */
	UINT_16			isrCount;	/* increments when DONE signal generated */
	eTachDirection	direction;	/* only valid when using quadrature tachs */

} TachStatus;

/*--------------------------------------------------------------------------
                              GLOBAL VARIABLES
  --------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
                              GLOBAL PROTOTYPES
  --------------------------------------------------------------------------*/
EXPORT void TO_Init(TachConfig *aConfig);
EXPORT void TO_GetStatus(eTachId aId, TachStatus *aPtr);
EXPORT BOOLEAN TO_GetFrequency(eTachId aId, UINT_16 *aFreq);
EXPORT BOOLEAN TO_GetAllFrequencies(UINT_16 aFreq[]);

#endif
