#if !defined(CAMERA_H)
#include "Vector2.h"


typedef struct Camera {
    //should probably be setting apparent sizes or something here so we can zoom in and out
    v2 Center;
    v2 Size; //? I can do something with this, but I don't know what
} camera;

v2 WorldPointToScreenPoint(camera* Camera, int TilePixels, v2 ScreenCenter, v2 WorldPoint)
{
    v2 DiffCamera = Camera->Center - WorldPoint;
    v2 ScreenDiff = DiffCamera*TilePixels; //convert world difference to pixel difference
    v2 ScreenPoint = V2(ScreenCenter.X - ScreenDiff.X, ScreenCenter.Y - ScreenDiff.Y);
    return ScreenPoint;
}

#define CAMERA_H
#endif
