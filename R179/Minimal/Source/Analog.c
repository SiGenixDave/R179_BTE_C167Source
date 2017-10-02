/******************************************************************************
*
* Copyright (C) 2014-15 Bombardier
*
* File Name: Analog.c
*
* Revision History:
*   12/31/2014 - das - Created
*
******************************************************************************/
///   \file
///   This file contains the code that services Analog I/O 
#define analog_C


/*--------------------------------------------------------------------------
                              INCLUDE FILES
  --------------------------------------------------------------------------*/
#include "types.h"
#include "mvbvar.h"

#include "AnalogIn.h"
#include "AlogOut.h"


/*--------------------------------------------------------------------------
                             MODULE CONSTANTS
  --------------------------------------------------------------------------*/
#define DAC_MAX_VALUE				4095

/*--------------------------------------------------------------------------
                              MODULE MACROS
  --------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
                             MODULE DATA TYPES
  --------------------------------------------------------------------------*/
typedef struct
{
	eAnalogInId analogIn;
	UINT_16 *mvbVar;
} AnalogInSample_t;

typedef struct
{
	eAlogOutId analogOut;
	UINT_16 *mvbVar;
} AnalogOutUpdate_t;

/*--------------------------------------------------------------------------
                              MODULE VARIABLES
  --------------------------------------------------------------------------*/
static AnalogInSample_t m_AnalogInput[] =
{
	{ANALOGIN_00, &HDVCU_650_128_UINT16},
	{ANALOGIN_01, &HDVCU_650_144_UINT16},
	{ANALOGIN_02, &HDVCU_650_160_UINT16},
	{ANALOGIN_03, &HDVCU_650_176_UINT16},
	{ANALOGIN_04, &HDVCU_650_192_UINT16},
	{ANALOGIN_05, &HDVCU_650_208_UINT16},
	{ANALOGIN_06, &HDVCU_650_224_UINT16},
	{ANALOGIN_07, &HDVCU_650_240_UINT16},
	{ANALOGIN_08, &HDVCU_651_16_UINT16 },
	{ANALOGIN_09, &HDVCU_651_32_UINT16 },
	{ANALOGIN_10, &HDVCU_651_48_UINT16 },
	{ANALOGIN_11, &HDVCU_651_64_UINT16 },
	{ANALOGIN_12, &HDVCU_651_80_UINT16 },
	{ANALOGIN_13, &HDVCU_651_96_UINT16 },
	{ANALOGIN_14, &HDVCU_651_112_UINT16},
	{ANALOGIN_15, &HDVCU_651_128_UINT16},
};

static AnalogOutUpdate_t m_AnalogOutput[] =
{
	{ALOG_OUT0, &VCUHD_659_16_UINT16 },
	{ALOG_OUT1, &VCUHD_659_32_UINT16 },
	{ALOG_OUT2, &VCUHD_659_48_UINT16 },
	{ALOG_OUT3, &VCUHD_659_64_UINT16 },
	{ALOG_OUT4, &VCUHD_659_80_UINT16 },
	{ALOG_OUT5, &VCUHD_659_96_UINT16 },
	{ALOG_OUT6, &VCUHD_659_112_UINT16},
	{ALOG_OUT7, &VCUHD_659_128_UINT16},
};

/*--------------------------------------------------------------------------
                             MODULE PROTOTYPES
  --------------------------------------------------------------------------*/

//--------------------------------------------------------------------------
// Module:
//  ReadAnalogInputsUpdateMvb
//
///   This function reads all analog inputs and populates the MVB variables 
///   with the raw ADC quantized data.
///
///
//--------------------------------------------------------------------------
void ReadAnalogInputsUpdateMvb (void)
{
	UINT_16 index;

	for (index = 0; index < sizeof(m_AnalogInput)/sizeof(AnalogInSample_t); index++)
	{
	    /* Trigger of the ADC channels */
		AI_TriggerADC (m_AnalogInput[index].analogIn);
	    /* Update MVB variable */
		*(m_AnalogInput[index].mvbVar) = AI_GetDataValueADC (m_AnalogInput[index].analogIn);
	}

}

//--------------------------------------------------------------------------
// Module:
//  ReadMvbUpdateAnalogOutputs
//
///   This function updates all analog outputs from the values in the 
///   MVB variables. It will set the outputs to default/safe states
///   when MVB communication is down
///
///   \param aCommunicationOk - TRUE if MVB communication OK; FALSE otherwise
///
//--------------------------------------------------------------------------
void ReadMvbUpdateAnalogOutputs (BOOLEAN aCommunicationOk)
{
	UINT_16 index;
	UINT_16 analogOutVal;

	for (index = 0; index < sizeof(m_AnalogOutput)/sizeof(AnalogOutUpdate_t); index++)
	{
		/* Get the value in the analog output MVB variable */
		analogOutVal = *(m_AnalogOutput[index].mvbVar);

		/* Validate MVB communication is good */
	    if (aCommunicationOk == TRUE)
		{
			/* Clamp the value */
	        if (analogOutVal > DAC_MAX_VALUE)
			{
				analogOutVal = DAC_MAX_VALUE;
			}
		}
		else
		{
			analogOutVal = 0;
		}

		/* Update the analog output value */
		AO_SetQuanta (m_AnalogOutput[index].analogOut, analogOutVal);
	}

    /* Write to Analog output hardware (All 8 DACs updated here) */
    AO_Service();
}

//--------------------------------------------------------------------------
// Module:
//  ReadCurrentAnalogInput
//
///   This function returns the most recent analog input sample
///
///   \param id - analog input id
///
///   \returns - ADC value sent to the MVB 
///
//--------------------------------------------------------------------------
UINT_16 ReadCurrentAnalogInput (UINT_16 id)
{
	UINT_16 val = 0xFFFF;

	if (id < NUM_ANALOGINS)
	{
		val = *(m_AnalogInput[id].mvbVar);
	}
	
	return val;
}


//--------------------------------------------------------------------------
// Module:
//  ReadCurrentAnalogOutput
//
///   This function returns the most recent analog output value.
///
///   \param id - analog output id
///
///   \returns - DAC value commanded by MVB 
///
//--------------------------------------------------------------------------
UINT_16 ReadCurrentAnalogOutput (UINT_16 id)
{
	UINT_16 val = 0xFFFF;

	if (id < NUM_ALOGOUTS)
	{
		val = *(m_AnalogOutput[id].mvbVar);
	}
	
	return val;
}
