/*
 * Flash.h
 *
 *  Created on: Nov 21, 2018
 *      Author: David Smail
 */

#ifndef RAM_H_
#define RAM_H_

#include "Types.h"

void RamService (const char *str);
BOOLEAN RamTableUpdate (char cmdPtr[][MAX_PARAM_LENGTH]);

#endif /* RAM_H_ */
