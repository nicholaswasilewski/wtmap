#include <stdlib.h>

inline int Rand(int Min, int Max)
{
    int Range = Max - Min;
    return (rand() % Range) + Min;
}

//TODO: create struct for tilearray, mapwidth, and mapheight together
inline void SetTile(int* Tiles,int MapWidth, v2 TilePosition, int TileID)
{
    Tiles[(int)(TilePosition.X + TilePosition.Y*MapWidth)] = TileID;
}

void MakeRoom(int* Tiles, int MapWidth, int MapHeight, v2 ulPosition, int RoomWidth, int RoomHeight)
{
    for(int y = ulPosition.Y; y < ulPosition.Y+RoomHeight && y < MapHeight; y++)
    {
        for(int x = ulPosition.X; x < ulPosition.X+RoomWidth && y < MapWidth; x++)
        {
            SetTile(Tiles, MapWidth, V2(x, y), TileTypes.StoneFloor.ID);
        }
    }
}

void ClearMap(int* Tiles)
{
    
}

void GenerateMap(int* Tiles, int MapWidth, int MapHeight, int Seed)
{
    for(int i = 0; i < MapWidth*MapHeight; i++)
    {
        Tiles[i] = 0;
    }
    
    srand(Seed);
    float RoomConnectivityFactor;
    
    int MinHallwayLength;
    int MaxHallwayLength;
    
    int MinRoomDimensions = 2;
    int MaxRoomDimensions = 20;

    v2 Entrance = V2(Rand(0, MapWidth-1), Rand(0, MapHeight-1)); 
    
    for(int x = 0; x < MapWidth; x++)
    {
        Tiles[0+ x] = TileTypes.StoneWall.ID;
        Tiles[((MAP_HEIGHT-1)*MAP_WIDTH)+x] = TileTypes.StoneWall.ID;
    }

    for(int y = 0; y < MapHeight;y++)
    {
        Tiles[y*MapWidth + 0] = TileTypes.StoneWall.ID;
        Tiles[y*MapWidth + MapWidth-1] = TileTypes.StoneWall.ID;
    }

    int NumberOfRooms = 5;
    for(int i = 0; i < NumberOfRooms; i++)
    {
        v2 RoomPos = V2(Rand(1, MapWidth-2),
                        Rand(1, MapHeight-2));
        MakeRoom(Tiles,
                 MapWidth,
                 MapHeight,
                 RoomPos,
                 Rand(MinRoomDimensions,
                      MaxRoomDimensions),
                 Rand(MinRoomDimensions,
                      MaxRoomDimensions));
    }

    SetTile(Tiles, MapWidth, Entrance, TileTypes.Entrance.ID);
}
