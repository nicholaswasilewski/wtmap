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
//map generation parameters
int MinHallLength = 0;
int MaxHallLength = 8;
    
int MinRoomDimensions = 5;
int MaxRoomDimensions = 8;

typedef struct{
    v2 ULPosition;
    v2 Dimensions;
} region;

//this is more like set region than make room
region MakeRegion(tileMap* TileMap, v2 ULPosition, int RoomWidth, int RoomHeight)
{
    region CreatedRoom = {0};
    
    int WidthOverflow = (ULPosition.X + RoomWidth) - (TileMap->Width-1);
    if (WidthOverflow > 0) RoomWidth -= WidthOverflow;
    int HeightOverflow = (ULPosition.Y + RoomHeight) - (TileMap->Height-1);
    if (HeightOverflow > 0) RoomHeight -= HeightOverflow;

    //check region

    
    for(int Y = ULPosition.Y; Y < ULPosition.Y+RoomHeight; Y++)
    {
        for(int X = ULPosition.X; X < ULPosition.X+RoomWidth; X++)
        {
            int TileType = SampleTile(TileMap, V2(X,Y));
            if (TileType != 0)
            {
                return CreatedRoom;
            }
        }
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

void MakeRoom(int RoomsToCreate, tileMap* TileMap, v2 ULPosition, v2 Dimensions)
{
    if(true)
        //if (RoomsToCreate != 0)
    {
        region NewRoom = MakeRegion(TileMap,
                                    ULPosition,
                                    Dimensions.X,
                                    Dimensions.Y);

        if (Dimensions.X != NewRoom.Dimensions.X ||
            Dimensions.Y != NewRoom.Dimensions.Y)
            return;
        //if (NewRoom.Dimensions.X == 0 ||
        //  NewRoom.Dimensions.Y == 0)
        //  return;
        
        //figure out where to go next;
        bool GoDirections[4] = {1,1,1,1};
        //bool GoDirections[4] = {RandBool(),RandBool(),RandBool(),RandBool()};
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
            v2 NextRoomUL = V2((int)(Hallway.ULPosition.X),
                               (int)(Hallway.ULPosition.Y - NextRoomDims.Y/2));
            MakeRoom(RoomsToCreate-1, TileMap, NextRoomUL, NextRoomDims);
        }
    }
    else
    {
        return;
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

    v2 Entrance = V2(Rand(0, MapWidth), Rand(0, MapHeight)); 

    
    for(int x = 0; x < MapWidth; x++)
    {
        int id = x%10==0?TileTypes.Water.ID:TileTypes.StoneWall.ID;
        Tiles[0+ x] = id;
        Tiles[((MAP_HEIGHT-1)*MAP_WIDTH)+x] = id;
    }

    for(int y = 0; y < MapHeight;y++)
    {
        Tiles[y*MapWidth + 0] = TileTypes.StoneWall.ID;
        Tiles[y*MapWidth + MapWidth-1] = TileTypes.StoneWall.ID;
    }

    int NumberOfRooms = 4;
    v2 NextRoomUL = V2(Rand(0, MapWidth),
                   Rand(0, MapHeight));
    v2 NextRoomDims = V2(Rand(MinRoomDimensions,
                              MaxRoomDimensions),
                         Rand(MinRoomDimensions,
                              MaxRoomDimensions));

    MakeRoom(NumberOfRooms, &TileMap, NextRoomUL, NextRoomDims);
}
