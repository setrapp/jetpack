#ifndef GENERAL_UTILITY_MATH_UTILITY_H
#define GENERAL_UTILITY_MATH_UTILITY_H

#ifdef _WIN32
#elif __APPLE__
	#include "TargetConditionals.h"
	#ifdef TARGET_OS_IPHONE
	#elif TARGET_OS_IPHONE_SIMULATOR
	#elif TARGET_OS_MAC
	#endif
#elif _linux
#endif

#ifndef PI
#define PI 3.14159265358979323846f
#endif
	
#ifndef NORMAL_EPSILON
#define NORMAL_EPSILON .00001f
#endif

#ifndef NULL
#define NULL 0x00000000
#endif

#define GU_COLUMN_MAJOR 0
#define GU_ROW_MAJOR 1

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef __cplusplus
extern "C"
{
#endif

    //unsigned byte
    typedef unsigned char GUubyte;
    
    //signed byte
    typedef char GUbyte;
    
    //2D Vector, Normal
    typedef struct
    {
        //coordinates
        float x;
        float y;
    }GUVector2D;
    typedef GUVector2D GUPoint2D;
    typedef GUVector2D GUNormal2D;
    
    //2D Vector, Normal(integer)
    typedef struct
    {
        //coordinates
        int x;
        int y;
    }GUIntVector2D;
    typedef GUVector2D GUIntPoint2D;
    typedef GUVector2D GUIntNormal2D;
    
    //3D Vector, Normal, Rotation
    typedef struct
    {
        //coordinates
        float x;
        float y;
        float z;
    }GUVector3D;
    typedef GUVector3D GUPoint3D;
    typedef GUVector3D GUNormal3D;
    typedef GUVector3D GURotation3D;
    
    //3D Vector, Normal, Rotation (integer)
    typedef struct
    {
        //coordinates
        int x;
        int y;
        int z;
    }GUIntVector3D;
    typedef GUVector3D GUIntPoint3D;
    typedef GUVector3D GUIntNormal3D;
    typedef GUVector3D GUIntRotation3D;

    //--- Exponential Functions ---//
    int guPowInt(int base, int power);
    unsigned int guPowUInt(unsigned int base, int power);
    long guPowLong(long base, int power);
    unsigned long guPowULong(unsigned long base, int power);
    
    //--- Geometry Calculations ---//
    GUNormal3D guCalcPlaneNormal(GUPoint3D const* point1, GUPoint3D const* point2, GUPoint3D const* point3);
    void guNormalize(GUVector3D* vector);
    float guDotProduct(GUVector3D vector1, GUVector3D vector2);
    GUVector3D guCrossProduct(GUVector3D vector1, GUVector3D vector2);
    

#ifdef __cplusplus
}
#endif

#endif
