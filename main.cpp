#include <iostream>
#include <SDL2/SDL.h>

#define TICK_INTERVAL 10

static Uint32 next_time;
Uint32 time_left(void)
{
    Uint32 now;
    now = SDL_GetTicks();
    if(next_time <= now)
        return 0;
    else
        return next_time - now;
}

float randomf(int min, int max, int div){
    int r = rand()% max;
    return (float)min + ((float)r / (float)div);
}

namespace vars {
    /* Game state */
    namespace g {
        int state; /* 0 = playing, 1 = over, 2 = waiting */
    }
    /* Window */
    namespace w {
        int w, h;
        int m; /* Margin */
    }
    /* Player */
    namespace p {
        int w, h;
        int x, y;
    }
    /* Mouse */
    namespace m {
        int x, y;
    }
    /* Ball */
    namespace b {
        int x, y;
        float dirX, dirY;
        int s; /* Size */
        float spm; /* Speed modifier */
    }
}

int main() {
    srand(time(NULL));

    /* --- Init SDL --- */
    if (SDL_Init(SDL_INIT_VIDEO)) {
        std::cout << "SDL failed to init video." << std::endl;
    }

    /* --- Setup window --- */
    vars::w::m = 40;
    vars::w::w = 1024;
    vars::w::h = 768;
    bool running = true;
    SDL_Window* window = NULL;
    SDL_Renderer* renderer = NULL;
    window = SDL_CreateWindow("Bounce Game", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, vars::w::w, vars::w::h, SDL_WINDOW_OPENGL);
    renderer = SDL_CreateRenderer(window, -1, 0);
    SDL_Rect topMarginRect = {0, 0, vars::w::w, vars::w::m};
    SDL_Rect leftMarginRect = {0, 0, vars::w::m, vars::w::h};
    SDL_Rect rightMarginRect = {vars::w::w - vars::w::m, 0, vars::w::w - vars::w::m, vars::w::h};
    SDL_Rect bottomMarginRect = {0, vars::w::h - vars::w::m, vars::w::w, vars::w::h};

    /* --- Init player vars --- */
    vars::p::w = 100;
    vars::p::h = 30;
    vars::p::x = (vars::w::w / 2) - (vars::p::w / 2);
    vars::p::y = vars::w::h - (vars::p::h * 4);

    vars::g::state = 1;
    /* --- Game Loop --- */
    next_time = SDL_GetTicks() + TICK_INTERVAL;
    SDL_Event event;
    while (running){


        if (vars::g::state == 1) {
            /* --- Init ball vars --- */
            vars::b::x = vars::w::w / 2;
            vars::b::y = vars::w::m * 3;
            vars::b::s = 50;
            vars::b::spm = 0.1;
            vars::b::dirX = 0;//0.5;
            vars::b::dirY = 1;
            vars::g::state += 1;

        }
        
        /* ----------------- */
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
        SDL_RenderClear(renderer); 


        /* Margins */
        SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
        SDL_RenderDrawLine(renderer, vars::w::m, vars::w::m, vars::w::w - vars::w::m, vars::w::m);
        SDL_RenderDrawLine(renderer, vars::w::m, vars::w::m, vars::w::m, vars::w::h - vars::w::m);
        SDL_RenderDrawLine(renderer, vars::w::w - vars::w::m, vars::w::m, vars::w::w - vars::w::m, vars::w::h - vars::w::m);
        SDL_SetRenderDrawColor(renderer, 255, 140, 80, 255);
        SDL_RenderDrawLine(renderer, vars::w::m, vars::w::h - vars::w::m, vars::w::w - vars::w::m, vars::w::h - vars::w::m);

        /* Player */
        SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
        SDL_Rect playerRect = {vars::p::x, vars::p::y, vars::p::w, vars::p::h};
        SDL_RenderDrawRect(renderer, &playerRect);
        /* Ensure player stays in the window */
        {
            vars::p::x = vars::m::x - (vars::p::w / 2);
            if (vars::p::x > (vars::w::w - vars::p::w - vars::w::m)){
                vars::p::x = vars::w::w - vars::w::m - vars::p::w;
            }
            if (vars::p::x < vars::w::m){
                vars::p::x = vars::w::m;
            }
        }

        /* Ball */
        SDL_SetRenderDrawColor(renderer, 60, 200, 255, 255);
        SDL_RenderDrawLine(renderer, vars::b::x + (vars::b::s / 2), vars::b::y,  vars::b::x, vars::b::y - (vars::b::s / 2));
        SDL_RenderDrawLine(renderer, vars::b::x + (vars::b::s / 2), vars::b::y,  vars::b::x, vars::b::y + (vars::b::s / 2));
        SDL_RenderDrawLine(renderer, vars::b::x - (vars::b::s / 2), vars::b::y, vars::b::x, vars::b::y - (vars::b::s / 2));
        SDL_RenderDrawLine(renderer, vars::b::x - (vars::b::s / 2), vars::b::y, vars::b::x, vars::b::y + (vars::b::s / 2));
        /* Ball Collision Detection */
        {
            SDL_Rect ballRect = {vars::b::x - (vars::b::s / 2), vars::b::y - (vars::b::s / 2), vars::b::s, vars::b::s};
            if (SDL_HasIntersection(&ballRect, &playerRect)) {
                vars::b::dirY *= -1;
                vars::b::dirX += randomf(0, 10, 10) * (vars::b::dirX > 1 ? 1 : -1);
                vars::b::dirY += vars::b::spm * (vars::b::dirY > 1 ? 1 : -1);
                vars::b::y = vars::p::y - vars::b::s;
            }
            else if (SDL_HasIntersection(&ballRect, &topMarginRect)) {
                vars::b::dirY *= -1;
                vars::b::dirX += randomf(0, 10, 10) * (vars::b::dirX > 1 ? 1 : -1);
                vars::b::dirY += vars::b::spm * (vars::b::dirY > 1 ? 1 : -1);
                vars::b::y = vars::w::m + vars::b::s;
            }
            else if (SDL_HasIntersection(&ballRect, &leftMarginRect)) {
                vars::b::dirX *= -1;
                vars::b::dirY += randomf(0, 10, 10) * (vars::b::dirY > 1 ? 1 : -1);
                vars::b::dirX += vars::b::spm * (vars::b::dirX > 1 ? 1 : -1);
                vars::b::x = vars::w::m + vars::b::s;
            }
            else if (SDL_HasIntersection(&ballRect, &rightMarginRect)) {
                vars::b::dirX *= -1;
                vars::b::dirY += randomf(0, 10, 10) * (vars::b::dirY > 1 ? 1 : -1);
                vars::b::dirX += vars::b::spm * (vars::b::dirX > 1 ? 1 : -1);
                vars::b::x = vars::w::w - vars::w::m - vars::b::s;
            }
            else if (SDL_HasIntersection(&ballRect, &bottomMarginRect)) {
                vars::g::state = 1;
            }
        }

        if (vars::g::state == 0) {
            /* Ball Movement */
            vars::b::x += vars::b::dirX;
            vars::b::y += vars::b::dirY;
        }

        SDL_RenderPresent(renderer);
        /* ----------------- */

        /* --- Events --- */
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_MOUSEMOTION:
                    SDL_GetMouseState(&vars::m::x, &vars::m::y);
                    break;
                case SDL_QUIT:
                    running = false;
                    break;
                case SDL_KEYDOWN:
                    switch (event.key.keysym.sym) {
                        case SDLK_ESCAPE:
                            running = false;
                            break;
                        case SDLK_SPACE:
                            if (vars::g::state == 2){
                                vars::g::state = 0;
                            }
                            break;
                            break;
                        default:
                            break;
                    }
                    break;
                default:
                    break;
            }
        }

        /* --- Handle Framerate --- */
        SDL_Delay(time_left());
        next_time += TICK_INTERVAL;
    }
    
    /* --- Clean up --- */
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    std::cout << "Closing." << std::endl;
    return 0;
}