#include "sdlutil.h"
#include "farmsim.h"
#include "drawing.h"

#define ROWS 15
#define COLS 20
#define SCREENW COLS*TILESIZE
#define SCREENH (ROWS+1)*TILESIZE

void handle_events(Farmsim* sim, int* quit); // handle-events.c

int main()
{
	int quit = 0;
	Farmsim farmsim;
	init_farmsim(&farmsim, ROWS, COLS, 5);
	plant_crop(&farmsim, 1, 1, 6); // test crop

	init_sdl("farming simulator", SCREENW, SCREENH);
	init_draw_state(&farmsim);

	start_ticks_count();
	
	while (!quit) {
		handle_events(&farmsim, &quit);
		
		update_farmsim(&farmsim, get_elapsed_ticks());
		start_ticks_count();
		
		clear_screen();
		draw_everything();
		update_screen();
	}
	
	clean_draw_state();
	clean_farmsim(&farmsim);
	quit_sdl();
	
	return 0;
}