
// Permission to copy is granted provided that this header remains intact. 
// This software is provided with no warranties.

////////////////////////////////////////////////////////////////////////////////

#ifndef QUEUE_H
#define QUEUE_H

#include <stdlib.h> // required for malloc
#include "utils.h"

//Circular Queue
//Data Structure to implement circular queue MAX_SIZE 255
// ** IMPORTANT ** : Global definitions work, but initalization must occur inside main()
struct Queue
{
    struct Location** buffer;
    unsigned char front;
    unsigned char back;
    unsigned char num_objects;
    unsigned char capacity;
    
    unsigned char  (*QueueIsEmpty)(struct Queue* queue);
    unsigned char  (*QueueIsFull)(struct Queue* queue);
    void  (*QueueMakeEmpty)(struct Queue* queue);
    unsigned char  (*QueueEnqueue)(struct Queue* queue, struct Location* value);
    struct Location*  (*QueueDequeue)(struct Queue* queue);
};

////////////////////////////////////////////////////////////////////////////////
//Functionality - Allocates space and initializes a queue
//Parameter: Positive integer for the size of queue (0~255)
//Returns: Reference to a queue
struct Queue* QueueInit(unsigned char size);

////////////////////////////////////////////////////////////////////////////////
//Functionality - Checks if queue is empty
//Parameter: None
//Returns: 1 if empty else 0
unsigned char QueueIsEmpty(struct Queue* Q);

////////////////////////////////////////////////////////////////////////////////
//Functionality - Checks if queue is full
//Parameter: None
//Returns: 1 if full else 0
unsigned char QueueIsFull(struct Queue* Q);

////////////////////////////////////////////////////////////////////////////////
//Functionality - Flushes the queue
//Parameter: None
//Returns: None
void QueueMakeEmpty(struct Queue* Q);

////////////////////////////////////////////////////////////////////////////////
//Functionality - Push a character onto back of queue
//Parameter: Takes a single unsigned char value
//Returns: 1 if full else 0
unsigned char QueueEnqueue(struct Queue* Q, struct Location* value);

////////////////////////////////////////////////////////////////////////////////
//Functionality - Pop first character from front of queue
//Parameter: None
//Returns: Unsigned char from queue else null character
struct Location* QueueDequeue(struct Queue* Q);

#endif //QUEUE_H