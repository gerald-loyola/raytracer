#if !defined(MATH_HELPER_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Janastin Gerald $
   $Notice: (C) Copyright 2018 by Janastin Gerald. All Rights Reserved. $
   ======================================================================== */

#define MATH_HELPER_H

#include <math.h> //sqrt, ceil, cos, sin

#define PI 3.1415926535897932384626433832795f
#define DegreeToRadiant(degree)  ((degree)  * (PI/180.0f))
#define RadiantToDegree(radiant) ((radiant) * (180.0f/PI))

struct v2;
struct v3;
struct v4;

inline f32
Cos(f32 A)
{
    f32 Result = cos(A);
    return(Result);
}

inline f32
Sin(f32 A)
{
    f32 Result = sin(A);
    return(Result);
}

inline f32
Ceil(f32 A)
{
    f32 Result =  ceil(A);
    return(Result);
}

inline void
Swap(f32 &A, f32 &B)
{
    f32 Temp = A;
    A = B;
    B = Temp;
}

inline f32
Pow(f32 A, f32 B)
{
    f32 Result = pow(A, B);
    return(Result); 
}

inline f32
Square(f32 A)
{
    f32 Result = A * A;
    return(Result);
}

inline f32
SquareRoot(f32 A)
{
    f32 Result = sqrt(A);
    return(Result);
}

inline f32
Min(f32 A, f32 B)
{
    f32 Result = A < B ? A : B;
    return(Result);
}

inline f32
Max(f32 A, f32 B)
{
    f32 Result = A > B ? A : B;
    return(Result);
}

inline f32
Lerp(f32 A, f32 t, f32 B)
{
    f32 Result = (1.0f - t) * A + (t * B);
    return(Result);
}

inline f32
RandomUnilateral()
{
    f32 Result = (f32)rand()/(f32)RAND_MAX;
    return(Result);
}

inline f32
RandomBilateral()
{
    f32 Result = -1.0f + 2.0f * RandomUnilateral();
    return(Result);
}

inline u32 
RoundReal32(f32 A)
{
    u32 Result = (u32)(A + 0.5f);
    return(Result);
}

internal u32
PackedARGB(f32 r, f32 g, f32 b, f32 a)
{
    u32 Red = RoundReal32(r * 255.0f);
    u32 Green = RoundReal32(g * 255.0f);
    u32 Blue = RoundReal32(b * 255.0f);
    u32 Alpha = RoundReal32(a * 255.0f);

    //0XAA RR GG BB
    u32 Result =
        Alpha << 24 |
        Red << 16 |
        Green << 8 |
        Blue << 0; 

    return(Result);
}


inline f32
SlowLinearToSRGB(f32 L)
{
    if(L < 0.0f)
    {
        L = 0.0f;
    }
    else if(L > 1.0f)
    {
        L = 1.0f;
    }
    
    f32 S = L * 12.92f;
    if(L > 0.0031308f)
    {
        S = 1.055f * Pow(L, 1.0f/2.4f) - 0.055f; 
    }
    return(S);
}

struct v2
{
    union
    {
        struct
        {
            f32 x;
            f32 y;
        };
        
        struct
        {
            f32 s;
            f32 t;
        };

        struct
        {
            f32 u;
            f32 v;
        };

        f32 e[2];
    };
};

struct v3
{
    union
    {
        struct
        {
            f32 x;
            f32 y;
            f32 z;
        };

        struct
        {
            f32 tn;//Tangent
            f32 bt;//Bi-Tangent
            f32 nr;//Normal
        };

        struct
        {
            f32 r;
            f32 g;
            f32 b;
        };

        f32 e[3];
    };

    v3 operator=(v3);
    v3 operator=(v4);
    f32 operator[](int);
};

struct v4
{
    union
    {
        struct
        {
            f32 x;
            f32 y;
            f32 z;
            f32 w;
        };

        struct
        {
            f32 r;
            f32 g;
            f32 b;
            f32 a;
        };
        
        f32 e[4];
    };

    v4 operator=(v4);
    f32 operator[](int);
};

//
//NOTE(gerald): Vector3 ops
//

internal u32
PackedARGB(v3 Color, f32 a = 1.0f)
{
    u32 Result = PackedARGB(Color.r, Color.g, Color.b, a);
    return(Result);
}

internal u32
PackedARGB(v4 Color)
{
    u32 Result = PackedARGB(Color.r, Color.g, Color.b, Color.a);
    return(Result);
}

internal f32
Inner(v3 A, v3 B)
{
    f32 Result = (A.x * B.x) + (A.y * B.y) + (A.z * B.z);
    return(Result);
}

internal v3
Cross(v3 A, v3 B)
{
    v3 Result = { (A.y * B.z) - (A.z * B.y),
                  (A.z * B.x) - (A.x * B.z),
                  (A.x * B.y) - (A.y * B.x)};
    
    return(Result);
}

internal v3
V3(f32 value)
{
    v3 Result = {value, value, value};
    return(Result);
}

internal v3
V3(f32 x, f32 y, f32 z)
{
    v3 Result = {x, y, z};
    return(Result);
}

internal v3
V3(v4 A)
{
    v3 Result = {A.x, A.y, A.z};
    return(Result);
}

v3 v3::
operator=(v3 A)
{
    this->x = A.x;
    this->y = A.y;
    this->z = A.z;
    return(*this);
}

v3 v3::
operator=(v4 A)
{
    this->x = A.x;
    this->y = A.y;
    this->z = A.z;
    return(*this);
}

f32 v3::
operator[](int index)
{
    Assert(index < 3);
    return this->e[index];
}

internal v3
Lerp(v3 A, f32 t, v3 B)
{
    v3 Result = {Lerp(A.x, t, B.x),
                 Lerp(A.y, t, B.y),
                 Lerp(A.z, t, B.z)};
    return(Result);
}

//Unary minus
internal v3
operator-(v3 A)
{
    v3 Result = {};
    Result.x = -A.x;
    Result.y = -A.y;
    Result.z = -A.z;
    return(Result);
}

internal v3
operator+(v3 A, v3 B)
{
    v3 Result = {};
    Result.x = A.x + B.x;
    Result.y = A.y + B.y;
    Result.z = A.z + B.z;
    return(Result);
}

internal v3
operator-(v3 A, v3 B)
{
    v3 Result = {};
    Result.x = A.x - B.x;
    Result.y = A.y - B.y;
    Result.z = A.z - B.z;
    return(Result);
}

internal v3
Hadamard(v3 A, v3 B)
{
    v3 Result = {};
    Result.x = A.x * B.x;
    Result.y = A.y * B.y;
    Result.z = A.z * B.z;
    return(Result);
}

internal v3
operator*(v3 A, f32 B)
{
    v3 Result = {};
    Result.x = A.x * B;
    Result.y = A.y * B;
    Result.z = A.z * B;
    return(Result);
}


internal v3
operator*(f32 A, v3 B)
{
    v3 Result = B * A;
    return(Result);
}

internal v3
operator/(v3 A, v3 B)
{
    v3 Result = {};
    Result.x = A.x / B.x;
    Result.y = A.y / B.y;
    Result.z = A.z / B.z;
    return(Result);
}

internal v3
operator+=(v3 &A, v3 B)
{
    A = A + B;
    return(A);
}

internal v3
operator-=(v3 &A, v3 B)
{
    A = A - B;
    return(A);
}

internal v3
operator/=(v3 &A, v3 B)
{
    A = A / B;
    return(A);
}

internal f32
LengthSq(v3 A)
{
    f32 Result = (A.x * A.x) + (A.y * A.y) + (A.z * A.z);
    return(Result);
}

internal f32
Length(v3 A)
{
    f32 LengthSquare = LengthSq(A);
    f32 Result = SquareRoot(LengthSquare);
    return(Result);
}

internal v3
Normalize(v3 A)
{
    f32 Denom = 1.0f / Length(A);
    v3 Result = A * Denom;
    return(Result);
}

internal v3
Reflect(v3 In, v3 Normal)
{
    v3 Result = {};
    Result = In - (2.0f * (Inner(In, Normal)) * Normal);
    return(Result);
}

//
//NOTE(gerald): Vector4 ops
//

internal v4
V4(f32 v)
{
    v4 Result = {v, v, v, v};
    return(Result);
}

internal v4
V4(f32 x, f32 y, f32 z)
{
    v4 Result = {x, y, z, 1.0f};
    return(Result);
}

internal v4
V4(f32 x, f32 y, f32 z, f32 w)
{
    v4 Result = {x, y, z, w};
    return(Result);
}

internal v4
V4(v3 A, f32 w = 1.0f)
{
    v4 Result = {A.x, A.y, A.z, w};
    return(Result);
}


f32 v4::
operator[](int index)
{
    Assert(index < 4);
    return this->e[index];
}

v4 v4::
operator=(v4 A)
{
    this->x = A.x;
    this->y = A.y;
    this->z = A.z;
    this->w = A.w;
    return(*this);
}


//Unary minus
internal v4
operator-(v4 A)
{
    v4 Result = {};
    Result.x = -A.x;
    Result.y = -A.y;
    Result.z = -A.z;
    Result.w = -A.w;
    return(Result);
}

internal v4
operator+(v4 A, v4 B)
{
    v4 Result = {};
    Result.x = A.x + B.x;
    Result.y = A.y + B.y;
    Result.z = A.z + B.z;
    Result.w = A.w + B.w;
    return(Result);
}

internal v4
operator-(v4 A, v4 B)
{
    v4 Result = {};
    Result.x = A.x - B.x;
    Result.y = A.y - B.y;
    Result.z = A.z - B.z;
    Result.w = A.w - B.w;
    return(Result);
}

internal v4
Hadamard(v4 A, v4 B)
{
    v4 Result = {};
    Result.x = A.x * B.x;
    Result.y = A.y * B.y;
    Result.z = A.z * B.z;
    Result.w = A.w * B.w;
    return(Result);
}

internal v4
operator*(v4 A, f32 B)
{
    v4 Result = {};
    Result.x = A.x * B;
    Result.y = A.y * B;
    Result.z = A.z * B;
    Result.w = A.w * B;
    return(Result);
}


internal v4
operator*(f32 A, v4 B)
{
    v4 Result = B * A;
    return(Result);
}

internal v4
operator/(v4 A, v4 B)
{
    v4 Result = {};
    Result.x = A.x / B.x;
    Result.y = A.y / B.y;
    Result.z = A.z / B.z;
    Result.w = A.w / B.w;
    return(Result);
}

internal v4
operator+=(v4 &A, v4 B)
{
    A = A + B;
    return(A);
}

internal v4
operator-=(v4 &A, v4 B)
{
    A = A - B;
    return(A);
}

internal v4
operator/=(v4 &A, v4 B)
{
    A = A / B;
    return(A);
}

#endif
