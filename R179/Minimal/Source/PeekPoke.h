/*
 * PeekPoke.h
 *
 *  Created on: Nov 30, 2018
 *      Author: David Smail
 */

#ifndef PEEKPOKE_H_
#define PEEKPOKE_H_

void PeekSingleService (const char *str);
BOOLEAN PeekSingle (char cmdPtr[][MAX_PARAM_LENGTH]);
void PokeSingleService (const char *str);
BOOLEAN PokeSingle (char cmdPtr[][MAX_PARAM_LENGTH]);
void PeekContinuousService (const char *str);
BOOLEAN PeekContinuous (char cmdPtr[][MAX_PARAM_LENGTH]);
void PokeContinuousService (const char *str);
BOOLEAN PokeContinuous (char cmdPtr[][MAX_PARAM_LENGTH]);

BOOLEAN PeekContinuousKill (char cmdPtr[][MAX_PARAM_LENGTH]);
BOOLEAN PokeContinuousKill (char cmdPtr[][MAX_PARAM_LENGTH]);

#endif /* PEEKPOKE_H_ */
