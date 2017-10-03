/*****************************************************************************
*
* Copyright (C) 2010 Bombardier
*
* File Name: AlogOut.h
*
* Abstract: Header file for the high precision analog output driver
*
* Revision History:
*   05/31/2010 - das - Created
*   10/22/2010 - prs - Include definition of structure AlogOut_t from here to 
*                      provide global access to variable 'alogOut'
*
******************************************************************************/
#ifndef ALOGOUT_H
#define ALOGOUT_H

/* Needed for definition of "eVarType" */
#include "ChartRec.h"

/*--------------------------------------------------------------------------
                             GLOBAL CONSTANTS
  --------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
                               GLOBAL TYPES
  --------------------------------------------------------------------------*/
/* Ids of all the analog outputs */
typedef enum 
{
    ALOG_OUT0,			// P3-C19
    ALOG_OUT1,			// P3-C17
    ALOG_OUT2,			// P3-C15
    ALOG_OUT3,			// P3-C13
    ALOG_OUT4,			// P3-C11
    ALOG_OUT5,			// P3-C9
    ALOG_OUT6,			// P3-C7
    ALOG_OUT7,			// P3-C5
    NUM_ALOGOUTS
} eAlogOutId;

typedef enum
{
    ALOGOUT_QUANTA,	/* Outputs raw DAC values */
    ALOGOUT_DATA,	/* Outputs scaled variable values */
    ALOGOUT_RAMP,	/* Outputs a ramp signal */
    ALOGOUT_MAX,	/* Outputs the max DAC value */
    ALOGOUT_MIN		/* Outputs the min DAC value */
} eAlogOutState;

/*--------------------------------------------------------------------------
                             MODULE DATA TYPES
  --------------------------------------------------------------------------*/
typedef struct
{
    INT_32      maxScale;
    INT_32      minScale;
    BOOLEAN     clamp;
    BOOLEAN     enabled;
#ifdef BIPOLAR
    INT_16  * dacPtr;
#else
    UINT_16 * dacPtr;
#endif
    void			*varPtr;
    eVarType		varType;
    eAlogOutState	state;
    UINT_16			rampStepSize;
    INT_32			value;  
} AlogOut_t;

/*--------------------------------------------------------------------------
                              GLOBAL VARIABLES
  --------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
                              GLOBAL PROTOTYPES
  --------------------------------------------------------------------------*/
EXPORT void AO_Init(void);
EXPORT void AO_SetEnable(eAlogOutId aId, BOOLEAN aEnable);
EXPORT void AO_SetScaling(eAlogOutId aId, INT_32 aMaxScale, INT_32 aMinScale);
EXPORT void AO_SetState(eAlogOutId aId, eAlogOutState aState);
EXPORT void AO_SetClamp(eAlogOutId aId, BOOLEAN aClamp);
EXPORT void AO_SetVariable(eAlogOutId aId, void *aVarPtr, eVarType aVarType);
EXPORT void AO_SetQuanta(eAlogOutId aId, INT_32 aQuanta);
EXPORT void AO_SetRampStepSize(eAlogOutId aId, UINT_16 aRampSize);
EXPORT void AO_Service(void);
EXPORT void AO_Ramp(UINT_16 aId);
EXPORT void AO_Data(UINT_16 aId);

#endif
