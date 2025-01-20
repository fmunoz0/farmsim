#ifndef SPRITESHEET_H
#define SPRITESHEET_H

typedef struct Spritesheet Spritesheet;

Spritesheet* load_spritesheet(const char* filename, int framew, int frameh);
void free_spritesheet(Spritesheet* sheet);

void draw_spritesheet_frame(Spritesheet* sheet, int framerow, int framecol, int dstx, int dsty, int dstw, int dsth);

#endif