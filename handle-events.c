#include "drawing.h" // get_menu_croptype_from_screen_coord(), get_tile_from_screen_coord()
#include <SDL.h>

void handle_events(Farmsim* sim, int* quit)
{
	SDL_Event e;
	while (SDL_PollEvent(&e))
		switch (e.type) {
			case SDL_QUIT:
				*quit = 1;
				break;
			case SDL_MOUSEBUTTONDOWN:
				CropType crop;
				Tile tile;
				get_menu_item_at_screen_coord(e.button.x, e.button.y, &crop, &tile);
				if (crop != INVALID_CROPTYPE) {// selected a crop from the menu
					selectedcrop = crop;
					selectedtile = INVALID_TILE;
				} else if (tile != INVALID_TILE) {
					selectedtile = tile;
					selectedcrop = INVALID_CROPTYPE;
				} else if (selectedcrop != INVALID_CROPTYPE || selectedtile != INVALID_TILE) { // clicked outside the menu and we have a previously selected item
					int row, col;
					get_tile_at_screen_coord(e.button.x, e.button.y, &row, &col);
					if (row >= 0 && col >= 0) { // if clicked inside the tilemap
						if (selectedcrop != INVALID_CROPTYPE)
							plant_crop(sim, row, col, selectedcrop);
						else if (selectedtile != INVALID_TILE)
							set_tile(sim, row, col, selectedtile);
					}
				}
				break;
			case SDL_KEYUP:
				switch (e.key.keysym.sym) {
					case SDLK_ESCAPE:
						selectedcrop = INVALID_CROPTYPE;
						selectedtile = INVALID_TILE;
						break;
					case SDLK_1: set_speed(sim, S_NORMAL); break;
					case SDLK_2: set_speed(sim, S_FAST); break;
					case SDLK_3: set_speed(sim, S_SUPERFAST); break;
					case SDLK_0: set_speed(sim, S_STOP); break;
				}
				break;
		}
}