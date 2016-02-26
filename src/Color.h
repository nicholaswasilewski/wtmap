#if !defined(COLOR_H)

#include "wtmap.h"

typedef struct Color {
    union
    {
        struct { uint8 R, G, B, A; };
        
    };
} color;

color Red = { 255, 0, 0, 255 };
color Green = { 0, 255, 0, 255};
color Blue = { 0, 0, 255, 255 };
color Black = { 0, 0, 0, 255 };
color White = { 255, 255, 255, 255 };

#define COLOR_H
#endif
