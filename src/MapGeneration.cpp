#include <stdlib.h>
#include <stdio.h>

inline int Rand(int Min, int MaxExclusive)
{
    int Range = (MaxExclusive-1) - Min;
    if (Range == 0) Range = 1;
    return (rand() % Range) + Min;
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

typedef struct{
    v2 ULPosition;
    v2 Dimensions;
} region;

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

//map generation parameters
int MinHallLength = 0;
int MaxHallLength = 8;
    
int MinRoomDimensions = 5;
int MaxRoomDimensions = 8;

region LastRoomGenerated;
void MakeRoom(int RoomsToCreate, tileMap* TileMap, v2 ULPosition, v2 Dimensions)
{
    region NewRoom = MakeRegion(TileMap,
                                ULPosition,
                                Dimensions.X,
                                Dimensions.Y);
    if (Dimensions.X != NewRoom.Dimensions.X ||
        Dimensions.Y != NewRoom.Dimensions.Y)
        return;
    LastRoomGenerated = NewRoom;
    
    bool GoDirections[4] = {1,1,1,1};//I'm sure this'll get optimized out by the compiler...
    if (GoDirections[0]) //u
    {
        v2 DoorPosition = NewRoom.ULPosition + V2((int)NewRoom.Dimensions.X/2,
                                                  0);
        
        //making the hall
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
        
        MakeRoom(RoomsToCreate-1, TileMap, NextRoomUL, NextRoomDims);
    }
    if (GoDirections[1]) //d
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
        MakeRoom(RoomsToCreate-1, TileMap, NextRoomUL, NextRoomDims);
    }
    if (GoDirections[2]) //l 
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
        MakeRoom(RoomsToCreate-1, TileMap, NextRoomUL, NextRoomDims);
    }
    if (GoDirections[3]) //r
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
        MakeRoom(RoomsToCreate-1, TileMap, NextRoomUL, NextRoomDims);
    }
}

void GenerateMap(int* Tiles, int MapWidth, int MapHeight, int Seed)
{
    tileMap TileMap = {
        Tiles,
        MapWidth,
        MapHeight
    };

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
    v2 NextRoomDims = V2(Rand(MinRoomDimensions,
                              MaxRoomDimensions),
                         Rand(MinRoomDimensions,
                              MaxRoomDimensions));
    v2 NextRoomUL = V2(Rand(0, MapWidth-NextRoomDims.X),
                       Rand(0, MapHeight-NextRoomDims.Y));
    v2 Entrance = NextRoomUL + V2((int)NextRoomDims.X/2, (int)NextRoomDims.Y/2);

    MakeRoom(NumberOfRooms, &TileMap, NextRoomUL, NextRoomDims);
    SetTile(&TileMap, Entrance, TileTypes.Entrance.ID);
    v2 Exit = LastRoomGenerated.ULPosition + V2((int)LastRoomGenerated.Dimensions.X/2,
                                                (int)LastRoomGenerated.Dimensions.Y/2);
    SetTile(&TileMap, Exit, TileTypes.Exit.ID);
}
