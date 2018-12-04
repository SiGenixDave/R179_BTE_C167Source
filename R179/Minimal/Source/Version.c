/*
 * Version.c
 *
 *  Created on: Dec 3, 2018
 *      Author: David Smail
 */
#include "Types.h"
#include "CmdProc.h"
#include "SerComm.h"

BOOLEAN VersionUpdate (char cmdPtr[][MAX_PARAM_LENGTH])
{
    SC_PutsAlways ("<VER,1.0>");
    return (TRUE);
}
