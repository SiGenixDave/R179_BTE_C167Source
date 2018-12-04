/*****************************************************************************
 *
 * Copyright (C) 2018-19 Bombardier
 *
 * File Name: PeekPoke.h
 *
 * Abstract: Header file for the PeekPoke
 *
 * Revision History:
 *   12/03/2018 - das - Created
 *
 ******************************************************************************/

#ifndef PEEKPOKE_H_
#define PEEKPOKE_H_

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
