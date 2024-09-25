#include "particle.h"

void init_particle(float x, float y, float mass, Particle *p){
    p->a_x = 0;
    p->a_y = 0;
    p->v_x = 0;
    p->v_y = 0;

    p->mass = mass;
    p->x = x;
    p->y = y;
}


void update_particle(Particle *particle, Particle *read_only_swarm, int num_of_particles, int particle_index){
    for(int i = 0; i<num_of_particles; i++){
        if(particle_index == i) continue;
        float delta_x = particle->x - read_only_swarm[i].x;
        float delta_y = particle->y - read_only_swarm[i].y;
        float r_2 = delta_x * delta_x + delta_y * delta_y;
        float a = (G * read_only_swarm[i].mass) / (r_2 * r_2);

        particle->a_x += a * delta_x;
        particle->a_y += a * delta_y;
    }

    particle->v_x += particle->a_x * dT;
    particle->v_y += particle->a_y * dT;
    particle->x += particle->v_x * dT;
    particle->y += particle->v_y *dT;
}


void init_swarm(Swarm *swarm, float x_min, float x_max, float y_min, float y_max, float mass_min, float mass_max, int num_of_particles){
    swarm->num_of_particles = num_of_particles;
    swarm->swarm = (Particle *)malloc(sizeof(Particle) * num_of_particles);
    swarm->read_only_swarm = (Particle *)malloc(sizeof(Particle) * num_of_particles);

    for(int i = 0; i<num_of_particles; i++){
        Particle *p = &swarm->swarm[i];
        p->a_x = 0;
        p->a_y = 0;
        p->v_x = 0;
        p->v_y = 0;
        p->mass = (float)rand() / ((float)RAND_MAX / (mass_max - mass_min)) + mass_min;
        p->radius = 1;
        p->x = (float)rand() / ((float)RAND_MAX / (x_max - x_min)) + x_min;
        p->y = (float)rand() / ((float)RAND_MAX / (y_max - y_min)) + y_min;
    }

    memcpy(swarm->read_only_swarm, swarm->swarm, sizeof(Particle) * num_of_particles);
}


void update_swarm_chunk(void *data){
    Update_data *data_for_update = (Update_data*)data;
    if(data_for_update->swarm == NULL) return;
    WaitForSingleObject(data_for_update->chunk_mutex, INFINITE);
    int starting_pos = data_for_update->starting_pos;
    for(int i = 0; i<data_for_update->num_to_update; i++){
        update_particle(
            &data_for_update->swarm->swarm[starting_pos + i], 
            data_for_update->swarm->read_only_swarm, 
            data_for_update->swarm->num_of_particles,
            data_for_update->starting_pos + i
            );
    };
    ReleaseMutex(data_for_update->chunk_mutex);
    free(data);
}

Update_data *create_update_data(Swarm *swarm, int starting_pos, int num_to_update, HANDLE chunk_mutex){
    Update_data *ret = (Update_data*)malloc(sizeof(Update_data));
    ret->swarm = swarm;
    ret->starting_pos = starting_pos;
    ret->num_to_update = num_to_update;
    ret->chunk_mutex = chunk_mutex;
    return ret;
}