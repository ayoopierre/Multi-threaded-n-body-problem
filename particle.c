#include "particle.h"

void init_particle(int x, int y, float mass, Particle *p){
    p->a_x = 0;
    p->a_y = 0;
    p->v_x = 0;
    p->v_y = 0;

    p->mass = mass;
    p->x = x;
    p->y = y;
}

void update_particle(Particle *particle, Particle *particle_list, int num_of_particles){
    for(int i = 0; i<num_of_particles; i++){
        if(particle == &particle_list[i]) continue;
        float delta_x = particle->x - particle_list[i].x;
        float delta_y = particle->y - particle_list[i].y;
        float r_2 = delta_x * delta_x + delta_y * delta_y;
        float a = (G * particle_list[i].mass) / (r_2 * r_2);

        particle->a_x = a * delta_x;
        particle->a_y = a * delta_y;
    }
}