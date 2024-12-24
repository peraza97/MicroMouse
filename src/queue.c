#include <stdlib.h> // required for malloc
#include "queue.h"
#include "API.h"

struct Queue* QueueInit(unsigned char size) 
{
    struct Queue* Q  = malloc(sizeof(struct Queue));
    Q->buffer = malloc (size * sizeof(struct Location*));
    Q->front = 0;
    Q->back = 0;
    Q->num_objects = 0;
    Q->capacity = size;
    Q->QueueIsEmpty = &QueueIsEmpty;
    Q->QueueIsFull = &QueueIsFull;
    Q->QueueMakeEmpty = &QueueMakeEmpty;
    Q->QueueEnqueue = &QueueEnqueue;
    Q->QueueDequeue = &QueueDequeue;

    return Q;
}

unsigned char QueueIsEmpty(struct Queue* Q) 
{
    return (Q->num_objects == 0);
}

unsigned char QueueIsFull(struct Queue* Q) 
{
    return (Q->num_objects == Q->capacity);
}

void QueueMakeEmpty(struct Queue* Q)
{
    Q->front = 0;
    Q->back = 0;
    Q->num_objects = 0;
}

unsigned char QueueEnqueue(struct Queue* Q, struct Location* value) 
{
    if(Q->num_objects < Q->capacity)
    {
        Q->buffer[Q->back] = value;
        Q->back++;
        Q->back %= Q->capacity;
        Q->num_objects++;
        return 0;
    }
    return 1;
}

struct Location* QueueDequeue(struct Queue* Q) 
{
    if(Q->num_objects > 0)
    {
        struct Location* tmp = Q->buffer[Q->front];	
        Q->buffer[Q->front] = NULL;
        Q->front++;
        Q->front %= Q->capacity;
        Q->num_objects--;
        return tmp;	
    }
    return NULL;
}
