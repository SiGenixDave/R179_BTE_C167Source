/*****************************************************************************
*
* Copyright (c) 2010... Bombardier Transportation 
                        Control and Test Equipment
                        Propulsion and Control (LAE)                                
*
******************************************************************************
*   file.............:  c167_vector_table.h
*   creation date....:  05.10.2006
*   author(s)........:  P. Sukthankar (P.S.)
*   description......:  header file to define vector table locations for Inter
                        Service Routines
*-----------------------------------------------------------------------------
*
*   COPYRIGHT           Bombardier Inc., 2006
*
*                       This document must not be copied without our written
*                       permission, and the contents thereof must not be
*                       imparted to a third party nor be used for any
*                       unauthorized purpose.
*                       Contravention will be prosecuted.
*
*=============================================================================
*   updates/comments.:
*-----------------------------------------------------------------------------
* mm.dd.yyyy
* 09.01.2010 (P.S.) module creation
*****************************************************************************/

#ifndef _VEC_TBL_H_
#define _VEC_TBL_H_

#define CAPCOM_TIMER_0      0x20  /* interrupt vector for timer 0 interrupt */
#define CAPCOM_TIMER_1      0x21  /* interrupt vector for timer 1 interrupt */
#define CAPCOM_TIMER_7      0x3D  /* interrupt vector for timer 7 interrupt */
#define CAPCOM_TIMER_8      0x3E  /* interrupt vector for timer 8 interrupt */

#define CAPCOM_REGISTER_0   0x10                                /* Not Used */
#define CAPCOM_REGISTER_1   0x11                                /* Not Used */
#define CAPCOM_REGISTER_2   0x12                                /* Not Used */
#define CAPCOM_REGISTER_3   0x13                                /* Not Used */
#define CAPCOM_REGISTER_4   0x14                                /* Not Used */
#define CAPCOM_REGISTER_5   0x15                                  /* TACH 4 */
#define CAPCOM_REGISTER_6   0x16                                  /* TACH 3 */
#define CAPCOM_REGISTER_7   0x17                                  /* TACH 2 */

#define CAPCOM_REGISTER_8   0x18                                  /* TACH 1 */
#define CAPCOM_REGISTER_9   0x19                                /* Not Used */
#define CAPCOM_REGISTER_10  0x1A                                /* Not Used */
#define CAPCOM_REGISTER_11  0x1B                                /* Not Used */
#define CAPCOM_REGISTER_12  0x1C                                /* Not Used */
#define CAPCOM_REGISTER_13  0x1D                                /* Not Used */
#define CAPCOM_REGISTER_14  0x1E       /* interrupt vector for -15V failure */
#define CAPCOM_REGISTER_15  0x1F       /* interrupt vector for +15V failure */

#define CAPCOM_REGISTER_16  0x30                       /* Phase A- Armature */
#define CAPCOM_REGISTER_17  0x31                       /* Phase B- Armature */
#define CAPCOM_REGISTER_18  0x32                       /* Phase C- Armature */
#define CAPCOM_REGISTER_19  0x33                         /* Phase AB- Field */
#define CAPCOM_REGISTER_20  0x34                        /* Phase A- Battery */
#define CAPCOM_REGISTER_21  0x35                        /* Phase B- Battery */
#define CAPCOM_REGISTER_22  0x36                        /* Phase C- Battery */
#define CAPCOM_REGISTER_23  0x37                        /* Power Self Test  */
                                                          
#define CAPCOM_REGISTER_24  0x38                                /* Not Used */
#define CAPCOM_REGISTER_25  0x39                                /* Not Used */
#define CAPCOM_REGISTER_26  0x3A                                /* Not Used */
#define CAPCOM_REGISTER_27  0x3B                                /* Not Used */
#define CAPCOM_REGISTER_28  0x3C                                /* Not Used */
#define CAPCOM_REGISTER_29  0x44                                /* Not Used */
#define CAPCOM_REGISTER_30  0x45                                /* Not Used */
#define CAPCOM_REGISTER_31  0x46                                /* Not Used */


#endif/*_VEC_TBL_H_*/

