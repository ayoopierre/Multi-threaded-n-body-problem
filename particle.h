#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <Windows.h>

#define G 0.0000000001
#define dT 0.001
#define NUM_OF_PARTICLES 1000 // For 2000 we still get multiple simulations per frame


#ifndef G
#define G (double)0.0000000000066743015
#endif

typedef struct Particle{
    float a_x;
    float a_y;
    float v_x;
    float v_y;

    float x;
    float y;
    float radius;

    float mass;
} Particle;

typedef struct Swarm{
    Particle *swarm;
    Particle *read_only_swarm;
    int num_of_particles;
} Swarm;

typedef struct Update_data
{
    Swarm *swarm;
    int starting_pos;
    int num_to_update;
    HANDLE chunk_mutex;
} Update_data;


void init_particle(float x, float y, float mass, Particle *p);
void update_particle(Particle *particle, Particle *read_only_swarm, int num_of_particles, int particle_index);

void init_swarm(Swarm *swarm, float x_min, float x_max, float y_min, float y_max, float mass_min, float mass_max, int num_of_particles);
void update_swarm_chunk(void *data);

Update_data *create_update_data(Swarm *swarm, int starting_pos, int num_to_update, HANDLE chunk_mutex);