#ifndef MODEL_LOAD_MODEL_VIEWER_H
#define MODEL_LOAD_MODEL_VIEWER_H

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
#endif
	*/

#include <stdio.h>
#include "MLMaterialLoader.h"
#include "../GeneralUtility/GUParseUtility.h"
#include "../GeneralUtility/GUMathUtility.h"

#ifdef __cplusplus
extern "C"
{
#endif

    //Vertex
    typedef struct _ml_vertex3D MLVertex3D;

    //Texel
    typedef struct _ml_texelXY MLTexelXY;
        
    //Face
    typedef struct _ml_face3D MLFace3D;

    //Model
    typedef struct _ml_model3D MLModel3D;

    //enum of model faces to reference
    typedef enum
    {
        ML_MODEL_FACE_NONE,
        ML_MODEL_FACE_FRONT,
        ML_MODEL_FACE_BACK,
        ML_MODEL_FACE_FRONT_AND_BACK
    }MLModelFaces;
    
    MLModel3D* MLModel3DCreate(MLVertex3D* vertices,  unsigned int numVertices, MLFace3D* faces,  unsigned int numFaces, MLTexelXY* textureVertices,  unsigned int numTextureVertices, MLModelMaterial** materials,  unsigned int numMaterials);
    void MLModel3DDelete(MLModel3D* deletee);
    MLModel3D* mlModel3DLoadOBJ(const char* filename);
    void mlModel3DLoadTextures(MLModel3D* targetModel);
    void mlModel3DDraw(MLModel3D* targetModel, GLfloat transform[16]);
    void mlModel3DDrawForPick(MLModel3D* targetModel, GLfloat transform[16], GLubyte red, GLubyte green, GLubyte blue);
    void mlModel3DShowFaceNormals(MLModel3D* targetModel);
    void mlModel3DHideFaceNormals(MLModel3D* targetModel);
    void mlModel3DShowVertexNormals(MLModel3D* targetModel);
    void mlModel3DHideVertexNormals(MLModel3D* targetModel);
    void mlModel3DSetFaceCulling(MLModel3D* targetModel, int enumModelFace);

	char const* mlModel3DGetFilename(MLModel3D const* targetModel);
	MLVertex3D const* mlModel3DGetVertex(MLModel3D const* targetModel, unsigned int index);
	unsigned int mlModel3DGetVertexCount(MLModel3D const* targetModel);
	MLFace3D const* mlModel3DGetFace(MLModel3D const* targetModel, unsigned int index);
	unsigned int mlModel3DGetFaceCount(MLModel3D const* targetModel);
	MLTexelXY const* mlModel3DGetTextureVertex(MLModel3D const* targetModel, unsigned int index);
	unsigned int mlModel3DGetTextureVertexCount(MLModel3D const* targetModel);
	MLModelMaterial const* mlModel3DGetMaterial(MLModel3D const* targetModel, unsigned int index);
	unsigned int mlModel3DGetMaterialCount(MLModel3D const* targetModel);
	GLushort mlFace3DGetVertex1(MLFace3D const* targetFace);
	GLushort mlFace3DGetVertex2(MLFace3D const* targetFace);
	GLushort mlFace3DGetVertex3(MLFace3D const* targetFace);
	GLushort mlFace3DGetTextureVertex1(MLFace3D const* targetFace);
	GLushort mlFace3DGetTextureVertex2(MLFace3D const* targetFace);
	GLushort mlFace3DGetTextureVertex3(MLFace3D const* targetFace);
	GUNormal3D mlFace3DGetNormal(MLFace3D const* targetFace);
	GLushort mlFace3DGetMaterial(MLFace3D const* targetFace);
	GUPoint3D mlVertex3DGetPosition(MLVertex3D const* targetVertex);
	GUNormal3D mlVertex3DGetNormal(MLVertex3D const* targetVertex);
	GUPoint2D mlTexelXYGetPosition(MLTexelXY const* targetTexel);
	


#ifdef __cplusplus
}
#endif
#endif
