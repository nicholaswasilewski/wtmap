#include "wtmap.h"
#include "Color.h"
#include "Tile.h"
#include "MapGeneration.cpp"
#include <Math.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdio.h>

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
    v2 TileCenterPosition = TilePosition + V2(0.5f, 0.5f);
    v2 TileCenterScreenPoint = WorldPointToScreenPoint(Camera,
                                                       V2(ScreenBuffer->Width/2, ScreenBuffer->Height/2),
                                                       TileCenterPosition);

    float TilePixels = Camera->WorldUnitsToPixels;
    v2 ScreenTileSize = V2(0.5f*TilePixels, 0.5f*TilePixels);
    v2 Min = TileCenterScreenPoint - ScreenTileSize + V2(1, 1);
    v2 Max = TileCenterScreenPoint + ScreenTileSize - V2(1, 1);
    DrawRectangle(ScreenBuffer, Min, Max, TileColor.R, TileColor.G, TileColor.B);
}

void DrawMap(game_screen_buffer* ScreenBuffer, camera* Camera, tileMap* TileMap)
{
    int* Row = TileMap->Tiles; 
    for(int y = 0; y < TileMap->Height; y++)
    {
        int* Tile = Row;
        for(int x = 0; x < TileMap->Width; x++)
        {   
            DrawTile(ScreenBuffer, Camera, TileTypes.Values[*Tile].Color, V2(x, y));
            Tile += 1;
        }
        Row += TileMap->Width;
    }
}

void DrawEntities(game_state* GameState, game_screen_buffer* ScreenBuffer, camera* Camera)
{
    for(int i = 0; i < ENTITY_COUNT; i++)
    {
        entity* Entity = &GameState->Entities[i];
        if (Entity->Alive)
        {
            DrawTile(ScreenBuffer, Camera, TileTypes.Values[TileTypes.GenericMonster.ID].Color, Entity->Position);
        }
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

void ProcessConsoleInput(game_input* Input, game_memory* Memory, game_screen_buffer *ScreenBuffer, char* Words)
{
	if (strlen(Words) > 0)
    {
        char* commandString = strtok(Words, " ");
        if (strcmp(commandString, "genmap") == 0)
        {
            char* paramString = strtok(0, " ");
            int Seed = 0;
            if (paramString)
            {
                Seed = atoi(paramString);
            }
            else
            {
                Seed = (int)time(0);
            }
            
            game_state *GameState = (game_state *)Memory->PermanentStorage;
            
            int MinHallLength = 0;
            int MaxHallLength = 8;
        
            int MinRoomDimensions = 4;
            int MaxRoomDimensions = 8;
    
            int EntitiesToPlace = 20;
    
            map_gen_parameters Params = {
                MinHallLength,
                MaxHallLength,
                MinRoomDimensions,
                MaxRoomDimensions,
                EntitiesToPlace,
                GameState->Entities,
            };
            GenerateMap(GameState, &Params, Seed);
        }
        else if (strcmp(commandString, "camspeed") == 0)
        {
            char* paramString = strtok(0, " ");
            if (paramString)
            {
                int i = atoi(paramString);
                game_state* State = (game_state *)Memory->PermanentStorage;
                State->CameraMoveSpeed = (float)i;
            }
        }
        else if (strcmp(commandString, "camzoom") == 0)
        {
            char* paramString = strtok(0, " ");
            if (paramString)
            {
                int i = atoi(paramString);
                game_state* State = (game_state *)Memory->PermanentStorage;
                if (i >= 4)
                    State->Camera.WorldUnitsToPixels = i;
            }
        }
    }
}

void UpdateEntity(game_state* GameState, tileMap* TileMap, entity* Entity)
{
    v2 NextPosition = Entity->Position + Entity->Direction;
    if(SampleTile(TileMap,NextPosition) != TileTypes.StoneFloor.ID || Rand(10) == 0)
    {
        switch(Rand(3))
        {
            case 0:
                Entity->Direction = -1.0 * Entity->Direction;
                break;
            case 1:
                Entity->Direction = V2(Entity->Direction.Y, -Entity->Direction.X);
                break;
            default:
                Entity->Direction = V2(- Entity->Direction.Y, Entity->Direction.X);
        }  
    }
    else
    {
        Entity->Position += Entity->Direction;
    }
}


void UpdateEntities(game_state* GameState, tileMap* TileMap)
{
    for(int i = 0; i < ENTITY_COUNT; i++)
    {
        if (GameState->Entities[i].Alive)
        {
            UpdateEntity(GameState, TileMap, &GameState->Entities[i]);
        }
    }
}

void UpdateAndRender(game_input* Input, game_input* LastInput, game_memory *Memory, game_screen_buffer *ScreenBuffer)
{
    game_state *GameState = (game_state *)Memory->PermanentStorage;
    if (!Memory->IsInitialized)
    {
        GameState->Camera.WorldUnitsToPixels = 16;
        GameState->CameraMoveSpeed = 10.0f;
        
        int MinHallLength = 0;
        int MaxHallLength = 8;
        
        int MinRoomDimensions = 4;
        int MaxRoomDimensions = 8;
    
        int EntitiesToPlace = ENTITY_COUNT;
    
        int MapWidth = 200;
        int MapHeight = 200;

        memory_arena* TilesArena = &GameState->TilesArena;
        InitArena(TilesArena, MapWidth*MapHeight*sizeof(int),
                  (uint8 *)Memory->PermanentStorage+sizeof(game_state));
        
        GameState->TileMap = {
            PushArray(TilesArena, MapWidth*MapHeight, int),
            MapWidth,
            MapHeight
        };
    
        map_gen_parameters MapGenParams = {
            MinHallLength,
            MaxHallLength,
            MinRoomDimensions,
            MaxRoomDimensions,
            EntitiesToPlace,
            GameState->Entities,
        };
        
        GenerateMap(GameState,
                    &MapGenParams,
                    (int)time(0));
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

    if (Input->Keyboard.LeftShoulder.Down)
    {
        GameState->Camera.WorldUnitsToPixels -= 0.25f;
        if (GameState->Camera.WorldUnitsToPixels < 4.0f)
        {
            GameState->Camera.WorldUnitsToPixels = 4.0f;
        }
    }
    if (Input->Keyboard.RightShoulder.Down)
    {
        GameState->Camera.WorldUnitsToPixels += .25f;
        if (GameState->Camera.WorldUnitsToPixels > 64.0f)
        {
            GameState->Camera.WorldUnitsToPixels = 64.0f;
        }
    }

    if (Input->Keyboard.Start.Down && !LastInput->Keyboard.Start.Down)
    {
        
        int MinHallLength = 0;
        int MaxHallLength = 8;
        
        int MinRoomDimensions = 4;
        int MaxRoomDimensions = 8;
    
        int EntitiesToPlace = ENTITY_COUNT;
    
        map_gen_parameters MapGenParams = {
            MinHallLength,
            MaxHallLength,
            MinRoomDimensions,
            MaxRoomDimensions,
            EntitiesToPlace,
            GameState->Entities,
        };

        GenerateMap(GameState, &MapGenParams, (int)time(0));
    }
    
    GameState->Camera.Center = GameState->Camera.Center + (CameraMove*CameraMoveSpeed);

    
    GameState->TickCounter += Input->dt;
    if (GameState->TickCounter >= 0.2f)
    {
        UpdateEntities(GameState, &GameState->TileMap);
        GameState->TickCounter -= 0.2f;
    }

    
    color FillColor = Black;
    ClearScreenBuffer(ScreenBuffer, FillColor);
    DrawMap(ScreenBuffer, &GameState->Camera, &GameState->TileMap);
    DrawEntities(GameState, ScreenBuffer, &GameState->Camera);
}
