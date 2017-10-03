/*****************************************************************************
*
* Copyright (C) 2010 Bombardier
*
* File Name: ChartRec.c
*
* Revision History:
*   08/31/2009 - das - Created
*
******************************************************************************/
///   \file
///   This file contains the code that update the chart recorder outputs

#define chartrec_C

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
#include "ChartRec.h"

/*--------------------------------------------------------------------------
                             MODULE CONSTANTS
  --------------------------------------------------------------------------*/
//#define BIPOLAR

/* The DACs output range is:
	
	DAC_MAX --> +5 volts
	DAC_MIN --> -5 volts

*/
#define DAC_SIZE_QUANTA					4096l  

#define DAC_MAX							(DAC_SIZE_QUANTA - 1)
#define DAC_MIN							(0)
#define DAC_MID							(DAC_SIZE_QUANTA/2)
/*--------------------------------------------------------------------------
                              MODULE MACROS
  --------------------------------------------------------------------------*/


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
    void        *varPtr;
    eVarType    varType;
    eChartState state;
    UINT_16     rampStepSize;
    INT_32      value;  
} ChartRec_t;


/*--------------------------------------------------------------------------
                              MODULE VARIABLES
  --------------------------------------------------------------------------*/
static ChartRec_t chartRec[NUM_CHARTRECS];  /*  */

#ifdef _DEBUG
UINT_16 cr[NUM_CHARTRECS];
#endif
/*--------------------------------------------------------------------------
                             MODULE PROTOTYPES
  --------------------------------------------------------------------------*/

#ifdef _DEBUG
static void CR_Print(void);
#endif

//--------------------------------------------------------------------------
// Module:
//  CR_Init
//   
///   This function is responsible for initializing all chart recorder 
///   channels. This function must be called prior to invoking CR_Service(). 
/// 
//--------------------------------------------------------------------------
void CR_Init(void)
{
    UINT_16 index;

	UINT_16 *dacAddress[] = 
	{
#ifdef _DEBUG
		&cr[0],
		&cr[1],
		&cr[2],
		&cr[3],
		&cr[4],
		&cr[5],
		&cr[6],
		&cr[7],
#else
		/* This is the memory mapped addresses to update the chart recorder
		   outputs on the attached DAC Card */
		(UINT_16 *)0x500056,	/* CHART1: DB25-Pin 1 */
		(UINT_16 *)0x500054,	/* CHART2: DB25-Pin 2 */
		(UINT_16 *)0x500052,	/* CHART3: DB25-Pin 3 */
		(UINT_16 *)0x500050,	/* CHART4: DB25-Pin 4 */
		(UINT_16 *)0x50005E,	/* CHART5: DB25-Pin 5 */
		(UINT_16 *)0x50005C,	/* CHART6: DB25-Pin 6 */
		(UINT_16 *)0x50005A,	/* CHART7: DB25-Pin 7 */
		(UINT_16 *)0x500058,	/* CHART8: DB25-Pin 8 */
#endif
	};

    for (index = 0; index < NUM_CHARTRECS; index++)
    {
        chartRec[index].maxScale  = 1;
        chartRec[index].minScale  = 1;
        chartRec[index].clamp     = TRUE;
        chartRec[index].enabled   = FALSE;
        chartRec[index].value     = 0;
        chartRec[index].state     = CHART_DATA;
        chartRec[index].dacPtr    = dacAddress[index];
        chartRec[index].varPtr    = NULL;
    }

}

//--------------------------------------------------------------------------
// Module:
//  CR_SetEnable
// 
///   This function sets the enable member of a chart recorder output. If 
///   enable is TRUE, the chart recorder channel will be updated based on 
///   the state. When FALSE, the chart recorder channel will not be updated 
///   and will maintain the previous output prior to being updated. 
///
///   \param aId - the chart recorder channel 
///   \param aEnable - TRUE to enable the channel; FALSE to disable it 
///
//--------------------------------------------------------------------------
void CR_SetEnable(eChartRecId aId, BOOLEAN aEnable)
{
    chartRec[aId].enabled   = aEnable;
}

//--------------------------------------------------------------------------
// Module:
//  CR_SetScaling
// 
///   This function sets the maxScale and minScale of a chart recorder channel.
///   For example, if one is charting a variable that ranges from 0 to 1000 and 
///   wants to use the full dynamic range of the chart output, then 1000 and 0 
///   should be passed as arguments. Using the same variable, if one wants to 
///   "zoom in" on the variable, the values 250 and 0 could be passed as 
///   arguments. As long as the structure member clamp is FALSE, then a 
///   "wrap-around" will occur when the variable is exceeds 250.
///
///   \param aId - the chart recorder channel 
///   \param aMaxScale - the maximum value of the variable
///   \param aMinScale - the minimum value of the variable
///
//--------------------------------------------------------------------------
void CR_SetScaling(eChartRecId aId, INT_32 aMaxScale, INT_32 aMinScale)
{
    chartRec[aId].maxScale   = aMaxScale;
    chartRec[aId].minScale   = aMinScale;
}

//--------------------------------------------------------------------------
// Module:
//  CR_SetState
// 
///   This function sets the state of the chart recorder channel. The state 
///   allows either a variable to be output, a ramp pattern, chart minimum 
///   or chart maximum. Also, one output a raw binary DAC quantize value.
///
///   \param aId - the chart recorder channel 
///   \param aState -  the desired state of the chart recorder channel
///
//--------------------------------------------------------------------------
void CR_SetState(eChartRecId aId, eChartState aState)
{
    chartRec[aId].state      = aState;
    chartRec[aId].value      = 0;
}

//--------------------------------------------------------------------------
// Module:
//  CR_SetClamp
//  
///   This function sets the state of the clamp member for the chart recorder 
///   channel. If the chart recorder clamp is TRUE and the state is CHART_DATA , 
//    the DAC will clamp the chart recorder channel to either the DAC_MAX or
///   DAC_MIN if the calculated value exceeds these bounds (the output will 
///   not rollover). If FALSE, then no clamping is applied.
///
///   \param aId - the chart recorder channel 
///   \param aClamp - 
///
//--------------------------------------------------------------------------
void CR_SetClamp(eChartRecId aId, BOOLEAN aClamp)
{
    chartRec[aId].clamp   = aClamp;
}

//--------------------------------------------------------------------------
// Module:
//  CR_SetVariable
//  
///   This function sets the variable to chart. The size and type of the 
///   variable must be passed as an argument so that the pointer access 
///   is done properly. If an incompatible type is passed, the output 
///   will be wrong. For example, of the variable to be charted is declared 
///   as an INT_16, the type passed must be INT_16_TYPE.
///
///   \param aId - the chart recorder channel id
///   \param aVarPtr - the variable to chart 
///   \param aVarType - the variable type (e.g. UINT_16, INT_32)
///
//--------------------------------------------------------------------------
void CR_SetVariable(eChartRecId aId, void *aVarPtr, eVarType aVarType)
{
    chartRec[aId].varPtr   = aVarPtr;
    chartRec[aId].varType  = aVarType;
}


//--------------------------------------------------------------------------
// Module:
//  CR_SetQuanta
//  
///   This function sets the state to of the chart recorder channel to 
///   CHART_QUANTA and assigns the value. The application software must 
///   be aware of the chart recorder channel DAC specs (i.e. UNIPOLAR or 
///   BIPOLAR; DAC Size (number of bits), etc.) before calling this 
///   function. No bounds checks are made.
///
///   \param aId - the chart recorder channel 
///   \param aQuanta - the value directly written to the chart recorder channel 
///                    DAC 
///
//--------------------------------------------------------------------------
void CR_SetQuanta(eChartRecId aId, INT_32 aQuanta)
{
    chartRec[aId].state = CHART_QUANTA;
    chartRec[aId].value = aQuanta;
}


//--------------------------------------------------------------------------
// Module:
//  CR_SetRampStepSize
//  
///   This function sets the ramp step size. This allows the application to 
///   change the ramp step size when the state is CHART_RAMP. Therefore, 
///   while a channel is ramping, the slope of the ramp can be changed.
///
///   \param aId - the chart recorder channel 
///   \param aRampStepSize - the desired ramp step size
///
//--------------------------------------------------------------------------
void CR_SetRampStepSize(eChartRecId aId, UINT_16 aRampStepSize)
{
    chartRec[aId].rampStepSize  = aRampStepSize;
}

//--------------------------------------------------------------------------
// Module:
//  CR_Service
//  
///   This function is responsible for outputting the chart recorder data. 
///   It determines if a channel is enabled. If so, the state of the channel 
///   determines what is output. This function should be called periodically,
///   typically from a timer interrupt service routine to avoid jittery chart 
///   recorder outputs. 
///
//--------------------------------------------------------------------------
void CR_Service(void)
{
    UINT_16 index;

    /* Scan all chart recorder channels */
    for (index = 0; index < NUM_CHARTRECS; index++)
    {
        /* Is the channel enabled */
        if (chartRec[index].enabled)
        {
            switch (chartRec[index].state)
            {
                /* Output the quanta value directly */
                case CHART_QUANTA:
                    *chartRec[index].dacPtr = chartRec[index].value;
                    break;

                /* Output variable data if a variable is set; otherwise
                   output the DAC midpoint. */
                case CHART_DATA:
                    if (chartRec[index].varPtr != NULL)
                    {
                        CR_Data(index);
                    }
                    else
                    {
                        chartRec[index].value = DAC_MID;
                    }
                    break;

                /* Ramp the chart recorder output (sawtooth)*/
                case CHART_RAMP:
                    CR_Ramp(index);
                    break;

                /* Output the maximum value of the DAC */
                case CHART_MAX:
                    chartRec[index].value = DAC_MAX;
                    break;

                /* Output the minimum value of the DAC */
                case CHART_MIN:
                default:
                    chartRec[index].value = DAC_MIN;
                    break;

            }

            /* Update the chart recorder output */
            if (chartRec[index].state != CHART_QUANTA)
            {
#ifdef BIPOLAR
                /* Subtract half of the DAC's dynamic range to level shift for a
                   BIPOLAR DAC. */
                *chartRec[index].dacPtr = (INT_16)(chartRec[index].value - (DAC_SIZE_QUANTA/2));
#else
                *chartRec[index].dacPtr = (UINT_16)(chartRec[index].value);
#endif
            }

        }

    }

}

//--------------------------------------------------------------------------
// Module:
//  CR_Ramp
//  
///   This function ramps the chart recorder channel output in a sawtooth 
///   fashion.
/// 
///   \param aId - the chart recorder channel 
///
//--------------------------------------------------------------------------
void CR_Ramp(UINT_16 aId)
{
    /* Increment the ramp by the step size. */
    chartRec[aId].value += chartRec[aId].rampStepSize;
    /* If the value exceeds the DAC bounds, wrap the value around. */
    if (chartRec[aId].value >= DAC_SIZE_QUANTA)
    {
        chartRec[aId].value %= DAC_SIZE_QUANTA;
    }
}

//--------------------------------------------------------------------------
// Module:
//  CR_Data
//  
///   This function calculates the DAC quanta when the channel is in 
///   CHART_DATA mode. If the DAC is BIPOLAR, value is adjusted in 
///   CR_Service().
/// 
///   \param aId - the chart recorder channel 
///
//--------------------------------------------------------------------------
void CR_Data(UINT_16 aId)
{
    INT_32  scaleDifference;
    UINT_32 unsignedValue;
    INT_32  value;
    INT_32  dacValue;

    /* Calculate the scaling to be used based on the max and min scaling */
    if (chartRec[aId].maxScale != chartRec[aId].minScale)
    {
		/* Handle the case where the user accidentally makes the minScale
		   larger than the max scale. That is why "abs" is used. */
        scaleDifference = abs(chartRec[aId].maxScale - chartRec[aId].minScale);
    }
    else
    {
        scaleDifference = 1;
    }

    /* Read the channel variable's value */
    switch (chartRec[aId].varType)
    {
        case UINT_8_TYPE:
            unsignedValue = *(UINT_8 *)chartRec[aId].varPtr;
            value = (INT_32)unsignedValue;
            break;
        case UINT_16_TYPE:
            unsignedValue = *(UINT_16 *)chartRec[aId].varPtr;
            value = (INT_32)unsignedValue;
            break;
        case UINT_32_TYPE:
            unsignedValue = *(UINT_32 *)chartRec[aId].varPtr;
            value = (INT_32)unsignedValue;
            break;
        case INT_8_TYPE:
            value   = *(INT_8 *)chartRec[aId].varPtr;
            break;
        case INT_16_TYPE:
            value   = *(INT_16 *)chartRec[aId].varPtr;
            break;
        case INT_32_TYPE:
        default:
            value   = *(INT_32 *)chartRec[aId].varPtr;
            break;
    }

    /* Calculate the DAC value in quanta. */
    dacValue = (value - chartRec[aId].minScale) * (DAC_SIZE_QUANTA - 1) / scaleDifference;

    /* Clamp the DAC output if the dacValue exceeds the DAC's dynamic range
       and clamp is TRUE */
    if (chartRec[aId].clamp)
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
    chartRec[aId].value = dacValue;
}

#ifdef _DEBUG
static void CR_Print(void)
{
    UINT_16 index;
    printf("Chnl   Value   Enable\n");

    for (index = 0; index < NUM_CHARTRECS; index++)
    {
        printf("%1d %8d %8d\n",index,*chartRec[index].dacPtr,chartRec[index].enabled);
    }
    puts("\n");

}
#endif

