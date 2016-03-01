#if !defined(WTMAP__H)
#include <stdint.h>
#include "Vector2.h"
#include "Camera.h"

typedef __int8 int8;
typedef unsigned __int8 uint8;

typedef __int32 int32;
typedef unsigned __int32 uint32;

typedef __int64 int64;
typedef unsigned __int64 uint64;

#define Kilobytes(Val) ((Val)*1024LL)
#define Megabytes(Val) (Kilobytes(Val)*1024LL)
#define Gigabytes(Val) (Megabytes(Val)*1024LL)

#define Assert(Expression) \
    if(!(Expression)) (*(int*)0 = 0)

struct memory_arena
{
    size_t Size;
    uint8 *Base;
    size_t Used;
};

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

typedef struct game_button_state
{
    bool Down;
} game_button_state;

typedef struct game_controller
{
    union
    {
        game_button_state Buttons[8];
        struct
        {
            game_button_state MoveUp;
            game_button_state MoveDown;
            game_button_state MoveLeft;
            game_button_state MoveRight;
            game_button_state LeftShoulder;
            game_button_state RightShoulder;
            game_button_state ActionUp;
            game_button_state ActionDown;
            game_button_state ActionLeft;
            game_button_state ActionRight;
            game_button_state Back;
            game_button_state Start;
        };
    };
} game_controller;

typedef struct game_input
{
    float dt;
    game_controller Keyboard;
} game_input;

typedef struct game_memory
{
    bool IsInitialized;
    int64 PermanentStorageSize;
    void* PermanentStorage;

    int64 TransientStorageSize;
    void* TransientStorage;
} game_memory;

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

typedef struct game_screen_buffer
{
	void *Memory;
	int Width;
	int Height;
	int Pitch;
	int BytesPerPixel;
} game_screen_buffer;

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
