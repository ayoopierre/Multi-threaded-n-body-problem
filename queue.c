#include "queue.h"

void init_queue(Queue *queue){
    queue->first = NULL;
    queue->last = NULL;
    queue->len = 0;
}

void push(Queue *queue, Task *task){
    if(queue->len == 0){
        queue->first = task;
        queue->last = task;
        queue->len++;
    }
    else{
        queue->last->next_task = task;
        queue->last = task;
    }
}

Task *pop(Queue *queue){
    Task *ret = queue->first;
    queue->first = ret->next_task;
    queue->len--;
    return ret;
}
