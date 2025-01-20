#include "farmsim.h"
#include <assert.h>
#include <stdlib.h>
#include <time.h>

static const char* cropnames[] = {
	"turnip",
	"cucumber",
	"tomato",
	"eggplant",
	"pineapple",
	"wheat",
	"strawberry",
	"potato",
	"orange",
	"corn",
	"rose",
	"tulip",
	"melon",
	"lemon",
	"rice",
	"grapes",
	"cassava",
	"coffee",
	"avocado",
	"sunflower"
};

static void init_tilemap(Tilemap* tilemap, int rows, int cols)
{
	assert(rows < MAX_TILEMAP_ROWS);
	assert(cols < MAX_TILEMAP_COLS);
	tilemap->rows = rows;
	tilemap->cols = cols;
	
	for (int r = 0; r < rows; r++)
		for (int c = 0; c < cols; c++)
			tilemap->tiles[r][c] = TILE_SOIL;
		
	// grass borders
	for (int r = 0; r < rows; r++)
		tilemap->tiles[r][0] = tilemap->tiles[r][cols-1] = TILE_GRASS;
		
	for (int c = 0; c < cols; c++)
		tilemap->tiles[0][c] = tilemap->tiles[rows-1][c] = TILE_GRASS;
	
	// canal
	tilemap->tiles[2][4] = TILE_WATER;
	tilemap->tiles[2][5] = TILE_WATER;
	tilemap->tiles[2][6] = TILE_WATER;
	tilemap->tiles[2][7] = TILE_WATER;
	tilemap->tiles[2][8] = TILE_WATER;
	tilemap->tiles[2][9] = TILE_WATER;
	tilemap->tiles[2][10] = TILE_WATER;
	tilemap->tiles[2][11] = TILE_WATER;
	tilemap->tiles[2][12] = TILE_WATER;
	tilemap->tiles[2][13] = TILE_WATER;
	tilemap->tiles[3][13] = TILE_WATER;
	tilemap->tiles[3][14] = TILE_WATER;
	tilemap->tiles[3][15] = TILE_WATER;
	tilemap->tiles[4][15] = TILE_WATER;
	tilemap->tiles[5][15] = TILE_WATER;
	tilemap->tiles[6][15] = TILE_WATER;
	tilemap->tiles[3][4] = TILE_WATER;
	tilemap->tiles[4][4] = TILE_WATER;
	tilemap->tiles[5][4] = TILE_WATER;
	tilemap->tiles[6][4] = TILE_WATER;
	tilemap->tiles[7][4] = TILE_WATER;
	
	// canal
	tilemap->tiles[6][10] = TILE_WATER;
	tilemap->tiles[7][10] = TILE_WATER;
	tilemap->tiles[8][10] = TILE_WATER;
	tilemap->tiles[9][10] = TILE_WATER;
	tilemap->tiles[10][10] = TILE_WATER;
	tilemap->tiles[11][10] = TILE_WATER;
	
	tilemap->tiles[11][9] = TILE_WATER;
	tilemap->tiles[11][8] = TILE_WATER;
	tilemap->tiles[11][7] = TILE_WATER;
	tilemap->tiles[11][6] = TILE_WATER;
	tilemap->tiles[11][5] = TILE_WATER;
	tilemap->tiles[11][4] = TILE_WATER;
	tilemap->tiles[11][3] = TILE_WATER;
	
	// small lake
	tilemap->tiles[9][14] = TILE_WATER;
	tilemap->tiles[9][15] = TILE_WATER;
	tilemap->tiles[9][16] = TILE_WATER;
	tilemap->tiles[10][14] = TILE_WATER;
	tilemap->tiles[10][15] = TILE_WATER;
	tilemap->tiles[10][16] = TILE_WATER;
	tilemap->tiles[11][16] = TILE_WATER;
	tilemap->tiles[11][15] = TILE_WATER;
	tilemap->tiles[11][14] = TILE_WATER;
	
}

void init_farmsim(Farmsim* sim, int rows, int cols, int cropstages)
{
	assert(rows > 0);
	assert(cols > 0);
	assert(cropstages > 0);
	
	srand(time(NULL));
	
	init_tilemap(&sim->tilemap, rows, cols);
	sim->crops = 0;
	sim->numcrops = 0;
	sim->elapsedmillis = 0;
	sim->cropstages = cropstages;
	set_speed(sim, S_NORMAL);
}

static int calc_steps_for_next_stage()
{
	return 3 + (rand() % 6);
}

void update_farmsim(Farmsim* sim, unsigned dt)
{
	if (!sim->millisperstep)
		return;
	
	// "dt" can be large if the main loop got stuck atending events (holding the window titlebar for example)
	// put an upper limit just to simplify logic and pretend it didn't get stuck
	dt = dt % 100;
	
	sim->elapsedmillis += dt;
	
	if (sim->elapsedmillis >= sim->millisperstep) {
		sim->elapsedmillis -= sim->millisperstep;

		// for each crop, advance a step
		for (Crop* c = sim->crops; c; c = c->next) {
			assert(c->remainingsteps > 0);
			assert(c->stage >= 0 && c->stage < sim->cropstages);
			
			if (c->stage == sim->cropstages - 1)
				continue;
			
			c->remainingsteps--;
			
			if (!c->remainingsteps) { // next stage
				c->stage++;
				c->remainingsteps = calc_steps_for_next_stage();
			}
		}
	}
}

void clean_farmsim(Farmsim* sim)
{
	for (Crop* c = sim->crops; c; ) {
		Crop* next = c->next;
		free(c);
		c = next;
	}
}

#include <stdio.h>

void plant_crop(Farmsim* sim, int row, int col, CropType type)
{
	if (type < 0 || type >= 20) {
		printf("warning: tried planting invalid crop %d\n", type);
		return;
	}
	
	if (row < 0 || row >= sim->tilemap.rows) {
		printf("warning: tried planting crop at out of range row %d\n", row);
		return;
	}
	
	if (col < 0 || col >= sim->tilemap.cols)  {
		printf("warning: tried planting crop at out of range col %d\n", col);
		return;
	}
	
	if (sim->tilemap.tiles[row][col] != TILE_SOIL)
		return;
	
	sim->tilemap.tiles[row][col] = TILE_PLANTED;
	
	Crop* newcrop = malloc(sizeof(Crop));
	newcrop->type = type;
	newcrop->row = row;
	newcrop->col = col;
	newcrop->stage = 0;
	newcrop->remainingsteps = calc_steps_for_next_stage();
	newcrop->next = 0;
	
	Crop** pnext = &sim->crops;
	while (*pnext) {
		Crop* next = *pnext;
		pnext = &next->next;
	}
	*pnext = newcrop;
	sim->numcrops++;
	
	printf("planted crop '%s' at (%d, %d)\n", cropnames[type], row, col);
}

void set_speed(Farmsim* fs, Speed s)
{
	switch (s) {
		case S_STOP: fs->millisperstep = 0; break;
		case S_NORMAL: fs->millisperstep = 5000; break;
		case S_FAST: fs->millisperstep = 3000; break;
		case S_SUPERFAST: fs->millisperstep = 1000; break;
	}
}