/*****************************************************************************
*
* Copyright (C) 2010 Bombardier
*
* File Name: AlogOut.c
*
* Revision History:
*   05/31/2010 - das - Created
*   10/22/2010 - prs - Remove definition of structure AlogOut_t from here to 
                       provide global access to variable 'alogOut'
                     - Removed classifier "static" from declaration of 
                       alogOut to provide it global access
*
*
******************************************************************************/
///   \file
///   This file contains the code that update the high precision analog outputs

#define alogout_C

/*--------------------------------------------------------------------------
                              INCLUDE FILES
  --------------------------------------------------------------------------*/
#ifndef _DEBUG
    #include <c166.h>
    #include <reg167.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include "Types.h"
#include "AlogOut.h"

/*--------------------------------------------------------------------------
                             MODULE CONSTANTS
  --------------------------------------------------------------------------*/
//#define BIPOLAR

/* The DACs output range is:
	
	MAX -->  5 volts
	MIN -->  0 volts

*/
#define DAC_SIZE_QUANTA					4096l  

#define DAC_MAX							(DAC_SIZE_QUANTA - 1)
#define DAC_MIN							(0)
#define DAC_MID							(DAC_SIZE_QUANTA/2)
/*--------------------------------------------------------------------------
                              MODULE MACROS
  --------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------
                              MODULE VARIABLES
  --------------------------------------------------------------------------*/
//static AlogOut_t alogOut[NUM_ALOGOUTS];  /*  */
AlogOut_t alogOut[NUM_ALOGOUTS];  /* removed "static" by Pranav  */

#ifdef _DEBUG
UINT_16 dac[NUM_ALOGOUTS];
#endif
/*--------------------------------------------------------------------------
                             MODULE PROTOTYPES
  --------------------------------------------------------------------------*/

#ifdef _DEBUG
static void AO_Print(void);
#endif

//--------------------------------------------------------------------------
// Module:
//  AO_Init
//   
///   This function is responsible for initializing all analog output 
///   channels. This function must be called prior to invoking AO_Service(). 
/// 
//--------------------------------------------------------------------------
void AO_Init(void)
{
    UINT_16 index;

	UINT_16 *dacAddress[] = 
	{
#ifdef _DEBUG
		&dac[0],
		&dac[1],
		&dac[2],
		&dac[3],
		&dac[4],
		&dac[5],
		&dac[6],
		&dac[7],
#else
		/* This is the memory mapped addresses to update the 
		   high precision DAC */
		(UINT_16 *)0x500030,	
		(UINT_16 *)0x500032,
		(UINT_16 *)0x500034,
		(UINT_16 *)0x500036,
		(UINT_16 *)0x500038,
		(UINT_16 *)0x50003A,
		(UINT_16 *)0x50003C,
		(UINT_16 *)0x50003E,
#endif
	};

    for (index = 0; index < NUM_ALOGOUTS; index++)
    {
        alogOut[index].maxScale  = 1;
        alogOut[index].minScale  = 1;
        alogOut[index].clamp     = TRUE;
        alogOut[index].enabled   = FALSE;
        alogOut[index].value     = 0;
        alogOut[index].state     = ALOGOUT_DATA;
        alogOut[index].dacPtr    = dacAddress[index];
        alogOut[index].varPtr    = NULL;
    }

}

//--------------------------------------------------------------------------
// Module:
//  AO_SetEnable
// 
///   This function sets the enable member of a analog output. If 
///   enable is TRUE, the analog output will be updated based on 
///   the state. When FALSE, the analog output will not be updated 
///   and will maintain the previous output prior to being updated. 
///
///   \param aId - the id of the analog output
///   \param aEnable - TRUE to enable the output; FALSE to disable it 
///
//--------------------------------------------------------------------------
void AO_SetEnable(eAlogOutId aId, BOOLEAN aEnable)
{
    alogOut[aId].enabled   = aEnable;
}

//--------------------------------------------------------------------------
// Module:
//  AO_SetScaling
// 
///   This function sets the maxScale and minScale of a analog output.
///   For example, if one is outputting a variable that ranges from 0 to 1000 and 
///   wants to use the full dynamic range of the analog output, then 1000 and 0 
///   should be passed as arguments. Using the same variable, if one wants to 
///   "zoom in" on the variable, the values 250 and 0 could be passed as 
///   arguments. As long as the structure member clamp is FALSE, then a 
///   "wrap-around" will occur when the variable is exceeds 250.
///
///   \param aId - the id of the analog output
///   \param aMaxScale - the maximum value of the variable
///   \param aMinScale - the minimum value of the variable
///
//--------------------------------------------------------------------------
void AO_SetScaling(eAlogOutId aId, INT_32 aMaxScale, INT_32 aMinScale)
{
    alogOut[aId].maxScale   = aMaxScale;
    alogOut[aId].minScale   = aMinScale;
}

//--------------------------------------------------------------------------
// Module:
//  AO_SetState
// 
///   This function sets the state of the analog output. The state 
///   allows either a variable to be output, a ramp pattern, analog output 
///   minimum or analog output maximum. Also, one can output a raw binary 
///   DAC quantized value.
///
///   \param aId - the id of the analog output
///   \param aState -  the desired state of the analog output
///
//--------------------------------------------------------------------------
void AO_SetState(eAlogOutId aId, eAlogOutState aState)
{
    alogOut[aId].state      = aState;
    alogOut[aId].value      = 0;
}

//--------------------------------------------------------------------------
// Module:
//  AO_SetClamp
//  
///   This function sets the state of the clamp member for the analog output.
///   If the analog output clamp is TRUE and the state is ALOGOUT_DATA, 
//    the DAC will clamp the analog output to either the DAC_MAX or
///   DAC_MIN if the calculated value exceeds these bounds (the output will 
///   not rollover). If FALSE, then no clamping is applied.
///
///   \param aId - id of the analog output
///   \param aClamp - TRUE to clamp the output, FALSE to enable "wrap-around"
///
//--------------------------------------------------------------------------
void AO_SetClamp(eAlogOutId aId, BOOLEAN aClamp)
{
    alogOut[aId].clamp   = aClamp;
}

//--------------------------------------------------------------------------
// Module:
//  AO_SetVariable
//  
///   This function sets the variable to output. The size and type of the 
///   variable must be passed as an argument so that the pointer access 
///   is done properly. If an incompatible type is passed, the output 
///   will be wrong. For example, if the variable to be output is declared 
///   as an INT_16, the type passed must be INT_16_TYPE.
///
///   \param aId - the id of the analog output
///   \param aVarPtr - the variable to output
///   \param aVarType - the variable type (e.g. UINT_16, INT_32)
///
//--------------------------------------------------------------------------
void AO_SetVariable(eAlogOutId aId, void *aVarPtr, eVarType aVarType)
{
    alogOut[aId].varPtr   = aVarPtr;
    alogOut[aId].varType  = aVarType;
}


//--------------------------------------------------------------------------
// Module:
//  AO_SetQuanta
//  
///   This function sets the state to of the analog output to 
///   ALOGOUT_QUANTA and assigns the value. The application software must 
///   be aware of the analog output channel DAC specs (i.e. UNIPOLAR or 
///   BIPOLAR; DAC Size (number of bits), etc.) before calling this 
///   function. No bounds checks are made.
///
///   \param aId - the id of the analog output
///   \param aQuanta - the value directly written to the analog output DAC 
///
//--------------------------------------------------------------------------
void AO_SetQuanta(eAlogOutId aId, INT_32 aQuanta)
{
    alogOut[aId].state = ALOGOUT_QUANTA;
    alogOut[aId].value = aQuanta;
}


//--------------------------------------------------------------------------
// Module:
//  AO_SetRampStepSize
//  
///   This function sets the ramp step size. This allows the application to 
///   change the ramp step size when the state is ALOGOUT_RAMP. Therefore, 
///   while a channel is ramping, the slope of the ramp can be changed.
///
///   \param aId - the id of the analog output
///   \param aRampStepSize - the desired ramp step size
///
//--------------------------------------------------------------------------
void AO_SetRampStepSize(eAlogOutId aId, UINT_16 aRampStepSize)
{
    alogOut[aId].rampStepSize  = aRampStepSize;
}

//--------------------------------------------------------------------------
// Module:
//  AO_Service
//  
///   This function is responsible for outputting the analog output data. 
///   It determines if a channel is enabled. If so, the state of the channel 
///   determines what is output. This function should be called periodically,
///   typically from a timer interrupt service routine to avoid jittery  
///   analog outputs. 
///
//--------------------------------------------------------------------------
void AO_Service(void)
{
    UINT_16 index;

    /* Scan all analog output channels */
    for (index = 0; index < NUM_ALOGOUTS; index++)
    {
        /* Is the channel enabled */
        if (alogOut[index].enabled)
        {
            switch (alogOut[index].state)
            {
                /* Output the quanta value directly */
                case ALOGOUT_QUANTA:
                    *alogOut[index].dacPtr = alogOut[index].value;
                    break;

                /* Output variable data if a variable is set; otherwise
                   output the DAC midpoint. */
                case ALOGOUT_DATA:
                    if (alogOut[index].varPtr != NULL)
                    {
                        AO_Data(index);
                    }
                    else
                    {
                        alogOut[index].value = DAC_MID;
                    }
                    break;

                /* Ramp the analog output (sawtooth)*/
                case ALOGOUT_RAMP:
                    AO_Ramp(index);
                    break;

                /* Output the maximum value of the DAC */
                case ALOGOUT_MAX:
                    alogOut[index].value = DAC_MAX;
                    break;

                /* Output the minimum value of the DAC */
                case ALOGOUT_MIN:
                default:
                    alogOut[index].value = DAC_MIN;
                    break;

            }

            /* Update the analog output */
            if (alogOut[index].state != ALOGOUT_QUANTA)
            {
#ifdef BIPOLAR
                /* Subtract half of the DAC's dynamic range to level shift for a
                   BIPOLAR DAC. */
                *alogOut[index].dacPtr = (INT_16)(alogOut[index].value - (DAC_SIZE_QUANTA/2));
#else
                *alogOut[index].dacPtr = (UINT_16)(alogOut[index].value);
#endif
            }

        }

    }

}

//--------------------------------------------------------------------------
// Module:
//  AO_Ramp
//  
///   This function ramps the analog output in a sawtooth fashion.
/// 
///   \param aId - the id of the analog output
///
//--------------------------------------------------------------------------
void AO_Ramp(UINT_16 aId)
{
    /* Increment the ramp by the step size. */
    alogOut[aId].value += alogOut[aId].rampStepSize;
    /* If the value exceeds the DAC bounds, wrap the value around. */
    if (alogOut[aId].value >= DAC_SIZE_QUANTA)
    {
        alogOut[aId].value %= DAC_SIZE_QUANTA;
    }
}

//--------------------------------------------------------------------------
// Module:
//  AO_Data
//  
///   This function calculates the DAC quanta when the channel is in 
///   ALOGOUT_DATA mode. If the DAC is BIPOLAR, value is adjusted in 
///   AO_Service().
/// 
///   \param aId - the id of the analog output 
///
//--------------------------------------------------------------------------
void AO_Data(UINT_16 aId)
{
    INT_32  scaleDifference;
    UINT_32 unsignedValue;
    INT_32  value;
    INT_32  dacValue;

    /* Calculate the scaling to be used based on the max and min scaling */
    if (alogOut[aId].maxScale != alogOut[aId].minScale)
    {
		/* Handle the case where the user accidentally makes the minScale
		   larger than the max scale. That is why "abs" is used. */
        scaleDifference = abs(alogOut[aId].maxScale - alogOut[aId].minScale);
    }
    else
    {
        scaleDifference = 1;
    }

    /* Read the channel variable's value */
    switch (alogOut[aId].varType)
    {
        case UINT_8_TYPE:
            unsignedValue = *(UINT_8 *)alogOut[aId].varPtr;
            value = (INT_32)unsignedValue;
            break;
        case UINT_16_TYPE:
            unsignedValue = *(UINT_16 *)alogOut[aId].varPtr;
            value = (INT_32)unsignedValue;
            break;
        case UINT_32_TYPE:
            unsignedValue = *(UINT_32 *)alogOut[aId].varPtr;
            value = (INT_32)unsignedValue;
            break;
        case INT_8_TYPE:
            value   = *(INT_8 *)alogOut[aId].varPtr;
            break;
        case INT_16_TYPE:
            value   = *(INT_16 *)alogOut[aId].varPtr;
            break;
        case INT_32_TYPE:
        default:
            value   = *(INT_32 *)alogOut[aId].varPtr;
            break;
    }

    /* Calculate the DAC value in quanta. */
    dacValue = (value - alogOut[aId].minScale) * (DAC_SIZE_QUANTA - 1) / scaleDifference;

    /* Clamp the DAC output if the dacValue exceeds the DAC's dynamic range
       and clamp is TRUE */
    if (alogOut[aId].clamp)
    {
        if (dacValue > DAC_MAX)
        {
            dacValue = DAC_MAX;
        }
        else if (dacValue < DAC_MIN)
        {
            dacValue = DAC_MIN;
        }
    }
    /* Allow the channel to "rollover" if dacValue exceeds the DAC's dynamic
       range. */
    else
    {
        if (dacValue > DAC_MAX)
        {
            dacValue %= DAC_SIZE_QUANTA;
        }
        else if (dacValue < DAC_MIN)
        {
            dacValue = DAC_MIN - dacValue;
            dacValue %= DAC_SIZE_QUANTA;
            if (dacValue != 0)
            {
                dacValue = DAC_SIZE_QUANTA - dacValue;
            }
        }
    }

    /* Update the value with the calculated DAC value. */
    alogOut[aId].value = dacValue;
}

#ifdef _DEBUG
static void AO_Print(void)
{
    UINT_16 index;
    printf("Chnl   Value   Enable\n");

    for (index = 0; index < NUM_ALOGOUTS; index++)
    {
        printf("%1d %8d %8d\n",index,*alogOut[index].dacPtr,alogOut[index].enabled);
    }
    puts("\n");

}
#endif

