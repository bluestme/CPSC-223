#ifndef __GQUEUE_H__
#define __GQUEUE_H__

#include <stddef.h>
#include <stdbool.h>

typedef struct _n
{
    void* container;
    struct _n* next;
    struct _n* prev;
} n;

typedef struct _gqueue
{
    n* head;
    n* tail;
    size_t length;
} gqueue;

/**
 * Create a general queue
 *
 * @return a pointer to the newly instantiated gqueue
 */
gqueue* gqueue_create();

/**
 * Destroy a provided gqueue
 *
 * @param q a pointer to a gqueue, non NULL
 */
void gqueue_destroy(gqueue* q);

/**
 * Enqueue a generic value to a provided gqueue
 *
 * @param q a pointer to a gqueue, non NULL
 * @param v a pointer to a value
 */
void gqueue_enqueue(gqueue* q, void* v);

/**
 * Dequeue a generic value to a provided gqueue
 *
 * @param q a pointer to a gqueue, non NULL
 * @return a pointer to the newly dequeued value
 */
void* gqueue_dequeue(gqueue* q);

/**
 * Return if the gqueue is empty
 *
 * @param q a pointer to a gqueue, non NULL
 * @return whether or not the gqueue is empty
 */
bool is_gqueue_empty(gqueue* q);

#endif