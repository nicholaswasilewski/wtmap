#if !defined(TILE_H)

#include "Color.h"

//there's a better way to construct this so it acts more like a java enum,
//but this is all I've got for now
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
            tileData Entrance;
            tileData Exit;
        };
        tileData Values[4];
    };
} tilesData;

tileData Blank = {0, 0, 0, 0, 0};
tileData StoneFloor = {1, 127, 127, 127, 255};
tileData StoneWall = {2, 64, 64, 64, 255};
tileData Water = {3, 100, 100, 255, 255};
tileData Entrance = {4, 0, 255, 0, 255};
tileData Exit = {5, 255, 0, 0, 255};

tilesData TileTypes = {
Blank,
StoneFloor,
StoneWall,
Water,
Entrance,
Exit
};


#define TILE_H
#endif
