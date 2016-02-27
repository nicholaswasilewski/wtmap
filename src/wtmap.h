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

typedef struct game_screen_buffer
{
	void *Memory;
	int Width;
	int Height;
	int Pitch;
	int BytesPerPixel;
} game_screen_buffer;
#define MAP_WIDTH 100
#define MAP_HEIGHT 100
typedef struct game_state
{
    int FloorNumber;
    float CameraMoveSpeed;
    
    //I really should put this
    //somewhere else so I can do variably sized
    //maps, but for now...
    int Tiles[MAP_WIDTH*MAP_HEIGHT];
    //floor map, other state
    camera Camera;
    //player position
} game_state;

#define WTMAP__H
#endif
