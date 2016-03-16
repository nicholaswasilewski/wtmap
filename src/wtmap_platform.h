#ifndef WTMAP_PLATFORM_H
#define WTMAP_PLATFORM_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdint.h>
#include <stddef.h>

typedef int8_t int8;
typedef uint8_t uint8;

typedef int32_t int32;
typedef uint32_t uint32;

typedef int64_t int64;
typedef uint64_t uint64;

#define Kilobytes(Val) ((Val)*1024LL)
#define Megabytes(Val) (Kilobytes(Val)*1024LL)
#define Gigabytes(Val) (Megabytes(Val)*1024LL)

#define Assert(Expression) \
if(!(Expression)) (*(int*)0 = 0)

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


void UpdateAndRender(game_input* Input, game_input* LastInput, game_memory *Memory,game_screen_buffer *ScreenBuffer);

    
#ifdef __cplusplus
}
#endif

#endif
