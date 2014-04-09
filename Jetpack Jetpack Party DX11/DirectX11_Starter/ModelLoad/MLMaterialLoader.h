#ifndef MODEL_LOAD_MODEL_MATERIAL_LOADER_H
#define MODEL_LOAD_MODEL_MATERIAL_LOADER_H

/*#ifdef _WIN32
	#include <GL/gl.h>
#elif __APPLE__
	#include "TargetConditionals.h"
	#ifdef TARGET_OS_IPHONE
		#import <OpenGLES/ES1/gl.h>
		#import <OpenGLES/ES1/glext.h>
	#elif TARGET_OS_IPHONE_SIMULATOR
		#import <OpenGLES/ES1/gl.h>
		#import <OpenGLES/ES1/glext.h>
	#elif TARGET_OS_MAC
	#endif
#elif __linux__
	#include <GL/gl.h>
#endif*/

#include <stdio.h>
//#include "../TextureLoad/TLTextureLoader.h"
#include "../GeneralUtility/GUParseUtility.h"
//#include "../GeneralUtility/GUMathUtility.h"

// Define GL Variables for use in DirectX
typedef float GLfloat;
typedef char GLbyte;
typedef unsigned char GLubyte;
typedef short GLshort;
typedef unsigned short GLushort;
typedef int GLint;
typedef unsigned int GLuint;
typedef long GLlong;
typedef unsigned long GLulong;

#ifdef __cplusplus
extern "C"
{
#endif    
    typedef struct
    {
        char* name;                         //name of material
        /*TLTextureImage* textureMapAm;		//texture map for amibient light
        TLTextureImage* textureMapDi;		//texture map for diffuse light
        TLTextureImage* textureMapSp;		//texture map for specular light
        TLTextureImage* textureMapAl;		//texture map for alpha
        TLTextureImage* textureMapSh;		//texture map for specular highlight
        TLTextureImage* textureMapRe;		//texture map for index of refraction (optical density)
        TLTextureImage* bumpMap;			//bump map*/
        GLfloat ambient[4];                 //rgb values of ambient light reflection
        GLfloat diffuse[4];                 //rgb values of diffuse light reflection
        GLfloat specular[4];                //rgb values of specular light reflection
        GLfloat alpha;                      //opaqueness of material
        GLfloat shininess;                  //intenstity of specular highlight
        GLfloat density;                    //optical density of material
        short illuminationModel;            //illumantion model defined by obj format 
    }MLModelMaterial;

    //enum of Illumination models
    typedef enum
    {
        ML_COLOR_NO_AMBIENT = 0,								//Do not consider ambient lighting while coloring
        ML_COLOR_WITH_AMBIENT,									//Consider ambient lighting while coloring
        ML_HIGHLIGHT,											//Specular highlight on
        ML_REFLECTION_RAY_TRACE,								//Reflection on, Ray Trace on
        ML_TRANSPARENCY_GLASS_RAY_TRACE,						//Transparency: Glass, Reflection: Ray Trace on
        ML_REFLECTION_FRESNEL_RAY_TRACE,						//Reflection: Fresnel on, Ray Trace on
        ML_TRANSPARENCY_REFRECTION_NO_FRESNEL_RAY_TRACE,		//Transparency: Refraction on, Reflection: Fresnel off, Ray Trace on
        ML_TRANSPARENCY_REFRACTION_FRESNEL_RAY_TRACE,			//Transparency: Refraction on, Reflection: Fresnel off, Ray Trace on
        ML_REFLECTION_NO_RAY_TRACE,                             //Reflection on, Ray Trace off
        ML_TRANSPARENCY_GLASS_NO_RAY_TRACE,                     //Transparency: Glass on, Reflection: Ray Trace off
        ML_SHADOWS_ON_INVISIBLE                                 //Cast Shadows on invisible surfaces
    }MLIlluminationModels;

    //enum of software that might have exported material
    typedef enum
    {
        ML_MAT_EXPORTER_UNKNOWN,	//exported by unknown software
        ML_MAT_EXPORTER_BLENDER,	//exported by Blender3D
        ML_MAT_EXPORTER_MAYA,		//exported by Maya
        ML_MAT_EXPORTER_MAX         //exported by 3ds Max
    }MLMaterialExporter;

    MLModelMaterial* MLModelMaterialCreate();
    void MLModelMaterialDelete(MLModelMaterial* deletee);
    MLModelMaterial** mlModelMaterialLibsLoadMTL(char** matLibs, int matLibCount, unsigned int* numMatsOut);

#ifdef __cplusplus
}
#endif

#endif
