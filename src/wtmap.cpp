#include "wtmap.h"
#include "Color.h"
#include "Tile.h"
#include <Math.h>

void DrawRectangle(game_screen_buffer* ScreenBuffer,
                   v2 Min,
                   v2 Max,
                   int R,
                   int G,
                   int B)
{
    int32 MinX = (int32)roundf(Min.X);
    int32 MinY = (int32)roundf(Min.Y);
    int32 MaxX = (int32)roundf(Max.X);
    int32 MaxY = (int32)roundf(Max.Y);
    
    if (MinX < 0)
        MinX = 0;
    if (MinY < 0)
        MinY = 0;
    if (MaxX > ScreenBuffer->Width)
    {
        MaxX = ScreenBuffer->Width;
    }
    if (MaxY > ScreenBuffer->Height)
    {
        MaxY = ScreenBuffer->Height;
    }

    uint8 *Row = ((uint8*)ScreenBuffer->Memory +
                  MinX*ScreenBuffer->BytesPerPixel +
                  MinY*ScreenBuffer->Pitch);

    uint32 Red = R << 16;
    uint32 Green = G << 8;
    uint32 Blue = B << 0;
    uint32 Color = Red|Green|Blue;

    for(int y = MinY;
        y < MaxY;
        y++)
    {
        uint32 *Pixel = (uint32 *)Row;

        for(int x = MinX;
            x < MaxX;
            x++)
        {
            *(uint32 *)Pixel++ = Color;
        }
        Row += ScreenBuffer->Pitch;
    }
}

void DrawTile(game_screen_buffer* ScreenBuffer, color TileColor, v2 TilePosition)
{
    int TilePixels = 16;
    v2 Min = V2((TilePosition.X * TilePixels)+1, (TilePosition.Y * TilePixels)+1);
    v2 Max = Min + V2(TilePixels-1, TilePixels-1);
    DrawRectangle(ScreenBuffer, Min, Max, TileColor.R, TileColor.G, TileColor.B);
}

void DrawMap(game_screen_buffer* ScreenBuffer, v2 CameraPosition, int* Tiles)
{
    int* Row = Tiles; 
    for(int y = 0; y < MAP_HEIGHT; y++)
    {
        int* Tile = Row;
        for(int x = 0; x < MAP_WIDTH; x++)
        {   
            DrawTile(ScreenBuffer, TileData[*Tile].Color, V2(x, y));
            Tile += 1;
        }
        Row += MAP_WIDTH;
    }
}

void UpdateAndRender(game_input* Input, game_memory *Memory, game_screen_buffer *ScreenBuffer)
{
    game_state *GameState = (game_state *)Memory->PermanentStorage;
    int *Tiles = GameState->Tiles;
    if (!Memory->IsInitialized)
    {
        //generate map
        for(int y = 1; y < MAP_HEIGHT-1; y++)
        {
            for(int x = 1; x < MAP_WIDTH-1; x++)
            {
                Tiles[y*MAP_WIDTH + x] = StoneFloor.ID;
            }
        }
        for(int x = 0; x < MAP_WIDTH; x++)
        {
            Tiles[x] = Water.ID;
        }
        Memory->IsInitialized = true;
    }

    DrawMap(ScreenBuffer, GameState->CameraPosition, Tiles);
}
