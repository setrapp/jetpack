#include "TLTextureLoader.h"

//construct a new texture using default data
TLTextureImage* TLTextureImageCreate()
{
	TLTextureImage* texture = (TLTextureImage*)malloc(sizeof(TLTextureImage));
	texture->filename = NULL;
	texture->imageData = NULL;
	texture->bpp = 0;
	texture->width = 0;
	texture->height = 0;
	texture->textureID = 0;
	return texture;
}

//dellocate texture from memory
void TLTextureImageDelete(TLTextureImage* deletee)
{
    if(!deletee)
        return;
    free(deletee->filename);
	free(deletee->imageData);
    if(glIsTexture(deletee->textureID))
       glDeleteTextures(1, &deletee->textureID);
    free(deletee);
}

//Load a TGA for use as a texture
int TLTextureImageLoadTGA(TLTextureImage* texture, char const* filename)
{
	GLubyte TGAheader[12] = {0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0};	//uncompressed TGA Header
	GLubyte checkFileHeader[12];									//used as a comparison to TGAheader to ensure the correct file type
	GLubyte headerInfo[6];											//first 6 useful bytes from the header
	GLuint bytesPerPixel;											//number of bytes per pixel in the TGA file
	GLuint imageSize;												//size needed on memeory
	GLuint colorTemp;												//a temporary varible used in rearrangement of color data 
	GLuint type = GL_RGBA;											//use RGBA(32bpp) as default GL Mode
	int i = 0;														//counter
	
	//open TGA file
	FILE* file;
	file = fopen(filename, "rb");

	//if the file cannot be correctly opened, return a failure
	//note that the pointers used in fread are given allocated memeory so they can be used later without a second call
	if(file == NULL ||																			//file does not exist
		fread(checkFileHeader, 1, sizeof(checkFileHeader), file) != sizeof(checkFileHeader) ||	//less than 12 bytes to read
		memcmp(TGAheader, checkFileHeader, sizeof(TGAheader)) != 0 ||							//the files header does not 
		fread(headerInfo, 1, sizeof(headerInfo), file) != sizeof(headerInfo))					//less than 6 useful bytes remain in header
	{
		//if the file could not be opened, return a failure without closing the file
		if(file == NULL)
		{
			return 0;
		}
		//close file and return a failure
		fclose(file);
		return 0;
	}

	/*------------TGA Header Bytes [12 - 17]-------------------------
	short xstart;             // image x origin
	short ystart;             // image y origin
	short width;              // image width in pixels
	short height;             // image height in pixels
	byte  bits;               // image bits per pixel
	byte  descriptor;         // image descriptor bits
	-----------------------------------------------------------------*/

	//determine dimensions of image using headerInfo (highbyte*256 + lowbyte)
	texture->width = headerInfo[1] * 256 + headerInfo[0];
	texture->height = headerInfo[3] * 256 + headerInfo[2];
	//if the either dimension is less than or equal to zero, or the TGA is not 24 or 32 bit, return a failure
	if(texture->width <= 0 || texture->height <= 0 || (headerInfo[4] != 24 && headerInfo[4] != 32))
	{
		fclose(file);
		return 0;
	}

	//set bpp texture of texture
	texture->bpp = headerInfo[4];
	//track byte per pixel
	bytesPerPixel = texture->bpp/8;
	//calculate memory needed to store texture
	imageSize = texture->width * texture->height * bytesPerPixel;

	//allocate memory for texture
	texture->imageData = (GLubyte*)malloc((int)imageSize);

	//if the memory is not available or the image size does not equal the amount of memory allocated, return a failure
	if(texture->imageData == NULL || fread(texture->imageData, 1, imageSize, file) != imageSize)
	{
		fclose(file);
		return 0;
	}

	//TGA files store color as BGR so run through each pixel and convet to RGB
	//note that i is incremented by pixels, while i and i+2 are bytes defining the pixel
	for(i = 0; i < (int)imageSize; i += bytesPerPixel)
	{
		colorTemp = texture->imageData[i];
		texture->imageData[i] = texture->imageData[i+2];
		texture->imageData[i+2] = colorTemp;
	}

	//close file
	fclose(file);

	//prepare to build texture
	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &texture->textureID);                                  //generate texture IDs
	glBindTexture(GL_TEXTURE_2D, texture->textureID);                       //bind texture
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);       //linear filtered
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);       //linear filtered
#ifdef GL_CLAMP
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);            //clamp texture to edges
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);            //clamp texture to edges
#elif GL_CLAMP_TO_EDGE
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	//clamp texture to edges
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);	//clamp texture to edges
#endif
    
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);		//allow texture to be affected be lighting and such
	
	//if TGA is 24 bit then remove alpha channel
	if(texture->bpp == 24)
		type = GL_RGB;
		
	//finally build texture
	glTexImage2D(GL_TEXTURE_2D, 0, type, texture->width, texture->height, 0, type, GL_UNSIGNED_BYTE, texture->imageData);

	//return a success 
	return 1;
}
