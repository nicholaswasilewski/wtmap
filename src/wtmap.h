#if !defined(WTMAP__H)

#include "wtmap_platform.h"
#include "Vector2.h"
#include "Camera.h"

typedef struct memory_arena
{
    size_t Size;
    uint8 *Base;
    size_t Used;
} memory_arena;

#define PushSize(Arena, type) (type *)PushSize_(Arena, sizeof(type))
#define PushArray(Arena, Count, type) (type *)PushSize_(Arena, (Count)*sizeof(type))
void*
PushSize_(memory_arena *Arena, size_t Size)
{
    Assert((Arena->Used + Size) <= (Arena->Size));
    void *NewSpace = Arena->Base + Arena->Used;
    Arena->Used += Size;
    
    return NewSpace;
}

void InitArena(memory_arena *Arena, size_t Size, uint8 *Base)
{
    Arena->Size = Size;
    Arena->Base = Base;
    Arena->Used = 0;
}

typedef struct tileMap{
    int* Tiles;
    int Width;
    int Height;
} tileMap;

typedef struct entity{
    bool Alive;
    v2 Direction;
    v2 Position;
} entity;

//these should ABSOLUTELY not be hardcoded
#define ENTITY_COUNT 150
typedef struct game_state
{
    int FloorNumber;
    float CameraMoveSpeed;
    float TickCounter;

    memory_arena TilesArena;
    
    //I really should put this
    //somewhere else so I can do variably sized
    //maps, but for now...
    tileMap TileMap;
    int *Tiles;
    entity Entities[ENTITY_COUNT];
    //floor map, other state
    camera Camera;
    //player position
} game_state;


#define WTMAP__H
#endif
