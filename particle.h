#include <math.h>

#define G 1

#ifndef G
#define G (double)0.0000000000066743015
#endif

typedef struct Particle
{
    float a_x;
    float a_y;
    float v_x;
    float v_y;

    int x;
    int y;
    float radius;

    float mass;
} Particle;

typedef struct Swarm
{
    Particle *swarm;
    int num_of_particles;
} Swarm;


void init_particle(int x, int y, float mass, Particle *p);
void update_particle(Particle *particle, Particle *swarm, int num_of_particles);