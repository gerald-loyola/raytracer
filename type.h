#if !defined(TYPE_H)
/* ========================================================================
   $File: $
   $Date: $
   $Revision: $
   $Creator: Janastin Gerald $
   $Notice: (C) Copyright 2018 by Janastin Gerald. All Rights Reserved. $
   ======================================================================== */

#define TYPE_H

#include <float.h> //FLT_MAX
#define True 1
#define False 0

//TODO(gerald):Fix this
#define F32_MAX FLT_MAX;


#define internal static
#define Assert(expression) if(!(expression)) (*(int*)0 = 0)
#define ArrayCount(elements) (sizeof(elements) / sizeof(elements[0]))


typedef unsigned char u8;
typedef unsigned short int u16;
typedef unsigned int u32;
typedef unsigned long int u64;

typedef float f32;
typedef double d32;

typedef int b32;

typedef int s32;
typedef long int s64;

typedef size_t memory_index;

#define U32_MAX ((u32)-1)

#endif
