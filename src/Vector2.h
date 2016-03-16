#if !defined(VECTOR2_H)
#define VECTOR2_H
typedef struct Vector2
{
    union
    {
        struct { float X, Y; };
        float E[2];
    };
    float operator[](int Index) { return E[Index]; }
}  v2;

inline Vector2
V2(float X, float Y)
{
    Vector2 Result;
    Result.X = X;
    Result.Y = Y;
    return Result;
}

Vector2
operator*(float A, Vector2 B)
{
    Vector2 Result;
    Result.X = A*B.X;
    Result.Y = A*B.Y;

    return Result;
}

inline Vector2
operator*(Vector2 B, float A)
{
    Vector2 Result;
    Result = A*B;
    return Result;
}

inline Vector2 &
operator*=(Vector2 &A, float B)
{
    A = A * B;
    return A;
}

inline Vector2
operator-(Vector2 A)
{
    Vector2 Result;
    Result.X = -A.X;
    Result.Y = -A.Y;

    return Result;
}

inline Vector2
operator+(Vector2 A, Vector2 B)
{
    Vector2 Result;
    Result.X = A.X + B.X;
    Result.Y = A.Y + B.Y;
    
    return Result;
}

inline Vector2 &
operator+=(Vector2 &A, Vector2 B)
{
    A = A + B;
    return A;
}

inline
Vector2 operator-(Vector2 A, Vector2 B)
{
    Vector2 Result;
    Result.X = A.X - B.X;
    Result.Y = A.Y - B.Y;
    
    return Result;
}

#endif
