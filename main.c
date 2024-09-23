#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include "init_SDL.h"

#include <Windows.h>
#include "queue.h"

#define NUM_OF_THREADS 4
#define NUM_OF_PARTICLES 100

void thread_routine(void *data){
    Queue *task_queue = (Queue*)data;
    
}

int main(int argc, char **argv){
    App app;
    int code = init_app(&app);
    if(code != SDL_SUCCSSES){
        printf("Failed to initialise app. Error code: %d\n", code);
        return EXIT_FAILURE;
    }

    Queue task_queue;
    init_queue(&task_queue);

    HANDLE threads[NUM_OF_THREADS];

    for(int i = 0; i<NUM_OF_THREADS; i++){
        threads[i] = CreateThread(NULL, 0, (void *)&thread_routine, (void *)&task_queue, 0, NULL);
        if(threads[i] == NULL){
            printf("Failed to create thread %d...", i);
            return EXIT_FAILURE;
        }
    }

    while(true){

        if(!update_app(&app)){
            break;
        }
    }

    close_app(&app);

    return EXIT_SUCCESS;
}