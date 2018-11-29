/*
 * Flash.h
 *
 *  Created on: Nov 21, 2018
 *      Author: David Smail
 */

#ifndef FLASH_H_
#define FLASH_H_

#include "Types.h"

void FlashService (const char *str);
BOOLEAN FlashTableUpdate (char cmdPtr[][MAX_PARAM_LENGTH]);

#endif /* FLASH_H_ */
