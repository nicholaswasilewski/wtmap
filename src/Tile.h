#if !defined(TILE_H)

#include "Color.h"

typedef struct tileData {
    int ID;
    color Color;
} tileData;

typedef struct tilesData {
    union {
        struct {
            tileData Blank;
            tileData StoneFloor;
            tileData StoneWall;
            tileData Water;
        };
        tileData Values[4];
    };
} tilesData;

tileData Blank = {0, 0, 0, 0, 0};
tileData StoneFloor = {1, 127, 127, 127, 255};
tileData StoneWall = {2, 64, 64, 64, 255};
tileData Water = {3, 100, 100, 255, 255};

tilesData TileTypes= {
    Blank = {0,0,0,0,0},
    StoneFloor = {1, 127, 127, 127, 255},
    StoneWall = StoneWall,
    Water = {3, 100, 100, 255, 255 }
};

#define TILE_H
#endif
