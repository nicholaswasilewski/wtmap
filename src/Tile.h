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
        tileData *Values;
    };
} tilesData;

tileData Blank = {0, 0, 0, 0, 0};
tileData StoneFloor = {1, 127, 127, 127, 255};
tileData StoneWall = {2, 255, 255, 255, 255};
tileData Water = {3, 100, 100, 255, 255};

tileData TileData[] = {
    {0, {0, 0, 0, 0}},
    {1, { 127, 127, 127, 255 } },
    {2, { 255, 255, 255, 255 } },
    Water
};

#define TILE_H
#endif
