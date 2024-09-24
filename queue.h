#include <stdlib.h>
#include "particle.h"

#define NUM_OF_THREADS 4

typedef struct Task Task;

typedef struct Task{
    void *data;
    void (*routine)(void *data);
    Task *next_task; // It should be set to NULL bny default
} Task;

Task *create_task(void *data, void (*function)(void *));

typedef struct Queue{
    int len;
    Task *first;
    Task *last;
} Queue;

typedef struct Scheduler_arg{
    Swarm *swarm;
    Queue *queue;
} Scheduler_arg;

void init_queue(Queue *queue);
void push_task(Queue *queue, Task *task);
Task *pop_task(Queue *queue);
