#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include "init_SDL.h"

#include <Windows.h>
#include "queue.h"

#define NUM_OF_THREADS 4
#define NUM_OF_PARTICLES 100

HANDLE particle_chunk_mutex[NUM_OF_THREADS];
HANDLE task_queue_mutex;
HANDLE task_added_event;

void thread_routine(void *data){
    Queue *task_queue = (Queue*)data;
    WaitForSingleObject(task_added_event, INFINITE);
    printf("In thread we got signaled that task added...\n");
}

int main(int argc, char **argv){
    //Creating window
    App app;
    int code = init_app(&app);
    if(code != SDL_SUCCSSES){
        printf("Failed to initialise app. Error code: %d\n", code);
        return EXIT_FAILURE;
    }

    //Adding task queue
    Queue task_queue;
    init_queue(&task_queue);
    task_added_event = CreateEvent(NULL, false, false, NULL); // Configuration: auto event reset.

    //Setting up threads
    HANDLE threads[NUM_OF_THREADS];

    for(int i = 0; i<NUM_OF_THREADS; i++){
        threads[i] = CreateThread(NULL, 0, (void *)&thread_routine, (void *)&task_queue, 0, NULL);
        if(threads[i] == NULL){
            printf("Failed to create thread %d...\n", i);
            return EXIT_FAILURE;
        }
    }

    for(int i = 0; i<NUM_OF_THREADS; i++){
        particle_chunk_mutex[i] = CreateMutex(NULL, false, NULL);
        if(particle_chunk_mutex[i] == NULL){
            printf("Failed to create particle chunk mutex %d...\n", i);
            return EXIT_FAILURE;
        }
    }

    task_queue_mutex = CreateMutex(NULL, true, NULL);
    if(task_queue_mutex == NULL){
        printf("Failed to create task queue mutex...\n");
        return EXIT_FAILURE;
    }

    while(true){

        if(!update_app(&app)){
            break;
        }
    }

    for(int i = 0; i<NUM_OF_THREADS; i++){
        CloseHandle(threads[i]);
        CloseHandle(particle_chunk_mutex[i]);
    }
    CloseHandle(task_queue_mutex);

    close_app(&app);

    return EXIT_SUCCESS;
}