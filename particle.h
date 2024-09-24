#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define G 1
#define dT 0.001
#define NUM_OF_PARTICLES 100


#ifndef G
#define G (double)0.0000000000066743015
#endif

typedef struct Particle{
    float a_x;
    float a_y;
    float v_x;
    float v_y;

    int x;
    int y;
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
} Update_data;


void init_particle(int x, int y, float mass, Particle *p);
void update_particle(Particle *particle, Particle *read_only_swarm, int num_of_particles);

void init_swarm(Swarm *swarm, int x_min, int x_max, int y_min, int y_max, int mass_min, int mass_max, int num_of_particles);
void update_swarm_chunk(void *data);

Update_data *create_update_data(Swarm *swarm, int starting_pos, int num_to_update);