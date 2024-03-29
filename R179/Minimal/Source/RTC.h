/*****************************************************************************
 *
 * Copyright (C) 2018-19 Bombardier
 *
 * File Name: RTC.h
 *
 * Abstract: Header file for the RAM
 *
 * Revision History:
 *   12/03/2018 - das - Created
 *
 ******************************************************************************/
#ifndef RTC_H_
#define RTC_H_

#include "Types.h"

/*--------------------------------------------------------------------------
 GLOBAL CONSTANTS
 --------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 GLOBAL TYPES
 --------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 GLOBAL VARIABLES
 --------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
 GLOBAL PROTOTYPES
 --------------------------------------------------------------------------*/
void RTCService (const char *str);
BOOLEAN RTCTableUpdate (char cmdPtr[][MAX_PARAM_LENGTH]);

#endif /* RTC_H_ */
