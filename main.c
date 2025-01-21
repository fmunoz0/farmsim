#include "sdlutil.h"
#include "farmsim.h"
#include "drawing.h"

void handle_events(Farmsim* sim, int* quit); // handle-events.c

int main()
{
	int quit = 0;
	Farmsim farmsim;
	init_farmsim(&farmsim, 20, 30, 5);

	init_sdl("farming simulator", 30<<5, 21<<5);
	init_draw_state(&farmsim);

	start_ticks_count();
	
	while (!quit) {
		handle_events(&farmsim, &quit);
		
		update_farmsim(&farmsim, get_elapsed_ticks());
		update_draw_state(get_elapsed_ticks());
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