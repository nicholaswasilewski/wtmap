

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

    int y = 0;
    for(int x = 0; x < MAP_WIDTH; x++)
    {
        Tiles[y*MAP_WIDTH + x] = TileTypes.StoneWall.ID;
    }

    y = MAP_HEIGHT-1;
    for(int x = 0; x < MAP_WIDTH; x++)
    {
        Tiles[y*MAP_WIDTH + x] = TileTypes.StoneWall.ID;
    }
}
