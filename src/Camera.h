#if !defined(CAMERA_H)
#include "Vector2.h"


typedef struct Camera {
    //should probably be setting apparent sizes or something here so we can zoom in and out
    v2 Center;
    //world units to pixel
    float WorldUnitsToPixels = 16;
} camera;

v2 WorldPointToScreenPoint(camera* Camera, v2 ScreenCenter, v2 WorldPoint)
{
    v2 DiffCamera = Camera->Center - WorldPoint;
    v2 ScreenDiff = DiffCamera*Camera->WorldUnitsToPixels; //convert world difference to pixel difference
    v2 ScreenPoint = V2(ScreenCenter.X - ScreenDiff.X, ScreenCenter.Y - ScreenDiff.Y);
    return ScreenPoint;
}

#define CAMERA_H
#endif
