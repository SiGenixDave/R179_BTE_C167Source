/* This file contains dummy declarations of Operating System functions */
/* All of these functions are used by Message Data */

#include "types.h"
#include <stddef.h>

INT_16 pi_create_queue (INT_16 size, INT_16 option, INT_16 *p_err)
{
    return (0);
}

INT_8 *pi_receive_queue (INT_16 id, INT_32 timeout, INT_16 *p_err)
{
    return (0);
}

INT_8 *pi_accept_queue (INT_16 id, INT_16 *p_err)
{
    return (0);
}

INT_16 pi_create_semaphore (INT_16 init, INT_16 option, INT_16 *p_err)
{
    return (0);
}

INT_16 pi_inquiry_semaphore (INT_16 id, INT_16 *p_err)
{
    return (0);
}

void pi_init_timeout (INT_16 nr_timeouts)
{
}

INT_16  pi_create_timeout (INT_16 *p_id, void (*p_calledfunc) (void *),
                           void *p_para, INT_16 count)
{
    return (0);
}
INT_16  pi_delete_timeout (INT_16 id)
{
    return (0);
}

INT_16  pi_disable_timeout (INT_16 id)
{
    return (0);
}
INT_16 pi_enable_timeout (INT_16 id, UINT_32 ticks)
{
    return (0);
}
void pi_start_timeout (void)
{
}

void  KS_nop (void)
{
}

void* my_malloc (size_t  size)
{
    return ((void *) 0);
}

void enable (void)
{
}

void disable (void)
{
}
