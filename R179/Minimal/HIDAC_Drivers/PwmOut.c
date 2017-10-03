/*****************************************************************************
*
* Copyright (C) 2010 Bombardier
*
* File Name: Pwmout.c
*
* Revision History:
*   03/19/2014 - steeriks - Created
*
******************************************************************************/
///   \file
///   This file contains functions that write to the PWM output channels

/*--------------------------------------------------------------------------
                              INCLUDE FILES
  --------------------------------------------------------------------------*/
/* _DEBUG is defined in Microsoft Visual Studio 2008: UNIT TEST ONLY */
#ifndef _DEBUG
#include <c166.h>
#include <reg167.h>
#endif
#include "Types.h"
#include "PwmOut.h"

/*--------------------------------------------------------------------------
                             MODULE CONSTANTS
  --------------------------------------------------------------------------*/
#define FREQUENCY_CPU_DIV_1     (40000000.0)         /*  40MHz     */
#define FREQUENCY_CPU_DIV_64    (40000000.0/64.0)    /* (40/64)MHz */

/*--------------------------------------------------------------------------
                              MODULE MACROS
  --------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------
                             MODULE DATA TYPES
  --------------------------------------------------------------------------*/
typedef struct
{
    BOOLEAN  enabled;               /* PWM output channel enabled */
    UINT_16  frequency;             /* frequency for PWM output channel  */
    UINT_16  dutyCycle;             /* duty cycle for PWM output channel */
} PwmOutInfo_t;

/*--------------------------------------------------------------------------
                              MODULE VARIABLES
  --------------------------------------------------------------------------*/

static PwmOutInfo_t PwmOutInfo[NUM_PWMOUT] =
{
    {FALSE, 0, 0},
    {FALSE, 0, 0},
    {FALSE, 0, 0},
    {FALSE, 0, 0}
};


/*--------------------------------------------------------------------------
                             MODULE PROTOTYPES
  --------------------------------------------------------------------------*/
static void setPwmPeriodRegister (ePwmOutId aId, UINT_16 value);
static void setPwmInputClock (ePwmOutId aId, ePwmClkInput clkInput);
static void setPwmPulseWidthRegister (ePwmOutId aId, UINT_32 PwmDutyCycle);

//--------------------------------------------------------------------------
// Module:
//   PWMO_SetEnable
//
///   This function sets the enable bit in PWM control register 1 for the
///   selected PWM input channel.
///
///	  \param aId - the id of the PWM output channel
///	  \param aEnable - the new enable setting of the PWM output channel
///
//--------------------------------------------------------------------------
void PWMO_SetEnable (ePwmOutId aId, BOOLEAN aEnable)
{
    UINT_16 mask = 0;

    PwmOutInfo[aId].enabled = aEnable;

    /* select PWM channel */
    if (aId == PWMOUT_0)
    {
        if (aEnable)
        {
            mask = 1 << PWMOUT_0_ENABLE;
            PWMCON1 |= mask;
        }
        else
        {
            mask = ~ ((UINT_16) (1 << PWMOUT_0_ENABLE));
            PWMCON1 &= mask;
        }
    }
    else if (aId == PWMOUT_1)
    {
        if (aEnable)
        {
            mask = 1 << PWMOUT_1_ENABLE;
            PWMCON1 |= mask;
        }
        else
        {
            mask = ~ ((UINT_16) (1 << PWMOUT_1_ENABLE));
            PWMCON1 &= mask;
        }
    }
    else if (aId == PWMOUT_2)
    {
        if (aEnable)
        {
            mask = 1 << PWMOUT_2_ENABLE;
            PWMCON1 |= mask;
        }
        else
        {
            mask = ~ ((UINT_16) (1 << PWMOUT_2_ENABLE));
            PWMCON1 &= mask;
        }
    }
    else if (aId == PWMOUT_3)
    {
        if (aEnable)
        {
            mask = 1 << PWMOUT_3_ENABLE;
            PWMCON1 |= mask;
        }
        else
        {
            mask = ~ ((UINT_16) (1 << PWMOUT_3_ENABLE));
            PWMCON1 &= mask;
        }
    }
}


//--------------------------------------------------------------------------
// Module:
//   PWMO_SetPwmOutput
//
///   This function sets the enable bit in PWM control register 1 for the
///   selected PWM input channel.
///
///	  \param aId - the id of the PWM output channel
///	  \param pwmFrequency - selected PWM frequency
///	  \param pwmDutyCycle - selected PWM duty cycle
///
//--------------------------------------------------------------------------
void PWMO_SetPwmOutput (ePwmOutId aId, UINT_16 pwmFrequency, UINT_16 pwmDutyCycle)
{
    UINT_32 timerResolution = 0;
    UINT_16 PwmResolution = 0;

    /* Check if PWM channel has been changed */
    if ((PwmOutInfo[aId].frequency != pwmFrequency) || (PwmOutInfo[aId].dutyCycle != pwmDutyCycle) || (PwmOutInfo[aId].enabled == FALSE))
    {
        /* save new values */
        PwmOutInfo[aId].frequency = pwmFrequency;
        PwmOutInfo[aId].dutyCycle = pwmDutyCycle;

        /* Define resolution of timer for the PWM channel */
        if (pwmFrequency == 0)
        {
            PWMO_SetEnable (aId, FALSE);
            setPwmPeriodRegister (aId, 0);
        }
        else
        {
            if (pwmFrequency < 700)
            {
                /* Use freqCPU/64 */
                setPwmInputClock (aId, CLK_CPU_DIV_64);
                timerResolution = FREQUENCY_CPU_DIV_64;
            }
            else
            {
                /* Use freqCPU/1 */
                setPwmInputClock (aId, CLK_CPU_DIV_1);
                timerResolution = FREQUENCY_CPU_DIV_1;
            }
            PwmResolution = timerResolution / pwmFrequency;
            setPwmPeriodRegister (aId, PwmResolution);
            PWMO_SetEnable (aId, TRUE);
        }

        /* Define the duty cycle for the PWM channel */
        setPwmPulseWidthRegister (aId, pwmDutyCycle);
    }
}


//--------------------------------------------------------------------------
// Module:
//   setPwmPeriodRegister
//
///   This function sets the PWM pulse width register for the selected PWM
///   output channel.
///
///	  \param aId - the id of the PWM output channel
///	  \param value - value to be written to period register
///
//--------------------------------------------------------------------------
static void setPwmPeriodRegister (ePwmOutId aId, UINT_16 value)
{
    /* select PWM channel */
    if (aId == PWMOUT_0)
    {
        PP0 = value;
    }
    else if (aId == PWMOUT_1)
    {
        PP1 = value;
    }
    else if (aId == PWMOUT_2)
    {
        PP2 = value;
    }
    else if (aId == PWMOUT_3)
    {
        PP3 = value;
    }
}

//--------------------------------------------------------------------------
// Module:
//   setPwmInputClock
//
///   This function sets the input clock for the selected PWM
///   output channel.
///
///	  \param aId - the id of the PWM output channel
///	  \param clkInput - input clock selection for the PWM output channel
///
//--------------------------------------------------------------------------
static void setPwmInputClock (ePwmOutId aId, ePwmClkInput clkInput)
{
    /* select PWM channel */
    if (aId == PWMOUT_0)
    {
        if (clkInput == CLK_CPU_DIV_1)
        {
            /* Use freqCPU/1 */
            PWMCON0 &= 0xFFEF;  /* PTI0 = 0 */
        }
        else if (clkInput == CLK_CPU_DIV_64)
        {
            /* Use freqCPU/64 */
            PWMCON0 |= 0x0010;  /* PTI0 = 1 */
        }
    }
    else if (aId == PWMOUT_1)
    {
        if (clkInput == CLK_CPU_DIV_1)
        {
            /* Use freqCPU/1 */
            PWMCON0 &= 0xFFDF;  /* PTI1 = 0 */
        }
        else if (clkInput == CLK_CPU_DIV_64)
        {
            /* Use freqCPU/64 */
            PWMCON0 |= 0x0020;  /* PTI1 = 1 */
        }
    }
    else if (aId == PWMOUT_2)
    {
        if (clkInput == CLK_CPU_DIV_1)
        {
            /* Use freqCPU/1 */
            PWMCON0 &= 0xFFBF;  /* PTI2 = 0 */
        }
        else if (clkInput == CLK_CPU_DIV_64)
        {
            /* Use freqCPU/64 */
            PWMCON0 |= 0x0040;  /* PTI2 = 1 */
        }
    }
    else if (aId == PWMOUT_3)
    {
        if (clkInput == CLK_CPU_DIV_1)
        {
            /* Use freqCPU/1 */
            PWMCON0 &= 0xFF7F;  /* PTI3 = 0 */
        }
        else if (clkInput == CLK_CPU_DIV_64)
        {
            /* Use freqCPU/64 */
            PWMCON0 |= 0x0080;  /* PTI3 = 1 */
        }
    }
}

//--------------------------------------------------------------------------
// Module:
//   setPwmPulseWidthRegister
//
///   This function sets the PWM pulse width register for the selected PWM
///   output channel.
///
///	  \param aId - the id of the PWM output channel
///	  \param PwmDutyCycle - duty cycle (%) of the PWM output channel * 100
///                         (1% => 100, 99% => 9900)
///
//--------------------------------------------------------------------------
static void setPwmPulseWidthRegister (ePwmOutId aId, UINT_32 PwmDutyCycle)
{
    /* select PWM channel */
    if (aId == PWMOUT_0)
    {
        /* PW0 = 0 ==> 100% duty cycle, PW0 = PP0 + 1 ==> 0% duty cycle */
        PW0 = ((PwmDutyCycle * PP0) / 10000) + 1;
    }
    else if (aId == PWMOUT_1)
    {
        /* PW1 = 0 ==> 100% duty cycle, PW1 = PP1 + 1 ==> 0% duty cycle */
        PW1 = ((PwmDutyCycle * PP1) / 10000) + 1;
    }
    else if (aId == PWMOUT_2)
    {
        /* PW2 = 0 ==> 100% duty cycle, PW2 = PP2 + 1 ==> 0% duty cycle */
        PW2 = ((PwmDutyCycle * PP2) / 10000) + 1;
    }
    else if (aId == PWMOUT_3)
    {
        /* PW3 = 0 ==> 100% duty cycle, PW3 = PP3 + 1 ==> 0% duty cycle */
        PW3 = ((PwmDutyCycle * PP3) / 10000) + 1;
    }

}

