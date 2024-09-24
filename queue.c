#include "queue.h"

Task *create_task(void *data, void (*function)(void *)){
    Task *t = (Task *)malloc(sizeof(Task));
    t->data = data;
    t->routine = function;
    t->next_task = NULL;
    return t;
}

void init_queue(Queue *queue){
    queue->first = NULL;
    queue->last = NULL;
    queue->len = 0;
}

void push_task(Queue *queue, Task *task){
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

Task *pop_task(Queue *queue){
    Task *ret = queue->first;
    queue->first == NULL;
    queue->first = ret->next_task;
    queue->len--;
    return ret;
}
