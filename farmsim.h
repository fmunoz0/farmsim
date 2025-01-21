// farm simulation logic
// all structs are readonly from outside farmsim.c

#ifndef FARMSIM_H
#define FARMSIM_H

#define MAX_TILEMAP_ROWS 100
#define MAX_TILEMAP_COLS 100

#define INVALID_CROPTYPE -1

typedef enum {
	TILE_SOIL = 1,
	TILE_PLANTED = 3,
	TILE_GRASS = 4,
	TILE_WATER = 8,
	INVALID_TILE = 16
} Tile;

typedef int CropType;

typedef struct {
	int rows;
	int cols;
	Tile tiles[MAX_TILEMAP_ROWS][MAX_TILEMAP_COLS];
} Tilemap;

typedef struct Crop {
	CropType type;
	int stage; // from 0 to "cropstages"
	int remainingsteps; // remaining steps for next stage
	int row;
	int col;
	struct Crop* next;
} Crop;

typedef enum {
	S_STOP,
	S_NORMAL,
	S_FAST,
	S_SUPERFAST
} Speed;

typedef struct {
	Tilemap tilemap;
	Crop* crops;
	int numcrops;
	unsigned millisperstep; // 0 means paused
	unsigned elapsedmillis; // goes from 0 to "millisperstep"
	int cropstages;
} Farmsim;

void init_farmsim(Farmsim* sim, int rows, int cols, int cropstages);
void clean_farmsim(Farmsim* sim);

void update_farmsim(Farmsim* sim, unsigned dt);
void plant_crop(Farmsim* sim, int row, int col, CropType type);
void set_speed(Farmsim* fs, Speed s);
void set_tile(Farmsim* fs, int row, int col, Tile t);

#endif