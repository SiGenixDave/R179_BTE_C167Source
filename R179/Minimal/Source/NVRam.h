/*
 * NVRAM.h
 *
 *  Created on: Nov 26, 2018
 *      Author: David Smail
 */

#ifndef NVRAM_H_
#define NVRAM_H_

#include "Types.h"

void NVRamService (const char *str);
BOOLEAN NVRamTableUpdate (char cmdPtr[][MAX_PARAM_LENGTH]);

#endif /* NVRAM_H_ */
