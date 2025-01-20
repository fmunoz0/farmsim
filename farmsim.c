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
	for (int r = 0; r < rows; r++) {
		tilemap->tiles[r][0] = TILE_GRASS;
		tilemap->tiles[r][cols - 1] = TILE_GRASS;
	}
	
	for (int c = 0; c < cols; c++) {
		tilemap->tiles[0][c] = TILE_GRASS;
		tilemap->tiles[rows - 1][c] = TILE_GRASS;
	}
	
	// small lake on the center
	tilemap->tiles[rows>>1][cols>>1] = TILE_WATER;
	tilemap->tiles[(rows>>1)-1][cols>>1] = TILE_WATER;
	tilemap->tiles[rows>>1][(cols>>1)-1] = TILE_WATER;
	tilemap->tiles[(rows>>1)-1][(cols>>1)-1] = TILE_WATER;
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
	sim->stepmillis = 2000;
	sim->elapsedmillis = 0;
	sim->cropstages = cropstages;
}

static int calc_steps_for_next_stage()
{
	return 3 + (rand() % 6);
}

static void update_crops_single_step(Farmsim* sim)
{
	for (Crop* c = sim->crops; c; c = c->next) {
		assert(c->remainingsteps > 0);
		assert(c->stage >= 0 && c->stage < sim->cropstages);
		
		if (c->stage == sim->cropstages - 1)
			continue;
		
		c->remainingsteps--;
		
		if (c->remainingsteps == 0) { // next stage
			c->stage++;
			c->remainingsteps = calc_steps_for_next_stage();
		}
	}
}

void update_farmsim(Farmsim* sim, unsigned dt)
{
	int updatesteps = 0;
	while (dt >= sim->stepmillis) {
		dt -= sim->stepmillis;		
		updatesteps++;
	}
	
	if (sim->elapsedmillis + dt >= sim->stepmillis) {
		sim->elapsedmillis = sim->elapsedmillis + dt - sim->stepmillis;
		dt = 0;
		updatesteps++;
	}
	
	sim->elapsedmillis += dt;
	
	for (int i = 0; i < updatesteps; i++)
		update_crops_single_step(sim);
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