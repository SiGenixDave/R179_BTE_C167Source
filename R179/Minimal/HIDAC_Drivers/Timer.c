/*****************************************************************************
*
* Copyright (C) 2010 Bombardier
*
* File Name: Timer.c
*
* Revision History:
*   08/31/2009 - das - Created
*   07/19/2011 - PRS - Changed max number of timers from 50 to 5
*
******************************************************************************/
///   \file
///   This file contains functions that support timers 

#define timer_C

/*--------------------------------------------------------------------------
                              INCLUDE FILES
  --------------------------------------------------------------------------*/
/* _DEBUG is defined in Microsoft Visual Studio 2008: UNIT TEST ONLY */
#ifndef _DEBUG
    #include <c166.h>
    #include <reg167.h>
#endif
#include <stdio.h>
#include "Types.h"
#include "Timer.h"

/*--------------------------------------------------------------------------
                             MODULE CONSTANTS
  --------------------------------------------------------------------------*/

/*-------------------------------------------------*/
/*---------- User configurable parameters ---------*/
#define NUM_TIMERS  5  /* maximum amount of timers 
                           needed at any given 
						   instant */
/* 
    Enable this define if the application wants to 
    use a linked list of active timers. Should be used
    if execution speed is desirable in TM_Service() 
*/
#define TIMER_USE_LINK_LIST 
/*-------------------------------------------------*/


/*--------------------------------------------------------------------------
                              MODULE MACROS
  --------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
                             MODULE DATA TYPES
  --------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------
                              MODULE VARIABLES
  --------------------------------------------------------------------------*/
static Timer  timer[NUM_TIMERS];
static UINT_16 numActiveTimers;
#ifdef TIMER_USE_LINK_LIST
static Timer  *tmInUseList;
#endif

/*-------------------------------------------------*/
/*---------- User configurable parameters ---------*/
/* "CLOCK_TICK" must be defined in the application
	software as:

	const UINT_16 CLOCK_TICK = x; 
	
	where x is the period in milliseconds that
	the function TM_Service is called. When compiling 
	and linking the driver software, a warning will 
	occur indicating that CLCOK_TICK is not defined.
	This will be resolved at link time with the application
	code. If the timer services are not required, remove
	the timer.obj from the link file to avoid a linker
	error when linking the application software or just
	declare the above const UINT_16 CLOCK_TICK = x;, even
	though it won't be used */
extern const UINT_16 CLOCK_TICK;  /* milliseconds */

/*--------------------------------------------------------------------------
                             MODULE PROTOTYPES
  --------------------------------------------------------------------------*/


//--------------------------------------------------------------------------
// Module:
//  TM_Init
//
///   This function is responsible for initializing all timers to the initial 
///   state. 
///
//--------------------------------------------------------------------------
void TM_Init(void)
{
    Timer *t;
    UINT_16 tId;

    for (t = timer, tId = 0; t < &timer[NUM_TIMERS]; t++, tId++)
    {
        t->signal = NULL;
        t->id     = tId;
        t->inUse  = FALSE;
#ifdef TIMER_USE_LINK_LIST
        t->next   = NULL;
        t->prev   = NULL;
#endif
    }

#ifdef TIMER_USE_LINK_LIST
    tmInUseList = NULL;
#endif

	numActiveTimers = 0;
}

//--------------------------------------------------------------------------
// Module:
//  TM_Allocate
//
///   This function is called by the application software to allocate a 
///   timer. It scans the list of timers and gets the first available timer. 
///   If no timer is available, the function immediately returns NULL. However, 
///   if a free timer is found, it adds the timer to the active timer list 
///   and sets the expiration time as well as the pointer to the BOOLEAN variable
///   that will be made TRUE by TM_Service() when the timer expires. Note: 
///   in order to prevent a timer from expiring and automatically being removed, 
///   the application software must pass NULL as the aSignal argument. Also, 
///   if the application passes a non-NULL aSignal, this value must remain in 
///   memory (e.g. a static BOOLEAN) and not on the stack (e.g. local variable). 
///   Unexpected results will occur if aSignal is a pointer to a stack variable.
///                    
///   \param aValue - the amount of time (milliseconds) before the timer expires 
///                   (if aSignal is NULL, the value of this argument is ignored) 
///   \param aSignal - pointer to the BOOLEAN variable that will be made TRUE when 
///                    the timer expires (see function description for important 
///                    information regarding this argument.
///
///   \return Timer * -  pointer to the timer just allocated; NULL if a timer 
///                      could not be allocated 
/// 
//--------------------------------------------------------------------------
Timer* TM_Allocate(INT_32 aValue, BOOLEAN *aSignal)
{

    Timer *t;
    DISABLE_ALL_INTERRUPTS();


#ifndef TIMER_USE_LINK_LIST
    /* Scan through all of the timers and cancel any timers that 
	   use the same signal as this newly allocated timer*/
    for (t = timer; t < &timer[NUM_TIMERS]; t++)
    {
        /* Is the timer active */
        if (t->inUse)
		{
			if (aSignal == t->signal)
			{
				TM_Free(t);
			}
		}
    }
#endif

	t = timer;

    /* Scan through timers and get first available Timer */
    while (t < &timer[NUM_TIMERS])
    {
        if ( !t->inUse )
        {
            break;
        }
        t++;
    }

    /* Couldn't find any available timers, inform calling function */
    if (t == &timer[NUM_TIMERS])
    {
        ENABLE_ALL_INTERRUPTS();
        return NULL;
    }

    /* Set all params */
    t->signal = aSignal;
    t->inUse = TRUE;
    t->runningTime = 0;
	
	/* Increment the number of active timers */
	numActiveTimers++;

	/* Update the flag if isn't NULL */
    if (aSignal != NULL)
    {
        *aSignal = FALSE;
		t->desiredValue = aValue / CLOCK_TICK;
    }

#ifdef TIMER_USE_LINK_LIST 
    /* Add the timer to the active timer linked list */
    if (tmInUseList == NULL)
    {
        /* First timer and only timer on the list */
        tmInUseList = t;
    }
    else
    {
        Timer *tList = tmInUseList;

		if (aSignal != NULL)
		{
			/* Scan through the active timer linked list until the end */
			do
			{
				/* Cancel any existing timer that is using the same signal */
				if (tList->signal == aSignal)
				{
					Timer *nextTimer;
					nextTimer = tList->next;
					TM_Free(tList);
					tList = nextTimer;
				}
				else
				{
					tList = tList->next;
				}
			}
			while (tList);
		}

		/* tmInUseList will be NULL if the only active timer was freed due
		   to the signal already being present in this active timer (see above code) */
		if (tmInUseList != NULL)
		{
			tList = tmInUseList;
			while (tList->next)
			{
				tList = tList->next;
			}
			/* Set the last timer in the linked list to this timer just allocated */
			tList->next = t;
			t->prev = tList;
			t->next = NULL;
		}
		else
		{
			tmInUseList = t;
		}
    }
#endif

    ENABLE_ALL_INTERRUPTS();
    return t;

}


//--------------------------------------------------------------------------
// Module:
//  TM_Service
//
///   This function must be called from the interrupt service routine that 
///   is triggered every clock tick. This function updates all of the active 
///   timers and determines if any of them have expired. 
///                    
//--------------------------------------------------------------------------
void TM_Service(void)
{
#ifdef TIMER_USE_LINK_LIST 
    Timer *tm = tmInUseList;
    Timer *next;
    while (tm != NULL)
    {
        next = tm->next;

        /* Update the time */
        tm->runningTime++;
        /* Does the application that allocated this timer
           want to be informed the timer expired. */
        if (tm->signal != NULL)
        {
            /* See if the timer has expired */
            if (tm->runningTime >= tm->desiredValue)
            {
                TM_Free(tm);
                *(tm->signal) = TRUE;
            }
        }

        tm = next;
    }
#else
    Timer *tm;
    /* Scan through all of the timers */
    for (tm = timer; tm < &timer[NUM_TIMERS]; tm++)
    {
        /* Is the timer active */
        if (tm->inUse)
        {
            /* Update the time */
            tm->runningTime++;
            /* Does the application that allocated this timer
               want to be informed the timer expired. */
            if (tm->signal != NULL)
            {
                /* See if the timer has expired */
                if (tm->runningTime >= tm->desiredValue)
                {
                    tm->inUse = FALSE;
                    *(tm->signal) = TRUE;
                }
            }
        }
    }
#endif
}

//--------------------------------------------------------------------------
// Module:
//  TM_Free
//
///   This function removes a timer from the active timer linked list. This 
///   function is only valid when maintaining timers with a linked list. 
///
///   \param aTm - pointer to the timer to remove from the active timer linked 
///                list
///
///   \return INT_32 - the amount of time expired (most likely a don't care 
///                    by the application
///                    
//--------------------------------------------------------------------------
INT_32 TM_Free(Timer *aTm)
{
	INT_32 timeExpired;

	if (aTm == NULL)
	{
		return 0;
	}

    DISABLE_ALL_INTERRUPTS();

	timeExpired = aTm->runningTime;

#ifdef TIMER_USE_LINK_LIST
	/* Make sure the time is still in use; don't want to free an already freed timer
	   because the logic below would reset the tmInUseList */
	if (aTm->inUse)
	{
		/* Is the timer the 1st in the list */
		if (!aTm->prev)
		{
			/* Is the timer the only one in the list */
			if (!aTm->next)
			{
				tmInUseList = NULL;
			}
			/* The timer is the 1st in the list, but there are others */
			else
			{
				/* Make the next timer the 1st in the list */
				aTm->next->prev = NULL;
				tmInUseList = aTm->next;
			}
		}
		/* Timer is in the middle or the end of the list */
		else
		{
			/* Link the previous timer to the next */
			aTm->prev->next = aTm->next;

			/* Check to see if this timer is at the end of the list */
			if (aTm->next)
			{
				/* Link the next timer to the previous */
				aTm->next->prev = aTm->prev;
			}

		}
		aTm->next = NULL;
		aTm->prev = NULL;
	}

#endif
	numActiveTimers--;

    aTm->inUse = FALSE;

    ENABLE_ALL_INTERRUPTS();

	return timeExpired * CLOCK_TICK;
}

//--------------------------------------------------------------------------
// Module:
//  TM_Wait
//
///   This function allocates a timer and then waits for it to expire. The 
///   amount of time it takes to expire is determined by aTime (milliseconds). 
///   After the timer expires, the function returns. This function should only 
///   be called from a background task; never from an interrupt service routine.
///
///   \param aTime - the amount of time (in milliseconds) to wait (sit idle).
///                    
//--------------------------------------------------------------------------
void TM_Wait(INT_32 aTime)
{
    BOOLEAN flag;
    TM_Allocate(aTime, &flag);
    while (!flag);
}

//--------------------------------------------------------------------------
// Module:
//  TM_GetExpiredTime
//
///   This function determines how much time has expired. If aTerminate is set 
///   to TRUE, it not only returns the expired time, but also removes the 
///   timer	from the active list. If aReset is set to TRUE, the expired time 
///   is returned and the timer is reset.
///
///   \param aTm - pointer to the timer that was allocated to the application
/// 
///   \return UINT_32 - the amound of time expired (milliseconds) since the 
///                     timer was either started or reset
///                    
//--------------------------------------------------------------------------
INT_32 TM_GetExpiredTime(Timer *aTm)
{
	if (aTm == NULL)
	{
		return 0;
	}
	else
	{
		/* Get the current expired time */
		return aTm->runningTime * CLOCK_TICK;
	}

}


//--------------------------------------------------------------------------
// Module:
//  TM_GetExpiredTime
//
///   This function determines how much time has expired. If aTerminate is set 
///   to TRUE, it not only returns the expired time, but also removes the 
///   timer	from the active list. If aReset is set to TRUE, the expired time 
///   is returned and the timer is reset.
///
///   \param aTm - pointer to the timer that was allocated to the application
/// 
///   \return UINT_32 - the amound of time expired (milliseconds) since the 
///                     timer was either started or reset
///                    
//--------------------------------------------------------------------------
INT_32 TM_ResetTimer(Timer *aTm)
{
    /* Get the current expired time */
    INT_32 timeExpired;

	if (aTm == NULL)
	{
		return 0;
	}
	else
	{
		timeExpired = aTm->runningTime * CLOCK_TICK;

		aTm->runningTime = 0;
		
		return timeExpired;
	}

}



//--------------------------------------------------------------------------
// Module:
//  TM_TimeToExpire
//
///   This function determines how much time (milliseconds) is left before 
///   the timer expires.
///
///   \param aTm - pointer to the timer
/// 
///   \return UINT_32 - the amount of time (milliseconds) until the timer 
///                     expires; set to -1 if it never will
///                    
//--------------------------------------------------------------------------
INT_32 TM_TimeToExpire(Timer *aTm)
{
    /* Get the current expired time */
    INT_32 timeToExpire;

    /* Check the timer to see if it is a timer that is to expire or not */
    if (aTm->signal != NULL)
    {
        timeToExpire = (aTm->desiredValue - aTm->runningTime) * CLOCK_TICK;
    }
    else
    {
        timeToExpire = -1;
    }

    return timeToExpire;

}

//--------------------------------------------------------------------------
// Module:
//  TM_NumActiveTimers
//
///   This function returns the number of active (in use) timers.
///
///   \return UINT_16 - the number of active timers
///                    
//--------------------------------------------------------------------------
UINT_16 TM_NumActiveTimers(void)
{
    return numActiveTimers;
}

