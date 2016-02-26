#if !defined(CAMERA_H)
#include "Vector2.h"


struct Camera {
    //should probably be setting apparent sizes or something here so we can zoom in and out
    v2 Center;
    v2 Size; //? I can do something with this, but I don't know what
};

#define CAMERA_H
#endif
