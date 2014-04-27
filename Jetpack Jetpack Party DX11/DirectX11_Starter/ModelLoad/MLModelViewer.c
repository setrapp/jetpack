#include "MLModelViewer.h"

int parseModelData(void** data, int* dataTypes, int* dataSizes, int dataCount, char* read, char* substringBuffer, int readSize, FILE* file, int isCountOnly_bool);
void recalcVertexNormal(MLVertex3D* vert, MLFace3D const* modelFaceList, int addedFaceIndex);
int freeReadModelReturn(int* delimiterIndices, int out);

//Vertex
struct _ml_vertex3D
{
	//position
	GUPoint3D position;
	//normal of vertex
	GUNormal3D normal;
	//number of faces defined by this vertex
	GLint numFacesDefined;
	//number of face normals averaged
	GLint numFacesAveraged;
	//list of (model indices of) faces whose normals have been averaged together to calculate this vertex's normal
	int* facesAveraged;	
};

//unsigned byte rgba color
typedef struct
{
    GLubyte red, green, blue, alpha;
}MLColor4ub;

//Rendering Vertex
typedef struct
{
	//position
	GUPoint3D position;
	//normal of vertex
	GUNormal3D normal;
}MLRenderingVertex3D;

//Picking Vertex
typedef struct
{
	//position
	GUPoint3D position;
	//color of vertex
	MLColor4ub color;
}MLPickingVertex3D;

//Texel
struct _ml_texelXY
{
	//texture position
	GUPoint2D position;
};

//Face
struct _ml_face3D
{
	//physical vertices
	GLushort vert1;
	GLushort vert2;
	GLushort vert3;
	//texture vertices
	GLushort texVert1;
	GLushort texVert2;
	GLushort texVert3;
	//surface normal of face
	GUNormal3D normal;
	//material of face
	GLushort materialIndex;
};

struct _ml_model3D
{
	const char* filename;
	unsigned int numVertices, numFaces, numTextureVertices, numMaterials;
	MLVertex3D* vertices;
	MLFace3D* faces;
	//int* textures;
	MLTexelXY* textureVertices;
	MLModelMaterial** materials;
	MLModelMaterial* currentMaterial;
	int showFaceNormalsFlag;
	int showVertexNormalsFlag;
    int cullFace;
};

//enumeration of the model feature types and data read types
typedef enum
{
	MODEL_UNUSABLE,				//data is unusable in defining the model
	MODEL_END,					//the end of the model definition
	MODEL_VERTEX,				//position of physical vertex of model
	MODEL_TEXTURE_VERTEX,		//position on texture
	MODEL_VERTEX_NORMAL,		//normal of vertex
	MODEL_FACE_NORMAL,			//normal of face
	MODEL_FACE,					//single face of model
	MODEL_SPLIT_FACE,			//face needing to be split from a quad into two triangles
	MODEL_MAT_LIB,				//filename for a material library
	MODEL_MAT_NAME				//name of material being used
}MLModelFeatures;

typedef struct
{
	int switchFace;
	char* name;
}MLMaterialSwitch;

//construct model
MLModel3D* MLModel3DCreate(MLVertex3D* vertices,unsigned int numVertices, MLFace3D* faces, unsigned int numFaces, MLTexelXY* textureVertices, unsigned int numTextureVertices, MLModelMaterial** materials, unsigned int numMaterials) 
{
	MLModel3D* newModel = (MLModel3D*)malloc(sizeof(MLModel3D));
	newModel->vertices = vertices, 
	newModel->faces = faces; 
	newModel->textureVertices = textureVertices; 
	newModel->numVertices = numVertices;
	newModel->numFaces = numFaces;
	newModel->numTextureVertices = numTextureVertices;
	newModel->materials = materials;
	newModel->numMaterials = numMaterials;
	newModel->currentMaterial = NULL;
	newModel->showFaceNormalsFlag = 0;
	newModel->showVertexNormalsFlag = 0;
    newModel->cullFace = ML_MODEL_FACE_BACK;
	
	return newModel;
}

//dellocate texture from memory
void MLModel3DDelete(MLModel3D* deletee)
{
	int i = 0;
	if(!deletee)
        return;
	if(deletee->filename)
		free((char*)deletee->filename);
    if(deletee->vertices != NULL)
		free(deletee->vertices);
	if(deletee->textureVertices != NULL)
		free(deletee->textureVertices);
	if(deletee->faces != NULL)
		free(deletee->faces);
	if(deletee->materials)
	{
		MLModelMaterial** materials = deletee->materials;
		int numMaterials = deletee->numMaterials;
		for(i = 0; i < numMaterials; i++)
			free(materials[i]);
		free(materials);
	}
	free(deletee);
}

//load model from OBJ file
MLModel3D* mlModel3DLoadOBJ(const char* filename)
{	
	/* Specifications:
	 * v:		Vertex
	 * vt:		Texture Vertex
	 * vn:		Normal (not used here because face normals are calculated later)
	 * f:		Face (vertex indices order: vertexIndex/textureVertexIndex/normalIndex)
	 *		Note that if a face is defined as a quad rather than a triangle, it will be converted into two triangular faces
	 * mtllib:	Material Library file
	 * usemtl:	Material to be used until next call
	 */

	unsigned int i = 0; //counter
	int failed = 0;		//failure check

	//extract path from filename for future use
	char* filePath = guExtractFilePath(filename, NULL, NULL, GU_DIRECTORY_DELIMITER);

	//--- Declare data reading varibles (define what can be) ---//
	int readSize = GU_DEFAULT_BUFFER_SIZE;	//reading max length
	char* read = NULL;                      //character buffer for reading file
	char* substringBuffer = NULL;			//character buffer for holding substrings when parsing
	void** data = NULL;                     //data to be used
	int* dataTypes = NULL;                  //type of data
	int* dataSizes = NULL;                  //elements of data
	unsigned int maxDataCount = 8;			//max elements of data

	//--- Declare structures that will be used in creating Model ---//
	MLVertex3D* verts = NULL;				//array of physical vertices
	MLTexelXY* texVerts = NULL;             //array of texture vertices
	MLFace3D* faces = NULL;					//array of faces
	char** matLibs = NULL;					//array of material libraries (not actual materials)
	int* matLibNameLengths = NULL;			//size of each material library's filename
	MLMaterialSwitch* matSwitches = NULL;	//materials switches at faces
	MLModel3D* model = NULL;				//model to be created
	char featureType = '\0';				//type of feature being defined
	unsigned int vertexCount = 0, faceCount = 0, textureVertexCount = 0, matLibCount = 0, matSwitchCount = 0;
	char defaultSwitchName[] = {'-'};

	//--- Declare structures that will be used in linking materials ---//
	unsigned int matCount;						//number of mats found
	MLModelMaterial** mats = NULL;				//array of constructed materials
	GLushort* faceMats = NULL;					//array of indices of materials connected to model faces

	//attempt to open model file
	FILE* objFile;

	if(!guAttemptFileOpen(&objFile, filename, ".obj", GU_DEFAULT_BUFFER_SIZE, 3))
		failed = 1;

	if (!failed)
	{
		//allocate memory for temporary data storage
		data = (void**)malloc(sizeof(void*) * maxDataCount);
		dataTypes = (int*)malloc(sizeof(int) * maxDataCount);
		dataSizes = (int*)malloc(sizeof(int) * maxDataCount);
		for(i = 0; i < maxDataCount; i++)
			data[i] = (void*)malloc(sizeof(void*));

		//-- Parse through the the file once to calculate the number of each feature ---//	
		//setup stream reading buffer
		readSize = GU_DEFAULT_BUFFER_SIZE;
		read =(char*)malloc(sizeof(char)*readSize);
	
		//allocate buffer to hold substrings
		substringBuffer = (char*)malloc(sizeof(char) * 256);

		//counts
		while((featureType = parseModelData(NULL, NULL, NULL, 0, read, substringBuffer, GU_DEFAULT_BUFFER_SIZE, objFile, 1)) != MODEL_END)
		{
			//count model feature
			switch(featureType)
			{
				case  MODEL_VERTEX:						//vertex
					vertexCount++;
					break;
				case MODEL_TEXTURE_VERTEX:				//texture vertex
					textureVertexCount++;
					break;
				case MODEL_FACE:						//one face
					faceCount++;
					break;
				case MODEL_SPLIT_FACE:					//two faces
					faceCount += 2;
					break;
				case MODEL_MAT_LIB:						//material library filename
					matLibCount++;
					break;
				case MODEL_MAT_NAME:					//switch material being used
					matSwitchCount++;
					break;
				default:
					break;
				}
		}
		
		//---Reset counters and prepare to actually read data in second pass ---//
		//allocate memory for vertex and face arrays
		verts = (MLVertex3D*)malloc(sizeof(MLVertex3D)*(vertexCount+1));
		texVerts = (MLTexelXY*)malloc(sizeof(MLTexelXY)*(textureVertexCount+1));
		faces = (MLFace3D*)malloc(sizeof(MLFace3D)*(faceCount+1));
		matLibs = (char**)malloc(sizeof(char*)*matLibCount);											//note that this is not an array of actual materials but the files that specify them, materials will be loaded later
		matLibNameLengths = (int*)malloc(sizeof(int)*matLibCount);										//size of each material library's filename
		matSwitches = (MLMaterialSwitch*)malloc(sizeof(MLMaterialSwitch) * (matSwitchCount+1));         //materials switches at faces
	
		//set the first index of vertices to a default, verts[0] is used to signify no vertex is used (more useful for texture vertices, convention kept for consistency)
		verts[0].position.x = 0;
		verts[0].position.y = 0;
		verts[0].position.z = 0;
		verts[0].normal.x = 0;
		verts[0].normal.y = 0;
		verts[0].normal.z = 0;
		verts[0].numFacesDefined = 0;
		verts[0].numFacesAveraged = 0;
		verts[0].facesAveraged = 0;
		//set the first index of faces to a default, faces[0] is used to signify no face is used (more useful for texture vertices, convention kept for consistency)
		faces[0].vert1 = 0;
		faces[0].vert2 = 0;
		faces[0].vert3 = 0;
		faces[0].texVert1 = 0;
		faces[0].texVert2 = 0;
		faces[0].texVert3 = 0;
		faces[0].normal.x = 0;
		faces[0].normal.y = 0;
		faces[0].normal.z = 0;
		//set the first index of texture vertices to a default, textureVerts[0] is used to signify no texture vertex is used
		texVerts[0].position.x = 0;
		texVerts[0].position.y = 0;
		//set the first index of material switch locations to 0 and first index of switched materials to the default to account for faces before first switch
		matSwitches[0].switchFace = 0;
		matSwitches[0].name = defaultSwitchName;

		//re-open file (closed by parseModelData at end of file)
		if((objFile = fopen(filename, "r")) == 0)
		{
			printf("%s could not be re-opened", filename);
			failed = 1;
		}
	}
	
	if(!failed)
	{
		//--- Parse through the file again to populate the vertex and face arrays ---//
		//reset feature type and feature counts
		featureType = MODEL_UNUSABLE;
		vertexCount = 0, faceCount = 0, textureVertexCount = 0, matLibCount = 0, matSwitchCount = 0;
		//start counting features (not including material libraries) vertices at one to allow for the special zero index case (the zero index is used in the absence of -1 so unsigned numbers can be used)
		vertexCount++;
		faceCount++;
		textureVertexCount++;

		while((featureType = parseModelData(data, dataTypes, dataSizes, maxDataCount, read, substringBuffer, readSize, objFile, 0)) != MODEL_END)
		{
			//objects that could be populated depending on model data retrieved
			MLVertex3D v;
			MLTexelXY t;
			MLFace3D f;
			MLFace3D f1;
			MLFace3D f2;
		
			//construct model feature
			switch(featureType)
			{
				case MODEL_VERTEX:					//vertex
					v.position.x = *(GLfloat*)data[0];
					v.position.y = *(GLfloat*)data[1];
					v.position.z = *(GLfloat*)data[2];
					v.normal.x = 0;
					v.normal.y = 0;
					v.normal.z = 0;
					v.numFacesDefined = 0;
					v.numFacesAveraged = 0;
					v.facesAveraged = NULL;
					verts[vertexCount] = v;
					vertexCount++;
					break;
				case MODEL_TEXTURE_VERTEX:			//texture vertex
					t.position.x = *(GLfloat*)data[0];
					t.position.y = *(GLfloat*)data[1];
					texVerts[textureVertexCount] = t;
					textureVertexCount++;
					break;
				case MODEL_FACE:					//one face
					f.vert1 = *(GLushort*)data[0];
					f.vert2 = *(GLushort*)data[1];
					f.vert3 = *(GLushort*)data[2];
					f.texVert1 = *(GLushort*)data[4];
					f.texVert2 = *(GLushort*)data[5];
					f.texVert3 = *(GLushort*)data[6];
					faces[faceCount] = f;
					faceCount++;
					break;
				case MODEL_SPLIT_FACE:				//two faces 
					f1.vert1 = *(GLushort*)data[0];
					f1.vert2 = *(GLushort*)data[1];
					f1.vert3 = *(GLushort*)data[2];
					f1.texVert1 = *(GLushort*)data[4];
					f1.texVert2 = *(GLushort*)data[5];
					f1.texVert3 = *(GLushort*)data[6];
					faces[faceCount] = f1;
					faceCount++;
					f2.vert1 = *(GLushort*)data[2];
					f2.vert2 = *(GLushort*)data[3];
					f2.vert3 = *(GLushort*)data[0];
					f2.texVert1 = *(GLushort*)data[6];
					f2.texVert2 = *(GLushort*)data[7];
					f2.texVert3 = *(GLushort*)data[4];
					faces[faceCount] = f2;
					faceCount++;
					break;
				case MODEL_MAT_LIB:					//material library filename
					matLibs[matLibCount] = guCopyString((char*)data[0], NULL, GU_DEFAULT_BUFFER_SIZE);
					matLibNameLengths[matLibCount] = dataSizes[0];
					matLibCount++;
					break;
				case MODEL_MAT_NAME:				//switch material being used
					//set material swith location to current face number 
					//materials will be applied to all following faces until next switch (inclusive of the face at the switch index)
					matSwitches[matSwitchCount].switchFace = faceCount;				

					//use default material if no material specified at switch
					if((data == NULL) || (guCompare((char*)data[0], "(null)", 0, GU_DEFAULT_BUFFER_SIZE) == 0) || (guCompare((char*)data[0], "Material", 0, GU_DEFAULT_BUFFER_SIZE) == 0))
					{
						matSwitches[matSwitchCount].name = (char*)malloc(sizeof(char)*2);
						matSwitches[matSwitchCount].name[0] = '-';
						matSwitches[matSwitchCount].name[1] = '\0';
					}
					else
						matSwitches[matSwitchCount].name = guCopyString((char*)data[0], NULL, GU_DEFAULT_BUFFER_SIZE);

					matSwitchCount++;
					break;
				default:
					break;
			}
		}

		//deallocate data buffers
		for(i = 0; i < maxDataCount; i++)
		{
			//avoid deallocating parsing buffers
			if(data[i] != read && data[i] != substringBuffer)
				free(data[i]);
		}
		free(data);
		free(dataTypes);
		free(dataSizes);

		//deallocate stream reading buffer and substring buffer
		free(read);
		read = NULL;
		free(substringBuffer);
		substringBuffer = NULL;

		//calculate surface normals
		for(i = 0; i < faceCount; i++)
			faces[i].normal = guCalcPlaneNormal(&verts[faces[i].vert1].position, &verts[faces[i].vert2].position, &verts[faces[i].vert3].position);

		//count how many faces are defined by each vertex for vector normal calculations to come
		for(i = 0; i < faceCount; i++)
		{
			verts[faces[i].vert1].numFacesDefined++;
			verts[faces[i].vert2].numFacesDefined++;
			verts[faces[i].vert3].numFacesDefined++;
		}

		//recalculate vertex normals
		for(i = 1; i < vertexCount; i++)
		{
			unsigned int j;
			for(j = 1; j < faceCount; j++)
			{
				MLFace3D checkFace = faces[j];
			
				if(&verts[i] == &verts[checkFace.vert1] || &verts[i] == &verts[checkFace.vert2] || &verts[i] == &verts[checkFace.vert3])
					recalcVertexNormal(&verts[i], faces, j);
			}
		}

		//deallocate vertex face references
		for(i = 1; i < vertexCount; i++)
		{
			free(verts[i].facesAveraged);
			verts[i].facesAveraged = NULL;
		}

		//load materials
		matCount = 0;
		for(i = 0; i < matLibCount; i++)
		{
			char* matLibPath = guConcat(filePath, matLibs[i], NULL, NULL, GU_DEFAULT_BUFFER_SIZE);
			free(matLibs[i]);
			matLibs[i] = matLibPath;
		}
		mats = mlModelMaterialLibsLoadMTL(matLibs, matLibCount, &matCount);	

		//deallocate material library list
		for(i = 0; i < matLibCount; i++)
			free(matLibs[i]);

		//link face names from model file to materials loaded from libraries
		if (matSwitchCount > 0)
		{
			faceMats = (GLushort*)malloc(sizeof(MLModelMaterial*)*matSwitchCount+1);
			faceMats[0] = 0;
			for(i = 1; i < matSwitchCount; i++)
			{
				int matFound = 0;						//is there a material with the same name as the switch
				if(matSwitches[i].name)					//if there is a name specified at the switch attempt to find the material in library
				{
					unsigned int j = 1;
					for(j = 1; j < matCount && !matFound; j++)
					{
						//match faces between model file and material file
						if(mats[j] && guCompare(matSwitches[i].name, mats[j]->name, 0, GU_DEFAULT_BUFFER_SIZE) == 0)	
						{
							faceMats[i] = j;
							matFound = 1;
						}
					}
				}

				//if no match found, use default
				if(!matFound)
					faceMats[i] = 0;
			}
		}
		else 
		{
			faceMats = NULL;
		}

		//connect faces to materials
		for(i = 0; i < matSwitchCount; i++)
		{
			unsigned int j = 0;
			unsigned int matBegin = matSwitches[i].switchFace;												//first face using material
			unsigned int matEnd = (i == matSwitchCount-1 ? faceCount : matSwitches[i+1].switchFace);		//first face not using material, use end of face array if no more switches exist
	
			//apply current material to all faces before next switch
			for(j = matBegin; j < matEnd; j++)
				faces[j].materialIndex = faceMats[i];
		}

		//create new model and using vertex and face arrays
		model =  MLModel3DCreate(verts, vertexCount, faces, faceCount, texVerts, textureVertexCount, mats, matCount);
		model->filename = guCopyString(filename, NULL, GU_DEFAULT_BUFFER_SIZE);
	}

	//deallocate buffers that are no longer needed
	free(matLibs);
	free(matLibNameLengths);
	for(i = 0; i < matSwitchCount; i++)
		free(matSwitches[i].name);
	free(matSwitches);
	free(faceMats);
	free(filePath);

	//return pointer to model
	return model;
}

//load textures for model
void mlModel3DLoadModelTextures(MLModel3D* targetModel)
{
    //declare counter
    unsigned int i = 0;
	
	//get directory path to model file to assist in finding texture files
	char* filePath = guExtractFilePath(targetModel->filename, NULL, NULL, GU_DIRECTORY_DELIMITER);
	
	//if attempt to open filename as specified fails, attempt to open file relative to the material library
	for(i = 0; i < targetModel->numMaterials; i++)
	{
		//only attempt to open files whose names are not empty strings
		/*if(!(guCompare(targetModel->materials[i]->textureMapDi->filename, "\0", 0, GU_DEFAULT_BUFFER_SIZE) == 0))
			if(!TLTextureImageLoadTGA(targetModel->materials[i]->textureMapDi, targetModel->materials[i]->textureMapDi->filename))
				if(!TLTextureImageLoadTGA(targetModel->materials[i]->textureMapDi, guConcat(filePath, targetModel->materials[i]->textureMapDi->filename, NULL, NULL, GU_DEFAULT_BUFFER_SIZE)))
					printf("The file, %s, could not be opened on an absolute or relative path", targetModel->materials[i]->textureMapDi->filename);
		*/
	}

	free(filePath);
}

//draw model
/*void mlModel3DDraw(MLModel3D* targetModel, GLfloat transform[16])
{    
    unsigned int i = 0;                                                     //counter
	float modelViewMatrix[16];                                              //modelview matrix
	GLboolean glLightingEnabledPrior	= glIsEnabled(GL_LIGHTING);			//is lighting enabled before drawing model
	GLboolean glNormalizeEnabledPrior	= glIsEnabled(GL_NORMALIZE);		//is normal normalizing enabled before drawing model
#ifdef GL_RESCALE_NORMAL
	GLboolean glRescaleEnabledPrior		= glIsEnabled(GL_RESCALE_NORMAL);	//is normal rescaling enabled before drawing model
#endif
	GLboolean glVertexArrayEnabledPrior	= glIsEnabled(GL_VERTEX_ARRAY);		//is the use of vertex arrays enabled before drawing model
	GLboolean glNormalArrayEnabledPrior	= glIsEnabled(GL_NORMAL_ARRAY);		//is the use of normal arrays before drawing model
    GLboolean glFaceCullingEnabledPrior = glIsEnabled(GL_CULL_FACE);        //is face culling enabled before drawing model
    GLint glFaceCulledPrior;                                                //what faces are being culled before drawing model
    GLint drawnFace;                                                        //faces (OpenGL enum) to be drawn based on what is not being automatically culled for winding order
    MLFace3D* faceArray = targetModel->faces;								//reference target models arrays of faces
	MLVertex3D* vertexArray = targetModel->vertices;						//reference target models arrays of vertices
	MLTexelXY* texelArray = targetModel->textureVertices;					//reference target models arrays of textures vertices
    
	//get face culling mode before drawing
	glGetIntegerv(GL_CULL_FACE_MODE, &glFaceCulledPrior);

	//prepare for drawing
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();

	//transform modelview matrix
	if(transform != NULL)
		glMultMatrixf(transform);
	
	//get current modelview matrix
	glGetFloatv(GL_MODELVIEW_MATRIX, modelViewMatrix);
	
	//disable both normalization and rescaling of normals to be enabled later
	glDisable(GL_NORMALIZE);
#ifdef GL_RESCALE_NORMAL
	glDisable(GL_RESCALE_NORMAL);
#endif
	
#ifdef GL_RESCALE_NORMAL
	//if ModelView is uniformly scaled (sx = sy = sz), return normals to unit length by scaling by derived scale factor after transformation 
	if(modelViewMatrix[0] == modelViewMatrix[5] == modelViewMatrix[10])
	{
		glEnable(GL_RESCALE_NORMAL);
	}
	//otherwise, return normals to unit length by normalizing them (more expensive)
	else		
	{
		glEnable(GL_NORMALIZE);
	}
#else
	glEnable(GL_NORMALIZE);
#endif
    
    //set OpenGl face culling for model
    if(targetModel->cullFace == ML_MODEL_FACE_NONE)
    {
        glDisable(GL_CULL_FACE);
        drawnFace = GL_FRONT_AND_BACK;
    }
    else if(targetModel->cullFace == ML_MODEL_FACE_FRONT)
    {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        drawnFace = GL_BACK;
    }
    else if(targetModel->cullFace == ML_MODEL_FACE_BACK)
    {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        drawnFace = GL_FRONT;
    }
    else if(targetModel->cullFace == ML_MODEL_FACE_FRONT_AND_BACK)
    {
        //both faces will be culled, so nothing will be drawn
        return; 
    }
    
	//enable drawing with vertex and normal arrays
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
    
    //enable lighting
    glEnable(GL_LIGHTING);
	
	//--- Draw model using vertex indices stored in each face ---// 
	for(i = 1; i < targetModel->numFaces; i++)							//start at face index 1 because 0 is empty
	{		
		//retrieve material of the current face
		MLModelMaterial* faceMaterial = targetModel->materials[targetModel->faces[i].materialIndex];

		//create array of vertices in face
		MLRenderingVertex3D verts[3];		

        //flag if modeled is textured
		int textured = 0;
        
        //populate rendering vertices with information from model vertices
		verts[0].position = vertexArray[faceArray[i].vert1].position;
		verts[0].normal = vertexArray[faceArray[i].vert1].normal;
		verts[1].position = vertexArray[faceArray[i].vert2].position;
		verts[1].normal = vertexArray[faceArray[i].vert2].normal;
		verts[2].position = vertexArray[faceArray[i].vert3].position;
		verts[2].normal = vertexArray[faceArray[i].vert3].normal;
        
		//setup face material
		if(faceMaterial != targetModel->currentMaterial)
		{
			targetModel->currentMaterial = faceMaterial;        
            glMaterialfv(drawnFace, GL_AMBIENT, faceMaterial->ambient);
			glMaterialfv(drawnFace, GL_DIFFUSE, faceMaterial->diffuse);            
			glMaterialfv(drawnFace, GL_SPECULAR, faceMaterial->specular);
			glMaterialf(drawnFace, GL_SHININESS, faceMaterial->shininess);
		}
        
		//apply texture (for now only use diffuse texture map)
		if(targetModel->currentMaterial->textureMapDi->imageData)
		{
			glEnable(GL_TEXTURE_2D);
			glBindTexture(GL_TEXTURE_2D, targetModel->currentMaterial->textureMapDi->textureID);
			textured = 1;
		}
		else
			glDisable(GL_TEXTURE_2D);

		//populate vertex array to be used in drawing
		glVertexPointer(3, GL_FLOAT, sizeof(MLRenderingVertex3D), &verts[0].position);
		//populate normal array to be used in drawing
		glNormalPointer(GL_FLOAT, sizeof(MLRenderingVertex3D), &verts[0].normal);
		
		//draw face 
		glDrawArrays(GL_TRIANGLES, 0, 3);		
		
		//draw face normals if flagged
		if(targetModel->showFaceNormalsFlag)
		{			
			//declare structure to draw normal
			GUVector3D triCenter;
			GUNormal3D faceNormal;
			GLfloat normalEnds[6];
			
			//calculate center of face
			triCenter.x = (targetModel->vertices[targetModel->faces[i].vert1].position.x + targetModel->vertices[targetModel->faces[i].vert2].position.x + targetModel->vertices[targetModel->faces[i].vert3].position.x)/3;
			triCenter.y = (targetModel->vertices[targetModel->faces[i].vert1].position.y + targetModel->vertices[targetModel->faces[i].vert2].position.y + targetModel->vertices[targetModel->faces[i].vert3].position.y)/3;
			triCenter.z = (targetModel->vertices[targetModel->faces[i].vert1].position.z + targetModel->vertices[targetModel->faces[i].vert2].position.z + targetModel->vertices[targetModel->faces[i].vert3].position.z)/3;
			
			//get normal vector
			faceNormal.x = targetModel->faces[i].normal.x;
			faceNormal.y = targetModel->faces[i].normal.y;
			faceNormal.z = targetModel->faces[i].normal.z;
			
			//create array to hold normal line to be drawn
			normalEnds[0] = triCenter.x;
			normalEnds[1] = triCenter.y;
			normalEnds[2] = triCenter.z;
			normalEnds[3] = triCenter.x+faceNormal.x;
			normalEnds[4] = triCenter.y+faceNormal.y;
			normalEnds[5] = triCenter.z+faceNormal.z;
			
			glDisable(GL_LIGHTING);
			glDisable(GL_NORMAL_ARRAY);
			glColor4f(1, 0, 0, 1);
			
			glVertexPointer(3, GL_FLOAT, 0, normalEnds);
			glDrawArrays(GL_LINES, 0, 2);
			
			glEnable(GL_NORMAL_ARRAY);
			glEnable(GL_LIGHTING);
		}
	}
	
	//draw vertex normals if flagged
	if(targetModel->showVertexNormalsFlag)
	{
		for(i = 1; i < targetModel->numVertices; i++)
		{
			GLfloat vertNormalEnds[6];
			
			glDisable(GL_LIGHTING);
			glDisable(GL_NORMAL_ARRAY);
			glColor4f(0, 1, 0, 1);
			
			//create array of start and end of normal
			vertNormalEnds[0] = vertexArray[i].position.x; 
			vertNormalEnds[1] = vertexArray[i].position.y; 
			vertNormalEnds[2] = vertexArray[i].position.z;
			vertNormalEnds[3] = vertexArray[i].position.x + vertexArray[i].normal.x; 
			vertNormalEnds[4] = vertexArray[i].position.y + vertexArray[i].normal.y; 
			vertNormalEnds[5] = vertexArray[i].position.z + vertexArray[i].normal.z;

			glVertexPointer(3, GL_FLOAT, 0, vertNormalEnds);
			glDrawArrays(GL_LINES, 0, 2);
			
			glEnable(GL_NORMAL_ARRAY);
			glEnable(GL_LIGHTING);
		}
	}
	
	//return Open GL toggled states to prior values
	if(glLightingEnabledPrior)
		glEnable(GL_LIGHTING);
	else
		glDisable(GL_LIGHTING);
	if(glNormalizeEnabledPrior)
	   glEnable(GL_NORMALIZE);
	else
		glDisable(GL_NORMALIZE);
#ifdef GL_RESCALE_NORMAL
	if(glRescaleEnabledPrior)
		glEnable(GL_RESCALE_NORMAL);
	else
		glDisable(GL_RESCALE_NORMAL);
#endif
	if(glVertexArrayEnabledPrior)
		glEnableClientState(GL_VERTEX_ARRAY);
	else
		glDisableClientState(GL_VERTEX_ARRAY);
	if(glNormalArrayEnabledPrior)
		glEnableClientState(GL_NORMAL_ARRAY);
	else
		glDisableClientState(GL_NORMAL_ARRAY);
    if(glFaceCullingEnabledPrior)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);
    glCullFace(glFaceCulledPrior);
	
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
}*/

//render model in the given color for pick testing
/*void mlModel3DDrawForPick(MLModel3D* targetModel, GLfloat transform[16], GLubyte red, GLubyte green, GLubyte blue)
{
    unsigned int i = 0;                                                     //counter
	float modelViewMatrix[16];                                              //modelview matrix
	GLboolean glLightingEnabledPrior	= glIsEnabled(GL_LIGHTING);			//is lighting enabled before drawing model
	GLboolean glNormalizeEnabledPrior	= glIsEnabled(GL_NORMALIZE);		//is normal normalizing enabled before drawing model
    #ifdef GL_RESCALE_NORMAL
	GLboolean glRescaleEnabledPrior		= glIsEnabled(GL_RESCALE_NORMAL);	//is normal rescaling enabled before drawing model
    #endif
	GLboolean glVertexArrayEnabledPrior	= glIsEnabled(GL_VERTEX_ARRAY);		//is the use of vertex arrays enabled before drawing model
	GLboolean glColorArrayEnabledPrior	= glIsEnabled(GL_COLOR_ARRAY);		//is the use of normal arrays before drawing model
    GLboolean glFaceCullingEnabledPrior = glIsEnabled(GL_CULL_FACE);        //is face culling enabled before drawing model
    GLint glFaceCulledPrior;                                                //what faces are being culled before drawing model
    GLint drawnFace;                                                        //faces (OpenGL enum) to be drawn based on what is not being automatically culled for winding order
    MLFace3D* faceArray = targetModel->faces;								//reference target models arrays of faces
	MLVertex3D* vertexArray = targetModel->vertices;						//reference target models arrays of vertices
    
	//get face culling mode before drawing
	glGetIntegerv(GL_CULL_FACE_MODE, &glFaceCulledPrior);
    
	//prepare for drawing
	glMatrixMode(GL_MODELVIEW);
	glPushMatrix();
    
	//transform modelview matrix
	if(transform != NULL)
		glMultMatrixf(transform);
	
	//get current modelview matrix
	glGetFloatv(GL_MODELVIEW_MATRIX, modelViewMatrix);
	
	//disable both normalization and rescaling of normals to be enabled later
	glDisable(GL_NORMALIZE);
#ifdef GL_RESCALE_NORMAL
	glDisable(GL_RESCALE_NORMAL);
#endif
	
#ifdef GL_RESCALE_NORMAL
	//if ModelView is uniformly scaled (sx = sy = sz), return normals to unit length by scaling by derived scale factor after transformation 
	if(modelViewMatrix[0] == modelViewMatrix[5] == modelViewMatrix[10])
	{
		glEnable(GL_RESCALE_NORMAL);
	}
	//otherwise, return normals to unit length by normalizing them (more expensive)
	else		
	{
		glEnable(GL_NORMALIZE);
	}
#else
	glEnable(GL_NORMALIZE);
#endif
    
    //set OpenGl face culling for model
    if(targetModel->cullFace == ML_MODEL_FACE_NONE)
    {
        glDisable(GL_CULL_FACE);
        drawnFace = GL_FRONT_AND_BACK;
    }
    else if(targetModel->cullFace == ML_MODEL_FACE_FRONT)
    {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_FRONT);
        drawnFace = GL_BACK;
    }
    else if(targetModel->cullFace == ML_MODEL_FACE_BACK)
    {
        glEnable(GL_CULL_FACE);
        glCullFace(GL_BACK);
        drawnFace = GL_FRONT;
    }
    else if(targetModel->cullFace == ML_MODEL_FACE_FRONT_AND_BACK)
    {
        //both faces will be culled, so nothing will be drawn
        return; 
    }
    
	//enable drawing with vertex and normal arrays
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	
    //disable lighting
    glDisable(GL_LIGHTING);
    
	//--- Draw model using vertex indices stored in each face ---// 
	for(i = 1; i < targetModel->numFaces; i++)							//start at face index 1 because 0 is empty
	{		
		//create array of vertices in face
		MLPickingVertex3D verts[3];		
        
        //set the vertex positions
		verts[0].position = vertexArray[faceArray[i].vert1].position;
		verts[1].position = vertexArray[faceArray[i].vert2].position;
		verts[2].position = vertexArray[faceArray[i].vert3].position;
        
        //set the vertex colors to the desired rendering color
        verts[0].color.red = verts[1].color.red = verts[2].color.red = red;
        verts[0].color.green = verts[1].color.green = verts[2].color.green = green;
        verts[0].color.blue = verts[1].color.blue = verts[2].color.blue = blue;
        verts[0].color.alpha = verts[1].color.alpha = verts[2].color.alpha = 255;
        
		//populate vertex array to be used in drawing
		glVertexPointer(3, GL_FLOAT, sizeof(MLPickingVertex3D), &verts[0].position);
		//populate color array to be used in drawing
		glColorPointer(4, GL_UNSIGNED_BYTE, sizeof(MLPickingVertex3D), &verts[0].color);
		//draw face 
		glDrawArrays(GL_TRIANGLES, 0, 3);		
    }
	
	//return Open GL toggled states to prior values
	if(glLightingEnabledPrior)
		glEnable(GL_LIGHTING);
	else
		glDisable(GL_LIGHTING);
	if(glNormalizeEnabledPrior)
        glEnable(GL_NORMALIZE);
	else
		glDisable(GL_NORMALIZE);
#ifdef GL_RESCALE_NORMAL
	if(glRescaleEnabledPrior)
		glEnable(GL_RESCALE_NORMAL);
	else
		glDisable(GL_RESCALE_NORMAL);
#endif
	if(glVertexArrayEnabledPrior)
		glEnableClientState(GL_VERTEX_ARRAY);
	else
		glDisableClientState(GL_VERTEX_ARRAY);
	if(glColorArrayEnabledPrior)
		glEnableClientState(GL_COLOR_ARRAY);
	else
		glDisableClientState(GL_COLOR_ARRAY);
    if(glFaceCullingEnabledPrior)
		glEnable(GL_CULL_FACE);
	else
		glDisable(GL_CULL_FACE);
    glCullFace(glFaceCulledPrior);
	
	glMatrixMode(GL_MODELVIEW);
	glPopMatrix();
    
}*/

//parse line into either vertex data, texture vertex data, face data, multiple face data (split from quad to triangles), unusable data, or end of file, return data type and populate data array
//if countOnly is 1 do not populate object only return the data type
int parseModelData(void** data, int* dataTypes, int* dataSizes, int dataCount, char* read, char* substringBuffer, int readSize, FILE* file, int isCountOnly)
{
    //declare counter
	int i = 0;
	
	//create pointers for GLushorts, GLints, and GLfloats to be given to data elements
	GLushort* ushortPointer = NULL;
	GLfloat* floatPointer = NULL;
	
	//declare delimiter trackers
	int delimiterCount;
	int* delimiterIndices = NULL;

	//fail if no space was given to store data and not just counting features
	if(data == NULL && !isCountOnly)
		return freeReadModelReturn(delimiterIndices, MODEL_UNUSABLE);
	
	//initialize data if not just counting
	if(!isCountOnly)
	{
		for(i = 0; i < dataCount; i++)
		{
			if(data[i] != read && data[i] != substringBuffer)
				free(data[i]);
			data[i] = NULL;
			dataTypes[i] = GU_NO_TYPE;
			dataSizes[i] = 0;
		}
	}

	//--- End of File ---//
	if(feof(file))
	{
		//close file
		fclose(file);
		return freeReadModelReturn(delimiterIndices, MODEL_END);
	}
	
	//read next line from file
	if(!fgets(read, readSize, file))
		return freeReadModelReturn(delimiterIndices, MODEL_UNUSABLE);

	//find the locations of ' ' in the read line for parsing
	delimiterCount = 0;
	delimiterIndices = guFindDelimiters(read, ' ', &delimiterCount, GU_DEFAULT_BUFFER_SIZE);

	//--- Parse Comment ---//
	if(read[0] == '#')
	{
		return freeReadModelReturn(delimiterIndices, MODEL_UNUSABLE);
	}

	//--- Parse Vertex ---//
	else if(read[0] == 'v' || read[0] == 'V')
	{
		//texture vertex
		if(read[1] == 't' || read[1] == 'T')
		{
			//return freeReadModelReturn(delimiterIndices, failure if not enough memory has been allocated to store data and the function is supposed to be populating data
			if(dataCount < 2 && !isCountOnly)
				return freeReadModelReturn(delimiterIndices, MODEL_UNUSABLE);
			//find xy texture coordinates one at a time
			for (i = 1; i < 3; i++)
			{
				float scanNum = 0;	//number to be found from scan
				//use delimiter to find each coordinate
				char* scan = guScanLine(read, &substringBuffer, delimiterIndices, delimiterCount, i);
				if(scan == NULL)
					return freeReadModelReturn(delimiterIndices, MODEL_UNUSABLE);
				//convert scan into a numerical format
				if(!guParseFloat(scan, &scanNum, GU_BASE_DECIMAL, GU_DEFAULT_DIGITS_MAX))
					return freeReadModelReturn(delimiterIndices, MODEL_UNUSABLE);
				//add value to data if not just counting
				if(!isCountOnly)
				{
					floatPointer = (GLfloat*)malloc(sizeof(GLfloat));
					*floatPointer = (GLfloat)scanNum;
					data[i-1] = floatPointer;
					dataTypes[i-1] = GU_FLOAT_ARRAY;
					dataSizes[i-1] = 1;
				}
			}
			return freeReadModelReturn(delimiterIndices, MODEL_TEXTURE_VERTEX);
		}
		//face vertex
		else
		{
			//return freeReadModelReturn(delimiterIndices, failure if not enough memory has been allocated to store data and the function is supposed to be populating data
			if(dataCount < 3 && !isCountOnly)
				return freeReadModelReturn(delimiterIndices, MODEL_UNUSABLE);
			//find xyz coordinates one at a time
			for (i = 1; i < 4; i++)
			{
				float scanNum = 0;	//number to be found from scan
				//use delimiter to find each coordinate
				char* scan = guScanLine(read, &substringBuffer, delimiterIndices, delimiterCount, i);
				if(scan == NULL)
					return freeReadModelReturn(delimiterIndices, MODEL_UNUSABLE);
				//convert scan into a numerical format
				if(!guParseFloat(scan, &scanNum, GU_BASE_DECIMAL, GU_DEFAULT_DIGITS_MAX))
					return freeReadModelReturn(delimiterIndices, MODEL_UNUSABLE);
				//add value to data if not just counting
				if(!isCountOnly)
				{
					floatPointer = (GLfloat*)malloc(sizeof(GLfloat));
					*floatPointer = (GLfloat)scanNum;
					data[i-1] = floatPointer;
					dataTypes[i-1] = GU_FLOAT_ARRAY;
					dataSizes[i-1] = 1;
				}
			}
			return freeReadModelReturn(delimiterIndices, MODEL_VERTEX);
		}
	}

	//--- Parse Face ---//
	else if(read[0] == 'f' || read[0] == 'F')
	{
		//declare slash count and dependent flags
		int slashes = 0;
		int textured = slashes > 0;	

		//determine how many vertices the face has according to the file
		int numVerts = delimiterCount > 5 ? 4 : 3;

		//if the number of vertice is below three the face is not usable
		if(numVerts < 3)
			return freeReadModelReturn(delimiterIndices, MODEL_UNUSABLE);

		//determine if texture coordinates are included in face
		slashes = 0;															//keep track of '/' count
		free(guFindDelimiters(read, '/', &slashes, GU_DEFAULT_BUFFER_SIZE));	//count '/' in line (do not need the actual list at this point
		slashes -= 2;															//discount beginning and termination indices
		textured = slashes > 0;													//flag as textured if any '/' appear (obj format lists faces as vertexCoor/textureCoor/normalCoor)

		if(dataCount < 8 && !isCountOnly)			//note that the data size must be enough for quad faces with textures, regardless of actual line data
			return freeReadModelReturn(delimiterIndices, MODEL_UNUSABLE);

		//find vertices one at a time
		for (i = 1; i < numVerts+1; i++)
		{
			unsigned short vertIndex = 0, texIndex = 0;
			//use delimiter to find each vertex
			char* scan = guScanLine(read, &substringBuffer, delimiterIndices, delimiterCount, i);
			if(scan == NULL)
				return freeReadModelReturn(delimiterIndices, MODEL_UNUSABLE);
			//convert scan into a numerical format
			if(!textured)							//only scan for vertex coordinate if not textured
			{
				if(!guParseUShort(scan, &vertIndex, GU_BASE_DECIMAL, GU_DEFAULT_DIGITS_MAX))
					return freeReadModelReturn(delimiterIndices, MODEL_UNUSABLE);
			}
			else									//scan for both vertex and texture coordinates if textured
			{
				int innerSlashes;                                                                           //keep track of '/' within the current scan
				int* slashesFound = guFindDelimiters(scan, '/', &innerSlashes, GU_DEFAULT_DIGITS_MAX);		//find '/' within the current scan rather than the whole line
				//retrieve vertex index
				char* slashScan = guScanLine(scan, &substringBuffer, slashesFound, delimiterCount, 0);	
				if(slashScan == NULL)
					return freeReadModelReturn(delimiterIndices, MODEL_UNUSABLE);
				if(!guParseUShort(slashScan, &vertIndex, GU_BASE_DECIMAL, GU_DEFAULT_DIGITS_MAX))
					return freeReadModelReturn(delimiterIndices, MODEL_UNUSABLE);
				//retrieve texture index
				texIndex = 0;
				slashScan = guScanLine(scan, &substringBuffer, slashesFound, delimiterCount, 1);	
				if(slashScan == NULL)
					texIndex = 0;
				if(!guParseUShort(slashScan, &texIndex, GU_BASE_DECIMAL, GU_DEFAULT_DIGITS_MAX))
					texIndex = 0;
				free(slashesFound);
			}
			//add value to data if not just counting
			if(!isCountOnly)
			{
				ushortPointer = (GLushort*)malloc(sizeof(GLushort));
				*ushortPointer = vertIndex;
				data[i-1] = ushortPointer;
				dataTypes[i-1] = GU_UNSIGNED_SHORT_ARRAY;
				dataSizes[i-1] = 1;
				ushortPointer = (GLushort*)malloc(sizeof(GLushort));
				*ushortPointer = (GLushort)texIndex;
				data[i-1+4] = ushortPointer;
				dataTypes[i-1+4] = GU_UNSIGNED_SHORT_ARRAY;
				dataSizes[i-1+4] = 1;
			}
		}

		//indicate single face if only 3 vertices appear or indicate two faces if four vertices appear
		return freeReadModelReturn(delimiterIndices, numVerts == 3 ? MODEL_FACE : MODEL_SPLIT_FACE);

	}

	//--- Parse Material Library ---//
	if(read[0] == 'm' || read[0] == 'M')
	{
		//determine if first set of characters is correct
		if((guCompare(guScanLine(read, &substringBuffer, delimiterIndices, delimiterCount, 0), "mtllib", 0, GU_DEFAULT_BUFFER_SIZE) == 0))
		{
			if(!isCountOnly)
			{
				int startPosition = 7;			//start substring at start position, 7 is used be cause 'mtllib ' is 7 characters long
				int filenameSize = 0;

				//need to set up return freeReadModelReturn(delimiterIndices, data, size, and type
				data[0] = guScanLineRaw(read, &substringBuffer, startPosition, GU_STRING_END, &filenameSize);	//read the rest of the line and use that as output data				
				dataTypes[0] = GU_CHAR_ARRAY;
				dataSizes[0] = filenameSize;
			}
			return freeReadModelReturn(delimiterIndices, MODEL_MAT_LIB);
		}	
	}

	//parse material name
	if(read[0] == 'u' || read[0] == 'U')
	{
		//determine if first set of characters is correct
		if((guCompare(guScanLine(read, &substringBuffer, delimiterIndices, delimiterCount, 0), "usemtl", 0, GU_DEFAULT_BUFFER_SIZE) == 0))
		{
			if(!isCountOnly)
			{
				int startPosition = 7;			//start substring at start position, 7 is used be cause 'usemtl ' is 7 characters long
				int filenameSize = 0;

				//read the rest of the line and use that as output data
				data[0] = guScanLineRaw(read, &substringBuffer, startPosition, GU_STRING_END, &filenameSize);	
				dataTypes[0] = GU_CHAR_ARRAY;
				dataSizes[0] = filenameSize;
			}
			return freeReadModelReturn(delimiterIndices, MODEL_MAT_NAME);
		}
	}

	//--- Unusable ---//
	return freeReadModelReturn(delimiterIndices, MODEL_UNUSABLE);
}

//deallocate buffers allocated during parsing
int freeReadModelReturn(int* delimiterIndices, int out)
{
	free(delimiterIndices);
	return out;
}

//recalculate the surface normal at a vertex by averaging its current normal and the added normal
//note that the averages are weighted to account for multiple normals already being averaged
void recalcVertexNormal(MLVertex3D* vert, MLFace3D const* modelFaceList, int addedFaceIndex)
{
	//declare counter
    int i = 0;
    //declare face and normal of face to be used in calculation
	MLFace3D const* addedFace = &modelFaceList[addedFaceIndex];
	GUNormal3D addedNormal = addedFace->normal;
	
	//if either the face to add, the face's normal, or the list of faces does not exist, return
	if(!(addedFace && modelFaceList))
		return;
	
	//if the vertex says that it does not define any faces, set its normal to 0 and return
	if(vert->numFacesDefined == 0)
	{
		vert->normal.x = vert->normal.y = vert->normal.z = 0;
		return;
	}
	
	//if the list of averaged normals has not yet been allocated, allocate enough memory to point to each defined face's normal
	if(!vert->facesAveraged)
	{
		vert->numFacesAveraged = 0;
		vert->facesAveraged = (int*)malloc(sizeof(int)*vert->numFacesDefined);
		//initally refer to face default face (never used)
		for(i = 0; i < vert->numFacesDefined; i++)
			vert->facesAveraged[i] = 0;
	}
	
	//if the specified normal is 0, ignore it
	if(addedNormal.x == 0 && addedNormal.y == 0 && addedNormal.z == 0)
		return;
	
	//if the specified normal is parallel to any previously averaged normal, consider ignoring it
	//this avoids averaging duplicates caused by tessellation, which produces faces that were originally part of the same complex face
	for(i = 0; i < vert->numFacesAveraged && vert->facesAveraged[i] != 0; i++)
	{
		MLFace3D checkFace = modelFaceList[vert->facesAveraged[i]];
		float dot = guDotProduct(addedNormal, checkFace.normal);
		//assume normals are of unit length and use dot product to check for parallel normals, allow for a small error tolerance due to round-off
		if((dot > 0 ? dot : dot*-1) >= 1 - NORMAL_EPSILON)
		{
			//track number of verticess shared between faces
			int sharedVerts = 0;
			if((addedFace->vert1 == checkFace.vert1) || (addedFace->vert1 == checkFace.vert2) || (addedFace->vert1 == checkFace.vert3))
				sharedVerts++;
			if((addedFace->vert2 == checkFace.vert1) || (addedFace->vert2 == checkFace.vert2) || (addedFace->vert2 == checkFace.vert3))
				sharedVerts++;
			if((addedFace->vert3 == checkFace.vert1) || (addedFace->vert3 == checkFace.vert2) || (addedFace->vert3 == checkFace.vert3))
				sharedVerts++;
			
			//if the faces share more than one vertex, they could be combined into one complex face so ignore the new face
			if(sharedVerts > 1)
				return;
		}
	}
	
	//sum surface normal at vertex with added normal (multiply current normal by number of faces to achieve weighting)
	vert->normal.x = (vert->normal.x*vert->numFacesAveraged) + addedNormal.x;
	vert->normal.y = (vert->normal.y*vert->numFacesAveraged) + addedNormal.y;
	vert->normal.z = (vert->normal.z*vert->numFacesAveraged) + addedNormal.z;
	//add face to list
	vert->facesAveraged[vert->numFacesAveraged] = addedFaceIndex;
	//increment number of normals averaged
	vert->numFacesAveraged++;
	//calculate new average based on new number of faces
	vert->normal.x /= vert->numFacesAveraged;
	vert->normal.y /= vert->numFacesAveraged;
	vert->normal.z /= vert->numFacesAveraged;
}

//enable flag target model to display face normals
void mlModel3DShowFaceNormals(MLModel3D* targetModel)
{
	if(targetModel)
		targetModel->showFaceNormalsFlag = 1;
}
//disable flag target model to display face normals
void mlModel3DHideFaceNormals(MLModel3D* targetModel)
{
	if(targetModel)
		targetModel->showFaceNormalsFlag = 0;
}
//enable flag target model to display vertex normals
void mlModel3DShowVertexNormals(MLModel3D* targetModel)
{
	if(targetModel)
		targetModel->showVertexNormalsFlag = 1;
}
//disable flag target model to display vertex normals
void mlModel3DHideVertexNormals(MLModel3D* targetModel)
{
	if(targetModel)
		targetModel->showVertexNormalsFlag = 0;
}

//set which faces (front and/or back) on target model should be culled by OpenGL
void mlModel3DSetFaceCulling(MLModel3D* targetModel, int enumModelFace)
{
    targetModel->cullFace = enumModelFace;
}

//accessors
MLVertex3D const* mlModel3DGetVertex(MLModel3D const* targetModel, unsigned int index)						{	return &targetModel->vertices[index];			}
unsigned int mlModel3DGetVertexCount(MLModel3D const* targetModel)											{	return targetModel->numVertices;				}
MLFace3D const* mlModel3DGetFace(MLModel3D const* targetModel, unsigned int index)							{	return &targetModel->faces[index];				}
unsigned int mlModel3DGetFaceCount(MLModel3D const* targetModel)											{	return targetModel->numFaces;					}
MLTexelXY const* mlModel3DGetTextureVertex(MLModel3D const* targetModel, unsigned int index)				{	return &targetModel->textureVertices[index];	}
unsigned int mlModel3DGetTextureVertexCount(MLModel3D const* targetModel)									{	return targetModel->numTextureVertices;			}
MLModelMaterial const* mlModel3DGetMaterial(MLModel3D const* targetModel, unsigned int index)				{	return targetModel->materials[index];			}
unsigned int mlModel3DGetMaterialCount(MLModel3D const* targetModel)										{	return targetModel->numMaterials;				}
GLushort mlFace3DGetVertex1(MLFace3D const* targetFace)														{	return targetFace->vert1;						}
GLushort mlFace3DGetVertex2(MLFace3D const* targetFace)														{	return targetFace->vert2;						}
GLushort mlFace3DGetVertex3(MLFace3D const* targetFace)														{	return targetFace->vert3;						}
GLushort mlFace3DGetTextureVertex1(MLFace3D const* targetFace)												{	return targetFace->texVert1;					}
GLushort mlFace3DGetTextureVertex2(MLFace3D const* targetFace)												{	return targetFace->texVert2;					}
GLushort mlFace3DGetTextureVertex3(MLFace3D const* targetFace)												{	return targetFace->texVert3;					}
GUNormal3D mlFace3DGetNormal(MLFace3D const* targetFace)													{	return targetFace->normal;						}
GLushort mlFace3DGetMaterial(MLFace3D const* targetFace)													{	return targetFace->materialIndex;				}
GUPoint3D mlVertex3DGetPosition(MLVertex3D const* targetVertex)												{	return targetVertex->position;					}
GUNormal3D mlVertex3DGetNormal(MLVertex3D const* targetVertex)												{	return targetVertex->normal;					}
GUPoint2D mlTexelXYGetPosition(MLTexelXY const* targetTexel)												{	return targetTexel->position;					}
