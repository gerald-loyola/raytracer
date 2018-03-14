/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Janastin Gerald $
   $Notice: (C) Copyright 2018 by Janastin Gerald. All Rights Reserved. $
   ======================================================================== */
#include <stdio.h>
#include <stdlib.h>
#include <windows.h> //QueryPerformace counter

#include "type.h"
#include "math_helper.h"
#include "ray.h"
#include "thread_work.h"

internal void
WriteBMPImage(char* FileName, u32 Width, u32 Height, void* Buffer)
{
    u32 BufferSizeInBytes = Width * Height * sizeof(u32);
    u32 HeaderOffsetInBytes = sizeof(BitmapFileHeader) + sizeof(BitmapInfoHeader);
    BitmapFileHeader FileHeader = {};
    FileHeader.Type = 0x4D42;//Magic Number for type 'BM'
    FileHeader.FileSize = HeaderOffsetInBytes + BufferSizeInBytes;
    FileHeader.OffBits = HeaderOffsetInBytes;

    BitmapInfoHeader InfoHeader = {};
    InfoHeader.Size = sizeof(BitmapInfoHeader);
    InfoHeader.Width = Width;
    InfoHeader.Height = Height;
    InfoHeader.Planes = 1;
    InfoHeader.BitCount = 32;
    InfoHeader.SizeImage = BufferSizeInBytes;
 
    FILE* bmpFile = fopen(FileName, "wb");
    if(bmpFile)
    {
        fwrite(&FileHeader, sizeof(BitmapFileHeader), 1, bmpFile);
        fwrite(&InfoHeader, sizeof(BitmapInfoHeader), 1, bmpFile);
        fwrite(Buffer, BufferSizeInBytes, 1, bmpFile);

        fflush(bmpFile);
        fclose(bmpFile);
    }
}

v4
Raycast(world* World, v3 RayOrigin, v3 RayDirection)
{
    v4 Result = V4(0.0f);
    f32 Tolerance = 0.00001f;

    v4 Attenuation = V4(1.0f);
    v3 NextRay = {};
    v3 NextNormal = {};
    for(u32 Bounce = 0;
        Bounce < MAX_BOUNCE;
        Bounce++)
    {
        LockedAdd(&World->TotalBounces, 1);
       
        u32 HitMaterialIndex = 0;
        f32 HitDistance = F32_MAX;
        for(u32 PlaneIndex = 0;
            PlaneIndex < World->PlaneCount;
            PlaneIndex++)
        {
            plane Plane = World->Planes[PlaneIndex];
            f32 Denom = Inner(Plane.N, RayDirection);
            if((Denom < -Tolerance) || (Denom > Tolerance))
            {
                f32 InnerP = Inner(Plane.N, RayOrigin);
                f32 t = (-Plane.D -InnerP) / Denom;
                if(t > 0.0f && t < HitDistance)
                {
                    v3 HitPoint = RayOrigin + (t * RayDirection);
                    HitDistance = t;
                    HitMaterialIndex = Plane.MaterialIndex;

                    NextRay = HitPoint;
                    NextNormal = Plane.N;
                }
            }
        }
        
        for(u32 SphereIndex = 0;
            SphereIndex < World->SphereCount;
            SphereIndex++)
        {
            sphere Sphere = World->Spheres[SphereIndex];
            v3 RelativeOrigin = RayOrigin - Sphere.P;

            f32 a = Inner(RayDirection, RayDirection);
            f32 b = 2.0f * Inner(RayDirection, RelativeOrigin);
            f32 c = Inner(RelativeOrigin, RelativeOrigin) - Square(Sphere.R);

            f32 RootTerm = Square(b) - (4.0f * a * c);
            if(RootTerm >= 0.0f)
            {
                f32 SqRootTerm = SquareRoot(RootTerm);
                f32 Denom = 2.0f * a;
                f32 t1 = (-b + SqRootTerm) / Denom;
                f32 t2 = (-b - SqRootTerm) / Denom;

                f32 t = t1;
                if(t2 > 0.0f && t2 < t)
                {
                    t = t2;
                }
                
                if(t > 0.0f && t < HitDistance)
                {
                    HitDistance = t;
                    HitMaterialIndex = Sphere.MaterialIndex;

                    v3 HitPoint = RayOrigin + (t * RayDirection);
                    NextRay = HitPoint;
                    NextNormal = Normalize(NextRay - Sphere.P);
                }
            }
        }
#if 0
        for(u32 BoxIndex = 0;
            BoxIndex < World->BoxCount;
            BoxIndex++)
        {
            box Box = World->Boxes[BoxIndex];
            v3 Min = Box.Min + Box.P;
            v3 Max = Box.Max + Box.P;
            
            f32 tMin = (Min.x - RayOrigin.x) / RayDirection.x; 
            f32 tMax = (Max.x - RayOrigin.x) / RayDirection.x; 
 
            if (tMin > tMax) Swap(tMin, tMax); 
 
            f32 tymin = (Min.y - RayOrigin.y) / RayDirection.y; 
            f32 tymax = (Max.y - RayOrigin.y) / RayDirection.y; 
 
            if (tymin > tymax) Swap(tymin, tymax); 
 
            if ((tMin > tymax) || (tymin > tMax))
            {
                continue;
            }
 
            if (tymin > tMin) 
                tMin = tymin; 
 
            if (tymax < tMax) 
                tMax = tymax; 
 
            f32 tzmin = (Min.z - RayOrigin.z) / RayDirection.z; 
            f32 tzmax = (Max.z - RayOrigin.z) / RayDirection.z; 
 
            if (tzmin > tzmax) Swap(tzmin, tzmax); 
 
            if ((tMin > tzmax) || (tzmin > tMax))
            {
                continue;
            }
 
            if (tzmin > tMin) 
                tMin = tzmin; 
 
            if (tzmax < tMax) 
                tMax = tzmax; 
#if 0
            f32 t1x = (Min.x - RayOrigin.x) / RayDirection.x;
            f32 t1y = (Min.y - RayOrigin.y) / RayDirection.y;

            f32 t2x = (Max.x - RayOrigin.x) / RayDirection.x;
            f32 t2y = (Max.y - RayOrigin.y) / RayDirection.y;

            if(t1x > t2y || t1y > t2x)
            {
                continue;
            }

            f32 tMin = t1x > t1y ? t1x : t1y;
            f32 tMax = t2x < t2y ? t2x : t2y;

            f32 t1z = (Min.z - RayOrigin.z) / RayDirection.z;
            f32 t2z = (Max.z - RayOrigin.z) / RayDirection.z;

            if(tMin > tMax)
            {
                f32 temp = tMin;
                tMin = tMax;
                tMax = temp;
            }

            if(tMin > t2z || t1z > tMax)
            {
                continue;
            }

            if(t1z > tMin)
            {
                tMin = t1z;
            }

            if(t2z < tMax)
            {
                tMax = t2z; 
            }
#endif
            
            f32 t = tMin;
            if(tMax > 0.0f && tMax < tMin)
            {
                t = tMax;
            }
            
            if(t > Tolerance && t < HitDistance)
            {
                HitDistance = t;
                HitMaterialIndex = Box.MaterialIndex;

                v3 HitPoint = RayOrigin + (t * RayDirection);
                NextRay = HitPoint;
                v3 LocaHitPoint = HitPoint - Box.P;
                v3 Dir = Normalize(LocaHitPoint);
                v3 PlaneNormals[6] = {
                    V3(0, 0, -1),
                    V3(0, 0, 1),
                    V3(0, 1, 0),
                    V3(0, -1, 0),
                    V3(1, 0, 0),
                    V3(-1, 0, 0)
                };
                f32 dp[6] = {};
                for(int i = 0; i < 6; i++)
                {
                    dp[i] = Inner(Dir, PlaneNormals[i]);
                }

                f32 MaxDp = -10000;
                u32 index = -1;
                for(int i = 0; i < 6; i++)
                {
                    if(dp[i] > MaxDp)
                    {
                        MaxDp = dp[i];
                        index = i;
                    }
                }

                NextNormal = PlaneNormals[index];
            }
        }
#endif

        if(HitMaterialIndex)
        {
            material Material = World->Materials[HitMaterialIndex];
            Result += Hadamard(Attenuation, Material.EmitColor);
            f32 Cosine = Max(Inner(-RayDirection, NextNormal), 0.0f);
            Attenuation = Hadamard(Attenuation, Cosine * Material.ReflectColor);

            RayOrigin = NextRay;
            v3 PureBounce = Reflect(RayDirection, NextNormal);
            v3 RandomNormal = NextNormal +
                V3(RandomBilateral(), RandomBilateral(), RandomBilateral());
            v3 RandomBounce = Reflect(RayDirection, RandomNormal);
            RayDirection = Normalize(Lerp(RandomBounce, Material.Specular, PureBounce));
        }
        else
        {
            //NOTE(gerald): Add SkyColor if we don't hit anything.
            Result += Hadamard(Attenuation, World->Materials[0].EmitColor);
            break;
        }
    }
    return(Result);
}

void
FillPixels(void *WorkData)
{
    raycaster_work_data *JobData = (raycaster_work_data*)WorkData;
    u32 StartX = JobData->StartX;
    u32 StartY = JobData->StartY;
    u32 ChunkWidth = JobData->ChunkWidth;
    u32 ChunkHeight = JobData->ChunkHeight;
    u32* Buffer = JobData->Buffer;
    u32 ImageWidth = JobData->ImageWidth;
    u32 ImageHeight = JobData->ImageHeight;
    f32 PixelW = JobData->PixelW;
    f32 PixelH = JobData->PixelH;
    f32 TotalPixelsWidth = JobData->TotalPixelsWidth;
    f32 TotalPixelsHeight = JobData->TotalPixelsHeight;
    v3 FilmCenter = JobData->FilmCenter;
    v3 FilmX = JobData->FilmX;
    v3 FilmY = JobData->FilmY;
    world *World = JobData->World;
    
    f32 ContribPerRay = 1.0f/RAYS_PER_PIXEL;
    camera* Camera = World->Camera;
    v3 RayOrigin = Camera->P;

    u32 MaxWidth = StartX + ChunkWidth;
    u32 MaxHeight = StartY + ChunkHeight;
    if(MaxHeight > ImageHeight)
    {
        MaxHeight = ImageHeight;
    }

    if(MaxWidth > ImageWidth)
    {
        MaxWidth = ImageWidth;
    }

    f32 TotalPixelsPct = 100.0f / (ImageHeight * ImageWidth);
    for(u32 Y = StartY; Y < MaxHeight; Y++)
    {
        u32* Pixels = Buffer + Y * ImageWidth + StartX;
        for(u32 X = StartX; X < MaxWidth; X++)
        {
            v4 PixelColor = {};
            for(u32 RayIndex = 0;
                RayIndex < RAYS_PER_PIXEL;
                RayIndex++)
            {
                f32 PixelY = (f32)Y * PixelH + PixelH * RandomUnilateral();
                f32 pY = -1.0f + 2.0f * PixelY / TotalPixelsHeight;
                
                f32 PixelX = (f32)X * PixelW + PixelW * RandomUnilateral();
                f32 pX = -1.0f + 2.0f * PixelX / TotalPixelsWidth;
                
                v3 FilmP = FilmCenter + FilmX * pX + FilmY * pY;
                v3 RayDir = Normalize(FilmP - RayOrigin);
                PixelColor += ContribPerRay * Raycast(World, RayOrigin, RayDir);
            }
            
            *Pixels++ = PackedARGB(SlowLinearToSRGB(PixelColor.r),
                                   SlowLinearToSRGB(PixelColor.g),
                                   SlowLinearToSRGB(PixelColor.b),
                                   PixelColor.a);
        }

        LockedAdd(&World->PixelsProcessed, ChunkWidth);

        if((Y-StartY) % 16 == 0)
        {
            printf("\rRaytracing ...%d%%", RoundReal32(TotalPixelsPct * World->PixelsProcessed));
        }
    }
}

int
main(char** args, int argc)
{
    LARGE_INTEGER PerformanceFrequencyData;
    QueryPerformanceFrequency(&PerformanceFrequencyData);
    s64 PerformanceCounterFrequency = PerformanceFrequencyData.QuadPart;
    
    LARGE_INTEGER LastCounter;
    QueryPerformanceCounter(&LastCounter);

    u64 LastCycleCount = __rdtsc();
    
    world World = {};

    material Materials[7] = {};
    Materials[0].EmitColor = V4(0.3, 0.4, 0.5);//Default Sky Color
    
    Materials[1].ReflectColor = V4(0.7f);//Floor Material
    
    Materials[2].ReflectColor = V4(0.8f, 0.6f, 0.01f);//Diffuse Sphere Material
    
    Materials[3].EmitColor = V4(5.0f, 0.0f, 0.0f);//Emissive Red Sphere Material
    Materials[4].EmitColor = V4(0.0f, 5.0f, 0.0f);//Emissive Green Sphere Material

    Materials[5].ReflectColor = V4(1.0f);//Mirror Material
    Materials[5].Specular = 1.0f;

    Materials[6].EmitColor = V4(0.0f, 0.0f, 5.0f);//Emissive Green Sphere Material
    
    World.MaterialsCount = ArrayCount(Materials);
    World.Materials = Materials;

    plane Planes[1] = {};
    Planes[0].N = V3(0.0f, 1.0f, 0.0f);
    Planes[0].D = 1.0f;
    Planes[0].MaterialIndex = 1;
    
    World.PlaneCount = ArrayCount(Planes);
    World.Planes = Planes;
    
    sphere Spheres[4] = {};
    Spheres[0].P = V3(0.0f, 0.0f, 0.0f);
    Spheres[0].R = 1.0f;
    Spheres[0].MaterialIndex = 2;

    Spheres[1].P = V3(2.0f, -1.0f, -1.0f);
    Spheres[1].R = 1.0f;
    Spheres[1].MaterialIndex = 3;
    
    Spheres[2].P = V3(-3.0f, 0.0f, -1.0f);
    Spheres[2].R = 1.0f;
    Spheres[2].MaterialIndex = 4;

    Spheres[3].P = V3(6.0f, 1.0f, 3.0f);
    Spheres[3].R = 2.0f;
    Spheres[3].MaterialIndex = 5;
    
    World.SphereCount = ArrayCount(Spheres);
    World.Spheres = Spheres;

#if 0    
    box Boxes[1] = {};
    Boxes[0].P = V3(-8.0f, 0.0f, 0.0f);
    Boxes[0].Min = V3(-1.5f);
    Boxes[0].Max = V3(1.5f);
    Boxes[0].MaterialIndex = 5;

    World.BoxCount = ArrayCount(Boxes);
    World.Boxes = Boxes;
#endif
    
    camera Camera;
    Camera.P = V3(0.0f, 3.0f, -8.0f);
    Camera.Target = V3(0.0f, 0.0f, 0.0f);

    v3 CameraZAxis = Normalize(Camera.Target - Camera.P);
    v3 CameraXAxis = Normalize(Cross(V3(0.0f, 1.0f, 0.0f), CameraZAxis));
    v3 CameraYAxis = Normalize(Cross(CameraZAxis, CameraXAxis));
    
    World.Camera = &Camera;
    
    //NOTE(gerald): FullHD - 1920 X 1080, AspectRatio - 16:9
    //NOTE(gerald): HalfHD - 960 X 540, AspectRatio - 16:9
    
#if FULL_HD
    u32 ImageWidth = 1920;
    u32 ImageHeight = 1080;
#elif HALF_HD
    u32 ImageWidth = 960;
    u32 ImageHeight = 540;
#else    
    u32 ImageWidth = 800;
    u32 ImageHeight = 800;
#endif
    
    u32 BufferSizeInBytes = ImageWidth * ImageHeight * sizeof(u32);
    u32* Buffer = (u32*)malloc(BufferSizeInBytes);

    f32 PixelW = 1.0f;
    f32 PixelH = 1.0f;
    
    if(ImageWidth > ImageHeight)
    {
        PixelW = PixelH * (f32)ImageWidth / (f32)ImageHeight;        
    }
    else if(ImageHeight > ImageWidth)
    {
        PixelH = PixelW * (f32)ImageHeight / (f32)ImageWidth;
    }

    f32 FOV = 90.0f;
    f32 FocalLength = tan(DegreeToRadiant(FOV * 0.5f));
    v3 FilmZ = CameraZAxis * FocalLength;
    v3 FilmCenter = Camera.P + FilmZ;

    //Fill pixels
    v3 FilmY = CameraYAxis * PixelH;
    v3 FilmX = CameraXAxis * PixelW;
    f32 TotalPixelsHeight = (f32)ImageHeight * PixelH;
    f32 TotalPixelsWidth = (f32)ImageWidth * PixelW;
    
#if SINGLE_THREAD
    raycaster_work_data RayCastData =  {
        0,
        0,
        ImageWidth,
        ImageHeight,
        (u32*)Buffer,
        ImageWidth,
        ImageHeight,
        PixelW,
        PixelH,
        TotalPixelsWidth,
        TotalPixelsHeight,
        FilmCenter,
        FilmX,
        FilmY,
        &World
    };
    FillPixels(&RayCastData);
#else
    u32 ChunkPixelSize = CHUNK_PIXEL_SIZE;
    ChunkPixelSize = Min(ChunkPixelSize, ImageWidth);
    ChunkPixelSize = Min(ChunkPixelSize, ImageHeight);
    
    u32 ChunkY = Ceil((f32)ImageHeight/ChunkPixelSize);
    u32 ChunkX = Ceil((f32)ImageWidth/ChunkPixelSize);

    u32 TotalChunks = ChunkX * ChunkY;
    raycaster_work_data *RayCastDatas = (raycaster_work_data*)malloc(TotalChunks * sizeof(raycaster_work_data));
    work_data *WorkDatas = (work_data*)malloc(TotalChunks * sizeof(work_data));
    u32 ChunkIndex = 0;
    for(u32 Y = 0;
        Y < ChunkY;
        Y++)
    {
        for(u32 X = 0;
            X < ChunkX;
            X++)
        {
            raycaster_work_data *RayCastData = RayCastDatas + ChunkIndex;
            
            *RayCastData = {
                X * ChunkPixelSize,
                Y * ChunkPixelSize,
                ChunkPixelSize,
                ChunkPixelSize,
                (u32*)Buffer,
                ImageWidth,
                ImageHeight,
                PixelW,
                PixelH,
                TotalPixelsWidth,
                TotalPixelsHeight,
                FilmCenter,
                FilmX,
                FilmY,
                &World
            };

            work_data* JobData = WorkDatas + ChunkIndex++;
            JobData->Data = RayCastData;
            JobData->Worker = FillPixels;
            JobData->Completed = 0;
        }
    }

    u32 JobIndex = 0;
    //NOTE(gerald):One Core is used for the main thread execution
    u32 ProcessorCount = GetProcessorCount() - 1;
    ProcessorCount = Min(ProcessorCount, TotalChunks);
    u32 *ThreadJobs = (u32*)malloc(sizeof(u32) * ProcessorCount);
    for(u32 ProcessorIndex = 0;
        ProcessorIndex < ProcessorCount;
        ProcessorIndex++)
    {
        ThreadJobs[ProcessorIndex] = JobIndex;
        work_data* JobData = WorkDatas + JobIndex++;
        AddWorkerThread(JobData);
    }

    u32 JobCompleted = 0;
    while(JobCompleted < TotalChunks)
    {
        for(u32 ProcessorIndex = 0;
            ProcessorIndex < ProcessorCount;
            ProcessorIndex++)
        {
            u32 Index = ThreadJobs[ProcessorIndex];
            if(Index == -1)
            {
                continue;
            }
            
            work_data* WorkData = WorkDatas + Index;
            if(WorkData->Completed == 1)
            {
                ++JobCompleted;
                if(JobIndex < TotalChunks)
                {
                    ThreadJobs[ProcessorIndex] = JobIndex;
                    work_data* JobData = WorkDatas + JobIndex++;
                    AddWorkerThread(JobData);
                }
                else
                {
                    ThreadJobs[ProcessorIndex] = -1;
                }
            }
        }
    }

    free(ThreadJobs);
    free(WorkDatas);
    free(RayCastDatas);
#endif
    
    u64 EndCycleCount = __rdtsc();
    LARGE_INTEGER EndCounter;
    QueryPerformanceCounter(&EndCounter);
                        
    u64 CycleElapsed = EndCycleCount - LastCycleCount;
    u64 CounterElapsed = EndCounter.QuadPart - LastCounter.QuadPart;
                        
    f32 MSPerFrame = 1000.0f * CounterElapsed / (f32)PerformanceCounterFrequency;
    f32 FPS = (f32)PerformanceCounterFrequency / CounterElapsed;
    f32 MSPerRay = MSPerFrame / (f32)World.TotalBounces;

    printf("\nRaysPerPixel : %d", RAYS_PER_PIXEL);
    printf("\nMaxBounce : %d", MAX_BOUNCE);
    
    printf("\nFPS : %f", FPS);
    printf("\nTotalBounce : %d", World.TotalBounces);
    printf("\nTotalTime : %f ms", MSPerFrame);
    printf("\nTimePerRay : %f ms", MSPerRay);
    

    WriteBMPImage("ray_trace_test.bmp", ImageWidth, ImageHeight, Buffer);

    free(Buffer);
    printf("\nDone.");
    return(0);    
}
