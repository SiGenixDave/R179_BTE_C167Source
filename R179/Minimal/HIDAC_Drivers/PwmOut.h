/*****************************************************************************
*
* Copyright (C) 2010 Bombardier
*
* File Name: PwmOut.h
*
* Abstract: Header file for the PWM output driver
*
* Revision History:
*   03/19/2014 - steeriks - Created
*
******************************************************************************/

#ifndef PWMOUT_H
#define PWMOUT_H

/*--------------------------------------------------------------------------
                             GLOBAL CONSTANTS
  --------------------------------------------------------------------------*/
/*--------------------------------------------------------------------------
                               GLOBAL TYPES
  --------------------------------------------------------------------------*/
typedef enum
{
    PWMOUT_0,	/* PWM Output 0 : P2-B6 */
    PWMOUT_1,	/* PWM Output 1 : P2-D6 */
    PWMOUT_2,	/* PWM Output 2 : P2-Z8 */
    PWMOUT_3,	/* PWM Output 3 : P2-B8 */
    NUM_PWMOUT
} ePwmOutId;

typedef enum
{
    PWMOUT_0_ENABLE,	/* Enable bit for PWM channel 0 in PWMCON1 */
    PWMOUT_1_ENABLE,	/* Enable bit for PWM channel 1 in PWMCON1  */
    PWMOUT_2_ENABLE,	/* Enable bit for PWM channel 2 in PWMCON1  */
    PWMOUT_3_ENABLE 	/* Enable bit for PWM channel 3 in PWMCON1  */
} ePwmOutEnableBit;


typedef enum
{
    CLK_CPU_DIV_1,	/* PWM timer PTx clocked with CLKcpu/1  */
    CLK_CPU_DIV_64	/* PWM timer PTx clocked with CLKcpu/64 */
} ePwmClkInput; 

/*--------------------------------------------------------------------------
                              GLOBAL VARIABLES
  --------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
                              GLOBAL PROTOTYPES
  --------------------------------------------------------------------------*/
EXPORT void PWMO_SetEnable(ePwmOutId aId, BOOLEAN aEnable);
EXPORT void PWMO_SetPwmOutput(ePwmOutId aId, UINT_16 pwmFrequency, UINT_16 pwmDutyCycle); 

#endif
