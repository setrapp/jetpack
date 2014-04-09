#include "MLMaterialLoader.h"

int countMaterialsMTL(char* filename);
MLModelMaterial** loadMaterialsMTL(char* filename, int matsCounted, int* defaultDefined);
int parseMaterialData(void** data, int* dataTypes, int* dataSizes, int dataCount, char* read, char* substringBuffer, int readSize, FILE* file, int isCountOnly_bool);
int freeReadMaterialReturn(int* delimiterIndices, int out);

//enumeration of material property types and data read types
typedef enum
{
	MAT_UNUSABLE,				//data is unusable in defining the material
	MAT_END,					//the end of the material definition
	MAT_NAME,					//name of material being read
	MAT_AMBIENT,				//multiplier of ambient light reflected
	MAT_DIFFUSE,				//multiplier of diffuse light reflected
	MAT_SPECULAR,				//multiplier of specular light reflected
	MAT_SHININESS,				//intensity of specular highlights
	MAT_REFRACTION,				//index of refraction (optical density)
	MAT_ALPHA,					//opacity of material
	MAT_DECAL,					//deform of surface to create roughness
	MAT_ANTI_ALIAS,				//anti-aliasing of material
	MAT_TEXTURE_AMBIENT,		//texure map of ambient light reflection (mulitplied by MAT_AMBIENT)
	MAT_TEXTURE_DIFFUSE,		//texure map of diffuse light reflection (mulitplied by MAT_DIFFUSE)
	MAT_TEXTURE_SPECULAR,		//texure map of specular light reflection (mulitplied by MAT_SPECULAR)
	MAT_TEXTURE_SHININESS,		//texure map of specular highlight intensity (mulitplied by MAT_SHININESS)
	MAT_TEXTURE_REFRACTION,		//texure map of optical density (mulitplied by MAT_REFRACTION)
	MAT_TEXTURE_ALPHA,			//texure map of opacity (mulitplied by MAT_ALPHA)
	MAT_BUMP,					//bump map of material
	MAT_ILLUMINATION,			//illumination model used by material
	MAT_EXPORTER				//software that was used to export material library
}MaterialProperties;

//construct material with default values
MLModelMaterial* MLModelMaterialCreate()
{
    MLModelMaterial* newMaterial = (MLModelMaterial*)malloc(sizeof(MLModelMaterial));
    newMaterial->name = NULL;
    /*newMaterial->textureMapAm = TLTextureImageCreate();
    newMaterial->textureMapDi = TLTextureImageCreate();
    newMaterial->textureMapSp = TLTextureImageCreate();
    newMaterial->textureMapAl = TLTextureImageCreate();
    newMaterial->textureMapSh = TLTextureImageCreate();
    newMaterial->textureMapRe = TLTextureImageCreate();
    newMaterial->bumpMap = NULL;*/
    newMaterial->ambient[0] = newMaterial->ambient[1] = newMaterial->ambient[2] = newMaterial->ambient[3] = 1.0f;
	newMaterial->diffuse[0] = newMaterial->diffuse[1] = newMaterial->diffuse[2] = newMaterial->diffuse[3] = 1.0f;
	newMaterial->specular[0] = newMaterial->specular[1] = newMaterial->specular[2] = newMaterial->specular[3] = 1.0f;
    newMaterial->alpha = 1.0f;
    newMaterial->shininess = 0.0f;
    newMaterial->density = 1.0f;
    newMaterial->illuminationModel = ML_COLOR_WITH_AMBIENT;
    return newMaterial;
}

//dellocate texture from memory
void MLModelMaterialDelete(MLModelMaterial* deletee)
{
    if(!deletee)
        return;
    free(deletee->name);
    /*TLTextureImageDelete(deletee->textureMapAm);
    TLTextureImageDelete(deletee->textureMapDi);
    TLTextureImageDelete(deletee->textureMapSp);
    TLTextureImageDelete(deletee->textureMapAl);
    TLTextureImageDelete(deletee->textureMapSh);
    TLTextureImageDelete(deletee->textureMapRe);
    TLTextureImageDelete(deletee->bumpMap);*/
    free(deletee->ambient);
    free(deletee->diffuse);
    free(deletee->specular);
    free(deletee);
}

//load materials from list of material libraries and return a final count
//note that the default material will be overwritten every time it is defined
MLModelMaterial** mlModelMaterialLibsLoadMTL(char** matLibs, int matLibCount, unsigned int* numMatsOut)
{
	int i = 0;                                                      //counter
	int* perLibCounts = (int*)malloc(sizeof(int)*matLibCount);		//count of materials in each library
	unsigned int matCount = 1;										//total count of materials, including a single default
	MLModelMaterial** mats;											//list of pointers to materials loaded
	unsigned int matIndex = 1;										//index in list of material to be added (starts at 1 to allow for default)

	//count non-default materials
	for(i = 0; i < matLibCount; i++)
	{
		perLibCounts[i] = countMaterialsMTL(matLibs[i]);
		matCount += perLibCounts[i];
	}

	//--- Load Materials ---//
	mats = (MLModelMaterial**)malloc(sizeof(MLModelMaterial*)*matCount);
	mats[0] = MLModelMaterialCreate();

	//add materials from each library
	for(i = 0; i < matLibCount; i++)
	{
		int j = 0;

		//create a temporary list of materials from the current library and track if default was defined
		int defaultDefined = 0;
		MLModelMaterial** libMats = loadMaterialsMTL(matLibs[i], perLibCounts[i]+1, &defaultDefined);		//add an extra index temporarily for the possibility of a default material

		//add non-default materials from library to the comprehensive list
		for(j = 1; j < perLibCounts[i]+1; j++)		//add one to count to account for default material not being counted by library loader
		{
			mats[matIndex] = libMats[j];
			matIndex++;
		}
		//if the current library contained a specific definition of the default material overwrite the current
		if(defaultDefined)
		{
			free(mats[0]);
			mats[0] = libMats[0];
		}
	}

	//output number of materials found
	if(numMatsOut)
		*numMatsOut = matCount;

	return mats;
}

//count number of materials specified in library do not include default material in count
int countMaterialsMTL(char* filename)
{
	//declare counter
    int i = 0;

	//--- Declare data reading varibles (define what can be) ---//
	int readSize = GU_DEFAULT_BUFFER_SIZE;	//reading max length
	char* read;                             //character buffer for reading file
	char* substringBuffer;					//character buffer for holding substrings when parsing
	void** data;							//reading max length
	int* dataTypes;							//type of data
	int* dataSizes;							//elements of data
	int maxDataCount = 8;					//max elements of data
	int matsCounted = 0;					//number of materials counted in file if not specified
	int matCountGiven = 0;					//number of materials said to be in file (may not be available)
	
	//--- Declare structures that will be used in creating Material ---//
	int propertyType;				//type of material property being read

	//attempt to open file
	FILE* mtlFile;
	if(!guAttemptFileOpen(&mtlFile, filename, ".mtl", GU_DEFAULT_BUFFER_SIZE, 3))
		return 0;

	//allocate memory for temporary data storage
	data = (void**)malloc(sizeof(void**) * maxDataCount);
	dataTypes = (int*)malloc(sizeof(int) * maxDataCount);
	dataSizes = (int*)malloc(sizeof(int) * maxDataCount);
	for(i = 0; i < maxDataCount; i++)
		data[i] = (void*)malloc(sizeof(void*));

	//--- Parse through file to find number of of materials included ---//	
	//setup stream reading buffer
	readSize = 256;
	read =(char*)malloc(sizeof(char)*readSize);

	//allocate buffer to hold substrings
	substringBuffer = (char*)malloc(sizeof(char) * 256);

	while((propertyType = parseMaterialData(data, dataTypes, dataSizes, maxDataCount, read, substringBuffer, readSize, mtlFile, 1)) != MAT_END && !matCountGiven)
	{
		switch(propertyType)
		{
            case MAT_NAME:			//name of new material found
                if((guCompare((char*)data[0], "(null)", 0, GU_DEFAULT_BUFFER_SIZE) != 0) && (guCompare((char*)data[0], "Material", 0, GU_DEFAULT_BUFFER_SIZE) != 0))		//do not count default material
                    matsCounted++;
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
	
	return matsCounted;
}

//load specified number of materials from a file
MLModelMaterial** loadMaterialsMTL(char* filename, int matsCounted, int* defaultDefined)
{
	/* Specifications:
	 * newmtl		name of material about to be defined
	 * Ka:			ambient rgb
	 * Ka:			diffuse rgb
	 * Ka:			specular rgb
	 * d/Tr:		Alpha
	 * Ns:			Shininess
	 * Ni:			Optical Density (index of refraction)
	 * illum:		Illumination Model
	 * map_Ka:		Ambient texture map filename
	 * map_Kd:		Diffuse texture map filename
	 * map_Ks:		Specular texture map filename
	 * map_d/Tr:	Alpha texture map filename
	 * map_bump:	Bump map filename
	 */
    
    //declare counter
	int i = 0;

	//retrieve file path for texture loading
	//char* filePath = extractFilePath(filename, NULL, DIRECTORY_DELIMITER, DEFAULT_BUFFER_SIZE);

	//--- Declare data reading varibles (define what can be) ---//
	int readSize = GU_DEFAULT_BUFFER_SIZE;	//reading max length
	char* read;                             //character buffer for reading file
	char* substringBuffer;					//character buffer for holding substrings when parsing
	void** data;                            //data to be used
	int* dataTypes;                         //type of data
	int* dataSizes;                         //elements of data
	int maxDataCount = 8;                   //max elements of data
	int matsCountedOnLoad = 0;              //number of non-default materials counted during this load operation
	int curMat = 0;                         //index of current material being defined
	int propertyType;                       //type of material property being read

	//--- Declare structures that will used in creating material ---//
	MLModelMaterial** mats;				//array of materials found in library
	int materialLibraryExporter;		//software that exported this library


	//attempt to open file
	FILE* mtlFile;
	if(!guAttemptFileOpen(&mtlFile, filename, ".mtl", GU_DEFAULT_BUFFER_SIZE, 3))
		return NULL;

	//allocate memory for temporary data storage
	data = (void**)malloc(sizeof(void**) * maxDataCount);
	dataTypes = (int*)malloc(sizeof(int) * maxDataCount);
	dataSizes = (int*)malloc(sizeof(int) * maxDataCount);
	for(i = 0; i < maxDataCount; i++)
		data[i] = (void*)malloc(sizeof(void*));

	//--- Parse through file to find number of of materials included ---//
	//allocate memory for materials
	mats = (MLModelMaterial**)malloc(sizeof(MLModelMaterial*) * matsCounted);

	//flag that the default material has not been specifically defined in this library
	*defaultDefined = 0;			

	//type of material property being read
	propertyType = MAT_UNUSABLE;
	
	//allocate buffer for reading
	read = (char*)malloc(sizeof(char)*readSize);
	
	//allocate buffer to hold substrings
	substringBuffer = (char*)malloc(sizeof(char) * 256);

	//parse through each line
	while((propertyType = parseMaterialData(data, dataTypes, dataSizes, maxDataCount, read, substringBuffer, readSize, mtlFile, 0)) != MAT_END)
	{
		switch(propertyType)
		{
            case MAT_NAME:					//material name
                if((guCompare((char*)data[0], "(null)", 0, GU_DEFAULT_BUFFER_SIZE) == 0) || 
                   (guCompare((char*)data[0], "Material", 0, GU_DEFAULT_BUFFER_SIZE) != 0))		//begin defining non-default material
                {
                    curMat = matsCountedOnLoad+1;
                    matsCountedOnLoad++;
                    mats[curMat] = MLModelMaterialCreate();
                    if(mats[curMat]->name)
                        free(mats[curMat]->name);
                    mats[curMat]->name = guCopyString((char*)data[0], NULL, GU_DEFAULT_BUFFER_SIZE);
                }
                else																		//begin defining default material
                {
                    mats[0] = MLModelMaterialCreate();
                    curMat = 0;
                    *defaultDefined = 1;													//flag that the default material has been specifically defined in this library
                }
                break;
            case MAT_AMBIENT:				//ambient lighting component
                mats[curMat]->ambient[0] = *(float*)data[0];
                mats[curMat]->ambient[1] = *(float*)data[1];
                mats[curMat]->ambient[2] = *(float*)data[2];
                break;
            case MAT_DIFFUSE:				//diffuse lighting component
                mats[curMat]->diffuse[0] = *(float*)data[0];
                mats[curMat]->diffuse[1] = *(float*)data[1];
                mats[curMat]->diffuse[2] = *(float*)data[2];
                break;
            case MAT_SPECULAR:				//specular lighting component
                mats[curMat]->specular[0] = *(float*)data[0];
                mats[curMat]->specular[1] = *(float*)data[1];
                mats[curMat]->specular[2] = *(float*)data[2];
                break;
            case MAT_SHININESS:				//specular highlight intensity (MTL specifies 0 - 1000, OpenGl wants 0 - 128)
                mats[curMat]->shininess = *(float*)data[0] * 128.0f/1000.0f;
                break;
            case MAT_REFRACTION:			//index of refraction (optical density)
                mats[curMat]->density = *(float*)data[0];
                break;
            case MAT_ALPHA:					//opacity of material
                mats[curMat]->alpha = *(float*)data[0];
                mats[curMat]->ambient[3] = mats[curMat]->alpha;
                mats[curMat]->diffuse[3] = mats[curMat]->alpha;
                mats[curMat]->specular[3] = mats[curMat]->alpha;
                break;
            /*case MAT_TEXTURE_AMBIENT:		//texture map of ambient lighting component
                if(mats[curMat]->textureMapAm->filename)
                    free(mats[curMat]->textureMapAm->filename);
                mats[curMat]->textureMapAm->filename = guCopyString((char*)data[0], NULL, GU_DEFAULT_BUFFER_SIZE);
                break;
            case MAT_TEXTURE_DIFFUSE:		//texture map of diffuse lighting component
                if(mats[curMat]->textureMapDi->filename)
                    free(mats[curMat]->textureMapDi->filename);
                mats[curMat]->textureMapDi->filename = guCopyString((char*)data[0], NULL, GU_DEFAULT_BUFFER_SIZE);
                break;
            case MAT_TEXTURE_SPECULAR:		//texture map of specular lighting component
                if(mats[curMat]->textureMapSp->filename)
                    free(mats[curMat]->textureMapSp->filename);
                mats[curMat]->textureMapSp->filename = guCopyString((char*)data[0], NULL, GU_DEFAULT_BUFFER_SIZE);
                break;
            case MAT_TEXTURE_ALPHA:			//texture map of opacity
                if(mats[curMat]->textureMapAl->filename)
                    free(mats[curMat]->textureMapAl->filename);
                mats[curMat]->textureMapAl->filename = guCopyString((char*)data[0], NULL, GU_DEFAULT_BUFFER_SIZE);
                break;
            case MAT_TEXTURE_SHININESS:		//texture map of shininess
                if(mats[curMat]->textureMapSh->filename)
                    free(mats[curMat]->textureMapSh->filename);
                mats[curMat]->textureMapSh->filename = guCopyString((char*)data[0], NULL, GU_DEFAULT_BUFFER_SIZE);
                break;
            case MAT_TEXTURE_REFRACTION:	//texture map of optical density
                if(mats[curMat]->textureMapRe->filename)
                    free(mats[curMat]->textureMapRe->filename);
                mats[curMat]->textureMapRe->filename = guCopyString((char*)data[0], NULL, GU_DEFAULT_BUFFER_SIZE);
                break;
            case MAT_BUMP:					//texture map of optical density
                if(mats[curMat]->bumpMap->filename)
                    free(mats[curMat]->bumpMap->filename);
                mats[curMat]->bumpMap->filename = guCopyString((char*)data[0], NULL, GU_DEFAULT_BUFFER_SIZE);
                break;*/
            case MAT_ILLUMINATION:			//texture map of optical density
                mats[curMat]->illuminationModel = mats[curMat]->illuminationModel | *(short*)data[0];
                break;
            case MAT_EXPORTER:				//texture map of optical density
                materialLibraryExporter = *(int*)data[0];
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

	//define default material if it has not been already
	if(!(*defaultDefined))
		mats[0] = NULL;

	return mats;
}

//parse line into either vertex data, texture vertex data, face data, multiple face data (split from quad to triangles), unusable data, or end of file, return data type and populate data array
//if countOnly is 1 do not populate object only return the data type
int parseMaterialData(void** data, int* dataTypes, int* dataSizes, int dataCount, char* read, char* substringBuffer, int readSize, FILE* file, int isCountOnly)
{
	//declare counter
    int i = 0;
	
	//create pointers for GLushorts, GLints, and GLfloats to be given to data elements
	GLshort* shortPointer = NULL;
	GLint* intPointer = NULL;
	GLfloat* floatPointer = NULL;

	//declare delimiter trackers
	int delimiterCount;
	int* delimiterIndices;
	
	//fail if no space was given to store data and not just counting features
	if(data == NULL && !isCountOnly)
		return freeReadMaterialReturn(NULL,  MAT_UNUSABLE);
	
	//initialize data if not just counting
	if(!isCountOnly)
	{
		for(i = 0; i < dataCount; i++)
		{
			data[i] = NULL;
			dataTypes[i] = GU_NO_TYPE;
			dataSizes[i] = 0;
		}
	}
	
	//--- End of File ---//
	if(feof(file))
	{
		//close
		fclose(file);
		return freeReadMaterialReturn(NULL,  MAT_END);
	}

	//read next line from file
	if(!fgets(read, readSize, file))
		return freeReadMaterialReturn(NULL,  MAT_UNUSABLE);
		

	//find the locations of ' ' in the read line for parsing
	delimiterCount = 0;
	delimiterIndices = guFindDelimiters(read, ' ', &delimiterCount, GU_DEFAULT_BUFFER_SIZE);

	//--- Parse comment ---//
	if(read[0] == '#')
	{
		if(read[1] == ' ')		//check in case a ' ' is present after '#'
		{
			if(!isCountOnly &&  delimiterCount > 2 && guCompare(guScanLine(read, &substringBuffer, delimiterIndices, delimiterCount, 1), "Blender3D", 0, GU_DEFAULT_BUFFER_SIZE) == 0)
			{
				intPointer = (GLint*)malloc(sizeof(GLint));
				*intPointer = (GLint)(ML_MAT_EXPORTER_BLENDER);
				data[0] = intPointer;
				dataTypes[0] = GU_INT_ARRAY;
				dataSizes[0] = 1;
				return freeReadMaterialReturn(delimiterIndices,  MAT_EXPORTER);
			}
		}
		else					//check in case a ' ' is not present after '#'
		{
			if(!isCountOnly &&  delimiterCount > 1 && guCompare(guScanLine(read, &substringBuffer, delimiterIndices, delimiterCount, 1), "Blender3D", 0, GU_DEFAULT_BUFFER_SIZE) == 0)
			{
				intPointer = (GLint*)malloc(sizeof(GLint));
				*intPointer = (GLint)(ML_MAT_EXPORTER_BLENDER);
				data[0] = intPointer;
				dataTypes[0] = GU_INT_ARRAY;
				dataSizes[0] = 1;
				return freeReadMaterialReturn(delimiterIndices,  MAT_EXPORTER);
			}
		}
		return freeReadMaterialReturn(delimiterIndices,  MAT_UNUSABLE);
	}

	//--- Parse Color Vectors of Lighting Components ---//
	else if(read[0] == 'k' || read[0] == 'K')
	{
		//this type of data is not useful when counting
		if(isCountOnly)
			return freeReadMaterialReturn(delimiterIndices,  MAT_UNUSABLE);
		
		//return freeReadMaterialReturn(delimiterIndices,  failure if not enough memory has been allocated to store data and the function is supposed to be populating data
		if(dataCount < 1 && !isCountOnly)
			return freeReadMaterialReturn(delimiterIndices,  MAT_UNUSABLE);
		//find RGB values one at a time
		for(i = 1; i < 4; i++)
		{
			float scanNum = 0;	//number to be found from scan
			//use delimiter to find each color value
			char* scan = guScanLine(read, &substringBuffer, delimiterIndices, delimiterCount, i);
			if(scan == NULL)
				return freeReadMaterialReturn(delimiterIndices,  MAT_UNUSABLE);
			//convert scan into a numerical format
			if(!guParseFloat(scan, &scanNum, GU_BASE_DECIMAL, GU_DEFAULT_DIGITS_MAX))
				return freeReadMaterialReturn(delimiterIndices,   MAT_UNUSABLE);
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
		//use as ambient lighting component
		if(read[1] == 'a' || read[1] == 'A')
			return freeReadMaterialReturn(delimiterIndices,  MAT_AMBIENT);
		//use as diffuse lighting component
		else if(read[1] == 'd' || read[1] == 'D')
			return freeReadMaterialReturn(delimiterIndices,  MAT_DIFFUSE);
		//use as specular lighting component
		else if(read[1] == 's' || read[1] == 'S')
			return freeReadMaterialReturn(delimiterIndices,  MAT_SPECULAR);
	}

	//--- Parse Scalar Multipliers of Lighting Components OR New Material Beginning ---//
	else if(read[0] == 'n' || read[0] == 'N')
	{
		//check if this is the beginning of a new material
		if(guCompare(guScanLine(read, &substringBuffer, delimiterIndices, delimiterCount, 0), "newmtl", 0, GU_DEFAULT_BUFFER_SIZE) == 0)
		{
			int startPosition = 7;			//start substring at start position, 7 is used be cause 'newmtl ' is 7 characters long
			int materialNameSize = 0;

			data[0] = guScanLineRaw(read, &substringBuffer, startPosition, GU_STRING_END, &materialNameSize);
			dataTypes[0] = GU_CHAR_ARRAY;
			dataSizes[0] = materialNameSize;
			return freeReadMaterialReturn(delimiterIndices,  MAT_NAME);
		}
		//otherwise assume it is related to shininess or index of refraction
		else
		{
			char* scan;			//scan to be made
			float scanNum = 0;	//number to be found from scan

			//this type of data is not useful when counting
			if(isCountOnly)
				return freeReadMaterialReturn(delimiterIndices,  MAT_UNUSABLE);

			//return freeReadMaterialReturn(delimiterIndices,  failure if not enough memory has been allocated to store data and the function is supposed to be populating data
			if(dataCount < 1 && !isCountOnly)
				return freeReadMaterialReturn(delimiterIndices,  MAT_UNUSABLE);
			//use delimiter to find shininess
			scan = guScanLine(read, &substringBuffer, delimiterIndices, delimiterCount, 1);
			if(scan == NULL)
				return freeReadMaterialReturn(delimiterIndices,  MAT_UNUSABLE);
			//convert scan into a numerical format
			if(!guParseFloat(scan, &scanNum, GU_BASE_DECIMAL, GU_DEFAULT_DIGITS_MAX))
				return freeReadMaterialReturn(delimiterIndices,  MAT_UNUSABLE);
			//add value to data if not just counting
			if(!isCountOnly)
			{
				floatPointer = (GLfloat*)malloc(sizeof(GLfloat));
				*floatPointer = (GLfloat)scanNum;
				data[0] = floatPointer;
				dataTypes[0] = GU_FLOAT_ARRAY;
				dataSizes[0] = 1;
			}

			//use as shininess (intensity of highlights)
			if(read[1] == 's' || read[1] == 'S')
				return freeReadMaterialReturn(delimiterIndices,  MAT_SHININESS);
			//use as optical density (index of refraction)
			else if(read[1] == 'i' || read[1] == 'I')
				return freeReadMaterialReturn(delimiterIndices,  MAT_REFRACTION);
		}
		
	}

	//--- Parse Opacity (Alpha) ---//
	else if(read[0] == 'd' || read[0] == 'D' || read[0] == 't' || read[0] == 'T')		//'d' and 'Tr' are synonymous
	{
		//this type of data is not useful when counting
		if(isCountOnly)
			return freeReadMaterialReturn(delimiterIndices,  MAT_UNUSABLE);

		//ensure that correct correct specifier is given
		if(((read[0] == 'd' || read[0] == 'D') && !(read[1] == 'r' || read[1] == 'R')) || (read[1] == 'r' || read[1] == 'R'))		//an 'r' is expected to follow 'T' but 'd' is enough of a specifier by itself
		{
			char* scan;			//scan to be made
			float scanNum = 0;	//number to be found from scan
			//return freeReadMaterialReturn(delimiterIndices,  failure if not enough memory has been allocated to store data and the function is supposed to be populating data
			if(dataCount < 1 && !isCountOnly)
				return freeReadMaterialReturn(delimiterIndices,  MAT_UNUSABLE);
			//use delimiter to find opacity
			scan = guScanLine(read, &substringBuffer, delimiterIndices, delimiterCount, 1);
			if(scan == NULL)
				return freeReadMaterialReturn(delimiterIndices,  MAT_UNUSABLE);
			//convert scan into a numerical format
			if(!guParseFloat(scan, &scanNum, GU_BASE_DECIMAL, GU_DEFAULT_DIGITS_MAX))
				return freeReadMaterialReturn(delimiterIndices,  MAT_UNUSABLE);
			//add value to data if not just counting
			if(!isCountOnly)
			{
				floatPointer = (GLfloat*)malloc(sizeof(GLfloat));
				*floatPointer = (GLfloat)scanNum;
				data[0] = floatPointer;
				dataTypes[0] = GU_FLOAT_ARRAY;
				dataSizes[0] = 1;
			}
			return freeReadMaterialReturn(delimiterIndices,  MAT_ALPHA);
		}
	}

	//--- Parse Illumination Model---//
	else if(read[0] == 'i' || read[0] == 'I')
	{
		//this type of data is not useful when counting
			if(isCountOnly)
				return freeReadMaterialReturn(delimiterIndices,  MAT_UNUSABLE);

		//ensure that correct correct specifier is given
		if(guCompare(guScanLine(read, &substringBuffer, delimiterIndices, delimiterCount, 0), "illum", 0, GU_DEFAULT_DIGITS_MAX) == 0)
		{
			char* scan;			//scan to be made
			short scanNum = 0;	//number to be found from scan
			//return freeReadMaterialReturn(delimiterIndices,  failure if not enough memory has been allocated to store data and the function is supposed to be populating data
			if(dataCount < 1 && !isCountOnly)
				return freeReadMaterialReturn(delimiterIndices,  MAT_UNUSABLE);
			//use delimiter to find opacity
			scan = guScanLine(read, &substringBuffer, delimiterIndices, delimiterCount, 1);
			if(scan == NULL)
				return freeReadMaterialReturn(delimiterIndices,  MAT_UNUSABLE);
			//convert scan into a numerical format
			if(!guParseShort(scan, &scanNum, GU_BASE_DECIMAL, GU_DEFAULT_DIGITS_MAX))
				return freeReadMaterialReturn(delimiterIndices,  MAT_UNUSABLE);
			//add value to data if not just counting
			if(!isCountOnly)
			{
				shortPointer = (GLshort*)malloc(sizeof(GLshort));
				*shortPointer = (GLshort)(1 << scanNum);
				//Illumination model will be stored as a short whose bits correspond to each model definition
				data[0] = shortPointer;								
				dataTypes[0] = GU_SHORT_ARRAY;
				dataSizes[0] = 1;
			}
			return freeReadMaterialReturn(delimiterIndices,  MAT_ILLUMINATION);
		}
	}

	//--- Parse Texture Map Filename ---//
	else if(read[0] == 'm' || read[0] == 'M')
	{
		//this type of data is not useful when counting
		if(isCountOnly)
			return freeReadMaterialReturn(delimiterIndices,  MAT_UNUSABLE);

		//if the line begins with "map_" then parse a map filename
		if(guCompare(guScanLineRaw(read, &substringBuffer, 0, 3, NULL), "map_", 0, GU_DEFAULT_BUFFER_SIZE) == 0)
		{				
			//determine type of map
			char* mapType = guScanLine(read, &substringBuffer, delimiterIndices, delimiterCount, 0);
			//add length of mapType to start position of substring
			int startPosition = guStringLength(mapType, GU_SINGLE_LINE, GU_DEFAULT_BUFFER_SIZE);
			//track length of map filename
			int mapNameSize = 0;

			//parse ambient texture map
			if(guCompare(mapType, "map_Ka", 0, GU_DEFAULT_BUFFER_SIZE) == 0)
			{
				if(!isCountOnly)
				{
					data[0] = guScanLineRaw(read, &substringBuffer, startPosition, GU_STRING_END, &mapNameSize);
					dataTypes[0] = GU_CHAR_ARRAY;
					dataSizes[0] = mapNameSize;
				}
				return freeReadMaterialReturn(delimiterIndices,  MAT_TEXTURE_AMBIENT);
			}
			//parse diffuse texture map
			if(guCompare(mapType, "map_Kd", 0, GU_DEFAULT_BUFFER_SIZE) == 0)
			{
				if(!isCountOnly)
				{
					data[0] = guScanLineRaw(read, &substringBuffer, startPosition, GU_STRING_END, &mapNameSize);
					dataTypes[0] = GU_CHAR_ARRAY;
					dataSizes[0] = mapNameSize;
				}
				return freeReadMaterialReturn(delimiterIndices,  MAT_TEXTURE_DIFFUSE);
			}
			//parse shininess map
			if(guCompare(mapType, "map_Ns", 0, GU_DEFAULT_BUFFER_SIZE) == 0)
			{
				if(!isCountOnly)
				{
					data[0] = guScanLineRaw(read, &substringBuffer, startPosition, GU_STRING_END, &mapNameSize);
					dataTypes[0] = GU_CHAR_ARRAY;
					dataSizes[0] = mapNameSize;
				}
				return freeReadMaterialReturn(delimiterIndices,  MAT_TEXTURE_SHININESS);
			}
			//parse optical density map
			if(guCompare(mapType, "map_Ni", 0, GU_DEFAULT_BUFFER_SIZE) == 0)
			{
				if(!isCountOnly)
				{
					data[0] = guScanLineRaw(read, &substringBuffer, startPosition, GU_STRING_END, &mapNameSize);
					dataTypes[0] = GU_CHAR_ARRAY;
					dataSizes[0] = mapNameSize;
				}
				return freeReadMaterialReturn(delimiterIndices,  MAT_TEXTURE_REFRACTION);
			}
			//parse alpha texture map
			if(guCompare(mapType, "map_d", 0, GU_DEFAULT_BUFFER_SIZE) == 0 || guCompare(mapType, "Tr", 0, GU_DEFAULT_BUFFER_SIZE) == 0)
			{
				if(!isCountOnly)
				{
					data[0] = guScanLineRaw(read, &substringBuffer, startPosition, GU_STRING_END, &mapNameSize);
					dataTypes[0] = GU_CHAR_ARRAY;
					dataSizes[0] = mapNameSize;
				}
				return freeReadMaterialReturn(delimiterIndices,  MAT_TEXTURE_ALPHA);
			}
			//parse bump map
			if(guCompare(mapType, "map_bump", 0, GU_DEFAULT_BUFFER_SIZE) == 0)
			{
				if(!isCountOnly)
				{
					data[0] = guScanLineRaw(read, &substringBuffer, startPosition, GU_STRING_END, &mapNameSize);
					dataTypes[0] = GU_CHAR_ARRAY;
					dataSizes[0] = mapNameSize;
				}
				return freeReadMaterialReturn(delimiterIndices,  MAT_BUMP);
			}
		}
	}

	//--- Unusable ---//
	return freeReadMaterialReturn(delimiterIndices,  MAT_UNUSABLE);
}

//deallocate buffers allocated during parsing
int freeReadMaterialReturn(int* delimiterIndices, int out)
{
	free(delimiterIndices);
	return out;
}