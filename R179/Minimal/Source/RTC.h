/*
 * RTC.h
 *
 *  Created on: Nov 26, 2018
 *      Author: David Smail
 */

#ifndef RTC_H_
#define RTC_H_

#include "Types.h"

void RTCService (const char *str);
BOOLEAN RTCTableUpdate (char cmdPtr[][MAX_PARAM_LENGTH]);

#endif /* RTC_H_ */
