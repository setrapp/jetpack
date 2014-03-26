#ifndef TEXTURE_LOAD_TEXTURE_LOADER_H
#define TEXTURE_LOAD_TEXTURE_LOADER_H

#ifdef _WIN32
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
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __cplusplus
extern "C"
{
#endif

    typedef struct
    {
        char* filename;			//filename of texture image
        GLubyte* imageData;		//image data (32 bits max)
        GLuint bpp;				//image color depth (Bits per pixel)
        GLuint width;			//image width
        GLuint height;			//imagie height
        GLuint textureID;		//ID used to select texture
    }TLTextureImage;

    TLTextureImage* TLTextureImageCreate();
    void TLTextureImageDelete(TLTextureImage* deletee);
    int TLTextureImageLoadTGA(TLTextureImage* texture, char const* filename);

#ifdef __cplusplus
}
#endif

#endif
