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
typedef int CropStage;

typedef struct {
	int rows;
	int cols;
	Tile tiles[MAX_TILEMAP_ROWS][MAX_TILEMAP_COLS];
} Tilemap;

typedef struct Crop {
	CropType type;
	CropStage stage; // from 0 to Farmsim.cropstages - 1
	int remainingsteps; // remaining steps for next stage (always positive)
	int row;
	int col;
	struct Crop* next;
} Crop;

typedef struct {
	Tilemap tilemap;
	Crop* crops;
	int numcrops;
	unsigned stepmillis; // milliseconds per step
	unsigned elapsedmillis; // goes from 0 to "stepmillis"
	int cropstages;
} Farmsim;

void init_farmsim(Farmsim* sim, int rows, int cols, int cropstages);
void clean_farmsim(Farmsim* sim);

void update_farmsim(Farmsim* sim, unsigned dt);
void plant_crop(Farmsim* sim, int row, int col, CropType type);

#endif