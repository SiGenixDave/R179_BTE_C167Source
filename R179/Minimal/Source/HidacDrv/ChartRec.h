/*****************************************************************************
*
* Copyright (C) 2010 Bombardier
*
* File Name: ChartRec.h
*
* Abstract: Header file for the chart recorder driver
*
* Revision History:
*   08/31/2009 - das - Created
*
******************************************************************************/
#ifndef CHARTREC_H
#define CHARTREC_H

/*--------------------------------------------------------------------------
                             GLOBAL CONSTANTS
  --------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
                               GLOBAL TYPES
  --------------------------------------------------------------------------*/
typedef enum 
{
    CHART_REC1,	/* DB25-Pin 1 */
    CHART_REC2,	/* DB25-Pin 2 */
    CHART_REC3,	/* DB25-Pin 3 */
    CHART_REC4,	/* DB25-Pin 4 */
    CHART_REC5,	/* DB25-Pin 5 */
    CHART_REC6,	/* DB25-Pin 6 */
    CHART_REC7,	/* DB25-Pin 7 */
    CHART_REC8,	/* DB25-Pin 8 */
    NUM_CHARTRECS
} eChartRecId;

typedef enum
{
    CHART_QUANTA,	/* Outputs raw DAC values */
    CHART_DATA,		/* Outputs scaled variable values */
    CHART_RAMP,		/* Outputs a ramp signal */
    CHART_MAX,		/* Outputs the max DAC value */
    CHART_MIN		/* Outputs the min DAC value */
} eChartState;

typedef enum
{
    UINT_8_TYPE,
    UINT_16_TYPE,
    UINT_32_TYPE,
    INT_8_TYPE,
    INT_16_TYPE,
    INT_32_TYPE
} eVarType;


/*--------------------------------------------------------------------------
                              GLOBAL VARIABLES
  --------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
                              GLOBAL PROTOTYPES
  --------------------------------------------------------------------------*/
EXPORT void CR_Init(void);
EXPORT void CR_SetEnable(eChartRecId aId, BOOLEAN aEnable);
EXPORT void CR_SetScaling(eChartRecId aId, INT_32 aMaxScale, INT_32 aMinScale);
EXPORT void CR_SetState(eChartRecId aId, eChartState aState);
EXPORT void CR_SetClamp(eChartRecId aId, BOOLEAN aClamp);
EXPORT void CR_SetVariable(eChartRecId aId, void *aVarPtr, eVarType aVarType);
EXPORT void CR_SetQuanta(eChartRecId aId, INT_32 aQuanta);
EXPORT void CR_SetRampStepSize(eChartRecId aId, UINT_16 aRampSize);
EXPORT void CR_Service(void);
EXPORT void CR_Ramp(UINT_16 aId);
EXPORT void CR_Data(UINT_16 aId);

#endif
