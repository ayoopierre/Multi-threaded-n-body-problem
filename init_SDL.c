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

    app->flag = IDLE;
    app->buffer_copied_event = CreateEvent(NULL, false, false, NULL);
    app->swarm = (Particle*)malloc(sizeof(Particle) * NUM_OF_PARTICLES);

    return SDL_SUCCSSES;
}

bool update_app(App *app){

    // We calculate current FPS, from time elapsed, if not enough time elapsed since last time
    // image got refreshed we skip it to go calculate physics for more accurate results.
    float curr_fps = (float)SDL_GetPerformanceFrequency() / (SDL_GetPerformanceCounter() - app->last_tick); 
    if(curr_fps < app->fps){
        printf("FPS: %f\n", curr_fps);
        // get data from simulation to draw an image
        //memset(app->buffer, 0xFF00FFFF, sizeof(uint32_t) * WINDOW_HEIGHT * WINDOW_WIDTH);
        update_graphics(app);
        write_buffer_to_texture(app);
        SDL_RenderClear(app->renderer);
        SDL_RenderCopy(app->renderer, app->texture, NULL, NULL);
        SDL_RenderPresent(app->renderer);
        memset(app->buffer, 0, sizeof(uint32_t)*WINDOW_HEIGHT*WINDOW_WIDTH);
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
    app->flag = DATA_REQUESTED;
    WaitForSingleObject(app->buffer_copied_event, INFINITE);
    app->flag = IDLE;

    if(app->swarm == NULL) return;

    // Now write data to buffer according to particles postions...
    // It might be good idea to render some circles in additional threads.
    // But still if there are N particles simulation needs to N^2 operations while rendering needs only N.
    for(int i = 0; i<NUM_OF_PARTICLES; i++){
        if(app->swarm[i].x + app->swarm[i].radius < 0 || app->swarm[i].x - app->swarm[i].radius > WINDOW_WIDTH) continue;
        if(app->swarm[i].y + app->swarm[i].radius < 0 || app->swarm[i].y - app->swarm[i].radius > WINDOW_WIDTH) continue;
        draw_circle_to_buffer(app, app->swarm[i].x, app->swarm[i].y, app->swarm[i].radius);
    }
}

void draw_circle_to_buffer(App *app, float x, float y, float radius){
    for(int i = x - radius; i < x + radius; i++){
        for(int j = y - radius; j < y + radius; j++){
            // if current pixel is not in the window, we skip it...
            if(i < 0 || i >= WINDOW_WIDTH) continue;
            if(j < 0 || j >=  WINDOW_HEIGHT) continue;
            // we check whether current pixel from bounding box of a circle is inside a circle...
            if((i - x) * (i - x) + (j - y) * (j - y) < radius * radius){ 
                app->buffer[i + j * WINDOW_WIDTH] = 0x0000FFFF;
            }
        }
    }
}

bool handle_input(App *app){
    int x_mouse;
    int y_mouse;
    Uint32 state = SDL_GetMouseState(&x_mouse, &y_mouse);
    //handle mouse movements

    //
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
    SDL_DestroyTexture(app->texture);
    SDL_DestroyRenderer(app->renderer);
    SDL_DestroyWindow(app->window);
    CloseHandle(app->buffer_copied_event);
    free(app->buffer);
    free(app->swarm);
}