#include <stdlib.h>
#include <stdbool.h>
#include "pqueue.h"



gqueue* gqueue_create()
{
    gqueue* q = malloc(sizeof((*q)));
    if (q == NULL)
    {
        return q;
    }
    q->length = 0;
    q->head = NULL;
    q->tail = NULL;
    return q;
}

void gqueue_destroy(gqueue* q)
{
    n* curr = q->head;
    while (curr != NULL)
    {
        n* temp = curr;
        curr = curr->next;
        free(temp);
    }
    free(q);
}

void gqueue_enqueue(gqueue* q, void* v)
{
    n* new_node = malloc(sizeof(*new_node));
    if (new_node == NULL)
    {
        return;
    }
    new_node->container = v;
    new_node->prev = NULL;
    n* old_head = q->head;
    if (q->head != NULL) {
        old_head->prev = new_node;
    }
    else {
        q->tail = new_node;
    }
    q->head = new_node;
    new_node->next = old_head;
    q->length += 1;
}

bool is_gqueue_empty(gqueue* q)
{
    return (q->length == 0);
}

void* gqueue_dequeue(gqueue* q)
{
    if (is_gqueue_empty(q)) {
        return NULL;
    }
    n* t = q->tail;
    void* container = t->container;
    if (q->length != 1) {
        q->tail = q->tail->prev;
        q->tail->next = NULL;
    }
    else {
        q->tail = NULL;
        q->head = NULL;
    }
    free(t);
    q->length -= 1;
    return container;
}
