/*****************************************************************************
*
* Copyright (C) 2010 Bombardier
*
* File Name: AnalogIn.h
*
* Abstract: Header file for the Analog Input driver
*
* Revision History:
*   08/31/2009 - das - Created
*   10/22/2010 - prs - Included definition of structure AnalogIn_t here to 
*                      provide global access to variable 'analogInput'
*
******************************************************************************/
#ifndef ANALOGIN_H
#define ANALOGIN_H

/*--------------------------------------------------------------------------
                             GLOBAL CONSTANTS
  --------------------------------------------------------------------------*/
#define MAX_ADC_QUANTA				1023
#define MIN_ADC_QUANTA				0
/*--------------------------------------------------------------------------
                               GLOBAL TYPES
  --------------------------------------------------------------------------*/
typedef enum 
{
    ANALOGIN_00,	/* P3-A5  (AIN_P_0)   P3-B5  (AIN_N_0)  */
    ANALOGIN_01,	/* P3-A7  (AIN_P_1)   P3-B7  (AIN_N_1)  */
    ANALOGIN_02,	/* P3-A8  (AIN_P_2)   P3-B8  (AIN_N_2)  */
    ANALOGIN_03,	/* P3-A6  (AIN_P_3)   P3-B6  (AIN_N_3)  */
    ANALOGIN_04,	/* P3-A12 (AIN_P_4)   P3-B12 (AIN_N_4)  */
    ANALOGIN_05,	/* P3-A10 (AIN_P_5)   P3-B10 (AIN_N_5)  */
    ANALOGIN_06,	/* P3-A9  (AIN_P_6)   P3-B9  (AIN_N_6)  */
    ANALOGIN_07,	/* P3-A11 (AIN_P_7)   P3-B11 (AIN_N_7)  */
    ANALOGIN_08,	/* P3-A15 (AIN_P_8)   P3-B15 (AIN_N_8)  */
    ANALOGIN_09,	/* P3-A15 (AIN_P_9)   P3-B15 (AIN_N_9)  */
    ANALOGIN_10,	/* P3-A16 (AIN_P_10)  P3-B16 (AIN_N_10) */
    ANALOGIN_11,	/* P3-A17 (AIN_P_11)  P3-B17 (AIN_N_11) */
    ANALOGIN_12,	/* P3-A18 (AIN_P_12)  P3-B18 (AIN_N_12) */
    ANALOGIN_13,	/* P3-A19 (AIN_P_13)  P3-B19 (AIN_N_13) */
    ANALOGIN_14,	/* B+ voltage */
    ANALOGIN_15,	/* On board temp sensor */
    NUM_ANALOGINS
} eAnalogInId;

typedef enum
{
    ANALOGIN_DATA,
    ANALOGIN_OFFSET_CORRECTION
} eAnalogInState;

typedef INT_32 (*FilterPtr)(eAnalogInId aId, INT_32 aData);


/*--------------------------------------------------------------------------
                              GLOBAL VARIABLES
  --------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
                              GLOBAL PROTOTYPES
  --------------------------------------------------------------------------*/
EXPORT void AI_Init(void);
EXPORT void AI_Service(void);
EXPORT void AI_ProcessInput(eAnalogInId aId);
EXPORT void AI_TriggerADC(eAnalogInId aId);
EXPORT void AI_SetScaleFactors(eAnalogInId aId, INT_32 aMaxADC, INT_32 aMinADC, 
                        INT_32 aMaxScale, INT_32 aMinScale);
EXPORT void AI_SetEnable(eAnalogInId aId, BOOLEAN aEnable);
EXPORT void AI_SetBipolar(eAnalogInId aId, BOOLEAN aBipolar);
EXPORT void AI_SetDataFilter(eAnalogInId aId, FilterPtr aFilterPtr);
EXPORT void AI_SetOffCorrFilter(eAnalogInId aId, FilterPtr aFilterPtr);
EXPORT void AI_SetState(eAnalogInId aId, eAnalogInState aState);
EXPORT eAnalogInState AI_GetState(eAnalogInId aId);
EXPORT INT_32 AI_GetOffCorrValueADC(eAnalogInId aId);
EXPORT void AI_SetOffCorrValueADC(eAnalogInId aId, INT_32 aValue);
EXPORT INT_32 AI_GetDataValueADC(eAnalogInId aId);
EXPORT INT_32 AI_GetValue(eAnalogInId aId);
EXPORT void AI_Scale(eAnalogInId aId);

typedef struct
{
    BOOLEAN         biPolar;    /* Only set this to TRUE if raw ADC values
                                   need to be 2's complement */
    INT_32          maxScale;   /* The maximum scaling factor (units), should
                                   correspond to maxADC. */
    INT_32          minScale;   /* The minimum scaling factor (units), should
                                   correspond to minADC. */
    INT_32          diffScale;  /* The difference between maxScale and minScale
                                   exists to make scaling algorithm quicker. */
    BOOLEAN         scale;      /* TRUE if the application wants to convert from
                                   quanta to units (e.g. current) */
    INT_32          maxADC;     /* The maximum ADC value; should correspond to 
                                   maxScale. */
    INT_32          minADC;     /* The minimum ADC value; should correspond to 
                                   minScale. */
    INT_32          diffADC;    /* The difference between maxADC and minADC
                                   exists to make scaling algorithm quicker. */
    BOOLEAN         enabled;    /* TRUE if the ADC channel is enabled */
    INT_32          dataValueADC;   /* raw ADC value collected in state 
                                       ANALOGIN_DATA state */
    INT_32          offcorrValueADC;/* raw ADC value collected in state 
                                       ANALOGIN_OFFSET_CORRECTION state*/
    INT_32          value;      /* the final result after all filtering (if any)
                                   and scaling (if any) */
    FilterPtr       DataFilterPtr;      /* The function pointer to the filter used
                                           to filter the data while in state 
                                           ANALOGIN_DATA. The filter must be 
                                           supplied by the application. 
                                           If no filtering is needed, the value 
                                           will be set to NULL. */
    FilterPtr       OffCorrFilterPtr;   /* The function pointer to the filter used
                                           to filter the data while in state 
                                           ANALOGIN_OFFSET_CORRECTION. The filter 
                                           must be supplied by the application. 
                                           If no filtering is needed, the value 
                                           will be set to NULL. */
    eAnalogInState  state;      /* The state of the analog input (either normal
                                   (ANALOGIN_DATA) or offset correction 
                                   (ANALOGIN_OFFSET_CORRECTION). */
} AnalogIn_t;

#ifdef _DEBUG
EXPORT void AI_InjectSample(UINT_16 aId, INT_32 aValue);
#endif

#endif
