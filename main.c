#include <stdlib.h>
#include <stdbool.h>
#include <stdio.h>
#include <SDL2/SDL.h>
#include <Windows.h>
#include "init_SDL.h"
#include "queue.h"
#include "particle.h"

#ifndef NUM_OF_THREADS
#define NUM_OF_THREADS 1
#endif

#ifndef NUM_OF_PARTICLES
#define NUM_OF_PARTICLES 100
#endif

HANDLE particle_chunk_mutex[NUM_OF_THREADS];
HANDLE task_queue_mutex;
HANDLE task_added_event;

void thread_routine(void *data){
    Queue *task_queue = (Queue*)data;

    while(true){
        // Waiting until there is task to take from queue. Signaled from scheduling task...
        WaitForSingleObject(task_added_event, INFINITE);

        // While there are tasks inside the queue, we try to take a task...
        while(task_queue->len > 0){
            // Waiting for mutex, to lock the queue. After we acuire lock we can pop a task, and realese lock instantly...
            WaitForSingleObject(task_queue_mutex, INFINITE);
            Task *current_task = pop_task(task_queue);
            ReleaseMutex(task_queue_mutex);

            // We thought that there was a task but there wasn't, so we we breake of loop...
            if(current_task == NULL) break;

            // Executing current task...
            current_task->routine(current_task->data);
            free((void*)current_task);
        }
    }
}

// This function should be somewhere inside particle.h
void update_swarm_state(void *data){
    // Copying swarm particle buffer into read-only particle buffer...
    // This needs to be performed when all threads finish working with swarm...
    Scheduler_arg *scheduler_arg = (Scheduler_arg*)data;
    Swarm *swarm = scheduler_arg->swarm;
    App* app = scheduler_arg->app;

    if(swarm == NULL) return;

    WaitForMultipleObjects(NUM_OF_THREADS, particle_chunk_mutex, true, INFINITE);
    memcpy(swarm->read_only_swarm, swarm->swarm, swarm->num_of_particles * sizeof(Particle));
    printf("x: %f, y: %f, mass:%f\n", swarm->read_only_swarm[1].x, swarm->read_only_swarm[1].y, swarm->read_only_swarm[1].mass);
    for(int i = 0; i<NUM_OF_THREADS; i++) ReleaseMutex(particle_chunk_mutex[i]);
}

void schedule_tasks(void *data){
    Scheduler_arg *scheduler_arg = (Scheduler_arg*)data;
    Swarm *swarm = scheduler_arg->swarm;
    Queue *queue = scheduler_arg->queue;
    Task *current_task;
    Update_data *current_data;

    // Calculating how many particles should one thread take...
    // Its probably not the best solution but works well if there is much more particles than threads...
    int num_of_particles_to_schedule = NUM_OF_PARTICLES;
    int avg_chunk_size = (NUM_OF_PARTICLES / NUM_OF_THREADS) + 1; // Could this be pre-computed on compile time?
    int mutex_num = NUM_OF_THREADS;

    //Scheduling tasks to calculate particle movement...
    while(num_of_particles_to_schedule != 0){
        if(num_of_particles_to_schedule > avg_chunk_size){
            num_of_particles_to_schedule -= avg_chunk_size;
            current_data = create_update_data(swarm, num_of_particles_to_schedule, avg_chunk_size, particle_chunk_mutex[mutex_num]);
            current_task = create_task((void*)current_data, update_swarm_chunk);
            WaitForSingleObject(task_queue_mutex, INFINITE);
            push_task(queue, current_task);
        }
        else{
            current_data = create_update_data(swarm, 0, num_of_particles_to_schedule, particle_chunk_mutex[mutex_num]);
            current_task = create_task((void*)current_data, update_swarm_chunk);
            num_of_particles_to_schedule = 0;
            WaitForSingleObject(task_queue_mutex, INFINITE);
            push_task(queue, current_task);
        }

        mutex_num--;
        ReleaseMutex(task_queue_mutex);
        SetEvent(task_added_event);
    }

    // Scheduling task to update read-only buffer, and copy particle data to app...
    current_task = create_task((void*)data, update_swarm_state);
    WaitForSingleObject(task_queue_mutex, INFINITE);
    push_task(queue, current_task);
    ReleaseMutex(task_queue_mutex);
    SetEvent(task_added_event);

    // At the end we add scheduling task again...
    current_task = create_task(data, schedule_tasks);
    WaitForSingleObject(task_queue_mutex, INFINITE);
    push_task(queue, current_task);
    ReleaseMutex(task_queue_mutex);
    SetEvent(task_added_event);
}

int main(int argc, char **argv){
    printf("App starting...\n");

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

    //Setting up threads and mutexes
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

    task_queue_mutex = CreateMutex(NULL, false, NULL);
    if(task_queue_mutex == NULL){
        printf("Failed to create task queue mutex...\n");
        return EXIT_FAILURE;
    }

    Swarm swarm;
    init_swarm(&swarm, 0, 1, 0, 1, 1, 5, NUM_OF_PARTICLES);

    // This part is used for debugging how scheduler distributes tasks to threads...
    // That is why swarm pointer is NULL, in that case updating function will skip calculations...
    Scheduler_arg sched_arg =  {
        .queue = &task_queue,
        .swarm = &swarm,
        .app = &app
    };

    schedule_tasks((void*)&sched_arg);

    // Main app loop
    while(true){
        
        if(!update_app(&app)){
            break;
        }
    }

    // Closing handles to all thread related objects
    for(int i = 0; i<NUM_OF_THREADS; i++){
        TerminateThread(threads[i], 0);
        CloseHandle(threads[i]);
        CloseHandle(particle_chunk_mutex[i]);
    }
    CloseHandle(task_queue_mutex);
    CloseHandle(task_added_event);

    // Closing app object
    close_app(&app);

    return EXIT_SUCCESS;
}