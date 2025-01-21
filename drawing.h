#ifndef DRAWING_H
#define DRAWING_H

#include "farmsim.h"

extern CropType selectedcrop;
extern Tile selectedtile;

#define TILESIZE 32

void init_draw_state(const Farmsim*);
void clean_draw_state();
void draw_everything();
void update_draw_state(unsigned dt);

// used by click handler
void get_menu_item_at_screen_coord(int screenx, int screeny, CropType* crop, Tile* tile);
void get_tile_at_screen_coord(int screenx, int screeny, int* row, int* col);

#endif