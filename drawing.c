#include "drawing.h"
#include "spritesheet.h"
#include "sdlutil.h"
#include <assert.h>

#define CROPTYPES 20
#define TILEMAP_OFFSETY TILESIZE // the first row is for the menu

CropType selectedcrop = INVALID_CROPTYPE;

static const Farmsim* farmsim;

#define NUM_SPRITESHEETS 5
static Spritesheet* spritesheets[NUM_SPRITESHEETS];

#define T_SOIL 1
#define T_GRASS 2
#define T_WATER 3

#define RIGHT(t) (t)
#define LEFT(t) (t << 2)
#define BOTTOM(t) (t << 4)
#define TOP(t) (t << 6)
#define CENTER(t) (t << 8)

#define SPRT_UNKNOWN 0
#define SPRT_GRASS_MUD 1
#define SPRT_GRASS_WATER 2
#define SPRT_MUD_WATER 3
#define SPRT_CROPS 4

#define F_UNKNOWN 0
#define F_MUD (SPRT_GRASS_MUD<<8) | 5
#define F_GRASS (SPRT_GRASS_MUD<<8) | 4
#define F_WATER (SPRT_GRASS_WATER<<8) | 14


// encoded frame format: ssss ffff ffff
//                          |         +- frame number in the spritesheet
//                          +----------- spritesheet 

// encode the type of a tile and it's neighborgs in a single integer
// encoded tiles format: 00cc ttbb llrr
//                          |  | |  | +- right
//                          |  | |  +--- left
//                          |  | +------ bottom
//                          |  +-------- top
//                          +----------- center

// frametable[encoded_tiles] -> encoded_frame
static int frametable[1024];

static void init_frametable()
{
	for (int i = 0; i < 1024; i++)
		frametable[i] = F_UNKNOWN;
	
	// assign frame depending on the center tile
	for (int i = 0; i < 256; i++ ) {
		frametable[CENTER(T_SOIL) | i] = F_MUD;
		frametable[CENTER(T_GRASS) | i] = F_GRASS;
		frametable[CENTER(T_WATER) | i] = F_WATER;
	}
}

void init_draw_state(const Farmsim* _farmsim)
{
	farmsim = _farmsim;
	
	memset(spritesheets, 0, sizeof(spritesheets));
	spritesheets[SPRT_CROPS] = load_spritesheet("spritesheets/crops.bmp", 16, 16);
	spritesheets[SPRT_GRASS_MUD] = load_spritesheet("spritesheets/grass-mud.bmp", 16, 16);
	spritesheets[SPRT_MUD_WATER] = load_spritesheet("spritesheets/mud-water.bmp", 16, 16);
	spritesheets[SPRT_GRASS_WATER] = load_spritesheet("spritesheets/grass-water.bmp", 16, 16);
	
	init_frametable();
}

void clean_draw_state()
{
	for (int i = 0; i < NUM_SPRITESHEETS; i++)
		free_spritesheet(spritesheets[i]);
}

static int get_tile_type(Tile t)
{
	switch (t) {
		case TILE_PLANTED:
		case TILE_SOIL:
			return T_SOIL;
		case TILE_GRASS:
			return T_GRASS;
		case TILE_WATER:
			return T_WATER;
		default:
	}
	
	assert(0);
}

static int get_encoded_frame(const Tilemap* tilemap, int row, int col)
{
	int c = get_tile_type(tilemap->tiles[row][col]);
	
	// for border tiles only encode the center
	if (row == 0 || row == tilemap->rows-1 || col == 0 || col == tilemap->cols-1)
		return frametable[CENTER(c)];
	
	int t = get_tile_type(tilemap->tiles[row-1][col]);
	int b = get_tile_type(tilemap->tiles[row+1][col]);
	int l = get_tile_type(tilemap->tiles[row][col-1]);
	int r = get_tile_type(tilemap->tiles[row][col+1]);
	
	int encodedtiles = CENTER(c) | TOP(t) | BOTTOM(b) | LEFT(l) | RIGHT(r);
	
	return frametable[encodedtiles];
}

static void draw_tile(int row, int col, int encodedframe)
{
	int x = col * TILESIZE;
	int y = row * TILESIZE + TILEMAP_OFFSETY;
	int w = TILESIZE;
	int h = TILESIZE;
	
	int sprt = encodedframe >> 8;
	if (sprt == SPRT_UNKNOWN) {
		draw_rect(x, y, w, h, 255, 0, 255);
		return;
	}
	
	int frame = encodedframe & 0xff;
	
	assert(sprt < NUM_SPRITESHEETS);
	draw_spritesheet_frame(spritesheets[sprt], 0, frame, x, y, w, h);
}

void draw_everything()
{
	// draw tiles
	for (int r = 0; r < farmsim->tilemap.rows; r++)
		for (int c = 0; c < farmsim->tilemap.cols; c++) {
			int frame = get_encoded_frame(&farmsim->tilemap, r, c);
			draw_tile(r, c, frame);
		}
		
	// draw crops
	for (Crop* c = farmsim->crops; c; c = c->next) {
		int x = c->col * TILESIZE;
		int y = c->row * TILESIZE + TILEMAP_OFFSETY;
		int frame = 5 - c->stage; // crop stages go from right to left in the spritesheet
		draw_spritesheet_frame(spritesheets[SPRT_CROPS], c->type, frame, x, y, TILESIZE, TILESIZE);
	}

	// draw menu
	for (int ct = 0; ct < CROPTYPES; ct++)
		draw_spritesheet_frame(spritesheets[SPRT_CROPS], ct, 0, ct * TILESIZE, 0, TILESIZE, TILESIZE);

	// draw selected crop at mouse position
	if (selectedcrop != INVALID_CROPTYPE) {
		int mousex, mousey;
		get_mouse_coords(&mousex, &mousey);
		int x = mousex - (TILESIZE>>1);
		int y = mousey - (TILESIZE>>1);
		draw_spritesheet_frame(spritesheets[SPRT_CROPS], selectedcrop, 5, x, y, TILESIZE, TILESIZE);
	}
}

// all crop types are located in order on the first row
CropType get_menu_croptype_from_screen_coord(int screenx, int screeny)
{
	int row = screeny / TILESIZE;
	if (row != 0)
		return INVALID_CROPTYPE;
	
	int col = screenx / TILESIZE;
	return col < CROPTYPES ? col : INVALID_CROPTYPE;
}

// the tilemap starts from the second row of the screen
void get_tile_from_screen_coord(int screenx, int screeny, int* row, int* col)
{
	if (screeny < TILEMAP_OFFSETY) {
		*row = -1;
		*col = -1;
		return;
	}
	
	*row = (screeny - TILEMAP_OFFSETY) / TILESIZE;
	*col = screenx / TILESIZE;
}