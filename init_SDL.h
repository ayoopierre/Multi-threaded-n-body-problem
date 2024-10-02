#include <stdbool.h>
#include <stdlib.h>
#include <SDL2/SDL.h>
#include <string.h>
#include <Windows.h>
#include "particle.h"

#ifndef INIT_SDL_H
#define INIT_SDL_H

#define WINDOW_HEIGHT 600
#define WINDOW_WIDTH 800
#define FPS 20

//TODO -> change name to smth like handle_SDL.h

enum SDL_init_codes{
    SDL_SUCCSSES,
    SDL_FAILED,
    WINDOW_FAILED,
    RENDERER_FAILED,
    TEXTURE_FAILED
};

enum Graphics_state{
    IDLE,
    DATA_REQUESTED
};


typedef struct{
    SDL_Renderer *renderer;
    SDL_Window *window;
    SDL_Texture *texture;
    uint32_t *buffer;
    Uint64 last_tick;
    int last_x_mouse;
    int last_y_mouse;
    int fps;
    Particle *swarm;
    HANDLE buffer_copied_event;
    int flag;
} App;

int init_app(App *app);
bool update_app(App *app);
void close_app(App *app);
void write_buffer_to_texture(App *app);
void update_graphics(App *app);
void draw_circle_to_buffer(App *app, float x, float y, float radius);
bool handle_input(App *app);

#endif