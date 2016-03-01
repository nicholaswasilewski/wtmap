#include <stdlib.h>
#include <stdio.h>

typedef struct {
    int MinHallLength;
    int MaxHallLength;

    int MinRoomDimensions;
    int MaxRoomDimensions;

    int EntitiesToPlace;
    entity* Entities;
} map_gen_parameters;

typedef struct {
    int RoomsCreated;
    v2 Exit;
} map_gen_results;

typedef struct{
    v2 ULPosition;
    v2 Dimensions;
} region;

inline int Rand(int Min, int MaxExclusive)
{
    int Range = (MaxExclusive-1) - Min;
    if (Range == 0) Range = 1;
    return (rand() % Range) + Min;
}

inline int Rand(int MaxExclusive)
{
   return Rand(0, MaxExclusive);
}

inline bool RandBool()
{
    return rand() % 2;
}

bool SetTile(tileMap* TileMap, v2 TilePosition, int TileID)
{
    //Assert(TilePosition.X < TileMap->Width);
    //Assert(TilePosition.Y < TileMap->Height);
    if (TilePosition.X < TileMap->Width && TilePosition.Y < TileMap->Height
        && TilePosition.X >= 0 && TilePosition.Y >= 0)
    {
        TileMap->Tiles[(int)(TilePosition.X + (TilePosition.Y*TileMap->Width))] = TileID;
        return true;
    }
    else
    {
        return false;
    }   
}

void ClearMap(tileMap* TileMap)
{
    for(int y = 0; y < TileMap->Height; y++)
    {
        for(int x = 0; x < TileMap->Width; x++)
        {
            SetTile(TileMap, V2(x, y), 0);
        }
    }
}

//returns -1 when tileposition is outside of map
int SampleTile(tileMap* TileMap, v2 TilePosition)
{
    if (TilePosition.X < 0 || TilePosition.X >= TileMap->Width-1 ||
        TilePosition.Y < 0 || TilePosition.Y >= TileMap->Height-1 )
    {
        return -1;
    }
    else
    {
        return TileMap->Tiles[(int)(TilePosition.X + TilePosition.Y*TileMap->Width)];
    }
}

inline v2 GetCenterOfRegionInt(region FindMyCenter)
{
    return FindMyCenter.ULPosition + V2((int)FindMyCenter.Dimensions.X/2,
                                        (int)FindMyCenter.Dimensions.Y/2);
}

int CheckRegion(tileMap* TileMap, v2 ULPosition, int RoomWidth, int RoomHeight)
{
    int WidthOverflow = (ULPosition.X + RoomWidth) - (TileMap->Width-1);
    if (WidthOverflow > 0) RoomWidth -= WidthOverflow;
    int HeightOverflow = (ULPosition.Y + RoomHeight) - (TileMap->Height-1);
    if (HeightOverflow > 0) RoomHeight -= HeightOverflow;
    for(int Y = ULPosition.Y; Y < ULPosition.Y+RoomHeight; Y++)
    {
        for(int X = ULPosition.X; X < ULPosition.X+RoomWidth; X++)
        {
            int TileType = SampleTile(TileMap, V2(X,Y));
            if (TileType != 0)
            {
                return false;
            }
        }
    }
    return true;
}

//this is more like set region than make room
region MakeRegion(tileMap* TileMap, v2 ULPosition, int RoomWidth, int RoomHeight)
{
    region CreatedRoom = {0};
    
    int WidthOverflow = (ULPosition.X + RoomWidth) - (TileMap->Width-1);
    if (WidthOverflow > 0) RoomWidth -= WidthOverflow;
    int HeightOverflow = (ULPosition.Y + RoomHeight) - (TileMap->Height-1);
    if (HeightOverflow > 0) RoomHeight -= HeightOverflow;

    if (!CheckRegion(TileMap, ULPosition, RoomWidth, RoomHeight))
    {
        return CreatedRoom;
    }
    
    for(int y = ULPosition.Y; y < ULPosition.Y+RoomHeight && y < TileMap->Height; y++)
    {
        for(int x = ULPosition.X; x < ULPosition.X+RoomWidth && y < TileMap->Width; x++)
        {
            SetTile(TileMap, V2(x, y), TileTypes.StoneFloor.ID);
        }
    }
    CreatedRoom.ULPosition = ULPosition;
    CreatedRoom.Dimensions = V2(RoomWidth, RoomHeight);
    return CreatedRoom;
}

region LastRoomGenerated;
//TODO: Make rooms to create do something
//TODO: make this a breadth first generation instead of depth first
void MakeRoom(int RoomsToCreate,
              map_gen_parameters* MapGenParams,
              tileMap* TileMap,
              v2 ULPosition,
              v2 Dimensions,
              map_gen_results* Results)
{
    int MinRoomDimensions = MapGenParams->MinRoomDimensions;
    int MaxRoomDimensions = MapGenParams->MaxRoomDimensions;
    int MinHallLength = MapGenParams->MinHallLength;
    int MaxHallLength = MapGenParams->MaxHallLength;
    
    region NewRoom = MakeRegion(TileMap,
                                ULPosition,
                                Dimensions.X,
                                Dimensions.Y);
    if (Dimensions.X != NewRoom.Dimensions.X ||
        Dimensions.Y != NewRoom.Dimensions.Y)
        return;
    LastRoomGenerated = NewRoom;
    Results->RoomsCreated += 1;

    //equal chance to put the exit in any room! Unless we only make one room...
    if (Results->RoomsCreated-1 >= 0)
    {
        if (Rand(Results->RoomsCreated-1) == 0)
        {
            Results->Exit = GetCenterOfRegionInt(NewRoom);
        }
    }
    
    for(int i = 0; i < ENTITY_COUNT; i++)
    {
        if (Rand(Results->RoomsCreated) == 0)
        {
            v2 CenterOfRoom = GetCenterOfRegionInt(NewRoom);
            MapGenParams->Entities[i].Position = CenterOfRoom;
        }
    }
    
    //this is delicate work
    //random without replacement (while stubbornly refusing to use stl vector)
    //the slightest misstep can bring this all crashing down
#define NUM_DIRECTIONS 4 //maybe one day we'd do hex tiles? or multiple exits per region...
    int DirectionsLeft = NUM_DIRECTIONS; //4 directions
    int DirectionList[NUM_DIRECTIONS];
    //fill array
    //creates an ordered array of numbers from 1 to NUM_DIRECTIONS
    for(int i = 0, num = 1; i < NUM_DIRECTIONS; i++, num++)
    {
        DirectionList[i] = num;
    }
    
    int DirectionsOrderIndex = 0;
    int DirectionsOrder[NUM_DIRECTIONS];
    //reserve zero for not going that direction, do that check after
    while(DirectionsLeft > 0) {
        //select a direction from the list
        int SelectedDirectionIndex = Rand(0, DirectionsLeft);
        
        //place the selected direction into DirectionsOrder
        DirectionsOrder[DirectionsOrderIndex] = DirectionList[SelectedDirectionIndex];
        
        //Replace the direction we just used with the last direction in the list of directions we have yet to use 
        DirectionList[SelectedDirectionIndex] = DirectionList[DirectionsLeft-1];

        //decrement the number of directions we have left to use
        DirectionsLeft -= 1;
        //increment to the place to put the next direction we select
        DirectionsOrderIndex += 1;
    }
    //the end result of all that should be a list with 1, 2, 3, and 4 in some order
    //end of delicate work

    for(int i = 0; i < NUM_DIRECTIONS; i++)
    {
        int Direction = DirectionsOrder[i];
        switch(Direction)
        {
            case 1: //u
            {
                v2 DoorPosition = NewRoom.ULPosition + V2((int)NewRoom.Dimensions.X/2,
                                                               0);
                int HallLength = Rand(MinHallLength, MaxHallLength);
                region Hallway = MakeRegion(TileMap,
                                            DoorPosition - V2(0,HallLength),
                                            1,
                                            HallLength);
                v2 NextRoomDims = V2(Rand(MinRoomDimensions,
                                          MaxRoomDimensions),
                                     Rand(MinRoomDimensions,
                                          MaxRoomDimensions));
                v2 NextRoomUL = V2((int)(Hallway.ULPosition.X - NextRoomDims.X/2),
                                   (int)(Hallway.ULPosition.Y - NextRoomDims.Y));
                
                MakeRoom(RoomsToCreate-1, MapGenParams, TileMap, NextRoomUL, NextRoomDims, Results);
            } break;
            case 2: //d
            {
                v2 DoorPosition = NewRoom.ULPosition + V2((int)NewRoom.Dimensions.X/2,
                                                          NewRoom.Dimensions.Y-1);
                int HallLength = Rand(MinHallLength, MaxHallLength);
                region Hallway = MakeRegion(TileMap,
                                            DoorPosition + V2(0,1),
                                            1,
                                            HallLength);
                v2 NextRoomDims = V2(Rand(MinRoomDimensions,
                                          MaxRoomDimensions),
                                     Rand(MinRoomDimensions,
                                          MaxRoomDimensions));
                v2 NextRoomUL = V2((int)(Hallway.ULPosition.X - NextRoomDims.X/2),
                                   (int)(Hallway.ULPosition.Y+Hallway.Dimensions.Y));
                MakeRoom(RoomsToCreate-1, MapGenParams, TileMap, NextRoomUL, NextRoomDims, Results);
            } break;
            case 3://l 
            {
                v2 DoorPosition;
                DoorPosition = NewRoom.ULPosition + V2(0,
                                                       (int)NewRoom.Dimensions.Y/2);
                int HallLength = Rand(MinHallLength, MaxHallLength);
                region Hallway = MakeRegion(TileMap,
                                            DoorPosition - V2(HallLength, 0),
                                            HallLength,
                                            1);
                v2 NextRoomDims = V2(Rand(MinRoomDimensions,
                                          MaxRoomDimensions),
                                     Rand(MinRoomDimensions,
                                          MaxRoomDimensions));
                v2 NextRoomUL = V2((int)(Hallway.ULPosition.X - NextRoomDims.X),
                                   (int)(Hallway.ULPosition.Y - NextRoomDims.Y/2));
                MakeRoom(RoomsToCreate-1, MapGenParams, TileMap, NextRoomUL, NextRoomDims, Results);
            }
            case 4://r
            {
                v2 DoorPosition;
                DoorPosition = NewRoom.ULPosition + V2(NewRoom.Dimensions.X-1,
                                                       (int)NewRoom.Dimensions.Y/2);
                int HallLength = Rand(0, MaxHallLength);
                region Hallway = MakeRegion(TileMap,
                                            DoorPosition + V2(1, 0),
                                            HallLength,
                                            1);
                v2 NextRoomDims = V2(Rand(MinRoomDimensions,
                                          MaxRoomDimensions),
                                     Rand(MinRoomDimensions,
                                          MaxRoomDimensions));
                v2 NextRoomUL = V2((int)(Hallway.ULPosition.X+HallLength),
                                   (int)(Hallway.ULPosition.Y - NextRoomDims.Y/2));
                MakeRoom(RoomsToCreate-1, MapGenParams, TileMap, NextRoomUL, NextRoomDims, Results);
            } break;
            default:
            {
            }   break;
        }
    }
}

void GenerateMap(game_state *GameState, map_gen_parameters* Params,int* Tiles, int MapWidth, int MapHeight, int Seed)
{
    tileMap TileMap = {
        Tiles,
        MapWidth,
        MapHeight
    };

    map_gen_results Results = {0};

    //kill all entities;
    for(int i = 0; i < ENTITY_COUNT; i++)
    {
        int EntityDirectionCode = Rand(4);
        v2 EntityDirection;
        if (EntityDirectionCode == 0)
            EntityDirection = V2(0,-1);
        if (EntityDirectionCode == 1)
            EntityDirection = V2(0,1);
        if (EntityDirectionCode == 2)
            EntityDirection = V2(-1,0);
        if (EntityDirectionCode == 3)
            EntityDirection = V2(1,0);
                
        GameState->Entities[i].Direction = EntityDirection;
        GameState->Entities[i].Alive = true;
    }
    
    ClearMap(&TileMap);

    srand(Seed);
    printf("Seed: %d\n", Seed);


    for(int x = 0; x < MapWidth;x++)
    {
        SetTile(&TileMap, V2(x, 0),  TileTypes.StoneWall.ID);
        SetTile(&TileMap, V2(x, MapHeight-1), TileTypes.StoneWall.ID);
    }
    
    for(int y = 0; y < MapHeight;y++)
    {
        Tiles[y*MapWidth + 0] = TileTypes.StoneWall.ID;
        Tiles[y*MapWidth + MapWidth-1] = TileTypes.StoneWall.ID;
    }

    int NumberOfRooms = 4;
    v2 NextRoomDims = V2(Rand(Params->MinRoomDimensions,
                              Params->MaxRoomDimensions),
                         Rand(Params->MinRoomDimensions,
                              Params->MaxRoomDimensions));
    v2 NextRoomUL = V2(Rand(0, MapWidth-NextRoomDims.X),
                       Rand(0, MapHeight-NextRoomDims.Y));
    v2 Entrance = NextRoomUL + V2((int)NextRoomDims.X/2, (int)NextRoomDims.Y/2);
    
    MakeRoom(NumberOfRooms, Params, &TileMap, NextRoomUL, NextRoomDims, &Results);
    SetTile(&TileMap, Entrance, TileTypes.Entrance.ID);
    GameState->Camera.Center = Entrance;
    
    v2 Exit = Results.Exit;
    SetTile(&TileMap, Exit, TileTypes.Exit.ID);

    printf("Rooms Created: %d\n", Results.RoomsCreated);
}
