/*
 * event_queue.c
 *
 *  Created on: 24 Nov 2017
 *      Author: Milan
 */

#include "event_queue.h"

#define FALSE 0
#define TRUE -1

void init_q(EventQueue *q)
{
    q->head = 0;
    q->tail = 0;
    q->size = 0;
}

int read_q(EventQueue *q, Event *e)
{
    int rc;
    int s;
    unsigned int h;

    rc = FALSE;

    s = q->size;

    if (s > 0)
    {
        h = q->head;
        e->event  = q->queue[h].event;
        e->param1 = q->queue[h].param1;
        e->param2 = q->queue[h].param2;
        e->param3 = q->queue[h].param3;
        h = (h + 1) % SIZE_OF_EVENT_Q;
        s--;
        q->head = h;
        q->size = s;
        rc = TRUE;
    }

    return rc;
}

int write_q(EventQueue *q, Event e)
{
    int rc;
    int s;
    unsigned int t;

    rc = FALSE;

    s = q->size;

    if (s < SIZE_OF_EVENT_Q)
    {
        t = q->tail;
        q->queue[t].event  = e.event;
        q->queue[t].param1 = e.param1;
        q->queue[t].param2 = e.param2;
        q->queue[t].param3 = e.param3;
        t = (t + 1) % SIZE_OF_EVENT_Q;
        s++;
        q->tail = t;
        q->size = s;
        rc = TRUE;
    }

    return rc;
}

