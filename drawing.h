#ifndef DRAWING_H
#define DRAWING_H

#include "farmsim.h"

extern CropType selectedcrop;

#define TILESIZE 32

void init_draw_state(const Farmsim*);
void clean_draw_state();
void draw_everything();

// used by click handler
CropType get_menu_croptype_from_screen_coord(int screenx, int screeny);
void get_tile_from_screen_coord(int screenx, int screeny, int* row, int* col);

#endif