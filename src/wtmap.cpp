#include "wtmap.h"
#include "Color.h"
#include "Tile.h"
#include <Math.h>
#include <stdlib.h>

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


//world size = 1 tile = 1 meter (or something like that)
void DrawTile(game_screen_buffer* ScreenBuffer, camera* Camera, color TileColor, v2 TilePosition)
{
    int TilePixels = 16;

    v2 TileCenterPosition = TilePosition + V2(0.5f, 0.5f);
    v2 TileCenterScreenPoint = WorldPointToScreenPoint(Camera,
                                                       TilePixels,
                                                       V2(ScreenBuffer->Width/2, ScreenBuffer->Height/2),
                                                       TileCenterPosition);

    v2 ScreenTileSize = V2(0.5f*TilePixels, 0.5f*TilePixels);
    v2 Min = (TileCenterScreenPoint - ScreenTileSize) + V2(1,1);
    v2 Max = Min + V2(TilePixels-1, TilePixels-1);
    DrawRectangle(ScreenBuffer, Min, Max, TileColor.R, TileColor.G, TileColor.B);
}

void DrawMap(game_screen_buffer* ScreenBuffer, camera* Camera, int* Tiles)
{
    int* Row = Tiles; 
    for(int y = 0; y < MAP_HEIGHT; y++)
    {
        int* Tile = Row;
        for(int x = 0; x < MAP_WIDTH; x++)
        {   
            DrawTile(ScreenBuffer, Camera, TileData[*Tile].Color, V2(x, y));
            Tile += 1;
        }
        Row += MAP_WIDTH;
    }
}

void ClearScreenBuffer(game_screen_buffer *ScreenBuffer, color FillColor)
{
    DrawRectangle(ScreenBuffer,
                  V2(0,0),
                  V2(ScreenBuffer->Width, ScreenBuffer->Height),
                  FillColor.R,
                  FillColor.G,
                  FillColor.B);
}

void GenerateMap(int* Tiles, int MapWidth, int MapHeight)
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
}

void ProcessConsoleInput(game_input* Input, game_memory* Memory, game_screen_buffer *ScreenBuffer, char* Words)
{
    int i = atoi(Words);
    game_state* State = (game_state *)Memory->PermanentStorage;
    State->CameraMoveSpeed = (float)i;
}

void UpdateAndRender(game_input* Input, game_memory *Memory, game_screen_buffer *ScreenBuffer)
{
    game_state *GameState = (game_state *)Memory->PermanentStorage;
    int *Tiles = GameState->Tiles;
    if (!Memory->IsInitialized)
    {
        GenerateMap(Tiles, MAP_WIDTH, MAP_HEIGHT);
        Memory->IsInitialized = true;
    }

    float CameraMoveSpeed = GameState->CameraMoveSpeed*Input->dt;
    v2 CameraMove = V2(0,0);
    if (Input->Keyboard.MoveUp.Down)
    {
        CameraMove.Y -= 1;
    }
    if (Input->Keyboard.MoveDown.Down)
    {
        CameraMove.Y += 1;
    }
    if (Input->Keyboard.MoveLeft.Down)
    {
        CameraMove.X -= 1;
    }
    if (Input->Keyboard.MoveRight.Down)
    {
        CameraMove.X += 1;
    }
    GameState->Camera.Center = GameState->Camera.Center + (CameraMove*CameraMoveSpeed);

    color FillColor = Black;
    ClearScreenBuffer(ScreenBuffer, FillColor);
    DrawMap(ScreenBuffer, &GameState->Camera, Tiles);
}
