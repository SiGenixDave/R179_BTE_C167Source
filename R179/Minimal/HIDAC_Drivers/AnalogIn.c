/******************************************************************************
*
* Copyright (C) 2010 Bombardier
*
* File Name: AnalogIn.c
*
* Revision History:
*   08/31/2009 - das - Created
*   10/22/2010 - prs - Remove definition of structure AlogIn_t from here to 
                       provide global access to variable 'analogInput'
                     - Removed classifier "static" from declaration of 
                       analogInput to provide it global access
*
******************************************************************************/
///   \file
///   This file contains the code that handles the Analog Inputs

#define analogin_C

/*--------------------------------------------------------------------------
                              INCLUDE FILES
  --------------------------------------------------------------------------*/

#ifndef _DEBUG
    #include <c166.h>
    #include <reg167.h>
#endif

#include <stdio.h>
#include "Types.h"
#include "AnalogIn.h"

/*--------------------------------------------------------------------------
                             MODULE CONSTANTS
  --------------------------------------------------------------------------*/

#define ADC_SIZE_QUANTA					1024l

/*--------------------------------------------------------------------------
                              MODULE MACROS
  --------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------
                             MODULE DATA TYPES
  --------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
                              MODULE VARIABLES
  --------------------------------------------------------------------------*/

/*static AnalogIn_t analogInput[NUM_ANALOGINS];*/ //needs to be made global
AnalogIn_t analogInput[NUM_ANALOGINS];

#ifdef _DEBUG
static INT_32 aSample[NUM_ANALOGINS];
#endif

/*--------------------------------------------------------------------------
                             MODULE PROTOTYPES
  --------------------------------------------------------------------------*/

//--------------------------------------------------------------------------
// Module:
//  AI_Init
///   
///   This function initializes the entire all structure members of 
///   each analog input. 
// 
//--------------------------------------------------------------------------
void AI_Init(void)
{
    UINT_16 index;

    for (index = 0; index < NUM_ANALOGINS; index++)
    {
        analogInput[index].biPolar          = FALSE;
        analogInput[index].enabled          = FALSE;
        analogInput[index].DataFilterPtr    = NULL;
        analogInput[index].OffCorrFilterPtr = NULL;
        analogInput[index].maxADC           = ADC_SIZE_QUANTA;
        analogInput[index].minADC           = 0;
        analogInput[index].diffADC          = ADC_SIZE_QUANTA;
        analogInput[index].maxScale         = ADC_SIZE_QUANTA;
        analogInput[index].minScale         = 0;
        analogInput[index].diffScale        = ADC_SIZE_QUANTA;
        analogInput[index].scale            = FALSE;
        analogInput[index].state            = ANALOGIN_DATA;
        analogInput[index].dataValueADC     = 0;
        analogInput[index].offcorrValueADC  = 0;
        analogInput[index].value            = 0;
    }

}

//--------------------------------------------------------------------------
// Module:
//  AI_Service
//   
///   This function scans through all ADC inputs. It determines if the 
///   application software has enabled the input. If it is enabled, it samples 
///   and processes the input. 
/// 
//--------------------------------------------------------------------------
void AI_Service(void)
{
    UINT_16 index;

    for (index = 0; index < NUM_ANALOGINS; index++)
    {
        /* Does the application want the analog input to be sampled  */
        if (analogInput[index].enabled)
        {
            AI_ProcessInput(index);
        }
    }
}

//--------------------------------------------------------------------------
// Module:
//  AI_ProcessInput
//   
///   This function first triggers the ADC to get a sample. It then adjusts 
///   the sample if it is bipolar. It then filters the data if a filter is 
///   present. It subtracts the offset correction from the data value. It 
///   then scales the data to units if desired.
/// 
///	  \param aId - the id of the analog input 
/// 
//--------------------------------------------------------------------------
void AI_ProcessInput(eAnalogInId aId)
{
    /* Sample the input*/
    AI_TriggerADC(aId);

    /* Adjust the sample if bipolar (DC (or 0) is at half ADC range */
    if (analogInput[aId].biPolar)
    {
        if (analogInput[aId].state == ANALOGIN_DATA)
        {
            analogInput[aId].dataValueADC     -= (ADC_SIZE_QUANTA/2);
        }
        else
        {
            analogInput[aId].offcorrValueADC  -= (ADC_SIZE_QUANTA/2);
        }
    }
    /* Filter the data, if desired */
    if ((analogInput[aId].DataFilterPtr != NULL) && 
        (analogInput[aId].state == ANALOGIN_DATA))
    {
        analogInput[aId].dataValueADC = 
        analogInput[aId].DataFilterPtr(aId, analogInput[aId].dataValueADC);
    }
    else if ((analogInput[aId].OffCorrFilterPtr != NULL) && 
             (analogInput[aId].state == ANALOGIN_OFFSET_CORRECTION))
    {
        analogInput[aId].offcorrValueADC = 
        analogInput[aId].OffCorrFilterPtr(aId, analogInput[aId].offcorrValueADC);
    }

    /* Adjust the data with the offset correction (will be zero if no offset correction) */
    analogInput[aId].dataValueADC -= analogInput[aId].offcorrValueADC;

    /* Scale the data to units */
    if (analogInput[aId].scale)
    {
        AI_Scale(aId);
    }
    /* No scaling desired; set value to the ADC value */
    else
    {
        analogInput[aId].value = analogInput[aId].dataValueADC;
    }

}

//--------------------------------------------------------------------------
// Module:
//  AI_TriggerADC
//   
///   This function is responsible for triggering the proper ADC channel 
///   and waiting for the ADC to complete. Once complete, the state is 
///   examined to determine whether the sample is "real" data or offset 
///   correction data. The code to sample the C167 ADC is legacy code that has 
///   a proven track record of working... modify at your own risk!!!!
/// 
///	  \param aId - the id of the analog input 
/// 
//--------------------------------------------------------------------------
void AI_TriggerADC(eAnalogInId aId)
{
#ifdef _DEBUG
    if (analogInput[aId].state == ANALOGIN_DATA)
    {
        analogInput[aId].dataValueADC = aSample[aId];
    }
    else
    {
        analogInput[aId].offcorrValueADC = aSample[aId];
    }
#else
    UINT_16 timeOut;
    UINT_16 count;
    UINT_16 analogValue;
    UINT_16 readChannel;

    /* added or ADCRQ based on errata ADC.6 */
    timeOut = 0;
    while ( (ADBSY || ADCRQ) && (timeOut < 600) )
    {
        timeOut++;
    }

    if (timeOut < 600)
    {
        /* no timeout, use reading */
        ADCON = 0x0080 | aId;

        /* Clear inj conv request to prevent blocking per errata ADC.7 */
        ADCRQ = 0;
        /* read value with delay for validity */
        count = 0;
        while (ADBSY && count < 100)
        {
            count++;
        }

        analogValue = ADDAT;
        readChannel = ((analogValue & 0xf000) >> 12);
        if (readChannel != aId)
        {
            /* Read second time in case something was stuck in temp latch */
            analogValue = ADDAT;
            readChannel = ((analogValue & 0xf000) >> 12);
            if (readChannel != aId)
            {
                /* results from incorrect channel so throw out read */
                count = 200;
            }
        }
    }
    if ( (count < 100) && (timeOut < 600) )
    {
        /* only use analog value if valid read */
        analogValue &= 0x3ff;

		/* For analog inputs 0 thru 7, the hardware has a LPF IC that inverts
		   the signal around 2.5 volts. Therefore, in order to be consistent with
		   the other inputs (8-13), the signal is inverted so that a positive voltage
		   produces a quanta values from 513 - 1023 and a negative voltage produces
		   quanta values from 0 - 511. */
        if (analogInput[aId].state == ANALOGIN_DATA)
        {
			if (aId <= ANALOGIN_07)
			{
				analogInput[aId].dataValueADC = MAX_ADC_QUANTA - analogValue;
			}
			else
			{
				analogInput[aId].dataValueADC = analogValue;
			}
        }
        else
        {
			if (aId <= ANALOGIN_07)
			{
				analogInput[aId].offcorrValueADC = MAX_ADC_QUANTA - analogValue;
			}
			else
			{
				analogInput[aId].offcorrValueADC = analogValue;
			}
        }

    }

#endif

}

//--------------------------------------------------------------------------
// Module:
//  AI_SetScaleFactors
//   
///   This function allows the application to set all of the scaling information 
///   required to convert from quanta to application units (e.g. current, 
///   volts, etc.).
/// 
///	  \param aId - the id of the analog input 
///	  \param aMaxADC - the maximum ADC value    
///	  \param aMinADC -  the minimum ADC value
///	  \param aMaxScale -  the maximum scaled value (in units)
///	  \param aMinScale -  the minimum scaled value (in units)
/// 
//--------------------------------------------------------------------------
void AI_SetScaleFactors(eAnalogInId aId, INT_32 aMaxADC, INT_32 aMinADC, 
                        INT_32 aMaxScale, INT_32 aMinScale)
{
    analogInput[aId].maxADC     = aMaxADC;
    analogInput[aId].minADC     = aMinADC;
    /* This difference is calculated here so that it is done every time
       scaling is performed (see AI_Scale()). This will conserve some
       CPU time. */
    analogInput[aId].diffADC    = (aMaxADC - aMinADC) + 1;

    analogInput[aId].maxScale   = aMaxScale;
    analogInput[aId].minScale   = aMinScale;
    /* This difference is calculated here so that it is done every time
       scaling is performed (see AI_Scale()). This will conserve some
       CPU time. */
    analogInput[aId].diffScale  = aMaxScale - aMinScale;

    /* Enable scaling */
    analogInput[aId].scale      = TRUE;
}

//--------------------------------------------------------------------------
// Module:
//  AI_SetEnable
//   
///   This function is a mutator of the analog input's enable.
/// 
///	  \param aId - the id of the analog input 
///	  \param aEnable - the new enable setting of the analog input 
/// 
//--------------------------------------------------------------------------
void AI_SetEnable(eAnalogInId aId, BOOLEAN aEnable)
{
    analogInput[aId].enabled    = aEnable;
}

//--------------------------------------------------------------------------
// Module:
//  AI_SetBipolar
//   
///   This function is a mutator of the analog input's bipolar setting.
/// 
///	  \param aId - the id of the analog input 
///	  \param aBipolar - the new bipolar setting of the analog input 
/// 
//--------------------------------------------------------------------------
void AI_SetBipolar(eAnalogInId aId, BOOLEAN aBipolar)
{
    analogInput[aId].biPolar    = aBipolar;
}

//--------------------------------------------------------------------------
// Module:
//  AI_SetDataFilter
//   
///   This function is a mutator of the analog input's Data filter. This 
///   purpose of the function is to provide a filter for the data while 
///   the analog input is sampling data. 
/// 
///	  \param aId - the id of the analog input 
///	  \param aFilterPtr - a function pointer to the analog input data 
///						  filter 
/// 
//--------------------------------------------------------------------------
void AI_SetDataFilter(eAnalogInId aId, FilterPtr aFilterPtr)
{
    analogInput[aId].DataFilterPtr      = aFilterPtr;
}

//--------------------------------------------------------------------------
// Module:
//  AI_SetOffCorrFilter
//   
///   This function is a mutator of the analog input's Offset Correction 
///   filter. The purpose of the function is to provide a filter for the 
///   offset correction data while the analog input is collecting offset 
///   correction data. 
/// 
///	  \param aId - the id of the analog input 
///	  \param aFilterPtr - a function pointer to the analog input offset 
///						  correction filter 
/// 
//--------------------------------------------------------------------------
void AI_SetOffCorrFilter(eAnalogInId aId, FilterPtr aFilterPtr)
{
    analogInput[aId].OffCorrFilterPtr   = aFilterPtr;
}

//--------------------------------------------------------------------------
// Module:
//  AI_SetState
//   
///   This function is a mutator of the analog input's state.
/// 
///	  \param aId - the id of the analog input 
///	  \param aState - the new state of the analog input 
/// 
//--------------------------------------------------------------------------
void AI_SetState(eAnalogInId aId, eAnalogInState aState)
{
    analogInput[aId].state  = aState;
}

//--------------------------------------------------------------------------
// Module:
//  AI_GetState
//   
///   This function is an accessor to the analog input's state.
/// 
///	  \param aId - the id of the analog input 
/// 
///   \return eAnalogInState - the analog input's state
/// 
//--------------------------------------------------------------------------
eAnalogInState AI_GetState(eAnalogInId aId)
{
    return analogInput[aId].state;
}

//--------------------------------------------------------------------------
// Module:
//  AI_GetOffCorrValueADC
//   
///   This function is an accessor to the raw ADC offset correction value.
/// 
///	  \param aId - the id of the analog input 
/// 
///   \return INT_32 - the raw offset correction analog input value
/// 
//--------------------------------------------------------------------------
INT_32 AI_GetOffCorrValueADC(eAnalogInId aId)
{
    return analogInput[aId].offcorrValueADC;
}

//--------------------------------------------------------------------------
// Module:
//  AI_SetOffCorrValueADC
//   
///   This function is mutator of the raw ADC offset correction value. 
///   This function is provided so that the application software can 
///   manually adjust the offset correction. For example, if after offset 
///   correction the offset correction value is determined to be to out 
///   of range, the application can manually set the offset correction 
///   value. 
/// 
///	  \param aId - the id of the analog input 
///   \param aValue - the new raw offset correction analog input value
/// 
//--------------------------------------------------------------------------
void AI_SetOffCorrValueADC(eAnalogInId aId, INT_32 aValue)
{
    analogInput[aId].offcorrValueADC = aValue;
}

//--------------------------------------------------------------------------
// Module:
//  AI_GetDataValueADC
//   
///   This function is an accessor to the raw ADC value. If offset 
///   correction is enabled, it would be the raw ADC value with 
///   offset correction. 
/// 
///	  \param aId - the id of the analog input 
/// 
///   \return INT_32 - the raw analog input value
/// 
//--------------------------------------------------------------------------
INT_32 AI_GetDataValueADC(eAnalogInId aId)
{
    return analogInput[aId].dataValueADC;
}

//--------------------------------------------------------------------------
// Module:
//  AI_GetValue
//   
///   This function is an accessor to the scaled ADC value.
/// 
///	  \param aId - the id of the analog input 
/// 
///   \return INT_32 - the scaling analog input value (user spcified units) 
/// 
//--------------------------------------------------------------------------
INT_32 AI_GetValue(eAnalogInId aId)
{
    return analogInput[aId].value;
}

//--------------------------------------------------------------------------
// Module:
//  AI_Scale
//   
///   This function scales the ADC value into user specified units. The 
///   maximum and minimum ADC values and scale factors are set by calling 
///   AI_SetScaleFactors(). 
/// 
///	  \param aId - the id of the analog input 
/// 
//--------------------------------------------------------------------------
void AI_Scale(eAnalogInId aId)
{
    analogInput[aId].value = 
    ((analogInput[aId].dataValueADC - analogInput[aId].minADC) * analogInput[aId].diffScale
     / 
     analogInput[aId].diffADC) 
    + 
    analogInput[aId].minScale;
}


#ifdef _DEBUG
void AI_InjectSample(UINT_16 aId, INT_32 aValue)
{
    aSample[aId] = aValue;
}

#endif


