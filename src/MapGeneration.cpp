#include <stdlib.h>


inline int Rand(int Min, int Max)
{
    int Range = Max - Min;
    return (rand() % Range) + Min;
}

inline void SetTile(tileMap* TileMap, v2 TilePosition, int TileID)
{
    Assert(TilePosition.X < TileMap->Width);
    Assert(TilePosition.Y < TileMap->Height);
    TileMap->Tiles[(int)(TilePosition.X + TilePosition.Y*TileMap->Width)] = TileID;
}

//returns -1 when tileposition is outside of map
int SampleTile(tileMap* TileMap, v2 TilePosition)
{
    if (TilePosition.X < TileMap->Width || TilePosition.Y < TileMap->Height)
    {
        return -1;
    }
    else
    {
        return TileMap->Tiles[(int)(TilePosition.X + TilePosition.Y*TileMap->Width)];
    }
}

void MakeRoom(tileMap* TileMap, v2 ulPosition, int RoomWidth, int RoomHeight)
{
    int WidthOverflow = (ulPosition.X + RoomWidth) - (TileMap->Width-1);
    if (WidthOverflow > 0) RoomWidth -= WidthOverflow;
    int HeightOverflow = (ulPosition.Y + RoomHeight) - (TileMap->Height-1);
    if (HeightOverflow > 0) RoomHeight -= HeightOverflow;
    
    
    for(int y = ulPosition.Y; y < ulPosition.Y+RoomHeight && y < TileMap->Height; y++)
    {
        for(int x = ulPosition.X; x < ulPosition.X+RoomWidth && y < TileMap->Width; x++)
        {
            SetTile(TileMap, V2(x, y), TileTypes.StoneFloor.ID);
        }
    }
}

void ClearMap(tileMap* TileMap)
{
    for(int i = 0; i < TileMap->Width*TileMap->Height; i++)
    {
        TileMap->Tiles[i] = 0;
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
    
    int MinRoomDimensions = 3;
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
        MakeRoom(&TileMap,
                 RoomPos,
                 Rand(MinRoomDimensions,
                      MaxRoomDimensions),
                 Rand(MinRoomDimensions,
                      MaxRoomDimensions));
    }

    
}
