#include <stdlib.h>

typedef struct Task Task;

typedef struct Task{
    void *data;
    void (*routine)(void *data);
    Task *next_task;
} Task;

typedef struct Queue{
    int len;
    Task *first;
    Task *last;
} Queue;

void init_queue(Queue *queue);
void push(Queue *queue, Task *task);
Task *pop(Queue *queue);
