#include "spritesheet.h"
#include "sdlutil.h"
#include <assert.h>
#include <stdlib.h>

struct Spritesheet {
	SDL_Surface* surf;
	int rows;
	int cols;
	int framew;
	int frameh;
};

Spritesheet* load_spritesheet(const char* filename, int framew, int frameh)
{
	assert(framew > 0);
	assert(frameh > 0);
	
	SDL_Surface* surf = load_surface(filename);
	int w = surf->w;
	int h = surf->h;
	
	// surface size is multiple of frame size
	assert((w % framew) == 0);
	assert((h % frameh) == 0);
	
	Spritesheet* sheet = malloc(sizeof(Spritesheet));
	sheet->rows = w / framew;
	sheet->cols = h / frameh;
	sheet->framew = framew;
	sheet->frameh = frameh;
	sheet->surf = surf;
	return sheet;
}

void free_spritesheet(Spritesheet* sheet)
{
	if (sheet) {
		SDL_FreeSurface(sheet->surf);
		free(sheet);
	}
}

void draw_spritesheet_frame(Spritesheet* sheet, int row, int col, int dstx, int dsty, int dstw, int dsth)
{
	assert(sheet);
	int srcx = col * sheet->framew;
	int srcy = row * sheet->frameh;
	blit_surface(sheet->surf, srcx, srcy, sheet->framew, sheet->frameh, dstx, dsty, dstw, dsth);
}
