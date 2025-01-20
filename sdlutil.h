#ifndef SDLUTIL_H
#define SDLUTIL_H

#include <SDL.h>

void init_sdl(const char* wname, int screenw, int screenh);
void quit_sdl();
void draw_rect(int x, int y, int w, int h, int r, int g, int b);
void clear_screen();
void update_screen();

void start_ticks_count();
unsigned get_elapsed_ticks();

void get_mouse_coords(int* x, int* y);

SDL_Surface* load_surface(const char* filename);

void blit_surface(SDL_Surface* surf, int srcx, int srcy, int srcw, int srch, int dstx, int dsty, int dstw, int dsth);

#endif