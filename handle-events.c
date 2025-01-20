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
				CropType type = get_menu_croptype_from_screen_coord(e.button.x, e.button.y);
				if (type != INVALID_CROPTYPE) // selected a crop from the menu
					selectedcrop = type;
				else if (selectedcrop != INVALID_CROPTYPE) { // clicked outside the menu
					int row, col;
					get_tile_from_screen_coord(e.button.x, e.button.y, &row, &col);
					if (row >= 0 && col >= 0) // if clicked inside the tilemap, try placing the selected crop
						plant_crop(sim, row, col, selectedcrop);
				}
				break;
			case SDL_KEYDOWN:
				if (e.key.keysym.sym == SDLK_ESCAPE)
					selectedcrop = INVALID_CROPTYPE;
				break;
		}
}