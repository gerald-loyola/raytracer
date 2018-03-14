#if !defined(RAY_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Janastin Gerald $
   $Notice: (C) Copyright 2018 by Janastin Gerald. All Rights Reserved. $
   ======================================================================== */

#define RAY_H

#pragma pack(push, 1)
struct BitmapFileHeader
{
  u16 Type;  //specifies the file type
  u32 FileSize;  //specifies the size in bytes of the bitmap file
  u16 Reserved1;  //reserved; must be 0
  u16 Reserved2;  //reserved; must be 0
  u32 OffBits; //species the offset in bytes from the bitmapfileheader to the bitmap bits
};

struct BitmapInfoHeader
{
  u32 Size;
  s32 Width;
  s32 Height;
  u16 Planes;
  u16 BitCount;
  u32 Compression;
  u32 SizeImage;

  s32 XPelsPerMeter;  //number of pixels per meter in x axis
  s32 YPelsPerMeter;  //number of pixels per meter in y axis
  u32 ClrUsed;  //number of colors used by th ebitmap
  u32 ClrImportant;  //number of colors that are important
};
#pragma pack(pop)

struct material
{
    //NOTE(gerald): 0 is diffuse like chalk and 1 is specular like mirror
    f32 Specular;
    v4 EmitColor;
    v4 ReflectColor;
};

struct sphere
{
    v3 P;
    f32 R;
    u32 MaterialIndex;
};

struct plane
{
    v3 N;
    f32 D;
    u32 MaterialIndex;
};

struct box
{
    v3 P;
    v3 Min;
    v3 Max;
    u32 MaterialIndex;
};


struct camera
{
    v3 P;
    v3 Target;
};

struct world
{
    u32 MaterialsCount;
    material *Materials;

    u32 PlaneCount;
    plane *Planes;

    u32 SphereCount;
    sphere *Spheres;

    u32 BoxCount;
    box* Boxes;

    volatile u32 TotalBounces;
    volatile u32 PixelsProcessed;
    
    //TODO(gerald); Make room for multiple cameras
    camera *Camera;
};

struct raycaster_work_data
{
    u32 StartX;
    u32 StartY;

    //Common Data for all jobs
    u32 ChunkWidth;
    u32 ChunkHeight;
    u32* Buffer;
    u32 ImageWidth;
    u32 ImageHeight;
    f32 PixelW;
    f32 PixelH;
    f32 TotalPixelsWidth;
    f32 TotalPixelsHeight;
    v3 FilmCenter;
    v3 FilmX;
    v3 FilmY;
    world *World;
};

#endif
