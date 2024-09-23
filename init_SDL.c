#include "init_SDL.h"

int init_app(App *app){
    app->fps = FPS;

    if(SDL_Init(SDL_INIT_EVERYTHING) < 0){
        return SDL_FAILED;
    }

    app->window = SDL_CreateWindow("Window", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
    WINDOW_WIDTH, WINDOW_HEIGHT, 0);

    if(!app->window){
        return WINDOW_FAILED;
    }

    app->renderer = SDL_CreateRenderer(app->window, -1, SDL_RENDERER_ACCELERATED);

    if(!app->renderer){
        return RENDERER_FAILED;
    }

    app->texture = SDL_CreateTexture(app->renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, WINDOW_WIDTH, WINDOW_HEIGHT);

    if(!app->texture){
        return TEXTURE_FAILED;
    }

    app->buffer = malloc(sizeof(uint32_t) * WINDOW_HEIGHT * WINDOW_WIDTH);
    app->last_x_mouse = -1;
    app->last_y_mouse = -1;
    return SDL_SUCCSSES;
}

bool update_app(App *app){

    // We calculate current FPS, from time elapsed, if not enough time elapsed since last time
    // image got refreshed we skip it to go calculate physics for more accurate results.
    float curr_fps = (float)SDL_GetPerformanceFrequency() / (SDL_GetPerformanceCounter() - app->last_tick); 
    if(curr_fps < app->fps){
        printf("FPS: %f\n", curr_fps);
        // get data from simulation to draw an image
        write_buffer_to_texture(app);
        SDL_RenderClear(app->renderer);
        SDL_RenderCopy(app->renderer, app->texture, NULL, NULL);
        SDL_RenderPresent(app->renderer);
        app->last_tick = SDL_GetPerformanceCounter();
    }
    return handle_input(app);
}

void write_buffer_to_texture(App *app){
    uint32_t *pixel;
    int pitch;

    // In case that lock fails skip copying data to texture (this could crash program idk).
    if(SDL_LockTexture(app->texture, NULL, (void **)&pixel, &pitch)) return;
    memcpy(pixel, app->buffer, sizeof(uint32_t) * WINDOW_HEIGHT * WINDOW_WIDTH);
    SDL_UnlockTexture(app->texture);
}

void update_graphics(App *app){

}

bool handle_input(App *app){
    int x_mouse;
    int y_mouse;
    Uint32 state = SDL_GetMouseState(&x_mouse, &y_mouse);
    //handle mouse movements

    app->last_x_mouse = x_mouse;
    app->last_y_mouse = y_mouse;

    SDL_Event event;
    while (SDL_PollEvent(&event)){
        switch (event.type)
        {
            case SDL_QUIT:
                return false;
            default:
                break;
        }
    }

    return true;
}

void close_app(App *app){
    SDL_DestroyRenderer(app->renderer);
    SDL_DestroyWindow(app->window);
}