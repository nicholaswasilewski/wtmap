#include "wtmap.h"


void DrawRectangle(game_screen_buffer* ScreenBuffer,
                   v2 Min,
                   v2 Max,
                   float R,
                   float G,
                   float B)
{
}

void UpdateAndRender(game_input* Input, game_memory *Memory, game_screen_buffer *Buffer)
{

    game_state *GameState = (game_state *)Memory->PermanentStorage;
    if (!Memory->IsInitialized)
    {
        //generate map
        Memory->IsInitialized = true;
    }

    //draw map
    uint8 *Row = (uint8 *)Buffer->Memory;
    for (int Y = 0;
         Y < Buffer->Height;
         Y++)
    {
        uint32 *Pixel = (uint32 *)Row;
        for (int X = 0;
             X < Buffer->Width;
            X++)
        {
            *(uint32 *)Pixel++ = 0x000000FF;
        }
        Row += Buffer->Pitch;
    }
}
