#include "sdlutil.h"

static SDL_Window* window;
static SDL_Surface* screen;

void init_sdl(const char* wname, int screenw, int screenh)
{
	SDL_Init(SDL_INIT_VIDEO);
	window = SDL_CreateWindow(wname, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, screenw, screenh, 0);
	screen = SDL_GetWindowSurface(window);
}

void quit_sdl()
{
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void draw_rect(int x, int y, int w, int h, int r, int g, int b)
{
	SDL_Rect rect = { x, y, w, h };
	SDL_FillRect(screen, &rect, SDL_MapRGB(screen->format, r, g, b));
}

void clear_screen()
{
	SDL_FillRect(screen, 0, SDL_MapRGB(screen->format, 0, 0, 0));
}

void update_screen()
{
	SDL_UpdateWindowSurface(window);
	SDL_Delay(16);
}

static Uint64 start_ticks;

void start_ticks_count()
{
	start_ticks = SDL_GetTicks64();
}

unsigned get_elapsed_ticks()
{
	return SDL_GetTicks64() - start_ticks;
}

void get_mouse_coords(int* x, int* y)
{
	SDL_GetMouseState(x, y);
}

#include <assert.h>

SDL_Surface* load_surface(const char* filename)
{
	SDL_Surface* loaded = SDL_LoadBMP(filename);
	assert(loaded);
	SDL_Surface* converted = SDL_ConvertSurface(loaded, screen->format, 0);
	assert(converted);
	SDL_FreeSurface(loaded);
	assert(!SDL_SetColorKey(converted, SDL_TRUE, SDL_MapRGB(screen->format, 255, 255, 255)));
	assert(!SDL_SetSurfaceRLE(converted, 1));
	return converted;
}

void blit_surface(SDL_Surface* surf, int srcx, int srcy, int srcw, int srch, int dstx, int dsty, int dstw, int dsth)
{
	assert(surf);
	SDL_Rect srcrect = { srcx, srcy, srcw, srch };
	SDL_Rect dstrect = { dstx, dsty, dstw, dsth };
	SDL_BlitScaled(surf, &srcrect, screen, &dstrect);
}
